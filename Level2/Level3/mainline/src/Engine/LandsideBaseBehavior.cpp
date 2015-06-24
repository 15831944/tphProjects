#include "StdAfx.h"
#include ".\paxlandsidebehavior.h"
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
#include "LandsideObjectInSim.h"

void LandsideBaseBehavior::writeLog( ElapsedTime time, bool _bBackup, bool bOffset /*= true */ )
{
	ARCVector3 pre_location = location;

	if (m_pPerson->getType().GetTypeIndex() != 0)
	{
		PaxVisitor* pVisitor = (PaxVisitor*)m_pPerson;
		//WriteVisitorEntryLog(pre_location,pVisitor,time,_bBackup,bOffset);
	}
	else
	{
		if (location != m_ptDestination)
		{
			ARCVector3 vector (location, m_ptDestination);
			ARCVector3 latPoint (vector.PerpendicularLCopy());
			if( m_pPerson->getType().GetTypeIndex() > 1 )// only pax and visitor have side step
			{
				latPoint.SetLength( 0.0 );
			}
			else
			{
				latPoint.SetLength (m_pPerson->getSideStep());
			}

			if(bOffset)
				location = m_ptDestination  + latPoint;
			else
				location = m_ptDestination;
		}

		MobEventStruct track;
		track.backup = _bBackup; 

		track.time = (long)time;
		track.state = (short)m_pPerson->getState();

		//resource index 
		/*if(GetCurrentResource())
			track.procNumber = GetCurrentResource()->GetResourceID();
		else*/
			track.procNumber = -1;

		track.bDynamicCreatedProc = false;
		track.followowner	= false;
		track.reason = -1;
		track.m_IsRealZ = TRUE;


		track.speed = (float)m_pPerson->getSpeed();
		//track.speed = (float)m_lCellIndex; for debug

		track.x = (float)location.n[VX];
		track.y = (float)location.n[VY];
		track.m_RealZ= (float)location.n[VZ];
		track.z = 0;

		m_pPerson->getLogEntry().addEvent (track);
		//CMobDebugLog::LogEvent(m_pPerson->getID(),track);

		MobElementsDiagnosis()->Diagnose( m_pPerson, time);
		//write non passenger which owner take
		//writeVisitorLog(pre_location,time,_bBackup,bOffset);
	}
}

void LandsideBaseBehavior::SetFollowerLocation( const ARCVector3& _ptCurrent )
{
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup() || m_pPerson->m_pGroupInfo->IsFollower())
		return;

	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;  

		LandsideBaseBehavior* spLBehavior = _pFollower->getLandsideBehavior();
		if( _pFollower->getState() != Death )
			spLBehavior->SetEnterLandsideLocation( ARCVector3(_ptCurrent) );
	}
}

void LandsideBaseBehavior::SetFollowerDestination( const ARCVector3& _ptCurrent, const ARCVector3& _ptDestination, float* _pRotation )
{
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup() || m_pPerson->m_pGroupInfo->IsFollower())
		return;

	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;  
		CPoint2008 ptDestination(_ptDestination.n[VX], _ptDestination.n[VY],_ptDestination.n[VZ]);
		CPoint2008 currentPt(_ptCurrent.n[VX],_ptCurrent.n[VY],_ptCurrent.n[VZ]);
		float fDir = (i> (MAX_GROUP-1)) ? (float)0.0 : _pRotation[i+1];
		ptDestination.offsetCoords( currentPt, (double)fDir, GROUP_OFFSET );

		LandsideBaseBehavior* spFollowerBehavior = _pFollower->getLandsideBehavior();
		if( _pFollower->getState() != Death && spFollowerBehavior)
			spFollowerBehavior->setDestination( ARCVector3(ptDestination) );
	}
}

int LandsideBaseBehavior::getState( void ) const
{
	return m_pPerson->getState();
}

void LandsideBaseBehavior::SetRelativePosition( const Point& paxDirection,const Point& paxLocation,MobDir emWalk )
{
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup() || m_pPerson->m_pGroupInfo->IsFollower())
		return;
	m_ptDestination = location;

	Person* pLeader = pGroupLeaderInfo->GetGroupLeader();
	PaxOnboardBaseBehavior* pLeaderBehavior = pLeader->getOnboardBehavior();
	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;  

		LandsideBaseBehavior* pFollowerBehavior = _pFollower->getLandsideBehavior();
		if (pFollowerBehavior == NULL)
			continue;

		CNonPaxRelativePosSpec* pNonPaxRelPosSpec = _pFollower->GetTerminal()->GetNonPaxRelativePosSpec();
		ASSERT( NULL != pNonPaxRelPosSpec );
		CPoint2008 ptRelatePos;
		ARCVector4 rotation;
		bool bRes = pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, _pFollower->getType().GetTypeIndex(),i+1);

		if (bRes)
		{
			Point vectorLat( ptRelatePos.getX(),ptRelatePos.getY(), 0.0);
			Point ptCurrent(paxLocation);
			Point dir(paxDirection); 
			double dAngle = GetRotateAngle(emWalk);
			dir.rotate(dAngle);
			dir.Normalize();
			vectorLat.rotate(dir, ptCurrent);

			pFollowerBehavior->setDestination(ARCVector3(vectorLat.getX(),vectorLat.getY(),paxLocation.getZ()));
		}

	}
}

void LandsideBaseBehavior::SetEnterLandsideLocation( const ARCVector3& _ptCurrent )
{
	location = _ptCurrent;
	SetFollowerLocation(location);

	if (m_pPerson->getType().GetTypeIndex() != 0)
	{
		return;
	}

	int nCount = ((Passenger*)m_pPerson)->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = ((Passenger*)m_pPerson)->m_pVisitorList[i];
		if( pVis )
		{
			LandsideBaseBehavior* spFollowerBehavior = pVis->getLandsideBehavior();
			if (spFollowerBehavior)
			{
				spFollowerBehavior->SetEnterLandsideLocation( _ptCurrent );
			}
		}
	}
}

void LandsideBaseBehavior::GenerateEvent( ElapsedTime eventTime )
{
	GenerateEvent(m_pPerson, eventTime);
}

void LandsideBaseBehavior::GenerateEvent( Person *pPerson, ElapsedTime eventTime )
{
	bool bDisallowGroup = false;

	enum EVENT_OPERATION_DECISTION enumRes =pPerson->m_pGroupInfo->ProcessEvent( eventTime, bDisallowGroup );
	if( enumRes == REMOVE_FROM_PROCESSOR_AND_NO_EVENT )
	{
		return;
	}
	if( enumRes == NOT_GENERATE_EVENT )
		return;

	pPerson->MobileElement::generateEvent( eventTime, false);
}

ElapsedTime LandsideBaseBehavior::moveTime( void ) const
{
	ElapsedTime t;
	//	if (location.getZ() != destination.getZ() || !location)
	if (!location)
		t = 0l;
	else
	{

		double dLxy = m_ptDestination.DistanceTo(location);
		double dLz = 0.0;
		int nFloorFrom = (int)(location.z / SCALE_FACTOR);
		int nFloorTo = (int)(m_ptDestination.z / SCALE_FACTOR);
		double dL = dLxy;

		dLz = location.z - m_ptDestination.z;
		if( dLz < 0 )
			dLz = -dLz;
		Point pt(dLxy, dLz, 0.0);
		dL = pt.length();


		//		double time = destination.distance(location);
		double time = dL;
		t = (float) (time / m_pPerson->getSpeed());
	}
	return t;
}

void LandsideBaseBehavior::terminate( const ElapsedTime&t )
{
	if(getPerson()->getState()==Death)
		return;
	writeLog(t);
	getPerson()->flushLog(t);
	setState(Death);
}

void LandsideBaseBehavior::FlushLog( ElapsedTime p_time )
{
	if(m_pPerson)
		m_pPerson->flushLog(p_time,false);
}

void LandsideBaseBehavior::setState( short nState )
{
	if(m_pPerson)
		m_pPerson->setState(nState);
}

LandsideBaseBehavior::LandsideBaseBehavior( Person* p )
:m_pPerson(p)
,m_pVehicle(NULL)
,location(0.0, 0.0, 0.0)
,pure_location(0.0, 0.0, 0.0)

{
	m_pResource = NULL;
	//m_pAssCurbside=NULL;
	//m_pUseBusStation=NULL;
	//m_eUseVehicleType=Use_PrivateVehicle;
	//m_pVehicleType=NULL;
}

int LandsideBaseBehavior::GetPersonID() const
{
	if(m_pPerson){
		return m_pPerson->getID();
	}
	return -1;
}

bool LandsideBaseBehavior::CanPaxTakeThisBus( LandsideResidentVehicleInSim *pResidentVehicle, LandsideSimulation *pSimulation )
{
	ASSERT(0);
	return false;
}

bool LandsideBaseBehavior::CanPaxTkeOffThisBusAtStation( LandsideBusStationInSim* pBusStation, LandsideSimulation *pSimulation )
{
	ASSERT(0);
	return false;

}


