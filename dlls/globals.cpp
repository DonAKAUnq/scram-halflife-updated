/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== globals.cpp ========================================================

  DLL-wide global variable definitions.
  They're all defined here, for convenient centralization.
  Source files that need them should "extern ..." declare each
  variable, to better document what globals they care about.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "soundent.h"
#include "globals.h" // richard & unq


DLL_GLOBAL ULONG		g_ulFrameCount;
DLL_GLOBAL ULONG		g_ulModelIndexEyes;
DLL_GLOBAL ULONG		g_ulModelIndexPlayer;
DLL_GLOBAL Vector		g_vecAttackDir;
DLL_GLOBAL int			g_iSkillLevel;
DLL_GLOBAL int			gDisplayTitle;
DLL_GLOBAL BOOL			g_fGameOver;
DLL_GLOBAL const Vector	g_vecZero = Vector(0,0,0);
DLL_GLOBAL int			g_Language;

//start richard & unq, global counter code
CGlobalCounters* CGlobalCounters::m_pInstance = NULL;

TYPEDESCRIPTION	gGlobalCounterSaveData[] =
{
	DEFINE_ARRAY(sGlobalCounter_t, szName, FIELD_CHARACTER, 64),
	DEFINE_ARRAY(sGlobalCounter_t, szLevelName, FIELD_CHARACTER, 32),
	DEFINE_FIELD(sGlobalCounter_t, iValue, FIELD_INTEGER),
	DEFINE_FIELD(sGlobalCounter_t, bSetCVAR, FIELD_BOOLEAN),
};


TYPEDESCRIPTION	CGlobalCounters::m_SaveData[] =
{
	DEFINE_FIELD(CGlobalCounters, m_iCounters, FIELD_INTEGER),
};

int CGlobalCounters::Save(CSave& save)
{
	sGlobalCounter_t* pEntity;

	if (!save.WriteFields("CNTGLOBAL", this, m_SaveData, ARRAYSIZE(m_SaveData)))
		return 0;

	pEntity = m_pList;
	for (int i = 0; i < m_iCounters && pEntity; i++)
	{
		if (!save.WriteFields("CNTGENT", pEntity, gGlobalCounterSaveData, ARRAYSIZE(gGlobalCounterSaveData)))
			return 0;

		pEntity = pEntity->pNext;
	}

	return 1;
};

int CGlobalCounters::Restore(CRestore& restore)
{
	int iListCount;
	sGlobalCounter_t tmpEntity;

	ClearStates();
	if (!restore.ReadFields("CNTGLOBAL", this, m_SaveData, ARRAYSIZE(m_SaveData)))
		return 0;

	iListCount = m_iCounters;
	m_iCounters = 0;

	for (int i = 0; i < iListCount; i++)
	{
		if (!restore.ReadFields("CNTGENT", &tmpEntity, gGlobalCounterSaveData, ARRAYSIZE(gGlobalCounterSaveData)))
			return 0;

		EntityAdd(MAKE_STRING(tmpEntity.szName), MAKE_STRING(tmpEntity.szLevelName), tmpEntity.bSetCVAR, true, tmpEntity.iValue);
	}
	return 1;
}

#define SF_GLBCNT_HUD	1

class CEnvGlobalCounter : public CPointEntity
{
public:
	void	Spawn(void);
	void	KeyValue(KeyValueData* pkvd);
	void	Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);

	virtual int		Save(CSave& save);
	virtual int		Restore(CRestore& restore);

	static	TYPEDESCRIPTION m_SaveData[];

	string_t	m_sCounterName;
	int			m_iChangeMode;
	int			m_iValue;
};

TYPEDESCRIPTION CEnvGlobalCounter::m_SaveData[] =
{
	DEFINE_FIELD(CEnvGlobalCounter, m_sCounterName, FIELD_STRING),
	DEFINE_FIELD(CEnvGlobalCounter, m_iChangeMode, FIELD_INTEGER),
	DEFINE_FIELD(CEnvGlobalCounter, m_iValue, FIELD_INTEGER),
};

LINK_ENTITY_TO_CLASS(env_global_counter, CEnvGlobalCounter);

int CEnvGlobalCounter::Save(CSave& save)
{
	if (!CPointEntity::Save(save))
		return 0;
	return save.WriteFields("CEnvGlobalCounter", this, m_SaveData, ARRAYSIZE(m_SaveData));
};

int CEnvGlobalCounter::Restore(CRestore& restore)
{
	if (!CPointEntity::Restore(restore))
		return 0;

	int iRet = restore.ReadFields("CEnvGlobalCounter", this, m_SaveData, ARRAYSIZE(m_SaveData));
	return iRet;
};

void CEnvGlobalCounter::KeyValue(KeyValueData* pkvd)
{
	pkvd->fHandled = TRUE;

	if (FStrEq(pkvd->szKeyName, "countername"))
		m_sCounterName = ALLOC_STRING(pkvd->szValue);
	else if (FStrEq(pkvd->szKeyName, "modiftype"))
		m_iChangeMode = atoi(pkvd->szValue);
	else if (FStrEq(pkvd->szKeyName, "counterval"))
		m_iValue = atoi(pkvd->szValue);
	else
		CPointEntity::KeyValue(pkvd);
}

void CEnvGlobalCounter::Spawn(void)
{
	if (!m_sCounterName)
	{
		ALERT(at_console, "Global counter error: no counter name specified!!!\n");
		REMOVE_ENTITY(ENT(pev));
		return;
	}

	CGlobalCounters::Instance()->EntityAdd(m_sCounterName, gpGlobals->mapname, (pev->spawnflags & SF_GLBCNT_HUD) ? true : false);
}

void CEnvGlobalCounter::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	int iOldValue = CGlobalCounters::Instance()->EntityGetState(m_sCounterName);
	int iNewValue;

	switch (m_iChangeMode)
	{
	case 0:
		iNewValue = m_iValue;
		break;

	case 1:
		iNewValue = iOldValue + m_iValue;
		break;

	default:
		break;
	}

	CGlobalCounters::Instance()->EntitySetState(m_sCounterName, iNewValue);
}// end richard & unq