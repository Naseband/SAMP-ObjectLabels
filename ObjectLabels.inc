#if defined __OLABELS_INCLUDED__
#endinput
#endif
#define __OLABELS_INCLUDED__

// ----------------------------------------------------

#if !defined MAX_OBJECT_LABELS
#define MAX_OBJECT_LABELS				1024
#endif

#if !defined OBJECT_LABEL_DEFAULT_RANGE
#define OBJECT_LABEL_DEFAULT_RANGE		25.0
#endif

#if !defined OBJECT_LABEL_INTERVAL
#define OBJECT_LABEL_INTERVAL 			200
#endif

// ----------------------------------------------------

#define OBJECT_LABEL_MODEL 				19477
#define OBJECT_LABEL_INDEX				0
#define OBJECT_LABEL_OFF_X				0.0
#define OBJECT_LABEL_OFF_Y				0.0
#define OBJECT_LABEL_OFF_Z				0.0
#define OBJECT_LABEL_OFF_RX				0.0
#define OBJECT_LABEL_OFF_RY				0.0
#define OBJECT_LABEL_OFF_RZ				270.0

// ----------------------------------------------------

#define OL_EXTRA 						-420133769

// ----------------------------------------------------

enum E_OBJECT_LABEL
{
	olabelExists,
	olabelPlayerID,

	Float:olabelX,
	Float:olabelY,
	Float:olabelZ,
	olabelInteriorID,
	olabelVirtualWorld,

	olabelFont[24],
	olabelFontSize,
	olabelColor,

	olabelNumPlayers,

	olabelObjectID,
	olabelAreaID
};
static OL_Labels[MAX_OBJECT_LABELS][E_OBJECT_LABEL];

// ----------------------------------------------------

static bool:OL_Initialized = false, OL_TimerID;

static Iterator:OL_itLabels<MAX_OBJECT_LABELS>, Iterator:OL_itActivePlayers<MAX_PLAYERS>,
	OL_PlayerLabels[MAX_PLAYERS];

// ----------------------------------------------------

forward OL_Timer();

// ----------------------------------------------------

static stock OL_Init()
{
	if(OL_Initialized) return 0;

	OL_Initialized = true;
	OL_TimerID = SetTimer("OL_Timer", OBJECT_LABEL_INTERVAL, 1);

	Iter_Clear(OL_itActivePlayers);
	for(new i = 0; i < MAX_PLAYERS; i ++) OL_PlayerLabels[i] = 0;

	return 1;
}

static stock OL_Exit()
{
	if(!OL_Initialized) return 0;

	DestroyAllObjectLabels();

	OL_Initialized = false;
	KillTimer(OL_TimerID);

	return 1;
}

// ----------------------------------------------------

public OL_Timer()
{
	new areas[25], num;

	foreach(new i : OL_itActivePlayers)
	{
		num = GetPlayerDynamicAreas(i, areas);

		if(!num) continue;

		new extra, id, objectid, Float:x, Float:y, Float:z, Float:vx, Float:vy, Float:vz, Float:rx, Float:rz, Float:dist;

		//GetPlayerCameraPos(i, x, y, z); // Slow update rate except when the player is aiming
		GetPlayerPos(i, x, y, z);

		for(new j = 0; j < num; j ++)
		{
			GetDynamicAreaExtra(areas[j], extra, id);

			if(extra == OL_EXTRA && IsValidObjectLabel(id))
			{
				objectid = Streamer_GetItemInternalID(i, STREAMER_TYPE_OBJECT, OL_Labels[id][olabelObjectID]);

				if(IsValidPlayerObject(i, objectid))
				{
					vx = OL_Labels[id][olabelX] - x;
					vy = OL_Labels[id][olabelY] - y;
					vz = OL_Labels[id][olabelZ] - z;

					dist = VectorSize(vx, vy, vz);

					if(dist == 0.0) continue;

					vx /= dist;
					vy /= dist;
					vz /= dist;

					OL_GetRXFromVectorZ(vz, rx);
					OL_GetRZFromVectorXY(vx, vy, rz);

					if(OBJECT_LABEL_OFF_RZ == 180.0) SetPlayerObjectRot(i, objectid, OBJECT_LABEL_OFF_RX - rx, OBJECT_LABEL_OFF_RY, OBJECT_LABEL_OFF_RZ + rz);
					else if(OBJECT_LABEL_OFF_RZ == 90.0) SetPlayerObjectRot(i, objectid, OBJECT_LABEL_OFF_RX, OBJECT_LABEL_OFF_RY - rx, OBJECT_LABEL_OFF_RZ + rz);
					else if(OBJECT_LABEL_OFF_RZ == 270.0) SetPlayerObjectRot(i, objectid, OBJECT_LABEL_OFF_RX, OBJECT_LABEL_OFF_RY + rx, OBJECT_LABEL_OFF_RZ + rz);
					else SetPlayerObjectRot(i, objectid, OBJECT_LABEL_OFF_RX - rx, OBJECT_LABEL_OFF_RY, OBJECT_LABEL_OFF_RZ + rz);
				}
			}
		}
	}
}

// ----------------------------------------------------

stock CreateObjectLabel(const text[], Float:x, Float:y, Float:z, Float:range = OBJECT_LABEL_DEFAULT_RANGE, const font[] = "Verdana", fontsize = 16, color = 0xFFFFFFFF, interior = -1, virtual_world = -1, playerid = -1)
{
	if(!OL_Initialized) return -1;

	if(playerid != -1 && !IsPlayerConnected(playerid)) return -1;

	new id = GetFreeObjectLabelSlot();

	if(id == -1) return -1;

	//printf("id: %d", id);

	OL_Labels[id][olabelPlayerID] = playerid;

	OL_Labels[id][olabelX] = x;
	OL_Labels[id][olabelY] = y;
	OL_Labels[id][olabelZ] = z;

	OL_Labels[id][olabelInteriorID] = interior;
	OL_Labels[id][olabelVirtualWorld] = virtual_world;

	OL_Labels[id][olabelAreaID] = CreateDynamicSphere(x, y, z, range, virtual_world, interior, playerid);
	OL_Labels[id][olabelObjectID] = CreateDynamicObject(OBJECT_LABEL_MODEL, OBJECT_LABEL_OFF_X + x, OBJECT_LABEL_OFF_Y + y, OBJECT_LABEL_OFF_Z + z, 0.0, 0.0, 0.0, virtual_world, interior, playerid, 250.0, 250.0, OL_Labels[id][olabelAreaID]);

	SetDynamicObjectExtra(OL_Labels[id][olabelObjectID], OL_EXTRA, id);
	SetDynamicAreaExtra(OL_Labels[id][olabelAreaID], OL_EXTRA, id);

	OL_Labels[id][olabelFont][0] = EOS;
	strcat(OL_Labels[id][olabelFont], font, 24);
	OL_Labels[id][olabelFontSize] = fontsize;

	OL_Labels[id][olabelColor] = color;

	OL_Labels[id][olabelNumPlayers] = 0;

	OL_Labels[id][olabelExists] = 1;

	UpdateObjectLabelText(id, text);

	Iter_Add(OL_itLabels, id);

	return id;
}

stock DestroyObjectLabel(id, bool:remove_from_iter = true)
{
	if(!IsValidObjectLabel(id)) return 0;

	DestroyDynamicObject(OL_Labels[id][olabelObjectID]);
	DestroyDynamicArea(OL_Labels[id][olabelAreaID]);

	OL_Labels[id][olabelExists] = 0;

	if(remove_from_iter) Iter_Remove(OL_itLabels, id);

	return 1;
}

stock DestroyAllObjectLabels()
{
	new c = 0;

	foreach(new i : OL_itLabels)
	{
		DestroyObjectLabel(i, false);

		c ++;
	}

	Iter_Clear(OL_itLabels);

	return c;
}

stock IsValidObjectLabel(id)
{
	if(id < 0 || id >= MAX_OBJECT_LABELS) return 0;

	return OL_Labels[id][olabelExists];
}

stock GetFreeObjectLabelSlot()
{
	for(new i = 0; i < MAX_OBJECT_LABELS; i ++) if(!IsValidObjectLabel(i)) return i;

	return -1;
}

stock UpdateObjectLabelText(id, const text[])
{
	if(!IsValidObjectLabel(id)) return 0;

	SetDynamicObjectMaterialText(OL_Labels[id][olabelObjectID], OBJECT_LABEL_INDEX, text, .fontface = OL_Labels[id][olabelFont], .fontsize = OL_Labels[id][olabelFontSize], .bold = 1, .fontcolor = OL_Labels[id][olabelColor], .backcolor = 0, .textalignment = 1);

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
	if(vx == 0.0 && vy == 0.0) return 0;

	a = atan2(vy, vx) - 90.0;

	return 1;
}

// ---------------------------------------------------- Hooks

public OnPlayerEnterDynamicArea(playerid, areaid)
{
	if(OL_Initialized)
	{
		new extra, id;
		GetDynamicAreaExtra(areaid, extra, id);

		if(extra == OL_EXTRA)
		{
			if(IsValidObjectLabel(id))
			{
				OL_PlayerLabels[playerid] ++;

				if(OL_PlayerLabels[playerid] == 1)
				{
					Iter_Add(OL_itActivePlayers, playerid);
				}

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
	forward OL_OnPlayerEnterDynamicArea(playerid, areaid);
#endif


public OnPlayerLeaveDynamicArea(playerid, areaid)
{
	if(OL_Initialized)
	{
		new extra, id;
		GetDynamicAreaExtra(areaid, extra, id);

		if(extra == OL_EXTRA)
		{
			if(IsValidObjectLabel(id))
			{
				OL_PlayerLabels[playerid] --;

				if(OL_PlayerLabels[playerid] == 0)
				{
					Iter_Remove(OL_itActivePlayers, playerid);
				}

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
	forward OL_OnPlayerLeaveDynamicArea(playerid, areaid);
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
	OL_PlayerLabels[playerid] = 0;


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
	if(Iter_Contains(OL_itActivePlayers, playerid)) Iter_Remove(OL_itActivePlayers, playerid);


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
