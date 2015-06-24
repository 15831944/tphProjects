#include "StdAfx.h"
#include "ServiceObject.h"

/**
* Register an instance of a DelObject derived class for deletion.
*/
void DeletionManager::Register(DelObject* pObject)
{
	if(!s_pFirst)
	{
		s_pFirst = s_pLast = pObject;
	}
	else
	{
		s_pLast->m_pNextToDelete = pObject;
		s_pLast = pObject;
	}
}

/**
* Unregister an instance of a DelObject derived class for deletion.
*/
void DeletionManager::UnRegister(DelObject* pObject)
{
	if(!s_pFirst)
		return;

	if(pObject == s_pFirst && pObject == s_pLast)
	{
		s_pFirst = s_pLast = NULL;
	}
	else if(pObject == s_pFirst)
	{
		s_pFirst = pObject->m_pNextToDelete;
	}
	else
	{
		DelObject* pObj = s_pFirst;
		while(pObj->m_pNextToDelete != pObject && pObj != NULL)
		{
			pObj = pObj->m_pNextToDelete;
		}

		if(pObj != NULL)
		{
			pObj->m_pNextToDelete = pObject->m_pNextToDelete;
			if(pObject == s_pLast)
				s_pLast = pObj;
		}
	}
}

/**
* Delete all registered instances.
*/
void DeletionManager::DeleteAll()
{
	DelObject* pObj = s_pFirst;
	DelObject* pNext = NULL;

	while(pObj)
	{
		pNext = pObj->m_pNextToDelete;
		delete pObj;
		pObj = pNext;
	}

	s_pFirst = s_pLast = NULL;
}

DelObject* DeletionManager::s_pFirst = NULL;
DelObject* DeletionManager::s_pLast = NULL;
