#ifndef __WEATHER_SITUATION_H__
#define __WEATHER_SITUATION_H__

#include "IWeatherSituation.h"
#include <vector>
#include "SituationAttributes.h"
#include "TimeOfDayFader.h"
#include "ISituationListener.h"

class CWeatherSituation : public IWeatherSituation
{
public:

	CWeatherSituation();
	virtual ~CWeatherSituation();


	// IWeatherSituation
	virtual bool LoadFromXml(XmlNodeRef root) override;
	virtual void Reset() override;
	virtual void Apply() override;
	virtual void Update(float fFrameTime) override;
	virtual string GetName() const override
	{
		return m_sName;
	}
	virtual void GetTimeSpan(float &fFrom, float &fTo) const override
	{
		fFrom = m_fTimeFrom;
		fTo = m_fTimeTo;
	}
	virtual const std::vector<string> &GetPossibleSuccessors() const override
	{
		return m_followedBy;
	}
	void GetMinMaxDuration(float &fMin, float &fMax) const override
	{
		fMin = m_fMinDuration;
		fMax = m_fMaxDuration;
	}

	// IWeatherSystemListener
	virtual void OnSituationAboutToChange(float fTimeLeft) override;
	virtual void OnSituationChanging(string sNextSituation) override;

	//=============================================================================
	// CWeatherSituation
	//=============================================================================

	/**
	* Gets a vector with all names of situations which may appear after this one
	* @returns vector with names */
	const std::vector<string> &GetFollowedByCollection() const
	{
		return m_followedBy;
	}
private:

	//==================================================================
	// UTILITY
	//==================================================================
	/**
	* Converts a string in the form of "14:56" to ist corresponding float time (14.93) */
	float ConvertStringToFloatTime(string sStringToConvert);

	/**
	* Converts a string to an EPositioningMode mode
	* Will return ePM_AroundWorld if there is no appropriate string given */
	EPositioningMode GetPositioningMode(const char *sString);


	//==================================================================
	// PARSING
	//==================================================================


	/**
	* Parses the "occursAfter attribute of this weather situation
	* Separates the entries and fills m_followedBy with them */
	void ParseFollowedByAttribute(string sFollowedBy);
	/**
	* Parses the <Particles> tag and add each particle to m_particleCollection */
	void ParseParticles(XmlNodeRef particleRoot);
	/**
	* Parses the <Sounds> tag and ads each sound to m_soundCollection */
	void ParseSounds(XmlNodeRef soundRoot);
	/**
	* Parses sky node */
	void ParseSky(XmlNodeRef skyRoot);
	/**
	* Parses <TimeOfDay> node  */
	void ParseTimeOfDay(XmlNodeRef todRoot);
	/**
	* Parses the <Clouds> node  */
	void ParseClouds(XmlNodeRef cloudsRoot);
	/**
	* Parses <Entities> node */
	void ParseEntities(XmlNodeRef entRoot);

	//=============================================================================
	// Updating
	//=============================================================================

	/**
	* Updates the global wind in game */
	void UpdateWind(float fFrameTime);
	/**
	* Updates rain (if defined) */
	void UpdateRain(float fFrameTime);
	/**
	* Updates snow (if defined)  */
	void UpdateSnow(float fFrameTime);
	/**
	* Updates clouds (they're faded in)  */
	void UpdateClouds(float fFrameTime);
	/**
	* Updates sounds  */
	void UpdateSounds(float fFrameTime);
	/**
	* Updates particles  */
	void UpdateParticle(float fFrameTime);
	/**
	* Updates entities  */
	void UpdateEntity(float fFrameTime);


	/** name of this situation */
	string  m_sName;
	/** Can this situation be used in the weather cycle? */
	bool    m_bIsUsedInCycle;
	/** earliest time of day this situation can occur */
	float   m_fTimeFrom;
	/** time of day up to which this situation may occur */
	float   m_fTimeTo;
	/** Does this situation affect wind? */
	bool    m_bAffectsGlobalWind;
	/** does this situation have rain? */
	bool    m_bHasRain;
	/** rain fully present */
	bool    m_bRaining;
	/** snow params present? */
	bool    m_bHasSnow;
	/** snow fully faded in? */
	bool    m_bSnowFadeFinished;
	/** Is this situation currently active? */
	bool    m_bIsActive;
	/** Time until this situation get replaced by another */
	float   m_fTimeRemaining;
	/** Minimum time this situations has to last in secs */
	float m_fMinDuration;
	/** Maximum time this situation may last in sec */
	float m_fMaxDuration;
	/** Array of situations this one may appear after (to prevent illogical weather changes) */
	std::vector<string> m_followedBy;
	/** Wind properties for this situation */
	SSituationWindStats m_windStats;
	/** Paticles to be spawned during this situaion */
	std::vector<SSituationParticle *> m_particleCollection;
	/** Sounds to be player during this situation */
	std::vector<SSituationSound *> m_soundCollection;
	/** rain parameters */
	SSituationRain m_rainParams;
	/** Snow params */
	SNOWPARAMS m_snowParams;
	/** Time of day handling */
	CTimeOfDayFader m_todFader;
	/** Simple clouds to be spawned during a situation */
	std::vector<SSituationSimpleCloud *> m_simpleClouds;
	/** Entities */
	std::vector<SSituationEntity *> m_entityCollection;
};

#endif // !__WEATHER_SITUATION_H__
