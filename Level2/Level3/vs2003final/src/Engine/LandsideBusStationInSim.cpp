#include "StdAfx.h"
#include ".\landsidebusstationinsim.h"
#include ".\laneseginsim.h"
#include "LandsideResourceManager.h"
#include "Landside/LandsideBusStation.h"
#include "PaxLandsideBehavior.h"
#include "Landside/VehicleItemDetail.h"
#include "LandsideQueueProcess.h"
#include "LandsideParkingLotInSim.h"
#include "Common\States.h"
#include "landsideCellPhoneProcess.h"
#include "LandsideSimulation.h"
#include "LandsideResidentVehicleInSim.h"
#include "PERSON.H"
#include "Landside\LandsidePublicExtBusInSim.h"
#include "Landside\LandsideVehicleAssignment.h"



LandsideBusStationInSim::LandsideBusStationInSim( LandsideBusStation *pStation,bool bLeftDrive )
:LandsideLayoutObjectInSim(pStation)
{
	m_pBusStation=pStation;

	m_pEmbeddedParkingLotInSim = NULL;
	
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

LandsideBusStationInSim::~LandsideBusStationInSim(void)
{
	if (m_queueProcessSys)
	{
		delete m_queueProcessSys;
		m_queueProcessSys = NULL;
	}


}
CString LandsideBusStationInSim::print() const
{
	return m_pBusStation->getName().GetIDString();
}

//void LandsideBusStationInSim::RemoveVehicleParkPos( LandsideVehicleInSim* pVehicle )
//{
//	for(size_t i=0;i<m_vLaneOccupy.size();i++)
//	{
//		LaneSegInSim* pSeg = m_vLaneOccupy[i];
//		pSeg->removeVehicleOcy(pVehicle);
//	}
//}
LandsideLaneNodeInSim* LandsideBusStationInSim::GetExitNode( LandsideLaneInSim* pLane )
{
	for(size_t i=0;i<m_vLaneEntries.size();i++)
	{
		LandsideLaneEntry* pEntry = m_vLaneEntries[i];
		if(pEntry->mpLane == pLane)
			return pEntry;
	}
	return NULL;
}
//LandsideVehicleInSim* LandsideBusStationInSim::GetWantBus(int nDetailID)
//{
//	for (int i=0;i<(int)m_vWaitingBus.size();i++)
//	{
//		/*if (((LandsideVehicleInSim*)m_vWaitingBus.at(i))->GetVehicleItemDetail()->GetID()==nDetailID)
//		{
//			return m_vWaitingBus.at(i);
//		}*/
//	}
//	return NULL;
//}
//void LandsideBusStationInSim::DelWaitingPax(PaxLandsideBehavior* pPax)
//{
//	m_vWaitingPax.Remove(pPax);
//}
void LandsideBusStationInSim::DelWaitingBus(LandsideVehicleInSim* pBus)
{
	m_vWaitingBus.Remove(pBus);
}
void LandsideBusStationInSim::NoticeWaitingPax(const ElapsedTime &time,LandsideVehicleInSim* pBus, LandsideSimulation* pSimulation)
{
	if (m_queueProcessSys)
	{
		//m_queueProcessSys->InvokeWaitQueue(time);
		LandsideQueueWakeUpEvent* pQueueWakeupEvent = new LandsideQueueWakeUpEvent(m_queueProcessSys,time);
		pQueueWakeupEvent->addEvent();

	}
// 	int nCount = static_cast<int>(m_vWaitingPax.size());
// 	for (int nPax = nCount -1; nPax >= 0; nPax--)
// 	{	
// 		PaxLandsideBehavior* pPax = m_vWaitingPax[nPax];
// 		if(PaxTakeOnBus( pPax, pSimulation, time ))
// 		{
// 			pPax->setState(MoveToVehicle);
// 			pPax->GenerateEvent(time);
// 			DelWaitingPax(pPax);
// 		}
// 	}
	
}
//bool LandsideBusStationInSim::HavePaxWaitingForThisBus(LandsideVehicleInSim *pBus)
//{
//	std::vector<Pax*>::iterator iter=m_vWaitingPax.begin();
//	for (;iter!=m_vWaitingPax.end();++iter)
//	{
//		/*if (((PaxLandsideBehavior*)(*iter))->GetVehicleType()->GetID()==pBus->GetVehicleItemDetail()->GetID())
//		{
//			return true;
//		}*/
//	}
//	return false;
//}
void LandsideBusStationInSim::InitRelateWithOtherObject(LandsideResourceManager* allRes)
{
	if(!m_pBusStation->getStrech())
		return;

	LandsideStretchInSim*pStretch = allRes->getStretchByID(m_pBusStation->getStrech()->getID());
	if(pStretch)
	{
		CPoint2008 ptFrom = pStretch->m_Path.GetDistPoint(m_pBusStation->getDistFrom());
		CPoint2008 ptTo = pStretch->m_Path.GetDistPoint(m_pBusStation->getDistTo());

		int iLaneForm = MIN(m_pBusStation->GetLaneTo()-1,m_pBusStation->GetLaneFrom());
		int iLaneTo = MAX(m_pBusStation->GetLaneTo()-1,m_pBusStation->GetLaneFrom());
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

				pLaneExit->SetPosition(pLane,distF);
				pLaneExit->SetToRes(this);
				m_vLaneExits.push_back(pLaneExit);
				pLane->AddLaneNode(pLaneExit);

				LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
				pLaneEntry->SetPosition(pLane,distT);
				pLaneEntry->SetFromRes(this);				
				pLane->AddLaneNode(pLaneEntry);
				m_vLaneEntries.push_back(pLaneEntry);

				mParkingSpots.addLane(pLane,distF,distT,600,this);
				//m_vLaneOccupy.push_back(new LaneSegInSim(pLane, pLaneExit->m_distInlane,pLaneEntry->m_distInlane));
			}
		}
		mParkingSpots.InitSpotRelations();


		//add next to lane
		if(iLaneForm>0)
		{
			LandsideLaneInSim* pLane = pStretch->GetLane(iLaneForm-1);
			DistanceUnit distF = pLane->GetPointDist(ptFrom);
			DistanceUnit distT = pLane->GetPointDist(ptTo);

			{
				LandsideBusStationExit* pStationExit = new LandsideBusStationExit;
				pStationExit->SetFromRes(this);
				pStationExit->SetDistRangeInlane(distF,distT);
				pStationExit->SetPosition(pLane,distF);
				m_vLaneEntries.push_back(pStationExit);
				pLane->AddLaneNode(pStationExit);
			}
			{
				LandsideBusStationEntry* pStationEntry = new LandsideBusStationEntry;
				pStationEntry->SetToRes(this);
				pStationEntry->SetDistRangeInlane(distF,distT);
				pStationEntry->SetPosition(pLane,distF);
				m_vLaneExits.push_back(pStationEntry);
				pLane->AddLaneNode(pStationEntry);
			}
		}
		if(iLaneTo<pStretch->GetLaneCount()-1)
		{
			LandsideLaneInSim* pLane = pStretch->GetLane(iLaneTo+1);
			DistanceUnit distF = pLane->GetPointDist(ptFrom);
			DistanceUnit distT = pLane->GetPointDist(ptTo);

			{
				LandsideBusStationExit* pStationExit = new LandsideBusStationExit;
				pStationExit->SetFromRes(this);
				pStationExit->SetDistRangeInlane(distF,distT);
				pStationExit->SetPosition(pLane,distF);
				m_vLaneEntries.push_back(pStationExit);
				pLane->AddLaneNode(pStationExit);
			}
			{
				LandsideBusStationEntry* pStationEntry = new LandsideBusStationEntry;
				pStationEntry->SetToRes(this);
				pStationEntry->SetDistRangeInlane(distF,distT);
				pStationEntry->SetPosition(pLane,distF);
				m_vLaneExits.push_back(pStationEntry);
				pLane->AddLaneNode(pStationEntry);
			}
		}

	}

	m_pEmbeddedParkingLotInSim = NULL;
	if(m_pBusStation->getEmbedParkingLotID() >= 0)
		m_pEmbeddedParkingLotInSim = allRes->getParkingLotByID(m_pBusStation->getEmbedParkingLotID());
	if(m_pEmbeddedParkingLotInSim)
		m_pEmbeddedParkingLotInSim->AddEmbedBusStation(this);
	
}


LandsideLayoutObjectInSim* LandsideBusStationInSim::getLayoutObject() const
{
	return const_cast<LandsideBusStationInSim*>(this);
}

LandsideBusStation * LandsideBusStationInSim::getBusStation()
{
	return (LandsideBusStation*)getInput();//m_pBusStation;
}

CPath2008 LandsideBusStationInSim::GetQueue()
{
	return m_waitingPath; // m_pBusStation->getWaittingPath();
}

LandsideParkingLotInSim * LandsideBusStationInSim::getEmbeddedParkingLotInSim() const
{
	return m_pEmbeddedParkingLotInSim;
}

void LandsideBusStationInSim::PassengerMoveInto( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime )
{
	AddWaitingPax(pPaxBehvior->GetPersonID());
	pPaxBehvior->setResource(this);
	m_queueProcessSys->EnterQueueProcess(pPaxBehvior,LandsideLeaveQueue,eTime);
}

void LandsideBusStationInSim::PassengerMoveOut( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime )
{
	DelWaitingPax(pPaxBehvior->GetPersonID());	
}

void LandsideBusStationInSim::AddWaitingBus( LandsideVehicleInSim* pBus )
{
	m_vWaitingBus.Add(pBus);
}

//void LandsideBusStationInSim::AddWaitingPax( PaxLandsideBehavior* pPax )
//{
//	m_vWaitingPax.Add(pPax);
//}
//
LaneParkingSpotsGroup& LandsideBusStationInSim::getParkingSpot()
{
	return mParkingSpots;
}

bool LandsideBusStationInSim::PaxTakeOnBus( PaxLandsideBehavior *pBehavior, LandsideSimulation *pSimulation, const ElapsedTime& eTime )
{
	//pax will take the first bus suitable for him
	//the rule is the destination that the bus could reach
	int nCount = static_cast<int>(m_vWaitingBus.size());
	for (int nVehicle = 0; nVehicle < nCount; ++nVehicle )
	{
		LandsideVehicleInSim *pVehicle = m_vWaitingBus[nVehicle];
		if(pVehicle == NULL)
			continue;

		LandsideResidentVehicleInSim  *pResidentVehicle = pVehicle->toResidentVehicle();
		if(pResidentVehicle && pResidentVehicle->IsAvailable())
		{
			////check pax on vehicle whether get off
			//if (!pResidentVehicle->GetPaxGetoffDoneTag())
			//	continue;
			
			//check capacity of vehicle whether exceed
			if (!pResidentVehicle->CapacityAvailable(pBehavior))
				continue;

			if(pBehavior->CanPaxTakeThisBus(pResidentVehicle, pSimulation))
			{
				pVehicle->AddOnPax(pBehavior);
				pBehavior->setVehicle(pVehicle);
				PassengerMoveOut(pBehavior,eTime);
				return true;
			}
		}
		else if(LandsidePublicExtBusInSim *pPublicBus =  pVehicle->toPublicExBus())
		{
			if(pPublicBus->CanLoadPax(pBehavior))
			{
				pVehicle->AddOnPax(pBehavior);
				pBehavior->setVehicle(pPublicBus);
				PassengerMoveOut(pBehavior,eTime);
				return true;

			}			
		}
	}

	return false;
}

CPoint2008 LandsideBusStationInSim::GetPaxWaitPos() const
{
	//return m_pBusStation->GetRandWaitingPoint();
	int nPointCount = m_waitingPath.getCount();///m_pBusStation->getWaittingPath().getCount();
	if(nPointCount > 0)
	{
		return m_waitingPath.getPoint(nPointCount-1);//m_pBusStation->getWaittingPath().getPoint(nPointCount - 1);
	}

	return m_pBusStation->GetRandWaitingPoint();
}

void LandsideBusStationInSim::PaxLeaveQueue( const ElapsedTime& eTime )
{
	if (m_queueProcessSys)
	{
		m_queueProcessSys->LeaveQueue(eTime);
	}
}

bool LandsideBusStationInSim::GetLastServiceTime( ResidentRelatedVehicleTypePlan* pPlan , ElapsedTime& t ) const
{
	std::map<ResidentRelatedVehicleTypePlan*, ElapsedTime>::const_iterator itrFind = m_ServiceTimeMap.find(pPlan);
	if(itrFind !=m_ServiceTimeMap.end())
	{
		t= itrFind->second;
		return true;
	}
	return false;
}

void LandsideBusStationInSim::SetLastServiceTime( ResidentRelatedVehicleTypePlan* pPlan , const ElapsedTime& t )
{
	m_ServiceTimeMap[pPlan] = t;
}































