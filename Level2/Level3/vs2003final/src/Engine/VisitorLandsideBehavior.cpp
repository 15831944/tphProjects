#include "StdAfx.h"
#include ".\visitorlandsidebehavior.h"
#include "person.h"
#include "MobElementsDiagnosis.h"
#include "pax.h"
#include "GroupLeaderInfo.h"
#include "terminal.h"
#include <common/Point2008.h>
#include <common/ARCVector4.h>
#include "Inputs/NonPaxRelativePosSpec.h"
#include "LandsideResourceManager.h"
#include "LandsideVehicleInSim.h"
#include "LandsideParkingLotInSim.h"
#include "./ARCportEngine.h"
#include "LandsideSimulation.h"
#include ".\SimEngineConfig.h"
#include "..\Landside\LandsideCurbSide.h"
#include "PaxTerminalBehavior.h"
#include "PaxLandsideBehavior.h"

VisitorLandsideBehavior::VisitorLandsideBehavior( Person* p ) 
:LandsideBaseBehavior(p)
{

}
VisitorLandsideBehavior::~VisitorLandsideBehavior(void)
{
}

int VisitorLandsideBehavior::performanceMove( ElapsedTime p_time,bool bNoLog )
{
	//ASSERT(0);
	return 0;
}

void VisitorLandsideBehavior::InitializeBehavior()
{
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(pGroupLeaderInfo->isInGroup() && !pGroupLeaderInfo->IsFollower())
	{
		MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
		int nFollowerCount = elemList.getCount();						

		for( int i=0; i< nFollowerCount; i++ )
		{
			Person* _pFollower = (Person*) elemList.getItem( i );
			if(_pFollower == NULL)
				continue;  

			VisitorLandsideBehavior* pBehavior = new VisitorLandsideBehavior(_pFollower);						
			_pFollower->setBehavior(pBehavior);
			_pFollower->setState(EntryLandside);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//write log itself
int VisitorLandsideBehavior::WriteLogEntry(ElapsedTime time, int ntype,bool _bBackup /*= false*/, bool bOffset /*= true */ )
{
	MobEventStruct track;
	track.time = (long)time;
	track.state = m_pPerson->getState();
	track.procNumber = -1;
	track.bDynamicCreatedProc = FALSE;
	track.reason = -1;
	track.backup = _bBackup;
	track.followowner	= false;
	track.m_IsRealZ = TRUE;
	//LandsideBaseBehavior* spLandsideBehavior = getLBehavior();
	//if (spLandsideBehavior == NULL)
	//	return false;

	////resource index 
	//if(spOnboardBehavior->GetCurrentResource())
	//	track.procNumber = spOnboardBehavior->GetCurrentResource()->GetResourceID();
	//else
	track.procNumber = -1;

	//visitor follow with passenger
	if(m_pPerson->getType().GetTypeIndex() != 0 && m_pPerson->getType().GetTypeIndex() == 0)
	{
		// get object's Altitude
		CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pPerson->GetTerminal()->GetNonPaxRelativePosSpec();
		ASSERT( NULL != pNonPaxRelPosSpec );
		CPoint2008 ptRelatePos;
		ARCVector4 rotation;
		pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_pPerson->getType().GetTypeIndex(),ntype);
		track.m_RealZ = (float)ptRelatePos.getZ();
		track.m_bWithOwner = true;

		if (track.m_bWithOwner)
		{
			track.m_bVisible = pNonPaxRelPosSpec->GetTypeItemVisible(m_pPerson->getType().GetTypeIndex());
		}
	}

	pure_location= m_ptDestination;
	if (location != m_ptDestination)
	{
		location = m_ptDestination;
		setDestination( location );
	}


	track.x = static_cast<float>(getPoint().n[VX]);
	track.y = static_cast<float>(getPoint().n[VY]);
	track.z = static_cast<short>(getPoint().n[VZ]);
	track.m_RealZ += track.z;

	//TRACE("\r\nTime: %d   X = %f, Y = %f, Z = %f \r\n", track.time, track.x, track.y, track.z);
	m_pPerson->getLogEntry().addEvent (track); // if simulation stops here, it means memory insufficient.

	writeFollowerLog(time,_bBackup, bOffset);

	return true;
}

void VisitorLandsideBehavior::writeFollowerLog( ElapsedTime time, bool _bBackup /*= false*/, bool bOffset /*= true */ )
{
	ASSERT(m_pPerson);
	if(m_pPerson == NULL || m_pPerson->m_pGroupInfo->IsFollower())
		return;

	//write log for follower
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	//check in group
	if (pGroupLeaderInfo->isInGroup() == false)
		return;

	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;
		VisitorLandsideBehavior *pBehavior = (VisitorLandsideBehavior *)_pFollower->getLandsideBehavior();
		pBehavior->setState(m_pPerson->getState());
		pBehavior->WriteLogEntry(time,i+1,_bBackup,bOffset);
		//WriteVisitorEntryLog(,_pFollower,time,i+1,_bBackup,bOffset);
	}
}


//////////////////////////////////////////////////////////////////////////
//write log, called by owned passenger
bool VisitorLandsideBehavior::WriteVisitorTrack( const ARCVector3& pax_pre_location,PaxLandsideBehavior* pOwnerPax,ElapsedTime time, bool _bBackup /*= false*/, bool bOffset /*= true*/ )
{
	MobEventStruct track;
	track.time = (long)time;
	track.state = m_pPerson->getState();
	track.procNumber = -1;
	track.bDynamicCreatedProc = FALSE;
	track.followowner	= true;
	track.reason = -1;
	track.backup = _bBackup;
	track.m_IsRealZ = TRUE;


	////resource index 
	//if(pLandsideBehavior->GetCurrentResource())
	//	track.procNumber = pLandsideBehavior->GetCurrentResource()->GetResourceID();
	//else
	track.procNumber = -1;

	track.state = pOwnerPax->getState();
	setState(pOwnerPax->getState());
	Point _OwnerPreLocation;
	_OwnerPreLocation.init(pax_pre_location.n[VX],pax_pre_location.n[VY],pax_pre_location.n[VZ]);

	ARCVector3 vectorLocation;
	vectorLocation = getPoint();
	//ASSERT(m_type.GetTypeIndex() != 0);
	CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pPerson->GetTerminal()->GetNonPaxRelativePosSpec();
	ASSERT( NULL != pNonPaxRelPosSpec );
	CPoint2008 ptRelatePos;
	ARCVector4 rotation;
	pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_pPerson->getType().GetTypeIndex() );
	track.m_bWithOwner = true;
	track.m_bVisible = pNonPaxRelPosSpec->GetTypeItemVisible(m_pPerson->getType().GetTypeIndex());


	/////////////////////////////////////////////////////////
			// pretrack
	long lLogCount = m_pPerson->getLogEntry().getCurrentCount();

	Point pre_location;
	pre_location.init(getPoint().n[VX],getPoint().n[VY],getPoint().n[VZ]);
	//get pretrack if it exist
	MobEventStruct *PreTrack = m_pPerson->getLogEntry().getPreEvent();	
	//if( lLogCount >=1 )
	//{
	//	PreTrack = m_pPerson->getLogEntry().getEvent( lLogCount-1 );	
	//}

	if(  PreTrack == NULL || PreTrack->followowner == false )
	{
		if( getPoint() != getDest() )
		{
			ARCVector3 vectorPt(getPoint(), getDest()); 
			Point vector;
			vector.init(vectorPt.n[VX],vectorPt.n[VY],vectorPt.n[VZ]);
			Point latPoint (vector.perpendicular());
			latPoint.length (m_pPerson->getSideStep());

			setLocation(getDest()+latPoint);
			setDestination(getPoint());
		}
	}
	else	//PreTrack.followowner = true
	{
		//write the first log item
		if(pOwnerPax) 
		{
			writeCurrentLogItem(pOwnerPax,_OwnerPreLocation, firstLog);
			
			MobEventStruct evntItem = *PreTrack;

			// add the first log entry
			evntItem.x = static_cast<float>(getPoint().n[VX]);
			evntItem.y = static_cast<float>(getPoint().n[VY]);
			evntItem.z = static_cast<short>(getPoint().n[VZ]);

			//if the previous item is terminal log
			//here need to set the flag to real Z
			evntItem.m_IsRealZ = TRUE;
			evntItem.m_RealZ= static_cast<float>(getPoint().n[VZ]);
		
			m_pPerson->getLogEntry().addEvent (evntItem);
		}

		// use following code to repeat last log for all follower one by one
		RepeatFollowersLastLogWithNewDest();		

		// write the second log item
		writeCurrentLogItem(pOwnerPax,_OwnerPreLocation, secondLog);
	}	


	track.x = static_cast<float>(getPoint().n[VX]);
	track.y = static_cast<float>(getPoint().n[VY]);
	track.z = static_cast<short>(getPoint().n[VZ]);
	track.m_RealZ = (float)ptRelatePos.getZ() + track.z;
	/////////////////////////////////////////////////////////
	m_pPerson->getLogEntry().addEvent (track);
	//TRACE("\r\n 2Time: %d   X = %f, Y = %f, Z = %f \r\n", track.time, track.x, track.y, track.z);
	writeFollowerLog(time,_bBackup, bOffset);
	return true;
}

void VisitorLandsideBehavior::writeCurrentLogItem(PaxLandsideBehavior* pOwnerPax,Point _OwnerPreLocation, theVisitorLogNum logNum/* = secondLog*/,MobDir emWalk/* = FORWARD*/)
{
	ASSERT(firstLog == logNum || secondLog == logNum);

	if (pOwnerPax == NULL)
		return;

	Point OwnerLocation;
	OwnerLocation.init(pOwnerPax->getPoint().n[VX],pOwnerPax->getPoint().n[VY],pOwnerPax->getPoint().n[VZ]); 

	if( _OwnerPreLocation == OwnerLocation )return;



	// use vector(_OwnerPreLocation -> OwnerLocation) as passenger direction
	Point paxDirection(_OwnerPreLocation, OwnerLocation);
	Point paxLogPoint = (firstLog == logNum)?_OwnerPreLocation:OwnerLocation;

	CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pPerson->GetTerminal()->GetNonPaxRelativePosSpec();
	ASSERT( NULL != pNonPaxRelPosSpec );
	CPoint2008 ptRelatePos;
	ARCVector4 rotation;
	bool bRes = pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_pPerson->getType().GetTypeIndex());
	if (bRes)
	{
		Point vectorLat( ptRelatePos.getX(),ptRelatePos.getY(), 0.0);
		double dAngle = GetRotateAngle(emWalk);
		Point dir(paxDirection);
		dir.rotate(dAngle);
		dir.Normalize();
		vectorLat.rotate(dir, paxLogPoint);

		//	location = vectorLat;
		ARCVector3 vectorPt;
		vectorPt.n[VX] = vectorLat.getX();
		vectorPt.n[VY] = vectorLat.getY();
		vectorPt.n[VZ] = paxLogPoint.getZ();
		setLocation(vectorPt);
		SetRelativePosition(paxDirection,paxLogPoint,emWalk);
	}
	else
	{
		switch( m_pPerson->getType().GetTypeIndex() )
		{
		case 1:	//visitor  // left
			{
				Point vectorLat( paxDirection.perpendicular() );
				vectorLat.length(100.0);
				setLocation(paxLogPoint + vectorLat);
				setDestination(getPoint());
				break;
			}
		case 2:	//checked bag	//front
			{
				paxDirection.length(25.0);//( 150.0 );
				setDestination(paxLogPoint + paxDirection);
				setLocation(paxLogPoint + paxDirection);
				break;
			}
		case 3:	//hand bag	// left
			{
				Point vectorLat( paxDirection.perpendicular() );
				vectorLat.length(20.0);
				setLocation(paxLogPoint + vectorLat);
				setDestination(paxLogPoint + vectorLat);
				break;
			}
		case 4:	//carte		// front
			{
				paxDirection.length( 100.0 );
				setDestination(paxLogPoint + paxDirection);
				setLocation(paxLogPoint + paxDirection);
				break;
			}
		case 6: //dog		//right_front
			{
				Point vectorLat( paxDirection.perpendicular() );
				vectorLat.length(-100.0);
				setDestination(paxLogPoint + vectorLat);
				paxDirection.length( 100.0 );
				setDestination(paxDirection + getDest());
				setLocation(getDest());
				break;
			}
		default: //other visitor
			{
				paxDirection.length(25.0);//( 150.0 );
				setDestination(paxLogPoint + paxDirection);
				setLocation(getDest());
				break;
			}
		}	
		SetFollowerDestination( getPoint(), getDest(), Person::m_pRotation );
	}

}


// Repeat every followers' last log item in log entry with new destination set by former code
// this is useful for PaxVisitor to make a quick shift
// 
// NOTE: to use this method correctly, look at void Person::RepeatLastLogWithNewDest().
void VisitorLandsideBehavior::RepeatFollowersLastLogWithNewDest()
{
	if(m_pPerson->m_pGroupInfo->IsFollower())
		return;

	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i<nFollowerCount; i++ )
	{
		PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
		RepeatLastLogWithNewDest(_pFollower);
	}
}

void VisitorLandsideBehavior::RepeatLastLogWithNewDest(PaxVisitor* pVisitor)
{
	LandsideBaseBehavior* spLandsideBehavior = pVisitor->getLandsideBehavior();
	MobEventStruct *pPreTrack = pVisitor->getLogEntry().getPreEvent();
	if( pPreTrack )
	{

		MobEventStruct eventItem = *pPreTrack;
		eventItem.x = static_cast<float>(spLandsideBehavior->m_ptDestination.n[VX]);
		eventItem.y = static_cast<float>(spLandsideBehavior->m_ptDestination.n[VY]);
		eventItem.z = static_cast<short>(spLandsideBehavior->m_ptDestination.n[VZ]);

		//if the previous item is terminal log
		//here need to set the flag to real Z
		eventItem.m_IsRealZ = TRUE;
		eventItem.m_RealZ= static_cast<float>(spLandsideBehavior->m_ptDestination.n[VZ]);


		pVisitor->getLogEntry().addEvent(eventItem);
	}
}
//only used for passenger
void VisitorLandsideBehavior::UpdateEntryTime( const ElapsedTime& eEntryTime )
{
	//him self
	m_pPerson->getLogEntry().setEntryTime(eEntryTime);

	//follows
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(pGroupLeaderInfo->isInGroup() && !pGroupLeaderInfo->IsFollower())
	{
		MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
		int nFollowerCount = elemList.getCount();						

		for( int i=0; i< nFollowerCount; i++ )
		{
			Person* _pFollower = (Person*) elemList.getItem( i );
			if(_pFollower == NULL)
				continue;  
			VisitorLandsideBehavior *pVisitor = (VisitorLandsideBehavior *)_pFollower->getLandsideBehavior();
			pVisitor->UpdateEntryTime(eEntryTime );

		}
	}

}
void VisitorLandsideBehavior::setDestination( const ARCVector3& p ,MobDir emWalk )
{
	//destination is equal to current location
	ASSERT(m_pPerson->getType().GetTypeIndex() != 0);
	
	//carrion that defines relative position should use own relative position
	PaxOnboardBaseBehavior* spCarrionBehavior = m_pPerson->getOnboardBehavior();
	//ASSERT(spCarrionBehavior);
	if (!m_pPerson->m_pGroupInfo->IsFollower())
	{
		PaxVisitor* pVisitor = (PaxVisitor*)m_pPerson;
		Passenger* pOwner = pVisitor->GetOwner();
		if (pOwner && pOwner->GetVisitorItemByType(pVisitor->getType().GetTypeIndex()))//does not separate with passenger
		{
			CGroupLeaderInfo* pGroupLeader = (CGroupLeaderInfo*)(m_pPerson->m_pGroupInfo);
			if (pGroupLeader->isInGroup())//separate with group
			{
				LandsideBaseBehavior* spOwnerBehavior = pOwner->getLandsideBehavior();
				ASSERT(spOwnerBehavior);
				ARCVector3 _OwnerPreLocation = spOwnerBehavior->location;
				ARCVector3 OwnerLocation = spOwnerBehavior->m_ptDestination;

				if (_OwnerPreLocation == OwnerLocation && spOwnerBehavior->getState() != EntryLandside)//can not calculate direction of passenger and current location
				{
					m_ptDestination = p;
					SetFollowerDestination(location,m_ptDestination,Person::m_pRotation);
					return;
				}

				ARCVector3 paxDirection(_OwnerPreLocation, OwnerLocation);
				ARCVector3 paxLogPoint = _OwnerPreLocation;

				Point ptDir(paxDirection[VX],paxDirection[VY],paxDirection[VZ]);
				Point ptCenter(paxLogPoint.n[VX],paxLogPoint.n[VY],paxLogPoint.n[VZ]);

				CNonPaxRelativePosSpec* pNonPaxRelPosSpec = pVisitor->GetTerminal()->GetNonPaxRelativePosSpec();
				ASSERT(pNonPaxRelPosSpec);
				CPoint2008 ptRelatePos;
				ARCVector4 rotation;
				bool bRes = pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, pVisitor->getType().GetTypeIndex());
				if (bRes)
				{
					Point vectorLat( ptRelatePos.getX(),ptRelatePos.getY(), 0.0);
					double dAngle = GetRotateAngle(emWalk);
					Point dir(paxDirection.n[VX],paxDirection.n[VY],paxDirection.n[VZ]);
					Point ptRotate(paxLogPoint.n[VX],paxLogPoint.n[VY],paxLogPoint.n[VZ]);
					dir.rotate(dAngle);
					dir.Normalize();
					vectorLat.rotate(dir, ptRotate);
					ARCVector3 vectorPt;
					vectorPt.n[VX] = vectorLat.getX();
					vectorPt.n[VY] = vectorLat.getY();
					vectorPt.n[VZ] = paxLogPoint.n[VZ];
					setLocation(vectorPt);

					SetRelativePosition(ptDir,ptCenter,emWalk);
					return;
				}
			}	
		}
		m_ptDestination = p;
		SetFollowerDestination(location,m_ptDestination,Person::m_pRotation);
	}

	//separate with passenger or not in group
	m_ptDestination = p;
	
}
