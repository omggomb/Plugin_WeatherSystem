#include "StdAfx.h"
#include "FlowWeatherSystem.h"
#include "WeatherSystem.h"
#include "Game.h"

using namespace WeatherSystemPlugin;

CFlowNode_WeatherSystem::CFlowNode_WeatherSystem(SActivationInfo *pActInfo)
{
}

void CFlowNode_WeatherSystem::GetConfiguration(SFlowNodeConfig &config)
{
	static const SInputPortConfig in_config[] =
	{
		InputPortConfig_Void("Get", _HELP("Get info available for this node")),
		InputPortConfig<bool>("SetActive", _HELP("Toggle weather system active (hard resets it, no fade out)")), // InputPortConfig<datatype> ("PortName", _HELP("Help message")),
		InputPortConfig_Null(),
	};

	static const SOutputPortConfig out_config[] =
	{
		OutputPortConfig<bool> ("IsActive", _HELP("Is weather system active")),
		OutputPortConfig<string>("CurrentSit", _HELP("Current situation's name")),
		OutputPortConfig<float>("CurrentSit", _HELP("Current situation's total time limit")),
		OutputPortConfig<float>("CurrentSit", _HELP("Current situation's time remaining")),
		OutputPortConfig<string>("CurrentSit", _HELP("Next situation's name")),
		OutputPortConfig_Null(),
	};

	config.sDescription = _HELP("Get info about weather system and toggles its activity"); // Node description
	config.pInputPorts = in_config;
	config.pOutputPorts = out_config;                                       // special things like target entity
	config.SetCategory(EFLN_APPROVED);  // category
}

void CFlowNode_WeatherSystem::ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo)
{
	switch (ev)
	{
	case EFlowEvent::eFE_Activate:
	{


		if (IsPortActive(pActInfo, eIn_Get))
		{
			ActivateOutput(pActInfo, eOut_IsActive, gPlugin->GetWeatherSystem()->IsActive());
			ActivateOutput(pActInfo, eOut_CurrentSit, gPlugin->GetWeatherSystem()->GetCurrentlyActiveSituation()->GetName());
			ActivateOutput(pActInfo, eOut_TimeLimit, gPlugin->GetWeatherSystem()->GetCurrentSitTimeLimit());
			ActivateOutput(pActInfo, eOut_TimeLeft, gPlugin->GetWeatherSystem()->GetTimeLeftForCurrentSit());
			ActivateOutput(pActInfo, eOut_NextSit, gPlugin->GetWeatherSystem()->GetNextSituation()->GetName());
		}

		if (IsPortActive(pActInfo, eIn_SetActive))
		{
			gPlugin->GetWeatherSystem()->ToggleSystem(GetPortBool(pActInfo, eIn_SetActive));
		}
	}
	break;
	default:
		break;
	}
}

void CFlowNode_WeatherSystem::GetMemoryUsage(ICrySizer *s) const
{
	s->Add(*this);
}
// Register your flownode REGISTER_FLOW_NODE("<folder>:<name>", <implementation>)
