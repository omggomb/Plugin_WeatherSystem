#ifndef __SITUATION_SOUND_H__
#define __SITUATION_SOUND_H__

#include "AttributeEnums.h"
#include "Cry_Math.h"

/**
* Represents a sound that can be played during a wather situation
*/
struct SSituationSound
{
	SSituationSound() : x_sPath(""),
		x_fSpawnAfterSeconds(0.f),
		x_bSpawnOnce(false),
		x_vPosition(Vec3(0)),
		x_positioningMode(EPositioningMode::ePM_AroundPlayer),
		x_fRadius(1.f),
		x_fVolume(1.f),
		x_fFadeInTime(1.f),
		x_bLoop(false),
		x_fMinSpawnDelay(0.f),
		x_fMaxSpawnDelay(1.f),
/** Private */
		m_bHasBeenPlayed(false),
		m_fFadeProgression(0.f),
		m_fTime(0.f),
		m_fSpawnTime(0.f),
		m_nSoundFlags(0),
		m_bFadeOut(false),
		m_fFadeOutTime(1.f),
		m_pEntity(nullptr),
		m_pSoundProxy(nullptr),
		m_pSound(nullptr)
	{
	}

	void LoadFromXml(XmlNodeRef root);

	/**
	* Initiates this sound
	* Update still needs to be called every frame */
	void SpawnSound();

	void Update(float fFrameTime);

	void Reset();

	void FadeOut(float fTimeLeft);

	//=============================================================================
	// Public Attributes
	//=============================================================================

	/** path to the sound inside a sound library */
	string x_sPath;
	/** spawndelay after wather situation has started, -1 means random */
	float x_fSpawnAfterSeconds;
	/** only spawn this sound once during the weather situaion */
	bool x_bSpawnOnce;
	/** position, see EPositioningMode for details */
	Vec3 x_vPosition;
	/** determines how position is interpreted */
	EPositioningMode x_positioningMode;
	/** only used when positioningMode is set to ePM_AroundPlayer */
	float x_fRadius;
	/** may this sound be queued to be played randomly with others? */
	bool x_bCanBeCycled;
	/** Volume of the sound, -1 means random but can never be 0 */
	float x_fVolume;
	/** Fade in time until sound reaches full volume */
	float x_fFadeInTime;
	/** Should this sound be looped?*/
	bool x_bLoop;
	/** Minimum delay after sound has stoppen until it's played again */
	float x_fMinSpawnDelay;
	/** Max delay */
	float x_fMaxSpawnDelay;


private:


	/** Has this sound been player at least once already? */
	bool m_bHasBeenPlayed;
	/** Fade progression */
	float m_fFadeProgression;
	/** Timer */
	float m_fTime;
	/** Disgnated spawn time */
	float m_fSpawnTime;
	/** Sound flags set with SpawnSound */
	int m_nSoundFlags;
	/** Determines wether sound is faded out or in */
	bool m_bFadeOut;
	/** Fadeout time */
	float m_fFadeOutTime;
	/** Entity container for this sound */
	IEntity *m_pEntity;
	/** The sound proxy of the container entity */
	IEntitySoundProxy *m_pSoundProxy;
	/** The sound interface created by SpawnSound */
	ISound *m_pSound;
};

#endif // !__SITUATION_SOUND_H__

