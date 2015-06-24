#include "StdAfx.h"
#include ".\simflowmobitem.h"
#include "person.h"
#include "visitor.h"
#include "..\Common\elaptime.h"

CSimFlowMobItem::CSimFlowMobItem(void)
:m_pMobileSyncState(NULL)
{
}

CSimFlowMobItem::~CSimFlowMobItem(void)
{
	if (m_pMobileSyncState)
	{
		delete m_pMobileSyncState;
		m_pMobileSyncState = NULL;
	}
}

void CSimFlowMobItem::SetMobileState( MobileSyncState* pMobileSyncState )
{
	m_pMobileSyncState = pMobileSyncState;
}

MobileSyncState* CSimFlowMobItem::GetMobileState() const
{
	return m_pMobileSyncState;
}

void CSimFlowMobItem::SetSyncProcID( const ProcessorID& syncProcID )
{
	m_syncProcID = syncProcID;
}

const ProcessorID& CSimFlowMobItem::GetSyncProcID() const
{
	return m_syncProcID;
}


CSimFlowPaxItem::CSimFlowPaxItem( void )
:m_bReady(false)
{

}

CSimFlowPaxItem::~CSimFlowPaxItem( void )
{
	ClearData();
}

int CSimFlowPaxItem::GetCount() const
{
	return (int)m_vNonPaxSyncItem.size();
}

void CSimFlowPaxItem::AddItem( CSimFlowMobItem* pNonPaxSyncItem )
{
	return m_vNonPaxSyncItem.push_back(pNonPaxSyncItem);
}

void CSimFlowPaxItem::DeleteItem( CSimFlowMobItem* pNonPaxSyncItem )
{
	std::vector<CSimFlowMobItem*>::iterator iter = m_vNonPaxSyncItem.begin();
	iter = std::find(m_vNonPaxSyncItem.begin(),m_vNonPaxSyncItem.end(),pNonPaxSyncItem);
	if (iter != m_vNonPaxSyncItem.end())
	{
		m_vNonPaxSyncItem.erase(iter);
		delete pNonPaxSyncItem;
	}
}

CSimFlowMobItem* CSimFlowPaxItem::GetItem( int idx ) const
{
	ASSERT(idx >= 0 && idx < GetCount());

	if (idx >= 0 && idx < GetCount())
	{
		return m_vNonPaxSyncItem.at(idx);
	}
	return NULL;
}

bool CSimFlowPaxItem::IsExsit( Person* pPerson ) const
{
	for (unsigned i = 0; i < m_vNonPaxSyncItem.size(); i++)
	{
		CSimFlowMobItem* pItem = m_vNonPaxSyncItem.at(i);
		if (pItem == NULL)
			continue;
		
		MobileSyncState* pMobState = pItem->GetMobileState();
		if (pMobState == NULL)
			continue;
		
		if (pMobState->m_pMobileElement == pPerson)
			return true;
	}
	return false;
}

bool CSimFlowPaxItem::GetCanGoFurther(const ElapsedTime& curTime )
{
	for (unsigned i = 0; i < m_vNonPaxSyncItem.size(); i++)
	{
		CSimFlowMobItem* pItem = m_vNonPaxSyncItem.at(i);
		if (pItem->GetMobileState()->m_bReady == true)
		{
			return true;
		}
	}
	return false;
}

void CSimFlowPaxItem::notifyNonPaxLeave( const ElapsedTime& curTime )
{
	for (unsigned i = 0; i < m_vNonPaxSyncItem.size(); i++)
	{
		CSimFlowMobItem* pItem = m_vNonPaxSyncItem.at(i);
		if (pItem->GetMobileState()->m_bReady == true)
		{
			Person* pPerson = pItem->GetMobileState()->m_pMobileElement;
			if (pPerson)
			{
				pPerson->generateEvent(curTime,false);
			}
		}
	}
}

void CSimFlowPaxItem::SetSyncProcID( const ProcessorID& syncProcID )
{
	m_syncProcID = syncProcID;
}

const ProcessorID& CSimFlowPaxItem::GetSyncProcID() const
{
	return m_syncProcID;
}

CSimFlowMobItem* CSimFlowPaxItem::GetSyncVisitor( Person* pVisitor,const ProcessorID& procID )
{
	CSimFlowMobItem* pBestItem = NULL;
	for (unsigned i = 0; i < m_vNonPaxSyncItem.size(); i++)
	{
		CSimFlowMobItem* pItem = m_vNonPaxSyncItem.at(i);
		MobileSyncState* pVisitorState = pItem->GetMobileState();
		if (pVisitorState->m_pMobileElement == pVisitor && pItem->GetSyncProcID().idFits(procID))
		{
			if (pItem->GetSyncProcID() == procID)
			{
				return pItem;
			}

			if (pBestItem == NULL)
			{
				pBestItem = pItem;
				continue;
			}
			
			if (pBestItem->GetSyncProcID().idFits(pItem->GetSyncProcID()))
			{
				pBestItem = pItem;
			}
		}
	}
	return pBestItem;
}

void CSimFlowPaxItem::SetState( bool bState )
{
	m_bReady = bState;
}

bool CSimFlowPaxItem::GetState() const
{
	return m_bReady;
}

bool CSimFlowPaxItem::NeedSync() const
{
	if (m_vNonPaxSyncItem.empty())
		return false;
	
	return true;
}

void CSimFlowPaxItem::ClearData()
{
	for (unsigned i = 0; i < m_vNonPaxSyncItem.size(); i++)
	{
		delete m_vNonPaxSyncItem[i];
	}
	m_vNonPaxSyncItem.clear();
}

bool CSimFlowPaxItem::available()const
{
	std::vector<ProcessorID> vIDs;
	for (unsigned i = 0; i < m_vNonPaxSyncItem.size(); i++)
	{
		CSimFlowMobItem* pItem = m_vNonPaxSyncItem.at(i);
		if(std::find(vIDs.begin(),vIDs.end(),pItem->GetSyncProcID()) == vIDs.end())
		{
			vIDs.push_back(pItem->GetSyncProcID());
		}
	}

	if (vIDs.size() == m_vNonPaxSyncItem.size())
	{
		return true;
	}

	return false;
}
