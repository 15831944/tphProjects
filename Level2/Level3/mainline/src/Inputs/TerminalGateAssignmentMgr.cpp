#include "StdAfx.h"
#include ".\terminalgateassignmentmgr.h"
#include "InputAirside\TowOffStandAssignmentData.h"
#include "Flight.h"
#include "schedule.h"
//#include "engine\terminal.h"
#include "engine\proclist.h"
#include "engine\gate.h"
#include "Engine\StandProc.h"
#include "AirsideInput.h"
#include "../InputAirside/InputAirside.h"
#include <CommonData/procid.h>
#include "FlightForGateAssign.h"
#include "in_term.h"

TerminalGateAssignmentMgr::TerminalGateAssignmentMgr(int nProjID)
: CGateAssignmentMgr(nProjID)
,m_std2gateConstraint(nProjID)
{
    m_std2gateConstraint.setGateAssignManager(this) ;
}

TerminalGateAssignmentMgr::~TerminalGateAssignmentMgr(void)
{
}

void TerminalGateAssignmentMgr::ResetGateContent(OpType eType, InputTerminal* _pInTerm)
{
	m_vectGate.clear();
	m_vectUnassignedFlight.clear();

	ProcessorArray processArray;
	_pInTerm->procList->getProcessorsOfType( GateProc, processArray );

	GateType gateType;
	if (eType == ARR_OP)
		gateType = ArrGate;
	if (eType == DEP_OP)
		gateType = DepGate;

	int nGateCount = processArray.getCount();
	for( int i = 0; i < nGateCount; i++ )
	{
		GateProcessor* pGate = (GateProcessor*)processArray[i];
		if( !pGate || pGate->getGateType() != gateType)
			continue;

		CAssignTerminalGate* pAssignGate = new CAssignTerminalGate();
		if(!pAssignGate)continue;
		pAssignGate->SetParent(this);
		pAssignGate->SetProcessorID( *(pGate->getID()));
		m_vectGate.push_back( pAssignGate );
	}

	//Stand Need to be Added
	ProcessorArray procArray;
	_pInTerm->GetAirsideInput()->GetProcessorList()->getProcessorsOfType( StandProcessor, procArray);
	int nStandCount = procArray.getCount();
	for( int i = 0; i < nStandCount; i++ )
	{
		StandProc* pGate = (StandProc*)procArray[i];
		if( !pGate)continue;

		CAssignTerminalGate* pAssignGate = new CAssignTerminalGate();
		if(!pAssignGate)continue;
		pAssignGate->SetParent(this);
		pAssignGate->SetProcessorID( *(pGate->getID()) );
		m_vectGate.push_back( pAssignGate );
	}

	nGateCount =(int)m_vectGate.size();

	sortAllGateByName();

	ALTObjectID blankStandID;
	ProcessorID blankGateID;

	ElapsedTime tEnplane = -1L;
	ElapsedTime	tDeplane = -1L;

	int nFlightCount = _pInTerm->flightSchedule->getCount();
	for( int i=0; i<nFlightCount; i++ )
	{
		Flight* pFlight = _pInTerm->flightSchedule->getItem(i);
		//TRACE("%s %s\n",(pFlight->getArrTime()).printTime(),(pFlight->getDepTime()).printTime());

		tEnplane = pFlight->getEnplaneTime();
		tDeplane = pFlight->getDeplaneTime();

		//TRACE("%s %s %s\n",tMaxParking.printTime(), tEnplane.printTime(), tDeplane.printTime());

		if (pFlight->isTurnaround() && tEnplane > pFlight->getDepTime() - pFlight->getArrTime() )
			tEnplane = pFlight->getDepTime() - pFlight->getArrTime();
		if (pFlight->isTurnaround() && tDeplane > pFlight->getDepTime() - pFlight->getArrTime())
			tDeplane = pFlight->getDepTime() - pFlight->getArrTime();

		//TRACE("%s %s \n",tEnplane.printTime(), tDeplane.printTime());

		FlightForAssignment* pFlightInGate = new FlightForAssignment(pFlight);

		if (eType == ARR_OP && pFlight->isArriving()) 
		{
			CFlightOperationForGateAssign* aFlight = new CFlightOperationForGateAssign(pFlightInGate,ARR_OP);

			{
				aFlight->SetTimeRange(pFlight->getArrTime(),pFlight->getArrTime()+ tDeplane);
				//TRACE("%s %s\n",aFlight->GetStartTime().printTime(), aFlight->GetEndTime().printTime());

				pFlightInGate->SetFlightArrPart(aFlight);
			}

			if( pFlight->getArrGate().GetIDString() == blankGateID.GetIDString())
			{
				m_vectUnassignedFlight.push_back( pFlightInGate );
			}
			else
			{
				int j = 0;
				for( ; j < (nGateCount); j++ )
				{
					if( pFlight->getArrGate().GetIDString() == ((CAssignTerminalGate *)&*m_vectGate[j])->GetProcessID().GetIDString() )
					{
						break;
					}
				}

				if( j < (nGateCount) )
				{
					m_vAssignedFlight.push_back(pFlightInGate);
					aFlight->SetGateIdx(j);
					m_vectGate[j]->AddFlight( *aFlight);

				}
				else
				{
					// the gate proc id is invalide
					aFlight->EmptyGate();
					m_vectUnassignedFlight.push_back( pFlightInGate );
				}
			}
		
		}
		else if (eType == DEP_OP && pFlight->isDeparting()) 
		{

			CFlightOperationForGateAssign* aFlight = new CFlightOperationForGateAssign(pFlightInGate,DEP_OP );
			pFlightInGate->SetFlightDepPart(aFlight);

			{
				aFlight->SetTimeRange(pFlight->getDepTime() - tEnplane,pFlight->getDepTime());
				//TRACE("%s %s\n",aFlight->GetStartTime().printTime(), aFlight->GetEndTime().printTime());
			}
			if( pFlight->getDepGate().GetIDString() == blankGateID.GetIDString())
			{
				m_vectUnassignedFlight.push_back( pFlightInGate );
			}
			else
			{
				int j = 0;
				for(  ; j < (nGateCount); j++ )
				{
					if( pFlight->getDepGate().GetIDString() == ((CAssignTerminalGate *)&*m_vectGate[j])->GetProcessID().GetIDString() )
					{
						break;
					}
				}
				if( j < (nGateCount) )
				{
					aFlight->SetGateIdx(j);
					m_vectGate[j]->AddFlight( *aFlight);

					m_vAssignedFlight.push_back(pFlightInGate);
				}
				else
				{
					// the gate proc id is invalide
					aFlight->EmptyGate();
					m_vectUnassignedFlight.push_back( pFlightInGate );
				}
			}
			
		}
	}
}

void TerminalGateAssignmentMgr::RemoveSelectedFlight()
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

		m_vectUnassignedFlight.push_back(pFlight);
	}


}

void TerminalGateAssignmentMgr::Save( InputTerminal* _pInTerm, const CString& _csProjPath )
{
	int nUnAssignFlight = m_vectUnassignedFlight.size();
	for (int j =0; j < nUnAssignFlight; j++)
	{
		m_vectUnassignedFlight[j]->EmptyGate();
	}
	CGateAssignmentMgr::Save(_pInTerm,_csProjPath);
}

void TerminalGateAssignmentMgr::RemoveAssignedFlight(FlightForAssignment* pFlight)
{
	CFlightOperationForGateAssign* _flight = pFlight->GetFlightArrPart();
	if (_flight)
	{
		int nGateIdx = _flight->GetGateIdx();
		if (nGateIdx > -1)
		{
			m_vectGate[nGateIdx]->RemoveAssignedFlight(*_flight);
		}
	}

	_flight = pFlight->GetFlightDepPart();
	if (_flight)
	{
		int nGateIdx = _flight->GetGateIdx();
		if (nGateIdx > -1)
		{
			m_vectGate[nGateIdx]->RemoveAssignedFlight(*_flight);
		}	
	}

	std::vector<FlightForAssignment*>::iterator iter = std::find(m_vAssignedFlight.begin(),m_vAssignedFlight.end(),pFlight);
	if (iter != m_vAssignedFlight.end())
	{
		m_vAssignedFlight.erase(iter);
	}

	iter = std::find(m_vectUnassignedFlight.begin(),m_vectUnassignedFlight.end(),pFlight);
	if (iter == m_vectUnassignedFlight.end())
		m_vectUnassignedFlight.push_back(pFlight);
}

bool TerminalGateAssignmentMgr::IsFlightOperationFitInGate( int _nGateIdx, CFlightOperationForGateAssign* pFlight )
{
	if( !pFlight->IsSelected() )
		return true;

	if (!m_vectGate[_nGateIdx]->BeAbletoTake(pFlight->getFlight(),pFlight->GetStartTime(),pFlight->GetEndTime(), NULL))
		return false;

	return true;
}

int TerminalGateAssignmentMgr::AssignSelectedFlightToGate(int _nGateIdx, std::vector<int>& vGateIdx)
{
	int nFailedCount = 0;
	int nFlightCount = m_vectUnassignedFlight.size();
	for( int i=nFlightCount-1; i>=0; i-- )
	{
		FlightForAssignment* flight = m_vectUnassignedFlight[i];
		bool bMatch = true;
		if( flight->IsSelected())
		{

			CFlightOperationForGateAssign* pArrOp = flight->GetFlightArrPart();
			if (pArrOp && (!IsFlightOperationFitInGate( _nGateIdx, pArrOp)))
			{
				bMatch = false;
				nFailedCount++;
				continue;
			}

			CFlightOperationForGateAssign* pDepOp = flight->GetFlightDepPart();
			if (pDepOp && (!IsFlightOperationFitInGate( _nGateIdx, pDepOp)))
			{
				bMatch = false;
				nFailedCount++;
				continue;
			}			

			m_vAssignedFlight.push_back(flight);
			m_vectUnassignedFlight.erase( m_vectUnassignedFlight.begin() + i );

			if (pArrOp)
			{
				pArrOp->SetGateIdx(_nGateIdx);
				m_vectGate[_nGateIdx]->AddFlight(*pArrOp);
			}

			if (pDepOp)
			{
				pDepOp->SetGateIdx(_nGateIdx);
				m_vectGate[_nGateIdx]->AddFlight(*pDepOp);
			}

			vGateIdx.push_back(_nGateIdx);

		}
	}

	return nFailedCount;
}

void TerminalGateAssignmentMgr::LoadData( InputTerminal* _pInTerm, const PROJECTINFO& ProjInfo )
{
	m_std2gateConstraint.SetInputTerminal(_pInTerm);
	m_std2gateConstraint.loadDataSet(ProjInfo.path);
}