#ifndef __SITUATION_ENTITY_H__
#define __SITUATION_ENTITY_H__

#include "AttributeEnums.h"
#include "Cry_Math.h"
#include "IXml.h"
#include "IScriptSystem.h"

struct SEntityParamPair
{


	XmlString p_sName;
	ScriptAnyValue p_val;
	string  p_sStringVal;
};

struct SSituationEntity
{
	SSituationEntity();
	~SSituationEntity();

	void LoadFromXml(XmlNodeRef root);

	/**
	* Inititalizes timers  */
	void SpawnEntity();

	void Update(float fFrameTime);
	void Reset();

	//=============================================================================
	// Public attributes
	//=============================================================================

	/** Entity class name */
	XmlString p_sEntityClass;
	/** Position of entity */
	Vec3 p_vPos;
	/** Direction of entity */
	Vec3 p_vDir;
	/** Scale factor */
	float p_fScaleFactor;
	/** Positioning mode for entity */
	EPositioningMode p_positioningMode;
	/** Only spawn this entity once per situation */
	bool p_bSpawnOnce;
	/** Remove the entity instead of hiding it when situation becomes inactive */
	bool p_bRemoveOnHide;
	/** Spawn entity after situation has been active for this long */
	float p_fSpawnAfterSecs;
	/** Remove entity when this much seconds have passed after it has been spawned */
	float p_fRemoveAfterSecs;
	/** Min delay after which entity is repositioned */
	float p_fMinSpawnDelay;
	/** Max delay after which entity is repositioned */
	float p_fMaxSpawnDelay;
	/** Radius around player where entity is spawned */
	float p_fRadius;
	/** Min height relative to player  */
	float p_fMinHeight;
	/** Max height relative to player */
	float p_fMaxHeight;

private:

	/**
	* Converts a string to an ScriptValueType */
	ScriptAnyType GetTypeFromString(XmlString sType);

	/**
	* Creates the entity using m_spawnParams and sets custom values */
	void CreateEntity();

	/** Spawning timer */
	float m_fSpawnTimer;
	/** Time until spawn */
	float m_fTimeTilSpawn;
	/** Total time this entity has been active */
	float m_fTotalActiveTime;
	/** Has this entity been spawned at least once */
	bool m_bHasBeenSpawned;
	/** Has this entity been removed already */
	bool m_bHasBeenRemoved;
	/** Custom values */
	std::vector<SEntityParamPair *> m_values;
	/** Spawn params for this entity */
	SEntitySpawnParams m_spawnParams;
	/** The spawned entity */
	IEntity *m_pEntity;


};

#endif