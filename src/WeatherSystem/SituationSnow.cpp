#include "StdAfx.h"
#include "SituationSnow.h"
#include "IGameFramework.h"
#include "Actor.h"


SSituationSnow::SSituationSnow()
{
	Init();
}

SSituationSnow::~SSituationSnow()
{
}

void SSituationSnow::Init()
{
	fFadeInTime = 0;
	fFadeProgression = 0;
	positioningMode = EPositioningMode::ePM_AroundPlayer;

	snowFallParams.fSnowFallBrightness = 1;
	snowFallParams.fSnowFallGravity = 1;
	snowFallParams.fSnowFallGravityScale = 1;
	snowFallParams.fSnowFallTurbulence = 1;
	snowFallParams.fSnowFallTurbulenceFreq = 1;
	snowFallParams.fSnowFallWindScale = 1;
	snowFallParams.fSnowFlakeSize = 1;
	snowFallParams.snowFlakeCount = 1;


	surfaceParams.center = Vec3(0, 0, 0);
	surfaceParams.fFrostAmount = 1;
	surfaceParams.fRadius = 5;
	surfaceParams.fSnowAmount = 1;
	surfaceParams.fSurfaceFreezing = 1;
}

void SSituationSnow::ReadCurrentSnowParams()
{
	SurfaceParams &suPam = oldSnowParams.surfaceParams;
	SnowFallParams &faPam = oldSnowParams.snowFallParams;
	gEnv->p3DEngine->GetSnowSurfaceParams(suPam.center, suPam.fRadius, suPam.fSnowAmount, suPam.fFrostAmount, suPam.fSurfaceFreezing);
	gEnv->p3DEngine->GetSnowFallParams(faPam.snowFlakeCount, faPam.fSnowFlakeSize, faPam.fSnowFallBrightness, faPam.fSnowFallGravityScale,
									   faPam.fSnowFallWindScale, faPam.fSnowFallTurbulence, faPam.fSnowFallTurbulenceFreq);
}

SSituationSnow::ELoadingResult SSituationSnow::LoadFromXml(XmlNodeRef root)
{
	if (!root)
	{
		return ELoadingResult::eLFR_Nullptr;
	}

	string modName = "Snow";

	XMLGET(root, "fadeInTime", fFadeInTime, modName);

	XmlString posMode;
	XMLGET(root, "positioningMode", posMode, modName);
	positioningMode = GetPositioningModeFromString(posMode.c_str());

	if (XmlNodeRef surface = root->findChild("Surface"))
	{
		modName = "Snow Surface";

		XmlString sPos;
		XMLGET(surface, "position", sPos, modName);
		surfaceParams.center = ParsePositionAttribute(sPos);

		XMLGET(surface, "radius", surfaceParams.fRadius, modName);
		XMLGET(surface, "snowAmount", surfaceParams.fSnowAmount, modName);
		XMLGET(surface, "frostAmount", surfaceParams.fFrostAmount, modName);
		XMLGET(surface, "surfaceFreezing", surfaceParams.fSurfaceFreezing, modName);
	}
	else
	{
		return ELoadingResult::eLFR_NoSurface;
	}

	if (XmlNodeRef fall = root->findChild("SnowFall"))
	{
		modName = "Snow Fall";
		XMLGET(fall, "snowFlakeCount", snowFallParams.snowFlakeCount, modName);
		XMLGET(fall, "snowFlakeSize", snowFallParams.fSnowFlakeSize, modName);
		XMLGET(fall, "snowFallBrightness", snowFallParams.fSnowFallBrightness, modName);
		XMLGET(fall, "snowFallGravity", snowFallParams.fSnowFallGravity, modName);
		XMLGET(fall, "snowFallGravityScale", snowFallParams.fSnowFallGravityScale, modName);
		XMLGET(fall, "snowFallWindScale", snowFallParams.fSnowFallWindScale, modName);
		XMLGET(fall, "snowFallTurbulence", snowFallParams.fSnowFallTurbulence, modName);
		XMLGET(fall, "snowFallTurbulenceFreq", snowFallParams.fSnowFallTurbulenceFreq, modName);
	}
	else
	{
		return ELoadingResult::eLFR_NoFall;
	}


	return ELoadingResult::eLFR_Succes;
}

void SSituationSnow::Update(float fFrameTime)
{
	float fFlakeSize = snowFallParams.fSnowFlakeSize;
	float fSnowAmount = surfaceParams.fSnowAmount;
	float fFrostAmount = surfaceParams.fFrostAmount;
	float fSurfaceFreezing = surfaceParams.fSurfaceFreezing;

	if (fFadeProgression < fFadeInTime)
	{
		// Lerp:
		// snoflakesCount
		// snowamoutnt

		float currentPercentage = fFadeProgression / fFadeInTime;

		fFlakeSize = LERP(oldSnowParams.snowFallParams.fSnowFlakeSize,
				snowFallParams.fSnowFlakeSize,
				currentPercentage);
		fSnowAmount = LERP(oldSnowParams.surfaceParams.fSnowAmount,
												surfaceParams.fSnowAmount,
												currentPercentage);

		fFrostAmount = LERP(oldSnowParams.surfaceParams.fFrostAmount,
												surfaceParams.fFrostAmount,
												currentPercentage);

		fSurfaceFreezing = LERP(oldSnowParams.surfaceParams.fSurfaceFreezing,
												surfaceParams.fSurfaceFreezing,
												currentPercentage);

		fFadeProgression += fFrameTime;
	}

	Vec3 vNewCenter = surfaceParams.center;

	switch (positioningMode)
	{
	case (EPositioningMode::ePM_AroundPlayer):
	{
		vNewCenter = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos();
	}
	break;
	case (EPositioningMode::ePM_Local):
	{
		vNewCenter = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos() + surfaceParams.center;
	}
	break;
	default:
		break;
	}

	gEnv->p3DEngine->SetSnowFallParams(snowFallParams.snowFlakeCount, fFlakeSize,
										snowFallParams.fSnowFallBrightness, snowFallParams.fSnowFallGravityScale,
										snowFallParams.fSnowFallWindScale, snowFallParams.fSnowFallTurbulence,
										snowFallParams.fSnowFallTurbulenceFreq);

	gEnv->p3DEngine->SetSnowSurfaceParams(vNewCenter, surfaceParams.fRadius,
											fSnowAmount,
											fFrostAmount, fSurfaceFreezing);
}

void SSituationSnow::Reset()
{
	fFadeProgression = 0.f;
}