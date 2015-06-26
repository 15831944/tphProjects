#include "stdafx.h"
#include "engine\visitor.h"
#include "pax.h"
#include "greet.h"
#include "inputs\AllGreetingProcessors.h"
#include "inputs\PAXDATA.H"
#include "inputs\SubFlowList.h"
#include "inputs\SubFlow.h"
#include "Inputs\TrayHost.h"
#include "common\CodeTimeTest.h"
#include "ConvergePair.h"
#include "Engine\GroupLeaderInfo.h"
#include "engine\terminal.h"
#include "inputs/NonPaxRelativePosSpec.h"
#include "inputs/MobileElemTypeStrDB.h"
#include "Engine/ARCportEngine.h"
#include <Common/ProbabilityDistribution.h>
#include "VisitorTerminalBehavior.h"
#include "TurnaroundVisitorTerminalBehavior.h"
#include "SimFlowSync.h"

#include "Engine/MobileElementExitSystemEvent.h"



#include "PaxTerminalBehavior.h"
extern std::vector<ConvergePair *> GreetPool;

PaxVisitor::PaxVisitor(MobLogEntry& _entry, CARCportEngine* _pEngine )
	: Person( _entry, _pEngine )
	,m_bOwnerInitialized(FALSE)
	,m_nOwnerGroupSize(0)
{
		if (_entry.isTurnaround())
	{
		MobileElement::setBehavior( new TurnaroundVisitorTerminalBehavior( (TurnaroundVisitor*)this ));
	}
	else
	{
		MobileElement::setBehavior( new VisitorTerminalBehavior( this ));
	}

	m_nOwnerID = _entry.getOwnerID();
	//////////////////////////////////////////////////////////////////////////
	// only to sender, the visit time is validate
	if( m_type.GetTypeIndex() == 1 && m_type.isDeparture() )
	{
		const ProbabilityDistribution* visitTimeDist;
		visitTimeDist = (m_pTerm->paxDataList->getVisitTime())->FindFit (m_type);
		if (visitTimeDist == NULL)			
		{
			//throw new StringError ("Missing visit time distribution");
			visitTime = -1l;
		}
		else
		{
			visitTime = ((ElapsedTime)(60.0 * visitTimeDist->getRandomValue()))
						+ _entry.getEntryTime();
		}
	}
	if( _entry.getGroupMember() == 0 ) 
		NewConvergePair(m_nOwnerID);

	
}

void PaxVisitor::InitVisitorAngle()
{
	CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pTerm->GetNonPaxRelativePosSpec();
	ASSERT( NULL != pNonPaxRelPosSpec );
	CPoint2008 ptRelatePos;
	ARCVector4 rotation;
	RotateAngle angle;
	int nType = 0;

	int idx = m_pGroupInfo->GetPosition(this);
	if (idx >= 0)
	{
		nType = idx;
	}
	
	if (pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_type.GetTypeIndex(),nType ))
	{
		angle.x = rotation.x;
		angle.y = rotation.y;
		angle.z = rotation.z;
		angle.w = rotation.w;
	}
	else
	{
		angle.x = 0.0;
		angle.y = 0.0;
		angle.z = 0.0;
		angle.w = 1.0;
	}
	
	m_logEntry.SetAngle(angle);
}

void PaxVisitor::NewConvergePair(long _iPassengerId,bool _ConvergeTag)
{
	bool exitTag = false;
	std::vector<ConvergePair*>::iterator ite,iterat;
	
	for(ite= GreetPool.begin();ite!=GreetPool.end();ite++)
	{
		if((*ite)->IsMyPair(_iPassengerId,id))
		{
			bool localExit = false;
			for(iterat = m_vpConvergePair.begin();iterat!=m_vpConvergePair.end();iterat++)
			{
				if((*iterat)->IsMyPair(id,_iPassengerId)) localExit = true;
			}
			if(!localExit) m_vpConvergePair.push_back((*ite));
			exitTag = true;
			break;
		}
	}
	
	if(exitTag) return;
	
	ConvergePair *pPair = new ConvergePair(id,_iPassengerId);//defaulse visitor is leader
	pPair->SetPoint(id,this);

	if(_ConvergeTag) 
	{
		if(GetOwner())
		{
			pPair->SetPoint(_iPassengerId,GetOwner());
			//// TRACE("\r\nConverge__Create Pair!!");
		}
	}
	else //// TRACE("\r\nmmmmm__Create Pair in VISITOR!!");
	
	pPair->SetTerminal(m_pTerm);
	pPair->setVisitorType(m_type.GetTypeIndex());
	GreetPool.push_back(pPair);
    m_vpConvergePair.push_back(pPair);
	if(_ConvergeTag) 
	{
		if (GetOwner())
		{
			GetOwner()->NewConvergePair(id,m_type.GetTypeIndex());
		}

	}
}

//attaches visitor to a passenger.
void PaxVisitor::initOwnerPassenger( Passenger* _pPax )
{
	m_bOwnerInitialized = TRUE;
    m_nOwnerGroupSize = _pPax->GetAdminGroupSize();
}


void PaxVisitor::DeleteGroupFollower(PaxVisitor* pPaxVisitor)
{
	((CGroupLeaderInfo*)m_pGroupInfo)->RemoveFollower(pPaxVisitor);
}

// Writes the element's current location, state to the log file.
// in:
// _bBackup: if the log is backwards.
void PaxVisitor::writeLogEntry (ElapsedTime _time, bool _bBackup, bool bOffset /*= true*/ )
{
	writeLog( _time, _bBackup, bOffset );
	m_pGroupInfo->WriteLogForFollower( _time, _bBackup, bOffset );
}


void PaxVisitor::MakeVisitorSyncAvailable(const ElapsedTime& curTime)
{
	//make sync non passenger available
	if (getCurrentBehavior()->getBehaviorType() == MobElementBehavior::TerminalBehavior)
	{
		Passenger* pOwner = (Passenger*)getEngine()->m_simBobileelemList.GetAvailableElement(m_nOwnerID);
		if (pOwner)
		{
			PaxTerminalBehavior* spTerminalBehavior = (PaxTerminalBehavior*)pOwner->getTerminalBehavior();
			if (spTerminalBehavior)
			{
				spTerminalBehavior->MakeVisitorSyncAvailable(this,curTime);
			}
		}
		
	}
}
// Dumps all tracks to the log and updates ElementDescription record
// called at death of mobile element
// must repeat once for each member of group
void PaxVisitor::flushLog( ElapsedTime _time, bool bmissflight)
{
	//make sync passenger available
	MakeVisitorSyncAvailable(_time);

	ElapsedTime terminalTime;
	if (!(m_logEntry.getTerminalEntryTime() == terminalTime) && m_logEntry.getTerminalEndTime() == terminalTime)//terminal end time did not set
	{
		m_logEntry.setTerminalEndTime(_time);
		writeTerminalEndLogEntry(_time);
	}

	m_iPreState = state;
	state = Death;
	writeLogEntry( _time, false );
	m_logEntry.setExitTime( _time );
	
	int nTrackCount = m_logEntry.getCurrentCount();
	//if( id == 83)
	//{

	//	for (int nTrack = 0; nTrack < nTrackCount; ++nTrack)
	//	{
	//		MobEventStruct track = m_logEntry.getEvent(nTrack);
	//		ElapsedTime eTime;
	//		eTime.setPrecisely(track.time);

	//		TRACE("\r\n, %s, %d, %f, %f, %d, %f, %s , %s ",eTime.printTime(), 
	//			track.time, track.x, track.y, track.z, track.m_RealZ, (track.m_IsRealZ == TRUE)?"TRUE":"FALSE",track.m_bWithOwner?"TRUE":"FALSE" );
	//	}
	//}



	// write first member of group
//	MobEventStruct* pLog = NULL;
//    m_logEntry.getLog( pLog );
	VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior *)getBehavior(MobElementBehavior::TerminalBehavior);
	if(spTerminalBehavior)
	{
		spTerminalBehavior->m_nFreeMovingIndex = ( spTerminalBehavior->m_nFreeMovingIndex != -1)? spTerminalBehavior->m_nFreeMovingIndex: nTrackCount-2;
	}
	
	float fPaxRotation = ( m_nOwnerGroupSize >= MAX_GROUP ) ? (float)0.0 : m_pRotation[m_nOwnerGroupSize];
//	m_logEntry.setEventList( pLog, nTrackCount );
	m_logEntry.saveEventsToLog();
	m_pTerm->paxLog->updateItem (m_logEntry, m_logEntry.getIndex());
	
//	delete [] pLog;
//	pLog = NULL;
	
	bool flag = m_logEntry.getEachPaxOwnNopax();
	int iNoPaxCountOfEachPax;
	// if have no owner, the m_nOwnerGroupSize ==0;
	if( GetOwner() && flag )
	{
		iNoPaxCountOfEachPax = m_logEntry.getGroupSize()/m_nOwnerGroupSize;
		assert(iNoPaxCountOfEachPax>=1);
	}

	long owner = m_logEntry.getOwnerID();

	m_pGroupInfo->FlushLogforFollower( _time );

/* 20050607	
	// write other member of group
	for( int i=0; i< m_pGroupFollowerList.getCount(); i++ )
	{
		Person* _pFollower = (Person*) m_pGroupFollowerList.getItem(i);
		assert( _pFollower->getGroupMemberMoveState() == move_follow_leader );
		
		if( m_pOwnerPax && flag)
		{
			int div = (i+1)/iNoPaxCountOfEachPax;
			_pFollower->getLogEntry().setOwner( owner + div );
			fPaxRotation = ( div >= MAX_GROUP ) ? 0.0 : rotation[div] + i % iNoPaxCountOfEachPax * 5.0;
		}
		else
		{
			fPaxRotation = ( i > MAX_GROUP-1 ) ? 0.0 : rotation[i+1];
		}
		
		
		// copy part event list 
		_pFollower->copyPassengerEventLog( _pFollower->getGroupAttachLogIndex(), m_logEntry.getCurrentCount(), m_logEntry );
		_pFollower->adjustCoordinates( _pFollower->getLogEntry().getEventList(), _pFollower->getGroupAttachLogIndex(), _pFollower->getLogEntry().getCurrentCount(),
										fPaxRotation, GROUP_OFFSET );

		// write log entry
		_pFollower->flushGroupFollowerLog( _time, fPaxRotation, i+1 );
	}
*/
    m_logEntry.clearLog();
	
	//// when the person death, decrease the count of area
	TerminalMobElementBehavior* pOwnerBehavior = NULL;
	TerminalMobElementBehavior* pVisitorBehavior = getTerminalBehavior();
	if (GetOwner())
	{
		pOwnerBehavior = GetOwner()->getTerminalBehavior();
	}
	m_pTerm->m_pAreas->handlePersonDeath( this, pOwnerBehavior ? pOwnerBehavior->getPoint() : pVisitorBehavior->getLocation(), _time );

	

	//person death,delete convergePair

	if(m_vpConvergePair.size()>0) DeleteConvergePair(m_nOwnerID,true);



	if(getEngine()->m_simBobileelemList.IsAlive(this))
	{
		MobileElementExitSystemEvent *pEvent = new MobileElementExitSystemEvent(this, _time);
		pEvent->addEvent();
	}
	else
	{
		ASSERT(0);
	}
}


void PaxVisitor::writeVisitorLog(ElapsedTime time, Point _OwnerPreLocation, bool _bBackup, bool bOffset)
{
	MobEventStruct track;
    track.time = (long)time;
	track.state = LOBYTE( LOWORD(state) );
	VisitorTerminalBehavior* spTerminalBehavior = (VisitorTerminalBehavior *)getBehavior(MobElementBehavior::TerminalBehavior);

	if (spTerminalBehavior == NULL)
	{
		return;
	}

	if (spTerminalBehavior->getProcessor() == 0)
    	return ;


	if (state == TerminalBegin)
	{
		m_logEntry.setTerminalEntryTime(time);
	}
	else if (state == TerminalEnd)
	{
		m_logEntry.setTerminalEndTime(time);
	}
	
	track.procNumber = (short)spTerminalBehavior->getProcessor()->getIndex();
	track.bDynamicCreatedProc = spTerminalBehavior->getProcessor()->GetDynamicCreatedFlag();
	track.followowner = true;
	track.backup = _bBackup;

	//////////////////////for bridge realz////////////////////////////////////////////////////
	BOOL bInBridge = spTerminalBehavior->IsWalkOnBridge() && spTerminalBehavior->getBridgeState()==DepBridge;
	BOOL bIsArrivalDoor = spTerminalBehavior->m_bIsArrivalDoor;
	BOOL bIsRealZ = bInBridge || (bIsArrivalDoor?TRUE:FALSE);
	//////////////////////////////////////////////////////////////////////////

	track.m_IsRealZ = bIsRealZ;

	//ASSERT(m_type.GetTypeIndex() != 0);
	CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pTerm->GetNonPaxRelativePosSpec();
	ASSERT( NULL != pNonPaxRelPosSpec );
	CPoint2008 ptRelatePos;
	ARCVector4 rotation;
	int nType = m_pGroupInfo->GetPosition(this);
	pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_type.GetTypeIndex(),nType);
	track.m_RealZ = (float)ptRelatePos.getZ();
// 	track.xAngle = rotation.x;
// 	track.yAngle = rotation.y;
// 	track.zAngle = rotation.z;
// 	track.w = rotation.w;
	track.m_bWithOwner = spTerminalBehavior->m_bWithOwner;

	if (spTerminalBehavior->m_bWithOwner)
	{
		track.m_bVisible = pNonPaxRelPosSpec->GetTypeItemVisible(m_type.GetTypeIndex());
	}

	/////////////////////////////////////////////////////////
	MobEventStruct *pPreTrack = NULL;				// pretrack
	long lLogCount = m_logEntry.getCurrentCount();

	Point pre_location = spTerminalBehavior->getLocation();
	//get pretrack if it exist
	//if( lLogCount >=1 )
	//{
	//	PreTrack = m_logEntry.getEvent( lLogCount-1 );	
	//}
	pPreTrack = m_logEntry.getPreEvent();

	if(  pPreTrack == NULL || pPreTrack->followowner == false )
	{
		if( spTerminalBehavior->getLocation() != spTerminalBehavior->getDest() )
		{
			Point vector (spTerminalBehavior->getLocation(), spTerminalBehavior->getDest());
	        Point latPoint (vector.perpendicular());
			latPoint.length (getSideStep());

			spTerminalBehavior->setLocation(spTerminalBehavior->getDest()+latPoint);
			spTerminalBehavior->setDestination(spTerminalBehavior->getLocation());
		}
	}
	else	//PreTrack.followowner = true
	{
		// comment by Benny, 2009-07-20
		//
		// here we think that, two log must be written
		//

		//write the first log item
		TerminalMobElementBehavior* spOwerBehavior = NULL;
		if (GetOwner())
		{
			spOwerBehavior = (TerminalMobElementBehavior *)GetOwner()->getBehavior(MobElementBehavior::TerminalBehavior);
		}
		
		if(spOwerBehavior && !spOwerBehavior->inElevotor) 
		{
			writeCurrentLogItem(_OwnerPreLocation, firstLog);
			// add the first log entry
			MobEventStruct eventItem = *pPreTrack;

			spTerminalBehavior->getLocation().getPoint (eventItem.x, eventItem.y, eventItem.z);
			/*if(spTerminalBehavior->getBridgeState()==DepBridge)
			{
				eventItem.m_IsRealZ = track.m_IsRealZ;
				if(eventItem.m_IsRealZ)
					eventItem.m_RealZ = eventItem.z;
			}*/
			m_logEntry.addEvent (eventItem);
		}


		// modified by Benny, 2009-07-30
		// don't simply use this
// 		m_pGroupInfo->WriteLogForFollower( PreTrack.time, _bBackup, bOffset);
		// use following code to repeat last log for all follower one by one
		m_pGroupInfo->RepeatFollowersLastLogWithNewDest();		

		// write the second log item
		writeCurrentLogItem(_OwnerPreLocation, secondLog);
	}	

	if (spTerminalBehavior && spTerminalBehavior->m_bIsArrivalDoor)
	{
		track.m_RealZ = (float)spTerminalBehavior->getLocation().getZ();
		spTerminalBehavior->m_bIsArrivalDoor = false;
	}
	if(bIsRealZ)
	{
		track.m_RealZ = (float)spTerminalBehavior->getLocation().getZ();
	}
	spTerminalBehavior->getLocation().getPoint (track.x, track.y, track.z);

	/////////////////////////////////////////////////////////
    m_logEntry.addEvent (track);


	m_pGroupInfo->WriteLogForFollower( time, _bBackup, bOffset);

	//// update group follower
	//updateGroupFollowerLog( time, pre_location, location,true );
}


/////////////////////////////////////////////////////////////////////
// add a log to make the distance between visitor and it's owner don't changed
// but this way will make some flash
//
// no exception
void PaxVisitor::addAngleLogItem(Point _OwnerPreLocation)
{
	ASSERT( GetOwner() );
	
	TerminalMobElementBehavior* pOwnerBehavior = NULL;
	if (GetOwner())
	{
		pOwnerBehavior = GetOwner()->getTerminalBehavior();
	}

	Point OwnerLocation = pOwnerBehavior->getPoint();

	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if(pOwnerBehavior&&pOwnerBehavior->inElevotor) 
	{
		spTerminalBehavior->setLocation(OwnerLocation);
	    return;
	}

	if( _OwnerPreLocation != OwnerLocation )
	{
		//vector
		Point OwnerVector( _OwnerPreLocation, OwnerLocation );

		long lLogCount = m_logEntry.getCurrentCount();
		MobEventStruct *pPreTrack = m_logEntry.getPreEvent();

		MobEventStruct newTrack;
		if(pPreTrack)
			newTrack = *pPreTrack;

		//Point
		Point newPoint( newTrack.x, newTrack.y, newTrack.z );

		Point pre_location = newPoint;

		switch( m_type.GetTypeIndex() )
		{
			case 1:		//visitor  // left
			{
				Point vectorLat( OwnerVector.perpendicular() );
				vectorLat.length(100.0);
				newPoint = _OwnerPreLocation + vectorLat;
				break;
			}
			case 2:	//checked bag	//front
			{
				OwnerVector.length(25.0);//( 150.0 ); // change 3 feed to 6 inch(12*3 /6 ==small 6 time)
				newPoint = _OwnerPreLocation + OwnerVector;
				break;
			}
			case 3:	//hand bag	// left
			{
				Point vectorLat( OwnerVector.perpendicular() );
				vectorLat.length(20.0);
				newPoint = _OwnerPreLocation + vectorLat;
				break;
			}
			case 4:	//carte		// front
			{
				OwnerVector.length( 100.0 );
				newPoint = _OwnerPreLocation + OwnerVector;
				break;
			}
			case 6: //dog		//right_front
			{
				Point vectorLat( OwnerVector.perpendicular() );
				vectorLat.length(-100.0);
				newPoint = _OwnerPreLocation + vectorLat;
				OwnerVector.length(100);
				newPoint += OwnerVector;
				break;
			}
			default: //other visitor
				break;
		}
		newPoint.getPoint( newTrack.x, newTrack.y, newTrack.z );

		//newTrack.time += 10;
		m_logEntry.addEvent( newTrack );

		//update group follower
		//ElapsedTime pre_time( newTrack.time/100.0 );
		//updateGroupFollowerLog( pre_time, pre_location, newPoint, newTrack.followowner );
	}

	return;
}


////////////////////////////////////////////////////////////////////////////
// write visitor the current point
void PaxVisitor::writeCurrentLogItem(Point _OwnerPreLocation, theVisitorLogNum logNum/* = secondLog*/)
{
	ASSERT(PaxVisitor::firstLog == logNum || PaxVisitor::secondLog == logNum);

	if(GetOwner() == NULL )
		return;

	TerminalMobElementBehavior* pOwnerBehavior = GetOwner()->getTerminalBehavior();
	if(pOwnerBehavior == NULL)
		return;


	Point OwnerLocation = pOwnerBehavior->getPoint();

	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
 	if(pOwnerBehavior&&pOwnerBehavior->inElevotor) 
 	{
 		spTerminalBehavior->setLocation(OwnerLocation);
 		return;
 	}

	if( _OwnerPreLocation == OwnerLocation )return;


	// use vector(_OwnerPreLocation -> OwnerLocation) as passenger direction
	Point paxDirection(_OwnerPreLocation, OwnerLocation);
	Point paxLogPoint = (PaxVisitor::firstLog == logNum)?_OwnerPreLocation:OwnerLocation;


// 	Point lastLocation = location;
	CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pTerm->GetNonPaxRelativePosSpec();
	ASSERT( NULL != pNonPaxRelPosSpec );
	CPoint2008 ptRelatePos;
	ARCVector4 rotation;
	int nType = m_pGroupInfo->GetPosition(this);
	bool bRes = pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_type.GetTypeIndex(),nType);
	if (bRes)
	{
		Point vectorLat( ptRelatePos.getX(),ptRelatePos.getY(), 0.0 );
		vectorLat.rotate(paxDirection, paxLogPoint);



	//	location = vectorLat;
		spTerminalBehavior->getLocation().setPoint(vectorLat.getX(),vectorLat.getY(),paxLogPoint.getZ());
		spTerminalBehavior->SetRelativePosition(paxDirection,paxLogPoint);
	}
	else
	{
		switch( m_type.GetTypeIndex() )
		{
		case 1:	//visitor  // left
			{
				Point vectorLat( paxDirection.perpendicular() );
				vectorLat.length(100.0);
				spTerminalBehavior->setLocation(paxLogPoint + vectorLat);
				spTerminalBehavior->setDestination(spTerminalBehavior->getPoint());
				break;
			}
		case 2:	//checked bag	//front
			{
				paxDirection.length(25.0);//( 150.0 );
				spTerminalBehavior->setDestination(paxLogPoint + paxDirection);
				spTerminalBehavior->setLocation(paxLogPoint + paxDirection);
				break;
			}
		case 3:	//hand bag	// left
			{
				Point vectorLat( paxDirection.perpendicular() );
				vectorLat.length(20.0);
				spTerminalBehavior->setLocation(paxLogPoint + vectorLat);
				spTerminalBehavior->setDestination(paxLogPoint + vectorLat);
				break;
			}
		case 4:	//carte		// front
			{
				paxDirection.length( 100.0 );
				spTerminalBehavior->setDestination(paxLogPoint + paxDirection);
				spTerminalBehavior->setLocation(paxLogPoint + paxDirection);
				break;
			}
		case 6: //dog		//right_front
			{
				Point vectorLat( paxDirection.perpendicular() );
				vectorLat.length(-100.0);
				spTerminalBehavior->setDestination(paxLogPoint + vectorLat);
				paxDirection.length( 100.0 );
				spTerminalBehavior->setDestination(paxDirection + spTerminalBehavior->getDest());
				spTerminalBehavior->setLocation(spTerminalBehavior->getDest());
				break;
			}
		default: //other visitor
			{
				paxDirection.length(25.0);//( 150.0 );
				spTerminalBehavior->setDestination(paxLogPoint + paxDirection);
				spTerminalBehavior->setLocation(spTerminalBehavior->getDest());
				break;
			}
		}	
		m_pGroupInfo->SetFollowerDestination( spTerminalBehavior->getLocation(), spTerminalBehavior->getDest(), m_pRotation );
	}

}


//////////////////////////////////////////////////////////////////////////
// discarded by Benny, 2009-07-20
////////////////////////////////////////////////////////////////////////////
// write visitor the current point
// void PaxVisitor::writeCurrentLogItem(Point _OwnerPreLocation)
// void PaxVisitor::writeCurrentLogItem(Point _OwnerPreLocation, theVisitorLogNum logNum/* = secondLog*/)
// {
// 	Point OwnerLocation = m_pOwnerPax->getPoint();
// 
// 	int tag = m_pOwnerPax->inElevotor;
// 	if(tag) 
// 	{
// 		location = OwnerLocation;
// 		return;
// 	}
// 
// 	if( _OwnerPreLocation == OwnerLocation )return;
// 
// 
// 	if( location != m_ptDestination )
// 	{
// 		Point vector (location, m_ptDestination);
// 		Point latPoint (vector.perpendicular());
// 		latPoint.length (getSideStep());
// 
// 		location = m_ptDestination + latPoint;
// 		m_ptDestination = location;
// 	}
// 	
// 	if( _OwnerPreLocation != OwnerLocation)
// 	{
// 
// 		CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pTerm->GetNonPaxRelativePosSpec();
// 		ASSERT( NULL != pNonPaxRelPosSpec );
// 		CPoint2008 ptRelatePos;
// 		Ogre::Quaternion rotation;
// 		if(true == pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_type.GetTypeIndex() ))
// 		{
// 			Point vectorLat( ptRelatePos.getX(),ptRelatePos.getY(), 0.0 );
// 			location = OwnerLocation + vectorLat;
// 			m_ptDestination = location;
// 		}
// 		else
// 		{
// 			Point vector( _OwnerPreLocation, OwnerLocation );
// 
// 			switch( m_type.GetTypeIndex() )
// 			{
// 			case 1:	//visitor  // left
// 				{
// 					Point vectorLat( vector.perpendicular() );
// 					vectorLat.length(100.0);
// 					location = OwnerLocation + vectorLat;
// 					m_ptDestination = location;
// 					break;
// 				}
// 			case 2:	//checked bag	//front
// 				{
// 					vector.length(25.0);//( 150.0 );
// 					m_ptDestination = OwnerLocation + vector;
// 					location = m_ptDestination;
// 					break;
// 				}
// 			case 3:	//hand bag	// left
// 				{
// 					Point vectorLat( vector.perpendicular() );
// 					vectorLat.length(20.0);
// 					location = OwnerLocation + vectorLat;
// 					m_ptDestination = location;
// 					break;
// 				}
// 			case 4:	//carte		// front
// 				{
// 					vector.length( 100.0 );
// 					m_ptDestination = OwnerLocation + vector;
// 					location = m_ptDestination;
// 					break;
// 				}
// 			case 6: //dog		//right_front
// 				{
// 					Point vectorLat( vector.perpendicular() );
// 					vectorLat.length(-100.0);
// 					m_ptDestination = OwnerLocation + vectorLat;
// 					vector.length( 100.0 );
// 					m_ptDestination += vector;
// 					location = m_ptDestination;
// 					break;
// 				}
// 			default: //other visitor
// 				{
// 					vector.length(25.0);//( 150.0 );
// 					m_ptDestination = OwnerLocation + vector;
// 					location = m_ptDestination;
// 					break;
// 				}
// 			}	
// 		}
// 	}
// 	m_pGroupInfo->SetFollowerDestination( location, m_ptDestination, m_pRotation );
// 	return;
// }
// new person for group follower
void PaxVisitor::newGroupFollower( ) 
{
	MobLogEntry _followerEntry = m_logEntry;

	InitVisitorAngle();
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior *)getBehavior(MobElementBehavior::TerminalBehavior);
	for( int i=1; i< GetAdminGroupSize(); i++ )
	{
		_followerEntry.incPax();
		PaxVisitor* _pFollower = new PaxVisitor( _followerEntry, getEngine() );
		TerminalMobElementBehavior* spFollowerBehavior = (TerminalMobElementBehavior*)_pFollower->getBehavior(MobElementBehavior::TerminalBehavior);
		if (spTerminalBehavior && spFollowerBehavior)
		{
			getEngine()->m_simBobileelemList.Register( _pFollower );
			((CGroupLeaderInfo*)m_pGroupInfo)->AddFollower( _pFollower, this );	
			_pFollower->InitVisitorAngle();
		}
	}
}

void PaxVisitor::DeleteConvergePair(long _iPaxId,bool _IsMyWantDele) //delete local pair.
{
	if(m_vpConvergePair.size() ==0) return;
	std::vector<ConvergePair*>::iterator ite = m_vpConvergePair.begin();
	
	
	if((*ite)->IsMyPair(_iPaxId,id))
	{
		if(_IsMyWantDele)
		{
			Passenger * pPass = NULL;
			if((*ite)->IsLeader(id) && (*ite)->GetFollowerPoint())
			{
				pPass = (Passenger *)((*ite)->GetFollowerPoint());
				if(pPass->getID()==_iPaxId)
			       pPass->DeleteConvergePair(id,false);
			}
			else if((!(*ite)->IsLeader(id)) &&(*ite)->GetLeaderPoint())
			{
				pPass = (Passenger *)((*ite)->GetLeaderPoint());
				if(pPass->getID() ==_iPaxId)
				   pPass->DeleteConvergePair(id,false);
			}
			ite =m_vpConvergePair.erase(ite);
			DeleteGreetPool(id,_iPaxId);
			return;
		}
		else
		{
			ite =m_vpConvergePair.erase(ite);
		    DeleteGreetPool(id,_iPaxId);
			return;
		}
	}
}

void PaxVisitor::DeleteGreetPool(long _iFirId,long _iSecoId) //_iFirId is visitor
{
	if(GreetPool.size()<=0) return;
	std::vector<ConvergePair*>::iterator ite = GreetPool.begin();
	for(ite;ite!=GreetPool.end();ite++)
	{
		if((*ite)->IsMyPair(_iFirId,_iSecoId))
		{
			if((*ite)->IsLeader(_iFirId)) 
				(*ite)->ClearLeader();
			else 
				(*ite)->ClearFollower();

			return;
		}
	}
}

void PaxVisitor::SetTrayGroupItemAndFollowerDesitination(Point _HostPt,Point _DesitinationPt,int nRadius)
{
	TerminalMobElementBehavior* pVisitorBehavior = getTerminalBehavior();
	pVisitorBehavior->setDestination(_DesitinationPt);
	m_pGroupInfo->SetTrayGroupItemFollowerDesitination(_HostPt,nRadius);
}

Passenger* PaxVisitor::GetOwner( void )
{
	if (m_bOwnerInitialized)
	{
		return (Passenger *)(getEngine()->m_simBobileelemList.GetAvailableElement(m_nOwnerID));
	}
	return NULL;
}