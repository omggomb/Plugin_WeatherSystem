#ifndef __CPLUGIN_WEATHER_SYSTEM_H__
#define __CPLUGIN_WEATHER_SYSTEM_H__

/* WeatherSystem_Plugin - for licensing and copyright see license.txt */

#include <IPluginBase.h>
#include <Game.h>

#include <IPluginManager.h>
#include <CPluginBase.hpp>

#include <IPluginWeatherSystem.h>

#include "WeatherSystem.h"
#include "ISystem.h"

#define PLUGIN_NAME "WeatherSystem"
#define PLUGIN_CONSOLE_PREFIX "[" PLUGIN_NAME " " PLUGIN_TEXT "] " //!< Prefix for Logentries by this plugin

namespace WeatherSystemPlugin
{
/**
* @brief Provides information and manages the resources of this plugin.
*/
class CPluginWeatherSystem :
	public PluginManager::CPluginBase,
	public IPluginWeatherSystem,
	public ISystemEventListener
{
public:
	CPluginWeatherSystem();
	~CPluginWeatherSystem();

	// IPluginBase
	bool Release(bool bForce = false) override;

	int GetInitializationMode() const override;

	bool Init(SSystemGlobalEnvironment &env, SSystemInitParams &startupParams, IPluginBase *pPluginManager, const char *sPluginDirectory) override;

	bool RegisterTypes(int nFactoryType, bool bUnregister) override;

	const char *GetVersion() const override;

	const char *GetName() const override;

	const char *GetCategory() const override;
	const char *ListAuthors() const override;

	const char *ListCVars() const override;

	const char *GetStatus() const override;

	const char *GetCurrentConcreteInterfaceVersion() const override;

	void *GetConcreteInterface(const char *sInterfaceVersion)override;

	// IPluginWeatherSystem
	IPluginBase *GetBase() override;

	// ISysteEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;

	// CPluginWeatherSystem
	CWeatherSystem *GetWeatherSystem() const
	{
		return m_pWeatherSystem;
	}

private:

	CWeatherSystem *m_pWeatherSystem;

	// TODO: Add your concrete interface implementation
};

extern CPluginWeatherSystem *gPlugin;
}

/**
* @brief This function is required to use the Autoregister Flownode without modification.
* Include the file "CPluginWeatherSystem.h" in front of flownode.
*/
inline void GameWarning(const char *sFormat, ...) PRINTF_PARAMS(1, 2);
inline void GameWarning(const char *sFormat, ...)
{
	va_list ArgList;
	va_start(ArgList, sFormat);
	WeatherSystemPlugin::gPlugin->LogV(ILog::eWarningAlways, sFormat, ArgList);
	va_end(ArgList);
};


#endif // !__CPLUGIN_WEATHER_SYSTEM_H__
