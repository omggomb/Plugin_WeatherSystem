#ifndef __SITUATION_WIND_H__
#define __SITUATION_WIND_H__

#include "Cry_Math.h"
#include "IXml.h"

/**
* Wind stats specific to a single weather situation */
struct SSituationWindStats
{
	SSituationWindStats() : x_vDirectionAndSpeed(Vec3(1, 0, 0)),
		x_fFadeInTime(1.f),
		x_fMaxLowerFluctuation(1.f),
		x_fMaxHigherFluctuation(1.f),
		x_fFadeProgression(0.f),
		x_vOldWindVector(Vec3(1, 0, 0)),
		x_fTurbulenceDuration(0.f),
		x_vTurbulenceSpeed(Vec3(0)),
		x_bTurbulenceGoing(false),
		x_fCurrentTurbulenceTime(0.f)

	{
	}

	/**
	* Loads wind stats from xml node  */
	void LoadFromXml(XmlNodeRef root);

	//=============================================================================
	// Public attributes
	//=============================================================================

	/** Direction and speed of the global wind, higher numbers = higher speed */
	Vec3 x_vDirectionAndSpeed;
	/** How long will it take for the wind to reach full speed? (seconds) */
	float x_fFadeInTime;
	/** Random speed decreases by this amount */
	float x_fMaxLowerFluctuation;
	/** Random speed increases by this amount */
	float x_fMaxHigherFluctuation;
	/** Min turbulence duration */
	float x_fMinTurbulenceDuration;
	/** Max turbulence duration */
	float x_fMaxTurbulenceDuration;


	/** INTERNAL Fade progression */
	float x_fFadeProgression;
	/** INTERNAL Old wind vector used for interpolation */
	Vec3 x_vOldWindVector;
	/** INTERNAL time turbulence has been active */
	float x_fTurbulenceDuration;
	/** INTERNAL random wind speed delta */
	Vec3 x_vTurbulenceSpeed;
	/** INTERNAL is a turbulence on going */
	bool x_bTurbulenceGoing;
	/** INTERNAL random duration set for the current turbulence */
	float x_fCurrentTurbulenceTime;
};


#endif // !__SITUATION_WIND_H__