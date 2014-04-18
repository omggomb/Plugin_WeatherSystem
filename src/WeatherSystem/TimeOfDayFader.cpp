#include "StdAfx.h"
#include "TimeOfDayFader.h"
#include "IWeatherSituation.h"
#include "Game.h"

CTimeOfDayFader::CTimeOfDayFader()
{
	m_bFadeOut = false;
	m_bIsActive = false;
}

CTimeOfDayFader::~CTimeOfDayFader()
{

}

bool CTimeOfDayFader::Init()
{
	gEnv->p3DEngine->GetTimeOfDay()->SetUpdateCallback(this);
	//g_pGame->GetWeatherSystem()->AddWeatherSystemListener(this); // Can't have it here :(
	m_bIsActive = true;
	return true;
}

void CTimeOfDayFader::BeginUpdate()
{
}

bool CTimeOfDayFader::GetCustomValue(ITimeOfDay::ETimeOfDayParamID paramID, int dim, float *pValues, float &blendWeight)
{
	std::vector<SSituationTodVar>::iterator it;

	// TODO: Maybe there is a less performance intensive solution

	for (it = m_todVars.begin(); it != m_todVars.end(); ++it)
	{
		if ((*it).id == paramID)
		{
			if (dim > 1)
			{
				if (it->type == ITimeOfDay::EVariableType::TYPE_COLOR)
				{
					pValues[0] = it->pValues[0];
					pValues[1] = it->pValues[1];
					pValues[2] = it->pValues[2];

					if (it->bFading)
					{
						blendWeight = it->fFadeProgression / it->fFadeInTime;
					}
					else
					{
						blendWeight = 1.0f;
					}
					break;
				}
			}
			else
			{
				if (it->type == ITimeOfDay::EVariableType::TYPE_FLOAT)
				{
					pValues[0] = it->pValues[0];

					if (it->bFading)
					{
						blendWeight = it->fFadeProgression / it->fFadeInTime;
					}
					else
					{
						blendWeight = 1.0f;
					}
					break;
				}
			}
		}
	}

	return true;
}

void CTimeOfDayFader::EndUpdate()
{
}

//=============================================================================
// IWeatherSystemListener
//=============================================================================


void CTimeOfDayFader::OnSituationAboutToChange(float fTimeLeft)
{
	if (m_bIsActive)
	{
		std::vector<SSituationTodVar>::iterator it;

		for (it = m_todVars.begin(); it != m_todVars.end(); ++it)
		{
			it->bFading = true;
			it->fFadeProgression = fTimeLeft;
			it->fFadeInStorage = it->fFadeInTime;
			it->fFadeInTime = fTimeLeft;
		}
		m_bFadeOut = true;
	}
}

void CTimeOfDayFader::OnSituationChanging(string sNextSituation)
{
	if (m_bIsActive)
	{
		std::vector<SSituationTodVar>::iterator it;

		for (it = m_todVars.begin(); it != m_todVars.end(); ++it)
		{
			// Rassign the original fFadeInTime as it has been changed to fTimeLeft by
			// OnSituationAboutToChange
			it->fFadeInTime = it->fFadeInStorage;
		}
	}

	m_bIsActive = false;
}

//=============================================================================
// CTimeOfDayFader
//=============================================================================

void CTimeOfDayFader::Update(float fFrameTime)
{
	std::vector<SSituationTodVar>::iterator it;



	for (it = m_todVars.begin(); it != m_todVars.end(); ++it)
	{
		SSituationTodVar &var = *it;

		if (var.bFading)
		{
			if (!m_bFadeOut)
			{
				var.fFadeProgression += fFrameTime;
			}
			else
			{
				var.bFading = true;
				var.fFadeProgression -= fFrameTime;
			}
		}

		if (var.fFadeProgression >= var.fFadeInTime && !m_bFadeOut)
		{
			var.bFading = false;
		}
	}


}

void CTimeOfDayFader::LoadFromXml(XmlNodeRef root)
{

	for (int i = 0; i < root->getChildCount(); ++i)
	{
		SSituationTodVar var;
		XmlNodeRef child = root->getChild(i);

		XmlString id;
		child->getAttr("id", id);


		ITimeOfDay::ETimeOfDayParamID paramId;
		if (CTimeOfDayFader::GetIdFromReadableName(id.c_str(), paramId))
		{
			var.id = paramId;
		}
		else
		{
			var.id = atoi(id.c_str());
		}

		child->getAttr("fadeInTime", var.fFadeInTime);
		child->getAttr("value0", var.pValues[0]);
		child->getAttr("value1", var.pValues[1]);
		child->getAttr("value2", var.pValues[2]);

		XmlString type;
		child->getAttr("type", type);
		if (strcmp(type.c_str(), "Float") == 0)
		{
			var.type = ITimeOfDay::EVariableType::TYPE_FLOAT;
		}
		else
		{
			var.type = ITimeOfDay::EVariableType::TYPE_COLOR;
		}

		m_todVars.push_back(var);
	}
}

void CTimeOfDayFader::Reset()
{
	for (auto var : m_todVars)
	{
		var.fFadeProgression = 0.f;
		var.bFading = true;


	}
	m_bFadeOut = false;
	gEnv->p3DEngine->GetTimeOfDay()->SetUpdateCallback(nullptr);
}

bool CTimeOfDayFader::GetIdFromReadableName(const char *sName, ITimeOfDay::ETimeOfDayParamID &out_paramId)
{
	if (gEnv)
	{
		string sNameString(sName);
		ITimeOfDay *ptod = gEnv->p3DEngine->GetTimeOfDay();
		ITimeOfDay::SVariableInfo varInfo;

		for (int i = 0; i < ptod->GetVariableCount(); ++i)
		{
			if (ptod->GetVariableInfo(i, varInfo))
			{
				string sVarDisplayName(varInfo.displayName);
				if (strcmp(sNameString.MakeLower(), sVarDisplayName.MakeLower()) == 0)
				{
					out_paramId = (ITimeOfDay::ETimeOfDayParamID)varInfo.nParamId;
					return true;
				}
			}
		}
		WEATHERSYSTEM_WARNING("Couldn't find matching paramId for \"%s\"", sName);

	}

	return false;
}