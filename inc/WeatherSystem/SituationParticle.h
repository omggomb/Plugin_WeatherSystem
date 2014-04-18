#ifndef __SITUATION_PARTICLE_H__
#define __SITUATION_PARTICLE_H__

#include "Cry_Math.h"
#include "AttributeEnums.h"

/**
* Represents a particle effect spawnable during a weather situation
*/
struct SSituationParticle
{
	SSituationParticle() :  x_sPath(""),
		x_fSpawnAferSeconds(1.f),
		x_bSpawnOnce(true),
		x_vPosition(Vec3(0)),
		x_positioningMode(EPositioningMode::ePM_AroundPlayer),
		x_fRadius(1.f),
		x_fMinSpawnDelay(0.f),
		x_fMaxSpawnDelay(1.f),
		x_fScaleFactor(1.f),
		x_vDir(Vec3(1, 0, 0)),
		x_fMinHeight(1.f),
		x_fMaxHeight(1.f),
/** Private */
		m_fSpawnTimer(0.f),
		m_fSpawnAfterSecs(0.f),
		m_bHasBeenSpawned(false),
		m_pParticleEffect(nullptr),
		m_pEmitter(nullptr)
	{
	}

	/**
	* Load attributes from xml node  */
	void LoadFromXml(XmlNodeRef root);

	/**
	* Initializes this particle effect  */
	void SpawnParticle();

	/**
	* Update this particle  */
	void Update(float fFrameTime);

	/**
	* Reset this particle effect for later spawnage  */
	void Reset();

	//=============================================================================
	// Public Attributes
	//=============================================================================


	/** path to the particle inside a library */
	string x_sPath;
	/** spawn delay for first spawn, -1 means random */
	float x_fSpawnAferSeconds;
	/** Only spawn this particle once during the current wather situation */
	bool x_bSpawnOnce;
	/** optional position, see EPositioningMode */
	Vec3 x_vPosition;
	/** determines how position, radius and keepAroundPlayer are interpreted */
	EPositioningMode x_positioningMode;
	/** only used when positioningMode is set to ePM_AroundPlayer */
	float x_fRadius;
	/** Minimum delay between to spawned particle effects */
	float x_fMinSpawnDelay;
	/** Max delay between two spawned particles */
	float x_fMaxSpawnDelay;
	/** Scale factor */
	float x_fScaleFactor;
	/** Direction of particle emitter */
	Vec3 x_vDir;
	/** Minimum height for the particle */
	float x_fMinHeight;
	/** Maximum height for the particle */
	float x_fMaxHeight;


private:

	/** Spawn timer */
	float m_fSpawnTimer;
	/** Time that must pass until particle is spawned again */
	float m_fSpawnAfterSecs;
	/** Has this particle been spawned at least once? */
	bool m_bHasBeenSpawned;
	/** Particle interface */
	IParticleEffect *m_pParticleEffect;
	/** Spawned Emitter */
	IParticleEmitter *m_pEmitter;
};
#endif // !__SITUATION_PARTICLE_H__

