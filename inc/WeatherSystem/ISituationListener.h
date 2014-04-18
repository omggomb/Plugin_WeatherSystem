#ifndef __IWEATHER_SYSTEM_LISTENER_H__
#define __IWEATHER_SYSTEM_LISTENER_H__

struct IWeatherSystemListener
{
	virtual ~IWeatherSystemListener() {}

	/**
	* Called when a weahter situation is about to be changed
	* Called when 1/10th of total time is left
	* @param fTimeLeft Time left until current situation changes */
	virtual void OnSituationAboutToChange(float fTimeLeft) = 0;


	/**
	* Called when the current situation is updated one last time before
	* a new situation takes place. The final chance to set things straight if you will */
	virtual void OnSituationChanging(string sNextSituation) = 0;

};

#endif // !__IWEATHER_SYSTEM_LISTENER_H__

