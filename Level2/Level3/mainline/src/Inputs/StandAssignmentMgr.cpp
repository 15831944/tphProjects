#include "StdAfx.h"
#include ".\standassignmentmgr.h"

#include "Flight.h"
#include "schedule.h"
//#include "engine\terminal.h"
#include "engine\proclist.h"
#include "engine\gate.h"
#include "Engine\StandProc.h"
#include "AirsideInput.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/CParkingStandBuffer.h"
#include "../InputAirside/TowOffStandAssignmentDataList.h"
#include "../InputAirside/Stand.h"
#include "../InputAirside/ItinerantFlightSchedule.h"
#include "../InputAirside/ACTypeStandConstraint.h"
#include "../InputAirside/ItinerantFlight.h"
#include "FlightForGateAssign.h"
#include "InputAirside\TowOffStandAssignmentData.h"
#include "in_term.h"
#include "FlightPriorityInfo.h"

////////////////////////////////////////////////////////////////////////////////////
bool FlightTimeCompare(FlightForAssignment* pFlight1, FlightForAssignment* pFlight2)
{
	if(pFlight1->getStartTime() < pFlight2->getStartTime())
		return true;

	if (pFlight1->getStartTime() == pFlight2->getStartTime())
	{
		ARCFlight* pArcFlight1 = pFlight1->getFlight();
		ARCFlight* pArcFlight2 = pFlight2->getFlight();
		char flightSz1[128];
		pArcFlight1->getFlightIDString(flightSz1);
		char flightSz2[128];
		pArcFlight2->getFlightIDString(flightSz2);

		if (strcmp(flightSz1,flightSz2) < 0)
			return true;
	}
	return false;
}

bool FlightCountInStandCompare(CAssignStand* pStand1, CAssignStand* pStand2)
{
	if(pStand1->GetFlightCount() < pStand2->GetFlightCount())
		return true;

	if (pStand1->GetFlightCount() == pStand2->GetFlightCount())
	{
		if (pStand1->GetStandIdx() < pStand2->GetStandIdx())
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////
StandAssignmentMgr::StandAssignmentMgr(int nProjID): CGateAssignmentMgr(nProjID)
{
	m_itinerantFlight = new ItinerantFlightSchedule();
//	m_pTowOffStandAssignmentList = new CTowOffStandAssignmentDataList;
}

StandAssignmentMgr::~StandAssignmentMgr(void)
{
	if (m_itinerantFlight)
	{
		delete m_itinerantFlight;
		m_itinerantFlight = NULL;
	}

// 	if (m_pTowOffStandAssignmentList)
// 	{
// 		delete m_pTowOffStandAssignmentList;
// 		m_pTowOffStandAssignmentList = NULL;
// 	}
	//m_vFlightList.clear();
}

bool StandAssignmentMgr::isNeedtoTowOff(ARCFlight* pFlight,int nGateIdx)
{

	return false;
}

bool StandAssignmentMgr::FlightOperationAssignErrorMessage(CFlightOperationForGateAssign* pFlightOperation)
{
	if (pFlightOperation == NULL)
		return false;
	
	int _nGateIdx = pFlightOperation->GetGateIdx();
	ARCFlight* pFlight = pFlightOperation->getFlight();

	CString strStandConError;
	if (!m_constraints.IsFlightAndGateFit(pFlight,((CAssignStand*)&*m_vectGate[_nGateIdx])->GetStandID(),strStandConError))
	{
		pFlightOperation->AddErrorMessage(CFlightOperationForGateAssign::StandConstraintError,strStandConError);
	}

	CString strAdjacencyError;
	if ( !AdjacencyConstraintsConflictErrorMessage(pFlight,_nGateIdx,pFlightOperation->GetStartTime(), pFlightOperation->GetEndTime(),strAdjacencyError) )
	{
		pFlightOperation->AddErrorMessage(CFlightOperationForGateAssign::StandAdjacenceError,strAdjacencyError);
	}

	CString strStandBufferError;
	if (!m_vectGate[_nGateIdx]->StandBufferErrorMessage(pFlight, pFlightOperation->GetStartTime(), pFlightOperation->GetEndTime(), m_constraints.GetParkingStandBufferList(),strStandBufferError))
	{
		pFlightOperation->AddErrorMessage(CFlightOperationForGateAssign::StandBufferError,strStandBufferError);
	}

	CString strOverlapError;
	if (!m_vectGate[_nGateIdx]->OverlapStandErrorMessage(pFlight, pFlightOperation->GetStartTime(), pFlightOperation->GetEndTime(),strOverlapError))
	{
		pFlightOperation->AddErrorMessage(CFlightOperationForGateAssign::StandOverlapError,strOverlapError);
	}

	if (!pFlightOperation->FlightAssignToStandValid())
		return false;
	
	return true;
}

void StandAssignmentMgr::AddFlightToStand(FlightForAssignment* pFlightInStand)
{
	if (pFlightInStand == NULL)
		return;

	//add arr part
	CFlightOperationForGateAssign* pFlightArrOperation = pFlightInStand->GetFlightArrPart();
	if (pFlightArrOperation)
	{
		int nGateIndex = pFlightArrOperation->GetGateIdx();
		if (nGateIndex > -1)
		{
			m_vectGate[nGateIndex]->AddFlight(*pFlightArrOperation);
		}
	}


	//add int part
	CFlightOperationForGateAssign* pFlightIntOperation = pFlightInStand->GetFlightIntPart();
	if (pFlightIntOperation)
	{
		int nGateIndex = pFlightIntOperation->GetGateIdx();
		if (nGateIndex > -1)
		{
			m_vectGate[nGateIndex]->AddFlight(*pFlightIntOperation);
		}
	}


	//add dep part
	CFlightOperationForGateAssign* pFlightDepOperaiton = pFlightInStand->GetFlightDepPart();
	if (pFlightDepOperaiton)
	{
		int nGateIndex = pFlightDepOperaiton->GetGateIdx();
		if (nGateIndex > -1)
		{
			m_vectGate[nGateIndex]->AddFlight(*pFlightDepOperaiton);
		}
	}
}

void StandAssignmentMgr::CreateAssignFlightErrorMessage(FlightForAssignment* pFlightInStand)
{
	if (pFlightInStand == NULL)
		return;
	
	//Check arr part
	FlightOperationAssignErrorMessage(pFlightInStand->GetFlightArrPart());

	//check int part
	FlightOperationAssignErrorMessage(pFlightInStand->GetFlightIntPart());

	//check dep part
	FlightOperationAssignErrorMessage(pFlightInStand->GetFlightDepPart());
}

void StandAssignmentMgr::ResetGateContent(OpType eType, InputTerminal* _pInTerm)
{
	m_vectUnassignedFlight.clear();

	std::vector<int> vAirports;
	InputAirside::GetAirportList(m_nProjID,vAirports);
	int airportID = -1;
	if(vAirports.size()>0) 
		airportID = vAirports.at(0);

	int nFlightCount = m_vFlightList.size();
	for( int i=0; i<nFlightCount; i++ )
	{
		FlightForAssignment* pFlightInStand = m_vFlightList.at(i);
		if (!IsAssignedFlight(pFlightInStand))
		{
			m_vectUnassignedFlight.push_back(pFlightInStand);
			continue;
		}

		if (!IsValidStandAssignment(pFlightInStand))
		{
			RemoveAssignedFlight(pFlightInStand);
			continue;
		}

		CreateAssignFlightErrorMessage(pFlightInStand);

		//put flight onto stand
		AddFlightToStand(pFlightInStand);

		//clear invalid stand
		if (pFlightInStand->GetFlightArrPart() == NULL && !pFlightInStand->getFlight()->getArrStand().IsBlank())
		{
			ALTObjectID objName;
			pFlightInStand->getFlight()->setArrStand(objName);
		}

		if (pFlightInStand->GetFlightDepPart() == NULL && pFlightInStand->getFlight()->getArrStand().GetIDString() != pFlightInStand->getFlight()->getDepStand().GetIDString())
		{
			ALTObjectID objName;
			pFlightInStand->getFlight()->setDepStand(objName);
		}

		if (pFlightInStand->GetFlightIntPart() == NULL && !pFlightInStand->getFlight()->getIntermediateStand().IsBlank())
		{
			ALTObjectID objName;
			pFlightInStand->getFlight()->setIntermediateStand(objName);
		}

		m_vAssignedFlight.push_back(pFlightInStand);
	}
}

bool StandAssignmentMgr::IsValidStandAssignment( FlightForAssignment* pFlight )
{
	int nCount = (int)m_vectGate.size();

	for(int i=0;i< nCount;i++)
	{
		CString strName = ((CAssignStand*)&*m_vectGate[i])->GetStandID().GetIDString();

		if (pFlight->GetFlightArrPart() && pFlight->getFlight()->getArrStand().GetIDString() == strName)
		{
			pFlight->GetFlightArrPart()->SetGateIdx(i);
		//	m_vectGate[i]->AddFlight( *(pFlight->GetFlightArrPart()));
			pFlight->getFlight()->setArrStandID(i);
		}

		if (pFlight->GetFlightDepPart() && pFlight->getFlight()->getDepStand().GetIDString() == strName)
		{
			pFlight->GetFlightDepPart()->SetGateIdx(i);	
		//	m_vectGate[i]->AddFlight( *(pFlight->GetFlightDepPart()));
			pFlight->getFlight()->setDepStandID(i);
		}

		if (pFlight->GetFlightIntPart() && pFlight->getFlight()->getIntermediateStand().GetIDString() == strName)
		{
			pFlight->GetFlightIntPart()->SetGateIdx(i);
		//	m_vectGate[i]->AddFlight( *(pFlight->GetFlightIntPart()));
		}

		if (((pFlight->GetFlightArrPart() && pFlight->GetFlightArrPart()->GetGateIdx()>= 0)|| pFlight->GetFlightArrPart() == NULL)
			&& ((pFlight->GetFlightDepPart() && pFlight->GetFlightDepPart()->GetGateIdx()>= 0)|| pFlight->GetFlightDepPart() == NULL)
			&& ((pFlight->GetFlightIntPart() && pFlight->GetFlightIntPart()->GetGateIdx()>= 0)|| pFlight->GetFlightIntPart() == NULL))
			return true;

	}


	return false;

}

bool StandAssignmentMgr::InitFlightStandPartsAccordingToTowoffCriteria( FlightForAssignment* pFlightForAssign )
{
	CTowOffStandAssignmentDataList* pTowoffStandDataList = GetTowoffStandDataList();
	if (pTowoffStandDataList == NULL)
	{
		return false;
	}
	ARCFlight* pFlight =pFlightForAssign->getFlight();

	CTowOffStandAssignmentData* pFltData = pTowoffStandDataList->GetFlightTowoffData(pFlight,pFlight->getStand());
	CFlightOperationForGateAssign* pfltOp = NULL;
	//if (pFlightForAssign->getFlight()->isTurnaround() && (pFltData == NULL || (pFltData && !pFltData->IsTow())))		//not tow
	//{
	//	pfltOp = new CFlightOperationForGateAssign(pFlightForAssign, ARR_DEP_OP);
	//	pfltOp->SetTimeRange(pFlightForAssign->getStartTime(),pFlightForAssign->getEndTime());
	//	pFlightForAssign->SetFlightArrPart(pfltOp);
	//}
	//else
	{	
		m_TimePartOfFlight.GetFlightTimePartAccordingToTowData(pFlightForAssign,pFltData);
		FlightPartAccordingToTowData::TimePart timeArrPart, timeIntPart, timeDepPart;

		timeArrPart= m_TimePartOfFlight.GetArrPart();
		if (timeArrPart.tStart > -1L)
		{
			if (pFlight->getArrStand() == pFlight->getDepStand() &&  m_TimePartOfFlight.GetIntPart().tStart == -1L)		//same arr stand and dep stand without intermediate stand
			{
				pfltOp = new CFlightOperationForGateAssign(pFlightForAssign, ARR_DEP_OP);
				pfltOp->SetTimeRange(pFlightForAssign->getStartTime(),pFlightForAssign->getEndTime());
				pFlightForAssign->SetFlightArrPart(pfltOp);
			}
			else
			{
				pfltOp = new CFlightOperationForGateAssign(pFlightForAssign, ARR_OP);
				pfltOp->SetTimeRange(timeArrPart.tStart,timeArrPart.tEnd);
				pFlight->setArrParkingTime(timeArrPart.tEnd - timeArrPart.tStart);
			}

			pFlightForAssign->SetFlightArrPart(pfltOp);
		}

		timeDepPart = m_TimePartOfFlight.GetDepPart();
		if (timeDepPart.tStart > -1L)
		{
			pfltOp = new CFlightOperationForGateAssign(pFlightForAssign, DEP_OP);
			pfltOp->SetTimeRange(timeDepPart.tStart,timeDepPart.tEnd);
			pFlight->setDepParkingTime(timeDepPart.tEnd - timeDepPart.tStart);
			pFlightForAssign->SetFlightDepPart(pfltOp);
		}

		timeIntPart = m_TimePartOfFlight.GetIntPart();
		if (timeIntPart.tStart > -1L)
		{
			pfltOp = new CFlightOperationForGateAssign(pFlightForAssign, INTPAKRING_OP);
			pfltOp->SetTimeRange(timeIntPart.tStart,timeIntPart.tEnd);
			pFlight->setIntParkingTime(timeIntPart.tEnd - timeIntPart.tStart);
			pFlightForAssign->SetFlightIntPart(pfltOp);
		}


	}

	return true;
}

CTowOffStandAssignmentDataList* StandAssignmentMgr::GetTowoffStandDataList()
{
	TowoffStandStrategy* pTowoffStrategy = m_towoffStandConstraint.GetActiveStrategy();
	if (pTowoffStrategy == NULL)
		return NULL;
	
	return pTowoffStrategy->GetTowoffStandDataList();
}

void StandAssignmentMgr::LoadData(InputTerminal* _pInTerm, const PROJECTINFO& ProjInfo)
{
	m_constraints.SetInputTerminal(_pInTerm);
	m_constraints.loadDataSet(ProjInfo.path);

	m_constraints.GetStandAdjConstraint().loadDataSet(ProjInfo.path,_pInTerm);

	m_constraints.GetACTypeStandConstraints()->ReadData();

	m_towoffStandConstraint.SetAirportDatabase(_pInTerm->m_pAirportDB);
	m_towoffStandConstraint.ReadData(-1);
// 	m_pTowOffStandAssignmentList->SetAirportDatabase(_pInTerm->m_pAirportDB);
// 	m_pTowOffStandAssignmentList->ReadData(m_nProjID);

	m_vectGate.clear();

	std::vector<int> vAirports;
	InputAirside::GetAirportList(m_nProjID,vAirports);
	int airportID = -1;
	if(vAirports.size()>0) 
		airportID = vAirports.at(0);

	ALTObjectList standobj;
	ALTAirport::GetStandList(airportID,standobj);

	size_t nSize = standobj.size();
	for(size_t i=0;i< nSize;i++)
	{
		ALTObject * pObj = standobj[i].get();
		CAssignStand* pStand = new CAssignStand();

		if (!pStand)
			continue;
		pStand->SetParent(this);
		pStand->SetALTObjectID(pObj->GetObjectName());
		m_vectGate.push_back( pStand );
	}
	sortAllGateByName();

	nSize = m_vectGate.size();
	for(size_t i=0;i< nSize;i++)
	{
		((CAssignStand*)GetGate(i))->SetStandIdx((int)i);
	}

	m_constraints.GetStandAdjConstraint().InitConstraintGateIdx(m_vectGate);
	int nFlightCount = _pInTerm->flightSchedule->getCount();
	for( int i=0; i<nFlightCount; i++ )
	{
		ARCFlight* pFlight = _pInTerm->flightSchedule->getItem( i );
		FlightForAssignment* pFlightInStand = new FlightForAssignment(pFlight);
		
		AssignFlightToStand(pFlightInStand);
		m_vFlightList.push_back(pFlightInStand);
	}

	m_itinerantFlight->SetAirportDB(_pInTerm->m_pAirportDB);
	m_itinerantFlight->ReadData(m_nProjID);
	nFlightCount = (int)m_itinerantFlight->GetElementCount();
	for (int ii = 0; ii < nFlightCount;ii++)
	{
		ARCFlight* pFlight = m_itinerantFlight->GetItem(ii);
		ItinerantFlightScheduleItem* pFlightItem = (ItinerantFlightScheduleItem*)pFlight;
		if (pFlightItem->GetEnRoute())
		{
			continue;
		}
		FlightForAssignment* pFligtInStand = new FlightForAssignment(pFlight);
		AssignFlightToStand(pFligtInStand);
		m_vFlightList.push_back(pFligtInStand);
	}


}

void StandAssignmentMgr::AssignFlightToStand(FlightForAssignment* pFligtInStand)
{
	ARCFlight* pFlight = pFligtInStand->getFlight();

	CFlightOperationForGateAssign* pfltOp = NULL;
	ElapsedTime arrStartTime;
	ElapsedTime arrEndTime;
	if (pFligtInStand->getArrStartTime(arrStartTime) && pFligtInStand->getArrEndTime(arrEndTime))
	{
		if (pFlight->isTurnaround() && pFlight->GetTowoffTime() == -1l)
		{
			pfltOp = new CFlightOperationForGateAssign(pFligtInStand,ARR_DEP_OP);
		}
		else
		{
			pfltOp = new CFlightOperationForGateAssign(pFligtInStand, ARR_OP);
		}
		pfltOp->SetTimeRange(arrStartTime,arrEndTime);
		pFligtInStand->SetFlightArrPart(pfltOp);
	}

	ElapsedTime intStartTime;
	ElapsedTime intEndTime;
	CString strError;
	if (pFligtInStand->getIntStartTime(intStartTime,strError) && pFligtInStand->getIntEndTime(intEndTime,strError))
	{
		pfltOp = new CFlightOperationForGateAssign(pFligtInStand, INTPAKRING_OP);
		pfltOp->SetTimeRange(intStartTime,intEndTime);
		pFlight->setIntParkingTime(intEndTime - intStartTime);
		pFligtInStand->SetFlightIntPart(pfltOp);
		if (!strError.IsEmpty())
		{
			pfltOp->AddErrorMessage(CFlightOperationForGateAssign::StandIntermdiateTimeError,strError);
		}
	}

	ElapsedTime depStartTime;
	ElapsedTime depEndTime;
	if (pFligtInStand->getDepStartTime(depStartTime) && pFligtInStand->getDepEndTime(depEndTime))
	{
		pfltOp = new CFlightOperationForGateAssign(pFligtInStand, DEP_OP);
		pfltOp->SetTimeRange(depStartTime,depEndTime);
		pFlight->setDepParkingTime(depEndTime - depStartTime);
		pFligtInStand->SetFlightDepPart(pfltOp);
	}
}

// check flight whether can be assigned in a stand
bool StandAssignmentMgr::IsAWholeFlightFitInGate( int _nGateIdx, FlightForAssignment* pFlight )
{
	if( !pFlight->IsSelected() )
		return true;

	//constraints
	
	if (!m_constraints.IsFlightAndGateFit(pFlight->getFlight(),((CAssignStand*)&*m_vectGate[_nGateIdx])->GetStandID()))
		return false;

	if ( !IsAdjacencyConstraintsConflict(pFlight->getFlight(),_nGateIdx,pFlight->getStartTime(), pFlight->getEndTime()) )
		return false;

	if (!m_vectGate[_nGateIdx]->BeAbletoTake(pFlight->getFlight(),pFlight->getStartTime(), pFlight->getEndTime(),m_constraints.GetParkingStandBufferList()))
		return false;

	return true;

}

bool StandAssignmentMgr::AdjacencyConstraintsConflictErrorMessage(ARCFlight* flight,int nGateIdx,const ElapsedTime& tStart, const ElapsedTime& tEnd,CString& strError)
{
	ElapsedTime leftTime, rightTime;
	ElapsedTime bufTime = 0L;

	bufTime = m_constraints.GetParkingStandBufferList()->GetBufferTime(flight->getLogEntry(), flight->getLogEntry());
	leftTime = tStart - bufTime;
	rightTime = tEnd + bufTime;

	//adjacency constrain need consider of buffer time
	if (!m_constraints.GetStandAdjConstraint().StandAdjacencyFitErrorMessage(flight,nGateIdx,leftTime, rightTime,m_vectGate,strError))
		return false;

	return true;
}

bool StandAssignmentMgr::IsAdjacencyConstraintsConflict(ARCFlight* flight,int nGateIdx,const ElapsedTime& tStart, const ElapsedTime& tEnd)
{
	ElapsedTime leftTime, rightTime;
	ElapsedTime bufTime = 0L;

	bufTime = m_constraints.GetParkingStandBufferList()->GetBufferTime(flight->getLogEntry(), flight->getLogEntry());
	leftTime = tStart - bufTime;
	rightTime = tEnd + bufTime;

	//adjacency constrain need consider of buffer time
	if (!m_constraints.GetStandAdjConstraint().StandAdjacencyFit(flight,nGateIdx,leftTime, rightTime,m_vectGate))
		return false;

	return true;
}

bool StandAssignmentMgr::IsFlightOperationFitInGate( int _nGateIdx, CFlightOperationForGateAssign* pFlight )
{
	if( !pFlight->IsSelected() )
		return true;

	//constraints checker
	if (!m_constraints.IsFlightAndGateFit(pFlight->getFlight(),((CAssignStand*)&*m_vectGate[_nGateIdx])->GetStandID()))
		return false;

	if ( !IsAdjacencyConstraintsConflict(pFlight->getFlight(),_nGateIdx,pFlight->GetStartTime(), pFlight->GetEndTime()) )
		return false;

	if (!m_vectGate[_nGateIdx]->BeAbletoTake(pFlight->getFlight(),pFlight->GetStartTime(),pFlight->GetEndTime(),m_constraints.GetParkingStandBufferList()))
		return false;

	return true;
}

bool StandAssignmentMgr::IsFlightPartFitInGate( int _nGateIdx,ARCFlight* pFlight,const ElapsedTime& tStart,const ElapsedTime& tEnd )
{
	//constraints checker
	if (!m_constraints.IsFlightAndGateFit(pFlight,((CAssignStand*)&*m_vectGate[_nGateIdx])->GetStandID()))
		return false;

	if ( !IsAdjacencyConstraintsConflict(pFlight,_nGateIdx,tStart, tEnd) )
		return false;

	if (!m_vectGate[_nGateIdx]->BeAbletoTake(pFlight, tStart, tEnd, m_constraints.GetParkingStandBufferList()))
		return false;

	return true;
}

GateAssignmentConstraintList& StandAssignmentMgr::getConstraint()
{
	return m_constraints;
}

bool StandAssignmentMgr::ProcessAssignFailedError(int _nGateIdx, FlightForAssignment* pFlightAssign,CString& strError)
{
	CAssignStand* pStand = (CAssignStand*)GetGate(_nGateIdx);
	CTowOffStandAssignmentDataList* pTowoffStandDataList = GetTowoffStandDataList();
	if (pTowoffStandDataList == NULL)
	{
		return false;
	}
	CTowOffStandAssignmentData* pTowData = pTowoffStandDataList->GetFlightTowoffData(pFlightAssign->getFlight(),pStand->GetStandID());
	m_TimePartOfFlight.GetFlightTimePartAccordingToTowData(pFlightAssign,pTowData);
	FlightPartAccordingToTowData::TimePart timeArrPart, timeIntPart, timeDepPart;
	timeArrPart= m_TimePartOfFlight.GetArrPart();

	if (timeArrPart.tStart > -1L)			//with arr part
	{
		CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlightAssign, ARR_OP);
		pfltOp->SetTimeRange(timeArrPart.tStart,timeArrPart.tEnd);
		pfltOp->SetGateIdx(_nGateIdx);
		pFlightAssign->SetFlightArrPart(pfltOp);
		if (!FlightOperationAssignErrorMessage(pfltOp))
		{
			pfltOp->FlightAssignToStandValid(strError);
			pFlightAssign->ClearAllOperations();
			return false;
		}
	}

	timeDepPart = m_TimePartOfFlight.GetDepPart();
	if (timeDepPart.tStart > -1L)		//with dep part
	{
		CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlightAssign, DEP_OP);
		pfltOp->SetTimeRange(timeDepPart.tStart,timeDepPart.tEnd);
		pfltOp->SetGateIdx(_nGateIdx);
		pFlightAssign->SetFlightDepPart(pfltOp);
		if (!FlightOperationAssignErrorMessage(pfltOp))
		{
			pfltOp->FlightAssignToStandValid(strError);
			pFlightAssign->ClearAllOperations();
			return false;
		}
	}

	timeIntPart = m_TimePartOfFlight.GetIntPart();
	if (timeIntPart.tStart > -1L)		//with 
	{
		if (pFlightAssign == NULL || pTowData == NULL)
			return true;

		if (pFlightAssign->GetFlightIntPart() || pTowData->IsTow() == FALSE)
			return true;

		if (pTowData->IsTow() && pTowData->GetRepositionOrFreeup())
			return true;

		int nCount = pTowData->GetListPriorities().GetElementCount();
		CTowOffPriorityEntry* pData = NULL;
		CTowOffStandReturnToStandEntry* pStandData = NULL;

		ElapsedTime tDeplane = ElapsedTime(pTowData->GetDeplanementTime()* 60L);
		ElapsedTime tEnplane = ElapsedTime(pTowData->GetEnplanementTime()* 60L);
		int nStandCount = m_vectGate.size();
		for (int j =0; j < nStandCount; j++)
		{
			for (int i =0; i < nCount; i++)
			{
				pData = pTowData->GetListPriorities().GetItem(i);
				CAssignStand* pStand = (CAssignStand*)GetGate(j);
				if ( !pStand->GetStandID().idFits(pData->GetStandName()) )
					continue;

				if (IsFlightPartFitInGate(j, pFlightAssign->getFlight(),pFlightAssign->getStartTime()+ tDeplane,pFlightAssign->getEndTime()-tEnplane))
				{
					return true;
				}
			}
		}

		strError = CString(_T("Flight can't been assigned to valid Intermediate stand according to stand constraint!!."));
		pFlightAssign->ClearAllOperations();
		return false;
	}

	return true;
}

//the function is used for drag selected flights to a definite stand/gate
int StandAssignmentMgr::AssignSelectedFlightToGate(int _nGateIdx, std::vector<int>& vGateIdx)
{
	int nFailedCount = 0;
	int nFlightCount = m_vectUnassignedFlight.size();
	CTowOffStandAssignmentDataList* pTowoffStandDataList = GetTowoffStandDataList();
	if (pTowoffStandDataList == NULL)
	{
		return 0;
	}
	for( int i=nFlightCount-1; i>=0; i-- )
	{
		FlightForAssignment* flight = m_vectUnassignedFlight[i];
		bool bMatch = true;
		if( flight->IsSelected())
		{
			CAssignStand* pStand = (CAssignStand*)GetGate(_nGateIdx);
			CTowOffStandAssignmentData* pTowData = pTowoffStandDataList->GetFlightTowoffData(flight->getFlight(),pStand->GetStandID());
			m_TimePartOfFlight.GetFlightTimePartAccordingToTowData(flight,pTowData);
			FlightPartAccordingToTowData::TimePart timeArrPart, timeIntPart, timeDepPart;
			timeArrPart= m_TimePartOfFlight.GetArrPart();
			if (timeArrPart.tStart > -1L)			//with arr part
			{
				if (!IsFlightPartFitInGate(_nGateIdx,flight->getFlight(),timeArrPart.tStart,timeArrPart.tEnd))
				{
					nFailedCount++;
					continue;
				}
			}

			timeDepPart = m_TimePartOfFlight.GetDepPart();
			if (timeDepPart.tStart > -1L)		//with dep part
			{
				if (!IsFlightPartFitInGate(_nGateIdx,flight->getFlight(),timeDepPart.tStart,timeDepPart.tEnd))
				{
					nFailedCount++;
					continue;
				}
			}

			timeIntPart = m_TimePartOfFlight.GetIntPart();
			if (timeIntPart.tStart > -1L)		//with 
			{
				if (!tryToAssignIntermediateParkingStand(flight, pTowData))
				{
					nFailedCount++;
					continue;
				}
			}

			if (flight->getFlight()->isTurnaround() && (pTowData == NULL ||(pTowData && pTowData->IsTow() == FALSE)))		//not tow
			{
				CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(flight, ARR_DEP_OP);
				pfltOp->SetTimeRange(flight->getStartTime(),flight->getEndTime());
				flight->SetFlightArrPart(pfltOp);
				pfltOp->SetGateIdx(_nGateIdx);
				pfltOp->getFlight()->setArrStandID(_nGateIdx);
				m_vectGate[_nGateIdx]->AddFlight(*pfltOp);
			}
			else
			{
				if (timeArrPart.tStart > -1L)
				{
					CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(flight, ARR_OP);
					pfltOp->SetTimeRange(timeArrPart.tStart,timeArrPart.tEnd);
					flight->SetFlightArrPart(pfltOp);
					pfltOp->SetGateIdx(_nGateIdx);
					pfltOp->getFlight()->setArrStandID(_nGateIdx);
					m_vectGate[_nGateIdx]->AddFlight(*pfltOp);

				}

				if (timeDepPart.tStart > -1L)
				{
					CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(flight, DEP_OP);
					pfltOp->SetTimeRange(timeDepPart.tStart,timeDepPart.tEnd);
					flight->SetFlightDepPart(pfltOp);
					pfltOp->SetGateIdx(_nGateIdx);
					pfltOp->getFlight()->setDepStandID(_nGateIdx);
					m_vectGate[_nGateIdx]->AddFlight(*pfltOp);

				}

			}
			vGateIdx.push_back(_nGateIdx);			
			if (flight->GetFlightIntPart())
			{
				vGateIdx.push_back(flight->GetFlightIntPart()->GetGateIdx());
			}

 			m_vAssignedFlight.push_back(flight);
 			m_vectUnassignedFlight.erase( m_vectUnassignedFlight.begin() + i );
		}
	}

	return nFailedCount;
}


int StandAssignmentMgr::tryToAssignFlightPartToAppointedStandList(FlightForAssignment* pFlight,const ElapsedTime& tStart,const ElapsedTime& tEnd, std::vector<int> vGateIdxList )
{
	int nCount = vGateIdxList.size();
	for (int i =0; i < nCount; i++)
	{
		if(IsFlightPartFitInGate(vGateIdxList.at(i),pFlight->getFlight(),tStart,tEnd))
			return vGateIdxList.at(i);
	}

	return -1;
}

bool StandAssignmentMgr::tryToAssignIntermediateParkingStand(FlightForAssignment* pFlight,CTowOffStandAssignmentData* pTowData)
{
	if (pFlight == NULL || pTowData == NULL)
		return true;

	if (pFlight->GetFlightIntPart() || pTowData->IsTow() == FALSE)
		return true;

	if (pTowData->IsTow() && pTowData->GetRepositionOrFreeup())
		return true;

	int nCount = pTowData->GetListPriorities().GetElementCount();
	CTowOffPriorityEntry* pData = NULL;
	CTowOffStandReturnToStandEntry* pStandData = NULL;

	ElapsedTime tDeplane = ElapsedTime(pTowData->GetDeplanementTime()* 60L);
	ElapsedTime tEnplane = ElapsedTime(pTowData->GetEnplanementTime()* 60L);
	int nStandCount = m_vectGate.size();
	for (int j =0; j < nStandCount; j++)
	{
		for (int i =0; i < nCount; i++)
		{
			pData = pTowData->GetListPriorities().GetItem(i);
			CAssignStand* pStand = (CAssignStand*)GetGate(j);
			if ( !pStand->GetStandID().idFits(pData->GetStandName()) )
				continue;

			if (IsFlightPartFitInGate(j, pFlight->getFlight(),pFlight->getStartTime()+ tDeplane,pFlight->getEndTime()-tEnplane/* - tLeave*/))
			{
				CFlightOperationForGateAssign* pIntOp = new CFlightOperationForGateAssign(pFlight, INTPAKRING_OP);
				pIntOp->SetTimeRange(pFlight->getStartTime()+ tDeplane,pFlight->getEndTime()-tEnplane/* - tLeave*/);
				pIntOp->SetGateIdx(j);
				pFlight->SetFlightIntPart(pIntOp);
				m_vectGate[j]->AddFlight(*pIntOp);
				pFlight->getFlight()->setIntermediateStand(pStand->GetStandID());
				pFlight->getFlight()->SetTowoffTime(pFlight->getStartTime() + tDeplane);
				pFlight->getFlight()->SetExIntStandTime(pFlight->getEndTime() - tEnplane);
				return true;
			}
		}
	}

	return false;
}

void StandAssignmentMgr::RemoveAssignedFlight(FlightForAssignment* pFlight)
{	
	CFlightOperationForGateAssign* _flight = NULL;
	_flight = pFlight->GetFlightArrPart();
	if(_flight)
	{
		int nGateIdx = _flight->GetGateIdx();
		if (nGateIdx > -1)
			m_vectGate[nGateIdx]->RemoveAssignedFlight(*_flight);
	}

	_flight = pFlight->GetFlightDepPart();
	if(_flight)
	{
		int nGateIdx = _flight->GetGateIdx();
		if (nGateIdx > -1)
			m_vectGate[nGateIdx]->RemoveAssignedFlight(*_flight);
	}

	_flight = pFlight->GetFlightIntPart();
	if (_flight)
	{
		int nGateIdx = _flight->GetGateIdx();
		if (nGateIdx > -1)
			m_vectGate[nGateIdx]->RemoveAssignedFlight(*_flight);
	}

	std::vector<FlightForAssignment*>::iterator iter = std::find(m_vAssignedFlight.begin(),m_vAssignedFlight.end(),pFlight);
	if (iter != m_vAssignedFlight.end())
	{
		m_vAssignedFlight.erase(iter);
	}

	iter = std::find(m_vectUnassignedFlight.begin(),m_vectUnassignedFlight.end(),pFlight);
	if (iter == m_vectUnassignedFlight.end())
		m_vectUnassignedFlight.push_back(pFlight);

	pFlight->ClearAllOperations();
}



void StandAssignmentMgr::RemoveSelectedFlight()
{
	std::vector<CFlightOperationForGateAssign> vectFlight;
	vectFlight.clear();
	int nGateCount = m_vectGate.size();
	for( int i=0; i<nGateCount; i++ )
	{
		m_vectGate[i]->RemoveSelectedFlight( vectFlight );
	}

	int nSize = vectFlight.size();
	std::vector<FlightForAssignment*>::iterator iter;
	for (int i =0; i < nSize; i++)
	{
		FlightForAssignment* pFlight = vectFlight[i].GetAssignmentFlight();
		iter = std::find(m_vAssignedFlight.begin(),m_vAssignedFlight.end(),pFlight);
		if (iter != m_vAssignedFlight.end())
		{
			m_vAssignedFlight.erase(iter);
		}
		iter = std::find(m_vectUnassignedFlight.begin(),m_vectUnassignedFlight.end(),pFlight);
		if (iter != m_vectUnassignedFlight.end())
			continue;

		pFlight->ClearAllOperations();
		m_vectUnassignedFlight.push_back(pFlight);
		//RemoveAssignedFlight(pFlight);
	} 	
}

void StandAssignmentMgr::Save(InputTerminal* _pInTerm, const CString& _csProjPath )
{

	int nUnAssignFlight = m_vectUnassignedFlight.size();
	for (int j =0; j < nUnAssignFlight; j++)
	{
		m_vectUnassignedFlight[j]->EmptyStand();
	}
	CGateAssignmentMgr::Save(_pInTerm,_csProjPath);
	m_itinerantFlight->SaveData(m_nProjID);
}

bool StandAssignmentMgr::IsAssignedFlight( FlightForAssignment* pFlight )
{
	if (pFlight->getFlight()->isArriving() && pFlight->getFlight()->getArrStand().IsBlank())
		return false;

	if (pFlight->getFlight()->isDeparting() && pFlight->getFlight()->getDepStand().IsBlank())
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------------------
//Summary:
//		assign flight by single gate priority
//Parameter:
//		nGateIdx: single priority rule
//		vGateIdxList: assign dep part with all gate priority
//return:
//		true: assign success
//		false: failed
//-----------------------------------------------------------------------------------------------------------
bool StandAssignmentMgr::AssignFlightArccordingToGatePriority(int nGateIdx,const std::vector<int>& vGateIdxList)
{
	CTowOffStandAssignmentDataList* pTowoffStandDataList = GetTowoffStandDataList();
	if (pTowoffStandDataList == NULL)
	{
		return false;
	}

	int nCount = (int)m_vectUnassignedFlight.size();
	for (int i = 0; i < nCount; i++)
	{
		FlightForAssignment* pFlight = m_vectUnassignedFlight.at(i);
		pFlight->SetSelected(true);

		OpType _optype = ARR_DEP_OP;
		if (!pFlight->getFlight()->isArriving())
			_optype = DEP_OP;
		if (!pFlight->getFlight()->isDeparting())
			_optype = ARR_OP;

		CAssignStand* pGate = (CAssignStand*)GetGate(nGateIdx);
		CTowOffStandAssignmentData* pTowData = NULL;
		pTowData = pTowoffStandDataList->GetFlightTowoffData(pFlight->getFlight(),pGate->GetStandID());

		if (pTowData == NULL || (pTowData && pTowData->IsTow() == FALSE))	//not tow off
		{
			if (IsAWholeFlightFitInGate(nGateIdx,pFlight))
			{
				CFlightOperationForGateAssign* pFltOp = new CFlightOperationForGateAssign(pFlight,_optype);
				pFltOp->SetTimeRange(pFlight->getStartTime(),pFlight->getEndTime());
				pFltOp->SetGateIdx(nGateIdx);
				pFlight->SetFlightIntPart(pFltOp);
				m_vectGate[nGateIdx]->AddFlight(*pFltOp);
				pFlight->getFlight()->setStandID(nGateIdx);

				m_vAssignedFlight.push_back(pFlight);						
				m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+i);	

				i--;
				nCount--;
				continue;
			}
		}
		else
		{
			if (pTowData->GetRepositionOrFreeup())		//reposition for dep stand
			{
				if (IsAWholeFlightFitInGate(nGateIdx,pFlight))		//try to assign the whole flight to a stand
				{
					CFlightOperationForGateAssign* pFltOp = new CFlightOperationForGateAssign(pFlight,_optype);
					pFltOp->SetTimeRange(pFlight->getStartTime(),pFlight->getEndTime());
					pFltOp->SetGateIdx(nGateIdx);
					pFlight->SetFlightIntPart(pFltOp);
					m_vectGate[nGateIdx]->AddFlight(*pFltOp);
					pFlight->getFlight()->setStandID(nGateIdx);

					m_vAssignedFlight.push_back(pFlight);						
					m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+i);		
					i--;
					nCount--;
					continue;
				}

				ElapsedTime tArrParking = 0L;
				if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
					tArrParking = pFlight->getStartTime() + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
				else
					tArrParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

				if (IsFlightPartFitInGate(nGateIdx,pFlight->getFlight(),pFlight->getStartTime(),tArrParking))
				{
					int nDepIdx = tryToAssignFlightPartToAppointedStandList(pFlight,tArrParking, pFlight->getEndTime(),vGateIdxList);

					if (nDepIdx > -1)	//assigned
					{
						CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, ARR_OP);
						pfltOp->SetTimeRange(pFlight->getStartTime(),tArrParking);
						pFlight->SetFlightArrPart(pfltOp);
						pfltOp->SetGateIdx(nGateIdx);
						pfltOp->getFlight()->setArrStandID(nGateIdx);
						m_vectGate[nGateIdx]->AddFlight(*pfltOp);

						pfltOp = new CFlightOperationForGateAssign(pFlight, DEP_OP);
						pfltOp->SetTimeRange(tArrParking,pFlight->getEndTime());
						pFlight->SetFlightDepPart(pfltOp);
						pfltOp->SetGateIdx(nDepIdx);
						pfltOp->getFlight()->setDepStandID(nDepIdx);
						m_vectGate[nDepIdx]->AddFlight(*pfltOp);

						m_vAssignedFlight.push_back(pFlight);						
						m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+i);	
						i--;
						nCount--;
						continue;
					}
				}
			}
			else		//intermediate stand parking
			{
				ElapsedTime tArrParking = pFlight->getStartTime()+ ElapsedTime(pTowData->GetDeplanementTime()*60L);
				if ((pFlight->getFlight()->isArriving() &&IsFlightPartFitInGate(nGateIdx, pFlight->getFlight(),pFlight->getStartTime(),tArrParking))
					|| !pFlight->getFlight()->isArriving())
				{
					ElapsedTime tDepParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetEnplanementTime()*60L);
					bool bDepDiffernt = pTowData->GetBackToArrivalStand() == TRUE? true: false;
					int nDepIdx = -1;
					if (bDepDiffernt)		//arrival stand different with departure stand
						nDepIdx = tryToAssignFlightPartToAppointedStandList(pFlight,tDepParking,pFlight->getEndTime(),vGateIdxList);
					else
					{
						if (IsFlightPartFitInGate(nGateIdx, pFlight->getFlight(),tDepParking,pFlight->getEndTime()))
							nDepIdx = nGateIdx;
					}

					if (nDepIdx > -1)
					{
						if (tryToAssignIntermediateParkingStand(pFlight,pTowData))
						{
							if (pFlight->getFlight()->isArriving())
							{
								CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, ARR_OP);
								pfltOp->SetTimeRange(pFlight->getStartTime(),tArrParking);
								pFlight->SetFlightArrPart(pfltOp);
								pfltOp->SetGateIdx(nGateIdx);
								pfltOp->getFlight()->setArrStandID(nGateIdx);
								m_vectGate[nGateIdx]->AddFlight(*pfltOp);
							}

							if (pFlight->getFlight()->isDeparting())
							{
								CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, DEP_OP);
								pfltOp->SetTimeRange(tDepParking,pFlight->getEndTime());
								pFlight->SetFlightDepPart(pfltOp);
								pfltOp->SetGateIdx(nDepIdx);
								pfltOp->getFlight()->setDepStandID(nDepIdx);
								m_vectGate[nDepIdx]->AddFlight(*pfltOp);
							}


							m_vAssignedFlight.push_back(pFlight);						
							m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+i);	

							i--;
							nCount--;
							continue;						
						}					
					}
				}
			}
		}


		pFlight->SetSelected(false);
	}

	return true;
}

bool StandAssignmentMgr::AssignFlightAccordingToGatePrioritySequence( int nFltIdx,const std::vector<int>& vGateIdxList)
{
	
	FlightForAssignment* pFlight = m_vectUnassignedFlight.at(nFltIdx);
	pFlight->SetSelected(true);

	int nListCount =(int) vGateIdxList.size();

	OpType _optype = ARR_DEP_OP;
	if (!pFlight->getFlight()->isArriving())
		_optype = DEP_OP;
	if (!pFlight->getFlight()->isDeparting())
		_optype = ARR_OP;

	CTowOffStandAssignmentDataList* pTowoffStandDataList = GetTowoffStandDataList();
	if (pTowoffStandDataList == NULL)
		return false;
	
	CTowOffStandAssignmentData* pTowData = NULL;
	for(int i = 0; i < nListCount; i++)
	{
		int idx = vGateIdxList.at(i);

		CAssignStand* pGate = (CAssignStand*)GetGate(idx);

		pTowData = pTowoffStandDataList->GetFlightTowoffData(pFlight->getFlight(),pGate->GetStandID());
		if (pTowData == NULL || (pTowData && pTowData->IsTow() == FALSE))	//not tow off
		{
			if (IsAWholeFlightFitInGate(idx,pFlight))
			{
				CFlightOperationForGateAssign* pFltOp = new CFlightOperationForGateAssign(pFlight,_optype);
				pFltOp->SetTimeRange(pFlight->getStartTime(),pFlight->getEndTime());
				pFltOp->SetGateIdx(idx);
				pFlight->SetFlightIntPart(pFltOp);
				m_vectGate[idx]->AddFlight(*pFltOp);
				pFlight->getFlight()->setStandID(idx);

				m_vAssignedFlight.push_back(pFlight);						
				m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);			
				return true;
			}
		}
		else
		{
			if (pTowData->GetRepositionOrFreeup())		//reposition for dep stand
			{
				if (IsAWholeFlightFitInGate(idx,pFlight))		//try to assign the whole flight to a stand
				{
					CFlightOperationForGateAssign* pFltOp = new CFlightOperationForGateAssign(pFlight,_optype);
					pFltOp->SetTimeRange(pFlight->getStartTime(),pFlight->getEndTime());
					pFltOp->SetGateIdx(idx);
					pFlight->SetFlightIntPart(pFltOp);
					m_vectGate[idx]->AddFlight(*pFltOp);
					pFlight->getFlight()->setStandID(idx);

					m_vAssignedFlight.push_back(pFlight);						
					m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);			
					return true;
				}
				
				ElapsedTime tArrParking = 0L;
				if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
					tArrParking = pFlight->getStartTime() + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
				else
					tArrParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

				if (IsFlightPartFitInGate(idx,pFlight->getFlight(),pFlight->getStartTime(),tArrParking))
				{
					int nDepIdx = tryToAssignFlightPartToAppointedStandList(pFlight,tArrParking, pFlight->getEndTime(),vGateIdxList);

					if (nDepIdx > -1)	//assigned
					{
						CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, ARR_OP);
						pfltOp->SetTimeRange(pFlight->getStartTime(),tArrParking);
						pFlight->SetFlightArrPart(pfltOp);
						pfltOp->SetGateIdx(idx);
						pfltOp->getFlight()->setArrStandID(idx);
						m_vectGate[idx]->AddFlight(*pfltOp);

						pfltOp = new CFlightOperationForGateAssign(pFlight, DEP_OP);
						pfltOp->SetTimeRange(tArrParking,pFlight->getEndTime());
						pFlight->SetFlightDepPart(pfltOp);
						pfltOp->SetGateIdx(nDepIdx);
						pfltOp->getFlight()->setDepStandID(nDepIdx);
						m_vectGate[nDepIdx]->AddFlight(*pfltOp);

						m_vAssignedFlight.push_back(pFlight);						
						m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);	
						return true;
					}
				}
			}
			else		//intermediate stand parking
			{
				ElapsedTime tArrParking = pFlight->getStartTime()+ ElapsedTime(pTowData->GetDeplanementTime()*60L);
				if ((pFlight->getFlight()->isArriving() &&IsFlightPartFitInGate(idx, pFlight->getFlight(),pFlight->getStartTime(),tArrParking))
					|| !pFlight->getFlight()->isArriving())
				{
					ElapsedTime tDepParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetEnplanementTime()*60L);
					bool bDepDiffernt = pTowData->GetBackToArrivalStand() == TRUE? true: false;
					int nDepIdx = -1;
					if (bDepDiffernt)		//arrival stand different with departure stand
						nDepIdx = tryToAssignFlightPartToAppointedStandList(pFlight,tDepParking,pFlight->getEndTime(),vGateIdxList);
					else
					{
						if (IsFlightPartFitInGate(idx, pFlight->getFlight(),tDepParking,pFlight->getEndTime()))
							nDepIdx = idx;
					}

					if (nDepIdx > -1)
					{
						if (tryToAssignIntermediateParkingStand(pFlight,pTowData))
						{
							if (pFlight->getFlight()->isArriving())
							{
								CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, ARR_OP);
								pfltOp->SetTimeRange(pFlight->getStartTime(),tArrParking);
								pFlight->SetFlightArrPart(pfltOp);
								pfltOp->SetGateIdx(idx);
								pfltOp->getFlight()->setArrStandID(idx);
								m_vectGate[idx]->AddFlight(*pfltOp);
							}

							if (pFlight->getFlight()->isDeparting())
							{
								CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, DEP_OP);
								pfltOp->SetTimeRange(tDepParking,pFlight->getEndTime());
								pFlight->SetFlightDepPart(pfltOp);
								pfltOp->SetGateIdx(nDepIdx);
								pfltOp->getFlight()->setDepStandID(nDepIdx);
								m_vectGate[nDepIdx]->AddFlight(*pfltOp);
							}


							m_vAssignedFlight.push_back(pFlight);						
							m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);	
							return true;						
						}					
					}
				}
			}
		}


	}

	pFlight->SetSelected(false);
	return false;

}

int StandAssignmentMgr::AssignFlightDepPartAccordingToFlightPrioritySequence( FlightForAssignment*pFlight,const ElapsedTime& tStart, const ElapsedTime tEnd, const FlightPriorityInfo* pFltPriorities)
{
	int nSize = pFltPriorities->m_vectFlightGate.size();
	for (int i = 0; i < nSize; i++)
	{
		
		FlightGate _priority = pFltPriorities->m_vectFlightGate.at(i);

		if (!_priority.flight.IsFit(pFlight->getFlight(), 'D'))
			continue;

		std::vector<int> vFitStands;
		GetFitStandIdxList( _priority.procID, vFitStands);

		int nIdx = tryToAssignFlightPartToAppointedStandList(pFlight,tStart,tEnd,vFitStands);

		if (nIdx > -1)
			return nIdx;
		
	}

	return -1;
}

void StandAssignmentMgr::SortUnAssignedFlight()
{
	std::sort(m_vectUnassignedFlight.begin(),m_vectUnassignedFlight.end(), FlightTimeCompare);
}

void StandAssignmentMgr::SortAssignStandListByFlightCount( std::vector<CAssignStand*>& vStands )
{
	std::sort(vStands.begin(), vStands.end(), FlightCountInStandCompare);
}

void StandAssignmentMgr::GetFitStandIdxList(const ALTObjectID& objName, std::vector<int>& vFitStands )
{
	vFitStands.clear();

	std::map< CString, std::vector<CAssignStand*> >::iterator iter = m_mapPriorityStandFamilyList.find(objName.GetIDString());
	std::vector<CAssignStand*> vStands;

	if (m_mapPriorityStandFamilyList.empty() || iter == m_mapPriorityStandFamilyList.end())
	{
		int nGateCount = GetGateCount();

		for(int j = 0; j < nGateCount; j++)
		{		
			CAssignStand* pGate = (CAssignStand*)GetGate(j);
			if(pGate->GetStandID().idFits(objName))
				vStands.push_back(pGate);	
		}
		m_mapPriorityStandFamilyList.insert(std::map<CString, std::vector<CAssignStand*> >::value_type(objName.GetIDString(), vStands));
	}
	else
		vStands = iter->second;

	SortAssignStandListByFlightCount(vStands);

	size_t nSize = vStands.size();
	for (size_t i = 0; i < nSize; i++)
	{
		vFitStands.push_back((vStands.at(i))->GetStandIdx());
	}
}

//-------------------------------------------------------------------------------------------------------------
//Summary:
//		assign flight with single flight priority
//Parameters:
//		_priority: single flight priority
//		pFltPriorities: assign dep part with all flight priority
//-------------------------------------------------------------------------------------------------------------
bool StandAssignmentMgr::AssignFlightAccordingToFlightPriority(FlightGate& _priority,const FlightPriorityInfo* pFltPriorities)
{
	CTowOffStandAssignmentDataList* pTowoffStandDataList = GetTowoffStandDataList();
	if (pTowoffStandDataList == NULL)
	{
		return false;
	}
	int nCount = (int)m_vectUnassignedFlight.size();
	CTowOffStandAssignmentData* pTowData = NULL;
	for (int i = 0; i < nCount; i++)
	{
		FlightForAssignment* pFlight = m_vectUnassignedFlight.at(i);
		pFlight->SetSelected(true);
		if (!_priority.flight.IsFit(pFlight->getFlight()))	
			continue;

		// Include arrival part                    NOT arrival -> departure			//arrival part included in priority
		if (pFlight->getFlight()->isArriving() && !_priority.flight.IsArrToDep() && !_priority.flight.IsFit(pFlight->getFlight(),'A'))	//if not arr --> dep, first find arr part priority
			continue;//this judgment seems not necessary, duplicated with the previous fit

		//to judge whether the flight is only departure or only arrival or arrival part with departure part fit same priority
		bool bArrAndDepSamePriority = false;
		if (_priority.flight.IsArrToDep() || !pFlight->getFlight()->isArriving() ||  !pFlight->getFlight()->isDeparting())//arr --> dep, ony arr, only dep
			bArrAndDepSamePriority = true;
		else
		{
			if (_priority.flight.IsFit(pFlight->getFlight(),'D'))
				bArrAndDepSamePriority = true;
		}

		std::vector<int> vPriorityStands;
		GetFitStandIdxList( _priority.procID , vPriorityStands);
		for (int nStand = 0; nStand < (int)vPriorityStands.size(); nStand++)
		{
			int idx = vPriorityStands.at(nStand);
			std::vector<int> vStands;
			vStands.push_back(idx);

			CAssignStand* pGate = (CAssignStand*)GetGate(idx);
			std::map<CString,CTowOffStandAssignmentData*> mapTowDataList;
			pTowoffStandDataList->GetFlightTowoffDataList(pFlight->getFlight(), pGate->GetStandID(), mapTowDataList);

			bool bAssign = false;
			std::map<CString,CTowOffStandAssignmentData*>::iterator iter = mapTowDataList.begin();
			for (; iter != mapTowDataList.end(); iter++)
			{
				pTowData = iter->second;
				if (pTowData == NULL)
					continue;

				if (bArrAndDepSamePriority)		
				{
					if ( pTowData->IsTow() == FALSE)		//not tow
					{
						int nStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),pFlight->getEndTime(),vStands);
						if (nStandIdx > -1)		//assign success
						{
							AssignFlightToOneAppointedStand(pFlight,i,nStandIdx);
							i--;
							nCount--;
							bAssign = true;
							break;
						}
					}
					else			//tow
					{
						if (pTowData->GetRepositionOrFreeup())		//reposition for dep stand
						{
							int nStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),pFlight->getEndTime(),vStands);
							if (nStandIdx > -1)		//assign success
							{
								AssignFlightToOneAppointedStand(pFlight,i,nStandIdx);
								i--;
								nCount--;
								bAssign = true;
								break;
							}
							else
							{
								ElapsedTime tArrParking = 0L;
								if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
									tArrParking = pFlight->getStartTime() + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
								else
									tArrParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

								if (tArrParking < pFlight->getStartTime() || tArrParking > pFlight->getEndTime())		//error
								{
									break;
								}

								int nArrStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),tArrParking,vStands);
								if (nArrStandIdx < 0)			//arrival part assign failed
									break;

								int nDepStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight, tArrParking,pFlight->getEndTime(), vStands);
								if (nDepStandIdx < 0)		//departure part assign failed
									break;

								AssignFlightToTwoAppointedStands(pFlight,i, nArrStandIdx, nDepStandIdx, tArrParking);
								i--;
								nCount--;
								bAssign = true;
								break;
							}
						}
						else			//free up stand
						{
							if (AssignFlightToAppointedStandListWithIntermediateStand(pFlight,i,vStands,pTowData))
							{	
								i--;
								nCount--;
								bAssign = true;
								break;
							}
						}
					}
				}
				else			// not arr --> dep, and arr priority different with dep priority
				{
					if(pTowData->IsTow() == TRUE && pTowData->GetRepositionOrFreeup() == FALSE)		// free up stand
					{
						ElapsedTime tArrParking = pFlight->getStartTime()+ ElapsedTime(pTowData->GetDeplanementTime()*60L);

						int nArrStandIdx = -1;
						if (pFlight->getFlight()->isArriving())
						{
							nArrStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),tArrParking,vStands);
							if (nArrStandIdx < 0)			//arrival part assign failed
								break;
						}

						int nDepStandIdx = -1;
						ElapsedTime tDepParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetEnplanementTime()*60L);
						bool bDepDiffernt = pTowData->GetBackToArrivalStand() == TRUE? false: true;

						if (bDepDiffernt)		//arrival stand different with departure stand
							nDepStandIdx = AssignFlightDepPartAccordingToFlightPrioritySequence(pFlight,tDepParking,pFlight->getEndTime(),pFltPriorities);
						else
						{
							if (IsFlightPartFitInGate(nArrStandIdx, pFlight->getFlight(),tDepParking,pFlight->getEndTime()))
								nDepStandIdx = nArrStandIdx;
						}

						if (nDepStandIdx > -1)
						{
							if (tryToAssignIntermediateParkingStand(pFlight,pTowData))
							{
								if (pFlight->getFlight()->isArriving())
								{
									CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, ARR_OP);
									pfltOp->SetTimeRange(pFlight->getStartTime(),tArrParking);
									pFlight->SetFlightArrPart(pfltOp);
									pfltOp->SetGateIdx(nArrStandIdx);
									pfltOp->getFlight()->setArrStandID(nArrStandIdx);
									m_vectGate[nArrStandIdx]->AddFlight(*pfltOp);
								}

								if (pFlight->getFlight()->isDeparting())
								{
									CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, DEP_OP);
									pfltOp->SetTimeRange(tDepParking,pFlight->getEndTime());
									pFlight->SetFlightDepPart(pfltOp);
									pfltOp->SetGateIdx(nDepStandIdx);
									pfltOp->getFlight()->setDepStandID(nDepStandIdx);
									m_vectGate[nDepStandIdx]->AddFlight(*pfltOp);
								}


								m_vAssignedFlight.push_back(pFlight);						
								m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+i);	
								i--;
								nCount--;
								bAssign = true;
								break;						
							}					
						}

					}
					else			//not tow or reposition for dep
					{
						ElapsedTime tArrParking = -1L;
						if (pTowData->IsTow() == TRUE)		//reposition for dep
						{
							if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
								tArrParking = pFlight->getStartTime() + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
							else
								tArrParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

							if (tArrParking < pFlight->getStartTime() || tArrParking > pFlight->getEndTime())		//error
							{
								break;
							}
						}
						else				//not tow
							tArrParking = pFlight->getStartTime() + (pFlight->getEndTime() - pFlight->getStartTime())/2L;

						int nArrStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),tArrParking,vStands);
						if (nArrStandIdx < 0)			//arrival part assign failed
							break;

						int nDepStandIdx = AssignFlightDepPartAccordingToFlightPrioritySequence(pFlight,tArrParking,pFlight->getEndTime(),pFltPriorities);
						if (nDepStandIdx < 0)
							break;

						AssignFlightToTwoAppointedStands(pFlight,i, nArrStandIdx, nDepStandIdx, tArrParking);
						i--;
						nCount--;
						bAssign = true;
						break;

					}
				}
			}

			if (bAssign)
				break;
		}
		pFlight->SetSelected(false);
	}
	return true;
}

bool StandAssignmentMgr::AssignFlightAccordingToFlightPrioritySequence( int nFltIdx, const FlightPriorityInfo* pFltPriorities)
{
	CTowOffStandAssignmentDataList* pTowoffStandDataList = GetTowoffStandDataList();
	if (pTowoffStandDataList == NULL)
		return false;
	
	FlightForAssignment* pFlight = m_vectUnassignedFlight.at(nFltIdx);
	pFlight->SetSelected(true);


	CTowOffStandAssignmentData* pTowData = NULL;
	int nPriorityCount = pFltPriorities->m_vectFlightGate.size();

	for (int m =0; m < nPriorityCount; m++)
	{
		FlightGate _priority = pFltPriorities->m_vectFlightGate.at(m) ;

		if (!_priority.flight.IsFit(pFlight->getFlight()))	
			continue;

		//   arrival part						//NOT  arrival->departure           //NOT fit arrival part
		if (pFlight->getFlight()->isArriving() && !_priority.flight.IsArrToDep() && !_priority.flight.IsFit(pFlight->getFlight(),'A'))	//if not arr --> dep, first find arr part priority
			continue;

		//to judge whether the flight is only departure or only arrival or arrival part with departure part fit same priority
		bool bArrAndDepSamePriority = false;
		if (_priority.flight.IsArrToDep() || !pFlight->getFlight()->isArriving() ||  !pFlight->getFlight()->isDeparting())//arr --> dep, ony arr, only dep
		{
			bArrAndDepSamePriority = true;
		}
		else
		{
			if (_priority.flight.IsFit(pFlight->getFlight(),'D'))
				bArrAndDepSamePriority = true;
		}

		std::vector<int> vPriorityStands;
		GetFitStandIdxList( _priority.procID , vPriorityStands);
		for (int nStand = 0; nStand < (int)vPriorityStands.size(); nStand++)
		{
			int idx = vPriorityStands.at(nStand);
			std::vector<int> vStands;
			vStands.push_back(idx);

			CAssignStand* pGate = (CAssignStand*)GetGate(idx);
			std::map<CString,CTowOffStandAssignmentData*> mapTowDataList;
			pTowoffStandDataList->GetFlightTowoffDataList(pFlight->getFlight(), pGate->GetStandID(), mapTowDataList);

			std::map<CString,CTowOffStandAssignmentData*>::iterator iter = mapTowDataList.begin();
			for (; iter != mapTowDataList.end(); iter++)
			{
				pTowData = iter->second;
				if (pTowData == NULL)
					continue;

				if (bArrAndDepSamePriority)		
				{
					if ( pTowData->IsTow() == FALSE)		//not tow
					{
						int nStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),pFlight->getEndTime(),vStands);
						if (nStandIdx > -1)		//assign success
						{
							AssignFlightToOneAppointedStand(pFlight,nFltIdx,nStandIdx);
							return true;
						}
					}
					else			//tow
					{
						if (pTowData->GetRepositionOrFreeup())		//reposition for dep stand
						{
							int nStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),pFlight->getEndTime(),vStands);
							if (nStandIdx > -1)		//assign success
							{
								AssignFlightToOneAppointedStand(pFlight,nFltIdx,nStandIdx);
								return true;
							}
							else
							{
								ElapsedTime tArrParking = 0L;
								if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
									tArrParking = pFlight->getStartTime() + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
								else
									tArrParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

								if (tArrParking < pFlight->getStartTime() || tArrParking > pFlight->getEndTime())		//error
								{
									continue;
								}

								int nArrStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),tArrParking,vStands);
								if (nArrStandIdx < 0)			//arrival part assign failed
									continue;

								int nDepStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight, tArrParking,pFlight->getEndTime(), vStands);
								if (nDepStandIdx < 0)		//departure part assign failed
									continue;

								AssignFlightToTwoAppointedStands(pFlight,nFltIdx, nArrStandIdx, nDepStandIdx, tArrParking);
								return true;
							}
						}
						else			//free up stand
						{
							if (AssignFlightToAppointedStandListWithIntermediateStand(pFlight,nFltIdx,vStands,pTowData))
								return true;
						}
					}
				}
				else			// not arr --> dep, and arr priority different with dep priority
				{
					if(pTowData->IsTow() == TRUE && pTowData->GetRepositionOrFreeup() == FALSE)		// free up stand
					{
						ElapsedTime tArrParking = pFlight->getStartTime()+ ElapsedTime(pTowData->GetDeplanementTime()*60L);

						int nArrStandIdx = -1;
						if (pFlight->getFlight()->isArriving())
						{
							nArrStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),tArrParking,vStands);
							if (nArrStandIdx < 0)			//arrival part assign failed
								continue;
						}

						int nDepStandIdx = -1;
						ElapsedTime tDepParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetEnplanementTime()*60L);
						bool bDepDiffernt = pTowData->GetBackToArrivalStand() == TRUE? false: true;

						if (bDepDiffernt)		//arrival stand different with departure stand
							nDepStandIdx = AssignFlightDepPartAccordingToFlightPrioritySequence(pFlight,tDepParking,pFlight->getEndTime(),pFltPriorities);
						else
						{
							if (IsFlightPartFitInGate(nArrStandIdx, pFlight->getFlight(),tDepParking,pFlight->getEndTime()))
								nDepStandIdx = nArrStandIdx;
						}

						if (nDepStandIdx > -1)
						{
							if (tryToAssignIntermediateParkingStand(pFlight,pTowData))
							{
								if (pFlight->getFlight()->isArriving())
								{
									CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, ARR_OP);
									pfltOp->SetTimeRange(pFlight->getStartTime(),tArrParking);
									pFlight->SetFlightArrPart(pfltOp);
									pfltOp->SetGateIdx(nArrStandIdx);
									pfltOp->getFlight()->setArrStandID(nArrStandIdx);
									m_vectGate[nArrStandIdx]->AddFlight(*pfltOp);
								}

								if (pFlight->getFlight()->isDeparting())
								{
									CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, DEP_OP);
									pfltOp->SetTimeRange(tDepParking,pFlight->getEndTime());
									pFlight->SetFlightDepPart(pfltOp);
									pfltOp->SetGateIdx(nDepStandIdx);
									pfltOp->getFlight()->setDepStandID(nDepStandIdx);
									m_vectGate[nDepStandIdx]->AddFlight(*pfltOp);
								}


								m_vAssignedFlight.push_back(pFlight);						
								m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);	
								return true;						
							}					
						}

					}
					else			//not tow or reposition for dep
					{
						ElapsedTime tArrParking = -1L;
						if (pTowData->IsTow() == TRUE)		//reposition for dep
						{
							if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
								tArrParking = pFlight->getStartTime() + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
							else
								tArrParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

							if (tArrParking < pFlight->getStartTime() || tArrParking > pFlight->getEndTime())		//error
							{
								continue;
							}
						}
						else				//not tow
							tArrParking = pFlight->getStartTime() + (pFlight->getEndTime() - pFlight->getStartTime())/2L;

						int nArrStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),tArrParking,vStands);
						if (nArrStandIdx < 0)			//arrival part assign failed
							continue;

						int nDepStandIdx = AssignFlightDepPartAccordingToFlightPrioritySequence(pFlight,tArrParking,pFlight->getEndTime(),pFltPriorities);
						if (nDepStandIdx < 0)
							continue;

						AssignFlightToTwoAppointedStands(pFlight,nFltIdx, nArrStandIdx, nDepStandIdx, tArrParking);
						return true;

					}
				}
			}

// 			if (!vPriorityStands.empty())		//left priority stand assign
// 			{
// 				if (bArrAndDepSamePriority)		
// 				{
// 					int nStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),pFlight->getEndTime(),vStands);
// 					if (nStandIdx <0)		//assign failed
// 						continue;
// 
// 					AssignFlightToOneAppointedStand(pFlight,nFltIdx,nStandIdx);
// 					return true;
// 
// 				}
// 				else
// 				{
// 					ElapsedTime tArrParking = pFlight->getStartTime() + (pFlight->getEndTime() - pFlight->getStartTime())/2L;
// 					int nArrStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),tArrParking,vStands);
// 					if (nArrStandIdx < 0)			//arrival part assign failed
// 						continue;
// 
// 					int nDepStandIdx = AssignFlightDepPartAccordingToFlightPrioritySequence(pFlight,tArrParking,pFlight->getEndTime(),pFltPriorities);
// 					if (nDepStandIdx < 0)
// 						continue;
// 
// 					AssignFlightToTwoAppointedStands(pFlight,nFltIdx, nArrStandIdx, nDepStandIdx, tArrParking);
// 					return true;
// 				}
// 			}
		}
	}
	pFlight->SetSelected(false);
	return false;
}

		//int nListCount = vFitStands.size();

		//for(int i = 0; i < nListCount; i++)
		//{
		//	int idx = vFitStands.at(i);

		//	CAssignStand* pGate = (CAssignStand*)GetGate(idx);

		//	pTowData = m_pTowOffStandAssignmentList->GetFlightTowoffData(pFlight->getFlight(),pGate->GetStandID());
		//	if (pTowData == NULL || (pTowData && pTowData->IsTow() == FALSE))	//not tow off
		//	{
		//		if (IsAWholeFlightFitInGate(idx,pFlight))
		//		{
		//			OpType _optype = ARR_DEP_OP;
		//			if (!pFlight->getFlight()->isArriving())
		//				_optype = DEP_OP;
		//			if (!pFlight->getFlight()->isDeparting())
		//				_optype = ARR_OP;

		//			CFlightOperationForGateAssign* pFltOp = new CFlightOperationForGateAssign(pFlight,_optype);
		//			pFltOp->SetTimeRange(pFlight->getStartTime(),pFlight->getEndTime());
		//			pFltOp->SetGateIdx(idx);
		//			pFlight->SetFlightIntPart(pFltOp);
		//			m_vectGate[idx]->AddFlight(*pFltOp);
		//			pFlight->getFlight()->setStandID(idx);

		//			m_vAssignedFlight.push_back(pFlight);						
		//			m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);			
		//			return true;
		//		}
		//	}
		//	else
		//	{
		//		if (pTowData->GetRepositionOrFreeup())		//reposition for dep stand
		//		{
		//			ElapsedTime tArrParking = 0L;
		//			if (pTowData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
		//				tArrParking = pFlight->getStartTime() + ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);
		//			else
		//				tArrParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetTimeLeaveArrStand()*60L);

		//			if (IsFlightPartFitInGate(idx,pFlight->getFlight(),pFlight->getStartTime(),tArrParking))
		//			{

		//				int nDepIdx = AssignFlightDepPartAccordingToFlightPrioritySequence(pFlight,tArrParking, pFlight->getEndTime(),vFltPriorities);

		//				if (nDepIdx > -1)	//assigned
		//				{
		//					CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, ARR_OP);
		//					pfltOp->SetTimeRange(pFlight->getStartTime(),tArrParking);
		//					pFlight->SetFlightArrPart(pfltOp);
		//					pfltOp->SetGateIdx(idx);
		//					pfltOp->getFlight()->setArrStandID(idx);
		//					m_vectGate[idx]->AddFlight(*pfltOp);

		//					pfltOp = new CFlightOperationForGateAssign(pFlight, DEP_OP);
		//					pfltOp->SetTimeRange(tArrParking,pFlight->getEndTime());
		//					pFlight->SetFlightDepPart(pfltOp);
		//					pfltOp->SetGateIdx(nDepIdx);
		//					pfltOp->getFlight()->setDepStandID(nDepIdx);
		//					m_vectGate[nDepIdx]->AddFlight(*pfltOp);

		//					m_vAssignedFlight.push_back(pFlight);						
		//					m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);	
		//					return true;
		//				}
		//			}
		//		}
		//		else		//intermediate stand parking
		//		{

		//		}
		//	}
		//}

void StandAssignmentMgr::AssignFlightToOneAppointedStand( FlightForAssignment* pFlight, int nFltIdx, int nStandIdx )
{
	OpType _optype = ARR_DEP_OP;
	if (!pFlight->getFlight()->isArriving())
		_optype = DEP_OP;
	if (!pFlight->getFlight()->isDeparting())
		_optype = ARR_OP;

	CFlightOperationForGateAssign* pFltOp = new CFlightOperationForGateAssign(pFlight,_optype);
	pFltOp->SetTimeRange(pFlight->getStartTime(),pFlight->getEndTime());
	pFltOp->SetGateIdx(nStandIdx);
	pFlight->SetFlightIntPart(pFltOp);
	m_vectGate[nStandIdx]->AddFlight(*pFltOp);
	pFlight->getFlight()->setStandID(nStandIdx);

	m_vAssignedFlight.push_back(pFlight);						
	m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);			
}

void StandAssignmentMgr::AssignFlightToTwoAppointedStands( FlightForAssignment* pFlight, int nFltIdx, int nArrStandIdx, int nDepStandIdx,const ElapsedTime& tArrParking )
{

	if (nArrStandIdx == nDepStandIdx)		//same stand
	{
		CFlightOperationForGateAssign* pFltOp = new CFlightOperationForGateAssign(pFlight,ARR_DEP_OP);
		pFltOp->SetTimeRange(pFlight->getStartTime(),pFlight->getEndTime());
		pFltOp->SetGateIdx(nArrStandIdx);
		pFlight->SetFlightIntPart(pFltOp);
		m_vectGate[nArrStandIdx]->AddFlight(*pFltOp);
		pFlight->getFlight()->setStandID(nArrStandIdx);
	}
	else		//different stand
	{
		CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, ARR_OP);
		pfltOp->SetTimeRange(pFlight->getStartTime(),tArrParking);
		pFlight->SetFlightArrPart(pfltOp);
		pfltOp->SetGateIdx(nArrStandIdx);
		pfltOp->getFlight()->setArrStandID(nArrStandIdx);
		m_vectGate[nArrStandIdx]->AddFlight(*pfltOp);

		pfltOp = new CFlightOperationForGateAssign(pFlight, DEP_OP);
		pfltOp->SetTimeRange(tArrParking,pFlight->getEndTime());
		pFlight->SetFlightDepPart(pfltOp);
		pfltOp->SetGateIdx(nDepStandIdx);
		pfltOp->getFlight()->setDepStandID(nDepStandIdx);
		m_vectGate[nDepStandIdx]->AddFlight(*pfltOp);
	}

	m_vAssignedFlight.push_back(pFlight);						
	m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);	
}

bool StandAssignmentMgr::AssignFlightToAppointedStandListWithIntermediateStand( FlightForAssignment* pFlight, int nFltIdx, std::vector<int> vStandIdxList, CTowOffStandAssignmentData* pTowData )
{
	ElapsedTime tArrParking = pFlight->getStartTime()+ ElapsedTime(pTowData->GetDeplanementTime()*60L);
	int nArrStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,pFlight->getStartTime(),tArrParking,vStandIdxList);
	if (nArrStandIdx < 0)			//arrival part assign failed
		return false;

	int nDepStandIdx = -1;	
	ElapsedTime tDepParking = pFlight->getEndTime() - ElapsedTime(pTowData->GetEnplanementTime()*60L);
	if (IsFlightPartFitInGate(nArrStandIdx, pFlight->getFlight(),tDepParking,pFlight->getEndTime()))
		nDepStandIdx = nArrStandIdx;

	bool bDepDiffernt = pTowData->GetBackToArrivalStand() == TRUE? false: true;

	if (bDepDiffernt && nDepStandIdx < 0)		//arrival stand different with departure stand
		nDepStandIdx = tryToAssignFlightPartToAppointedStandList(pFlight,tArrParking, pFlight->getEndTime(), vStandIdxList);

	if (nDepStandIdx > -1)
	{
		if (tryToAssignIntermediateParkingStand(pFlight,pTowData))
		{
			if (pFlight->getFlight()->isArriving())
			{
				CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, ARR_OP);
				pfltOp->SetTimeRange(pFlight->getStartTime(),tArrParking);
				pFlight->SetFlightArrPart(pfltOp);
				pfltOp->SetGateIdx(nArrStandIdx);
				pfltOp->getFlight()->setArrStandID(nArrStandIdx);
				m_vectGate[nArrStandIdx]->AddFlight(*pfltOp);
			}

			if (pFlight->getFlight()->isDeparting())
			{
				CFlightOperationForGateAssign* pfltOp = new CFlightOperationForGateAssign(pFlight, DEP_OP);
				pfltOp->SetTimeRange(tDepParking,pFlight->getEndTime());
				pFlight->SetFlightDepPart(pfltOp);
				pfltOp->SetGateIdx(nDepStandIdx);
				pfltOp->getFlight()->setDepStandID(nDepStandIdx);
				m_vectGate[nDepStandIdx]->AddFlight(*pfltOp);
			}


			m_vAssignedFlight.push_back(pFlight);						
			m_vectUnassignedFlight.erase(m_vectUnassignedFlight.begin()+nFltIdx);	
			return true;						
		}					
	}
	return false;
}


void FlightPartAccordingToTowData::GetFlightTimePartAccordingToTowData( FlightForAssignment* pFlight, CTowOffStandAssignmentData* pFitData )
{
	if (pFitData == NULL || (pFitData && !pFitData->IsTow()))	//not tow
	{
		if (!pFlight->getFlight()->isArriving())		//only dep
		{
			m_DepPart.tStart = pFlight->getStartTime();
			m_DepPart.tEnd = pFlight->getEndTime();

			m_ArrPart.tStart = -1L;
			m_ArrPart.tEnd = -1L;
		}
		else
		{
			if (!pFlight->getFlight()->isDeparting() || pFlight->getFlight()->getArrStand() == pFlight->getFlight()->getDepStand())	//only arrival or arr stand same as dep stand
			{
				m_ArrPart.tStart = pFlight->getStartTime();
				m_ArrPart.tEnd = pFlight->getEndTime();

				m_DepPart.tStart = -1L;
				m_DepPart.tEnd = -1L;
			}
			else
			{
				m_ArrPart.tStart = pFlight->getStartTime();
				m_ArrPart.tEnd = pFlight->getStartTime() + (pFlight->getEndTime() - pFlight->getStartTime())/2L;

				m_DepPart.tStart = m_ArrPart.tEnd;
				m_DepPart.tEnd = pFlight->getEndTime();
			}

		}

		m_IntPart.tStart = -1L;
		m_IntPart.tEnd = -1L;
		return ;
	}

	if (pFitData->GetRepositionOrFreeup())
	{
		if (pFlight->getFlight()->getArrStand() == pFlight->getFlight()->getDepStand())
		{
			m_ArrPart.tStart = pFlight->getStartTime();
			m_ArrPart.tEnd = pFlight->getEndTime();

			m_DepPart.tStart = -1L;
			m_DepPart.tEnd = -1L;
		}
		else
		{
			m_ArrPart.tStart = pFlight->getStartTime();

			if (pFitData->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
				m_ArrPart.tEnd = pFlight->getStartTime() + ElapsedTime(pFitData->GetTimeLeaveArrStand()*60L);
			else
				m_ArrPart.tEnd = pFlight->getEndTime() - ElapsedTime(pFitData->GetTimeLeaveArrStand()*60L);

			m_DepPart.tStart = m_ArrPart.tEnd;
			m_DepPart.tEnd = pFlight->getEndTime();

		}

		m_IntPart.tStart = -1L;
		m_IntPart.tEnd = -1L;
	}
	else
	{

		m_ArrPart.tStart = pFlight->getStartTime();
		m_ArrPart.tEnd = pFlight->getStartTime() + ElapsedTime(pFitData->GetDeplanementTime()*60L);

		m_DepPart.tStart = pFlight->getEndTime() - ElapsedTime(pFitData->GetEnplanementTime()*60L);
		m_DepPart.tEnd = pFlight->getEndTime();

		m_IntPart.tStart = m_ArrPart.tEnd;
		m_IntPart.tEnd = m_DepPart.tStart;

		if (!pFlight->getFlight()->isArriving())
		{
			m_ArrPart.tStart = -1L;
			m_ArrPart.tEnd = -1L;
		}

		if (!pFlight->getFlight()->isDeparting())
		{
			m_DepPart.tStart = -1L;
			m_DepPart.tEnd = -1L;
		}

	}
		
}