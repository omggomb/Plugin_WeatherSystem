#include "StdAfx.h"
#include "WeatherSystem.h"
#include "Game.h"
#include "WeatherSituation.h"
#include "IWeatherSituation.h"
#include "CPluginWeatherSystem.h"

using namespace WeatherSystemPlugin;
extern CGame *g_pGame;

void OnForceSituationCommand(IConsoleCmdArgs *pArgs)
{
	if (pArgs->GetArgCount() == 2)
	{
		gPlugin->GetWeatherSystem()->TryForceSituation(pArgs->GetArg(1));
	}
}

void OnReloadSituationsCommand(IConsoleCmdArgs *pArgs)
{
	if (pArgs->GetArgCount() == 1)
	{
		gPlugin->GetWeatherSystem()->ReloadSituationDefinitions();
	}
}

void OnListSituationsCommand(IConsoleCmdArgs *pArgs)
{
	if (pArgs->GetArgCount() == 1)
	{
		gPlugin->GetWeatherSystem()->DumpSituationsToLog();
	}
}

void OnForceSituationChangeCommand(IConsoleCmdArgs *pArgs)
{
	if (pArgs->GetArgCount() == 1)
	{
		gPlugin->GetWeatherSystem()->ForceSituationChange();
	}
}

void OnSystemActiveCVarChanged(ICVar *pCVar)
{
	if (pCVar)
	{
		gPlugin->GetWeatherSystem()->ToggleSystem(pCVar->GetIVal());
	}
}


void OnFadeOutAndStopCommand(IConsoleCmdArgs *pArgs)
{
	float fFadeOutTime = 1.f;
	if (pArgs->GetArgCount() != 2)
	{
		WEATHERSYSTEM_WARNING("No fadeout time specified, using 1 secod");
	}
	else
	{
		fFadeOutTime = atof(pArgs->GetArg(1));
	}

	gPlugin->GetWeatherSystem()->FadeOutCurrentSituation(fFadeOutTime, true, true);
}


CWeatherSystem::CWeatherSystem()
{
	// true by default, since diabling it is less common
	m_pCurrentlyActiveSituation = nullptr;
	m_pSystemActiveCVar = nullptr;
	m_fSituationTimer = 0.f;
	m_fCurrentSituationsTimeLimit = 60.f;
	m_bIsAboutToChangeFired = false;
	m_bIsSystemActive = false;
	m_pNextSituation = nullptr;

	m_fDummyCounter = 0.0f;

	m_bFadeOutCurrent = false;
	m_fFadeOutTime = 0.f;
	m_bRestoreLevelWeatherAfterFadeout = false;
	m_bStopSystemAfterFadeOut = false;
}

CWeatherSystem::~CWeatherSystem()
{
	gEnv->pGame->GetIGameFramework()->UnregisterListener(this);


	ClearDefinedSituations();
}

/**
* Initializes the weather system, but does not load any definitions
* @returns True on succes
*/
bool CWeatherSystem::Init()
{
	// Add to framework listeners so we receive the OnPostUpdate event
	WEATHERSYSTEM_LOG("Adding to framework listeners...");
	gEnv->pGame->GetIGameFramework()->RegisterListener(this, "WeatherSystem", EFrameworkListenerPriority::eFLPriority_HUD);

	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);

	// Register system cvars and commands ...
	m_pSystemActiveCVar = gEnv->pConsole->RegisterInt(WESY_SYSTEM_ACTIVE_CVAR, 0, VF_CHEAT, "Toggles weather system on or off", OnSystemActiveCVarChanged);
	if (!m_pSystemActiveCVar)
	{
		WEATHERSYSTEM_WARNING("Failed to register %s CVar!", WESY_SYSTEM_ACTIVE_CVAR);
	}


	gEnv->pConsole->AddCommand(WESY_LOAD_SITUATION_CCOMMAND, OnForceSituationCommand, VF_CHEAT, "Forces a situation to be applied, args: name of situation");
	gEnv->pConsole->AddCommand(WESY_RELOAD_SITUATIONS_CCOMMAND, OnReloadSituationsCommand, VF_CHEAT, "Reload all situation definitions in the current search folder");
	gEnv->pConsole->AddCommand(WESY_LIST_SITUATIONS_CCOMMAND, OnListSituationsCommand, VF_CHEAT, "Lists all loaded situation definitions");
	gEnv->pConsole->AddCommand(WESY_FORCE_SITUATION_CHANGE_CCOMMAND, OnForceSituationChangeCommand, VF_CHEAT, "Forces the system to elect a new situation and spawn it");
	gEnv->pConsole->AddCommand(WESY_FADE_OUT_AND_STOP_CCOMMAND, OnFadeOutAndStopCommand, VF_CHEAT, "Fades out current situation and stops system, args: fadeouttime");
	LoadSituationsFromFile("Libs/WeatherSystem/SituationDefinitions/");
	return true;
}

/**
* Loads all defined weather situations inside the given folder
* @param sFolderPath Path to the containing folder
* @returns True on succes
*/
bool CWeatherSystem::LoadSituationsFromFile(string sFolderPath)
{
	char correctedPath[ICryPak::g_nMaxPath];
	string sCorrectedPath = gEnv->pCryPak->AdjustFileName(sFolderPath, correctedPath, ICryPak::EPathResolutionRules::FLAGS_ADD_TRAILING_SLASH);
	string sSearchPath = sCorrectedPath + "*.xml";

	//=============================================================================
	// Workaround since I can't get CryPak to work properly
	//=============================================================================
	string sListFilePath(sCorrectedPath + "files.txt");

	FILE *pFileList = gEnv->pCryPak->FOpen(sListFilePath.c_str(), "r");

	if (pFileList)
	{

		char buffer[2048];

		size_t read = gEnv->pCryPak->FRead(buffer, 2048, pFileList);

		string fileContent(buffer, read);

		size_t linePos = fileContent.find_first_of('\n');
		size_t lastPos = 0;
		if (linePos != string::npos)
		{
			while (linePos != string::npos)
			{
				string fileName = fileContent.substr(lastPos, linePos - lastPos);

				XmlNodeRef xmlNode = gEnv->pSystem->LoadXmlFromFile(sCorrectedPath + fileName);

				if (xmlNode)
				{
					IWeatherSituation *pSituation = new CWeatherSituation();
					if (pSituation->LoadFromXml(xmlNode))
					{
						m_definedSituations[pSituation->GetName()] = pSituation;
					}
					else
					{
						SAFE_DELETE(pSituation);
						WEATHERSYSTEM_WARNING("Failed to load definition %s", fileName);
					}
				}
				else
				{
					WEATHERSSYSTEM_ERROR("files.txt contains non existant entry: %s", fileName);
				}

				lastPos = ++linePos;

				linePos = fileContent.find_first_of('\n', linePos);
			}

			// last entry
			if (lastPos < fileContent.size())
			{
				string lastEntry = fileContent.substr(lastPos, fileContent.size() - lastPos);
				XmlNodeRef xmlNode = gEnv->pSystem->LoadXmlFromFile(sCorrectedPath + lastEntry);

				if (xmlNode)
				{
					IWeatherSituation *pSituation = new CWeatherSituation();
					if (pSituation->LoadFromXml(xmlNode))
					{
						m_definedSituations[pSituation->GetName()] = pSituation;
					}
					else
					{
						SAFE_DELETE(pSituation);
						WEATHERSYSTEM_WARNING("Failed to load definition %s", lastEntry);
					}
				}
				else
				{
					WEATHERSSYSTEM_ERROR("files.txt contains non existant entry: %s", lastEntry);
				}

			}
		}
		else
		{
			// Only one definition
			XmlNodeRef node = gEnv->pSystem->LoadXmlFromFile(sCorrectedPath + fileContent);
			if (node)
			{
				IWeatherSituation *pSituation = new CWeatherSituation();
				if (pSituation->LoadFromXml(node))
				{
					m_definedSituations[pSituation->GetName()] = pSituation;
				}
				else
				{
					SAFE_DELETE(pSituation);
					WEATHERSYSTEM_WARNING("Failed to load definition %s", fileContent);
				}
			}
		}

		gEnv->pCryPak->FClose(pFileList);
	}
	else
	{
		WEATHERSYSTEM_WARNING("Couldn't find %s. Please create it and list you definition files in there.", sCorrectedPath + "files.txt");
	}

	/*
	_finddata64i32_t findData;
	auto file = gEnv->pCryPak->FindFirst(sSearchPath.c_str(), &findData, 0, true);

	while (file == 0)
	{
	    string sNewPath = sCorrectedPath;
	    XmlNodeRef currentFile = gEnv->pSystem->LoadXmlFromFile(sNewPath.append(findData.name));

	    if (currentFile)
	    {

	        IWeatherSituation *pCurrentSituation = new CWeatherSituation();
	        if (pCurrentSituation->LoadFromXml(currentFile))
	        {
	            m_definedSituations[pCurrentSituation->GetName()] = pCurrentSituation;
	        }
	        else
	        {
	            SAFE_DELETE(pCurrentSituation);
	            WEATHERSYSTEM_WARNING("Failed to load situation definition :%s", findData.name);
	        }

	    }

	    file = gEnv->pCryPak->FindNext(file, &findData);
	}*/
	return true;
}

/**
* Switches the weather system on or off.
*/
void CWeatherSystem::ToggleSystem(bool bEnableWeatherSystem)
{
	m_bIsSystemActive = bEnableWeatherSystem;
	bool bCVar = m_pSystemActiveCVar->GetIVal();

	if (bCVar != bEnableWeatherSystem)
	{
		m_pSystemActiveCVar->Set(bEnableWeatherSystem ? 1 : 0);
	}

	if (!bEnableWeatherSystem)
	{
		Reset();
		RestoreWeather(true);
	}
}

void CWeatherSystem::OnPostUpdate(float fFrameTime)
{
	if (gEnv->IsEditing())
	{
		return;
	}

	if (m_bFadeOutCurrent)
	{
		FadeOutUpdate(fFrameTime);
	}
	else
	{
		Update(fFrameTime);
	}
}

void CWeatherSystem::OnActionEvent(const SActionEvent &event)
{
	if (event.m_event == EActionEvent::eAE_inGame)
	{
		m_stockRainParams.ReadCurrentRainParams();
		m_stockSnowParams.ReadCurrentSnowParams();
	}
}

void CWeatherSystem::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	if (event == ESystemEvent::ESYSTEM_EVENT_EDITOR_GAME_MODE_CHANGED)
	{
		if (gEnv->IsEditing())
		{
			Reset();
			RestoreWeather(true);
		}
	}
}

bool CWeatherSystem::TryForceSituation(string sName)
{
	auto iter = m_definedSituations.find(sName);

	if (iter == m_definedSituations.end())
	{
		WEATHERSYSTEM_WARNING("Couldn't find situation named: %s. Not loading", sName);
		return false;
	}

	//FireOnSituationChanging(sName);

	SpawnSituation(iter->second);
	return true;
}

void CWeatherSystem::ReloadSituationDefinitions()
{

	ClearDefinedSituations();

	m_pCurrentlyActiveSituation = nullptr;

	LoadSituationsFromFile("Libs/WeatherSystem/SituationDefinitions/");
}

void CWeatherSystem::AddWeatherSystemListener(IWeatherSystemListener *pListener)
{
	std::vector<IWeatherSystemListener *>::iterator it;

	for (it = m_systemListeners.begin(); it != m_systemListeners.end(); ++it)
	{
		IWeatherSystemListener *pCur = (*it);

		if (pCur == pListener)
		{
			return;
		}
	}

	m_systemListeners.push_back(pListener);
}

void CWeatherSystem::RemoveWeatherSystemListener(IWeatherSystemListener *pListener)
{
	std::vector<IWeatherSystemListener *> copy;

	for (auto listener : m_systemListeners)
	{
		if (listener != pListener)
		{
			copy.push_back(listener);
		}
	}

	m_systemListeners = copy;
}

void CWeatherSystem::DumpSituationsToLog()
{
	string sConcat;
	// Fancy c11 here hohohooooo
	for (auto pair : m_definedSituations)
	{
		sConcat.append(pair.first.c_str());
		sConcat.append("\n");
	}

	WEATHERSYSTEM_LOG("Currently loaded %d situation(s): \n %s", m_definedSituations.size(), sConcat);
}

void CWeatherSystem::ForceSituationChange()
{
	if (!m_pCurrentlyActiveSituation)
	{
		return;
	}
	SpawnSituation(m_pNextSituation);
}

void CWeatherSystem::ForceSituationChange(float fTimeToFade)
{
	m_fCurrentSituationsTimeLimit = fTimeToFade * 10 + 0.01f;
	m_fSituationTimer = m_fCurrentSituationsTimeLimit * 0.9f;
}

void CWeatherSystem::FadeOutCurrentSituation(float fTimeToFadeOut, bool bRestoreLevelWeather, bool bToggleWeatherSystem)
{
	m_bFadeOutCurrent = true;

	m_fFadeOutTime  = fTimeToFadeOut;
	m_bRestoreLevelWeatherAfterFadeout = bRestoreLevelWeather;
	m_bStopSystemAfterFadeOut = bToggleWeatherSystem;

	m_fSituationTimer = 0.f;

	m_fadeFromRainParams.ReadCurrentRainParams();
	m_fadeFromSnowParams.ReadCurrentSnowParams();

	if (bRestoreLevelWeather)
	{
		FireOnSituationAboutToChange(fTimeToFadeOut / 2);
	}
	else
	{
		FireOnSituationAboutToChange(fTimeToFadeOut);
	}
}

void CWeatherSystem::IgnoreSituation(string sName, bool bIgnore)
{
	auto it = m_ignoredSituations.find(sName);

	if (it == m_ignoredSituations.end() && bIgnore)
	{
		m_ignoredSituations.insert(sName);
	}

	if (!bIgnore && it != m_ignoredSituations.end())
	{
		m_ignoredSituations.erase(it);
	}
}

bool CWeatherSystem::TrySetNextSituation(string sName)
{
	auto it = m_definedSituations.find(sName);

	if (it == m_definedSituations.end())
	{
		WEATHERSYSTEM_WARNING("Trying to set non existent situation (%s) as next situation! Not setting....", sName);
		return false;
	}

	m_pNextSituation = it->second;
	return true;
}
//=====================================================
// PRIVATE
//=====================================================

void CWeatherSystem::Update(float fDeltaTime)
{
	if (m_bIsSystemActive)
	{
		if (m_pCurrentlyActiveSituation)
		{
			m_pCurrentlyActiveSituation->Update(fDeltaTime);

			if (!m_bIsAboutToChangeFired && m_fSituationTimer >= 0.9f * m_fCurrentSituationsTimeLimit)
			{
				FireOnSituationAboutToChange(m_fCurrentSituationsTimeLimit / 10);
				m_bIsAboutToChangeFired = true;
			}

			if (m_fSituationTimer > m_fCurrentSituationsTimeLimit)
			{
				SpawnSituation(m_pNextSituation ? m_pNextSituation : GetNextSituation(m_pCurrentlyActiveSituation->GetPossibleSuccessors()));
			}
			else
			{
				m_fSituationTimer += fDeltaTime;
			}

		}

	}
}

void CWeatherSystem::FadeOutUpdate(float fFrameTime)
{
	if (m_bRestoreLevelWeatherAfterFadeout)
	{
		if (m_fSituationTimer < m_fFadeOutTime / 2)
		{
			m_pCurrentlyActiveSituation->Update(fFrameTime);
			m_fSituationTimer += fFrameTime;
		}
		else if (m_fSituationTimer <= m_fFadeOutTime)
		{
			// Now start fading out rain and snow
			// Wind can be set at once since it doen't update on screen anyway

			// Rain
			float fFrom = m_fadeFromRainParams.x_pOldRain.fAmount;
			float fTo = m_stockRainParams.x_pOldRain.fAmount;

			// We need to lerp from half the time to zero
			float fHalfTime = m_fFadeOutTime / 2;
			float fTimeGone = m_fSituationTimer - fHalfTime;

			float fPercentage = fTimeGone / fHalfTime;

			float fRainAmount = LERP(fFrom, fTo, fPercentage);

			gEnv->p3DEngine->SetRainParams(m_fadeFromRainParams.x_pOldRain.fReflectionAmount, m_fadeFromRainParams.x_pOldRain.fFakeGlossiness, m_fadeFromRainParams.x_pOldRain.fPuddlesAmount,
										   true, m_fadeFromRainParams.x_pOldRain.fDropSpeed, m_fadeFromRainParams.x_pOldRain.fUmbrellaRadius);

			gEnv->p3DEngine->SetRainParams(m_fadeFromRainParams.x_pOldRain.center, m_fadeFromRainParams.x_pOldRain.fRadius, fRainAmount, m_fadeFromRainParams.x_pOldRain.color);


			// Snow
			// Flake size
			fFrom = m_fadeFromSnowParams.oldSnowParams.snowFallParams.fSnowFlakeSize;
			fTo = m_stockSnowParams.oldSnowParams.snowFallParams.fSnowFlakeSize;

			float fFlakeSize = LERP(fFrom, fTo, fPercentage);

			// Snow amount
			fFrom = m_fadeFromSnowParams.oldSnowParams.surfaceParams.fSnowAmount;
			fTo = m_stockSnowParams.oldSnowParams.surfaceParams.fSnowAmount;

			float fSnowAmount = LERP(fFrom, fTo, fPercentage);

			gEnv->p3DEngine->SetSnowFallParams(m_fadeFromSnowParams.oldSnowParams.snowFallParams.snowFlakeCount, fFlakeSize, m_fadeFromSnowParams.oldSnowParams.snowFallParams.fSnowFallBrightness,
											   m_fadeFromSnowParams.oldSnowParams.snowFallParams.fSnowFallGravityScale, m_fadeFromSnowParams.oldSnowParams.snowFallParams.fSnowFallWindScale, m_fadeFromSnowParams.oldSnowParams.snowFallParams.fSnowFallTurbulence,
											   m_fadeFromSnowParams.oldSnowParams.snowFallParams.fSnowFallTurbulenceFreq);

			gEnv->p3DEngine->SetSnowSurfaceParams(m_fadeFromSnowParams.oldSnowParams.surfaceParams.center, m_fadeFromSnowParams.oldSnowParams.surfaceParams.fRadius, fSnowAmount,
												  m_fadeFromSnowParams.oldSnowParams.surfaceParams.fFrostAmount, m_fadeFromSnowParams.oldSnowParams.surfaceParams.fSurfaceFreezing);

			m_fSituationTimer += fFrameTime;
		}
		else
		{
			RestoreWeather(true);
			if (m_bStopSystemAfterFadeOut)
			{
				ToggleSystem(false);
			}
		}
	}
	else
	{
		if (m_fSituationTimer < m_fFadeOutTime)
		{
			m_pCurrentlyActiveSituation->Update(fFrameTime);
		}
		else
		{
			if (m_bStopSystemAfterFadeOut)
			{
				ToggleSystem(false);
			}
		}
		m_fSituationTimer += fFrameTime;
	}


}


void CWeatherSystem::FireOnSituationAboutToChange(float fTimeLeft)
{
	if (m_pCurrentlyActiveSituation)
	{
		m_pCurrentlyActiveSituation->OnSituationAboutToChange(fTimeLeft);
	}

	for (auto li : m_systemListeners)
	{
		li->OnSituationAboutToChange(fTimeLeft);
	}
}

void CWeatherSystem::FireOnSituationChanging(string sNextSituation)
{
	if (m_pCurrentlyActiveSituation)
	{
		m_pCurrentlyActiveSituation->OnSituationChanging(sNextSituation);
	}

	for (auto li : m_systemListeners)
	{
		li->OnSituationChanging(sNextSituation);
	}
}

void CWeatherSystem::ClearDefinedSituations()
{
	for (auto it = m_definedSituations.begin();
			it != m_definedSituations.end();
			++it)
	{
		delete(it->second);
	}

	m_definedSituations.clear();
}

IWeatherSituation *CWeatherSystem::GetNextSituation(const std::vector<string> &currentFollowedBy)
{
	float fCurrentTime = gEnv->p3DEngine->GetTimeOfDay()->GetTime();

	std::vector<string> appropriateSiuations;

	// Did I mention c11 is quite nice.....
	for (string entry : currentFollowedBy)
	{
		if (m_definedSituations.find(entry) == m_definedSituations.end())
		{
			continue;
		}

		IWeatherSituation *pCurrentSit = m_definedSituations[entry];

		float fFrom, fTo;

		pCurrentSit->GetTimeSpan(fFrom, fTo);

		if (fCurrentTime >= fFrom && fCurrentTime < fTo)
		{
			// if it is nor on the ignored set of situations
			if (auto it = m_ignoredSituations.find(entry) == m_ignoredSituations.end())
			{
				appropriateSiuations.push_back(entry);
			}
		}
	}
	int size = appropriateSiuations.size();

	if (size <= 0)
	{
		return nullptr;
	}


	int index = cry_frand() * (size - 1);

	return m_definedSituations[appropriateSiuations[index]];
}

float CWeatherSystem::GetNewSituationDuration()
{
	float fMin, fMax;

	m_pCurrentlyActiveSituation->GetMinMaxDuration(fMin, fMax);

	return (fMin + (cry_frand() * (fMax - fMin)));
}

void CWeatherSystem::SpawnSituation(IWeatherSituation *pSitToSpawn)
{

	m_fSituationTimer = 0.f;
	if (!pSitToSpawn)
	{
		WEATHERSYSTEM_WARNING("Trying to spawn nullptr situation! Not changing!");
		return;
	}

	m_pNextSituation = GetNextSituation(pSitToSpawn->GetPossibleSuccessors());

	FireOnSituationChanging(pSitToSpawn->GetName());
	m_pCurrentlyActiveSituation = pSitToSpawn;

	m_fCurrentSituationsTimeLimit = GetNewSituationDuration();

	WEATHERSYSTEM_LOG("Spawning: %s, time limit %f", pSitToSpawn->GetName(), m_fCurrentSituationsTimeLimit);

	m_bIsAboutToChangeFired = false;

	m_pCurrentlyActiveSituation->Apply();
}

void CWeatherSystem::RestoreWeather(bool bResetSituation)
{
	if (m_pCurrentlyActiveSituation && bResetSituation)
	{
		m_pCurrentlyActiveSituation->Reset();
	}

	m_pCurrentlyActiveSituation = nullptr;

	gEnv->p3DEngine->SetRainParams(m_stockRainParams.x_pOldRain.fReflectionAmount, m_stockRainParams.x_pOldRain.fFakeGlossiness, m_stockRainParams.x_pOldRain.fPuddlesAmount,
								   true, m_stockRainParams.x_pOldRain.fDropSpeed, m_stockRainParams.x_pOldRain.fUmbrellaRadius);

	gEnv->p3DEngine->SetRainParams(m_stockRainParams.x_pOldRain.center, m_stockRainParams.x_pOldRain.fRadius, m_stockRainParams.x_pOldRain.fAmount, m_stockRainParams.x_pOldRain.color);

#ifdef SDK_HAS_SNOW
	gEnv->p3DEngine->SetSnowFallParams(m_stockSnowParams.oldSnowParams.snowFallParams.snowFlakeCount, m_stockSnowParams.oldSnowParams.snowFallParams.fSnowFlakeSize, m_stockSnowParams.oldSnowParams.snowFallParams.fSnowFallBrightness,
									   m_stockSnowParams.oldSnowParams.snowFallParams.fSnowFallGravityScale, m_stockSnowParams.oldSnowParams.snowFallParams.fSnowFallWindScale, m_stockSnowParams.oldSnowParams.snowFallParams.fSnowFallTurbulence,
									   m_stockSnowParams.oldSnowParams.snowFallParams.fSnowFallTurbulenceFreq);

	gEnv->p3DEngine->SetSnowSurfaceParams(m_stockSnowParams.oldSnowParams.surfaceParams.center, m_stockSnowParams.oldSnowParams.surfaceParams.fRadius, m_stockSnowParams.oldSnowParams.surfaceParams.fSnowAmount,
										  m_stockSnowParams.oldSnowParams.surfaceParams.fFrostAmount, m_stockSnowParams.oldSnowParams.surfaceParams.fSurfaceFreezing);
#endif

}

void CWeatherSystem::Reset()
{
	m_bFadeOutCurrent = false;
	m_fFadeOutTime = 0.f;
}

