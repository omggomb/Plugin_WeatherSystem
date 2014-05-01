#include "StdAfx.h"
#include "AttributeEnums.h"
#include <list>
#include "IEntitySystem.h"

EPositioningMode GetPositioningModeFromString(const char *sString)
{
	if (strcmp(sString, "AroundPlayer") == 0)
	{
		return EPositioningMode::ePM_AroundPlayer;
	}
	else if (strcmp(sString, "AroundWorld") == 0)
	{
		return EPositioningMode::ePM_AroundWorld;
	}
	else if (strcmp(sString, "Local") == 0)
	{
		return EPositioningMode::ePM_Local;
	}
	else
	{
		return EPositioningMode::ePM_World;
	}

}

void SetTableMemeberChain(IScriptTable *table, const string sMember, const ScriptAnyValue &val)
{
	string sCurrent, sNext;
	int nPos = 0;
	std::list<string> tables;

	sCurrent = sMember.Tokenize(".", nPos);

	while (!sCurrent.empty())
	{
		tables.push_back(sCurrent);
		
		sCurrent = sMember.Tokenize(".", nPos);
	}

	string sAttribute = (*(--tables.end()));
	tables.pop_back();

	SmartScriptTable currentTable;

	if (tables.size() > 0)
	{
		SmartScriptTable firstTable;
		table->GetValue((*tables.begin()), firstTable);
		tables.pop_front();

		if (tables.size() > 1)
		{
			currentTable = firstTable;
			for (auto it = tables.begin(); it != tables.end(); ++it)
			{
				currentTable->GetValue((*it), currentTable);
			}

			currentTable->SetValue(sAttribute, val);
		}
		else
		{
			firstTable->SetValue(sAttribute, val);
		}
	}
	else
	{
		table->SetValue(sAttribute, val);
	}
}

Vec3 ParsePositionAttribute(string sPos)
{
	if (IEntity *pEnt = gEnv->pEntitySystem->FindEntityByName(sPos))
	{
		return pEnt->GetPos();
	}
	else
	{
		// Ugly :/
		XmlNodeRef x = gEnv->pSystem->CreateXmlNode("Nothing");
		x->setAttr("pos", sPos);
		Vec3 v;
		x->getAttr("pos", v);
		return v;
	}
}