#include "StdAfx.h"
#include "SimFlowSyncItem.h"
#include "Person.h"
#include "GroupLeaderInfo.h"
#include "TerminalMobElementBehavior.h"
CSimFlowSyncItem::CSimFlowSyncItem(void)
{
}

CSimFlowSyncItem::~CSimFlowSyncItem(void)
{
}

//check if have entry in the flowSync data
// TODO
bool CSimFlowSyncItem::NeedToSync(Person* pMobElement, const ProcessorID& procID)
{
	// check for passenger.
	if( m_paxProcID.idFits( procID ) == false)
	{
		// did not match the pax
		for( int i=0; i< (int)m_vectNonPaxSyncItem.size(); i++ )
		{
			if( m_vectNonPaxSyncItem[i].nonPaxProcID.idFits( procID ) )
				break;
		}
		if( i == m_vectNonPaxSyncItem.size() )
			return false;
	}

		
	for (int i=0; i<(int)m_vectAllPaxState.size(); i++)
	{
		if (m_vectAllPaxState[i].pMobileElement == pMobElement)
			return true;
	}

	return false;
}

bool CSimFlowSyncItem::CanIGoFurther(Person* pMobElement, const ProcessorID& procID, const ElapsedTime& curTime)
{
	if (false == NeedToSync(pMobElement, procID))
		return true;
	
	TerminalMobElementBehavior* spTerminalBehavior = pMobElement->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return false;
	}

	//Need to check if all leader and follower in the header of the queue
	if(false == spTerminalBehavior->ifGroupOccupyHeadsOfQueue())
		return false;
	
	//TRACE("\nMobElement ID:%d, Is Follower: %d ",pMobElement->getID(),
	//	pMobElement->m_pGroupInfo->IsFollower());

	//set this person and all it's follower to be ready
	for (std::vector<AllMobileElementState>::iterator iter = m_vectAllPaxState.begin();
		iter != m_vectAllPaxState.end(); iter++)
	{
		if (iter->pMobileElement == pMobElement)
		{
			//update itself
			iter->bWait = true;
			iter->bReady = true;

			//if is leader, need update all it's follower
			if (false == pMobElement->m_pGroupInfo->IsFollower())
			{
				CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pMobElement->m_pGroupInfo;
				if(true == pGroupLeaderInfo->isInGroup())
				{
					//Set all follower state bReady == true
					MobileElementList& followerList = pGroupLeaderInfo->GetFollowerList();

					for (std::vector<AllMobileElementState>::iterator iter = m_vectAllPaxState.begin();
						iter != m_vectAllPaxState.end(); iter++)
					{
						if (followerList.Find(iter->pMobileElement) != INT_MAX)
							iter->bReady = true;			
					}
				}
			}
		}
	}


	//check if all family ready
//	bool bReady = true;
//	std::vector<AllMobileElementState>::iterator iterCheckingPax = m_vectAllPaxState.end();
	for (std::vector<AllMobileElementState>::iterator iter = m_vectAllPaxState.begin();
		iter != m_vectAllPaxState.end(); iter++)
	{
	//	TRACE("\nMobElement ID:%d, Is Follower: %d \t\tReady State: %d, Wait State: %d",
	//		iter->pMobileElement->getID(),pMobElement->m_pGroupInfo->IsFollower(),
	//		iter->bReady, iter->bWait);
		if (false == iter->bReady)
				return false;
	}

	
	NotifyPaxToGoFurther(curTime);
	
	return true;
}

void CSimFlowSyncItem::NotifyPaxToGoFurther(const ElapsedTime& curTime)
{
	for (std::vector<AllMobileElementState>::iterator iter = m_vectAllPaxState.begin();
		iter != m_vectAllPaxState.end(); iter++)
	{
		//wait state 
		if (iter->bWait == true)
		{
			Person* pPerson = iter->pMobileElement;
			pPerson->generateEvent(curTime, false);
		}
		iter->bWait = false;
	}
}
