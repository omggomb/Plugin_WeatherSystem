#include "StdAfx.h"
#include "FlowWeatherSysListener.h"
#include "Game.h"
#include "CPluginWeatherSystem.h"

using namespace WeatherSystemPlugin;

void CFlowNode_WeatherSysListener::OnSituationAboutToChange(float fTimeLeft)
{
	m_bAboutToChange = true;
	m_fTimeLeft = fTimeLeft;
}
void CFlowNode_WeatherSysListener::OnSituationChanging(string sNextSituation)
{
	m_sNextSituation = sNextSituation;
	m_bChanging = true;
}

CFlowNode_WeatherSysListener::CFlowNode_WeatherSysListener(SActivationInfo *pActInfo)
{
	m_bActive = false;
	m_bAboutToChange = false;
	m_bChanging = false;

	m_fTimeLeft = 0.f;
	m_sNextSituation = "";
}

void CFlowNode_WeatherSysListener::GetConfiguration(SFlowNodeConfig &config)
{
	static const SInputPortConfig in_config[] =
	{
		InputPortConfig_Void("Activate", _HELP("Activate listening")),
		InputPortConfig_Void("Deactivate", _HELP("Deactivate listening")),
		InputPortConfig_Null(),// InputPortConfig<datatype> ("PortName", _HELP("Help message")),
	};

	static const SOutputPortConfig out_config[] =
	{
		OutputPortConfig<float> ("OnSituationAboutToChange", _HELP("Called when time is up for situation")),
		OutputPortConfig<string>("OnSituationChanging", _HELP("Called when new situation is spawned")),
		OutputPortConfig_Null(),
	};

	config.sDescription = _HELP("Gets the current actor energy"); // Node description
	config.pInputPorts = in_config;
	config.pOutputPorts = out_config;                                       // special things like target entity
	config.SetCategory(EFLN_APPROVED);  // category

}

void CFlowNode_WeatherSysListener::ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo)
{
	switch (ev)
	{
	case EFlowEvent::eFE_Activate:
	{
		if (IsPortActive(pActInfo, Activate))
		{
			ToggleListening(true, pActInfo);
		}
		else if (IsPortActive(pActInfo, Deactivate))
		{
			ToggleListening(false, pActInfo);
		}
	}
	break;
	case EFlowEvent::eFE_Update:
	{
		if (m_bAboutToChange)
		{
			ActivateOutput(pActInfo, AboutToChange, m_fTimeLeft);
			m_bAboutToChange = false;
		}

		if (m_bChanging)
		{
			ActivateOutput(pActInfo, Changing, m_sNextSituation);
			m_bChanging = false;
		}
	}
	break;
	case EFlowEvent::eFE_Suspend:
	{
		ToggleListening(false, pActInfo);
	}
	break;
	case EFlowEvent::eFE_Resume:
	{
		ToggleListening(true, pActInfo);
	}
	break;
	default:
		break;
	}

}

IFlowNodePtr CFlowNode_WeatherSysListener::Clone(SActivationInfo *pActInfo)
{
	return new CFlowNode_WeatherSysListener(pActInfo);
}

void CFlowNode_WeatherSysListener::GetMemoryUsage(ICrySizer *s) const     // Every flownode needs this otherwise it won't compile
{
	s->Add(*this);
}

//=============================================================================
// Private
//=============================================================================

void CFlowNode_WeatherSysListener::ToggleListening(bool bListen, SActivationInfo *pActInfo)
{
	if (bListen)
	{
		gPlugin->GetWeatherSystem()->AddWeatherSystemListener(this);
	}
	else
	{
		gPlugin->GetWeatherSystem()->RemoveWeatherSystemListener(this);
	}

	pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, bListen);
	m_bActive = bListen;
}