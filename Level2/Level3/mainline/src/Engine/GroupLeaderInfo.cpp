// GroupLeaderInfo.cpp: implementation of the CGroupLeaderInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "GroupLeaderInfo.h"

#include "GroupFollowerInfo.h"
#include "person.h"
#include "pax.h"
#include "visitor.h"
#include "hold.h"
#include "Loader.h"
#include "../Inputs/TrayHost.h"
#include "terminal.h"
#include "TerminalMobElementBehavior.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGroupLeaderInfo::CGroupLeaderInfo( int _nGroupSize, Person* _pGroupLeader  )
	:CGroupInfo( _nGroupSize )
{
	SetGroupLeader( _pGroupLeader );
	m_pGroupFollowerList.ownsElements( 0 );// will explicitly delete.
	m_bInGroup = true;
}

CGroupLeaderInfo::~CGroupLeaderInfo()
{
}


// Add person in the group.
void CGroupLeaderInfo::AddFollower( Person* _pPerson, Person* _pGroupLeader )
{
	_pPerson->m_pGroupInfo->SetGroupLeader( _pGroupLeader );
	m_vGroupFollowerId.push_back( _pPerson->getID() );	
	m_pGroupFollowerList.addItem( _pPerson );	
}

PaxVisitor* CGroupLeaderInfo::GetFollowerInTrayHost()
{
	int nFollowerCount = m_pGroupFollowerList.getCount();
	for (int i = 0; i < nFollowerCount; i++)
	{
		PaxVisitor* pVisitor = (PaxVisitor*)m_pGroupFollowerList.getItem(i);
		VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior*)pVisitor->getBehavior(MobElementBehavior::TerminalBehavior);
		if (spTerminalBehavior&&spTerminalBehavior->IfInTrayHost())
		{
			return pVisitor;
		}
	}
	return NULL;
}

void CGroupLeaderInfo::SetFollowerInTray(bool bInTray)
{
	VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior*)m_pGroupLeader->getBehavior(MobElementBehavior::TerminalBehavior);
	spTerminalBehavior->SetInTrayHost(bInTray);
	int nFollowerCount = m_pGroupFollowerList.getCount();
	for (int i = 0; i < nFollowerCount; i++)
	{
		PaxVisitor* pVisitor = (PaxVisitor*)m_pGroupFollowerList.getItem(i);
		VisitorTerminalBehavior* spVisitorTerminalBehavior = (VisitorTerminalBehavior*)pVisitor->getBehavior(MobElementBehavior::TerminalBehavior);
		spVisitorTerminalBehavior->SetInTrayHost(bInTray);
	}
}

void CGroupLeaderInfo::RemoveFollower(Person* _pPerson)
{
	_pPerson->m_pGroupInfo->SetGroupLeader(_pPerson);
	std::vector<long>::iterator iter = std::find(m_vGroupFollowerId.begin(),m_vGroupFollowerId.end(),_pPerson->getID());
	int nIndex = m_pGroupFollowerList.findElement(_pPerson->getID());
	if ( iter != m_vGroupFollowerId.end() && nIndex != INT_MAX)
	{
		m_vGroupFollowerId.erase(iter);
		m_pGroupFollowerList.removeItem(nIndex);
	}
}
//Get the size of those people who walk together.
int CGroupLeaderInfo::GetActiveGroupSize() const
{
	if( !m_bInGroup )
		return 1;
	return GetAdminGroupSize();
}

// Set the destination for all the follower, will adjust the corrd.
void CGroupLeaderInfo::SetFollowerDestination( const Point& _ptCurrent, const Point& _ptDestination, float* _pRotation )
{
	if( !m_bInGroup )
		return;

	int nFollowerCount = m_pGroupFollowerList.getCount();
	if(m_pGroupLeader == NULL)
	{
		ASSERT(0);
		return;
	}
	
	if (m_pGroupLeader->getType().GetTypeIndex() == 0)
	{
		for(int i=0; i<nFollowerCount; i++ )
		{
			Point ptDestination(0.0, 0.0, 0.0);
			Person* _pFollower = (Person*) m_pGroupFollowerList.getItem( i );
			ptDestination = _ptDestination;
			float fDir = (i> (MAX_GROUP-1)) ? (float)0.0 : _pRotation[i+1];
			ptDestination.offsetCoords( _ptCurrent, (double)fDir, GROUP_OFFSET );
			if( _pFollower->getState() != Death )
				_pFollower->setTerminalDestination( ptDestination );
		}
		return;
	}
	

	std::vector<int> vNonPaxVector;
	vNonPaxVector.clear();
	CMobileElemConstraint mobConstrain;
	for (int i = 0; i < nFollowerCount; i++)
	{
		PaxVisitor* _pFollower = (PaxVisitor*) m_pGroupFollowerList.getItem( i );
		mobConstrain = _pFollower->getType();
		if(std::find(vNonPaxVector.begin(),vNonPaxVector.end(),mobConstrain.GetTypeIndex()) == vNonPaxVector.end() &&\
			mobConstrain.GetTypeIndex() != m_pGroupLeader->getType().GetTypeIndex()) 
		{
			vNonPaxVector.push_back(mobConstrain.GetTypeIndex());
		}
	}

	TrayHostList* pTrayHostList = m_pGroupLeader->GetTerminal()->GetTrayHostList();
	mobConstrain = m_pGroupLeader->getType();
	int nIndex = pTrayHostList->FindItem(mobConstrain.GetTypeIndex(),vNonPaxVector);
	TrayHost* pHost = NULL;
	if (nIndex != -1)
	{
		pHost = pTrayHostList->GetItem(nIndex);
	}
	
	for(int i=0; i<nFollowerCount; i++ )
	{
		Point ptDestination(0.0, 0.0, 0.0);
		PaxVisitor* _pFollower = (PaxVisitor*) m_pGroupFollowerList.getItem( i );
		VisitorTerminalBehavior* spFollowTerminalBehavior = (VisitorTerminalBehavior*)_pFollower->getBehavior(MobElementBehavior::TerminalBehavior);
		
		if (spFollowTerminalBehavior&&spFollowTerminalBehavior->IfInTrayHost() && pHost)
		{
			double xProb, yProb, zProb;

			do {
				// get 3 random numbers between 0 and 1
				xProb = (double)rand() / RAND_MAX;
				yProb = (double)rand() / RAND_MAX;
				zProb = (double)rand() / RAND_MAX;

				// create random point
				ptDestination.setX ( _ptDestination.getX() + (xProb * pHost->GetRadius()));
				ptDestination.setY ( _ptDestination.getY() + (yProb * pHost->GetRadius()));
				ptDestination.setZ ( _ptDestination.getZ() + (zProb * 2*pHost->GetRadius()));

				// if new point is not within polygon area, repeat
			} while (_ptDestination.distance(ptDestination) > pHost->GetRadius());
			if( _pFollower->getState() != Death )
				_pFollower->SetTrayGroupItemAndFollowerDesitination( _ptDestination, ptDestination, pHost->GetRadius());
		}
		else if(!spFollowTerminalBehavior->IfInTrayHost())
		{
			ptDestination = _ptDestination;
			float fDir = (i> (MAX_GROUP-1)) ? (float)0.0 : _pRotation[i+1];
			ptDestination.offsetCoords( _ptCurrent, (double)fDir, GROUP_OFFSET );
			if( _pFollower->getState() != Death )
				_pFollower->setTerminalDestination( ptDestination );
		}
	}
}


// Repeat every followers' last log item in log entry with new destination set by former code
// this is useful for PaxVisitor to make a quick shift
// 
// NOTE: to use this method correctly, look at void Person::RepeatLastLogWithNewDest().
void CGroupLeaderInfo::RepeatFollowersLastLogWithNewDest()
{
	int nFollowerCount = m_pGroupFollowerList.getCount();
	for( int i=0; i<nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) m_pGroupFollowerList.getItem( i );
		_pFollower->RepeatLastLogWithNewDest();
	}
}


// write log for all the follower if they are active grouped
void CGroupLeaderInfo::WriteLogForFollower( ElapsedTime _time, bool _bBackup, bool bOffset)
{
	if( !m_bInGroup )
		return;

	// write other member of group
	int nFollowerCount = m_pGroupFollowerList.getCount();
	TerminalMobElementBehavior* spLeadBehavior = m_pGroupLeader->getTerminalBehavior();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) m_pGroupFollowerList.getItem( i );
		TerminalMobElementBehavior* spTerminalBehavior = _pFollower->getTerminalBehavior();
		if(m_pGroupLeader->getState() != Death)
		{
			_pFollower->setState(m_pGroupLeader->getState());
		}
		
		if (spLeadBehavior&&spTerminalBehavior)
		{
			spTerminalBehavior->setProcessor(spLeadBehavior->getProcessor());
		}
		
		if( _pFollower->getState() != Death )
			_pFollower->writeLogEntry( _time, _bBackup, bOffset);
	}
	
}



// IN_GROUP --> OUT_GROUP: Generate event for all the followers, continue gen event
// OUT_GROUP --> JOIN_GROUP: Check if all member ready, if yes, continue gen event, else not gen event
enum EVENT_OPERATION_DECISTION CGroupLeaderInfo::ProcessEvent( ElapsedTime eventTime, bool _bDisallowGroupInNextProc )
{
	enum EVENT_OPERATION_DECISTION  enumRes = CONTINURE_GENERATE_EVENT;
	if( m_bInGroup && _bDisallowGroupInNextProc )
	{
		// IN_GROUP --> OUT_GROUP
		m_bInGroup = false;
		m_bWaitingForRegrouping = false;
		int nFollowerCount = m_pGroupFollowerList.getCount();
		for( int i=0; i< nFollowerCount; i++ )
		{
			Person* pFollower = (Person*) m_pGroupFollowerList.getItem( i );
			TerminalMobElementBehavior* spTerminalBehavior = pFollower->getTerminalBehavior();
			if( pFollower->getState() == Death)
				continue;

			if (spTerminalBehavior == NULL)
			{
				continue;
			}

			spTerminalBehavior->SetInfoFromLeader(m_pGroupLeader);
			if(m_pGroupLeader && m_pGroupLeader->getTerminalBehavior())
				m_pGroupLeader->getTerminalBehavior()->getProcessor()->addPerson(pFollower);
				
			//decause the conveyor processor is different from other common procs
			//it has it's process, must call the function addprocesson(),or there will be 
			//a assert() error,reference to the funtion
			//void Conveyor::addPerson (Person *aPerson)
			//void CLoader::addPersonIntoApproachList( Person *_pPerson )
			//void CLoader::beginService (Person *aPerson, ElapsedTime curTime)
			//add june 13, 2006 
//			if (m_pGroupLeader->getProcessor()->getProcessorType() == ConveyorProc )
//			{
//				m_pGroupLeader->getProcessor()->addPerson(pFollower);
//			}
		
			pFollower->generateEvent( eventTime + ElapsedTime( 0.6 * i ),false  );
		}
	}

	else if( !m_bInGroup && !_bDisallowGroupInNextProc )
	{
		// OUT_GROUP --> JOIN_GROUP
		//only used by LOADER proc,
		//it is the flag stand for the leader's next proc is a LOADER
		//it can't group
		if (m_pNextProc != NULL) 
		{
			m_bWaitingForRegrouping = false;
			return enumRes;
		}
		else
		{
			m_bWaitingForRegrouping = true;
		}
		if( AllReadytoRegroup() )
			m_bInGroup = true;
		else
			enumRes = NOT_GENERATE_EVENT;
	}
	return enumRes;
}


// check if all members are ready.
bool CGroupLeaderInfo::AllReadytoRegroup()
{
	// check leader
	if( !m_bWaitingForRegrouping )
		return false;

	// check followers
	int nFollowerCount = m_pGroupFollowerList.getCount();
	for( int i=0; i<nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) m_pGroupFollowerList.getItem( i );
		if( !_pFollower->m_pGroupInfo->IsWaitingForRegrouping() )
			return false;
	}

	// now ready.
	return true;
}



// Flush log for the follower.
void CGroupLeaderInfo::FlushLogforFollower( ElapsedTime _time )
{
	if( !m_bInGroup )
		return;

	// write other member of group
	int nFollowerCount = m_pGroupFollowerList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* pFollower = (Person*) m_pGroupFollowerList.getItem( i );
		if( pFollower && pFollower->getState() != Death )
			pFollower->flushLog( _time );
	}

}
//if next proc is loader, need to divided the group to leader and followers
//the function is separated the group, if the loader proc is full,
//set the follower's processor to linked holdarea proc,and generate event
void CGroupLeaderInfo::ProcessLoaderProc(ElapsedTime eventTime,Processor *pLoaderProc ,Processor* pHoldAreaProc )
{
	if (!m_bInGroup)
	{
		return;
	}
	//is not in group
	m_bInGroup = false;
	m_bWaitingForRegrouping = false;
	//get the loader's empty slot count
	int nEmptyCount = ((Conveyor *)pLoaderProc)->GetEmptySlotCount();

	//if (nEmptyCount == 0)//set the leader's next proc
	//{
	//	m_pGroupLeader->m_pGroupInfo->SetNextProc(pLoaderProc);
	//}
	//get the follower's count
	int nFollowerCount = m_pGroupFollowerList.getCount();

	for( int i=0; i< nFollowerCount; i++ )
	{

		Person* pFollower = (Person*) m_pGroupFollowerList.getItem( i );
		TerminalMobElementBehavior* spTerminalBehavior = pFollower->getTerminalBehavior();

		if (spTerminalBehavior == NULL)
		{
			return;
		}
		//if is not full
		if (nEmptyCount > 0)
		{
			//set the followers information from the leader
			spTerminalBehavior->SetInfoFromLeader(m_pGroupLeader);
			pLoaderProc->addPerson(pFollower);
			//generate the event to process 
			pFollower->generateEvent( eventTime + ElapsedTime( 0.6 * i ),false  );
			nEmptyCount -= 1;
		}
		else //is full
		{	
			//set the info from the leader
			spTerminalBehavior->SetInfoFromLeader(m_pGroupLeader);
			//set the flag ,stand the follower's next proc is LOADER,not ready to group 
			pFollower->m_pGroupInfo->SetNextProc(pLoaderProc);
			//generate the event
			pFollower->generateEvent( eventTime + ElapsedTime( 0.6 * i ),false  );
			//set the follower's current  processor to HOLDAREA proc
			spTerminalBehavior->setProcessor(pHoldAreaProc);
			((HoldingArea*)pHoldAreaProc)->storeBagtoHolding( pFollower, *(pLoaderProc->getID()) );

		}


	}	


	//if(pProc->getProcessorType() == ConveyorProc)
	//{

	//	int nEmptyCount = ((CLoader *)pProc)->GetEmptySlotCount();

	//	int nFollowerCount = m_pGroupFollowerList.getCount();
	//	for( int i=0; i< nFollowerCount; i++ )
	//	{
	//		Person* pFollower = (Person*) m_pGroupFollowerList.getItem( i );
	//		
	//	}
	//}
	//else if (pProc->getProcessorType() == HoldAreaProc)
	//{

	//}

}
int CGroupLeaderInfo::GetGroupSize()
{
	int nSize = 1;//leader
	nSize += m_pGroupFollowerList.getCount();
	return nSize;
}

bool CGroupLeaderInfo::IsSameGroup(Person* pPerson)
{
	if (pPerson == m_pGroupLeader)
		return true;

	if(m_pGroupFollowerList.Find(pPerson) != INT_MAX)
		return true;
	
	return false;
}

void CGroupLeaderInfo::SetFollowerWithOwner(bool bWithOwner)
{
	VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior*)m_pGroupLeader->getTerminalBehavior();

	if (spTerminalBehavior)
	{
		spTerminalBehavior->SetWithOwner(bWithOwner);
		int nFollowerCount = m_pGroupFollowerList.getCount();
		for( int i=0; i< nFollowerCount; i++ )
		{
			PaxVisitor* pFollower = (PaxVisitor*) m_pGroupFollowerList.getItem( i );
			VisitorTerminalBehavior* spFollowerBehavior = (VisitorTerminalBehavior*)pFollower->getTerminalBehavior();
			if (spFollowerBehavior)
			{
				spFollowerBehavior->SetWithOwner(bWithOwner);
			}
			
		}
	}
	
}

void CGroupLeaderInfo::setFollowerArrivalDoor(bool bArrivalDoor)
{
	VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior*)m_pGroupLeader->getTerminalBehavior();
	if (spTerminalBehavior)
	{
		spTerminalBehavior->m_bIsArrivalDoor = bArrivalDoor;
	}
	
	int nFollowerCount = m_pGroupFollowerList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* pFollower = (Person*) m_pGroupFollowerList.getItem( i );
		VisitorTerminalBehavior* spFollowerBehavior = (VisitorTerminalBehavior*)pFollower->getTerminalBehavior();
		if (spFollowerBehavior)
		{
			spFollowerBehavior->m_bIsArrivalDoor = bArrivalDoor;
		}
	}
}

void CGroupLeaderInfo::SetTrayGroupItemFollowerDesitination(Point _HostPt,int nRadius)
{
	int nFollowerCount = m_pGroupFollowerList.getCount();

	for(int i=0; i<nFollowerCount; i++ )
	{
		Point ptDestination(0.0, 0.0, 0.0);
		PaxVisitor* _pFollower = (PaxVisitor*) m_pGroupFollowerList.getItem( i );
		double xProb, yProb, zProb;

		do {
			// get 3 random numbers between 0 and 1
			xProb = (double)rand() / RAND_MAX;
			yProb = (double)rand() / RAND_MAX;
			zProb = (double)rand() / RAND_MAX;

			// create random point
			ptDestination.setX ( _HostPt.getX() + (xProb * nRadius));
			ptDestination.setY ( _HostPt.getY() + (yProb * nRadius));
			ptDestination.setZ ( _HostPt.getZ() + (zProb * 2*nRadius));

			// if new point is not within polygon area, repeat
		} while (_HostPt.distance(ptDestination) > nRadius);
		if( _pFollower->getState() != Death )
			_pFollower->setTerminalDestination( ptDestination );
		
	}
}

//leader position is alway first position
int CGroupLeaderInfo::GetPosition( Person* pPerson )
{
	return 0;
}

//follower search index by group leader
int CGroupLeaderInfo::SearchPostion( Person* pPerson )
{
	int nFollowerCount = m_pGroupFollowerList.getCount();

	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* pFollower = (Person*) m_pGroupFollowerList.getItem( i );
		if (pFollower == pPerson)
		{
			return i;
		}
	}
	return -1;//failed search and follower is not in the group leader 
}