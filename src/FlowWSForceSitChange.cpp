#include "StdAfx.h"
#include "FlowWSForceSitChange.h"

using namespace WeatherSystemPlugin;


CFlowNode_WSForceSitChange::CFlowNode_WSForceSitChange(SActivationInfo *pActInfo)
{
	m_fTimer = 0.f;
}

void CFlowNode_WSForceSitChange::GetConfiguration(SFlowNodeConfig &config)
{
	static const SInputPortConfig in_config[] =
	{
		InputPortConfig<SFlowSystemVoid> ("Force", _HELP("Force situation change")),
		InputPortConfig<string>("Name", _HELP("Name of situation to be changed to, leave empty to let system choose")),
		InputPortConfig<float>("Time", _HELP("Time for the current situation to fade out")),
		InputPortConfig_Null(),// InputPortConfig<datatype> ("PortName", _HELP("Help message")),
	};

	static const SOutputPortConfig out_config[] =
	{
		OutputPortConfig<SFlowSystemVoid> ("Changing", _HELP("Called when action has been performed")),
		OutputPortConfig<SFlowSystemVoid> ("Done", _HELP("Called when new situation has started")),
		OutputPortConfig_Null(),
	};

	config.sDescription = _HELP("Forces a new situation to be spawned"); // Node description
	config.pInputPorts = in_config;
	config.pOutputPorts = out_config;                                       // special things like target entity
	config.SetCategory(EFLN_APPROVED);  // category
}

void CFlowNode_WSForceSitChange::ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo)
{
	switch (ev)
	{
	case EFlowEvent::eFE_Activate:
	{
		if (IsPortActive(pActInfo, eIn_Force))
		{
			float fTimeToFade = GetPortFloat(pActInfo, EInPorts::eIn_Time);
			string sNewName = GetPortString(pActInfo, EInPorts::eIn_Name);

			if (sNewName.empty() == false)
			{
				gPlugin->GetWeatherSystem()->TrySetNextSituation(sNewName);
				gPlugin->GetWeatherSystem()->ForceSituationChange(fTimeToFade);
			}
			else
			{
				gPlugin->GetWeatherSystem()->ForceSituationChange(fTimeToFade);
			}

			m_fFadeOutTime = fTimeToFade;
			ToggleActive(true, pActInfo);
		}
	}
	break;
	case EFlowEvent::eFE_Update:
	{
		if (m_fTimer >= m_fFadeOutTime)
		{
			ToggleActive(false, pActInfo);
			ActivateOutput(pActInfo, EOutPorts::eOut_Done, SFlowSystemVoid());
			m_fTimer = 0.f;
		}
		else
		{
			m_fTimer += gEnv->pTimer->GetFrameTime();
		}
	}
	break;
	case EFlowEvent::eFE_Suspend:
	{
		ToggleActive(false, pActInfo);
	}
	break;
	case EFlowEvent::eFE_Resume:
	{
		ToggleActive(true, pActInfo);
	}
	break;
	default:
		break;
	}

}

void CFlowNode_WSForceSitChange::GetMemoryUsage(ICrySizer *s) const
{
	s->Add(*this);
}

//=============================================================================
// Private
//=============================================================================

void CFlowNode_WSForceSitChange::ToggleActive(bool bActive, SActivationInfo *pActInfo)
{
	if (bActive)
	{
		pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, true);
	}
	else
	{
		pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, false);
	}
}