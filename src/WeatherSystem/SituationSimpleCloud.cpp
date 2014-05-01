#include "StdAfx.h"
#include "SituationSimpleCloud.h"
#include "IEntitySystem.h"
#include "Actor.h"
#include "IGameFramework.h"

bool SSituationSimpleCloud::LoadFromXml(XmlNodeRef root)
{
	string modName = "Cloud definition";
	auto moveRoot = root->findChild("MovementParameters");

	XMLGET(root, "cloudFile", x_sCloudFile, modName);
	XMLGET(root, "material", x_sMaterialFile, modName);
	XMLGET(root, "distanceToPlayer", x_fDistanceToPlayer, modName);

	XMLGET(root, "maxCount", x_nMaxClouds, modName);
	XMLGET(root, "minCount", x_nMinClouds, modName);
	XMLGET(root, "minDistance", x_fMinDistance, modName);
	XMLGET(root, "maxDistance", x_fMaxDistance, modName);
	XMLGET(root, "randomScaleFactor", x_fScaleFactor, modName);
	XMLGET(root, "height", x_fHeight, modName);
	XMLGET(root, "fadeInTime", x_fFadeInTime, modName);

	if (!moveRoot)
	{
		WEATHERSYSTEM_WARNING("No movement parameters defined for %s! Defaults may not be suitable!", x_sCloudFile);
	}
	else
	{
		XMLGET(moveRoot, "autoMove", x_movementParams.m_autoMove, modName);
		XMLGET(moveRoot, "fadeDistance", x_movementParams.m_fadeDistance, modName);
		XMLGET(moveRoot, "spaceLoopBox", x_movementParams.m_spaceLoopBox, modName);
		XMLGET(moveRoot, "speed", x_movementParams.m_speed, modName);
	}

	/** Useless I know, that's what you get for designing on the fly :D */
	return true;
}

void SSituationSimpleCloud::SpawnCloud()
{

	int count = x_nMinClouds + (x_nMaxClouds - x_nMinClouds) * cry_frand();
	IMaterial *pMat = gEnv->p3DEngine->GetMaterialManager()->FindMaterial(gEnv->pCryPak->GetGameFolder() + x_sMaterialFile);

	if (!pMat)
	{
		WEATHERSYSTEM_LOG("Couldn't find material %s, loading...", x_sMaterialFile);
		pMat = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(x_sMaterialFile, false);
	}

	Vec3 vLastPos;
	vLastPos.zero();

	for (int i = 0; i < count; ++i)
	{
		SEntitySpawnParams params;
		params.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Cloud");
		vLastPos = params.vPosition = GetRandomCloudPos(vLastPos);
		params.vScale = Vec3(1, 1, 1) * (cry_frand() * x_fScaleFactor);

		IEntity *pCloudEnt = gEnv->pEntitySystem->SpawnEntity(params);

		if (pCloudEnt)
		{
			SmartScriptTable cloudTable = pCloudEnt->GetScriptTable();

			if (pMat)
			{
				pCloudEnt->SetSlotMaterial(0, pMat);
			}
			else
			{
				WEATHERSYSTEM_WARNING("Couldn't find material %s for cloud %s", x_sMaterialFile, x_sCloudFile);
			}

			IEntityRenderProxy *pProxy = (IEntityRenderProxy*)pCloudEnt->GetProxy(EEntityProxy::ENTITY_PROXY_RENDER);

			m_fTargetOpacity = pProxy->GetOpacity();
			pProxy->SetOpacity(0.f);


			SmartScriptTable props;
			SmartScriptTable move;

			cloudTable->GetValue("Properties", props);
			props->GetValue("Movement", move);



			props->SetValue("file_CloudFile", x_sCloudFile.c_str());

			move->SetValue("bAutoMove", x_movementParams.m_autoMove);
			move->SetValue("vector_Speed", x_movementParams.m_speed);
			move->SetValue("vector_SpaceLoopBox", x_movementParams.m_spaceLoopBox);
			move->SetValue("fFadeDistance", x_movementParams.m_fadeDistance);



			gEnv->pScriptSystem->BeginCall(cloudTable, "OnPropertyChange");
			gEnv->pScriptSystem->PushFuncParam(cloudTable);
			gEnv->pScriptSystem->EndCall();

			m_simpleCloudEntities.push_back(pCloudEnt);

		}
	}
}

void SSituationSimpleCloud::Update(float fFrameTime)
{
	if (m_bFadeOutClouds)
	{
		if (m_fCloudFadeTimer >= m_fTimeRemaining / 4)
		{
			int currentSize = m_simpleCloudEntities.size();

			int toRemove = currentSize <= m_nCloudsToRemovePerQuater ? currentSize : m_nCloudsToRemovePerQuater;

			std::vector<IEntity *>::iterator it;

			int i = 0;
			for (it = m_simpleCloudEntities.end() - 1; i < toRemove; --it, ++i)
			{
				IEntity *pEnt = (*it);
				gEnv->pEntitySystem->RemoveEntity(pEnt->GetId());

				m_simpleCloudEntities.pop_back();
			}
			m_fCloudFadeTimer = 0.f;

		}
		else
		{
			m_fCloudFadeTimer += fFrameTime;
		}
	}
}

void SSituationSimpleCloud::Reset()
{
	m_fCloudFadeTimer = 0.f;
	m_fTimeRemaining = 0.f;
	m_bFadeOutClouds = false;

	// After exiting game mode in editor, all entity interfaces become invalid
	if (gEnv->IsEditor() && gEnv->IsEditing())
	{
		m_simpleCloudEntities.clear();
	}
}

void SSituationSimpleCloud::OnSituationAboutToChange(float fTimeLeft)
{
	m_bFadeOutClouds = true;
	m_fTimeRemaining = fTimeLeft;
	m_nCloudsToRemovePerQuater = m_simpleCloudEntities.size() / 4;
}

//=============================================================================
// Private
//=============================================================================


Vec3 SSituationSimpleCloud::GetRandomCloudPos(Vec3 vLastPos)
{
	if (vLastPos.IsZero())
	{
		Vec3 vPlayerPos = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos();

		Vec3 vDir;
		vDir.SetRandomDirection();
		vDir.z = 0;
		vDir.Normalize();

		vPlayerPos.z += x_fHeight;

		return vPlayerPos + vDir * cry_frand() * x_fDistanceToPlayer;
	}
	else
	{
		Vec3 vDir;
		vDir.SetRandomDirection();
		vDir.z = 0;
		vDir.Normalize();

		return vLastPos + vDir * Random(x_fMinDistance, x_fMaxDistance);
	}
}