#ifndef __SITUATION_FAKE_SNOW_H__
#define __SITUATION_FAKE_SNOW_H__

#include "SituationSnow.h"

/**
* Dummy struct for Free SDK ver 3.5.x 
* since there is no snow */
struct SSituationFakeSnow
{
	void Init() {}

	void ReadCurrentSnowParams() {}

	SSituationSnow::ELoadingResult LoadFromXml(XmlNodeRef snowRoot) {return SSituationSnow::eLFR_Succes;}

	void Update(float fFrameTime) {}

	void Reset() {}

	float fFadeInTime;
	float fFadeProgression;
	EPositioningMode positioningMode;

	SurfaceParams surfaceParams;
	SnowFallParams snowFallParams;


	/** Snow params before the current situation was applied */
	SSituationOldSnow oldSnowParams;


};

#endif // !__SITUATION_FAKE_SNOW_H__