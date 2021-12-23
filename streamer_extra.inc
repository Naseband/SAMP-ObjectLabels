#if defined __streamer_extra_incl_
#endinput
#endif
#define __streamer_extra_incl_

/**********

- Streamer Extra Handler -

Name:			Streamer Extra Handler

Dependencies: 	streamer

**********/

#if !defined _streamer_included
#include <streamer>
#endif

// 			---			OBJECTS

stock SetDynamicObjectExtra(objectid, type, id) 
{ 
	new extra[2]; 

	extra[0] = type;
	extra[1] = id;

	return Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, extra); 
}

stock GetDynamicObjectExtra(objectid, &type, &id)
{ 
	new extra[2], ret = Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, extra); 

	type = extra[0];
	id = extra[1];

	return ret; 
}

stock SetDynamicObjectInterior(objectid, interior)
{
	new interiors[1];
	interiors[0] = interior;
	return Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_INTERIOR_ID, interiors);
}

stock SetDynamicObjectVirtualWorld(objectid, worldid)
{
	new worlds[1];
	worlds[0] = worldid;
	return Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID, worlds);
}

stock SetDynamicObjectModel(objectid, model)
{
	if(IsValidDynamicObject(objectid)) return Streamer_SetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_MODEL_ID, model);

	return 0;
}

stock SetDynamicObjectPosForPlayer(playerid, objectid, Float:x, Float:y, Float:z)
{
	if(Streamer_IsItemVisible(playerid, STREAMER_TYPE_OBJECT, objectid))
	{
		new playerobjid = Streamer_GetItemInternalID(playerid, STREAMER_TYPE_OBJECT, objectid);

		if(IsValidPlayerObject(playerid, playerobjid)) return SetPlayerObjectPos(playerid, playerobjid, x, y, z);
	}

	return 0;
}

stock SetDynamicObjectRotForPlayer(playerid, objectid, Float:rx, Float:ry, Float:rz)
{
	if(Streamer_IsItemVisible(playerid, STREAMER_TYPE_OBJECT, objectid))
	{
		new playerobjid = Streamer_GetItemInternalID(playerid, STREAMER_TYPE_OBJECT, objectid);

		if(IsValidPlayerObject(playerid, playerobjid)) return SetPlayerObjectRot(playerid, playerobjid, rx, ry, rz);
	}

	return 0;
}

stock MoveDynamicObjectForPlayer(playerid, objectid, Float:x, Float:y, Float:z, Float:speed, Float:rx = -1000.0, Float:ry = -1000.0, Float:rz = -1000.0)
{
	if(Streamer_IsItemVisible(playerid, STREAMER_TYPE_OBJECT, objectid))
	{
		new playerobjid = Streamer_GetItemInternalID(playerid, STREAMER_TYPE_OBJECT, objectid);

		if(IsValidPlayerObject(playerid, playerobjid)) return MovePlayerObject(playerid, playerobjid, x, y, z, speed, rx, ry, rz);
	}

	return 0;
}

stock IsDynamicObjectInRangeOfPoint(STREAMER_TAG_OBJECT:objectid, Float:radius, Float:x, Float:y, Float:z)
{
	new Float:x2, Float:y2, Float:z2;

	GetDynamicObjectPos(objectid, x2, y2, z2);

	return (VectorSize(x - x2, y - y2, z - z2) < radius);
}

// 			---			AREAS

stock SetDynamicAreaExtra(areaid, type, id)
{ 
	new extra[2]; 

	extra[0] = type;
	extra[1] = id;

	return Streamer_SetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, extra); 
}

stock GetDynamicAreaExtra(areaid, &type, &id)
{ 
	new extra[2], ret = Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, extra); 

	type = extra[0];
	id = extra[1];

	return ret; 
}

stock SetDynamicAreaInterior(areaid, interior)
{
	new interiors[1];
	interiors[0] = interior;
	return Streamer_SetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_INTERIOR_ID, interiors);
}

stock SetDynamicAreaVirtualWorld(areaid, worldid)
{
	new worlds[1];
	worlds[0] = worldid;
	return Streamer_SetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_WORLD_ID, worlds);
}

stock SetDynamicAreaPosXYZ(areaid, Float:x, Float:y, Float:z)
{
	if(	Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_X, x) &&
		Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_Y, y) &&
		Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_Z, z))
	{
		return 1;
	}

	return 0;
}

stock GetDynamicAreaPosXYZ(areaid, &Float:x, &Float:y, &Float:z)
{
	if(	Streamer_GetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_X, x) &&
		Streamer_GetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_Y, y) &&
		Streamer_GetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_Z, z))
	{
		return 1;
	}

	return 0;
}

// 			---			ACTORS

stock SetDynamicActorExtra(actorid, type, id)
{ 
	new extra[2]; 

	extra[0] = type;
	extra[1] = id;

	return Streamer_SetArrayData(STREAMER_TYPE_ACTOR, actorid, E_STREAMER_EXTRA_ID, extra); 
}

stock GetDynamicActorExtra(actorid, &type, &id)
{ 
	new extra[2], ret = Streamer_GetArrayData(STREAMER_TYPE_ACTOR, actorid, E_STREAMER_EXTRA_ID, extra); 

	type = extra[0];
	id = extra[1];

	return ret; 
}

stock SetDynamicActorInterior(actorid, interior)
{
	new interiors[1];
	interiors[0] = interior;
	return Streamer_SetArrayData(STREAMER_TYPE_ACTOR, actorid, E_STREAMER_INTERIOR_ID, interiors);
}

// 			---			3D TEXT LABELS

stock SetDynamicLabelExtra(Text3D:labelid, type, id)
{ 
	new extra[2]; 

	extra[0] = type;
	extra[1] = id;

	return Streamer_SetArrayData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_EXTRA_ID, extra); 
}

stock GetDynamicLabelExtra(Text3D:labelid, &type, &id)
{ 
	new extra[2], ret = Streamer_GetArrayData(STREAMER_TYPE_LABEL, labelid, E_STREAMER_EXTRA_ID, extra); 

	type = extra[0];
	id = extra[1];

	return ret; 
}

stock SetDynamicLabelInterior(Text3D:labelid, interior)
{
	new interiors[1];
	interiors[0] = interior;
	return Streamer_SetArrayData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_INTERIOR_ID, interiors);
}

stock SetDynamicLabelPos({Text3D, _}:labelid, Float:x, Float:y, Float:z)
{
	Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_X, x);
	Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_Y, y);
	return Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_Z, z);
}

// 			---			PICKUPS

stock SetDynamicPickupExtra(pickupid, type, id) 
{ 
	new extra[2]; 

	extra[0] = type;
	extra[1] = id;

	return Streamer_SetArrayData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_EXTRA_ID, extra); 
}

stock GetDynamicPickupExtra(pickupid, &type, &id)
{ 
	new extra[2], ret = Streamer_GetArrayData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_EXTRA_ID, extra); 

	type = extra[0];
	id = extra[1];

	return ret; 
}

stock SetDynamicPickupInterior(pickupid, interior)
{
	new interiors[1];
	interiors[0] = interior;
	return Streamer_SetArrayData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_INTERIOR_ID, interiors);
}

stock SetDynamicPickupVirtualWorld(pickupid, worldid)
{
	new worlds[1];
	worlds[0] = worldid;
	return Streamer_SetArrayData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_WORLD_ID, worlds);
}

stock SetDynamicPickupPos(pickupid, Float:x, Float:y, Float:z)
{
	Streamer_SetFloatData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_X, x);
	Streamer_SetFloatData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_Y, y);
	return Streamer_SetFloatData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_Z, z);
}

// 			---			CHECKPOINTS

stock SetDynamicCPExtra(checkpointid, type, id) 
{ 
	new extra[2]; 

	extra[0] = type;
	extra[1] = id;

	return Streamer_SetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_EXTRA_ID, extra); 
}

stock GetDynamicCPExtra(checkpointid, &type, &id)
{ 
	new extra[2], ret = Streamer_GetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_EXTRA_ID, extra); 

	type = extra[0];
	id = extra[1];

	return ret; 
}

stock SetDynamicCPInterior(checkpointid, interior)
{
	new interiors[1];
	interiors[0] = interior;
	return Streamer_SetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_INTERIOR_ID, interiors);
}

stock SetDynamicCPPos(checkpointid, Float:x, Float:y, Float:z)
{
	Streamer_SetFloatData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_X, x);
	Streamer_SetFloatData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_Y, y);
	return Streamer_SetFloatData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_Z, z);
}

// 			---			RACE CHECKPOINTS

stock SetDynamicRaceCPExtra(checkpointid, type, id) 
{ 
	new extra[2]; 

	extra[0] = type;
	extra[1] = id;

	return Streamer_SetArrayData(STREAMER_TYPE_RACE_CP, checkpointid, E_STREAMER_EXTRA_ID, extra); 
}

stock GetDynamicRaceCPExtra(checkpointid, &type, &id)
{ 
	new extra[2], ret = Streamer_GetArrayData(STREAMER_TYPE_RACE_CP, checkpointid, E_STREAMER_EXTRA_ID, extra); 

	type = extra[0];
	id = extra[1];

	return ret; 
}

stock SetDynamicRaceCPInterior(checkpointid, interior)
{
	new interiors[1];
	interiors[0] = interior;
	return Streamer_SetArrayData(STREAMER_TYPE_RACE_CP, checkpointid, E_STREAMER_INTERIOR_ID, interiors);
}

stock SetDynamicRaceCPVirtualWorld(checkpointid, worldid)
{
	new worlds[1];
	worlds[0] = worldid;
	return Streamer_SetArrayData(STREAMER_TYPE_RACE_CP, checkpointid, E_STREAMER_WORLD_ID, worlds);
}

stock SetDynamicRaceCPPos(checkpointid, Float:x, Float:y, Float:z)
{
	Streamer_SetFloatData(STREAMER_TYPE_RACE_CP, checkpointid, E_STREAMER_X, x);
	Streamer_SetFloatData(STREAMER_TYPE_RACE_CP, checkpointid, E_STREAMER_Y, y);
	return Streamer_SetFloatData(STREAMER_TYPE_RACE_CP, checkpointid, E_STREAMER_Z, z);
}

// 			---			MAP ICONS

stock SetDynamicMapIconExtra(iconid, type, id)
{ 
	new extra[2]; 

	extra[0] = type;
	extra[1] = id;

	return Streamer_SetArrayData(STREAMER_TYPE_MAP_ICON, iconid, E_STREAMER_EXTRA_ID, extra); 
}

stock GetDynamicMapIconExtra(iconid, &type, &id)
{ 
	new extra[2], ret = Streamer_GetArrayData(STREAMER_TYPE_MAP_ICON, iconid, E_STREAMER_EXTRA_ID, extra); 

	type = extra[0];
	id = extra[1];

	return ret; 
}

stock SetDynamicMapIconInterior(iconid, interior)
{
	new interiors[1];
	interiors[0] = interior;
	return Streamer_SetArrayData(STREAMER_TYPE_MAP_ICON, iconid, E_STREAMER_INTERIOR_ID, interiors);
}

stock SetDynamicMapIconVirtualWorld(iconid, worldid)
{
	new worlds[1];
	worlds[0] = worldid;
	return Streamer_SetArrayData(STREAMER_TYPE_MAP_ICON, iconid, E_STREAMER_WORLD_ID, worlds);
}

stock SetDynamicMapIconPos(iconid, Float:x, Float:y, Float:z)
{
	Streamer_SetFloatData(STREAMER_TYPE_MAP_ICON, iconid, E_STREAMER_X, x);
	Streamer_SetFloatData(STREAMER_TYPE_MAP_ICON, iconid, E_STREAMER_Y, y);
	return Streamer_SetFloatData(STREAMER_TYPE_MAP_ICON, iconid, E_STREAMER_Z, z);
}

// EOF