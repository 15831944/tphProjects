#include "StdAfx.h"
#include "StandInSim.h"
#include "Clearance.h"
#include "AirsideFlightInSim.h"
#include "AirTrafficController.h"
#include "StandAssignmentRegisterInSim.h"
#include "StandBufferInSim.h"
#include "StandLeadInLineInSim.h"
#include "StandLeadOutLineInSim.h"
#include "FlightPerformanceManager.h"
#include "VehicleServiceDelay.h"
#include "SimulationErrorShow.h"
#include "FlightDeiceStrategyDecision.h"
#include "FlightApplyForTowServiceEvent.h"
#include "FlightServiceInformation.h"
#include "VehicleStretchInSim.h"
#include "./DeiceVehicleServiceRequest.h"
#include "../../InputAirside/StandCriteriaAssignment.h"
#include "../OnboardFlightInSim.h"
#include "FlightPerformancesInSim.h"
#include "EnrouteQueueCapacityInSim.h"

#include <Results/AirsideFlightEventLog.h>

StandInSim::StandInSim( Stand * pStand )
{
	m_pStandInput = pStand;
	m_pLockFlight = NULL;
	m_pStandAssignmentRegister = new CStandAssignmentRegisterInSim(this);
	m_pStandCriteriaAssignment = NULL;

	int inLineCount = (int)GetStandInput()->GetLeadInLineList().GetItemCount();
	for(int i=0;i<inLineCount;i++)
	{
		StandLeadInLineInSim* pNewinLine = new StandLeadInLineInSim(this, i);
		m_vLeadInLines.push_back(pNewinLine);
	}
	int outLineCount = (int)GetStandInput()->GetLeadOutLineList().GetItemCount();
	for(int i=0;i<outLineCount;i++)
	{
		StandLeadOutLineInSim* pNewOutLine = new StandLeadOutLineInSim(this,i);
		m_vLeadOutLines.push_back( pNewOutLine );
	}
	m_bApplyForTowingService = false;
}

CPoint2008 StandInSim::GetDistancePoint( double dist ) const
{
	return m_pStandInput->GetServicePoint();
}


bool StandInSim::FindClearanceInConcern( AirsideFlightInSim * pFlight, ClearanceItem& lastItem ,ElapsedTime tParkingTime, Clearance& newClearance )
{
	
	//ASSERT(pFlight == m_pLockFlight);
	if(lastItem.GetMode() == OnBirth)
	{
		AirsideFlightStandOperationLog* pEnterLog = new AirsideFlightStandOperationLog(lastItem.GetTime().getPrecisely(), pFlight, GetID(), GetStandInput()->GetObjectName(),
			AirsideFlightStandOperationLog::EnteringStand);
		pEnterLog->m_eParkingOpType = DEP_PARKING;
		pFlight->LogEventItem(pEnterLog);		

		StandLeadInLineInSim* pLeadInLine = AssignLeadInLine(pFlight);
		if( pLeadInLine ) //held at the lead in line
		{			
			ClearanceItem atStandItem(pLeadInLine, OnHeldAtStand, pLeadInLine->GetEndDist()- pFlight->GetLength()*0.5);
			atStandItem.SetSpeed(0);
			atStandItem.SetTime(lastItem.GetTime());
			lastItem = atStandItem;
			newClearance.AddItem(lastItem);	
			lastItem.SetDistInResource( pLeadInLine->GetEndDist()- pFlight->GetLength()*0.5 + 1);
			//lastItem.SetTime(tParkingTime);
			newClearance.AddItem(lastItem);
		}
		else
		{
			ClearanceItem atStandItem(this, OnHeldAtStand, 0);
			atStandItem.SetSpeed(0);
			atStandItem.SetTime(lastItem.GetTime());
			lastItem = atStandItem;
			newClearance.AddItem(lastItem);
		}

		CFlightServiceInformation * pInfo =	pFlight->GetFlightServiceInformation();
		if (pInfo)
		{
			pInfo->GetFlightServiceRoute()->SetFlightServiceStand(this);
		}

		return true;
	}

	
	if(lastItem.GetMode() == OnTaxiToStand ) // get enter stand clearance
	{
		GetEnterStandClearance(pFlight,lastItem,tParkingTime,newClearance);

		if (pFlight->GetMode() != OnTerminate)
		{
			if(lastItem.GetTime() < tParkingTime)
			{	
				lastItem.SetTime(tParkingTime);
				newClearance.AddItem(lastItem);

				double dDist = lastItem.GetDistInResource()+1;
				lastItem.SetDistInResource(dDist);
				lastItem.SetMode(OnHeldAtStand);
				newClearance.AddItem(lastItem);
			}
		}
		

		return true;
	}
	
	if(lastItem.GetMode() == OnHeldAtStand )
	{
		if (tParkingTime > lastItem.GetTime())
		{
			if (!m_bApplyForTowingService)
			{
				FlightApplyForTowServiceEvent* pEvent = new FlightApplyForTowServiceEvent(pFlight);
				pEvent->setTime(tParkingTime - 1L);
				pEvent->addEvent();

				m_bApplyForTowingService = true;
			}

			lastItem.SetTime(tParkingTime);
			lastItem.SetMode(OnHeldAtStand);
			newClearance.AddItem(lastItem);	

			double dDist = lastItem.GetDistInResource()+1;
			lastItem.SetDistInResource(dDist);
			newClearance.AddItem(lastItem);
			return true;
		}

		if( !pFlight->IsGeneralVehicleAndPaxbusServiceCompleted() )
		{
  			if (pFlight->IsDeparture()&&pFlight->GetFlightInput()->RetrieveAirsideDepartureTime()&&lastItem.GetTime() > pFlight->GetFlightInput()->GetRealDepTime())
  			{
  				pFlight->WakeupAllPaxBusCompleteService();
  			}
  		//	else
			{
				VehicleServiceDelay* pDelay = new VehicleServiceDelay;
				pDelay->SetFlightID(pFlight->GetUID());
				pDelay->SetDelayResult(FltDelayReason_Service);
				pDelay->SetStartTime(pFlight->GetTime());
				CString strReason;
				strReason.Format("Vehicle Service delayed");
				pDelay->SetReasonDescription(strReason);
				pFlight->SetDelayed(pDelay);

				AirsideFlightStandOperationLog* pDelayLog = new AirsideFlightStandOperationLog(
					pFlight->GetTime().getPrecisely(), pFlight, GetID(), GetStandInput()->GetObjectName(),
					AirsideFlightStandOperationLog::StandDelay);
				pDelayLog->m_eDelayReason = AirsideFlightStandOperationLog::GSEDelay;
				pFlight->LogEventItem(pDelayLog);

				pFlight->StartDelay(lastItem, NULL, FlightConflict::STOPPED, FlightConflict::VEHICLE,FltDelayReason_Service,_T("Vehicle Service delayed"));
			}
			return true;
		}
		pFlight->SetDelayed(NULL);

		if (!pFlight->IsDeparture() && !pFlight->NeedMoveToIntStand())	//only arrival flight
			return false;

		//check passenger whether take on flight
		if(pFlight->IsDeparture())
		{
			if (pFlight->GetPaxCount() != 0)
			{
				pFlight->SetWaitPassengerTag(true);
				return true;
			}
		}

		FlightDeiceStrategyDecision& deiceStragy = pFlight->GetDeiceDecision();
		if ( IsStandResource(deiceStragy.m_deicePlace)  && !pFlight->IsAllVehicleServiceExceptTowingCompleted())
		{
			VehicleServiceDelay* pDelay = new VehicleServiceDelay;
			pDelay->SetFlightID(pFlight->GetUID());
			pDelay->SetDelayResult(FltDelayReason_Service);
			pDelay->SetStartTime(pFlight->GetTime());
			CString strReason;
			strReason.Format("Vehicle Service delayed");
			pDelay->SetReasonDescription(strReason);
			pFlight->SetDelayed(pDelay);

			pFlight->StartDelay(lastItem, NULL, FlightConflict::STOPPED, FlightConflict::VEHICLE,FltDelayReason_Service,_T("Vehicle Service delayed"));

			AirsideFlightStandOperationLog* pDelayLog = new AirsideFlightStandOperationLog(lastItem.GetTime().getPrecisely(), pFlight, GetID(), GetStandInput()->GetObjectName(),
				AirsideFlightStandOperationLog::StandDelay);
			pDelayLog->m_eDelayReason = AirsideFlightStandOperationLog::GSEDelay;
			pFlight->LogEventItem(pDelayLog);


			DeiceVehicleServiceRequest* pRequest = pFlight->getDeiceServiceRequest();
			if( pRequest && !pRequest->IsCompleteService() && pFlight->IsGeneralVehicleAndPaxbusServiceCompleted())
			{
				pFlight->SetReadyForDeice(true);
				pRequest->flightCallDeice(lastItem.GetTime());
				pRequest->flightCallAntiIce(lastItem.GetTime());
				return true;
			}

			return true;
		}
		pFlight->SetDelayed(NULL);

		bool bDelayPushback = pFlight->GetAirTrafficController()->IsDelayPushback(pFlight,lastItem);
		if(bDelayPushback)
		{
			AirsideFlightStandOperationLog* pDelayLog = new AirsideFlightStandOperationLog(lastItem.GetTime().getPrecisely(), pFlight, GetID(), GetStandInput()->GetObjectName(),
				AirsideFlightStandOperationLog::StandDelay);
			pDelayLog->m_eDelayReason = AirsideFlightStandOperationLog::PushbackClearance;
			pFlight->LogEventItem(pDelayLog);

			return true;
		}

		pFlight->EndDelay(lastItem);		//end leaving stand delay

		IntersectionNodeInSim* pOutNode = GetMaxDistOutNode();

		if (!pOutNode)
		{
			CString strWarn; 
			strWarn.Format( "%s: No push back route to taxiway, the parking flight will be terminate!",m_pStandInput->GetObjectName().GetIDString());
			CString strError = _T("STAND ERROR");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);

			pFlight->PerformClearance(newClearance);
			return true;
		}

		if (pFlight->IsWaitingForTowTruck())
		{
			AirsideFlightStandOperationLog* pDelayLog = new AirsideFlightStandOperationLog(lastItem.GetTime().getPrecisely(), pFlight, GetID(), GetStandInput()->GetObjectName(),
				AirsideFlightStandOperationLog::StandDelay);
			pDelayLog->m_eDelayReason = AirsideFlightStandOperationLog::GSEDelay;
			pFlight->LogEventItem(pDelayLog);

			return true;
		}

		if (pFlight->GetTowingServiceVehicle() && !pFlight->IsTowingServiceCompleted())
		{
			pFlight->GetTowingServiceVehicle()->ServicingFlight();
			return true;
		}
	
		OnboardFlightInSim* pOnboardFlightInSim = pFlight->GetOnboardFlight();
		if (pOnboardFlightInSim && !pOnboardFlightInSim->ReadyCloseDoor())
		{
			pOnboardFlightInSim->SetWaitAirsideFlight(pFlight);
			return true;
		}

		return GetExitStandClearance(pFlight,lastItem,newClearance);		
	}
	return true;
		
}

bool StandInSim::GetEnterStandClearance(AirsideFlightInSim * pFlight, ClearanceItem& lastItem,ElapsedTime tParkingTime, Clearance& newClearance)
{
	StandLeadInLineInSim* pLeadInLine = AssignLeadInLine(pFlight);
	ASSERT(pLeadInLine != NULL);		//there must have lead in line of stand

	ElapsedTime tEnter = lastItem.GetTime();
	if( pLeadInLine ) 
	{
		if(pFlight->GetLength()> pLeadInLine->GetEndDist())
		{
			char szBuff[1024];
			pFlight->GetFlightInput()->getACType(szBuff);
			CString strWarn = _T("");
			strWarn.Format(_T("%s: Not enough distance of lead in line for ACType:%s"),m_pStandInput->GetObjectName().GetIDString(),szBuff);
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);
			pFlight->PerformClearance(newClearance);
			return true;
		}
		else
		{
		/*	double taxispd =pFlight->GetPerformance()->getTaxiInNormalSpeed(pFlight);
			ClearanceItem midItem(pLeadInLine, OnEnterStand, pLeadInLine->GetEndDist() * 0.5);
			midItem.SetSpeed(taxispd*0.5);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,midItem);
			midItem.SetTime(lastItem.GetTime() + dT);
			lastItem = midItem;
			newClearance.AddItem(lastItem);*/

			ClearanceItem enterStand(pLeadInLine, OnEnterStand, pLeadInLine->GetEndDist() - pFlight->GetLength()*0.5);
			enterStand.SetSpeed(0);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,enterStand);
			enterStand.SetTime(lastItem.GetTime()  +dT);
			lastItem = enterStand;
			newClearance.AddItem(lastItem);	

			lastItem.SetDistInResource( lastItem.GetDistInResource() +1);
			lastItem.SetMode(OnHeldAtStand);
			newClearance.AddItem(lastItem);	
		}

		CFlightServiceInformation * pInfo =	pFlight->GetFlightServiceInformation();
		if (pInfo)
		{
			pInfo->GetFlightServiceRoute()->SetFlightServiceStand(this);
		}
	}
	else
	{
		ClearanceItem atStandItem(this, OnHeldAtStand, 0 );
		atStandItem.SetSpeed(0);
		ElapsedTime dT = pFlight->GetTimeBetween(lastItem,atStandItem);
		atStandItem.SetTime(lastItem.GetTime()  +dT);
		lastItem = atStandItem;
		newClearance.AddItem(lastItem);		

	}

	AirsideFlightStandOperationLog* pEnterLog = new AirsideFlightStandOperationLog(tEnter.getPrecisely(), pFlight, GetID(), GetStandInput()->GetObjectName(),
		AirsideFlightStandOperationLog::EnteringStand);

	if (pFlight->IsArrivingOperation())
	{
		pEnterLog->m_eParkingOpType = ARR_PARKING;
	}
	else
	{
		StandInSim* pIntermediateStand = pFlight->GetIntermediateParking();
		if (pIntermediateStand == this)
			pEnterLog->m_eParkingOpType = INT_PARKING;
		else
		{
			if (pIntermediateStand)
			{
				pIntermediateStand->ReleaseLock(pFlight,pFlight->GetTime());
				int nIdx = pIntermediateStand->GetStandAssignmentRegister()->FindRecord(pFlight, INT_PARKING);
				if (nIdx >= 0)
					pIntermediateStand->GetStandAssignmentRegister()->DeleteRecord(nIdx);	
				
			}
			pEnterLog->m_eParkingOpType = DEP_PARKING;
		}
	}
	pFlight->LogEventItem(pEnterLog);


	if (!m_bApplyForTowingService && pFlight->GetTowOffStandType() != NOTTOW)
	{
		FlightApplyForTowServiceEvent* pEvent = new FlightApplyForTowServiceEvent(pFlight);
		pEvent->setTime(tParkingTime - 1L);
		pEvent->addEvent();

		m_bApplyForTowingService = true;
	}

	//if(lastItem.GetTime() < tParkingTime)
	//{	
	//	lastItem.SetTime(tParkingTime);
	//	lastItem.SetDistInResource( lastItem.GetDistInResource() +1);
	//	lastItem.SetMode(OnHeldAtStand);
	//	newClearance.AddItem(lastItem);					
	//}


	return true;
}

bool StandInSim::GetExitStandClearance(AirsideFlightInSim * pFlight, ClearanceItem& lastItem, Clearance& newClearance)
{
	EnrouteQueueCapacityInSim* pEnrouteCapacity = pFlight->GetAirTrafficController()->GetEnrouteCapacity();
	if (pEnrouteCapacity && pEnrouteCapacity->PushBackExitEnrouteQCapacity(lastItem.GetTime(),pFlight) == false)//delay by enroute capacity
		return true;
	
	
	StandLeadOutLineInSim* pOutLine = AssignLeadOutLine(pFlight);
	if(pOutLine)
	{
		if (pOutLine->IsPushBack())
		{
			ClearanceItem exitItem(pOutLine, OnExitStand, pFlight->GetLength()*0.5);
			exitItem.SetSpeed(0);
			exitItem.SetTime( lastItem.GetTime() );
			exitItem.SetPushback(true);
			lastItem = exitItem;
			newClearance.AddItem(lastItem);

			if (pOutLine->IsReleasePoint())
			{
				if (pOutLine->GetReleaseDistance() > pFlight->GetLength()*0.5)
				{
					double pushbackspd =pFlight->GetPerformance()->getPushBackSpeed(pFlight);
					ClearanceItem releaseItem(pOutLine,OnExitStand,pOutLine->GetReleaseDistance());
					releaseItem.SetSpeed(pushbackspd);
					ElapsedTime dT = pFlight->GetTimeBetween(lastItem,releaseItem);
					releaseItem.SetTime(lastItem.GetTime() + dT);
					releaseItem.SetPushback(true);
					lastItem = releaseItem;
					newClearance.AddItem(lastItem);
				}
				ElapsedTime unhookTime = pFlight->GetPerformance()->getUnhookandTaxiTime(pFlight);
				lastItem.SetDistInResource(lastItem.GetDistInResource() + 1);
				lastItem.SetTime(lastItem.GetTime() + unhookTime);
				lastItem.SetSpeed(0);
				newClearance.AddItem(lastItem);
			}
			else
			{
				lastItem.SetDistInResource(lastItem.GetDistInResource() + 1);
				lastItem.SetTime(lastItem.GetTime());
				lastItem.SetSpeed(0);
				newClearance.AddItem(lastItem);
			}

			return false;
		}

		StandLeadInLineInSim* pInLine = AssignLeadInLine(pFlight);
		if (pInLine)
		{
			ClearanceItem inItem(pInLine,OnEnterStand,lastItem.GetDistInResource());
			inItem.SetSpeed(0);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,inItem);
			inItem.SetTime(lastItem.GetTime() + dT);
			lastItem = inItem;
			newClearance.AddItem(lastItem);

			double pushbackspd =pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight);
			ClearanceItem midItem(pOutLine,OnExitStand,pFlight->GetLength()*0.5);
			midItem.SetSpeed(pushbackspd);
			dT = pFlight->GetTimeBetween(lastItem,midItem);
			midItem.SetTime(lastItem.GetTime() + dT);
			lastItem = midItem;
			newClearance.AddItem(lastItem);
			
		/*	ClearanceItem exitItem(pOutLine,OnExitStand,pOutLine->GetEndDist() - 1000);
			exitItem.SetSpeed(lastItem.GetSpeed());
			dT = pFlight->GetTimeBetween(lastItem,exitItem);
			exitItem.SetTime(lastItem.GetTime() + dT);
			lastItem = exitItem;
			newClearance.AddItem(lastItem);*/
		}
		else
		{
			ClearanceItem exitItem(pOutLine, OnExitStand,pFlight->GetLength()*0.5);
			exitItem.SetSpeed(0);
			ElapsedTime Dt = pFlight->GetTimeBetween(lastItem,exitItem);
			exitItem.SetTime( lastItem.GetTime()+Dt );
			lastItem = exitItem;
			newClearance.AddItem(lastItem);	
			lastItem.SetDistInResource(1);		
			newClearance.AddItem(lastItem);
		}
		
	}
	else
	{		
		ClearanceItem exitItem(this, OnExitStand, 1);
		exitItem.SetSpeed(0);
		exitItem.SetTime( lastItem.GetTime() );
		lastItem = exitItem;
		newClearance.AddItem(lastItem);
	}

	return false;
}

void StandInSim::ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime)
{
	if(GetLockedFlight() == pFlight){
		m_pLockFlight = NULL;
		m_bApplyForTowingService = false;
	}
	m_releaseTime = tTime;
	NotifyObservers(tTime);
}

bool StandInSim::GetLock( AirsideFlightInSim * pFlight )
{
	if(TryLock(pFlight)){
		m_pLockFlight = pFlight;
		m_bApplyForTowingService = false;
		return true;
	}
	return false;
}

AirsideFlightInSim * StandInSim::GetLockedFlight()
{
	return m_pLockFlight;
}

bool StandInSim::TryLock( AirsideFlightInSim * pFlight )
{
	if(m_pLockFlight && m_pLockFlight!= pFlight) return false;
	return true;
}

CString StandInSim::PrintResource() const
{
	return m_pStandInput->GetObjNameString();
}

void StandInSim::Init( IntersectionNodeInSimList& nodeList )
{
	//Clear();
	
	for(int i=0;i<GetLeadInLineCount();i++)
	{
		StandLeadInLineInSim* pNewinLine = GetLeadInLineByIndex(i);
		pNewinLine->InitRelate(nodeList);
		
	}	
	for(int i=0;i<GetLeadOutLineCount();i++)
	{
		StandLeadOutLineInSim* pNewOutLine = GetLeadOutLineByIndex(i);
		pNewOutLine->InitRelate(nodeList);		
	}

	if (0 == GetLeadInLineCount() || 0 == GetLeadOutLineCount())
	{
		AirsideSimErrorShown::SimpleSimWarning(
			m_pStandInput->GetObjectName().GetIDString(),
			_T("The stand has no lead-in line or lead-out line. Please adapt the layout."),
			_T("Stand Floating")
			);
	}

}

void StandInSim::Clear()
{
	for(int i=0;i<(int)m_vLeadInLines.size();i++)
	{
		delete m_vLeadInLines[i];
	}
	m_vLeadInLines.clear();
	for(int i=0;i<(int)m_vLeadOutLines.size();i++)
	{
		delete m_vLeadOutLines[i];
	}
	m_vLeadOutLines.clear();
}

IntersectionNodeInSim* StandInSim::GetMinDistInNode()const
{
	//for(int i=0;i< GetLeadInLineCount();i++)
	//{
	//	StandLeadInLineInSim* leadInLine = GetLeadInLineByIndex(i);
	//	for(int j=0;j < leadInLine->GetIntersectNodeCount();j++)
	//		return leadInLine->GetIntersectNode(j);
	//}
	//return NULL;
	StandLeadInLineInSim* pLeadIn = GetLeadInLineByMinDist();
	if (pLeadIn)
		return pLeadIn->GetEndNode();

	return NULL;
}

IntersectionNodeInSim* StandInSim::GetMaxDistOutNode()const
{
	//for(int i=0;i< GetLeadOutLineCount();i++)
	//{
	//	StandLeadOutLineInSim* leadOutLine = GetLeadOutLineByIndex(i);
	//	for(int j=0;j < leadOutLine->GetIntersectNodeCount();j++)
	//		return leadOutLine->GetIntersectNode(j);
	//}

	//return NULL;
	StandLeadOutLineInSim* pLeadOut = GetLeadOutLineByMaxDist();
	if (pLeadOut)
		return pLeadOut->GetEndNode();

	return NULL;
}

StandLeadInLineInSim* StandInSim::AssignLeadInLine( AirsideFlightInSim* pFlight )
{
	if (m_pStandCriteriaAssignment)
	{
		for (int i =0; i < m_pStandCriteriaAssignment->getCount(); i++)
		{
			FlightTypeStandAssignment* pFlightType = NULL;
			pFlightType = m_pStandCriteriaAssignment->getItem(i);
			if(pFlight->fits(pFlightType->GetFlightConstraint()))
			{
				for (int j =0; j < GetLeadInLineCount(); j++)
				{
					StandLeadInLineInSim* pLeadInInsim = GetLeadInLineByIndex(j);
					if (pFlightType->getLeadInLineID() == pLeadInInsim->GetID())
					{
						return GetLeadInLineByIndex(j);
					}
				}
			}
		}
	}

	if (GetLeadInLineCount() > 0)
	{
		//return GetLeadInLineByIndex(0);
		return GetLeadInLineByMinDist();
	}
	return NULL;
}

StandLeadOutLineInSim* StandInSim::AssignLeadOutLine( AirsideFlightInSim* pFlight )
{
	if (m_pStandCriteriaAssignment)
	{
		FlightTypeStandAssignment* pFlightType = NULL;
		pFlightType = getFlightTypeStandAssignment(pFlight);
		if (pFlightType)
		{
			//if(pFlight->fits(pFlightType->GetFlightConstraint()))
			{
				for (int j =0; j < GetLeadOutLineCount(); j++)
				{
					StandLeadOutLineInSim* pLeadOutInsim = GetLeadOutLineByIndex(j);
					if (pFlightType->getLeadOutLineID() == pLeadOutInsim->GetID())
					{
						return GetLeadOutLineByIndex(j);
					}
				}
			}
		}
	}	
	
	if (GetLeadOutLineCount() > 0)
	{
		return GetLeadOutLineByMaxDist();
	}

   return NULL;
}

bool StandInSim::IsStandResource(AirsideResource* pResource)
{
	if (pResource == this)
		return true;
	if ( pResource && pResource->GetType() ==  AirsideResource::ResType_StandLeadInLine )
	{
		int nCount = GetLeadInLineCount();
		for (int i = 0; i < nCount; i++)
		{
			if (GetLeadInLineByIndex(i) == pResource)
				return true;
		}
	}
	if (pResource && pResource->GetType() == AirsideResource::ResType_StandLeadOutLine )
	{
		int nCount = GetLeadOutLineCount();
		for (int i =0; i < nCount; i++)
		{
			if (GetLeadOutLineByIndex(i) == pResource)
				return true;
		}
	}
	return false;
}

StandInSim* StandInSim::IsStandResource() const
{
	return const_cast<StandInSim*>(this);
}

FlightTypeStandAssignment* StandInSim::getFlightTypeStandAssignment(AirsideFlightInSim* pFlight)
{
	if (m_pStandCriteriaAssignment == NULL)
		return NULL;
	
	for (int i =0; i < m_pStandCriteriaAssignment->getCount(); i++)
	{
		FlightTypeStandAssignment* pFlightType = NULL;
		pFlightType = m_pStandCriteriaAssignment->getItem(i);
		if(pFlight->fits(pFlightType->GetFlightConstraint()))
		{
			return pFlightType;
		}
	}
	return NULL;
}

bool StandInSim::IsPushbackStandForFlight(AirsideFlightInSim* pFlight)
{
	StandLeadOutLineInSim* pOutLine = AssignLeadOutLine(pFlight);
	if(pOutLine && pOutLine->IsPushBack())
		return true;

	return false;
}

#include "..\..\Results\AirsideFlightEventLog.h"
void StandInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
	resDesc.resid = m_pStandInput->getID();
}

bool StandInSim::IsActiveFor( ARCMobileElement* pmob,const RouteDirPath* pPath ) const
{
	if(m_pLockFlight!=pmob)
		return false;
	return true;
}

bool StandInSim::bMayHaveConflict( const RouteDirPath *pPath ) const
{
	return true;
}

bool StandInSim::bLocked( ARCMobileElement* pmob,const RouteDirPath* pPath ) const
{
	if( m_pLockFlight && m_pLockFlight!=pmob)
		return true;
	return false;
}

bool StandInSim::bMobileInResource( ARCMobileElement* pmob ) const
{
	if( m_pLockFlight == pmob)
		return true;
	return false;
}

void StandInSim::AddOutLane(VehicleLaneEntry* laneEntry )
{
	if (std::find(m_vOutLanes.begin(),m_vOutLanes.end(),laneEntry) != m_vOutLanes.end())
		return;

	m_vOutLanes.push_back(laneEntry);
}

StandLeadInLineInSim* StandInSim::GetLeadInLineByMinDist() const
{
	if (m_vLeadInLines.empty())
		return NULL;

	StandLeadInLineInSim* pLeadIn = m_vLeadInLines.at(0);
	double dMinDist = pLeadIn->GetEndDist();
	for (size_t i =1; i < m_vLeadInLines.size(); i++)
	{
		StandLeadInLineInSim* pIn = m_vLeadInLines.at(i);
		if (dMinDist > pIn->GetEndDist())
		{
			pLeadIn = pIn;
			dMinDist = pIn->GetEndDist();
		}
	}

	return pLeadIn;
}

StandLeadOutLineInSim* StandInSim::GetLeadOutLineByMaxDist() const
{
	if (m_vLeadOutLines.empty())
		return NULL;

	StandLeadOutLineInSim* pLeadOut = m_vLeadOutLines.at(0);
	double dMaxDist = pLeadOut->GetEndDist();
	for (size_t i =1; i < m_vLeadOutLines.size(); i++)
	{
		StandLeadOutLineInSim* pOut = m_vLeadOutLines.at(i);
		if (dMaxDist < pOut->GetEndDist())
		{
			pLeadOut = pOut;
			dMaxDist = pOut->GetEndDist();
		}
	}

	return pLeadOut;
}

StandLeadInLineInSim* StandInSim::GetLeadInLineByIndex( int idx ) const
{
	if (idx <0 || idx >= (int)m_vLeadInLines.size())
		return NULL;

	return m_vLeadInLines.at(idx);
}

StandLeadOutLineInSim* StandInSim::GetLeadOutLineByIndex( int idx ) const
{
	if (idx <0 || idx >= (int)m_vLeadOutLines.size())
		return NULL;

	return m_vLeadOutLines.at(idx);
}

bool StandInSim::GetPullFromStandClearance( AirsideFlightInSim * pFlight, ClearanceItem& lastItem, Clearance& newClearance )
{
	StandLeadOutLineInSim* pOutLine = AssignLeadOutLine(pFlight);
	if(pOutLine)
	{
		if (pOutLine->IsPushBack())
		{
			ClearanceItem exitItem(pOutLine, OnExitStand, pFlight->GetLength()*0.5);
			exitItem.SetSpeed(0);
			exitItem.SetTime( lastItem.GetTime() );
			exitItem.SetPushback(false);
			lastItem = exitItem;
			newClearance.AddItem(lastItem);

			if (pOutLine->IsReleasePoint())
			{
				if (pOutLine->GetReleaseDistance() > pFlight->GetLength()*0.5)
				{
					double pushbackspd =pFlight->GetPerformance()->getPushBackSpeed(pFlight);
					ClearanceItem releaseItem(pOutLine,OnExitStand,pOutLine->GetReleaseDistance());
					releaseItem.SetSpeed(pushbackspd);
					ElapsedTime dT = pFlight->GetTimeBetween(lastItem,releaseItem);
					releaseItem.SetTime(lastItem.GetTime() + dT);
					releaseItem.SetPushback(false);
					lastItem = releaseItem;
					newClearance.AddItem(lastItem);
				}
				ElapsedTime unhookTime = pFlight->GetPerformance()->getUnhookandTaxiTime(pFlight);
				lastItem.SetDistInResource(lastItem.GetDistInResource() + 1);
				lastItem.SetTime(lastItem.GetTime() + unhookTime);
				lastItem.SetSpeed(0);
				newClearance.AddItem(lastItem);
			}
			else
			{
				lastItem.SetDistInResource(lastItem.GetDistInResource() + 1);
				lastItem.SetTime(lastItem.GetTime());
				lastItem.SetSpeed(0);
				newClearance.AddItem(lastItem);
			}

			return false;
		}

		StandLeadInLineInSim* pInLine = AssignLeadInLine(pFlight);
		if (pInLine)
		{
			ClearanceItem inItem(pInLine,OnEnterStand,lastItem.GetDistInResource());
			inItem.SetSpeed(0);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,inItem);
			inItem.SetTime(lastItem.GetTime() + dT);
			lastItem = inItem;
			newClearance.AddItem(lastItem);

			double pushbackspd =pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight);
			ClearanceItem midItem(pOutLine,OnExitStand,pFlight->GetLength()*0.5);
			midItem.SetSpeed(pushbackspd);
			dT = pFlight->GetTimeBetween(lastItem,midItem);
			midItem.SetTime(lastItem.GetTime() + dT);
			lastItem = midItem;
			newClearance.AddItem(lastItem);

		/*	ClearanceItem exitItem(pOutLine,OnExitStand,pOutLine->GetEndDist() - 1000);
			exitItem.SetSpeed(lastItem.GetSpeed());
			dT = pFlight->GetTimeBetween(lastItem,exitItem);
			exitItem.SetTime(lastItem.GetTime() + dT);
			lastItem = exitItem;
			newClearance.AddItem(lastItem);*/
		}
		else
		{
			ClearanceItem exitItem(pOutLine, OnExitStand,pFlight->GetLength()*0.5);
			exitItem.SetSpeed(0);
			ElapsedTime Dt = pFlight->GetTimeBetween(lastItem,exitItem);
			exitItem.SetTime( lastItem.GetTime()+Dt );
			lastItem = exitItem;
			newClearance.AddItem(lastItem);	
			lastItem.SetDistInResource(1);		
			newClearance.AddItem(lastItem);
		}

	}
	else
	{		
		ClearanceItem exitItem(this, OnExitStand, 1);
		exitItem.SetSpeed(0);
		exitItem.SetTime( lastItem.GetTime() );
		lastItem = exitItem;
		newClearance.AddItem(lastItem);
	}

	return false;
}

bool StandInSim::GetPushFromStandClearance( AirsideFlightInSim * pFlight, ClearanceItem& lastItem, Clearance& newClearance )
{
	StandLeadOutLineInSim* pOutLine = AssignLeadOutLine(pFlight);
	if(pOutLine)
	{
		if (pOutLine->IsPushBack())
		{
			ClearanceItem exitItem(pOutLine, OnExitStand, pFlight->GetLength()*0.5);
			exitItem.SetSpeed(0);
			exitItem.SetTime( lastItem.GetTime() );
			exitItem.SetPushback(true);
			lastItem = exitItem;
			newClearance.AddItem(lastItem);

			if (pOutLine->IsReleasePoint())
			{
				if (pOutLine->GetReleaseDistance() > pFlight->GetLength()*0.5)
				{
					double pushbackspd =pFlight->GetPerformance()->getPushBackSpeed(pFlight);
					ClearanceItem releaseItem(pOutLine,OnExitStand,pOutLine->GetReleaseDistance());
					releaseItem.SetSpeed(pushbackspd);
					ElapsedTime dT = pFlight->GetTimeBetween(lastItem,releaseItem);
					releaseItem.SetTime(lastItem.GetTime() + dT);
					releaseItem.SetPushback(true);
					lastItem = releaseItem;
					newClearance.AddItem(lastItem);
				}
				ElapsedTime unhookTime = pFlight->GetPerformance()->getUnhookandTaxiTime(pFlight);
				lastItem.SetDistInResource(lastItem.GetDistInResource() + 1);
				lastItem.SetTime(lastItem.GetTime() + unhookTime);
				lastItem.SetSpeed(0);
				newClearance.AddItem(lastItem);
			}
			else
			{
				lastItem.SetDistInResource(lastItem.GetDistInResource() + 1);
				lastItem.SetTime(lastItem.GetTime());
				lastItem.SetSpeed(0);
				newClearance.AddItem(lastItem);
			}

			return false;
		}

		StandLeadInLineInSim* pInLine = AssignLeadInLine(pFlight);
		if (pInLine)
		{
			double pushbackspd =pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight);
			ClearanceItem inItem(pInLine,OnExitStand,pInLine->GetEndDist());
			inItem.SetSpeed(pushbackspd);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,inItem);
			inItem.SetTime(lastItem.GetTime() + dT);
			lastItem = inItem;
			newClearance.AddItem(lastItem);


			ClearanceItem midItem(pOutLine,OnExitStand,0);
			midItem.SetSpeed(pushbackspd);
			dT = pFlight->GetTimeBetween(lastItem,midItem);
			midItem.SetTime(lastItem.GetTime() + dT);
			midItem.SetPushback(true);
			lastItem = midItem;
			newClearance.AddItem(lastItem);

			/*ClearanceItem exitItem(pOutLine,OnExitStand,pOutLine->GetEndDist() - 1000);
			exitItem.SetSpeed(lastItem.GetSpeed());
			dT = pFlight->GetTimeBetween(lastItem,exitItem);
			exitItem.SetTime(lastItem.GetTime() + dT);
			exitItem.SetPushback(true);
			lastItem = exitItem;
			newClearance.AddItem(lastItem);*/
		}
		else
		{
			ClearanceItem exitItem(pOutLine, OnExitStand,pFlight->GetLength()*0.5);
			exitItem.SetSpeed(0);
			ElapsedTime Dt = pFlight->GetTimeBetween(lastItem,exitItem);
			exitItem.SetTime( lastItem.GetTime()+Dt );
			exitItem.SetPushback(true);
			lastItem = exitItem;
			newClearance.AddItem(lastItem);	
			lastItem.SetDistInResource(1);		
			newClearance.AddItem(lastItem);
		}

	}
	else
	{		
		ClearanceItem exitItem(this, OnExitStand, 1);
		exitItem.SetSpeed(0);
		exitItem.SetTime( lastItem.GetTime() );
		exitItem.SetPushback(true);
		lastItem = exitItem;
		newClearance.AddItem(lastItem);
	}

	return false;

}

DeiceStandInSim::DeiceStandInSim(Stand * pStand)
:StandInSim(pStand)
{
	InitDefaultData();
}

DeiceStandInSim::~DeiceStandInSim()
{

}

void DeiceStandInSim::InitDefaultData()
{
	if (m_pStandInput == NULL)
		return;	

	if (m_pStandInput->GetType() != ALT_DEICEBAY)
		return;

	DeicePad* pDeiceInput = (DeicePad*)m_pStandInput.get();
	if (pDeiceInput->GetInContrain().getCount() > 0)
	{
		StandLeadInLineInSim* pNewinLine = new StandLeadInLineInSim(this);
		m_vLeadInLines.push_back(pNewinLine);
	}

	if (pDeiceInput->GetOutConstrain().getCount() > 0)
	{
		StandLeadOutLineInSim* pNewOutLine = new StandLeadOutLineInSim(this);
		m_vLeadOutLines.push_back( pNewOutLine );
	}
}
