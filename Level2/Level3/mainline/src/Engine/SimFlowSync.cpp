#include "StdAfx.h"
#include "SimFlowSync.h"
#include "GroupLeaderInfo.h"

#include "Pax.h"
#include "SimFlowMobItem.h"
#include "../Common/ARCTracker.h"

ARCSyncUnavailableError::ARCSyncUnavailableError( const CString& _strMobType, const CString& _strProcName, const CString& _strOtherMsg /*= ""*/, const CString& _strTime /*= "" */ )
:ARCFlowError( _strMobType, _strProcName, _strOtherMsg, _strTime )
{
	m_strErrorType = "SYNCHRONIZATION POINT UNAVAILABLE";
}

ARCSyncUnavailableError::~ARCSyncUnavailableError()
{

}

CString ARCSyncUnavailableError::getErrorMsg()
{
	CString strReturn;
	strReturn.Format("runtime error: pax type %s have not available synchronization point at processor %s ", m_strMobType, m_strProcName );

	if( !m_strOtherMsg.IsEmpty() )
		strReturn = strReturn + "(" + m_strOtherMsg + ")";

	return strReturn;
}

FORMATMSG ARCSyncUnavailableError::getFormatErrorMsg( void )
{
	FORMATMSG formatMsg;

	formatMsg.strTime = m_strErrorTime;
	formatMsg.strMsgType = m_strErrorType;
	formatMsg.strMobType = m_strMobType;
	formatMsg.strProcessor = m_strProcName;
	formatMsg.strOtherInfo = m_strOtherMsg;

	formatMsg.diag_entry = m_diagEntry;
	return formatMsg;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSimFlowSync::CSimFlowSync()
{

}

CSimFlowSync::~CSimFlowSync()
{
	ClearData();
}



void CSimFlowSync::LoadData( Passenger* pPassenger, CInputFlowSync* pInputFlowSync )
{
	ClearData();

	if (pInputFlowSync == NULL)
		return;
	
	if (pPassenger == NULL)
		return;
	
	InputFlowSyncItemList syncList;
	if(!pInputFlowSync->GetFlowPaxSync(pPassenger->getType(),syncList))
		return;

	m_pPax = pPassenger;
	for (unsigned i = 0; i < syncList.size(); i++)
	{
		CSimFlowPaxItem* pPaxSyncItem = CreateInstance(pPassenger,syncList.at(i));
		if (pPaxSyncItem == NULL)
			continue;
		if (!pPaxSyncItem->available())
		{
			ClearData();
			CString strMobType;
			TerminalMobElementBehavior* pPaxTerminalMobBehavior = m_pPax->getTerminalBehavior();
			if (pPaxTerminalMobBehavior)
			{
				strMobType = pPaxTerminalMobBehavior->getPersonErrorMsg();
				CString strErrorMsg;
				strErrorMsg.Format(_T("Passenger cann't synchronize with more than one type of visitor"));
				throw new ARCSyncUnavailableError(strMobType,pPaxSyncItem->GetSyncProcID().GetIDString(),strErrorMsg);
			}
		}
		m_vectSimFlowSyncItem.push_back(pPaxSyncItem);
	}
}

CSimFlowPaxItem* CSimFlowSync::CreateInstance(Passenger* pPassenger,CInputFlowPaxSyncItem* pPaxSyncItem)
{
	CSimFlowPaxItem* pPaxFlowSyncItem = new CSimFlowPaxItem();
	pPaxFlowSyncItem->SetSyncProcID(pPaxSyncItem->GetProcID());

	for (unsigned i = 0; i < pPassenger->m_pVisitorList.size(); i++)
	{
		PaxVisitor* paxVisitor = pPassenger->m_pVisitorList[i];
		if(paxVisitor == NULL)
			continue;
		
		const CMobileElemConstraint& nonPaxType = paxVisitor->getType();
		
		std::vector<CInputFlowSyncItem*> nonSyncList;
		pPaxSyncItem->GetFittestNonSyncItem(nonPaxType,nonSyncList);
		for (unsigned j = 0; j < nonSyncList.size(); j++)
		{
			CInputFlowSyncItem* pItem = nonSyncList.at(j);
			if (pItem)
			{
				CSimFlowMobItem* pNonSyncItem = new CSimFlowMobItem();
				MobileSyncState* pNonPaxState = new MobileSyncState(paxVisitor);
				pNonSyncItem->SetSyncProcID(pItem->GetProcID());
				pNonSyncItem->SetMobileState(pNonPaxState);
				pPaxFlowSyncItem->AddItem(pNonSyncItem);
			}

		}
	}
	return pPaxFlowSyncItem;
}

extern long eventNumber;
bool CSimFlowSync::ProcessPassengerSync(const ProcessorID& procID, const ElapsedTime& curTime)
{
	CSimFlowPaxItem* pBestItem = NULL;
	for (unsigned i = 0; i < m_vectSimFlowSyncItem.size(); i++)
	{
		CSimFlowPaxItem* pPaxItem = m_vectSimFlowSyncItem.at(i);
 		if (pPaxItem->NeedSync() == false)
			continue;
 		
		if (pPaxItem->GetSyncProcID().idFits(procID))
		{
			if (pPaxItem->GetSyncProcID() == procID)
			{
				pBestItem = pPaxItem;
				break;
			}

			if (pBestItem == NULL)
			{
				pBestItem = pPaxItem;
				continue;
			}

			if (pBestItem->GetSyncProcID().idFits(pPaxItem->GetSyncProcID()))
			{
				pBestItem = pPaxItem;
			}
		}
	}
	if (pBestItem)
	{
		pBestItem->SetState(true);
		if (pBestItem->GetCanGoFurther(curTime))
		{
		 	pBestItem->notifyNonPaxLeave(curTime);
		 	DeleteItem(pBestItem);
		 	return true;
		}
		return false;
	}
	return true;
}

bool CSimFlowSync::ProcessVisitorSync( Person* pVisitor,const ProcessorID& procID, const ElapsedTime& curTime )
{
	CSimFlowPaxItem* pBestPaxItem = NULL;
	CSimFlowMobItem* pBestNonPaxItem = NULL;
	for (unsigned i = 0; i < m_vectSimFlowSyncItem.size(); i++)
	{
		CSimFlowPaxItem* pPaxItem = m_vectSimFlowSyncItem.at(i);
		
		CSimFlowMobItem* pNonPaxItem = pPaxItem->GetSyncVisitor(pVisitor,procID);
		if(pNonPaxItem)
		{
			if (procID == pNonPaxItem->GetSyncProcID())
			{
				pBestNonPaxItem = pNonPaxItem;
				pBestPaxItem = pPaxItem;
				break;
			}

			if (pBestNonPaxItem == NULL)
			{
				pBestNonPaxItem = pNonPaxItem;
				pBestPaxItem = pPaxItem;
				continue;
			}

			if (pBestNonPaxItem->GetSyncProcID().idFits(pNonPaxItem->GetSyncProcID()))
			{
				pBestNonPaxItem = pNonPaxItem;
				pBestPaxItem = pPaxItem;
			}
		}
	}

	if (pBestNonPaxItem && pBestPaxItem)
	{
		pBestNonPaxItem->GetMobileState()->m_bReady = true;
		if (pBestPaxItem->GetState())
		{
			noticefyLeave(m_pPax,curTime);
			DeleteItem(pBestPaxItem);
			return true;
		}
		return false;
	}
	return true;
}

void CSimFlowSync::MakePassengerSyncAvailable( const ElapsedTime& curTime )
{
	PLACE_METHOD_TRACK_STRING();
	for (unsigned i = 0; i < m_vectSimFlowSyncItem.size(); i++)
	{
		CSimFlowPaxItem* pPaxItem = m_vectSimFlowSyncItem.at(i);
		if (pPaxItem == NULL)
			continue;
		
		if (pPaxItem->NeedSync() == false)
			continue;
	
		if (pPaxItem->GetCanGoFurther(curTime))
		{
			pPaxItem->notifyNonPaxLeave(curTime);
		}
	}
	ClearData();
}


void CSimFlowSync::MakeVisitorSyncAvailable( Person* pVisitor,const ElapsedTime& curTime )
{
	PLACE_METHOD_TRACK_STRING();
	std::vector<CSimFlowPaxItem*> vInvalidPaxItem;
	for (unsigned i = 0; i < m_vectSimFlowSyncItem.size(); i++)
	{
		CSimFlowPaxItem* pPaxItem = m_vectSimFlowSyncItem.at(i);
		if (pPaxItem == NULL)
			continue;
		
		std::vector<CSimFlowMobItem*> vInvalidMobItem;
		for (int j = 0; j < pPaxItem->GetCount(); j++)
		{
			CSimFlowMobItem* pNonPaxItem = pPaxItem->GetItem(j);
			if (pNonPaxItem == NULL)
				continue;
			
			MobileSyncState* pMobileSyncState = pNonPaxItem->GetMobileState();
			if (pMobileSyncState && pMobileSyncState->m_pMobileElement == pVisitor)
			{
				vInvalidMobItem.push_back(pNonPaxItem);
				if (pPaxItem->GetState())
				{
					noticefyLeave(m_pPax,curTime);
					vInvalidPaxItem.push_back(pPaxItem);
				}
			}
		}
		//Delete invalid mob
		for (int j = 0; j < (int)vInvalidMobItem.size(); j++)
		{
			pPaxItem->DeleteItem(vInvalidMobItem.at(j));
		}
		vInvalidMobItem.clear();
	}

	//Delete invalid pax
	for (int i = 0; i < static_cast<int>(vInvalidPaxItem.size()); i++)
	{
		DeleteItem(vInvalidPaxItem.at(i));
	}
	vInvalidPaxItem.clear();
}

void CSimFlowSync::noticefyLeave(Person* pPerson, const ElapsedTime& curTime )
{
	if (pPerson == NULL)
		return;
	
	pPerson->generateEvent(curTime,false);
	
}

void CSimFlowSync::ClearData()
{
	for (unsigned i = 0; i < m_vectSimFlowSyncItem.size(); i++)
	{
		delete m_vectSimFlowSyncItem[i];
	}
	m_vectSimFlowSyncItem.clear();
}

void CSimFlowSync::DeleteItem(CSimFlowPaxItem* pItem)
{
	if (pItem == NULL)
		return;
	
	std::vector<CSimFlowPaxItem*>::iterator iter = std::find(m_vectSimFlowSyncItem.begin(),m_vectSimFlowSyncItem.end(),pItem);
	if (iter != m_vectSimFlowSyncItem.end())
	{
		delete *iter;
		m_vectSimFlowSyncItem.erase(iter);
	}
}

//void CSimFlowSync::LoadData(Passenger* pPassenger, CInputFlowSync* pInputFlowSync)
//{
//	// get all input syncitem relate to this pax type
//	std::vector<int> vIndex;
//	pInputFlowSync->GetItemsFromPaxType(vIndex, pPassenger->getType());
//
//	if( vIndex.size() == 0 )
//		return;
//
//	
//	for (int i=0; i<(int)vIndex.size(); i++)
//	{
//		CInputFlowSyncItem inputItem = pInputFlowSync->GetFlowSyncItem(vIndex[i]);
//		
//		// check if available in SimSyncData already.
//		BOOL bFind = FALSE;
//		for (int j=0; j<(int)m_vectSimFlowSyncItem.size(); j++)
//		{
//
//			if (inputItem.m_syncPaxProcID == m_vectSimFlowSyncItem[j].m_paxProcID)
//			{
//				// find the proc id is the same
//				bFind = TRUE;
//
//
//				if( m_vectSimFlowSyncItem[j].m_paxConstraint.isEqual( &inputItem.m_paxType ) == false ) 
//				{
//					// not the same pax type
//					// check if the paxType is more detail than the current one.
//					if( m_vectSimFlowSyncItem[j].m_paxConstraint.fits( inputItem.m_paxType ))					
//					{
//						// replace the current record.
//						m_vectSimFlowSyncItem[j].m_paxConstraint = inputItem.m_paxType;
//						m_vectSimFlowSyncItem[j].m_vectNonPaxSyncItem.clear();
//					}
//					else
//					{
//						// the new paxtype is less detail so don't add to record
//						break;
//					}
//				}
//				NonPaxSyncItem nonPaxItem;
//				nonPaxItem.nonPaxIndex = inputItem.m_nonPaxTypeIndex;
//				nonPaxItem.nonPaxProcID = inputItem.m_syncNonPaxProcID;
//				m_vectSimFlowSyncItem[j].m_vectNonPaxSyncItem.push_back(nonPaxItem);	
//			}
//		}
//		if (bFind)
//			continue;
//			
//		//paxProcId 's first SimFlowSyncItem
//		CSimFlowSyncItem simItem;
//		simItem.m_paxProcID = inputItem.m_syncPaxProcID;
//		simItem.m_paxConstraint = inputItem.m_paxType;
//		
//		NonPaxSyncItem nonPaxItem;
//		nonPaxItem.nonPaxIndex = inputItem.m_nonPaxTypeIndex;
//		nonPaxItem.nonPaxProcID = inputItem.m_syncNonPaxProcID;
//		simItem.m_vectNonPaxSyncItem.push_back(nonPaxItem);
//
//		m_vectSimFlowSyncItem.push_back(simItem);
//	}
//
//	// set the state variable
//	for (int i=0; i<(int)m_vectSimFlowSyncItem.size(); i++)
//	{
//		// leader of pax
//
//		AllMobileElementState syncState;
//		syncState.pMobileElement = pPassenger;  
//		m_vectSimFlowSyncItem[i].m_vectAllPaxState.push_back(syncState);
//		
//		// follower of pax
//		MobileElementList& followerList = 
//			((CGroupLeaderInfo*)pPassenger->m_pGroupInfo)->GetFollowerList();
//		for (int j=0 ;j<(int)followerList.size(); j++)
//		{
//			AllMobileElementState followerSyncState;
//			followerSyncState.pMobileElement = (Person*)followerList[j];
//			m_vectSimFlowSyncItem[i].m_vectAllPaxState.push_back( followerSyncState );
//		}
//
//		// non pax
//		for(int j=0; j<(int)pPassenger->m_pVisitorList.size(); j++)
//		{
// 			PaxVisitor* paxVisitor = pPassenger->m_pVisitorList[j];
//			if(paxVisitor == NULL)
//				continue;
//
// 			int nNonPaxTypeIndex = paxVisitor->getType().GetTypeIndex();
//			//
//			for (int k = 0; k < (int)m_vectSimFlowSyncItem[i].m_vectNonPaxSyncItem.size(); k++)
//			{
//				if (m_vectSimFlowSyncItem[i].m_vectNonPaxSyncItem[k].nonPaxIndex == nNonPaxTypeIndex )
//				{
//					AllMobileElementState NonPaxSyncState;
//					NonPaxSyncState.pMobileElement = paxVisitor;
//					m_vectSimFlowSyncItem[i].m_vectAllPaxState.push_back( NonPaxSyncState );
//					break;
//				}
//			}
//		}
//	}
//}
//
//
//bool CSimFlowSync::CanIGoFurther(Person* pMobElement, const ProcessorID& procID, const ElapsedTime& curTime)
//{
//	for (std::vector<CSimFlowSyncItem>::iterator iter = m_vectSimFlowSyncItem.begin();
//		iter != m_vectSimFlowSyncItem.end(); iter++)
//	{
//		if(false == iter->CanIGoFurther(pMobElement,procID, curTime))
//			return false;
//	}
//
//	return true;
//}

