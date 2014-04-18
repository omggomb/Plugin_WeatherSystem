//=============================================================================
// Week 3: It shall from now on be know that every public attribute of a struct
// or class will be prefixed with "x_" in order to distinguish it from private
// attributes "m_". Think of it as a drunk UML plus sign.
//=============================================================================



#ifndef __WEATHER_SYSTEM_H__
#define __WEATHER_SYSTEM_H__

#include "IGameFramework.h"
#include <vector>
#include "IWeatherSituation.h"
#include "ISituationListener.h"
#include <map>
#include "SituationRain.h"
#include "SituationSnow.h"
#include "ISystem.h"
#include "SituationAttributes.h"
//#include <string>


#define WESY_SYSTEM_ACTIVE_CVAR                 "ws_weatherSystemActive"

#define WESY_LOAD_SITUATION_CCOMMAND            "WSForceSituation"
#define WESY_RELOAD_SITUATIONS_CCOMMAND         "WSReloadSituations"
#define WESY_LIST_SITUATIONS_CCOMMAND           "WSListSituations"
#define WESY_FORCE_SITUATION_CHANGE_CCOMMAND    "WSForceSituationChange"
#define WESY_FADE_OUT_AND_STOP_CCOMMAND         "WSFadeOutAndStop"

/**
* Callback for the WeatherSystemForceSituation console command
*/
void OnForceSituationCommand(IConsoleCmdArgs *pargs);
/**
* Callback for the WeatherSystemReloadSituations console command
*/
void OnReloadSituationsCommand(IConsoleCmdArgs *pArgs);

/**
* Callback for the WSListSituations command  */
void OnListSituationsCommand(IConsoleCmdArgs *pArgs);

/**
* Callback for force situation change command  */
void OnForceSituationChangeCommand(IConsoleCmdArgs *pArgs);

/**
* Callback for the WSFadeOutAndStopCommand  */
void OnFadeOutAndStopCommand(IConsoleCmdArgs *pArgs);

/**
* Callback for the ws_weatherSystemActive CVar  */
void OnSystemActiveCVarChanged(ICVar *pCVar);






class CWeatherSystem : public IGameFrameworkListener, ISystemEventListener
{
public:

	CWeatherSystem();
	virtual ~CWeatherSystem();

	// IGameFrameworkListener
	virtual void OnPostUpdate(float fDeltaTime) override;
	virtual void OnSaveGame(ISaveGame *pSaveGame) override {}
	virtual void OnLoadGame(ILoadGame *pLoadGame) override {}
	virtual void OnLevelEnd(const char *nextLevel) override {}
	virtual void OnActionEvent(const SActionEvent &event) override;

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;

	// CWeatherSystem

	/**
	* Initializes the weather system, but does not load any definitions
	* @returns True on succes
	*/
	bool Init();

	/**
	* Loads all defined weather situations inside the given folder
	* @param sFolderPath Path to the containing folder
	* @returns True on succes
	*/
	bool LoadSituationsFromFile(string sFolderPath);

	/**
	* Switches the weather system on or off.
	*/
	void ToggleSystem(bool bEnableWeatherSystem);

	/**
	* Returns wether the system is currently active
	*/
	bool IsActive()
	{
		return m_bIsSystemActive;
	}

	/**
	* If situation exists aborts current and applies specified
	*/
	bool TryForceSituation(string sName);

	/**
	* Elects a new situation and forces it immediately  */
	void ForceSituationChange();
	void ForceSituationChange(float fFadeOutTime);

	/**
	* Reloads situation definitions and deletes old ones
	*/
	void ReloadSituationDefinitions();

	/**
	* Add a system listener to the notify list
	* A listener can only be added once */
	void AddWeatherSystemListener(IWeatherSystemListener *pListener);

	/**
	* Removes a system listener fromm the notify list */
	void RemoveWeatherSystemListener(IWeatherSystemListener *pListener);

	/**
	* Dumps all loaded situation definitions to the log  */
	void DumpSituationsToLog();

	/**
	* Fades out current situation and if desired restores level weather
	* @param fTimeToFadeOut How much time should it take for the current situation to vanish
	* @param bRestoreLevelWeather Should the level weather be restored or the next situation be loaded
	* @param bToggleWeatherSystem Turn weather system on or off */
	void FadeOutCurrentSituation(float fTimeToFadeOut, bool bRestoreLevelWeather = false, bool bToggleWeatherSystem = true);

	/**
	* Get a pointer to the currently active situation  */
	IWeatherSituation *GetCurrentlyActiveSituation()
	{
		return m_pCurrentlyActiveSituation;
	}

	/**
	* Get a pointer to the next situation in line  */
	IWeatherSituation *GetNextSituation()
	{
		return m_pNextSituation;
	}

	/**
	* Gets the time that is left for the current situation  */
	float GetTimeLeftForCurrentSit()
	{
		return m_fCurrentSituationsTimeLimit - m_fSituationTimer;
	}

	/**
	* Gets the current situation's total time limit  */
	float GetCurrentSitTimeLimit()
	{
		return m_fCurrentSituationsTimeLimit;
	}

	/**
	* Ignored situation won't be spawned */
	void IgnoreSituation(string sName, bool bIgnore);

	/**
	* If specified situation exists, sets as next, does not check for other
	* validation criteria */
	bool TrySetNextSituation(string sName);


private:

	/** Decouple framework from actual update */
	void Update(float fFrameTime);

	/** Fadeout update cycle */
	void FadeOutUpdate(float fFrameTime);

	/** Fire OnSituationAboutToChange event */
	void FireOnSituationAboutToChange(float fTimeLeft);

	/** Fire OnSituationChanging event */
	void FireOnSituationChanging(string sNextSituation);

	/**
	* Clears the list of defined situations and deletes all of them  */
	void ClearDefinedSituations();

	/**
	* Iterates through appropriate definitions and chooses one radomly
	* @param currentFollowedBy vector with all possible successors for the current situation */
	IWeatherSituation *GetNextSituation(const std::vector<string> &currentFollowedBy);

	/**
	* Spawns a new situation and resets timers */
	void SpawnSituation(IWeatherSituation *pSitToSpawn);

	/**
	* Calcs time span for new situation within console var bounds  */
	float GetNewSituationDuration();

	/**
	* Restores weather to level defined */
	void RestoreWeather(bool bResetSituation);

	/**
	* Resets fade related vars */
	void Reset();

	/** Update weather system? != m_bIsSystemActive (Weather is preserved here) */
	bool m_bUpdate;
	/** Timer for current situation */
	float m_fSituationTimer;
	/** Calculated time for the currently active sitaution */
	float m_fCurrentSituationsTimeLimit;
	/** Has the OnSituationAboutToChange event been fired already? */
	bool m_bIsAboutToChangeFired;
	/** Should active situation be faded out atm? */
	bool m_bFadeOutCurrent;
	/** Time for the current situation to fade out */
	float m_fFadeOutTime;
	/** After fading out, stop system? */
	bool m_bStopSystemAfterFadeOut;
	/** After fading out restore level weather? */
	bool m_bRestoreLevelWeatherAfterFadeout;

	/** Contains all the loaded weather situations */
	std::map<string , IWeatherSituation *> m_definedSituations;
	/** Indicates wether the system is currently active */
	bool m_bIsSystemActive;
	/** id of currently active weather situation */
	IWeatherSituation *m_pCurrentlyActiveSituation;
	/** Next situation to be spawned */
	IWeatherSituation *m_pNextSituation;
	/** Stock weather, without any modifications by the system */
	SSituationRain m_stockRainParams;
	SNOWPARAMS m_stockSnowParams;
	/** Weather at the time fade out is requested */
	SSituationRain m_fadeFromRainParams;
	SNOWPARAMS m_fadeFromSnowParams;

	/** CVar use to deactivate or active the weather system */
	ICVar *m_pSystemActiveCVar;

	/** Registered weather system listeners */
	std::vector<IWeatherSystemListener *> m_systemListeners;

	/** Situation blacklist */
	std::set<string> m_ignoredSituations;

	float m_fDummyCounter;
};

#endif // !__WEATHER_SYSTEM_H__


