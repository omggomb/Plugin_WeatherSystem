#ifndef __IWEATHER_SITUATION_H__
#define __IWEATHER_SITUATION_H__

#include "IXml.h"
#include "ISituationListener.h"



#define WEAHTERSYSTEM_LOG_TAG "[Weather System]"
#define WEATHERSYSTEM_LOG(message, ...) char buffer[256]; sprintf_s(buffer, message, __VA_ARGS__); CryLogAlways("%s %s", WEAHTERSYSTEM_LOG_TAG, buffer)
#define WEATHERSYSTEM_WARNING(message, ...) char buffer[256]; sprintf_s(buffer, message, __VA_ARGS__); \
	CryWarning(EValidatorModule::VALIDATOR_MODULE_GAME, EValidatorSeverity::VALIDATOR_WARNING, \
			   "%s %s", WEAHTERSYSTEM_LOG_TAG, buffer);

#define WEATHERSSYSTEM_ERROR(message,...) char buffer[256]; sprintf_s(buffer, message, __VA_ARGS__); \
	CryWarning(EValidatorModule::VALIDATOR_MODULE_GAME, EValidatorSeverity::VALIDATOR_ERROR, \
			   "%s %s", WEAHTERSYSTEM_LOG_TAG, buffer);


/**
* A weather situation represents a single type of weather with all its global effects
*/
struct IWeatherSituation : public IWeatherSystemListener
{
	virtual ~IWeatherSituation() {}

	// IWeatherSystemListener
	virtual void OnSituationAboutToChange(float fTimeLeft) = 0;
	virtual void OnSituationChanging(string sNextSituation) = 0;

	// IWeatherSituation

	/**
	* Loads all relevant data from the given xml node
	* @returns True on success
	*/
	virtual bool LoadFromXml(XmlNodeRef root) = 0;

	/**
	* Resets all timers  */
	virtual void Reset() = 0;
	/**
	* Applies this weather situation to the gameworld
	*/
	virtual void Apply() = 0;

	/**
	* Used to update timings etc.
	*/
	virtual void Update(float fFrameTime) = 0;

	/**
	* Returns the name of this situation, must be unique
	*/
	virtual string GetName() const = 0;

	/**
	* Gets the time span during which this situation is allowed to begin  */
	virtual void GetTimeSpan(float &fFrom, float &fTo) const = 0;

	/**
	* Gets list of possible successors for this situation  */
	virtual const std::vector<string> &GetPossibleSuccessors() const = 0;

	virtual void GetMinMaxDuration(float &fMin, float &fMax) const = 0;

	/**
	* May this situation be used in the weather system cycle  */
	virtual bool CanBeUsedInCycle() const = 0;

	/**
	* Gets the factor for the situation's time after which it should be faded out 
	* If the situation lasts 100 secs, and the fadeOutTimeFactor is 0.9,
	* the situation starts to fade out after 90 secs*/
	virtual float GetFadeOutTimeFactor() const = 0;



};

#endif // !__IWEATHER_SITUATION_H__
