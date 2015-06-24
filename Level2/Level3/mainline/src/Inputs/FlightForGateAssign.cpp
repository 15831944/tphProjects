#include "StdAfx.h"
#include ".\flightforgateassign.h"
#include <CommonData/procid.h>
#include "../Common/ALTObjectID.h"
#include "../Common/ARCFlight.h"
#include "flight.h"
//////////////////////////////////////////////////////////////////////
// CFlightOperationForGateAssign Class
//////////////////////////////////////////////////////////////////////
CFlightOperationForGateAssign::~CFlightOperationForGateAssign()
{

}

CFlightOperationForGateAssign::CFlightOperationForGateAssign(FlightForAssignment* _flight, OpType eType)
{
	m_bSelected = false;
	m_pFlight = _flight;
	m_eOpType = eType;
	m_tStartTime = m_tEndTime = -1L;
	m_nGateIdx = -1;
}


void CFlightOperationForGateAssign::SetSelected(bool _bSelected)
{
	m_bSelected = _bSelected;
}

bool CFlightOperationForGateAssign::IsSelected()
{
	return m_bSelected;
}

bool CFlightOperationForGateAssign::operator < (const CFlightOperationForGateAssign& aFlight) const
{
	if(m_tStartTime < aFlight.m_tStartTime)
		return true;
	else 
		return false;
}

bool CFlightOperationForGateAssign::operator > (const CFlightOperationForGateAssign& aFlight) const
{
	if(m_tStartTime > aFlight.m_tStartTime)
		return true;
	else 
		return false;
}

bool CFlightOperationForGateAssign::operator == (const CFlightOperationForGateAssign& aFlight) const
{
	if (m_pFlight == aFlight.m_pFlight && m_eOpType == aFlight.m_eOpType)
	{
		return m_tStartTime == aFlight.m_tStartTime && m_tEndTime == aFlight.m_tEndTime;
	}
	return false;
}

bool CFlightOperationForGateAssign::operator != (const CFlightOperationForGateAssign& aFlight) const
{
	if (*this == aFlight)
		return false;

	return true;
}

ARCFlight* CFlightOperationForGateAssign::getFlight()
{
	return m_pFlight->getFlight();
}

OpType CFlightOperationForGateAssign::getOpType()
{
	return m_eOpType;
}

ElapsedTime CFlightOperationForGateAssign::GetStartTime()
{
	return m_tStartTime;
}

ElapsedTime CFlightOperationForGateAssign::GetEndTime()
{
	return m_tEndTime;
}

void CFlightOperationForGateAssign::SetTimeRange(ElapsedTime tStart, ElapsedTime tEnd)
{
	m_tStartTime = tStart;
	m_tEndTime = tEnd;
}

void CFlightOperationForGateAssign::EmptyGate()
{
	if(getFlight()->getFlightType() == TerminalFlight)
	{
		ProcessorID gateID;
		if (m_eOpType == ARR_DEP_OP)
		{
			((Flight*)getFlight())->setArrGate(gateID);
			((Flight*)getFlight())->setDepGate(gateID);
		}
		if (m_eOpType == ARR_OP)
		{
			((Flight*)getFlight())->setArrGate(gateID);
		}
		if (m_eOpType == DEP_OP)
		{
			((Flight*)getFlight())->setDepGate(gateID);
		}
		m_nGateIdx = -1;
	}
}

void CFlightOperationForGateAssign::EmptyStand()
{
	ALTObjectID standID;
	if (m_eOpType == ARR_DEP_OP)
	{
		getFlight()->setArrStand(standID);
		getFlight()->setDepStand(standID);
	}
	if (m_eOpType == ARR_OP)
	{
		getFlight()->setArrStand(standID);
	}
	if (m_eOpType == DEP_OP)
	{
		getFlight()->setDepStand(standID);
	}
	if (m_eOpType == INTPAKRING_OP)
	{
		getFlight()->setIntermediateStand(standID);
	}
	m_nGateIdx = -1;
}
bool CFlightOperationForGateAssign::FlightAssignToStandValid( CString& strError )
{
	if (m_vErrorMessageList.empty())
		return true;

	for (unsigned i = 0; i < m_vErrorMessageList.size(); i++)
	{
		FlightAssignToStandErrorMessage errorMessage = m_vErrorMessageList.at(i);
		if (strError.IsEmpty())
		{
			strError = CString(_T("Stand Allocation Error Message: \n"));
			strError += CString(_T("1. "))+ errorMessage.m_strError;
		}
		else
		{
			CString strIndex;
			strIndex.Format(_T("%d. "),i+1);
			strError += CString(_T("\n")) + strIndex + errorMessage.m_strError; 
		}
	}
	return false;
}

bool CFlightOperationForGateAssign::FlightAssignToStandValid() const
{
	if (m_vErrorMessageList.empty())
		return true;

	return false;
}
void CFlightOperationForGateAssign::AddErrorMessage( StandAssignmentErrorType emType,const CString& strError )
{
	FlightAssignToStandErrorMessage errorMessage;
	errorMessage.m_emType = emType;
	errorMessage.m_strError = strError;
	m_vErrorMessageList.push_back(errorMessage);
}

void CFlightOperationForGateAssign::ClearErrorMessage()
{
	m_vErrorMessageList.clear();
}
void CFlightOperationForGateAssign::SetStand(const ALTObjectID& standID)
{
	if (m_eOpType == ARR_DEP_OP)
	{
		getFlight()->setArrStand(standID);
		getFlight()->setDepStand(standID);
	}
	if (m_eOpType == ARR_OP)
	{
		getFlight()->setArrStand(standID);
	}
	if (m_eOpType == DEP_OP)
	{
		getFlight()->setDepStand(standID);
	}
	if (m_eOpType == INTPAKRING_OP)
	{
		getFlight()->setIntermediateStand(standID);
	}
	getFlight()->setStandID(m_nGateIdx);
}

void CFlightOperationForGateAssign::SetGate(const ProcessorID& gateID)
{
	if (m_eOpType == ARR_OP)
	{
		((Flight*)getFlight())->setArrGate(gateID);
	}
	if (m_eOpType == DEP_OP)
	{
		((Flight*)getFlight())->setDepGate(gateID);
	}
}

FlightForAssignment* CFlightOperationForGateAssign::GetAssignmentFlight()
{
	return m_pFlight;
}

void CFlightOperationForGateAssign::SetGateIdx(int nIdx)
{
	m_nGateIdx = nIdx;
}
int CFlightOperationForGateAssign::GetGateIdx()
{
	return m_nGateIdx;
}

///////////////////////////////////////////////FlightForAssignment//////////////////////////////////
FlightForAssignment::FlightForAssignment(ARCFlight* _flight)
:m_pFlight(_flight)
,m_bSelected(false)
,m_pFlightArr(NULL)
,m_pFlightDep(NULL)
,m_pFlightInt(NULL)
{

}

FlightForAssignment::~FlightForAssignment()
{
	ClearAllOperations();
}

void FlightForAssignment::ClearAllOperations()
{
	if (m_pFlightArr)
	{
		delete m_pFlightArr;
		m_pFlightArr = NULL;
	}

	if (m_pFlightDep)
	{
		delete m_pFlightDep;
		m_pFlightDep = NULL;
	}

	if (m_pFlightInt)
	{
		delete m_pFlightInt;
		m_pFlightInt = NULL;
	}
}

ElapsedTime FlightForAssignment::getStartTime()
{
	if (m_pFlight->isArriving())
		return m_pFlight->getArrTime();

	return m_pFlight->getDepTime() - m_pFlight->getServiceTime(); 
}

bool FlightForAssignment::IsSelected()
{
	return m_bSelected;
}

void FlightForAssignment::SetSelected( bool _bSelected )
{
	m_bSelected = _bSelected;
	if (m_pFlightArr)
		m_pFlightArr->SetSelected(_bSelected);
	
	if (m_pFlightDep)
		m_pFlightDep->SetSelected(_bSelected);

	if (m_pFlightInt)
		m_pFlightInt->SetSelected(_bSelected);
	

}

void FlightForAssignment::SetFlightIntPart(CFlightOperationForGateAssign* pflight)
{
	m_pFlightInt = pflight;
}

void FlightForAssignment::ClearGateIdx()
{
	if (m_pFlightArr)
		m_pFlightArr->SetGateIdx(-1);
	if (m_pFlightDep)
		m_pFlightDep->SetGateIdx(-1);	
	if (m_pFlightInt)
		m_pFlightInt->SetGateIdx(-1);
}

void FlightForAssignment::EmptyStand()
{
	ALTObjectID standID;
	m_pFlight->setArrStand(standID);
	m_pFlight->setDepStand(standID);
	m_pFlight->setIntermediateStand(standID);
	ClearGateIdx();
}

void FlightForAssignment::EmptyGate()
{
	if (m_pFlightArr)
		m_pFlightArr->EmptyGate();
	if (m_pFlightDep)
		m_pFlightDep->EmptyGate();
}

ARCFlight* FlightForAssignment::getFlight()
{
	return m_pFlight;
}

CFlightOperationForGateAssign* FlightForAssignment::GetFlightArrPart()
{
	return m_pFlightArr;
}

CFlightOperationForGateAssign* FlightForAssignment::GetFlightDepPart()
{
	return m_pFlightDep;
}

void FlightForAssignment::SetFlightArrPart( CFlightOperationForGateAssign* pflight )
{
	m_pFlightArr = pflight;
}

void FlightForAssignment::SetFlightDepPart( CFlightOperationForGateAssign* pflight )
{
	m_pFlightDep = pflight;
}

CFlightOperationForGateAssign* FlightForAssignment::GetFlightIntPart()
{
	return m_pFlightInt;
}

ElapsedTime FlightForAssignment::getEndTime()
{
	if (m_pFlight->isDeparting())
	{
		return m_pFlight->getDepTime();
	}

	return m_pFlight->getArrTime() + m_pFlight->getServiceTime();
}

void FlightForAssignment::SetFailAssignReason( const CString& strReason )
{
	m_strFailAssignReason = strReason;
}

const CString& FlightForAssignment::GetFailAssignReason()
{
	return m_strFailAssignReason;
}

bool FlightForAssignment::getArrStartTime( ElapsedTime& startTime )
{
	if (m_pFlight == NULL)
		return false;
	
	if (!m_pFlight->isArriving())
		return false;
	
	startTime = m_pFlight->getArrTime();
	return true;
}

bool FlightForAssignment::getIntStartTime( ElapsedTime& startTime,CString& strError )
{
	if (m_pFlight == NULL)
		return false;

	if (m_pFlight->getIntermediateStand().IsBlank())
		return false;
	
	if (m_pFlight->GetTowoffTime() == -1l)
		return false;
	
	startTime.setPrecisely(m_pFlight->GetTowoffTime());
	char sBuff[255];
	m_pFlight->getFlightIDString(sBuff);
	if (m_pFlight->isArriving())
	{
		if (startTime < m_pFlight->getArrTime())
		{
			CString strErrorMessage;
			strErrorMessage.Format(_T("Flight: %s time of starting towoff is earlier that arriving time"),sBuff);
			if (!strError.IsEmpty())
			{
				strError += CString(_T("   \n")) + strErrorMessage;
			}
			else
			{
				strError += strErrorMessage;
			}
			
		}

		if (!m_pFlight->isDeparting())
		{
			ElapsedTime eStayTime;
			eStayTime = m_pFlight->getArrTime() + m_pFlight->getServiceTime();
			if (startTime > eStayTime )
			{
				CString strErrorMessage;
				strErrorMessage.Format(_T("Flight: %s time of starting tow off exceeds time of flight alive"),sBuff);
				if (!strError.IsEmpty())
				{
					strError += CString(_T("   \n")) + strErrorMessage;
				}
				else
				{
					strError += strErrorMessage;
				}
			}
		}
		
	}

	if (m_pFlight->isDeparting())
	{
		if (startTime > m_pFlight->getDepTime())
		{
			CString strErrorMessage;
			strErrorMessage.Format(_T("Flight: %s time of starting towoff exceeds departing time"),sBuff);
			if (!strError.IsEmpty())
			{
				strError += CString(_T("   \n")) + strErrorMessage;
			}
			else
			{
				strError += strErrorMessage;
			}
		}

		if (!m_pFlight->isArriving())
		{
			ElapsedTime eArrTime;
			eArrTime = m_pFlight->getDepTime() - m_pFlight->getServiceTime();
			if (startTime < eArrTime )
			{
				CString strErrorMessage;
				strErrorMessage.Format(_T("Flight: %s time of ending towoff is earlier than arriving time"),sBuff);
				if (!strError.IsEmpty())
				{
					strError += CString(_T("   \n")) + strErrorMessage;
				}
				else
				{
					strError += strErrorMessage;
				}
			}
		}
	}

	return true;
}

bool FlightForAssignment::getDepStartTime( ElapsedTime& startTime )
{
	if (m_pFlight == NULL)
		return false;

	if (!m_pFlight->isDeparting())
		return false;
	
	if (m_pFlight->GetTowoffTime() > -1l)
	{
		ElapsedTime exIntStandTime;
		exIntStandTime.setPrecisely(m_pFlight->GetExIntStandTime());
		startTime = exIntStandTime;
		if (startTime < 0l)
		{
			return false;
		}
	}
	else if(m_pFlight->isTurnaround())
	{
		if (m_pFlight->getArrStand() == m_pFlight->getDepStand())
		{
			return false;
		}
		else
		{
			startTime = m_pFlight->getArrTime() + (m_pFlight->getDepTime() - m_pFlight->getArrTime())/2L;
		}
		 
	}
	else
	{
		startTime = m_pFlight->getDepTime() - m_pFlight->getServiceTime();
	}

	return true;
}

bool FlightForAssignment::getArrEndTime( ElapsedTime& endTime )
{
	if (m_pFlight == NULL)
		return false;

	if (!m_pFlight->isArriving())
		return false;


	if (m_pFlight->GetTowoffTime() > -1l)
	{
		endTime.setPrecisely(m_pFlight->GetTowoffTime());
	}
	else if(m_pFlight->isTurnaround())
	{
		if (m_pFlight->getArrStand() == m_pFlight->getDepStand())
		{
			endTime = m_pFlight->getDepTime();
		}
		else
		{
			endTime = m_pFlight->getArrTime() + (m_pFlight->getDepTime() - m_pFlight->getArrTime())/2L;
		}
	}
	else
	{
		endTime = m_pFlight->getArrTime() + m_pFlight->getServiceTime();
	}
	return true;
}

bool FlightForAssignment::getIntEndTime( ElapsedTime& endTime,CString& strError )
{
	if (m_pFlight == NULL)
		return false;

	if (m_pFlight->getIntermediateStand().IsBlank())
		return false;

	if (m_pFlight->GetExIntStandTime() == -1l)
		return false;

	endTime.setPrecisely(m_pFlight->GetExIntStandTime());

	char sBuff[255];
	m_pFlight->getFlightIDString(sBuff);
	if (m_pFlight->isArriving())
	{
		if (endTime < m_pFlight->getArrTime())
		{
			CString strErrorMessage;
			strErrorMessage.Format(_T("Flight: %s time off ending towoff is earlier than arriving time"),sBuff);
			if (!strError.IsEmpty())
			{
				strError += CString(_T("   \n")) + strErrorMessage;
			}
			else
			{
				strError += strErrorMessage;
			}

		}

		if (!m_pFlight->isDeparting())
		{
			ElapsedTime eStayTime;
			eStayTime = m_pFlight->getArrTime() + m_pFlight->getServiceTime();
			if (endTime > eStayTime )
			{
				CString strErrorMessage;
				strErrorMessage.Format(_T("Flight: %s time off ending towoff exceeds time of flight alive"),sBuff);
				if (!strError.IsEmpty())
				{
					strError += CString(_T("   \n")) + strErrorMessage;
				}
				else
				{
					strError += strErrorMessage;
				}
			}
		}

	}

	if (m_pFlight->isDeparting())
	{
		if (endTime > m_pFlight->getDepTime())
		{
			CString strErrorMessage;
			strErrorMessage.Format(_T("Flight: %s time off ending towoff exceeds departing time"),sBuff);
			if (!strError.IsEmpty())
			{
				strError += CString(_T("   \n")) + strErrorMessage;
			}
			else
			{
				strError += strErrorMessage;
			}
		}

		if (!m_pFlight->isArriving())
		{
			ElapsedTime eArrTime;
			eArrTime = m_pFlight->getDepTime() - m_pFlight->getServiceTime();
			if (endTime < eArrTime )
			{
				CString strErrorMessage;
				strErrorMessage.Format(_T("Flight: %s time of ending towoff is earlier than arriving time"),sBuff);
				if (!strError.IsEmpty())
				{
					strError += CString(_T("   \n")) + strErrorMessage;
				}
				else
				{
					strError += strErrorMessage;
				}
			}
		}
	}
	return true;
}

bool FlightForAssignment::getDepEndTime( ElapsedTime& endTime )
{
	if (m_pFlight == NULL)
		return false;

	if (!m_pFlight->isDeparting())
		return false;

	if (m_pFlight->getArrStand() == m_pFlight->getDepStand() && m_pFlight->GetTowoffTime() == -1l)
		return false;
	
	endTime = m_pFlight->getDepTime();
	return true;
}
