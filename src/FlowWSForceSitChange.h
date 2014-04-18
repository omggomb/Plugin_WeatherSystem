#ifndef __FLOW_WS_FORCE_SIT_CHANGE__
#define __FLOW_WS_FORCE_SIT_CHANGE__

#include "CPluginWeatherSystem.h"
#include "Nodes/G2FlowBaseNode.h"

class CFlowNode_WSForceSitChange : public CFlowBaseNode<eNCT_Singleton>    // Alternatively eNCT_Singleton, eNCT_Clone                                 // Instanced needs to implement Clone() while Cloned can't do that
{
public:

	enum EInPorts
	{
		eIn_Force,
		eIn_Name,
		eIn_Time
	};

	enum EOutPorts
	{
		eOut_Changing,
		eOut_Done
	};

	// CFlowNode_WeatherSystem

	CFlowNode_WSForceSitChange(SActivationInfo *pActInfo);

	virtual void GetConfiguration(SFlowNodeConfig &config) override;

	virtual void ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo) override;

	virtual void GetMemoryUsage(ICrySizer *s) const override;
private:

	void ToggleActive(bool bActive, SActivationInfo *pActInfo);

	float m_fTimer;
	float m_fFadeOutTime;
};

REGISTER_FLOW_NODE("WeatherSystem:ForceSituationChange", CFlowNode_WSForceSitChange)

#endif // !__FLOW_WS_FORCE_SIT_CHANGE__