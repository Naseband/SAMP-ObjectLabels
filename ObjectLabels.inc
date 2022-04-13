/*

--- Object Labels include by NaS/Naseband

Simple include that creates objects using material texts to display texts.
The objects will always face the player, like 3D text labels.

See definitions below for global/model customization.


--- Functions:


--- CreateObjectLabel(const text[], Float:x, Float:y, Float:z, Float:range = OBJECT_LABEL_DEFAULT_RANGE, const font_face[] = "Verdana", font_size = 16, font_color = 0xFFFFFFFF, interior = -1, virtual_world = -1, playerid = -1)

Returns ID on success, -1 otherwise.

--- DestroyObjectLabel(id, bool:remove_from_iter = true)

remove_from_iter should always be true unless you use it in OnGameModeExit/OnFilterScriptExit. Mostly for internal purposes.

Returns 1 on success, 0 otherwise (label does not exist).

--- DestroyAllObjectLabels()

Returns amount of destroyed labels.

--- IsValidObjectLabel(id)

Returns 1 if valid, 0 otherwise.

--- GetFreeObjectLabelSlot()

Returns first found empty ID, -1 if none found. Mostly for internal purposes.

--- UpdateObjectLabelText(id, const text[] = "")

Updates the label's text. If text is empty (""), the previous text will be kept, but it updates anyway.

Returns 1 on success, 0 otherwise (label does not exist).

--- SetObjectLabelModelData(id, modelid = OBJECT_LABEL_MODEL, material_index = OBJECT_LABEL_MATERIAL_INDEX, material_size = OBJECT_LABEL_MATERIAL_SIZE, Float:off_x = OBJECT_LABEL_OFF_X, Float:off_y = OBJECT_LABEL_OFF_Y, Float:off_z = OBJECT_LABEL_OFF_Z, Float:off_rx = OBJECT_LABEL_OFF_RX, Float:off_ry = OBJECT_LABEL_OFF_RY, Float:off_rz = OBJECT_LABEL_OFF_RZ)

Updates the label's model data. Offsets are relative to the world axes.
The rotation also used to determine the axes the object will rotate. Use values of 0.0, 90.0, 180.0 or 270.0 for RZ only and use the Y rotation axis to make the object face north.
Object models that have a different orientation (ie. 45.0 degrees on the Z axis) are not supported.

Returns 1 on success, 0 otherwise (label does not exist).

--- SetObjectLabelFontFace(id, const font_face[], bool:update = true)

Updates the label's font face.
If you change multiple options, like font size, font face and color, you can set update to false on all but the last call to save unneccessary calls to UpdateObjectLabelText.

Returns 1 on success, 0 otherwise (label does not exist).

--- SetObjectLabelFontSize(id, font_size, bool:update = true)

Updates the label's font size.
(See SetObjectLabelFontFace notes)

Returns 1 on success, 0 otherwise (label does not exist).

--- SetObjectLabelFontColor(id, font_color, bool:update = true)

Updates the label's font color.
(See SetObjectLabelFontFace notes)

Returns 1 on success, 0 otherwise (label does not exist).

--- SetObjectLabelFrontOffset(id, Float:offset = 0.0)

Updates the label's front offset. Use 0.0 to disable. Negative values are also supported.
If used, the object will be moved towards the player's position by (offset) meters, or away from them if negative.
This can be useful if creating a label inside another object, or creating two labels inside each other (one being background).

Returns 1 on success, 0 otherwise (label does not exist).
*/


#if defined __OLABELS_INCLUDED__
#endinput
#endif
#define __OLABELS_INCLUDED__

// ----------------------------------------------------

// Limits/global defaults, can be defined before including ObjectLabels

#if !defined MAX_OBJECT_LABELS
#define MAX_OBJECT_LABELS				1024
#endif

#if !defined OBJECT_LABEL_DEFAULT_RANGE
#define OBJECT_LABEL_DEFAULT_RANGE		25.0
#endif

#if !defined OBJECT_LABEL_INTERVAL
#define OBJECT_LABEL_INTERVAL 			200
#endif

#if !defined OL_USE_PLAYER_CAMERA
#define OL_USE_PLAYER_CAMERA 			false 		// If true the player camera position will be used, but it updates slowly unless KEY_AIM is pressed
#endif

// ----------------------------------------------------

// Default values for the model (can be overriden for each label with SetObjectLabelModelData)

#if !defined OBJECT_LABEL_MODEL
#define OBJECT_LABEL_MODEL 				19482
#endif

#if !defined OBJECT_LABEL_MATERIAL_INDEX
#define OBJECT_LABEL_MATERIAL_INDEX		0
#endif

#if !defined OBJECT_LABEL_MATERIAL_SIZE
#define OBJECT_LABEL_MATERIAL_SIZE 		OBJECT_MATERIAL_SIZE_512x256
#endif

#if !defined OBJECT_LABEL_OFF_X
#define OBJECT_LABEL_OFF_X				0.0
#endif

#if !defined OBJECT_LABEL_OFF_Y
#define OBJECT_LABEL_OFF_Y				0.0
#endif

#if !defined OBJECT_LABEL_OFF_Z
#define OBJECT_LABEL_OFF_Z				0.0
#endif

#if !defined OBJECT_LABEL_OFF_RX
#define OBJECT_LABEL_OFF_RX				0.0
#endif

#if !defined OBJECT_LABEL_OFF_RY
#define OBJECT_LABEL_OFF_RY				0.0
#endif

#if !defined OBJECT_LABEL_OFF_RZ
#define OBJECT_LABEL_OFF_RZ				270.0
#endif

// ----------------------------------------------------

#define OBJECT_LABEL_MAX_TEMP_TEXT 		512 // Max. temporary array size for storing texts (used in UpdateObjectLabelText)
#define OL_EXTRA 						-420133769

// ----------------------------------------------------

static enum E_OBJECT_LABEL
{
	bool:ol_Exists,
	ol_PlayerID,

	ol_ModelID,

	ol_MaterialIndex,
	ol_MaterialSize,

	Float:ol_ModelOffsetX,
	Float:ol_ModelOffsetY,
	Float:ol_ModelOffsetZ,
	Float:ol_ModelOffsetRX,
	Float:ol_ModelOffsetRY,
	Float:ol_ModelOffsetRZ,

	Float:ol_FrontOffset,

	Float:ol_PosX,
	Float:ol_PosY,
	Float:ol_PosZ,
	ol_InteriorID,
	ol_VirtualWorld,

	ol_FontFace[24],
	ol_FontSize,
	ol_FontColor,

	ol_LastMaterialIndex,

	ol_PlayerCount,

	ol_ObjectID,
	ol_AreaID
};
static g_LabelData[MAX_OBJECT_LABELS][E_OBJECT_LABEL];

// ----------------------------------------------------

static bool:g_Initialized = false, g_TimerID;

static Iterator:g_itLabels<MAX_OBJECT_LABELS>, Iterator:g_itActivePlayers<MAX_PLAYERS>,
	g_PlayerLabels[MAX_PLAYERS];

// ----------------------------------------------------

forward OL_Timer();

// ----------------------------------------------------

static stock OL_Init()
{
	if(g_Initialized)
		return 0;

	g_Initialized = true;
	g_TimerID = SetTimer("OL_Timer", OBJECT_LABEL_INTERVAL, 1);

	Iter_Clear(g_itActivePlayers);
	for(new i = 0; i < MAX_PLAYERS; i ++) g_PlayerLabels[i] = 0;

	return 1;
}

static stock OL_Exit()
{
	if(!g_Initialized)
		return 0;

	DestroyAllObjectLabels();

	g_Initialized = false;
	KillTimer(g_TimerID);

	return 1;
}

// ----------------------------------------------------

public OL_Timer()
{
	new areas[25], num;

	foreach(new i : g_itActivePlayers)
	{
		num = GetPlayerDynamicAreas(i, areas);

		if(!num)
			continue;

		if(num > sizeof(areas))
		{
			num = sizeof(areas);

			printf("OL WARNING: Player %i is in more than %i areas", i, sizeof(areas));
		}

		new extra, id, objectid, Float:x, Float:y, Float:z, Float:vx, Float:vy, Float:vz, Float:rx, Float:rz, Float:dist;

		#if OL_USE_PLAYER_CAMERA == true
		GetPlayerCameraPos(i, x, y, z); // Slow update rate except when the player is aiming
		#else
		GetPlayerPos(i, x, y, z);
		#endif

		for(new j = 0; j < num; j ++)
		{
			GetDynamicAreaExtra(areas[j], extra, id);

			if(extra == OL_EXTRA && IsValidObjectLabel(id))
			{
				objectid = Streamer_GetItemInternalID(i, STREAMER_TYPE_OBJECT, g_LabelData[id][ol_ObjectID]);

				if(IsValidPlayerObject(i, objectid))
				{
					vx = g_LabelData[id][ol_PosX] - x;
					vy = g_LabelData[id][ol_PosY] - y;
					vz = g_LabelData[id][ol_PosZ] - z;

					dist = VectorSize(vx, vy, vz);

					if(dist == 0.0) continue;

					vx /= dist;
					vy /= dist;
					vz /= dist;

					OL_GetRXFromVectorZ(vz, rx);
					OL_GetRZFromVectorXY(vx, vy, rz);

					if(g_LabelData[id][ol_FrontOffset] != 0.0)
					{
						vx *= -g_LabelData[id][ol_FrontOffset];
						vy *= -g_LabelData[id][ol_FrontOffset];
						vz *= -g_LabelData[id][ol_FrontOffset];

						SetPlayerObjectPos(i, objectid, g_LabelData[id][ol_PosX] + g_LabelData[id][ol_ModelOffsetX] + vx, g_LabelData[id][ol_PosY] + g_LabelData[id][ol_ModelOffsetY] + vy, g_LabelData[id][ol_PosZ] + g_LabelData[id][ol_ModelOffsetZ] + vz);
					}

					if(g_LabelData[id][ol_ModelOffsetRZ] == 180.0)
						SetPlayerObjectRot(i, objectid, g_LabelData[id][ol_ModelOffsetRX] - rx, g_LabelData[id][ol_ModelOffsetRY], g_LabelData[id][ol_ModelOffsetRZ] + rz);
					else if(g_LabelData[id][ol_ModelOffsetRZ] == 90.0)
						SetPlayerObjectRot(i, objectid, g_LabelData[id][ol_ModelOffsetRX], g_LabelData[id][ol_ModelOffsetRY] - rx, g_LabelData[id][ol_ModelOffsetRZ] + rz);
					else if(g_LabelData[id][ol_ModelOffsetRZ] == 270.0)
						SetPlayerObjectRot(i, objectid, g_LabelData[id][ol_ModelOffsetRX], g_LabelData[id][ol_ModelOffsetRY] + rx, g_LabelData[id][ol_ModelOffsetRZ] + rz);
					else
						SetPlayerObjectRot(i, objectid, g_LabelData[id][ol_ModelOffsetRX] - rx, g_LabelData[id][ol_ModelOffsetRY], g_LabelData[id][ol_ModelOffsetRZ] + rz);
				}
			}
		}
	}
}

// ----------------------------------------------------

stock CreateObjectLabel(const text[], Float:x, Float:y, Float:z, Float:range = OBJECT_LABEL_DEFAULT_RANGE, const font_face[] = "Verdana", font_size = 16, font_color = 0xFFFFFFFF, interior = -1, virtual_world = -1, playerid = -1)
{
	if(!g_Initialized || (playerid != -1 && !IsPlayerConnected(playerid)))
		return -1;

	new id = GetFreeObjectLabelSlot();

	if(id == -1)
		return -1;

	//printf("id: %d", id);

	g_LabelData[id][ol_PlayerID] = playerid;

	g_LabelData[id][ol_ModelID] = OBJECT_LABEL_MODEL;

	g_LabelData[id][ol_MaterialIndex] = OBJECT_LABEL_MATERIAL_INDEX;
	g_LabelData[id][ol_MaterialSize] = OBJECT_LABEL_MATERIAL_SIZE;

	g_LabelData[id][ol_ModelOffsetX] = OBJECT_LABEL_OFF_X;
	g_LabelData[id][ol_ModelOffsetY] = OBJECT_LABEL_OFF_Y;
	g_LabelData[id][ol_ModelOffsetZ] = OBJECT_LABEL_OFF_Z;
	g_LabelData[id][ol_ModelOffsetRX] = OBJECT_LABEL_OFF_RX;
	g_LabelData[id][ol_ModelOffsetRY] = OBJECT_LABEL_OFF_RY;
	g_LabelData[id][ol_ModelOffsetRZ] = OBJECT_LABEL_OFF_RZ;

	g_LabelData[id][ol_FrontOffset] = 0.0;

	g_LabelData[id][ol_PosX] = x;
	g_LabelData[id][ol_PosY] = y;
	g_LabelData[id][ol_PosZ] = z;

	g_LabelData[id][ol_InteriorID] = interior;
	g_LabelData[id][ol_VirtualWorld] = virtual_world;

	g_LabelData[id][ol_AreaID] = CreateDynamicSphere(x, y, z, range, virtual_world, interior, playerid);
	g_LabelData[id][ol_ObjectID] = CreateDynamicObject(g_LabelData[id][ol_ModelID], x + g_LabelData[id][ol_ModelOffsetX], y + g_LabelData[id][ol_ModelOffsetY], z + g_LabelData[id][ol_ModelOffsetZ], 0.0, 0.0, 0.0, virtual_world, interior, playerid, 250.0, 250.0, g_LabelData[id][ol_AreaID]);

	SetDynamicObjectExtra(g_LabelData[id][ol_ObjectID], OL_EXTRA, id);
	SetDynamicAreaExtra(g_LabelData[id][ol_AreaID], OL_EXTRA, id);

	g_LabelData[id][ol_FontFace][0] = EOS;
	strcat(g_LabelData[id][ol_FontFace], font_face, 24);
	g_LabelData[id][ol_FontSize] = font_size;

	g_LabelData[id][ol_FontColor] = font_color;

	g_LabelData[id][ol_PlayerCount] = 0;

	g_LabelData[id][ol_LastMaterialIndex] = -1;

	g_LabelData[id][ol_Exists] = true;

	UpdateObjectLabelText(id, text);

	Iter_Add(g_itLabels, id);

	return id;
}

stock DestroyObjectLabel(id, bool:remove_from_iter = true)
{
	if(!IsValidObjectLabel(id))
		return 0;

	DestroyDynamicObject(g_LabelData[id][ol_ObjectID]);
	DestroyDynamicArea(g_LabelData[id][ol_AreaID]);

	g_LabelData[id][ol_Exists] = false;

	if(remove_from_iter)
		Iter_Remove(g_itLabels, id);

	return 1;
}

stock DestroyAllObjectLabels()
{
	new c = 0;

	foreach(new i : g_itLabels)
	{
		DestroyObjectLabel(i, false);

		c ++;
	}

	Iter_Clear(g_itLabels);

	return c;
}

stock IsValidObjectLabel(id)
{
	if(id < 0 || id >= MAX_OBJECT_LABELS)
		return 0;

	return _:g_LabelData[id][ol_Exists];
}

stock GetFreeObjectLabelSlot()
{
	for(new i = 0; i < MAX_OBJECT_LABELS; i ++)
	{
		if(!IsValidObjectLabel(i))
			return i;
	}

	return -1;
}

stock UpdateObjectLabelText(id, const text[] = "")
{
	if(!IsValidObjectLabel(id))
		return 0;

	if(text[0] == EOS && g_LabelData[id][ol_LastMaterialIndex] != -1)
	{
		new prev_text[OBJECT_LABEL_MAX_TEMP_TEXT], wa[1], w;

		// Get the text from the current index and then remove it (it might change)

		GetDynamicObjectMaterialText(g_LabelData[id][ol_ObjectID], g_LabelData[id][ol_LastMaterialIndex], prev_text, w, wa, w, w, w, w, w, sizeof(prev_text), sizeof(wa));
		RemoveDynamicObjectMaterialText(g_LabelData[id][ol_ObjectID], g_LabelData[id][ol_LastMaterialIndex]);

		SetDynamicObjectMaterialText(g_LabelData[id][ol_ObjectID], g_LabelData[id][ol_MaterialIndex], prev_text, g_LabelData[id][ol_MaterialSize], g_LabelData[id][ol_FontFace], g_LabelData[id][ol_FontSize], 1, g_LabelData[id][ol_FontColor], 0xFFFFFFFF, 1);

		g_LabelData[id][ol_LastMaterialIndex] = g_LabelData[id][ol_MaterialIndex];
	}
	else
	{
		SetDynamicObjectMaterialText(g_LabelData[id][ol_ObjectID], g_LabelData[id][ol_MaterialIndex], text, g_LabelData[id][ol_MaterialSize], g_LabelData[id][ol_FontFace], g_LabelData[id][ol_FontSize], 1, g_LabelData[id][ol_FontColor], 0xFFFFFFFF, 1);

		g_LabelData[id][ol_LastMaterialIndex] = g_LabelData[id][ol_MaterialIndex];
	}

	return 1;
}

stock SetObjectLabelModelData(id, modelid = OBJECT_LABEL_MODEL, material_index = OBJECT_LABEL_MATERIAL_INDEX, material_size = OBJECT_LABEL_MATERIAL_SIZE, Float:off_x = OBJECT_LABEL_OFF_X, Float:off_y = OBJECT_LABEL_OFF_Y, Float:off_z = OBJECT_LABEL_OFF_Z, Float:off_rx = OBJECT_LABEL_OFF_RX, Float:off_ry = OBJECT_LABEL_OFF_RY, Float:off_rz = OBJECT_LABEL_OFF_RZ)
{
	if(!IsValidObjectLabel(id))
		return 0;

	SetDynamicObjectModel(g_LabelData[id][ol_ObjectID], modelid);
	g_LabelData[id][ol_ModelID] = modelid;

	g_LabelData[id][ol_MaterialIndex] = material_index;
	g_LabelData[id][ol_MaterialSize] = material_size;

	g_LabelData[id][ol_ModelOffsetX] = off_x;
	g_LabelData[id][ol_ModelOffsetY] = off_y;
	g_LabelData[id][ol_ModelOffsetZ] = off_z;
	g_LabelData[id][ol_ModelOffsetRX] = off_rx;
	g_LabelData[id][ol_ModelOffsetRY] = off_ry;
	g_LabelData[id][ol_ModelOffsetRZ] = off_rz;

	UpdateObjectLabelText(id);

	return 1;
}

stock SetObjectLabelFontFace(id, const font_face[], bool:update = true)
{
	if(!IsValidObjectLabel(id))
		return 0;

	g_LabelData[id][ol_FontFace][0] = EOS;
	strcat(g_LabelData[id][ol_FontFace], font_face, 24);

	if(update)
		UpdateObjectLabelText(id);

	return 1;
}

stock SetObjectLabelFontSize(id, font_size, bool:update = true)
{
	if(!IsValidObjectLabel(id))
		return 0;

	g_LabelData[id][ol_FontSize] = font_size;

	if(update)
		UpdateObjectLabelText(id);

	return 1;
}

stock SetObjectLabelFontColor(id, font_color, bool:update = true)
{
	if(!IsValidObjectLabel(id))
		return 0;

	g_LabelData[id][ol_FontColor] = font_color;

	if(update)
		UpdateObjectLabelText(id);

	return 1;
}

stock SetObjectLabelFrontOffset(id, Float:offset = 0.0)
{
	if(!IsValidObjectLabel(id))
		return 0;

	g_LabelData[id][ol_FrontOffset] = offset;

	return 1;
}

// ---------------------------------------------------- Internal Funcs

static stock OL_GetRXFromVectorZ(Float:vz, &Float:rx)
{
	rx = -(acos(vz) - 90.0);

	return 1;
}

static stock OL_GetRZFromVectorXY(Float:vx, Float:vy, &Float:a)
{
	if(vx == 0.0 && vy == 0.0)
		return 0;

	a = atan2(vy, vx) - 90.0;

	return 1;
}

// ---------------------------------------------------- Hooks

public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid)
{
	if(g_Initialized)
	{
		new extra, id;
		GetDynamicAreaExtra(areaid, extra, id);

		if(extra == OL_EXTRA)
		{
			if(IsValidObjectLabel(id))
			{
				if(++g_PlayerLabels[playerid] == 1)
					Iter_Add(g_itActivePlayers, playerid);

				return 1;
			}
		}
	}

	#if defined OL_OnPlayerEnterDynamicArea
		return OL_OnPlayerEnterDynamicArea(playerid, areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerEnterDynamicArea
	#undef OnPlayerEnterDynamicArea
#else
	#define _ALS_OnPlayerEnterDynamicArea
#endif

#define OnPlayerEnterDynamicArea OL_OnPlayerEnterDynamicArea
#if defined OL_OnPlayerEnterDynamicArea
	forward OL_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid);
#endif


public OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA:areaid)
{
	if(g_Initialized)
	{
		new extra, id;
		GetDynamicAreaExtra(areaid, extra, id);

		if(extra == OL_EXTRA)
		{
			if(IsValidObjectLabel(id))
			{
				if(--g_PlayerLabels[playerid] == 0)
					Iter_Remove(g_itActivePlayers, playerid);

				return 1;
			}
		}
	}

	#if defined OL_OnPlayerLeaveDynamicArea
		return OL_OnPlayerLeaveDynamicArea(playerid, areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerLeaveDynamicArea
	#undef OnPlayerLeaveDynamicArea
#else
	#define _ALS_OnPlayerLeaveDynamicArea
#endif

#define OnPlayerLeaveDynamicArea OL_OnPlayerLeaveDynamicArea
#if defined OL_OnPlayerLeaveDynamicArea
	forward OL_OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA:areaid);
#endif


public OnGameModeInit()
{
	OL_Init();


	#if defined OL_OnGameModeInit
		return OL_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit OL_OnGameModeInit
#if defined OL_OnGameModeInit
	forward OL_OnGameModeInit();
#endif


public OnGameModeExit()
{
	OL_Exit();


	#if defined OL_OnGameModeExit
		return OL_OnGameModeExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif

#define OnGameModeExit OL_OnGameModeExit
#if defined OL_OnGameModeExit
	forward OL_OnGameModeExit();
#endif


public OnFilterScriptInit()
{
	OL_Init();


	#if defined OL_OnFilterScriptInit
		return OL_OnFilterScriptInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif

#define OnFilterScriptInit OL_OnFilterScriptInit
#if defined OL_OnFilterScriptInit
	forward OL_OnFilterScriptInit();
#endif


public OnFilterScriptExit()
{
	OL_Exit();


	#if defined OL_OnFilterScriptExit
		return OL_OnFilterScriptExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptExit
	#undef OnFilterScriptExit
#else
	#define _ALS_OnFilterScriptExit
#endif

#define OnFilterScriptExit OL_OnFilterScriptExit
#if defined OL_OnFilterScriptExit
	forward OL_OnFilterScriptExit();
#endif


public OnPlayerConnect(playerid)
{
	g_PlayerLabels[playerid] = 0;


	#if defined OL_OnPlayerConnect
		return OL_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect OL_OnPlayerConnect
#if defined OL_OnPlayerConnect
	forward OL_OnPlayerConnect(playerid);
#endif


public OnPlayerDisconnect(playerid, reason)
{
	if(Iter_Contains(g_itActivePlayers, playerid))
		Iter_Remove(g_itActivePlayers, playerid);


	#if defined OL_OnPlayerDisconnect
		return OL_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect OL_OnPlayerDisconnect
#if defined OL_OnPlayerDisconnect
	forward OL_OnPlayerDisconnect(playerid, reason);
#endif

// EOF
