#ifndef __SITUATION_SIMPLE_CLOUD_H__
#define __SITUATION_SIMPLE_CLOUD_H__

#include "Cry_Math.h"
#include "I3DEngine.h"
#include "AttributeEnums.h"
#include "IEntity.h"

/**
* A simple 3d cloud (equals Entity->Render->Cloud)  */
struct SSituationSimpleCloud
{

	SSituationSimpleCloud() :   x_nMaxClouds(1),
		x_nMinClouds(1),
		x_fMinDistance(2.f),
		x_fMaxDistance(20.f),
		x_fDistanceToPlayer(100.f),
		x_fScaleFactor(1.f),
		x_fHeight(100.f),
		x_fFadeInTime(1.f),
		x_sCloudFile("Libs/Clouds/Default.xml"),
		x_sMaterialFile("materials/clouds/baseclouds"),
/** Private */
		m_bFadeOutClouds(false),
		m_fTimeRemaining(0.f),
		m_fCloudFadeTimer(0.f),
		m_nCloudsToRemovePerQuater(0)
	{
	}

	/**
	* Loads params from xml node.
	* @param cloudRoot node named <SimpleCloud>, must contain <MovementParameters>
	* @returns true on success */
	bool LoadFromXml(XmlNodeRef cloudRoot);

	/**
	* Spawns clouds and initializes everything of this one type */
	void SpawnCloud();

	/**
	* Needs to be called once per frame  */
	void Update(float fFrameTime);

	/**
	* Resets all values so clouds can be spawned anew  */
	void Reset();

	/**
	* Gets called by its parent situation */
	void OnSituationAboutToChange(float fTimeLeft);


	/**
	* Converts  SMovementParams to SCloudMovementProperties */
	SCloudMovementProperties GetMovementProperties();

	//=============================================================================
	// Public Attributes
	//=============================================================================


	/** Maximum number of this cloud spawned during one situation  */
	int x_nMaxClouds;
	/** Minimum number of clouds spawned during one situation */
	int x_nMinClouds;
	/** Minimum distance between two clouds of this type  */
	float x_fMinDistance;
	/** Maximum distance between two clouds */
	float x_fMaxDistance;
	/** Distance from the player at which the cloud is spawned  */
	float x_fDistanceToPlayer;
	/** Random scale factor */
	float x_fScaleFactor;
	/** Height at which cloud is spawned */
	float x_fHeight;
	/** Time until cloud material reaches target opacity */ 
	float x_fFadeInTime;
	/** Path to the cloud xml file  */
	XmlString x_sCloudFile;
	/** Path to the material file  */
	XmlString x_sMaterialFile;
	/** movement parameters for this cloud  */
	SCloudMovementProperties x_movementParams;

private:

	/**
	* Calculates a new random cloud pos taking min max distance into account
	* @returns the vector pos for the next cloud to be spawned */
	Vec3 GetRandomCloudPos(Vec3 vLastPos);

	/** Should clouds be faded out */
	bool m_bFadeOutClouds;
	/** Timer for cloud removal */
	float m_fCloudFadeTimer;
	/** Storage for time remaining on situation about to change event */
	float m_fTimeRemaining;
	/** How many clouds have to be removed every quater of m_fTimeRemaining */
	int m_nCloudsToRemovePerQuater;
	/** Set when entity is first created */ 
	float m_fTargetOpacity;

	/** Entity interfaces to the spawned clouds */
	std::vector<IEntity *> m_simpleCloudEntities;


};

#endif // !__SITUATION_SIMPLE_CLOUD_H__