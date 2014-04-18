#ifndef __SITUATION_SNOW_H__
#define __SITUATION_SNOW_H__

#include "Cry_Math.h"
#include "AttributeEnums.h"
#include "ISystem.h"

struct SurfaceParams
{
	Vec3 center;
	float fRadius, fSnowAmount, fFrostAmount, fSurfaceFreezing;
};

struct SnowFallParams
{
	int snowFlakeCount;
	float fSnowFlakeSize, fSnowFallBrightness, fSnowFallGravity, fSnowFallGravityScale,
		  fSnowFallWindScale, fSnowFallTurbulence, fSnowFallTurbulenceFreq;
};

struct SSituationOldSnow
{
	SSituationOldSnow() {}
	~SSituationOldSnow() {}

	SurfaceParams surfaceParams;
	SnowFallParams snowFallParams;
};
/**
* Snow parameters  */
struct SSituationSnow
{


	/**
	* Returned by LoadFrommXml  */
	enum ELoadingResult
	{
		/** <Surface> tag is missing */
		eLFR_NoSurface,
		/** <SnowFall> tag is missing */
		eLFR_NoFall,
		/** fadeInTime is missing */
		eLFR_NoSnow,
		/** root was null */
		eLFR_Nullptr,
		/** Everything went fine */
		eLFR_Succes
	};

	SSituationSnow();
	~SSituationSnow();

	/**
	* Initializes values with reasonable defaults  */
	void Init();

	/**
	* Reads current snow params and stores them inside oldSnowParams  */
	void ReadCurrentSnowParams();

	/**
	* Loads params from given root <Snow>
	* Returns true on success */
	ELoadingResult LoadFromXml(XmlNodeRef snowRoot);

	void Update(float fFrameTime);

	void Reset();


	float fFadeInTime;
	float fFadeProgression;
	EPositioningMode positioningMode;

	SurfaceParams surfaceParams;
	SnowFallParams snowFallParams;


	/** Snow params before the current situation was applied */
	SSituationOldSnow oldSnowParams;


};


#endif // !__SITUATION_SNOW_H__

