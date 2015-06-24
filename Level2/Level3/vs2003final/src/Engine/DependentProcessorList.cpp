#include "StdAfx.h"
#include ".\dependentprocessorlist.h"
#include "Process.h"
#include "Person.h"
#include "conveyor.h"
#include "TerminalMobElementBehavior.h"
#include "GroupLeaderInfo.h"
#include "GroupFollowerInfo.h"
#include "pax.h"

DependentProcessorList::DependentProcessorList(Processor* pProc)
:m_bEnableProcDependent(false)
,m_pProc(pProc)
{
}

DependentProcessorList::~DependentProcessorList(void)
{
}

bool DependentProcessorList::allProcessorsOccupied(Person* _person) const
{
	if( !m_bEnableProcDependent )
		return false;// do not need check

    if( m_vectDependentProc.size() == 0 )
		return false;

	ASSERT(m_pProc);
	std::vector<Processor*>::const_iterator iter = m_vectDependentProc.begin();

	for (; iter != m_vectDependentProc.end(); iter++)
	{
		const Processor* pDependentProc = *iter;

		//check with loader conveyor
		if (pDependentProc->getProcessorType() == ConveyorProc )
		{
			Conveyor* pConveyor = (Conveyor*)pDependentProc;

			if (pConveyor->GetSubConveyorType() == LOADER)
			{
				if( CheckLoaderVacant(pConveyor,_person))
					return false;
			}
			else
			{
				if (pDependentProc->isVacant(_person))
					return false;
			}
			
		}

		if (pDependentProc->getProcessorType() != ConveyorProc)
		{
			if (pDependentProc->isVacant(_person))
				return false;
			
		}
		
	}

	return true;

}

void DependentProcessorList::addDependentProcessor(Processor* _processor)
{
	m_vectDependentProc.push_back( _processor );

}

void DependentProcessorList::AddWaitingPairInDependentProc(const WaitingPair& _wait)
{
	std::vector<Processor*>::iterator iter = m_vectDependentProc.begin();

	for (; iter != m_vectDependentProc.end(); iter++)
	{
		Processor* pDependentProc = *iter;
		pDependentProc->AddWaitingPairInDependentProc( _wait );
	}
}

//retrieve check baggage count
int DependentProcessorList::GetCheckBaggageCount(Person* _person)const
{
	int nCount = 0;
	if (false == _person->m_pGroupInfo->IsFollower())//leader
	{
		if (_person->getType().GetTypeIndex() == 2)
		{
			nCount++;
		}

		CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)_person->m_pGroupInfo;
		if(true == pGroupLeaderInfo->isInGroup())
		{
			MobileElementList& followerList = pGroupLeaderInfo->GetFollowerList();
			for (int i=0 ; i<(int)followerList.size(); i++)
			{
				Person* pFollower = (Person*)followerList[i];
				if (_person->getType().GetTypeIndex() == 2)
				{
					nCount++;
				}
			}

			//NonPax do not has visitors
			if(_person->getType().GetTypeIndex() != 0)
				return nCount;

			//NonPax
			int nVisitorCount = ((Passenger*)_person)->m_pVisitorList.size();
			for(int j=0; j<nVisitorCount; j++)
			{
				PaxVisitor* paxVisitor = ((Passenger*)_person)->m_pVisitorList[j];
				if(paxVisitor == NULL)
					continue;

				TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)paxVisitor->getBehavior(MobElementBehavior::TerminalBehavior);

				if (paxVisitor->getType().GetTypeIndex() == 2)
				{
					nCount++;
				}
				
				CGroupLeaderInfo* pNonPaxGroupLeaderInfo = (CGroupLeaderInfo*)paxVisitor->m_pGroupInfo;
				if (true == pNonPaxGroupLeaderInfo->isInGroup())
				{
					MobileElementList& nonPaxfollowerList = pNonPaxGroupLeaderInfo->GetFollowerList();
					for (int i=0 ; i<(int)nonPaxfollowerList.size(); i++)
					{
						Person* pNonPaxFollower = (Person*)nonPaxfollowerList[i];
						if (pNonPaxFollower->getType().GetTypeIndex() == 2)
						{
							nCount++;
						}
					}
				}
			}
		}

	}
	else //follower
	{
		if(_person->getType().GetTypeIndex() == 2)
		{
			nCount++;
		}
	}
	return nCount;
}

//check loader is vacant for check baggage
bool DependentProcessorList::CheckLoaderVacant(Conveyor* pDependentProc,Person* _person)const
{
	int nEmptyCount = pDependentProc->GetEmptySlotCount();//leave empty count
	int nOccupied = m_pProc->GetOccupants()->getCount();

	//calculate check baggage count
	int nApproach = 0;
	nApproach += GetCheckBaggageCount(_person);
	

	//Get all  Occupant item info, passenger, follower, visitor, ...
	for (int i = 0; i < nOccupied; i++)
	{
		Person* pCurrentPerson = ((Person*)m_pProc->GetOccupants()->getItem( i ));	
	
		nApproach += GetCheckBaggageCount(pCurrentPerson);
	}


	return nApproach > nEmptyCount ? false : true;
}








