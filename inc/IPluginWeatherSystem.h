#ifndef __IPLUGIN_WEATHER_SYSTEM_H__
#define __IPLUGIN_WEATHER_SYSTEM_H__

#include <IPluginBase.h>

/**
* @brief WeatherSystem Plugin Namespace
*/
namespace WeatherSystemPlugin
{
/**
* @brief plugin WeatherSystem concrete interface
*/
struct IPluginWeatherSystem
{
	/**
	* @brief Get Plugin base interface
	*/
	virtual PluginManager::IPluginBase *GetBase() = 0;

	// TODO: Add your concrete interface declaration here
};
};

#endif // !__IPLUGIN_WEATHER_SYSTEM_H__/* WeatherSystem_Plugin - for licensing and copyright see license.txt */

