#include "StdAfx.h"
#include ".\LandsideTaxiQueueInSim.h"
#include ".\laneseginsim.h"
#include "LandsideResourceManager.h"
#include "Landside/LandsideTaxiQueue.h"
#include "PaxLandsideBehavior.h"
#include "LandsideTaxiPoolInSim.h"
#include "LandsideQueueProcess.h"
#include "Common/STATES.H"
#include "Landside/LandsideTaxiInSim.h"
#include "Landside/FacilityTaxiQBehavior.h"
#include "LandsideSimulation.h"
#include "ARCportEngine.h"
#define RADIUS_CONVERN 500

LandsideLayoutObjectInSim* LandsideTaxiQueueInSim::getLayoutObject() const
{
	return const_cast<LandsideTaxiQueueInSim*>(this);
}

LandsideTaxiQueueInSim::LandsideTaxiQueueInSim( LandsideTaxiQueue *pStation,bool bLeftDrive )
:LandsideLayoutObjectInSim(pStation)
{
	m_pAllRes = NULL;
	CPoint2008 center;
	pStation->get3DCenter(center);
	m_waitingPath = pStation->getWaittingPath();
	for(int i =0 ;i< m_waitingPath.getCount();i++)
	{
		m_waitingPath[i].z = center.z; 
	}
	m_queueProcessSys = new LandsideQueueSystemProcess(this);
	getParkingSpot().SetLeftDrive(bLeftDrive);

}

CString LandsideTaxiQueueInSim::print() const
{
	return m_pInput->getName().GetIDString();
}

void LandsideTaxiQueueInSim::InitRelateWithOtherObject( LandsideResourceManager* pRes )
{
	m_pAllRes = pRes;
	LandsideTaxiQueue* pQ= GetTaxiQInput();
	if(!pQ->getStrech())
		return;



	LandsideStretchInSim*pStretch = pRes->getStretchByID(pQ->getStrech()->getID());
	if(pStretch)
	{
		CPoint2008 ptFrom = pStretch->m_Path.GetDistPoint(pQ->getDistFrom());
		CPoint2008 ptTo = pStretch->m_Path.GetDistPoint(pQ->getDistTo());

		int iLaneForm = MIN(pQ->m_nLaneTo-1,pQ->m_nLaneFrom-1);
		int iLaneTo = MAX(pQ->m_nLaneTo-1,pQ->m_nLaneFrom-1);
		iLaneForm = MAX(0,iLaneForm); 
		iLaneForm = MIN(iLaneForm,pStretch->GetLaneCount()-1);
		iLaneTo = MAX(0,iLaneTo); 
		iLaneTo = MIN(iLaneTo,pStretch->GetLaneCount()-1);


		for(int i=iLaneForm;i<=iLaneTo;i++)
		{
			LandsideLaneInSim* pLane = pStretch->GetLane(i);
			if(pLane)
			{
				LandsideLaneExit* pLaneExit = new LandsideLaneExit();
				DistanceUnit distF = pLane->GetPointDist(ptFrom);
				DistanceUnit distT = pLane->GetPointDist(ptTo);

				pLaneExit->SetPosition(pLane,MAX(0,distF-RADIUS_CONVERN) );
				pLaneExit->SetToRes(this);			
				m_vLaneExits.push_back(pLaneExit);
				pLane->AddLaneNode(pLaneExit);

				LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
				pLaneEntry->SetPosition(pLane,distT);
				pLaneEntry->SetFromRes(this);				
				pLane->AddLaneNode(pLaneEntry);
				m_vLaneEntries.push_back(pLaneEntry);

				//m_vLaneOccupy.push_back(new LaneSegInSim(pLane, distF,distT));
				mParkingSpots.addLane(pLane,distF,distT,600,this);
			}
		}
		mParkingSpots.InitSpotRelations();		


		//add next to lane
		if(LandsideLaneInSim* pLane = pStretch->GetLane(iLaneForm-1))
		{
			
			DistanceUnit distF = pLane->GetPointDist(ptFrom);
			DistanceUnit distT = pLane->GetPointDist(ptTo);

			{
				LandsideCurbsideSideExit* pCurbExit = new LandsideCurbsideSideExit;
				pCurbExit->SetFromRes(this);
				pCurbExit->SetDistRangeInlane(distF,distT);
				pCurbExit->SetPosition(pLane,distF-RADIUS_CONVERN);
				m_vLaneEntries.push_back(pCurbExit);
				pLane->AddLaneNode(pCurbExit);
			}
			{
				LandsideCursideSideEntry* pCurbEntry = new LandsideCursideSideEntry;
				pCurbEntry->SetToRes(this);
				pCurbEntry->SetDistRangeInlane(distF,distT);
				pCurbEntry->SetPosition(pLane,distF);
				m_vLaneExits.push_back(pCurbEntry);
				pLane->AddLaneNode(pCurbEntry);
			}
		}
		if(LandsideLaneInSim* pLane = pStretch->GetLane(iLaneTo+1))
		{
			
			DistanceUnit distF = pLane->GetPointDist(ptFrom);
			DistanceUnit distT = pLane->GetPointDist(ptTo);

			{
				LandsideCurbsideSideExit* pCurbExit = new LandsideCurbsideSideExit;
				pCurbExit->SetFromRes(this);
				pCurbExit->SetDistRangeInlane(distF,distT);
				pCurbExit->SetPosition(pLane,distF);
				m_vLaneEntries.push_back(pCurbExit);
				pLane->AddLaneNode(pCurbExit);
			}
			{
				LandsideCursideSideEntry* pCurbEntry = new LandsideCursideSideEntry;
				pCurbEntry->SetToRes(this);
				pCurbEntry->SetDistRangeInlane(distF,distT);
				pCurbEntry->SetPosition(pLane,distF);
				m_vLaneExits.push_back(pCurbEntry);
				pLane->AddLaneNode(pCurbEntry);
			}
		}

	}
}


void LandsideTaxiQueueInSim::addServiceVehicle( LandsideVehicleInSim* pV )
{
	m_vServiceVehicles.Add(pV);
}

void LandsideTaxiQueueInSim::removeServiceVehicle( LandsideVehicleInSim* pV )
{
	m_vServiceVehicles.Remove(pV);
	UpdateOrderingVehicle(pV->curTime());
}

void LandsideTaxiQueueInSim::_UpdateOrderingVehicle(CARCportEngine* pEngine)
{
	if(m_vWaitingPax.empty())
		return;

	CFacilityBehavior* pBevhor  = GetBehavior();
	if(!pBevhor)
		return;

	CFacilityTaxiQBehavior* pTaxiQBehavior = (CFacilityTaxiQBehavior*)pBevhor;
	int nCount = m_vServiceVehicles.GetCount();
	CString vehicleType = pTaxiQBehavior->GetVehicleType();
	
	int nMoreNeed = m_vWaitingPax.size() - m_vServiceVehicles.size();
	if(nMoreNeed<=0)
		return;


	if(!m_pAllRes->m_vTaxiPools.empty())//call from all pools 
	{
		for(size_t i=0;i<m_pAllRes->m_vTaxiPools.size();i++)
		{
			if(nMoreNeed<=0)
			{
				break;
			}
			LandsideTaxiPoolInSim* pPool = m_pAllRes->m_vTaxiPools.at(i);
			int nSendNum = pPool->SentVehicleToQueue(vehicleType,nMoreNeed,this,pEngine);
			nMoreNeed -= nSendNum;
		}
	}
	//call from external entry
	{
		for(int i=0;i<nMoreNeed;i++)
		{
			NonPaxVehicleEntryInfo info;
			info.iDemandType = VehicleDemandAndPoolData::Demand_LayoutObject;
			info.iDemandID = this->getID();
			info.InitName(pTaxiQBehavior->GetVehicleType(),pEngine);

			if( info.InitNoResidentRoute(pEngine) )
			{
				LandsideTaxiInSim *pVehicle = new LandsideTaxiInSim(info);
				if( pVehicle->InitBirth(pEngine) )
				{
					pEngine->GetLandsideSimulation()->AddVehicle(pVehicle);
					pVehicle->StartServiceQueue(this);
				}
				else
				{
					cpputil::autoPtrReset(pVehicle);
				}
			}			
		}
	}
}


void LandsideTaxiQueueInSim::NoticeWaitingPax( const ElapsedTime &time, LandsideVehicleInSim* pTaxi, LandsideSimulation* pSimulation )
{
	if (m_queueProcessSys)
	{
	//	m_queueProcessSys->InvokeWaitQueue(time);
		LandsideQueueWakeUpEvent* pQueueWakeupEvent = new LandsideQueueWakeUpEvent(m_queueProcessSys,time);
		pQueueWakeupEvent->addEvent();
	}
}

LandsideTaxiQueueInSim::~LandsideTaxiQueueInSim()
{
	cpputil::autoPtrReset(m_queueProcessSys);
}
void LandsideTaxiQueueInSim::PassengerMoveInto( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime )
{
	AddWaitingPax(pPaxBehvior->GetPersonID());
	pPaxBehvior->setResource(this);
	m_queueProcessSys->EnterQueueProcess(pPaxBehvior,LandsideLeaveQueue,eTime);
	UpdateOrderingVehicle(eTime);
}

void LandsideTaxiQueueInSim::PassengerMoveOut( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime )
{
	DelWaitingPax(pPaxBehvior->GetPersonID());
}

CPath2008 LandsideTaxiQueueInSim::GetQueue()
{
	return m_waitingPath;
}

bool LandsideTaxiQueueInSim::PaxTakeOnBus( PaxLandsideBehavior *pBehavior, LandsideSimulation *pSimulation, const ElapsedTime& eTime )
{
	//pax will take the first bus suitable for him
	//the rule is the destination that the bus could reach
	int nCount = static_cast<int>(m_waitingtaxi.size());
	for (int nVehicle = 0; nVehicle < nCount; ++nVehicle )
	{
		LandsideVehicleInSim *pVehicle = m_waitingtaxi[nVehicle];
		if(pVehicle == NULL)
			continue;
		LandsideTaxiInSim* pTaxi = pVehicle->toTaxi();
		if(!pTaxi)
			continue;

		if( pTaxi->GetOnVehiclePaxNum() >0)
			continue;;

		pTaxi->AddOnPax(pBehavior);
		pBehavior->setVehicle(pTaxi);
		PassengerMoveOut(pBehavior,eTime);
		return true;		
		
	}

	return false;
}

void LandsideTaxiQueueInSim::PaxLeaveQueue( const ElapsedTime& eTime )
{
	if (m_queueProcessSys)
	{
		m_queueProcessSys->LeaveQueue(eTime);
	}
}

void LandsideTaxiQueueInSim::AddWaitingVehicle( LandsideVehicleInSim* pTaxi )
{
	m_waitingtaxi.Add(pTaxi);
	
}

void LandsideTaxiQueueInSim::DelWaitingVehicle( LandsideVehicleInSim* pTaxi )
{
	m_waitingtaxi.Remove(pTaxi);
}


