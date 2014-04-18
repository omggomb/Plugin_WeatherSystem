#ifndef __ATTRIBUTE_ENUMS_H__
#define __ATTRIBUTE_ENUMS_H__

#include "Cry_Math.h"
#include "IWeatherSituation.h"

/**
* Helper macro which checks if the wanted attribute was found and sends a warning if it wasn't 
* @param root The XmlNodeRef node in which the attribute should be searched for
* @param key Name of the attribute
* @param param Reference to the param to be filled with the value of key 
* @param module Display name for the warning message if key was not found */
#define XMLGET(root, key, param, module) if (!root->getAttr(key, param)) \
	{ \
		WEATHERSYSTEM_WARNING("Couldn't find attribute %s for %s", key, module); \
	}

/**
* Determines how the postion is deduced form the attributes
*/
enum EPositioningMode
{
	/** position attribute is treated as absolute world position */
	ePM_World,
	/** position attribute is treated as local, player related position */
	ePM_Local,
	/** position ignored, random position around player using radius attribute */
	ePM_AroundPlayer,
	/** position ingored, random position in game world */
	ePM_AroundWorld
};

/**
* Parses sString and returns a poisitioning mode or ePM_AroundWorld if no matching mode was found  */
EPositioningMode GetPositioningModeFromString(const char *sString);

/**
* Sets a member of the table or its subtable to the specified value, since SmartScriptTable::SetValueChain does 
* not seem to work. 
* @attention This method does not free the memory of table, hand it a SmartScriptTable to be safe 
* @param table table containing the value to be set
* @param sMember path to the member to be set (ex: "path.to.some.member.in.sub.tale" 
* @param val the value for the key to be set */
void SetTableMemeberChain(IScriptTable *table, const string sMember, const ScriptAnyValue &val);

/**
* Parses a string to see whether it contains a valid name of an entity in the current level (not an entity class!). 
* If the entity is found, its position is returned, else the string is interpreted as vector
* @attention Make sure a level is loaded, otherwise there are no entities to be looked for 
* @param sPos String containing either an entity name or a vector (="0,0,1") 
* @returns if no level is loaded returns zero vector */
Vec3 ParsePositionAttribute(string sPos);

#endif
