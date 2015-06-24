#include "stdafx.h"
#include "PaxOnboardBaseBehavior.h"
#include "person.h"
#include "pax.h"
#include "visitor.h"
#include "GroupLeaderInfo.h"
#include "MobElementsDiagnosis.h"
#include "Common/Point2008.h"
#include "PaxOnboardFreeMovingLogic.h"
#include "OnboardCellFreeMoveLogic.h"
#include "OnboardDeckGroundInSim.h"
#include "OnboardFlightInSim.h"
#include "OnBoardingStairInSim.h"
#include "OnBoardingElevatorInSim.h"
#include "OnBoardingEscalatorInSim.h"
#include "OnBoardDeviceInSim.h"
#include "EntryPointInSim.h"
#include "OnboardSeatInSim.h"
#include "Inputs/NonPaxRelativePosSpec.h"
#include "terminal.h"
#include "Inputs/MobileElemConstraint.h"
#include "CarryonVolumeInSim.h"
#include "OnboardStorageInSim.h"

#pragma warning (disable:4244)


class CMobDebugLog
{
public:
	static CString strFolder;
	static void InitLog( int nID)
	{		
		CString debugfileName;
		debugfileName.Format( "%s%d.txt",strFolder, nID);
		std::ofstream outfile;
		outfile.open(debugfileName);		
		outfile.close();	
	}


	static void LogEvent( int nID, const MobEventStruct& event)
	{	
		CString debugfileName;
		debugfileName.Format( "%s%d.txt",strFolder,nID);	
		//log to flight file
		std::ofstream outfile;
		outfile.open(debugfileName,std::ios::app);

		outfile <<long(event.time)<<'\t';
		outfile << "(" <<event.x <<" ," <<event.y << ","<<event.z<<")"<<'\t';
		outfile << event.m_IsRealZ?"true":"false";
		outfile << event.m_RealZ;
		outfile << std::endl;
		
		outfile.close();		
	}
};
CString CMobDebugLog::strFolder = _T("C:\\paxlog\\");


PaxOnboardBaseBehavior::PaxOnboardBaseBehavior(Person* pPerson)
:m_pPerson(pPerson)
,m_pSeat(NULL)
,m_pOnboardFlight(NULL)
,m_pDeviceInSim(NULL)
,m_pDoor(NULL)
,m_bCouldOverWirteReseve(true)
{
	m_pCellFreeMoveLogic = new OnboardCellFreeMoveLogic(this);
	m_lCellIndex = 0;

	m_bMovable = true;
	m_pWaitPax = NULL;

	m_pCurResource = NULL;

	m_dVolume = -1.0;

	CMobDebugLog::InitLog(pPerson->getID());
}

PaxOnboardBaseBehavior::~PaxOnboardBaseBehavior()
{
	//delete m_freeMovingLogic;m_freeMovingLogic = NULL;
}

char PaxOnboardBaseBehavior::GetWalkDir(MobDir emWalk)const
{
	if (emWalk == BACKWARD)
	{
		return 'B';
	}
	else if (emWalk == LEFTSIDLE)
	{
		return 'L';
	}
	else if (emWalk == RIGHTSIDLE)
	{
		return 'R';
	}

	return 'F';
}



void PaxOnboardBaseBehavior::writeSidleLog(ElapsedTime time,MobDir emWalk /* = */ )
{
	ARCVector3 locationPt = location;
	
	if (m_pPerson->getType().GetTypeIndex() != 0)
	{
		PaxVisitor* pVisitor = (PaxVisitor*)m_pPerson;
		WriteVisitorEntrySidleLog(locationPt,pVisitor,time,emWalk);
	}
	else
	{
		if (location != m_ptDestination)
		{
			location = m_ptDestination;
			setDestination( location );
		}

		MobEventStruct track;
		track.backup = false;
		track.m_motionDir = GetWalkDir(emWalk); 

		track.time = (long)time;
		track.state = m_pPerson->getState();
		track.procNumber = -1;
		track.bDynamicCreatedProc = false;
		track.followowner	= false;
		track.reason = -1;
		track.m_IsRealZ = TRUE;
		track.speed = (float)m_pPerson->getSpeed();
		//track.speed = (float)m_lCellIndex;

		track.x = (float)getPoint().n[VX];
		track.y = (float)getPoint().n[VY];
		track.m_RealZ= (float)getPoint().n[VZ];

		m_pPerson->getLogEntry().addEvent (track);
		MobElementsDiagnosis()->Diagnose( m_pPerson, time);
		// write log for follower.
		writeVisitorSidleLog(locationPt,time,emWalk);
	}
}

bool PaxOnboardBaseBehavior::WriteVisitorEntrySidleLog(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time,int ntype,MobDir emWalk /* = FORWARD */)
{
	MobEventStruct track;
	track.time = (long)time;
	track.m_motionDir = GetWalkDir(emWalk); 
	track.state = m_pPerson->getState();
	track.procNumber = -1;
	track.bDynamicCreatedProc = FALSE;
	track.reason = -1;
	track.backup = false;
	track.followowner	= true;
	track.m_IsRealZ = TRUE;
	PaxOnboardBaseBehavior* spOnboardBehavior = pVisitor->getOnboardBehavior();
	if (spOnboardBehavior == NULL)
		return false;

	//visitor follow with passenger
	if(pVisitor->getType().GetTypeIndex() != 0 && m_pPerson->getType().GetTypeIndex() == 0)
	{
		// get object's Altitude
		CNonPaxRelativePosSpec* pNonPaxRelPosSpec = pVisitor->GetTerminal()->GetNonPaxRelativePosSpec();
		ASSERT( NULL != pNonPaxRelPosSpec );
		CPoint2008 ptRelatePos;
		ARCVector4 rotation;
		pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, pVisitor->getType().GetTypeIndex(),ntype);
		track.m_RealZ = (float)ptRelatePos.getZ();
		track.m_bWithOwner = true;

		if (track.m_bWithOwner)
		{
			track.m_bVisible = pNonPaxRelPosSpec->GetTypeItemVisible(pVisitor->getType().GetTypeIndex());
		}
	}

	spOnboardBehavior->pure_location= spOnboardBehavior->m_ptDestination;
	if (spOnboardBehavior->location != spOnboardBehavior->m_ptDestination)
	{
		spOnboardBehavior->location = spOnboardBehavior->m_ptDestination;
		spOnboardBehavior->setDestination( spOnboardBehavior->location );
	}
	
	track.x = spOnboardBehavior->getPoint().n[VX];
	track.y = spOnboardBehavior->getPoint().n[VY];
	track.z = spOnboardBehavior->getPoint().n[VZ];
	track.m_RealZ += track.z;
	pVisitor->getLogEntry().addEvent (track); // if simulation stops here, it means memory insufficient.
	writeFollowerSidleLog(spOnboardBehavior->getPoint(), pVisitor, time,emWalk);
	return true;
}

bool PaxOnboardBaseBehavior::AddVisitorSidleTrack(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time,MobDir emWalk /* = FORWARD */)
{
	PaxOnboardBaseBehavior* pOnboardBehavior = pVisitor->getOnboardBehavior();
	if (pOnboardBehavior == NULL)
		return false;

	MobEventStruct track;
	track.time = (long)time;
	track.m_motionDir = GetWalkDir(emWalk); 
	track.state = m_pPerson->getState();
	track.procNumber = -1;
	track.bDynamicCreatedProc = FALSE;
	track.followowner = true;
	track.reason = -1;
	track.backup = false;
	track.m_IsRealZ = TRUE;

	PaxOnboardBaseBehavior* spOwerBehavior = (PaxOnboardBaseBehavior *)m_pPerson->getOnboardBehavior();

	Point _OwnerPreLocation;
	_OwnerPreLocation.init(locationPt.n[VX],locationPt.n[VY],locationPt.n[VZ]);
	//ASSERT(m_type.GetTypeIndex() != 0);
	CNonPaxRelativePosSpec* pNonPaxRelPosSpec = pVisitor->GetTerminal()->GetNonPaxRelativePosSpec();
	ASSERT( NULL != pNonPaxRelPosSpec );
	CPoint2008 ptRelatePos;
	ARCVector4 rotation;
	pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, pVisitor->getType().GetTypeIndex() );
	track.m_bWithOwner = true;
	track.m_bVisible = pNonPaxRelPosSpec->GetTypeItemVisible(pVisitor->getType().GetTypeIndex());
	

	/////////////////////////////////////////////////////////
	//MobEventStruct PreTrack;				// pretrack
	//long lLogCount = pVisitor->getLogEntry().getCurrentCount();

	Point pre_location;
	pre_location.init(pOnboardBehavior->getPoint().n[VX],pOnboardBehavior->getPoint().n[VY],pOnboardBehavior->getPoint().n[VZ]);
	//get pretrack if it exist
	//if( lLogCount >=1 )
	//{
	//	PreTrack = pVisitor->getLogEntry().getEvent( lLogCount-1 );	
	//}
	MobEventStruct* pPreTrack = pVisitor->getLogEntry().getPreEvent();		
	if(  pPreTrack == NULL || pPreTrack->followowner == false )
	{
		if( pOnboardBehavior->getPoint() != pOnboardBehavior->getDest() )
		{
			ARCVector3 vectorPt(pOnboardBehavior->getPoint(), pOnboardBehavior->getDest()); 
			Point vector;
			vector.init(vectorPt.n[VX],vectorPt.n[VY],vectorPt.n[VZ]);
			Point latPoint (vector.perpendicular());
			latPoint.length (pVisitor->getSideStep());

			pOnboardBehavior->setLocation(pOnboardBehavior->getDest()+latPoint);
			pOnboardBehavior->setDestination(pOnboardBehavior->getPoint());
		}
	}
	else	//PreTrack.followowner = true
	{
		//write the first log item
		if(spOwerBehavior) 
		{
			writeCurrentLogItem(pVisitor,_OwnerPreLocation, firstLog,emWalk);
			// add the first log entry
			MobEventStruct eventItem = *pPreTrack;
			eventItem.x = pOnboardBehavior->getPoint().n[VX];
			eventItem.y = pOnboardBehavior->getPoint().n[VY];
			eventItem.z = pOnboardBehavior->getPoint().n[VZ];
			pVisitor->getLogEntry().addEvent (eventItem);
		}

		// use following code to repeat last log for all follower one by one
		RepeatFollowersLastLogWithNewDest(pVisitor);		

		// write the second log item
		writeCurrentLogItem(pVisitor,_OwnerPreLocation, secondLog,emWalk);
	}	


	track.x = pOnboardBehavior->getPoint().n[VX];
	track.y = pOnboardBehavior->getPoint().n[VY];
	track.z = pOnboardBehavior->getPoint().n[VZ];
	track.m_RealZ = (float)ptRelatePos.getZ() + track.z;
	/////////////////////////////////////////////////////////
	pVisitor->getLogEntry().addEvent (track);

	writeFollowerSidleLog(locationPt, pVisitor, time,emWalk);
	return true;
}

ARCVector3 PaxOnboardBaseBehavior::GetRelativePosition()
{
	CPoint2008 ptRelatePos;

	CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pPerson->GetTerminal()->GetNonPaxRelativePosSpec();
	ASSERT( NULL != pNonPaxRelPosSpec );
	int nType = 0;
	if (m_pPerson->m_pGroupInfo->IsFollower())
	{
		CGroupLeaderInfo* pLeaderGroup =(CGroupLeaderInfo*)(m_pPerson->m_pGroupInfo)->GetGroupLeader();

		MobileElementList &elemList = pLeaderGroup->GetFollowerList();
		int nFollowerCount = elemList.getCount();
		for( int i=0; i< nFollowerCount; i++ )
		{
			PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
			if(_pFollower == m_pPerson)
			{
				nType = i + 1;
				break;
			}
		}
	}
	
	ARCVector4 rotation;
	pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_pPerson->getType().GetTypeIndex(),nType);

	return ARCVector3(ptRelatePos);
}

bool PaxOnboardBaseBehavior::WriteVisitorEntryLog(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time,int ntype,bool _bBackup /* = false */, bool bOffset /* = true */ )
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
	PaxOnboardBaseBehavior* spOnboardBehavior = pVisitor->getOnboardBehavior();
	if (spOnboardBehavior == NULL)
		return false;

	//resource index 
	if(spOnboardBehavior->GetCurrentResource())
		track.procNumber = spOnboardBehavior->GetCurrentResource()->GetResourceID();
	else
		track.procNumber = -1;

	//visitor follow with passenger
	if(pVisitor->getType().GetTypeIndex() != 0 && m_pPerson->getType().GetTypeIndex() == 0)
	{
		// get object's Altitude
		CNonPaxRelativePosSpec* pNonPaxRelPosSpec = pVisitor->GetTerminal()->GetNonPaxRelativePosSpec();
		ASSERT( NULL != pNonPaxRelPosSpec );
		CPoint2008 ptRelatePos;
		ARCVector4 rotation;
		pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, pVisitor->getType().GetTypeIndex(),ntype);
		track.m_RealZ = (float)ptRelatePos.getZ();
		track.m_bWithOwner = true;

		if (track.m_bWithOwner)
		{
			track.m_bVisible = pNonPaxRelPosSpec->GetTypeItemVisible(pVisitor->getType().GetTypeIndex());
		}
	}
	
	spOnboardBehavior->pure_location= spOnboardBehavior->m_ptDestination;
	if (spOnboardBehavior->location != spOnboardBehavior->m_ptDestination)
	{
		spOnboardBehavior->location = spOnboardBehavior->m_ptDestination;
		spOnboardBehavior->setDestination( spOnboardBehavior->location );
	}
	

	track.x = spOnboardBehavior->getPoint().n[VX];
	track.y = spOnboardBehavior->getPoint().n[VY];
	track.z = spOnboardBehavior->getPoint().n[VZ];
	track.m_RealZ += track.z;

	//TRACE("\r\nTime: %d   X = %f, Y = %f, Z = %f \r\n", track.time, track.x, track.y, track.z);
	pVisitor->getLogEntry().addEvent (track); // if simulation stops here, it means memory insufficient.

	writeFollowerLog(locationPt, pVisitor, time,_bBackup, bOffset);
	return true;
}

bool PaxOnboardBaseBehavior::AddVisitorTrack(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time, bool _bBackup /* = false */, bool bOffset /* = true */)
{
	PaxOnboardBaseBehavior* pOnboardBehavior = pVisitor->getOnboardBehavior();
	if (pOnboardBehavior == NULL)
		return false;

	MobEventStruct track;
	track.time = (long)time;
	track.state = m_pPerson->getState();
	track.procNumber = -1;
	track.bDynamicCreatedProc = FALSE;
	track.followowner	= true;
	track.reason = -1;
	track.backup = _bBackup;
	track.m_IsRealZ = TRUE;


	PaxOnboardBaseBehavior* spOwerBehavior = (PaxOnboardBaseBehavior *)m_pPerson->getOnboardBehavior();
	
	//resource index 
	if(pOnboardBehavior->GetCurrentResource())
		track.procNumber = pOnboardBehavior->GetCurrentResource()->GetResourceID();
	else
		track.procNumber = -1;


	Point _OwnerPreLocation;
	_OwnerPreLocation.init(locationPt.n[VX],locationPt.n[VY],locationPt.n[VZ]);

	ARCVector3 vectorLocation;
	vectorLocation = pOnboardBehavior->getPoint();
	//ASSERT(m_type.GetTypeIndex() != 0);
	CNonPaxRelativePosSpec* pNonPaxRelPosSpec = pVisitor->GetTerminal()->GetNonPaxRelativePosSpec();
	ASSERT( NULL != pNonPaxRelPosSpec );
	CPoint2008 ptRelatePos;
	ARCVector4 rotation;
	pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, pVisitor->getType().GetTypeIndex() );
	track.m_bWithOwner = true;
	track.m_bVisible = pNonPaxRelPosSpec->GetTypeItemVisible(pVisitor->getType().GetTypeIndex());
	

	/////////////////////////////////////////////////////////
	MobEventStruct *pPreTrack = NULL;				// pretrack
	long lLogCount = pVisitor->getLogEntry().getCurrentCount();

	Point pre_location;
	pre_location.init(pOnboardBehavior->getPoint().n[VX],pOnboardBehavior->getPoint().n[VY],pOnboardBehavior->getPoint().n[VZ]);
	//get pretrack if it exist
	//if( lLogCount >=1 )
	//{
	//	PreTrack = pVisitor->getLogEntry().getEvent( lLogCount-1 );	
	//}
	pPreTrack =  pVisitor->getLogEntry().getPreEvent();
	if(  pPreTrack == NULL || pPreTrack->followowner == false )
	{
		if( pOnboardBehavior->getPoint() != pOnboardBehavior->getDest() )
		{
			ARCVector3 vectorPt(pOnboardBehavior->getPoint(), pOnboardBehavior->getDest()); 
			Point vector;
			vector.init(vectorPt.n[VX],vectorPt.n[VY],vectorPt.n[VZ]);
			Point latPoint (vector.perpendicular());
			latPoint.length (pVisitor->getSideStep());

			pOnboardBehavior->setLocation(pOnboardBehavior->getDest()+latPoint);
			pOnboardBehavior->setDestination(pOnboardBehavior->getPoint());
		}
	}
	else	//PreTrack.followowner = true
	{
		//write the first log item
		if(spOwerBehavior) 
		{
			writeCurrentLogItem(pVisitor,_OwnerPreLocation, firstLog);
			// add the first log entry
			MobEventStruct eventItem = *pPreTrack;
			eventItem.x = pOnboardBehavior->getPoint().n[VX];
			eventItem.y = pOnboardBehavior->getPoint().n[VY];
			eventItem.z = pOnboardBehavior->getPoint().n[VZ];
			pVisitor->getLogEntry().addEvent (eventItem);
		}

		// use following code to repeat last log for all follower one by one
		RepeatFollowersLastLogWithNewDest(pVisitor);		

		// write the second log item
		writeCurrentLogItem(pVisitor,_OwnerPreLocation, secondLog);
	}	


	track.x = pOnboardBehavior->getPoint().n[VX];
	track.y = pOnboardBehavior->getPoint().n[VY];
	track.z = pOnboardBehavior->getPoint().n[VZ];
	track.m_RealZ = (float)ptRelatePos.getZ() + track.z;
	/////////////////////////////////////////////////////////
	pVisitor->getLogEntry().addEvent (track);
	//TRACE("\r\n 2Time: %d   X = %f, Y = %f, Z = %f \r\n", track.time, track.x, track.y, track.z);
	writeFollowerLog(vectorLocation, pVisitor, time,_bBackup, bOffset);
	return true;
}

void PaxOnboardBaseBehavior::writeFollowerLog(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time, bool _bBackup /* = false */, bool bOffset /* = true */ )
{
	ASSERT(pVisitor);
	if(pVisitor == NULL || pVisitor->m_pGroupInfo->IsFollower())
		return;

	//write log for follower
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVisitor->m_pGroupInfo;
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

		WriteVisitorEntryLog(locationPt,_pFollower,time,i+1,_bBackup,bOffset);
	}
}

void PaxOnboardBaseBehavior::writeFollowerSidleLog(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time,MobDir emWalk /* = FORWARD */)
{
	ASSERT(pVisitor);
	if(pVisitor == NULL || pVisitor->m_pGroupInfo->IsFollower())
		return;
 
	//write log for follower
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVisitor->m_pGroupInfo;
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

		WriteVisitorEntrySidleLog(locationPt,_pFollower,time,i+1,emWalk);
	}
}

void PaxOnboardBaseBehavior::writeVisitorLog(const ARCVector3& locationPt,ElapsedTime time, bool _bBackup /* = false */, bool bOffset /* = true */ )
{
	int nCount = ((Passenger*)m_pPerson)->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = ((Passenger*)m_pPerson)->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		
		AddVisitorTrack(locationPt,pVis,time,_bBackup,bOffset);
	}
}

void PaxOnboardBaseBehavior::writeVisitorSidleLog(const ARCVector3& locationPt,ElapsedTime time,MobDir emWalk /* = FORWARD */)
{
	int nCount = ((Passenger*)m_pPerson)->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = ((Passenger*)m_pPerson)->m_pVisitorList[i];
		if (pVis == NULL)
			continue;

		AddVisitorSidleTrack(locationPt,pVis,time,emWalk);
	}
}

void PaxOnboardBaseBehavior::writePassengerLog(ElapsedTime time, bool _bBackup /* = false */, bool bOffset /* = true */ )
{
	if (m_pPerson->getType().GetTypeIndex() != 0)
		return;
	
	if (location != m_ptDestination)
	{
		location = m_ptDestination;

		m_ptDestination = location;
	}

	MobEventStruct track;
	track.backup = _bBackup; 

	track.time = (long)time;
	track.state = m_pPerson->getState();
	track.procNumber = -1;
	track.bDynamicCreatedProc = false;
	track.followowner	= false;
	track.reason = -1;
	track.m_IsRealZ = TRUE;
	track.speed = (float)m_pPerson->getSpeed();
	//track.speed = (float)m_lCellIndex;

	track.x = (float)getPoint().n[VX];
	track.y = (float)getPoint().n[VY];
	track.m_RealZ= (float)getPoint().n[VZ];

	m_pPerson->getLogEntry().addEvent (track);

	MobElementsDiagnosis()->Diagnose( m_pPerson, time);
}

//passenger walk onboard mode does not have group concept. Just has non passenger
void PaxOnboardBaseBehavior::writeLog(ElapsedTime time, bool _bBackup, bool bOffset /* = true */ )
{
	ARCVector3 locationPt = location;

	if (m_pPerson->getType().GetTypeIndex() != 0)
	{
		PaxVisitor* pVisitor = (PaxVisitor*)m_pPerson;
		WriteVisitorEntryLog(locationPt,pVisitor,time,_bBackup,bOffset);
	}
	else
	{
		if (location != m_ptDestination)
		{
			if(bOffset)
				location = m_ptDestination;
			else
				location = m_ptDestination;

			setDestination( location );
		}

		MobEventStruct track;
		track.backup = _bBackup; 

		track.time = (long)time;
		track.state = m_pPerson->getState();
		
		//resource index 
		if(GetCurrentResource())
			track.procNumber = GetCurrentResource()->GetResourceID();
		else
			track.procNumber = -1;

		track.bDynamicCreatedProc = false;
		track.followowner	= false;
		track.reason = -1;
		track.m_IsRealZ = TRUE;


		track.speed = (float)m_pPerson->getSpeed();
		//track.speed = (float)m_lCellIndex; for debug

		track.x = (float)getPoint().n[VX];
		track.y = (float)getPoint().n[VY];
		track.m_RealZ= (float)getPoint().n[VZ];

		m_pPerson->getLogEntry().addEvent (track);
		CMobDebugLog::LogEvent(m_pPerson->getID(),track);

		MobElementsDiagnosis()->Diagnose( m_pPerson, time);
		//write non passenger which owner take
		writeVisitorLog(locationPt,time,_bBackup,bOffset);
	}
}

void PaxOnboardBaseBehavior::setDestination(const ARCVector3& p,MobDir emWalk /*= FORWARD*/)
{
	//destination is equal to current location
	if (m_pPerson->getType().GetTypeIndex() == 0)
	{
		m_ptDestination = p;
		SetFollowerDestination(location,m_ptDestination,Person::m_pRotation);
		//carrion set destination
		int nCount = ((Passenger*)m_pPerson)->m_pVisitorList.size();
		for( int i=nCount-1; i>=0; i-- )
		{
			PaxVisitor* pVis = ((Passenger*)m_pPerson)->m_pVisitorList[i];
			if( pVis )
			{
				PaxOnboardBaseBehavior* spFollowerBehavior = pVis->getOnboardBehavior();
				if (spFollowerBehavior)
				{
					spFollowerBehavior->setDestination( p );
				}
			}
		}
	}
	else // carrion
	{
		//carrion that defines relative position should use own relative position
		PaxOnboardBaseBehavior* spCarrionBehavior = m_pPerson->getOnboardBehavior();
		ASSERT(spCarrionBehavior);
		if (!m_pPerson->m_pGroupInfo->IsFollower())
		{
			PaxVisitor* pVisitor = (PaxVisitor*)m_pPerson;
			Passenger* pOwner = pVisitor->GetOwner();
			if (pOwner && pOwner->GetVisitorItemByType(pVisitor->getType().GetTypeIndex()))//does not separate with passenger
			{
				CGroupLeaderInfo* pGroupLeader = (CGroupLeaderInfo*)(m_pPerson->m_pGroupInfo);
				if (pGroupLeader->isInGroup())//separate with group
				{
					PaxOnboardBaseBehavior* spOwnerBehavior = pOwner->getOnboardBehavior();
					ASSERT(spOwnerBehavior);
					ARCVector3 _OwnerPreLocation = spOwnerBehavior->getPoint();
					ARCVector3 OwnerLocation = spOwnerBehavior->getDest();

					if (_OwnerPreLocation == OwnerLocation && spOwnerBehavior->getState() != EntryOnboard)//can not calculate direction of passenger and current location
						return;

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
		}
		
		//separate with passenger or not in group
		m_ptDestination = p;
		SetFollowerDestination(location,m_ptDestination,Person::m_pRotation);
	}
}

void PaxOnboardBaseBehavior::SetRelativePosition(const Point& paxDirection,const Point& paxLocation,MobDir emWalk)
{
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup() || m_pPerson->m_pGroupInfo->IsFollower())
		return;
	m_ptDestination = getPoint();

	Person* pLeader = pGroupLeaderInfo->GetGroupLeader();
	PaxOnboardBaseBehavior* pLeaderBehavior = pLeader->getOnboardBehavior();
	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;  

		PaxOnboardBaseBehavior* pFollowerBehavior = _pFollower->getOnboardBehavior();
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

void PaxOnboardBaseBehavior::SetFollowerDestination(const ARCVector3& _ptCurrent, const ARCVector3& _ptDestination, float* _pRotation )
{
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup() || m_pPerson->m_pGroupInfo->IsFollower())
		return;
	Person* pLeader = pGroupLeaderInfo->GetGroupLeader();
	PaxOnboardBaseBehavior* pLeaderBehavior = pLeader->getOnboardBehavior();

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
		
		PaxOnboardBaseBehavior* spFollowerBehavior = _pFollower->getOnboardBehavior();
		if( _pFollower->getState() != Death )
			spFollowerBehavior->setDestination( ARCVector3(ptDestination) );
	}
}

void PaxOnboardBaseBehavior::SetEnterOnboardLocation(const ARCVector3& _ptCurrent)
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
			PaxOnboardBaseBehavior* spFollowerBehavior = pVis->getOnboardBehavior();
			if (spFollowerBehavior)
			{
				spFollowerBehavior->SetEnterOnboardLocation( _ptCurrent );
			}
		}
	}
}

void PaxOnboardBaseBehavior::SetFollowerLocation(const ARCVector3& _ptCurrent)
{
	if ( m_pPerson->m_pGroupInfo->IsFollower())
		return;
	
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup())
		return;
	Person* pLeader = pGroupLeaderInfo->GetGroupLeader();
	PaxOnboardBaseBehavior* pLeaderBehavior = pLeader->getOnboardBehavior();

	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;  

		PaxOnboardBaseBehavior* spAirsideBehavior = _pFollower->getOnboardBehavior();
		if( _pFollower->getState() != Death )
			spAirsideBehavior->SetEnterOnboardLocation( ARCVector3(_ptCurrent) );
	}
}

double PaxOnboardBaseBehavior::GetSpeed(MobDir emWalk)const
{
	if (m_pPerson == NULL)
	{
		return 0.0;
	}

	if (emWalk == LEFTSIDLE || emWalk == RIGHTSIDLE)
	{
		return 0.5 * m_pPerson->getSpeed()*0.5;
	}

	return m_pPerson->getSpeed()*0.5;
}

ElapsedTime PaxOnboardBaseBehavior::sidleMoveTime(MobDir emWalk)const
{
	ElapsedTime t;
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

		double time = dL;
		t = (float) (time / GetSpeed(emWalk));
	}
	return t;
}

ElapsedTime PaxOnboardBaseBehavior::moveTime(void)const
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

ElapsedTime PaxOnboardBaseBehavior::moveTime( DistanceUnit _dExtraSpeed, bool _bExtra )const
{
	return ElapsedTime();
}

void PaxOnboardBaseBehavior::SetNextStatus( int nState, ARCVector3& pos, OnboardAircraftElementInSim *pElement )
{
	m_nextStatus.m_nState = nState;
	m_nextStatus.m_pAircraftElement = pElement;
	m_nextStatus.m_position = pos;
}

OnboardDeviceInSim* PaxOnboardBaseBehavior::SelectDeviceMoveDest(const OnboardCellInSim* pOriginalCell,const OnboardCellInSim* pDestCell)
{
	ASSERT(m_pOnboardFlight);
	CDeck* pFromDeck = pOriginalCell->GetDeckGround()->GetDeck();
	CDeck* pDestDeck = pDestCell->GetDeckGround()->GetDeck();
	std::vector<OnboardDeviceInSim*> vDeviceInSimList;
	//fit stair
	std::vector<OnboardStairInSim*>* stairList = m_pOnboardFlight->GetOnboardStairList();
	for (int i = 0; i < (int)stairList->size(); i++)
	{
		OnboardStairInSim* pStairInSim = (*stairList)[i];
		if (pStairInSim->onDeck(pFromDeck) && pStairInSim->onDeck(pDestDeck))
		{
			vDeviceInSimList.push_back(pStairInSim);
		}
	}
	//fit elevator
	std::vector<OnboardElevatorInSim*>* elevatorList = m_pOnboardFlight->GetOnboardElevatorList();
	for (int i = 0; i < (int)elevatorList->size(); i++)
	{
		OnboardElevatorInSim* pElevatorInSim = (*elevatorList)[i];
		if (pElevatorInSim->onDeck(pFromDeck) && pElevatorInSim->onDeck(pDestDeck))
		{
			vDeviceInSimList.push_back(pElevatorInSim);
		}
	}
	//fit escalator
	std::vector<OnboardEscalatorInSim*>* ecalatorList = m_pOnboardFlight->GetOnboardEscalatorList();
	for (int i = 0; i < (int)ecalatorList->size(); i++)
	{
		OnboardEscalatorInSim* pEscalatorInSim = (*ecalatorList)[i];
		if (pEscalatorInSim->onDeck(pFromDeck) && pEscalatorInSim->onDeck(pDestDeck))
		{
			vDeviceInSimList.push_back(pEscalatorInSim);
		}
	}

	if (vDeviceInSimList.empty())
		return NULL;

	int nCount = (int)vDeviceInSimList.size();

	//random
	int nRand = random(nCount);

	return vDeviceInSimList[nRand];
}

void PaxOnboardBaseBehavior::WriteCellPathLog(OnboardCellPath& cellPath,ElapsedTime& p_time)
{
	for (int i = 0; i < cellPath.getCellCount(); i++)
	{
		OnboardCellInSim* pOnboardCellInSim = cellPath.getCell(i);
		ARCVector3 dectPt(pOnboardCellInSim->getLocation());
		setDestination(dectPt);
		p_time += moveTime();
		writeLog(p_time);
	}
}

void PaxOnboardBaseBehavior::WriteSidleCellPathLog(OnboardCellPath& cellPath,ElapsedTime& p_time,MobDir emWalk)
{
	for (int i = 0; i < cellPath.getCellCount(); i++)
	{
		OnboardCellInSim* pOnboardCellInSim = cellPath.getCell(i);
		ARCVector3 dectPt(pOnboardCellInSim->getLocation());
		setDestination(dectPt);
		p_time += sidleMoveTime(emWalk);
		writeSidleLog(p_time,emWalk);
	}
}

bool PaxOnboardBaseBehavior::AddWaitingPax( PaxOnboardBaseBehavior *pBehavior )
{
	if(IsWaitingPax(pBehavior))
		return false;

	pBehavior->m_pCellFreeMoveLogic->setState(OnboardCellFreeMoveLogic::Waiting);
	m_vWaitingPax.push_back(pBehavior);
	return true;
}

void PaxOnboardBaseBehavior::WakeupWaitingPax( ElapsedTime& etime )
{
	//wait up the waiting passengers
	std::vector< PaxOnboardBaseBehavior *>::iterator iterPax = m_vWaitingPax.begin();
	for (; iterPax != m_vWaitingPax.end(); ++iterPax)
	{
		if(*iterPax)
		{
			(*iterPax)->GenerateEvent(etime);
		}
	}
	m_vWaitingPax.clear();
}

bool PaxOnboardBaseBehavior::IsWaitingPax(const PaxOnboardBaseBehavior *pBehavior ) const
{
	if(m_vWaitingPax.size() == 0)
		return false;

	//this passenger is waiting
	if(std::find(m_vWaitingPax.begin(),m_vWaitingPax.end(),pBehavior) != m_vWaitingPax.end())
		return true;

	std::vector<const PaxOnboardBaseBehavior *>  vpBehavior;
	vpBehavior.push_back(pBehavior);


	if(IsWaitingPax(vpBehavior))
		return true;

	////check the waiting passenger's waiting passenger is waiting 

	//std::vector< PaxOnboardBaseBehavior *>::const_iterator iter = m_vWaitingPax.begin();
	//for (; iter != m_vWaitingPax.end(); ++iter)
	//{
	//	PaxOnboardBaseBehavior *tmpBehavior = *iter;
	//	if(tmpBehavior && (tmpBehavior->IsWaitingPax(pBehavior) || pBehavior->IsWaitingPax(tmpBehavior)))
	//	{
	//		return true;
	//	}
	//}

	//if(std::find())
	return false;
}

bool PaxOnboardBaseBehavior::IsWaitingPax( std::vector<const PaxOnboardBaseBehavior *> vpBehavior ) const
{
	vpBehavior.push_back(this);
	std::vector< PaxOnboardBaseBehavior *>::const_iterator iter = m_vWaitingPax.begin();
	for (; iter != m_vWaitingPax.end(); ++iter)
	{	
		PaxOnboardBaseBehavior *tmpBehavior = *iter;

		std::vector<const PaxOnboardBaseBehavior *>::iterator iterPass = vpBehavior.begin();
		for (; iterPass != vpBehavior.end(); ++iterPass)
		{
			if(tmpBehavior && (tmpBehavior->IsWaitingPax(*iterPass)||(*iterPass)->IsWaitingPax(tmpBehavior) ||tmpBehavior->IsWaitingPax(vpBehavior) ))
			{
				return true;
			}
		}

	}

	return false;

}

int PaxOnboardBaseBehavior::getState( void ) const
{
	return m_pPerson->getState();
}

void PaxOnboardBaseBehavior::setState( short newState )
{
	if(m_pPerson)
		m_pPerson->setState(newState);
}

PaxCellLocation& PaxOnboardBaseBehavior::getCurLocation()
{
	return m_curLocation;
}

void PaxOnboardBaseBehavior::SetPaxLocation(const PaxCellLocation& cellLocation )
{
	m_curLocation = cellLocation;
}

////////////////////////////////////////////////////////////////////////////
// write visitor the current point
void PaxOnboardBaseBehavior::writeCurrentLogItem(PaxVisitor* pVisitor,Point _OwnerPreLocation, theVisitorLogNum logNum/* = secondLog*/,MobDir emWalk/* = FORWARD*/)
{
	ASSERT(firstLog == logNum || secondLog == logNum);


	PaxOnboardBaseBehavior* pOnboardBehavior = pVisitor->getOnboardBehavior();
	if (pOnboardBehavior == NULL)
		return;

	Passenger* pOwnerPax = (Passenger*)m_pPerson;
	if (pOwnerPax == NULL)
		return;
	
	PaxOnboardBaseBehavior* spOwerBehavior = (PaxOnboardBaseBehavior *)pOwnerPax->getOnboardBehavior();
	if (spOwerBehavior == NULL)
		return;
	
	Point OwnerLocation;
	OwnerLocation.init(spOwerBehavior->getPoint().n[VX],spOwerBehavior->getPoint().n[VY],spOwerBehavior->getPoint().n[VZ]); 
	
	if( _OwnerPreLocation == OwnerLocation )return;



	// use vector(_OwnerPreLocation -> OwnerLocation) as passenger direction
	Point paxDirection(_OwnerPreLocation, OwnerLocation);
	Point paxLogPoint = (firstLog == logNum)?_OwnerPreLocation:OwnerLocation;

	CNonPaxRelativePosSpec* pNonPaxRelPosSpec = pVisitor->GetTerminal()->GetNonPaxRelativePosSpec();
	ASSERT( NULL != pNonPaxRelPosSpec );
	CPoint2008 ptRelatePos;
	ARCVector4 rotation;
	bool bRes = pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, pVisitor->getType().GetTypeIndex());
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
		pOnboardBehavior->setLocation(vectorPt);

		pOnboardBehavior->SetRelativePosition(paxDirection,paxLogPoint,emWalk);
	}
	else
	{
		switch( pVisitor->getType().GetTypeIndex() )
		{
		case 1:	//visitor  // left
			{
				Point vectorLat( paxDirection.perpendicular() );
				vectorLat.length(100.0);
				pOnboardBehavior->setLocation(paxLogPoint + vectorLat);
				pOnboardBehavior->setDestination(pOnboardBehavior->getPoint());
				break;
			}
		case 2:	//checked bag	//front
			{
				paxDirection.length(25.0);//( 150.0 );
				pOnboardBehavior->setDestination(paxLogPoint + paxDirection);
				pOnboardBehavior->setLocation(paxLogPoint + paxDirection);
				break;
			}
		case 3:	//hand bag	// left
			{
				Point vectorLat( paxDirection.perpendicular() );
				vectorLat.length(20.0);
				pOnboardBehavior->setLocation(paxLogPoint + vectorLat);
				pOnboardBehavior->setDestination(paxLogPoint + vectorLat);
				break;
			}
		case 4:	//carte		// front
			{
				paxDirection.length( 100.0 );
				pOnboardBehavior->setDestination(paxLogPoint + paxDirection);
				pOnboardBehavior->setLocation(paxLogPoint + paxDirection);
				break;
			}
		case 6: //dog		//right_front
			{
				Point vectorLat( paxDirection.perpendicular() );
				vectorLat.length(-100.0);
				pOnboardBehavior->setDestination(paxLogPoint + vectorLat);
				paxDirection.length( 100.0 );
				pOnboardBehavior->setDestination(paxDirection + pOnboardBehavior->getDest());
				pOnboardBehavior->setLocation(pOnboardBehavior->getDest());
				break;
			}
		default: //other visitor
			{
				paxDirection.length(25.0);//( 150.0 );
				pOnboardBehavior->setDestination(paxLogPoint + paxDirection);
				pOnboardBehavior->setLocation(pOnboardBehavior->getDest());
				break;
			}
		}	
		pOnboardBehavior->SetFollowerDestination( pOnboardBehavior->getPoint(), pOnboardBehavior->getDest(), Person::m_pRotation );
	}

}

PaxOnboardBaseBehavior::MobDir PaxOnboardBaseBehavior::GetDirectionSidle( CPoint2008& dir, bool bMoveIn ) const
{
	OnboardSeatInSim* pCreateSeatInSim =m_pSeat->GetEntryPoint()->GetCreateSeat();

	OnboardCellInSim* pLastCellInSim = m_pSeat->GetEntryPoint()->GetOnboardCell();
	OnboardCellInSim* pFirstCellInSim= pCreateSeatInSim->GetOnboardCellInSim();

	CPoint2008 ptFirst = pFirstCellInSim->getLocation();
	CPoint2008 ptLast = pLastCellInSim->getLocation();
	CPoint2008 dirPt = CPoint2008(ptLast-ptFirst);
	dir = dirPt;
	dir.Normalize();


	ARCVector2 walkdir(dirPt);
	double dWalkAngle = walkdir.AngleFromCoorndinateX();

	//get the seat direction
	CPoint2008 ptSeatLocation;
	pCreateSeatInSim->GetLocation(ptSeatLocation);
	//seat front position
	ARCVector3 seatFrontVec;
	pCreateSeatInSim->GetFrontPosition(seatFrontVec);
	CPoint2008 ptSeatFrontPosition(seatFrontVec[VX],seatFrontVec[VY],seatFrontVec[VZ]);
	CPoint2008 seatDirection =CPoint2008(ptSeatFrontPosition - ptSeatLocation);
	ARCVector2 seatDir(seatDirection);
	double dSeatAngle = seatDir.AngleFromCoorndinateX();

	double dRotate = dWalkAngle - dSeatAngle;
	double dRadians = ARCMath::DegreesToRadians(dRotate);
	if(bMoveIn)
	{
		if (sin(dRadians) > 0)
		{
			return PaxOnboardBaseBehavior::LEFTSIDLE;
		}
		return PaxOnboardBaseBehavior::RIGHTSIDLE;
	}
	else
	{
		if (sin(dRadians) > 0)
		{
			return PaxOnboardBaseBehavior::RIGHTSIDLE;
		}

		return PaxOnboardBaseBehavior::LEFTSIDLE;
	}
}
// Repeat every followers' last log item in log entry with new destination set by former code
// this is useful for PaxVisitor to make a quick shift
// 
// NOTE: to use this method correctly, look at void Person::RepeatLastLogWithNewDest().
void PaxOnboardBaseBehavior::RepeatFollowersLastLogWithNewDest(PaxVisitor* pVisitor)
{
	if(pVisitor->m_pGroupInfo->IsFollower())
		return;

	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)pVisitor->m_pGroupInfo;
	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i<nFollowerCount; i++ )
	{
		PaxVisitor* _pFollower = (PaxVisitor*) elemList.getItem( i );
		RepeatLastLogWithNewDest(_pFollower);
	}
}

void PaxOnboardBaseBehavior::RepeatLastLogWithNewDest(PaxVisitor* pVisitor)
{
	//long lLogCount = pVisitor->getLogEntry().getCurrentCount();
	PaxOnboardBaseBehavior* spOnboardBehavior = pVisitor->getOnboardBehavior();
	MobEventStruct *pPreTrack = pVisitor->getLogEntry().getPreEvent(  );
	if( pPreTrack != NULL )
	{
		MobEventStruct eventItem = *pPreTrack;
		eventItem.x = spOnboardBehavior->m_ptDestination.n[VX];
		eventItem.y = spOnboardBehavior->m_ptDestination.n[VY];
		eventItem.z = spOnboardBehavior->m_ptDestination.n[VZ];

		pVisitor->getLogEntry().addEvent(eventItem);
	}
}

void PaxOnboardBaseBehavior::SetGroupBehavior()
{

	if(m_pPerson->m_pGroupInfo->IsFollower())
		return;

	//set group behavior
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i<nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );

		PaxOnboardBaseBehavior* pOnboardBehavior = _pFollower->getOnboardBehavior();
		if(pOnboardBehavior == NULL)
		{
			pOnboardBehavior = GetOnboardBehavior(_pFollower);
			_pFollower->setBehavior(pOnboardBehavior);
		}
		pOnboardBehavior->SetGroupBehavior();
	}
}

void PaxOnboardBaseBehavior::SetFollowBehavior()
{
	//passenger has visitor list
	if (m_pPerson->getType().GetTypeIndex() != 0)
		return;

	//set visitor behavior
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		Person* pVis = (Person*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		PaxOnboardBaseBehavior* pOnboardBehavior = GetOnboardBehavior(pVis);
		pVis->setBehavior(pOnboardBehavior);

		pOnboardBehavior->SetGroupBehavior();
	}
}

void PaxOnboardBaseBehavior::SetEnplaneTime(const ElapsedTime& time)
{
	m_pPerson->getLogEntry().setEnplanTime(time);
	SetFollowerEnplaneTime(time);

	if (m_pPerson->getType().GetTypeIndex() != 0)
		return;

	//set visitor enplane time
	Passenger* pPassenger = (Passenger*)m_pPerson;
	int nCount = pPassenger->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		Person* pVis = (Person*)pPassenger->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		PaxOnboardBaseBehavior* pOnboardBehavior = pVis->getOnboardBehavior();
		if (pOnboardBehavior == NULL)
		{
			pOnboardBehavior = GetOnboardBehavior(pVis);
			pVis->setBehavior(pOnboardBehavior);
		}
		pVis->getLogEntry().setEnplanTime(time);
		pOnboardBehavior->SetFollowerEnplaneTime(time);
	}
}

void PaxOnboardBaseBehavior::SetFollowerEnplaneTime(const ElapsedTime& time)
{
	if(m_pPerson->m_pGroupInfo->IsFollower())
		return;

	//set group behavior
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i<nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		
		PaxOnboardBaseBehavior* pOnboardBehavior = _pFollower->getOnboardBehavior();
		if (pOnboardBehavior == NULL)
		{
			pOnboardBehavior = GetOnboardBehavior(_pFollower);
			_pFollower->setBehavior(pOnboardBehavior);
		}
		_pFollower->getLogEntry().setEnplanTime(time);
		pOnboardBehavior->SetFollowerEnplaneTime(time);
	}
}

bool PaxOnboardBaseBehavior::bCanWaitFor( const PaxOnboardBaseBehavior* pOtherPax ) const
{
	if(pOtherPax)
	{			
		return pOtherPax->FindEndMovablePax(this)!=this;	
	}
	return true;
}

const PaxOnboardBaseBehavior* PaxOnboardBaseBehavior::FindEndMovablePax( const PaxOnboardBaseBehavior* pBreakPax ) const
{
	if(m_pWaitPax == pBreakPax)
		return pBreakPax;
	if(m_pWaitPax && m_pWaitPax->IsMovable() )
		return m_pWaitPax->FindEndMovablePax(pBreakPax);
	else if(m_pWaitPax)
		return pBreakPax;
	return NULL;
}

OnboardStorageInSim* PaxOnboardBaseBehavior::GetOnboardStorage(GroundInSim* pGroundInSim,const ARCVector3& pt)const
{
	return m_pOnboardFlight->GetOnboardStorage(pGroundInSim,pt);
}

double PaxOnboardBaseBehavior::GetWidth() const
{
	int nCount = ((Passenger*)m_pPerson)->m_pVisitorList.size();
	if(nCount)
		return 50;
	return 45;
}

double PaxOnboardBaseBehavior::GetThick() const
{
	return 20;
}

double PaxOnboardBaseBehavior::GetRadiusOfConcern() const
{
	return GetWidth()*1.5;
}

void PaxOnboardBaseBehavior::SetCurrentResource( OnboardAircraftElementInSim *pCurResource )
{
	m_pCurResource = pCurResource;
}

OnboardAircraftElementInSim * PaxOnboardBaseBehavior::GetCurrentResource() const
{
	return m_pCurResource;
}

double PaxOnboardBaseBehavior::GetVolume(OnboardFlightInSim *pFlight)
{
	if(m_dVolume >= 0.0 )
		return m_dVolume;
	if(pFlight == NULL)
		return 0.0;


	//initialize
	int nTypeIndex = m_pPerson->getType().GetTypeIndex();
	ASSERT(nTypeIndex > 0);
	CarryonVolumeInSim *pVolume = pFlight->GetCarryonVolume();
	ASSERT(pVolume != NULL);
	m_dVolume = pVolume->GetCarryOnVolume(nTypeIndex);

	return m_dVolume;
}

std::vector<OnboardStorageInSim*> PaxOnboardBaseBehavior::GetCanReachStorageDevice( GroundInSim* pGroundInSim,const ARCVector3& pt, double dReachDist ) const
{
	return m_pOnboardFlight->GetCanReachStorageDevice(pGroundInSim,pt,dReachDist);
}

void PaxOnboardBaseBehavior::SetEntryTime( const ElapsedTime& eTime )
{
	//set passenger entry time
	Passenger* pPax = (Passenger*)m_pPerson;
	pPax->getLogEntry().setEntryTime(eTime);

	//set visitor entry time
	int nCount = (int)pPax->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		Person* pVis = (Person*)pPax->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		PaxOnboardBaseBehavior* pOnboardBehavior = pVis->getOnboardBehavior();
		if (pOnboardBehavior == NULL)
		{
			pOnboardBehavior = GetOnboardBehavior(pVis);
			pVis->setBehavior(pOnboardBehavior);
		}
		pVis->getLogEntry().setEntryTime(eTime);
		pOnboardBehavior->SetFollowerEntryTime(eTime);
	}
}

void PaxOnboardBaseBehavior::SetFollowerEntryTime( const ElapsedTime& eTime )
{
	if(m_pPerson->m_pGroupInfo->IsFollower())
		return;

	//set group behavior
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i<nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );

		PaxOnboardBaseBehavior* pOnboardBehavior = _pFollower->getOnboardBehavior();
		if (pOnboardBehavior == NULL)
		{
			pOnboardBehavior = GetOnboardBehavior(_pFollower);
			_pFollower->setBehavior(pOnboardBehavior);
		}
		_pFollower->getLogEntry().setEntryTime(eTime);
		pOnboardBehavior->SetFollowerEntryTime(eTime);
	}
}

//-------------------------------------------------------------------------------------
//Summary:
//		adjust direction to move to storage
//-------------------------------------------------------------------------------------
void PaxOnboardBaseBehavior::AdjustDirectionStorage( ElapsedTime& eTime,OnboardStorageInSim* pStorageInSim,bool bDeplane)
{
	ARCVector3 vPaxPos = getPoint();
	ARCVector3 vStoragePos;
	pStorageInSim->GetPosition(vStoragePos);
	vStoragePos.n[VZ] = vPaxPos.n[VZ];

	ARCVector3 vDirFace = vStoragePos - vPaxPos;
	vDirFace.Normalize();
	vDirFace.SetLength(0.1);
	
	ARCVector3 destPos;
	if (bDeplane)
	{
		destPos = vPaxPos - vDirFace;
	}
	else
	{
		destPos = vPaxPos + vDirFace;
	}
	

	setDestination(destPos);
}