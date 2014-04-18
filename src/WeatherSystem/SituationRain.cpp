#include "StdAfx.h"
#include "SituationRain.h"


SSituationRain::SSituationRain() : x_fAmount(1), x_fFadeInTime(10), x_fFakeGlossiness(1),
	x_fPuddlesAmount(3), x_fRadius(50), x_fDropSpeed(1),
	x_fReflectionAmount(1), x_fUmbrellaRadius(0), x_fFadeProgression(0),
	x_vColor(Vec3(255, 255, 255)), x_vCenter(Vec3(0, 0, 0)), x_fFadingAmount(0)
{
}

SSituationRain::~SSituationRain()
{
}

void SSituationRain::LoadFromXml(XmlNodeRef root)
{
	if (!root)
	{
		return;
	}
	string modName = "Rain";
	XmlString sTempString;
	XMLGET(root, "amount", x_fAmount, modName);
	XMLGET(root, "fadeInTime", x_fFadeInTime, modName);

	XMLGET(root, "position", sTempString, modName);
	x_vCenter = ParsePositionAttribute(sTempString);

	XMLGET(root, "color", x_vColor, modName);
	XMLGET(root, "fakeGlossiness", x_fFakeGlossiness, modName);
	XMLGET(root, "puddlesAmount", x_fPuddlesAmount, modName);
	XMLGET(root, "radius", x_fRadius, modName);
	XMLGET(root, "dropSpeed", x_fDropSpeed, modName);
	XMLGET(root, "reflectionAmount", x_fReflectionAmount, modName);
	XMLGET(root, "umbrellaRadius", x_fUmbrellaRadius, modName);

	XMLGET(root, "positioningMode", sTempString, modName);
	x_positioningMode = GetPositioningModeFromString(sTempString.c_str());

}

void SSituationRain::ReadCurrentRainParams()
{

	bool dummyBool;
	gEnv->p3DEngine->GetRainParams(x_pOldRain.center, x_pOldRain.fRadius, x_pOldRain.fAmount, x_pOldRain.color);
	gEnv->p3DEngine->GetRainParams(x_pOldRain.fReflectionAmount, x_pOldRain.fFakeGlossiness, x_pOldRain.fPuddlesAmount,
								   dummyBool, x_pOldRain.fDropSpeed, x_pOldRain.fUmbrellaRadius);

	x_fFadingAmount = x_pOldRain.fAmount;

}