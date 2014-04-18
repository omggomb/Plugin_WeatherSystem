#ifndef __FLOW_WEATHER_SYS_LISTENER_H__
#define __FLOW_WEATHER_SYS_LISTENER_H__

#include "Nodes/G2FlowBaseNode.h"
#include "ISituationListener.h"

class CFlowNode_WeatherSysListener : public CFlowBaseNode<eNCT_Instanced>, IWeatherSystemListener   // Alternatively eNCT_Singleton, eNCT_Clone                                 // Instanced needs to implement Clone() while Cloned can't do that
{
public:

	enum EInPorts
	{
		Activate,
		Deactivate
	};

	enum EOutPorts
	{
		AboutToChange,
		Changing
	};

	// IWeatherSystemListener
	void OnSituationAboutToChange(float fTimeLeft) override;
	void OnSituationChanging(string sNextSituation) override;

	// CFlowNode_WeatherSysListener

	CFlowNode_WeatherSysListener(SActivationInfo *pActInfo);

	virtual void GetConfiguration(SFlowNodeConfig &config) override;

	virtual void ProcessEvent(EFlowEvent ev, SActivationInfo *pActInfo) override;

	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo) override;

	virtual void GetMemoryUsage(ICrySizer *s) const override;
private:

	/**
	* Toggles listening */
	void ToggleListening(bool bListen, SActivationInfo *pActInfo);

	bool m_bActive;
	/** Has OnSituationAboutToChange been fired? */
	bool m_bAboutToChange;
	/**  Has OnSituationChanging been fired  */
	bool m_bChanging;
	/** Gets filled by OnSituationAboutToChange event */
	float m_fTimeLeft;
	/** Gets filled by OnSituationChanging event */
	string m_sNextSituation;


}; // _GetActorEnergy

REGISTER_FLOW_NODE("WeatherSystem:Listener", CFlowNode_WeatherSysListener);// Register your flownode REGISTER_FLOW_NODE("<folder>:<name>", <implementation>)

#endif