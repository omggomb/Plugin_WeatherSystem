#ifndef __FLOW_WEATHER_SYS_IGNORE_SIT_H__
#define __FLOW_WEATHER_SYS_IGNORE_SIT_H__

#include "Nodes/G2FlowBaseNode.h"

class CFlowNode_WSIngoreSit : public CFlowBaseNode<eNCT_Singleton>      // Alternatively eNCT_Singleton, eNCT_Clone                                 // Instanced needs to implement Clone() while Cloned can't do that
{
public:

	enum EInPorts
	{
		eIn_Ignore,
		eIn_Name,
	};

	enum EOutPorts
	{
		eOut_Done
	};

	// CFlowNode_WSIngoreSit

	CFlowNode_WSIngoreSit(SActivationInfo *pActInfo);

	virtual void GetConfiguration(SFlowNodeConfig &config) override;

	virtual void ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo) override;

	virtual void GetMemoryUsage(ICrySizer *s) const override;
private:

};

REGISTER_FLOW_NODE("WeatherSystem:IgnoreSituation", CFlowNode_WSIngoreSit);

#endif // !__FLOW_WEATHER_SYS_IGNORE_SIT_H__