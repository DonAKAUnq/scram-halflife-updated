// Copyright (c) Richard Rohac, 2014, All rights reserved.

#pragma once
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define COUNTERS_DBG_INFO 0 // unq - change to see counter console info

typedef struct sGlobalCounter_s
{
	char szName[64];
	char szLevelName[32];
	int iValue;
	bool bSetCVAR;
	sGlobalCounter_s *pNext;
} sGlobalCounter_t;


class CGlobalCounters
{
public:
	static CGlobalCounters* Instance()
	{
	   if(!m_pInstance)
		  m_pInstance = new CGlobalCounters;

	   return m_pInstance;
	};

	void Reset( void )
	{
		m_pList = NULL; 
		m_iCounters = 0;
	};

	// add new counter to linked list, starting with 0
	void EntityAdd( string_t sCounterName, string_t sMapName, bool bCVAR, bool bForceValue = false, int iForcedVal = 0)
	{
		sGlobalCounter_t *pFound = Find(sCounterName);
		if(pFound)
		{
#ifdef COUNTERS_DBG_INFO
			ALERT(at_console, "Counter (%s) already exists, with (%i) value!\n", STRING(sCounterName), pFound->iValue );
#endif
			if(pFound->bSetCVAR)
				CVAR_SET_FLOAT("sv_numtokens", pFound->iValue);

			return;
		}

		sGlobalCounter_t *pNewEntity = (sGlobalCounter_t *)calloc(sizeof(sGlobalCounter_t), 1);
		ASSERT(pNewEntity != NULL);

		pNewEntity->pNext = m_pList;
		m_pList = pNewEntity;
		strcpy(pNewEntity->szName, STRING(sCounterName));
		strcpy(pNewEntity->szLevelName, STRING(sMapName));

		if(bForceValue)
			pNewEntity->iValue = iForcedVal;
		else
			pNewEntity->iValue = 0;

		pNewEntity->bSetCVAR = bCVAR;

		m_iCounters++;

#ifdef COUNTERS_DBG_INFO
		ALERT(at_console, "Added new counter (%s) with (%i) value!\n", STRING(sCounterName), pNewEntity->iValue);
#endif

		if(pNewEntity->bSetCVAR)
			CVAR_SET_FLOAT("sv_numtokens", pNewEntity->iValue);
	};

	void EntitySetState( string_t sCounterName, int iNewValue )
	{
		sGlobalCounter_t *pEntity = Find(sCounterName);

		if(pEntity)
		{
			pEntity->iValue = iNewValue;

#ifdef COUNTERS_DBG_INFO
			ALERT(at_console, "Counter (%s) set with (%i) value!\n", STRING(sCounterName), iNewValue);
#endif

			if(pEntity->bSetCVAR)
				CVAR_SET_FLOAT("sv_numtokens", pEntity->iValue);
		}
	};

	int	EntityGetState( string_t sCounterName )
	{
		sGlobalCounter_t *pEntity = Find(sCounterName);
		if(pEntity)
			return pEntity->iValue;

		return 0;
	};

	int Save( CSave &save );
	int Restore( CRestore &restore );

	void ClearStates( void )
	{
		sGlobalCounter_t *pFree = m_pList;
		while (pFree)
		{
			sGlobalCounter_t *pNext = pFree->pNext;
			free(pFree);
			pFree = pNext;
		}
		Reset();
	};


	static TYPEDESCRIPTION m_SaveData[];

private:
	CGlobalCounters()
	{
		Reset();
	};

	~CGlobalCounters()
	{
		ClearStates(); 
	};

	CGlobalCounters(CGlobalCounters const&){};
	CGlobalCounters& operator=(CGlobalCounters const&){};

	sGlobalCounter_t *Find( string_t sCounterName )
	{
		if(!sCounterName)
			return NULL;

		sGlobalCounter_t *pTest;
		const char *pEntityName = STRING(sCounterName);

		pTest = m_pList;
		while(pTest)
		{
			if(FStrEq(pEntityName, pTest->szName))
				break;
	
			pTest = pTest->pNext;
		}

		return pTest;
	};

	sGlobalCounter_t *m_pList;
	int m_iCounters;

	static CGlobalCounters *m_pInstance;
};

#endif