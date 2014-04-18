#ifndef __FLOW_WEATHER_SYSTEM_H__
#define __FLOW_WEATHER_SYSTEM_H__

#include "CPluginWeatherSystem.h"
#include "Nodes/G2FlowBaseNode.h"

class CFlowNode_WeatherSystem : public CFlowBaseNode<eNCT_Singleton>    // Alternatively eNCT_Singleton, eNCT_Clone                                 // Instanced needs to implement Clone() while Cloned can't do that
{
public:

	enum EInPorts
	{
		eIn_Get,
		eIn_SetActive,
	};

	enum EOutPorts
	{
		eOut_IsActive,
		eOut_CurrentSit,
		eOut_TimeLimit,
		eOut_TimeLeft,
		eOut_NextSit,
	};

	// CFlowNode_WeatherSystem

	CFlowNode_WeatherSystem(SActivationInfo *pActInfo);

	virtual void GetConfiguration(SFlowNodeConfig &config) override;

	virtual void ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo) override;

	virtual void GetMemoryUsage(ICrySizer *s) const override;
private:

};

REGISTER_FLOW_NODE("WeatherSystem:WeatherSystem", CFlowNode_WeatherSystem);

//=============================================================================
// Force situation
//=============================================================================

class CFlowNode_WSForceSit : public CFlowBaseNode<eNCT_Singleton>    // Alternatively eNCT_Singleton, eNCT_Clone                                 // Instanced needs to implement Clone() while Cloned can't do that
{
public:

	enum EInPorts
	{
		eIn_Force,
		eIn_Name

	};

	enum EOutPorts
	{
		eOut_Done
	};

	// CFlowNode_WSForceSit

	CFlowNode_WSForceSit(SActivationInfo *pActInfo)
	{
	}

	void GetConfiguration(SFlowNodeConfig &config) override
	{
		static const SInputPortConfig in_config[] =
		{
			InputPortConfig<SFlowSystemVoid> ("Force", _HELP("Force situation to be applied")),
			InputPortConfig<string>("Name", _HELP("Name of situation to be applied")),
			InputPortConfig_Null(),// InputPortConfig<datatype> ("PortName", _HELP("Help message")),
		};

		static const SOutputPortConfig out_config[] =
		{
			OutputPortConfig<bool> ("Done", _HELP("Called when new situation has started")),
			OutputPortConfig_Null(),
		};

		config.sDescription = _HELP("Forces a situation to be spawned"); // Node description
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;                                       // special things like target entity
		config.SetCategory(EFLN_APPROVED);  // category
	}

	void ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo) override
	{
		if (ev == EFlowEvent::eFE_Activate)
		{
			if (IsPortActive(pActInfo, eIn_Force))
			{
				string sName = GetPortString(pActInfo, eIn_Name);
				ActivateOutput(pActInfo, eOut_Done,
							   WeatherSystemPlugin::gPlugin->GetWeatherSystem()->TryForceSituation(sName));
			}
		}
	}

	void GetMemoryUsage(ICrySizer *s) const override
	{
		s->Add(*this);
	}

private:

};

REGISTER_FLOW_NODE("WeatherSystem:ForceSituation", CFlowNode_WSForceSit);

//=============================================================================
// Fade out and stop
//=============================================================================

class CFlowNode_WSFadeOutAndStop : public CFlowBaseNode<eNCT_Singleton>    // Alternatively eNCT_Singleton, eNCT_Clone                                 // Instanced needs to implement Clone() while Cloned can't do that
{
public:

	enum EInPorts
	{
		eIn_FadeOut,
		eIn_Time,
		eIn_KeepWeather

	};

	enum EOutPorts
	{
		eOut_Fading,
		eOut_Done
	};

	// CFlowNode_WSForceSit

	CFlowNode_WSFadeOutAndStop(SActivationInfo *pActInfo)
	{
	}

	void GetConfiguration(SFlowNodeConfig &config) override
	{
		static const SInputPortConfig in_config[] =
		{
			InputPortConfig<SFlowSystemVoid> ("FadeOut", _HELP("Fade out situation and stop weather system")),
			InputPortConfig<float>("FadeTime", _HELP("Time for the situation to be faded out")),
			InputPortConfig<bool>("KeepWeather", _HELP("Keep situation weather or restore level weather")),
			InputPortConfig_Null(),// InputPortConfig<datatype> ("PortName", _HELP("Help message")),
		};

		static const SOutputPortConfig out_config[] =
		{
			OutputPortConfig<SFlowSystemVoid> ("Fading", _HELP("Called when new fading has started")),
			OutputPortConfig<SFlowSystemVoid> ("Done", _HELP("Called when fading has finished")),
			OutputPortConfig_Null(),
		};

		config.sDescription = _HELP("Fades out current situation and stops weather system"); // Node description
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;                                       // special things like target entity
		config.SetCategory(EFLN_APPROVED);  // category
	}

	void ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo) override
	{
		switch (ev)
		{
		case EFlowEvent::eFE_Activate:
		{
			if (IsPortActive(pActInfo, eIn_FadeOut))
			{
				float fTimeToFade = GetPortFloat(pActInfo, EInPorts::eIn_Time);
				bool bKeepWeather = GetPortBool(pActInfo, EInPorts::eIn_KeepWeather);
				WeatherSystemPlugin::gPlugin->GetWeatherSystem()->FadeOutCurrentSituation(fTimeToFade, bKeepWeather);
				ActivateOutput(pActInfo, eOut_Fading, SFlowSystemVoid());
				m_fFadeTime = fTimeToFade;
				ToggleActive(true, pActInfo);
			}
		}
		break;
		case EFlowEvent::eFE_Update:
		{
			if (m_fFadeTimer >= m_fFadeTime)
			{
				ToggleActive(false, pActInfo);
				ActivateOutput(pActInfo, EOutPorts::eOut_Done, SFlowSystemVoid());
				m_fFadeTimer = 0.f;
			}
			else
			{
				m_fFadeTimer += gEnv->pTimer->GetFrameTime();
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

	void GetMemoryUsage(ICrySizer *s) const override
	{
		s->Add(*this);
	}

private:

	void ToggleActive(bool bActive, SActivationInfo *pActInfo)
	{
		pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, bActive);
	}

	float m_fFadeTimer;
	float m_fFadeTime;
};

REGISTER_FLOW_NODE("WeatherSystem:FadeOutAndStop", CFlowNode_WSFadeOutAndStop);

//=============================================================================
// Set next situation
//=============================================================================


class CFlowNode_WSSetNextSituation : public CFlowBaseNode<eNCT_Singleton>    // Alternatively eNCT_Singleton, eNCT_Clone                                 // Instanced needs to implement Clone() while Cloned can't do that
{
public:

	enum EInPorts
	{
		eIn_Set,
		eIn_Name
	};

	enum EOutPorts
	{

		eOut_Done
	};

	// CFlowNode_WSForceSit

	CFlowNode_WSSetNextSituation(SActivationInfo *pActInfo)
	{
	}

	void GetConfiguration(SFlowNodeConfig &config) override
	{
		static const SInputPortConfig in_config[] =
		{
			InputPortConfig<SFlowSystemVoid> ("Set", _HELP("Set next situation to be applied when current has finished")),
			InputPortConfig<string>("Name", _HELP("Name of situation to be set as next")),
			InputPortConfig_Null(),// InputPortConfig<datatype> ("PortName", _HELP("Help message")),
		};

		static const SOutputPortConfig out_config[] =
		{
			OutputPortConfig<bool> ("Done", _HELP("Called when next has been set, indicates succes of setting")),
			OutputPortConfig_Null(),
		};

		config.sDescription = _HELP("Set next situation to be applied when current has finished"); // Node description
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;                                       // special things like target entity
		config.SetCategory(EFLN_APPROVED);  // category
	}

	void ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo) override
	{
		switch (ev)
		{
		case EFlowEvent::eFE_Activate:
		{
			if (IsPortActive(pActInfo, eIn_Set))
			{
				string sName = GetPortString(pActInfo, eIn_Name);
				ActivateOutput(pActInfo, eOut_Done,
							   WeatherSystemPlugin::gPlugin->GetWeatherSystem()->TrySetNextSituation(sName));
			}
		}
		default:
			break;
		}
	}

	void GetMemoryUsage(ICrySizer *s) const override
	{
		s->Add(*this);
	}

private:
};

REGISTER_FLOW_NODE("WeatherSystem:SetNextSituation", CFlowNode_WSSetNextSituation);

#endif // !__FLOW_WEATHER_SYSTEM_H__