/* WeatherSystem_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include "CPluginWeatherSystem.h"

namespace WeatherSystemPlugin
{
CPluginWeatherSystem *gPlugin = NULL;

CPluginWeatherSystem::CPluginWeatherSystem()
{
	gPlugin = this;
	m_pWeatherSystem = new CWeatherSystem();
}

CPluginWeatherSystem::~CPluginWeatherSystem()
{
	Release(true);

	gPlugin = NULL;
}

bool CPluginWeatherSystem::Release(bool bForce)
{
	bool bRet = true;
	bool bWasInitialized = m_bIsFullyInitialized; // Will be reset by base class so backup

	if (!m_bCanUnload)
	{
		// Note: Type Unregistration will be automatically done by the Base class (Through RegisterTypes)
		// Should be called while Game is still active otherwise there might be leaks/problems
		bRet = CPluginBase::Release(bForce);

		if (bRet)
		{
			if (bWasInitialized)
			{
				// TODO: Cleanup stuff that can only be cleaned up if the plugin was initialized
			}

			// Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
			gPluginManager->UnloadPlugin(GetName());

			// Allow Plugin Manager garbage collector to unload this plugin
			AllowDllUnload();
		}
	}

	return bRet;
};

bool CPluginWeatherSystem::Init(SSystemGlobalEnvironment &env, SSystemInitParams &startupParams, IPluginBase *pPluginManager, const char *sPluginDirectory)
{
	gPluginManager = (PluginManager::IPluginManager *)pPluginManager->GetConcreteInterface(NULL);
	CPluginBase::Init(env, startupParams, pPluginManager, sPluginDirectory);

	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);

	return true;
}

bool CPluginWeatherSystem::RegisterTypes(int nFactoryType, bool bUnregister)
{
	// Note: Autoregister Flownodes will be automatically registered by the Base class
	bool bRet = CPluginBase::RegisterTypes(nFactoryType, bUnregister);

	using namespace PluginManager;
	eFactoryType enFactoryType = eFactoryType(nFactoryType);

	if (bRet)
	{
		if (gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting())
		{
			// UIEvents
			if (gEnv->pConsole && (enFactoryType == FT_All || enFactoryType == FT_UIEvent))
			{
				if (!bUnregister)
				{
					// TODO: Register CVars here if you have some
					// ...
				}

				else
				{
					// TODO: Unregister CVars here if you have some
					// ...
				}
			}

			// CVars
			if (gEnv->pConsole && (enFactoryType == FT_All || enFactoryType == FT_CVar))
			{
				if (!bUnregister)
				{
					// TODO: Register CVars here if you have some
					// ...
				}

				else
				{
					// TODO: Unregister CVars here if you have some
					// ...
				}
			}

			// CVars Commands
			if (gEnv->pConsole && (enFactoryType == FT_All || enFactoryType == FT_CVarCommand))
			{
				if (!bUnregister)
				{
					// TODO: Register CVar Commands here if you have some
					// ...
				}

				else
				{
					// TODO: Unregister CVar Commands here if you have some
					// ...
				}
			}

			// Game Objects
			if (gEnv->pGame && gEnv->pGame->GetIGameFramework() && (enFactoryType == FT_All || enFactoryType == FT_GameObjectExtension))
			{
				if (!bUnregister)
				{
					// TODO: Register Game Object Extensions here if you have some
					// ...
				}
			}

			if (enFactoryType == FT_Flownode)
			{

			}
		}
	}

	return bRet;
}

const char *CPluginWeatherSystem::ListCVars() const
{
	return "..."; // TODO: Enter CVARs/Commands here if you have some
}

const char *CPluginWeatherSystem::GetStatus() const
{
	return "OK";
}

int CPluginWeatherSystem::GetInitializationMode() const
{
	return int(PluginManager::IM_Default);
};

const char *CPluginWeatherSystem::GetVersion() const
{
	return "0.1";
};

const char *CPluginWeatherSystem::GetName() const
{
	return PLUGIN_NAME;
};

const char *CPluginWeatherSystem::GetCategory() const
{
	return "Visual";
};

const char *CPluginWeatherSystem::ListAuthors() const
{
	return "omggomb <omggomb at gmx dot net>";
};



const char *CPluginWeatherSystem::GetCurrentConcreteInterfaceVersion() const
{
	return "1.0";
};

void *CPluginWeatherSystem::GetConcreteInterface(const char *sInterfaceVersion)
{
	return static_cast <IPluginWeatherSystem *>(this);
};

// IPluginWeatherSystem
CPluginWeatherSystem::IPluginBase *CPluginWeatherSystem::GetBase()
{
	return static_cast<IPluginBase *>(this);
};

void CPluginWeatherSystem::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	if (event == ESystemEvent::ESYSTEM_EVENT_GAME_POST_INIT)
	{
		if (!m_pWeatherSystem->Init())
		{
			WEATHERSSYSTEM_ERROR("Failed to initialize weather system!");
			SAFE_DELETE(m_pWeatherSystem);
		}
	}
}

// TODO: Add your plugin concrete interface implementation
}