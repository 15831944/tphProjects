#include "StdAfx.h"
#include "PaxLandsideBehavior.h"
#include "VisitorLandsideBehavior.h"
#include "MobElementsDiagnosis.h"
#include "pax.h"
#include "GroupLeaderInfo.h"
#include "ARCportEngine.h"
#include "LandsideSimulation.h"

PaxLandsideBehavior::PaxLandsideBehavior( Person* p ) :LandsideBaseBehavior(p)
{
	m_pLandsideSim = p->getEngine()->GetLandsideSimulation();
	//const LandsideSimulation::PaxVehicleMap &paxVehicleMap=m_pLandsideSim->GetPaxVehicleMap();
	//LandsideSimulation::PaxVehicleMap::const_iterator iter=paxVehicleMap.find(m_pPerson->getLogEntry().getID());

	//if(iter!=paxVehicleMap.end())
	//{
	//	//find vehicle 
	//	m_pVehicleType=(CVehicleItemDetail*)iter->second;
	//}
	m_pDestResource = NULL;
}

PaxLandsideBehavior::~PaxLandsideBehavior()
{

}

void PaxLandsideBehavior::InitializeVisitorBehavior()
{
	//visitor
	int nCount =((Passenger*)m_pPerson)->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = ((Passenger*)m_pPerson)->m_pVisitorList[i];
		if (pVis == NULL)
			continue;

		VisitorLandsideBehavior* pBehavior = new VisitorLandsideBehavior(pVis);
		pVis->setBehavior(pBehavior);
		MobElementBehavior* pBeh = pVis->getBehavior(MobElementBehavior::LandsideBehavior);
		if(pBeh)
		{
			((VisitorLandsideBehavior*)(pBeh))->InitializeBehavior();
		}		
		pVis->setState(EntryLandside);
	}

}

void PaxLandsideBehavior::WriteLogEntry(ElapsedTime time, bool _bBackup, bool bOffset /*= true */)
{
	//passenger write log
	ARCVector3 pre_location = location;

	location = m_ptDestination;

	MobEventStruct track;
	track.backup = _bBackup; 

	track.time = (long)time;
	track.state = m_pPerson->getState();

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

	track.x = (float)location.n[VX];
	track.y = (float)location.n[VY];
	track.z = 0;
	track.m_RealZ= (float)location.n[VZ];

	m_pPerson->getLogEntry().addEvent (track);
	//CMobDebugLog::LogEvent(m_pPerson->getID(),track);

	MobElementsDiagnosis()->Diagnose( m_pPerson, time);

	//write follow log
	WriteFollowLog(time, _bBackup, bOffset);

	//write non passenger which owner take
	writeVisitorLog(pre_location,time,_bBackup,bOffset);

}

void PaxLandsideBehavior::WriteFollowLog( ElapsedTime _time, bool _bBackup, bool bOffset /*= true*/ )
{

	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(pGroupLeaderInfo->isInGroup() && !pGroupLeaderInfo->IsFollower())
	{
		// write other member of group
		MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
		int nFollowerCount = elemList.getCount();

		for( int i=0; i< nFollowerCount; i++ )
		{
			Person* _pFollower =(Person*) elemList.getItem( i );;
			PaxLandsideBehavior* spFollowBehavior = (PaxLandsideBehavior*) _pFollower->getLandsideBehavior();
			if(m_pPerson->getState() != Death)
			{
				_pFollower->setState(m_pPerson->getState());
			}

			if( _pFollower->getState() != Death  && spFollowBehavior)
				spFollowBehavior->WriteLogEntry( _time, _bBackup, bOffset);
		}
	}

}

void PaxLandsideBehavior::writeVisitorLog( const ARCVector3& pax_pre_location,ElapsedTime time, bool _bBackup /*= false*/, bool bOffset /*= true */ )
{
	int nCount = ((Passenger*)m_pPerson)->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = ((Passenger*)m_pPerson)->m_pVisitorList[i];
		if (pVis == NULL)
			continue;

		VisitorLandsideBehavior *pVisitorBehavior =(VisitorLandsideBehavior *)pVis->getLandsideBehavior();
		ASSERT(pVisitorBehavior);
		if(pVisitorBehavior)
			pVisitorBehavior->WriteVisitorTrack(pax_pre_location,this ,time,_bBackup,bOffset);
		//AddVisitorTrack();
	}
}

//only used for passenger
void PaxLandsideBehavior::UpdateEntryTime( const ElapsedTime& eEntryTime )
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

			_pFollower->getLogEntry().setEntryTime(eEntryTime);
		}
	}

	ASSERT((Passenger *)m_pPerson != NULL);

	int nCount =((Passenger*)m_pPerson)->m_pVisitorList.size();
	for( int i=nCount-1; i>=0; i-- )
	{
		PaxVisitor* pVis = ((Passenger*)m_pPerson)->m_pVisitorList[i];
		if (pVis == NULL)
			continue;
		VisitorLandsideBehavior *pVisitor = (VisitorLandsideBehavior *)pVis->getLandsideBehavior();
		pVisitor->UpdateEntryTime( eEntryTime );
		
	}

}

void PaxLandsideBehavior::setDestination( const ARCVector3& p,MobDir emWalk /*= FORWARD*/ )
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
			LandsideBaseBehavior* spFollowerBehavior = pVis->getLandsideBehavior();
			if (spFollowerBehavior)
			{
				spFollowerBehavior->setDestination( p );
			}
		}
	}
}

LandsideSimulation * PaxLandsideBehavior::getLandsideSimulation()
{
	return m_pLandsideSim;
}

LandsideResourceInSim * PaxLandsideBehavior::getDestResource() const
{
	return m_pDestResource;
}

void PaxLandsideBehavior::setDestResource( LandsideResourceInSim *pDestResource )
{
	m_pDestResource = pDestResource;
}
