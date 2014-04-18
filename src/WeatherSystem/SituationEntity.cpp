#include "StdAfx.h"
#include "SituationEntity.h"
#include "IEntity.h"
#include "IEntitySystem.h"
#include "IWeatherSituation.h"
#include "IGameFramework.h"
#include "Actor.h"

SSituationEntity::SSituationEntity() :
	p_sEntityClass("BasicEntity"),
	p_vPos(Vec3(0, 0, 0)),
	p_vDir(Vec3(1, 0, 0)),
	p_fScaleFactor(1.f),
	p_positioningMode(EPositioningMode::ePM_AroundPlayer),
	p_bSpawnOnce(true),
	p_fSpawnAfterSecs(1.f),
	p_fRemoveAfterSecs(10.f),
	p_fMinSpawnDelay(1.f),
	p_fMaxSpawnDelay(1.f),
	p_fRadius(10.f),
	p_fMinHeight(10.f),
	p_fMaxHeight(10.f),
	m_fSpawnTimer(0.f),
	m_fTimeTilSpawn(0.f),
	m_fTotalActiveTime(0.f),
	m_bHasBeenSpawned(false),
	m_bHasBeenRemoved(false),
	m_pEntity(nullptr)
{
}

SSituationEntity::~SSituationEntity()
{
	for (auto pair : m_values)
	{
		delete(pair);
	}
	m_values.clear();
}

void SSituationEntity::LoadFromXml(XmlNodeRef root)
{
	if (!root)
	{
		return;
	}

	string modName = "Entity";

	XMLGET(root, "class", p_sEntityClass, modName);

	XmlString sPos;
	XMLGET(root, "position", sPos, modName);
	p_vPos = ParsePositionAttribute(sPos);

	XMLGET(root, "direction", p_vDir, modName);
	XMLGET(root, "scaleFactor", p_fScaleFactor, modName);

	XmlString sPosMode;
	XMLGET(root, "positioningMode", sPosMode, modName);
	p_positioningMode = GetPositioningModeFromString(sPosMode);

	XMLGET(root, "spawnOnce", p_bSpawnOnce, modName);
	XMLGET(root, "spawnAfterSecs", p_fSpawnAfterSecs, modName);
	XMLGET(root, "removeAfterSecs", p_fRemoveAfterSecs, modName);
	XMLGET(root, "minSpawnDelay", p_fMinSpawnDelay, modName);
	XMLGET(root, "maxSpawnDelay", p_fMaxSpawnDelay, modName);
	XMLGET(root, "radius", p_fRadius, modName);
	XMLGET(root, "minHeight", p_fMinHeight, modName);
	XMLGET(root, "maxHeight", p_fMaxHeight, modName);
	XMLGET(root, "removeOnHide", p_bRemoveOnHide, modName);

	if (XmlNodeRef param = root->findChild("Params"))
	{
		modName = "Entity params";
		for (int i = 0; i < param->getChildCount(); ++i)
		{
			XmlNodeRef child = param->getChild(i);

			if (child)
			{
				SEntityParamPair *pair = new SEntityParamPair();
				XmlString sType;
				XMLGET(child, "type", sType, modName);
				XMLGET(child, "name", pair->p_sName, modName);

				ScriptAnyType type = GetTypeFromString(sType);

				switch (type)
				{
				case ANY_TBOOLEAN:
				{
					bool bVal;
					XMLGET(child, "value", bVal, modName);
					pair->p_val = bVal;
				}
				break;
				case ANY_TNUMBER:
				{
					float fVal;
					XMLGET(child, "value", fVal, modName);
					pair->p_val = fVal;
				}
				break;
				case ANY_TSTRING:
				{
					XmlString sVal;
					XMLGET(child, "value", sVal, modName);
					pair->p_val = sVal.c_str();
					pair->p_sStringVal = sVal;
				}
				break;
				case ANY_TVECTOR:
				{
					Vec3 vVal;
					XMLGET(child, "value", vVal, modName);
					pair->p_val = vVal;
				}
				break;
				default:
					break;
				}

				m_values.push_back(pair);
			}
		}
	}

	m_spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(p_sEntityClass);

}

void SSituationEntity::SpawnEntity()
{
	if (p_fSpawnAfterSecs == -1)
	{
		m_fTimeTilSpawn = Random(p_fMinSpawnDelay, p_fMaxSpawnDelay);
	}
	else
	{
		m_fTimeTilSpawn = p_fSpawnAfterSecs;
	}
}

void SSituationEntity::Update(float fFrameTime)
{
	if (m_bHasBeenRemoved)
	{
		return;
	}
	if (!m_spawnParams.pClass)
	{
		WEATHERSYSTEM_WARNING("Class %s not found, not spawning entity!", p_sEntityClass);
		return;
	}

	if (m_bHasBeenSpawned && p_bSpawnOnce)
	{
		return;
	}

	if (m_fSpawnTimer >= m_fTimeTilSpawn)
	{
		Vec3 pos = p_vPos;

		switch (p_positioningMode)
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
			Vec3 playerPos = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos();
			pos = playerPos + v * p_fRadius;
			pos.z = playerPos.z + Random(p_fMinHeight, p_fMaxHeight);
		}
		break;
		case ePM_AroundWorld:
		{
			Vec3 v;
			v.SetRandomDirection();
			int nSize = gEnv->p3DEngine->GetTerrainSize();
			pos = gEnv->pGame->GetIGameFramework()->GetClientActor()->GetEntity()->GetPos() + v * nSize;
		}
		break;
		default:
			break;


		}

		if (!m_pEntity)
		{
			m_spawnParams.vPosition = pos;
			m_spawnParams.vScale = Vec3(p_fScaleFactor);
			CreateEntity();
		}
		else
		{
			if (m_pEntity->IsHidden())
			{
				m_pEntity->Hide(false);
			}
			m_pEntity->SetPos(pos);
		}

		m_fTimeTilSpawn = Random(p_fMinSpawnDelay, p_fMaxSpawnDelay);
		m_fSpawnTimer = 0.f;
	}

	if (m_fTotalActiveTime >= p_fRemoveAfterSecs)
	{
		gEnv->pEntitySystem->RemoveEntity(m_pEntity->GetId());
		m_bHasBeenRemoved = true;
	}

	if (m_bHasBeenSpawned)
	{
		m_fTotalActiveTime += fFrameTime;
	}

	m_fSpawnTimer += fFrameTime;
}

void SSituationEntity::Reset()
{
	m_fSpawnTimer = 0.f;
	m_fTotalActiveTime = 0.f;
	m_fTimeTilSpawn = 0.f;
	m_bHasBeenRemoved = false;
	m_bHasBeenSpawned = false;

	if (m_pEntity && !gEnv->IsEditing())
	{
		m_pEntity->Hide(true);

		if (p_bRemoveOnHide)
		{
			gEnv->pEntitySystem->RemoveEntity(m_pEntity->GetId());
			m_pEntity = nullptr;
		}
	}
}

//=============================================================================
// Private
//=============================================================================

ScriptAnyType SSituationEntity::GetTypeFromString(XmlString sType)
{
	if (strcmp(sType, "bool") == 0)
	{
		return    ANY_TBOOLEAN;
	}
	else if (strcmp(sType, "number") == 0)
	{
		return    ANY_TNUMBER;
	}
	else if (strcmp(sType, "string") == 0)
	{
		return    ANY_TSTRING;
	}
	else if (strcmp(sType, "vector") == 0)
	{
		return    ANY_TVECTOR;
	}
	else
	{
		return ANY_ANY;
	}
}

void SSituationEntity::CreateEntity()
{
	m_pEntity = gEnv->pEntitySystem->SpawnEntity(m_spawnParams);

	if (m_pEntity && m_values.size() > 0)
	{
		SmartScriptTable table = m_pEntity->GetScriptTable();

		if (table)
		{
			for (auto pair : m_values)
			{
				SetTableMemeberChain(table, pair->p_sName, pair->p_val.type == ScriptAnyType::ANY_TSTRING ? pair->p_sStringVal.c_str() : pair->p_val);
			}

			gEnv->pScriptSystem->BeginCall(table, "OnPropertyChange");
			gEnv->pScriptSystem->PushFuncParam(table);
			gEnv->pScriptSystem->EndCall(table);
		}


	}

	m_bHasBeenSpawned = true;
}