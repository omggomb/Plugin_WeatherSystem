#include "StdAfx.h"
#include "SituationWind.h"
#include "AttributeEnums.h"

void SSituationWindStats::LoadFromXml(XmlNodeRef root)
{
	if (!root)
	{
		return;
	}

	string modName = "Wind";

	XMLGET(root, "directionAndSpeed", x_vDirectionAndSpeed, modName);
	XMLGET(root, "fadeInTime", x_fFadeInTime, modName);
	XMLGET(root, "maxLowerFluctuation", x_fMaxLowerFluctuation, modName);
	XMLGET(root, "maxHigherFluctuation", x_fMaxHigherFluctuation, modName);
	XMLGET(root, "minFluctuationTime", x_fMinTurbulenceDuration, modName);
	XMLGET(root, "maxFluctuationTime", x_fMaxTurbulenceDuration, modName);
}