#ifndef __SITUATION_ATTRIBUTES_H__
#define __SITUATION_ATTRIBUTES_H__

#include "Cry_Math.h"
#include "CDKVersion.h"

#include "AttributeEnums.h"
#include "SituationSound.h"
#include "SituationRain.h"
#include "SituationParticle.h"
#include "SituationEntity.h"
#include "SituationWind.h"
#include "SituationSimpleCloud.h"

#if CDK_VERSION > 345 || CDK_VERSION < 344
#undef SDK_HAS_SNOW
#else
#define SDK_HAS_SNOW
#endif


#ifndef SDK_HAS_SNOW
#include "SituationFakeSnow.h"
#define SNOWPARAMS SSituationFakeSnow
#else
#include "SituationSnow.h"
#define SNOWPARAMS SSituationSnow
#endif




#endif // !__SITUATION_ATTRIBUTES_H__
