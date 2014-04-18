#include "StdAfx.h"
#include "SituationParticle.h"
#include "IEntity.h"
#include "IEntitySystem.h"
#include "IParticles.h"
#include "IGame.h"
#include "IGameFramework.h"
#include "Actor.h"


struct SpawnParams;

void SSituationParticle::LoadFromXml(XmlNodeRef root)
{
	string modName = "Particle";

	XmlString sTempString;
	XMLGET(root, "path", sTempString, modName);
	x_sPath = sTempString.empty() ? "" : sTempString;

	XMLGET(root, "spawnAfterSecs", x_fSpawnAferSeconds, modName);
	XMLGET(root, "spawnOnce", x_bSpawnOnce, modName);

	XMLGET(root, "position", sTempString, modName);
	x_vPosition = ParsePositionAttribute(sTempString);

	XMLGET(root, "positioningMode", sTempString, modName);
	x_positioningMode = GetPositioningModeFromString(sTempString.c_str());

	XMLGET(root, "radius", x_fRadius, modName);
	XMLGET(root, "minSpawnDelay", x_fMinSpawnDelay, modName);
	XMLGET(root, "maxSpawnDelay", x_fMaxSpawnDelay, modName);
	XMLGET(root, "scaleFactor", x_fScaleFactor, modName);
	XMLGET(root, "direction", x_vDir, modName);
	XMLGET(root, "minHeight", x_fMinHeight, modName);
	XMLGET(root, "maxHeight", x_fMaxHeight, modName);
}

void SSituationParticle::SpawnParticle()
{


	m_pParticleEffect = gEnv->p3DEngine->GetParticleManager()->FindEffect(x_sPath);

	if (x_fSpawnAferSeconds == -1)
	{
		m_fSpawnAfterSecs = Random(x_fMinSpawnDelay, x_fMaxSpawnDelay);
	}
	else
	{
		m_fSpawnAfterSecs = x_fSpawnAferSeconds;
	}
}

void SSituationParticle::Update(float fFrameTime)
{
	if (m_bHasBeenSpawned && x_bSpawnOnce)
	{
		return;
	}

	if (m_fSpawnTimer >= m_fSpawnAfterSecs && m_pParticleEffect)
	{
		Vec3 pos = x_vPosition;
		switch (x_positioningMode)
		{
		case ePM_World:
			break;
		case ePM_Local:
			pos = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos() + pos;
			break;
		case ePM_AroundPlayer:
		{
			Vec3 v;
			v.SetRandomDirection();
			pos = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos() + v * x_fRadius;
		}
		break;
		case ePM_AroundWorld:
		{
			Vec3 v;
			v.SetRandomDirection();
			int terrainSize = gEnv->p3DEngine->GetTerrainSize();
			pos = v * terrainSize;
		}
		break;
		default:
			break;
		}

		float fHeight = Random(x_fMinHeight, x_fMaxHeight);
		pos.z = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos().z + fHeight;

		SpawnParams params;
		PivotLocation loc = m_pParticleEffect->ParticleLoc(pos, x_vDir, x_fScaleFactor);

		m_pEmitter = m_pParticleEffect->Spawn(loc, ePEF_Independent, &params);


		m_bHasBeenSpawned = true;


		if (!x_bSpawnOnce)
		{
			m_fSpawnTimer = 0.f;
			m_fSpawnAfterSecs = Random(x_fMinSpawnDelay, x_fMaxSpawnDelay);
		}
	}

	m_fSpawnTimer += fFrameTime;
}

void SSituationParticle::Reset()
{
	m_bHasBeenSpawned = false;
	m_fSpawnAfterSecs = 0.f;

	if (m_pEmitter)
	{
		m_pEmitter->ReleaseNode();
		m_pEmitter = nullptr;
	}
}

//=============================================================================
// Private
//=============================================================================