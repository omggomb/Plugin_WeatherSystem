#ifndef __SITUATION_RAIN_H__
#define __SITUATION_RAIN_H__

#include "Cry_Math.h"
#include "AttributeEnums.h"

/**
* Dedicated rain structure since the engine has built in rain functionality */
struct SSituationOldRain
{
	/** targte amount of rain */
	float fAmount;
	/** time until full amount of rain will fall */
	float fFadeInTime;
	float fFakeGlossiness;
	float fPuddlesAmount;
	float fRadius;
	float fDropSpeed;
	float fReflectionAmount;
	float fUmbrellaRadius;

	Vec3 color;
	Vec3 center;

	SSituationOldRain() {}
	~SSituationOldRain() {}
};

/**
* Dedicated rain structure since the engine has built in rain functionality */
struct SSituationRain
{
	SSituationRain();
	~SSituationRain();

	/**
	* Loads rain params from xml node  */
	void LoadFromXml(XmlNodeRef root);

	/**
	* Gets the current rain parameters from the engine and stores them
	* inside m_pOldRain */
	void ReadCurrentRainParams();

	//=============================================================================
	// Public attributes
	//=============================================================================

	/** targte amount of rain */
	float x_fAmount;
	/** time until full amount of rain will fall */
	float x_fFadeInTime;
	float x_fFakeGlossiness;
	float x_fPuddlesAmount;
	float x_fRadius;
	float x_fDropSpeed;
	float x_fReflectionAmount;
	float x_fUmbrellaRadius;
	Vec3 x_vColor;
	Vec3 x_vCenter;
	EPositioningMode x_positioningMode;

	/** INTERNAL fade progress */
	float x_fFadeProgression;
	/** INTERNAL old rain amount */
	SSituationOldRain x_pOldRain;
	/** INTERNAL fading rain amount */
	float x_fFadingAmount;
};



#endif // !__SITUATION_RAIN_H__