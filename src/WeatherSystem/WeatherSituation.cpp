#include "StdAfx.h"
#include "WeatherSituation.h"
#include "WeatherSystem.h"
#include "InterpolationHelpers.h"
#include "Actor.h"


CWeatherSituation::CWeatherSituation()
{
	m_sName = "UnnamedSituation";
	m_bIsUsedInCycle = true;
	m_fTimeFrom = 0.f;
	m_fTimeTo = 0.f;
	m_bAffectsGlobalWind = true;
	m_bHasRain = false;
	m_bRaining = false;
	m_bHasSnow = false;
	m_bSnowFadeFinished = false;
	m_bIsActive = false;
	m_fTimeRemaining = 0.f;
	m_fMinDuration = 20.f;
	m_fMaxDuration = 60.f;
}

CWeatherSituation::~CWeatherSituation()
{
	std::vector<SSituationParticle *>::iterator it;
	for (it = m_particleCollection.begin(); it != m_particleCollection.end(); ++it)
	{
		delete(*it);
	}
	m_particleCollection.clear();

	std::vector<SSituationSound *>::iterator itSound;
	for (itSound = m_soundCollection.begin(); itSound != m_soundCollection.end(); ++itSound)
	{
		delete(*itSound);
	}
	m_soundCollection.clear();

	std::vector<SSituationSimpleCloud *>::iterator itCl;

	for (itCl = m_simpleClouds.begin(); itCl != m_simpleClouds.end(); ++itCl)
	{
		delete(*itCl);
	}
	m_simpleClouds.clear();

	for (auto ent : m_entityCollection)
	{
		delete(ent);
	}
	m_entityCollection.clear();
}

bool CWeatherSituation::LoadFromXml(XmlNodeRef root)
{
	const char *tag = root->getTag();

	// First check if file is correctly structured (supposedly)
	if (strcmp("WeatherSituation", tag) != 0)
	{
		WEATHERSYSTEM_WARNING("Could't find root node when trying to load weather definition!");
		return false;
	}

	// now get general attributes
	string modName = "Weather Situation";
	XmlString name, timeFrom, timeTo, followedBy;
	XMLGET(root, "name", name, modName);
	m_sName = name.empty() ? "Invalid" : name;

	XMLGET(root, "isUsedInCycle", m_bIsUsedInCycle, modName);

	XMLGET(root, "timeFrom", timeFrom, modName);
	m_fTimeFrom = ConvertStringToFloatTime(timeFrom.c_str());

	XMLGET(root, "timeTo", timeTo, modName);
	m_fTimeTo = ConvertStringToFloatTime(timeTo.c_str());

	XMLGET(root, "followedBy", followedBy, modName);
	ParseFollowedByAttribute(followedBy);

	XMLGET(root, "minDuration", m_fMinDuration, modName);
	XMLGET(root, "maxDuration", m_fMaxDuration, modName);

	XMLGET(root, "fadeOutTimeFactor", m_fFadeOutTimeFactor, modName);
	CLAMP(m_fFadeOutTimeFactor, 0.f, 0.99f);

	// wind attributes;
	m_windStats.LoadFromXml(root->findChild("Wind"));


	// particles
	XmlNodeRef particles = root->findChild("Particles");
	ParseParticles(particles);

	XmlNodeRef sounds = root->findChild("Sounds");
	ParseSounds(sounds);

	XmlNodeRef sky = root->findChild("Sky");
	ParseSky(sky);

	XmlNodeRef tod = root->findChild("TimeOfDay");
	ParseTimeOfDay(tod);

	ParseEntities(root->findChild("Entities"));



	return true;
}

void CWeatherSituation::Apply()
{
	m_windStats.x_vOldWindVector = gEnv->p3DEngine->GetGlobalWind(false);
	m_rainParams.ReadCurrentRainParams();
	m_snowParams.ReadCurrentSnowParams();

	m_todFader.Init();
	m_bIsActive = true;

	for (auto cloud : m_simpleClouds)
	{
		cloud->SpawnCloud();
	}

	for (auto sound : m_soundCollection)
	{
		sound->SpawnSound();
	}

	for (auto part : m_particleCollection)
	{
		part->SpawnParticle();
	}

	for (auto ent : m_entityCollection)
	{
		ent->SpawnEntity();
	}
}

void CWeatherSituation::Update(float fFrameTime)
{
	UpdateWind(fFrameTime);
	UpdateRain(fFrameTime);
	UpdateSnow(fFrameTime);
	m_todFader.Update(fFrameTime);
	UpdateClouds(fFrameTime);
	UpdateSounds(fFrameTime);
	UpdateParticle(fFrameTime);
	UpdateEntity(fFrameTime);
}

void CWeatherSituation::Reset()
{
	m_fTimeRemaining = 0.f;
	//m_snowParams.fFadeProgression = 0.f;
	m_snowParams.Reset();
	m_rainParams.x_fFadeProgression = 0.f;
	m_todFader.Reset();

	for (auto cloud : m_simpleClouds)
	{
		cloud->Reset();
	}

	for (auto sound : m_soundCollection)
	{
		sound->Reset();
	}

	for (auto part : m_particleCollection)
	{
		part->Reset();
	}

	for (auto ent : m_entityCollection)
	{
		ent->Reset();
	}
}

//====================================================
// PRIVATE
//====================================================

#pragma region Utils

float CWeatherSituation::ConvertStringToFloatTime(string sString)
{
	size_t colonPos = sString.find(':');

	if (colonPos != -1)
	{
		string hours = sString.substr(0, colonPos);
		int intHours = atoi(hours.c_str());

		int minutes = intHours * 60;

		++colonPos;

		if (colonPos < sString.length())
		{
			string sminutes = sString.substr(colonPos, sString.length() - colonPos);
			int intMinutes = atoi(sminutes.c_str());

			minutes += intMinutes;
		}

		return minutes / 60.f;
	}

	return atoi(sString.c_str());
}

EPositioningMode CWeatherSituation::GetPositioningMode(const char *sString)
{
	if (strcmp(sString, "World") == 0)
	{
		return EPositioningMode::ePM_World;
	}
	else if (strcmp(sString, "Local") == 0)
	{
		return EPositioningMode::ePM_Local;
	}
	else if (strcmp(sString, "AroundPlayer") == 0)
	{
		return EPositioningMode::ePM_AroundPlayer;
	}
	else
	{
		return EPositioningMode::ePM_AroundWorld;
	}

	// We do not care if there are wrong string fed into the function
}

#pragma endregion

#pragma region Parsing

void CWeatherSituation::ParseFollowedByAttribute(string sFollowedBy)
{
	if (sFollowedBy.length() == 0)
	{
		return;
	}

	size_t commaPos = sFollowedBy.find_first_of(',');

	if (commaPos == -1)
	{
		m_followedBy.push_back(sFollowedBy);
		return;
	}

	size_t lastPos = 0;

	while (commaPos != -1)
	{
		string situation = sFollowedBy.substr(lastPos, commaPos - lastPos);
		situation = situation.Trim();

		if (!situation.empty())
		{
			m_followedBy.push_back(situation);
		}

		++commaPos;
		if (commaPos < sFollowedBy.length())
		{
			lastPos = commaPos;
			commaPos = sFollowedBy.find_first_of(',', lastPos);
		}
		else
		{
			// comma is at the end of the string so there's nothing more to parse
			commaPos = -1;
			return;
		}
	}

	if (lastPos < sFollowedBy.length() - 1)
	{
		string lastString = sFollowedBy.substr(lastPos, sFollowedBy.length() - lastPos);
		lastString = lastString.Trim();
		if (!lastString.empty())
		{
			m_followedBy.push_back(lastString);
		}
	}
}

void CWeatherSituation::ParseParticles(XmlNodeRef particleRoot)
{
	if (particleRoot && particleRoot->getChildCount() > 0)
	{
		int childCount = particleRoot->getChildCount();
		bool sux;
		for (int i = 0; i < childCount; ++i)
		{
			XmlNodeRef child = particleRoot->getChild(i);

			SSituationParticle *pParticle = new SSituationParticle();

			pParticle->LoadFromXml(child);

			m_particleCollection.push_back(pParticle);
		}
	}
}

void CWeatherSituation::ParseSounds(XmlNodeRef sounds)
{
	if (sounds && sounds->getChildCount() > 0)
	{
		int childCount = sounds->getChildCount();
		bool sux;
		for (int i = 0; i < childCount; ++i)
		{
			XmlNodeRef child = sounds->getChild(i);

			SSituationSound *pSound = new SSituationSound();

			pSound->LoadFromXml(child);
			m_soundCollection.push_back(pSound);
		}
	}
}

void CWeatherSituation::ParseSky(XmlNodeRef skyRoot)
{
	if (!skyRoot)
	{
		return;
	}

	auto rainNode = skyRoot->findChild("Rain");

	if (rainNode)
	{
		m_rainParams.LoadFromXml(rainNode);
		m_bHasRain = true;
	}

	auto snowNode = skyRoot->findChild("Snow");

	if (snowNode)
	{
		SSituationSnow::ELoadingResult  res = m_snowParams.LoadFromXml(snowNode);

		switch (res)
		{
		case SSituationSnow::eLFR_NoSurface:
		{
			WEATHERSYSTEM_WARNING("Snow loading failed! No <Surface> tag found");
		}
		break;
		case SSituationSnow::eLFR_NoFall:
		{
			WEATHERSYSTEM_WARNING("Snow loading failed! No <SnowFall> tag found");
		}
		break;
		case SSituationSnow::eLFR_NoSnow:
			break;
		case SSituationSnow::eLFR_Nullptr:
		{
			WEATHERSYSTEM_WARNING("Snow loading failed! xml pointer was null");
		}
		break;
		case SSituationSnow::eLFR_Succes:
			m_bHasSnow = true;
			break;
		default:
			break;
		}
	}

	if (m_bHasRain && m_bHasSnow)
	{
		WEATHERSYSTEM_LOG("Found rain and snow definitions in \"%s\"! Make sure you don't use both with excessive values for their amount",
							  m_sName);
	}

	auto clouds = skyRoot->findChild("Clouds");
	ParseClouds(clouds);
}

void CWeatherSituation::ParseTimeOfDay(XmlNodeRef root)
{
	m_todFader.LoadFromXml(root);
}

void CWeatherSituation::ParseClouds(XmlNodeRef root)
{
	for (int i = 0; i < root->getChildCount(); ++i)
	{
		auto child = root->getChild(i);

		if (strcmp(child->getTag(), "SimpleCloud") == 0)
		{
			auto simpleCloud = new SSituationSimpleCloud();
			simpleCloud->LoadFromXml(child);

			m_simpleClouds.push_back(simpleCloud);
		}
	}
}

void CWeatherSituation::ParseEntities(XmlNodeRef entRoot)
{
	if (!entRoot)
	{
		return;
	}

	for (int i = 0; i < entRoot->getChildCount(); ++i)
	{
		SSituationEntity *pEnt = new SSituationEntity();

		pEnt->LoadFromXml(entRoot->getChild(i));

		m_entityCollection.push_back(pEnt);
	}
}

#pragma endregion

void CWeatherSituation::UpdateWind(float fFrameTime)
{
	if (m_bAffectsGlobalWind)
	{
		// Fade in wind
		if (m_windStats.x_fFadeProgression < m_windStats.x_fFadeInTime)
		{
			Vec3 x  = Vec3::CreateLerp(m_windStats.x_vOldWindVector, m_windStats.x_vDirectionAndSpeed, m_windStats.x_fFadeProgression / m_windStats.x_fFadeInTime);
			m_windStats.x_fFadeProgression += fFrameTime;
			gEnv->p3DEngine->SetWind(x);
			// WEATHERSYSTEM_LOG("Wind fading in!");
		}
		else
		{
			// Wind fully active, now add random turbulences
			if (m_windStats.x_fMaxLowerFluctuation != 0 || m_windStats.x_fMaxHigherFluctuation != 0)
			{
				// If the turbulence limit is reached and the same period has passed, choose a new turbulence
				if (m_windStats.x_fTurbulenceDuration > m_windStats.x_fCurrentTurbulenceTime * 2)
				{
					// for maximum effect we choose the highest speed direction
					float highest = MAX(m_windStats.x_vDirectionAndSpeed.x, MAX(m_windStats.x_vDirectionAndSpeed.y, m_windStats.x_vDirectionAndSpeed.z));

					float min = highest - m_windStats.x_fMaxLowerFluctuation;
					float max = highest + m_windStats.x_fMaxHigherFluctuation;

					float newRandom = min + cry_frand() * (max - min);

					// difference...
					float diff = newRandom - highest;
					Vec3 deltaVector = Vec3(diff, diff, diff);

					m_windStats.x_vTurbulenceSpeed = m_windStats.x_vDirectionAndSpeed + deltaVector;

					// remember that there is little chance wind changes direction (clamp at zero)
					m_windStats.x_vTurbulenceSpeed = Vec3(MAX(m_windStats.x_vTurbulenceSpeed.x, 0),
														  MAX(m_windStats.x_vTurbulenceSpeed.y, 0),
														  MAX(m_windStats.x_vTurbulenceSpeed.z, 0));

					// reset duration and startover
					m_windStats.x_bTurbulenceGoing = false;
					m_windStats.x_fTurbulenceDuration  = 0;
					m_windStats.x_fCurrentTurbulenceTime = Random(m_windStats.x_fMinTurbulenceDuration, m_windStats.x_fMaxTurbulenceDuration);
					WEATHERSYSTEM_LOG("Turbulence duration: %f", m_windStats.x_fCurrentTurbulenceTime);
				}
				else if (m_windStats.x_fTurbulenceDuration < m_windStats.x_fCurrentTurbulenceTime)
				{
					// if the turbulence is ongoing just set it
					if (!m_windStats.x_bTurbulenceGoing)
					{
						gEnv->p3DEngine->SetWind(m_windStats.x_vTurbulenceSpeed);
						m_windStats.x_bTurbulenceGoing = true;
						WEATHERSYSTEM_LOG("Turbulence, Speed : %f %f %f", m_windStats.x_vTurbulenceSpeed.x,
										  m_windStats.x_vTurbulenceSpeed.y,
										  m_windStats.x_vTurbulenceSpeed.z);
					}
				}
				else
				{
					// if we're inbetween set wind back to normal speed
					if (m_windStats.x_bTurbulenceGoing)
					{
						gEnv->p3DEngine->SetWind(m_windStats.x_vDirectionAndSpeed);
						m_windStats.x_bTurbulenceGoing = false;
						WEATHERSYSTEM_LOG("No turbulence going atm");
					}

				}



				m_windStats.x_fTurbulenceDuration += fFrameTime;
			}
		}


	}
}



void CWeatherSituation::UpdateRain(float fFrameTime)
{
	if (m_bHasRain)
	{
		Vec3 pos = m_rainParams.x_vCenter;

		// Calc position
		switch (m_rainParams.x_positioningMode)
		{
		case (EPositioningMode::ePM_Local):
		{
			pos = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos() + pos;
		}
		break;
		case (EPositioningMode::ePM_AroundPlayer):
		{
			pos = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos();
		}
		break;
		default:
			break;
		}

		if (m_rainParams.x_fFadeProgression < m_rainParams.x_fFadeInTime)
		{

			gEnv->p3DEngine->SetRainParams(pos, m_rainParams.x_fRadius,
										   LERP(m_rainParams.x_pOldRain.fAmount, m_rainParams.x_fAmount,
												m_rainParams.x_fFadeProgression / m_rainParams.x_fFadeInTime),
										   m_rainParams.x_vColor);

			if (!m_bRaining)
			{
				gEnv->p3DEngine->SetRainParams(m_rainParams.x_fReflectionAmount, m_rainParams.x_fFakeGlossiness, m_rainParams.x_fPuddlesAmount,
											   true, m_rainParams.x_fDropSpeed, m_rainParams.x_fUmbrellaRadius);
				m_bRaining = true;
			}

			//WEATHERSYSTEM_LOG("Rain fader: %f", fam);
			m_rainParams.x_fFadeProgression += fFrameTime;
		}
		else
		{
			// fAmount != fFadingAmount !
			gEnv->p3DEngine->SetRainParams(pos, m_rainParams.x_fRadius, m_rainParams.x_fAmount, m_rainParams.x_vColor);
		}

	}
}

void CWeatherSituation::UpdateSnow(float fFrameTime)
{
	if (m_bHasSnow)
	{
		m_snowParams.Update(fFrameTime);
	}

	//=============================================================================
	// RETURN HERE
	//=============================================================================
	return;
}

void CWeatherSituation::UpdateClouds(float fFrameTime)
{
	for (auto cloud : m_simpleClouds)
	{
		cloud->Update(fFrameTime);
	}

	return;
}

void CWeatherSituation::UpdateSounds(float fFrameTime)
{
	for (auto sound : m_soundCollection)
	{
		sound->Update(fFrameTime);
	}
}

void CWeatherSituation::UpdateParticle(float fFrameTime)
{
	for (auto part : m_particleCollection)
	{
		part->Update(fFrameTime);
	}
}

void CWeatherSituation::UpdateEntity(float fFrameTime)
{
	for (auto ent : m_entityCollection)
	{
		ent->Update(fFrameTime);
	}
}
//=============================================================================
// IWeatherSystemListener
//=============================================================================


void CWeatherSituation::OnSituationAboutToChange(float fTimeLeft)
{
	if (m_bIsActive)
	{
		m_fTimeRemaining = fTimeLeft;
		m_todFader.OnSituationAboutToChange(fTimeLeft);

		for (auto cloud : m_simpleClouds)
		{
			cloud->OnSituationAboutToChange(fTimeLeft);
		}

		for (auto sound : m_soundCollection)
		{
			sound->FadeOut(fTimeLeft);
		}
	}
}

void CWeatherSituation::OnSituationChanging(string sNextSituation)
{
	if (m_bIsActive)
	{
		m_todFader.OnSituationChanging(sNextSituation);
		Reset();
	}
	m_bIsActive = false;

}

#pragma region Init
//=============================================================================
// Init
//=============================================================================

#pragma endregion
