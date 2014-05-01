#include "StdAfx.h"
#include "SituationSound.h"
#include "IWeatherSituation.h"
#include "IEntity.h"
#include "IEntitySystem.h"
#include "IGame.h"
#include "IGameFramework.h"
#include "Actor.h"

void SSituationSound::LoadFromXml(XmlNodeRef root)
{
	if (!root)
	{
		return;
	}

	string modName = "Sound";
	XmlString sTempString;
	XMLGET(root, "path", sTempString, modName);
	x_sPath = sTempString.empty() ? "Invalid" : sTempString;

	XMLGET(root, "spawnAfterSecs", x_fSpawnAfterSeconds, modName);
	XMLGET(root, "spawnOnce", x_bSpawnOnce, modName);


	XMLGET(root, "position", sTempString, modName);
	x_vPosition = ParsePositionAttribute(sTempString);

	XMLGET(root, "positioningMode", sTempString, modName);
	x_positioningMode = GetPositioningModeFromString(sTempString.c_str());

	XMLGET(root, "radius", x_fRadius, modName);
	XMLGET(root, "volume", x_fVolume, modName);

	XMLGET(root, "fadeInTime", x_fFadeInTime, modName);
	XMLGET(root, "loop", x_bLoop, modName);
	XMLGET(root, "minSpawnDelay", x_fMinSpawnDelay, modName);
	XMLGET(root, "maxSpawnDelay", x_fMaxSpawnDelay, modName);
}

void SSituationSound::Update(float fFrameTime)
{
	if (!m_pSound || !m_pSoundProxy)
	{
		return;
	}

	if (m_bHasBeenPlayed && x_bSpawnOnce)
	{
		return;
	}

	if (m_fTime >= m_fSpawnTime)
	{
		if (!m_pSound->IsPlaying())
		{
			m_fFadeProgression = 0.f;
			Vec3 finalPos = x_vPosition;
			switch (x_positioningMode)
			{
			case ePM_AroundPlayer:
			{
				Vec3 v;
				v.SetRandomDirection();
				finalPos = v * x_fRadius;
			}
			break;
			case ePM_AroundWorld:
			{
				int maxDim = gEnv->p3DEngine->GetTerrainSize();
				Vec3 v;
				v.SetRandomDirection();
				finalPos = v * maxDim;
			}
			break;
			default:
				break;
			}

			m_pEntity->SetPos(gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos() + finalPos);
			tSoundID id = m_pSoundProxy->PlaySound(x_sPath, finalPos, Vec3(0, 1, 0), m_nSoundFlags, 0, ESoundSemantic::eSoundSemantic_SoundSpot);
			m_pSound = m_pSoundProxy->GetSound(id);
			if (m_pSound)
				m_pSound->SetPaused(false);
			m_bHasBeenPlayed = true;

			if (!x_bSpawnOnce)
			{
				m_fSpawnTime = x_fMinSpawnDelay + (cry_frand() * (x_fMaxSpawnDelay - x_fMinSpawnDelay));
				m_fTime = 0.f;
			}
		}



		// Appearantly the sound may become invalid when shutting down engine
		if (m_pSound)
		{
			if (!m_bFadeOut)
			{
				if (m_fFadeProgression < x_fFadeInTime)
				{
					float fPercentage = m_fFadeProgression / x_fFadeInTime;
					m_pSound->GetInterfaceExtended()->SetVolume(x_fVolume * fPercentage);
				}
				else
				{
					m_pSound->GetInterfaceExtended()->SetVolume(x_fVolume);
				}
				m_fFadeProgression += fFrameTime;
			}
			else
			{
				if (m_fFadeProgression < x_fFadeInTime)
				{
					float fPercentage = m_fFadeProgression / m_fFadeOutTime;
					m_pSound->GetInterfaceExtended()->SetVolume(x_fVolume * fPercentage);
				}
				else
				{
					m_pSound->GetInterfaceExtended()->SetVolume(0.f);
				}

				m_fFadeProgression -= fFrameTime;
			}
		}


	}


	m_fTime += fFrameTime;

}

void SSituationSound::Reset()
{
	m_fTime = 0.f;
	if (x_fSpawnAfterSeconds >= 0)
	{
		m_fSpawnTime = x_fSpawnAfterSeconds;
	}
	else
	{
		m_fSpawnTime = 0.f;
	}

	m_bHasBeenPlayed = false;

	m_bFadeOut = false;
	m_fFadeOutTime = 0.f;
	m_fFadeProgression = 0.f;
	
	if (!gEnv->IsEditing() && m_pSound)
	{
		m_pSound->SetPaused(true);
		m_pSound->Stop();
	}

	if (gEnv->IsEditor())
	{
		m_pEntity = nullptr;
		m_pSound = nullptr;
		m_pSoundProxy = nullptr;
	}
}

void SSituationSound::SpawnSound()
{
	int &soundFlags = m_nSoundFlags;

	soundFlags |= FLAG_SOUND_3D | FLAG_SOUND_START_PAUSED;

	if (x_bLoop)
	{
		soundFlags |= FLAG_SOUND_LOOP;
	}

	switch (x_positioningMode)
	{
	case ePM_World:
		break;
	case ePM_Local:
	{
		soundFlags |= FLAG_SOUND_RELATIVE;
	}
	break;
	case ePM_AroundPlayer:
		break;
	case ePM_AroundWorld:
		break;
	default:
		break;


	}

	if (x_fSpawnAfterSeconds >= 0)
	{
		m_fSpawnTime = x_fSpawnAfterSeconds;
	}
	else
	{
		m_fSpawnTime = 0.f;
	}


	m_pSound = gEnv->pSoundSystem->CreateSound(x_sPath, soundFlags);

	SEntitySpawnParams params;
	params.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("BasicEntity");
	params.vPosition = Vec3(0);

	m_pEntity = gEnv->pEntitySystem->SpawnEntity(params);

	if (m_pEntity)
	{
		m_pEntity->Invisible(true);
	}

	if (m_pEntity)
	{
		m_pSoundProxy = (IEntitySoundProxy *)m_pEntity->CreateProxy(EEntityProxy::ENTITY_PROXY_SOUND);
	}


}

void SSituationSound::FadeOut(float fTimeLeft)
{
	m_bFadeOut = true;
	m_fFadeOutTime = fTimeLeft;
	m_fFadeProgression = fTimeLeft;
}
//=============================================================================
// Private
//=============================================================================


