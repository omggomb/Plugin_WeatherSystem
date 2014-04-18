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
	size_t dotPos = sMember.find_first_of('.');
	size_t lastDot = 0;

	std::list<string> tables;
	while (dotPos != string::npos)
	{
		tables.push_back(sMember.substr(lastDot, dotPos - lastDot));


		lastDot = dotPos + 1;

		if (!(lastDot < sMember.size()))
		{
			break;
		}

		dotPos = sMember.find_first_of('.', lastDot);
	}

	lastDot = sMember.find_last_of('.');

	string sTableMember;
	if (lastDot == string::npos)
	{
		sTableMember = sMember;
	}
	else
	{
		++lastDot;
		if (lastDot < sMember.size())
		{
			sTableMember = sMember.substr(lastDot, sMember.size() - lastDot);
		}
		else
		{
			sTableMember = "";
		}
	}

	if (tables.size() > 0)
	{
		SmartScriptTable firstTable;
		table->GetValue((*tables.begin()), firstTable);

		if (tables.size() > 1)
		{
			SmartScriptTable nextTable = firstTable;

			auto iter = tables.begin();
			++iter;

			while (iter != tables.end())
			{
				nextTable->GetValue((*iter), nextTable);

				++iter;
			}

			nextTable->SetValue(sTableMember, val);
		}
		else
		{
			firstTable->SetValue(sTableMember, val);
		}
	}
	else
	{
		table->SetValue(sTableMember, val);
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