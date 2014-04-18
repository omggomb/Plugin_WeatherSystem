#include "StdAfx.h"
#include "FlowWeatherSysIgnoreSit.h"
#include "CPluginWeatherSystem.h"
using namespace WeatherSystemPlugin;

CFlowNode_WSIngoreSit::CFlowNode_WSIngoreSit(SActivationInfo *pActInfo)
{
}

void CFlowNode_WSIngoreSit::GetConfiguration(SFlowNodeConfig &config)
{
	static const SInputPortConfig in_config[] =
	{
		InputPortConfig<bool> ("Ignore", _HELP("Ignore or don't ignore")),
		InputPortConfig<string>("Name", _HELP("Name of situation to perform action for")),
		InputPortConfig_Null(),// InputPortConfig<datatype> ("PortName", _HELP("Help message")),
	};

	static const SOutputPortConfig out_config[] =
	{
		OutputPortConfig<SFlowSystemVoid> ("Done", _HELP("Called when action has been performed")),
		OutputPortConfig_Null(),
	};

	config.sDescription = _HELP("Adds or remove a Situation to / from the ignored list"); // Node description
	config.pInputPorts = in_config;
	config.pOutputPorts = out_config;                                       // special things like target entity
	config.SetCategory(EFLN_APPROVED);  // category
}

void CFlowNode_WSIngoreSit::ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo)
{
	switch (ev)
	{
	case EFlowEvent::eFE_Activate:
	{
		if (IsPortActive(pActInfo, eIn_Ignore))
		{
			gPlugin->GetWeatherSystem()->IgnoreSituation(GetPortString(pActInfo, eIn_Name), GetPortBool(pActInfo, eIn_Ignore));
			ActivateOutput(pActInfo, eOut_Done, SFlowSystemVoid());
		}
	}
	break;

	default:
		break;
	}
}

void CFlowNode_WSIngoreSit::GetMemoryUsage(ICrySizer *s) const
{
	s->Add(*this);
}