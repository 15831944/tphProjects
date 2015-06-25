#include "StdAfx.h"
#include ".\airsidemobelementbasebehavior.h"
#include "Person.h"
#include "..\Main\TermPlanDoc.h"
#include "GroupLeaderInfo.h"
#include "..\Main\Floor2.h"
#include "MobElementsDiagnosis.h"
#include "..\Common\ARCTracker.h"





AirsideMobElementBaseBehavior::AirsideMobElementBaseBehavior(Person* pMobileElement)
:m_pMobileElemment(pMobileElement)
{

}

AirsideMobElementBaseBehavior::~AirsideMobElementBaseBehavior(void)
{
}
void AirsideMobElementBaseBehavior::ResetTerminalToAirsideLocation()
{
	PLACE_METHOD_TRACK_STRING();
	Point point;
	m_pMobileElemment->getTerminalPoint(point);

	CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	int nFloor = (int)(point.getZ() / SCALE_FACTOR);
	if (pDoc)
	{
		double dLz = pDoc->GetFloorByMode(EnvMode_Terminal).GetFloor2(nFloor)->RealAltitude();
		setLocation(CPoint2008(point.getX(),point.getY(),dLz));
	}
	else
	{
		setLocation(CPoint2008(point.getX(),point.getY(),point.getZ()));
	}
}

ElapsedTime AirsideMobElementBaseBehavior::moveTime( void ) const
{
	PLACE_METHOD_TRACK_STRING();
	ElapsedTime t;
	//	if (location.getZ() != destination.getZ() || !location)
	if (!location)
		t = 0l;
	else
	{

		double dLxy = m_ptDestination.distance(location);
		double dLz = 0.0;
		int nFloorFrom = (int)(location.getZ() / SCALE_FACTOR);
		int nFloorTo = (int)(m_ptDestination.getZ() / SCALE_FACTOR);
		double dL = dLxy;
		
		dLz = location.getZ() - m_ptDestination.getZ();
		if( dLz < 0 )
			dLz = -dLz;
		Point pt(dLxy, dLz, 0.0);
		dL = pt.length();
		

		//		double time = destination.distance(location);
		double time = dL;
		t = (float) (time / (double)m_pMobileElemment->getSpeed());
	}
	return t;
}

ElapsedTime AirsideMobElementBaseBehavior::moveTime( DistanceUnit _dExtraSpeed, bool _bExtra ) const
{
	return ElapsedTime();
}

void AirsideMobElementBaseBehavior::setDestination( CPoint2008 p )
{
	PLACE_METHOD_TRACK_STRING();
	m_ptDestination = p;
	SetFollowerDestination(location,m_ptDestination,Person::m_pRotation);

	if(m_pMobileElemment->getLogEntry().GetMobileType() != 0)
		return ;

	int nCount = ((Passenger*)m_pMobileElemment)->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = ((Passenger*)m_pMobileElemment)->m_pVisitorList[i];
		if( pVis )
		{
			AirsideMobElementBaseBehavior* spFollowerBehavior = pVis->getAirsideBehavior();
			if (spFollowerBehavior)
			{
				spFollowerBehavior->setDestination( p );
			}
		}
	}
}

void AirsideMobElementBaseBehavior::SetFollowerDestination(const CPoint2008& _ptCurrent, const CPoint2008& _ptDestination, float* _pRotation )
{
	PLACE_METHOD_TRACK_STRING();

	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pMobileElemment->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup() || m_pMobileElemment->m_pGroupInfo->IsFollower())
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

		AirsideMobElementBaseBehavior* spFollowerBehavior = _pFollower->getAirsideBehavior();
		if( _pFollower->getState() != Death && spFollowerBehavior)
			spFollowerBehavior->setDestination(ptDestination);
	}

}

int AirsideMobElementBaseBehavior::WriteLog( ElapsedTime time,const double speed /*= 0*/, enum EntityEvents enumState /*= FreeMoving*/ )
{
	PLACE_METHOD_TRACK_STRING();
	MobEventStruct track;
	track.time = (long)time;
	track.state = enumState;
	track.procNumber = -1;
	track.bDynamicCreatedProc = FALSE;
	track.followowner	= false;
	track.reason = -1;
	track.backup = FALSE;

	track.m_IsRealZ = TRUE ;

	track.speed = (float)speed ;
	getPoint().getPoint (track.x, track.y, track.m_RealZ);
	m_pMobileElemment->getLogEntry().addEvent (track);
	setDestination(getPoint()) ;

	MobElementsDiagnosis()->Diagnose( m_pMobileElemment, time);
	// write log for follower.

	if (m_pMobileElemment->m_pGroupInfo->IsFollower())
		return 1;

	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pMobileElemment->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup())
		return 1;
	Terminal* pterminal = const_cast<Terminal*> (m_pMobileElemment->GetTerminal()) ;
	Person* pLeader = pGroupLeaderInfo->GetGroupLeader();
	AirsideMobElementBaseBehavior* pLeaderBehavior = pLeader->getAirsideBehavior();
	CPoint2008 location = pLeaderBehavior->getPoint() ;
	{
		MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
		int nFollowerCount = elemList.getCount();
		for( int i=0; i< nFollowerCount; i++ )
		{
			Person* _pFollower = (Person*) elemList.getItem( i );
			if(_pFollower == NULL)
				continue; //Logic Error. 

			track.time = (long)time;
			track.state = enumState;
			track.procNumber = -1;
			track.bDynamicCreatedProc = FALSE;
			track.followowner	= false;
			track.reason = -1;
			track.backup = FALSE;

			track.m_IsRealZ = TRUE ;

			location.getPoint (track.x, track.y, track.m_RealZ);
			_pFollower->getLogEntry().addEvent (track);
		}
	}
	return true;
}

void AirsideMobElementBaseBehavior::setState( short newState )
{
	if(m_pMobileElemment)
		m_pMobileElemment->setState(newState);
}

int AirsideMobElementBaseBehavior::getState( void ) const
{
	return m_pMobileElemment->getState();
}

void AirsideMobElementBaseBehavior::GenetateEvent( ElapsedTime eventTime )
{	
	PLACE_METHOD_TRACK_STRING();
	m_pMobileElemment->MobileElement::generateEvent( eventTime, false);
}

int AirsideMobElementBaseBehavior::performanceMove( ElapsedTime p_time,bool bNoLog )
{
	ASSERT(0);
	return 1;
}

Person * AirsideMobElementBaseBehavior::getMobileElement()
{
	return m_pMobileElemment;
}

void AirsideMobElementBaseBehavior::setLocation( const CPoint2008& _ptLocation )
{
	location = _ptLocation;
}


























