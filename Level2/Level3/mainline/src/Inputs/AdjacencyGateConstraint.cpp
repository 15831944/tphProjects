#include "StdAfx.h"
#include "AdjacencyGateConstraint.h"
#include "../Common/ARCFlight.h"
#include "../Common/ACTypesManager.h"
#include "GateAssignmentMgr.h"
#include "in_term.h"
#include <common/ARCUnit.h>

GateAssignmentConstraint::GateAssignmentConstraint()
: m_conType(GateConType_Height)
, m_fMinValue(.0f)
, m_fMaxValue(.0f)
{
}

GateAssignmentConstraint::~GateAssignmentConstraint()
{
}

bool GateAssignmentConstraint::IsFlightFit(const ARCFlight& flight, InputTerminal* pInTerm)
{
	//FlightConstraint fltType = flight.getType();
	char str[AC_TYPE_LEN];
	flight.getACType(str);

	CACType acType, *anAircraft;
	acType.setIATACode(str);
	int ndx = _g_GetActiveACMan(pInTerm->m_pAirportDB)->findACTypeItem(&acType);
	anAircraft = _g_GetActiveACMan(pInTerm->m_pAirportDB)->getACTypeItem(ndx);

	float fCompareValue = .0f;

	switch(m_conType)
	{
	case GateConType_Height:
		fCompareValue = anAircraft->m_fHeight;
		break;

	case GateConType_Len:
		fCompareValue = anAircraft->m_fLen;
		break;

	case GateConType_Span:
		fCompareValue = anAircraft->m_fSpan;
		break;

	case GateConType_MZFW:
		fCompareValue = (float)ARCUnit::KgsToLBS(anAircraft->m_fMZFW);
		break;

	case GateConType_OEW:
		fCompareValue = (float)ARCUnit::KgsToLBS(anAircraft->m_fOEW);
		break;

	case GateConType_MTOW:
		fCompareValue = (float)ARCUnit::KgsToLBS(anAircraft->m_fMTOW);
		break;

	case GateConType_MLW:
		fCompareValue = (float)ARCUnit::KgsToLBS(anAircraft->m_fMLW);
		break;

	case GateConType_Capacity:
		fCompareValue = float(anAircraft->m_iCapacity);
		break;

	default:
		break;
	}

	if (fCompareValue <= m_fMaxValue && fCompareValue >= m_fMinValue)
		return true;

	return false;
}

//////////////////////////////////////AdjacencyGateContraint//////////////////////////////////////
AdjacencyGateConstraint::AdjacencyGateConstraint(GateAssignmentConstraint* pFirstConstraint, GateAssignmentConstraint* pSecondConstraint)
{
	m_nFirstGateIdxInAssignmentMgr = -1;
	m_nSecondGateIdxInAssignmentMgr = -1;

	m_pFirstConstraint = pFirstConstraint;
	m_pSecondConstraint = pSecondConstraint;
}

AdjacencyGateConstraint::AdjacencyGateConstraint(const AdjacencyGateConstraint& adjCon)
:m_pFirstConstraint(NULL)
,m_pSecondConstraint(NULL)
{
	*this = adjCon;
}

AdjacencyGateConstraint::~AdjacencyGateConstraint()
{
	if (m_pFirstConstraint != NULL)
	{
		delete m_pFirstConstraint;
		m_pFirstConstraint = NULL;
	}

	if (m_pSecondConstraint != NULL)
	{
		delete m_pSecondConstraint;
		m_pSecondConstraint = NULL;
	}
}

AdjacencyGateConstraint& AdjacencyGateConstraint::operator =(const AdjacencyGateConstraint& _constraint)
{
	if (m_pFirstConstraint != NULL)
		delete m_pFirstConstraint;

	m_pFirstConstraint = new GateAssignmentConstraint;
	m_pFirstConstraint->SetGate(_constraint.m_pFirstConstraint->GetGate());
	m_pFirstConstraint->SetConType(_constraint.m_pFirstConstraint->GetConType());
	m_pFirstConstraint->SetMaxValue(_constraint.m_pFirstConstraint->GetMaxValue());
	m_pFirstConstraint->SetMinValue(_constraint.m_pFirstConstraint->GetMinValue());


	if (m_pSecondConstraint != NULL)
		delete m_pSecondConstraint;

	m_pSecondConstraint = new GateAssignmentConstraint;
	m_pSecondConstraint->SetGate(_constraint.m_pSecondConstraint->GetGate());
	m_pSecondConstraint->SetConType(_constraint.m_pSecondConstraint->GetConType());
	m_pSecondConstraint->SetMaxValue(_constraint.m_pSecondConstraint->GetMaxValue());
	m_pSecondConstraint->SetMinValue(_constraint.m_pSecondConstraint->GetMinValue());

	m_nFirstGateIdxInAssignmentMgr =_constraint.m_nFirstGateIdxInAssignmentMgr ;
	m_nSecondGateIdxInAssignmentMgr = _constraint.m_nSecondGateIdxInAssignmentMgr ;

	return *this;

}

bool AdjacencyGateConstraint::IsFlightFitConstraint(const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights, int nGateIdx, InputTerminal* pInTerm,CString& strError)
{
	if (vFlights.empty())		//no adjacent flights
		return true;

	int nCount = vFlights.size();
	if (m_nFirstGateIdxInAssignmentMgr == nGateIdx)	//first constraint
	{
		for (int i =0; i < nCount; i++)
		{
			ARCFlight* pAdjFlight = vFlights.at(i);
			//if (!m_pFirstConstraint->IsFlightFit(*pFlight,pInTerm))
			//{
			//	CString strFirstStand;
			//	strFirstStand = m_pFirstConstraint->GetGate().GetIDString();
			//	CString strSecondStand;
			//	strSecondStand = m_pSecondConstraint->GetGate().GetIDString();
			//	char fristBuff[255];
			//	char secondBuff[255];
			//	pFlight->getFlightIDString(fristBuff);
			//	pAdjFlight->getFlightIDString(secondBuff);
			//	strError.Format(_T("Adjacency flight: %s at stand: %s of second constraint makes \n    flight: %s stand: %s doesn't satisfy first constraint"),secondBuff,\
			//		strSecondStand,fristBuff,strFirstStand);
			//	return false;
			//}

			//if (!m_pSecondConstraint->IsFlightFit(*pAdjFlight, pInTerm))
			//{
			//	CString strFirstStand;
			//	strFirstStand = m_pFirstConstraint->GetGate().GetIDString();
			//	CString strSecondStand;
			//	strSecondStand = m_pSecondConstraint->GetGate().GetIDString();
			//	char fristBuff[255];
			//	char secondBuff[255];
			//	pFlight->getFlightIDString(fristBuff);
			//	pAdjFlight->getFlightIDString(secondBuff);
			//	strError.Format(_T("Flight: %s at stand: %s of first constraint makes adjacency \n    flight: %s stand: %s doesn't satisfy second constraint"),fristBuff,\
			//		strFirstStand,secondBuff,strSecondStand);
			//	return false;
			//}
			if(!m_pFirstConstraint->IsFlightFit(*pFlight,pInTerm))
			{
				if (m_pSecondConstraint->IsFlightFit(*pAdjFlight, pInTerm))
				{
					CString strFirstStand;
					strFirstStand = m_pFirstConstraint->GetGate().GetIDString();
					CString strSecondStand;
					strSecondStand = m_pSecondConstraint->GetGate().GetIDString();
					char fristBuff[255];
					char secondBuff[255];
					pFlight->getFlightIDString(fristBuff);
					pAdjFlight->getFlightIDString(secondBuff);
					strError.Format(_T("Adjacency flight: %s at stand: %s of second constraint makes \n    flight: %s stand: %s doesn't satisfy first constraint"),secondBuff,\
						strSecondStand,fristBuff,strFirstStand);
					return false;
				}
			}
		}
	}
	else
	{
		for (int i =0; i < nCount; i++)
		{
			ARCFlight* pAdjFlight = vFlights.at(i);
			//if (!m_pSecondConstraint->IsFlightFit(*pFlight,pInTerm))
			//{
			//	CString strFirstStand;
			//	strFirstStand = m_pFirstConstraint->GetGate().GetIDString();
			//	CString strSecondStand;
			//	strSecondStand = m_pSecondConstraint->GetGate().GetIDString();
			//	char fristBuff[255];
			//	char secondBuff[255];
			//	pFlight->getFlightIDString(fristBuff);
			//	pAdjFlight->getFlightIDString(secondBuff);
			//	strError.Format(_T("Flight: %s at stand: %s of first constraint makes adjacency \n    flight: %s stand: %s doesn't satisfy second constraint"),fristBuff,\
			//		strFirstStand,secondBuff,strSecondStand);
			//	return false;
			//}

			//if (!m_pFirstConstraint->IsFlightFit(*pAdjFlight, pInTerm))
			//{
			//	CString strFirstStand;
			//	strFirstStand = m_pFirstConstraint->GetGate().GetIDString();
			//	CString strSecondStand;
			//	strSecondStand = m_pSecondConstraint->GetGate().GetIDString();
			//	char fristBuff[255];
			//	char secondBuff[255];
			//	pFlight->getFlightIDString(fristBuff);
			//	pAdjFlight->getFlightIDString(secondBuff);
			//	strError.Format(_T("Adjacency flight: %s at stand: %s of second constraint makes \n   flight: %s stand: %s doesn't satisfy first constraint"),secondBuff,\
			//		strSecondStand,fristBuff,strFirstStand);
			//	return false;
			//}

			if (!m_pSecondConstraint->IsFlightFit(*pFlight,pInTerm))
			{
				if (m_pFirstConstraint->IsFlightFit(*pAdjFlight,pInTerm))
				{
					CString strFirstStand;
					strFirstStand = m_pFirstConstraint->GetGate().GetIDString();
					CString strSecondStand;
					strSecondStand = m_pSecondConstraint->GetGate().GetIDString();
					char fristBuff[255];
					char secondBuff[255];
					pFlight->getFlightIDString(fristBuff);
					pAdjFlight->getFlightIDString(secondBuff);
					strError.Format(_T("Adjacency flight: %s at stand: %s of first constraint makes \n   flight: %s stand: %s doesn't satisfy second constraint"),secondBuff,\
						strFirstStand,fristBuff,strSecondStand);
					return false;
				}
			}
		}		
	}


	return true;
}

bool AdjacencyGateConstraint::IsFlightFitContraint(const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights,int nGateIdx, InputTerminal* pInTerm)
{
	if (vFlights.empty())		//no adjacent flights
		return true;

	int nCount = vFlights.size();
	if (m_nFirstGateIdxInAssignmentMgr == nGateIdx)	//first constraint
	{
		for (int i =0; i < nCount; i++)
		{
			ARCFlight* pAdjFlight = vFlights.at(i);
			/*if (!m_pFirstConstraint->IsFlightFit(*pFlight,pInTerm) || !m_pSecondConstraint->IsFlightFit(*pAdjFlight, pInTerm))
				return false;*/
			if (!m_pFirstConstraint->IsFlightFit(*pFlight,pInTerm))
			{
				if (m_pSecondConstraint->IsFlightFit(*pAdjFlight,pInTerm))
				{
					return false;
				}
			}
		}
	}
	else
	{
		for (int i =0; i < nCount; i++)
		{
			ARCFlight* pAdjFlight = vFlights.at(i);
		/*	if (!m_pSecondConstraint->IsFlightFit(*pFlight,pInTerm) || !m_pFirstConstraint->IsFlightFit(*pAdjFlight, pInTerm))
				return false;*/
			if (!m_pSecondConstraint->IsFlightFit(*pFlight,pInTerm))
			{
				if (m_pFirstConstraint->IsFlightFit(*pAdjFlight,pInTerm))
				{
					return false;
				}
			}
		}		
	}


	return true;
}

/////////////////////////////////////////////////AdjacencyGateConstraintList/////////////////////////////////////
AdjacencyGateConstraintList::AdjacencyGateConstraintList()
 : DataSet(AdjacencyGateConFile,2.3f)
{
}

AdjacencyGateConstraintList::~AdjacencyGateConstraintList()
{
}

void AdjacencyGateConstraintList::clear()
{
	m_adjacencyConstraints.clear();
}

void AdjacencyGateConstraintList::readData(ArctermFile& p_file)
{
	while (p_file.getLine())
	{
		GateAssignmentConstraint* gateConstraint1 = new GateAssignmentConstraint;
		//gateConstraint1.GetGate().SetStrDict(DataSet::m_pInTerm->inStrDict);
		gateConstraint1->GetGate().readALTObjectID(p_file);

		int conType;
		if (!p_file.getInteger(conType))
			return;
		gateConstraint1->SetConType(GateAssignConType(conType));

		float value;
		if (!p_file.getFloat(value))
			return;
		gateConstraint1->SetMinValue(value);

		if (!p_file.getFloat(value))
			return;
		gateConstraint1->SetMaxValue(value);


		GateAssignmentConstraint* gateConstraint2 = new GateAssignmentConstraint;
		//gateConstraint2.GetGate().SetStrDict(DataSet::m_pInTerm->inStrDict);
		gateConstraint2->GetGate().readALTObjectID(p_file);

		if (!p_file.getInteger(conType))
			return;
		gateConstraint2->SetConType(GateAssignConType(conType));

		if (!p_file.getFloat(value))
			return;
		gateConstraint2->SetMinValue(value);

		if (!p_file.getFloat(value))
			return;
		gateConstraint2->SetMaxValue(value);

		AdjacencyGateConstraint con(gateConstraint1, gateConstraint2);
		m_adjacencyConstraints.push_back(con);
	}
}

void AdjacencyGateConstraintList::writeData(ArctermFile& p_file) const
{
	size_t nCount = m_adjacencyConstraints.size();
	for(size_t i = 0; i < nCount; i++)
	{
		AdjacencyGateConstraint constraint = m_adjacencyConstraints.at(i);
		GateAssignmentConstraint* gateConstraint1 = constraint.GetFirstConstraint();
		gateConstraint1->GetGate().writeALTObjectID(p_file);
		p_file.writeInt(gateConstraint1->GetConType());
		p_file.writeFloat(gateConstraint1->GetMinValue());
		p_file.writeFloat(gateConstraint1->GetMaxValue());

		GateAssignmentConstraint* gateConstraint2 = constraint.GetSecondConstraint();
		gateConstraint2->GetGate().writeALTObjectID(p_file);
		p_file.writeInt(gateConstraint2->GetConType());
		p_file.writeFloat(gateConstraint2->GetMinValue());
		p_file.writeFloat(gateConstraint2->GetMaxValue());

		p_file.writeLine();
	}
}


int AdjacencyGateConstraintList::GetCount()
{
	return (int)m_adjacencyConstraints.size();
}

void AdjacencyGateConstraintList::AddItem(const AdjacencyGateConstraint& gateAssignCon)
{
	m_adjacencyConstraints.push_back(gateAssignCon);
}

void AdjacencyGateConstraintList::DeleteItem(int nIndex)
{
	ASSERT(nIndex > -1 && nIndex < GetCount());
	m_adjacencyConstraints.erase(m_adjacencyConstraints.begin() + nIndex);
}

AdjacencyGateConstraint& AdjacencyGateConstraintList::GetItem(int nIndex)
{
	ASSERT(nIndex > -1 && nIndex < GetCount());
	return m_adjacencyConstraints[nIndex];
}

bool AdjacencyGateConstraintList::FlightAndGateFitErrorMessage(const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList,CString& strError)
{
	for (std::vector<AdjacencyGateConstraint>::iterator iter = m_adjacencyConstraints.begin(); iter != m_adjacencyConstraints.end(); ++iter)
	{
		//TRACE("\nGate 1: %s, Gate 2: %s", gate1.GetIDString(), gate2.GetIDString());
		//TRACE("\nAdjacency Gate 1: %s  Adjacency Gate 2: %s", iter->first.GetGate().GetIDString(), iter->first.GetGate().GetIDString() );
		if (nGateIdx == iter->GetFirstGateIdx())
		{
			int nAdjGateIdx = iter->GetSecondGateIdx();
			if (nAdjGateIdx < 0 && iter->GetSecondConstraint())
			{
				CString strMsg;
				strMsg.Format(" The Stand: %s in Adjacent Constraint is not exists!",iter->GetSecondConstraint()->GetGate().GetIDString());
				AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
				continue;
			}

			if (nAdjGateIdx == nGateIdx)
				return true;

			std::vector<ARCFlight*> vAdjFlights;
			vAdjFlights.clear();

			vGateList[nAdjGateIdx]->GetFlightsInTimeRange(tStart,tEnd,vAdjFlights);

			if (!iter->IsFlightFitConstraint(flight,vAdjFlights,nGateIdx,m_pInTerm,strError))
				return false;
		}

		if (nGateIdx == iter->GetSecondGateIdx())
		{
			int nAdjGateIdx = iter->GetFirstGateIdx();
			if (nAdjGateIdx < 0 && iter->GetSecondConstraint())
			{
				CString strMsg;
				strMsg.Format(" The Stand: %s in Adjacent Constraint is not exists!",iter->GetSecondConstraint()->GetGate().GetIDString());
				AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
				continue;
			}

			if (nAdjGateIdx == nGateIdx)
				return true;

			std::vector<ARCFlight*> vAdjFlights;
			vAdjFlights.clear();

			vGateList[nAdjGateIdx]->GetFlightsInTimeRange(tStart,tEnd,vAdjFlights);

			if (!iter->IsFlightFitConstraint(flight,vAdjFlights,nGateIdx,m_pInTerm,strError))
				return false;
		}
	}


	return true;
}

bool AdjacencyGateConstraintList::IsFlightAndGateFit(const ARCFlight* flight,int nGateIdx,const ElapsedTime& tStart, const ElapsedTime& tEnd,std::vector<ref_ptr_bee<CAssignGate> >& vGateList)
{
	for (std::vector<AdjacencyGateConstraint>::iterator iter = m_adjacencyConstraints.begin(); iter != m_adjacencyConstraints.end(); ++iter)
	{
		//TRACE("\nGate 1: %s, Gate 2: %s", gate1.GetIDString(), gate2.GetIDString());
		//TRACE("\nAdjacency Gate 1: %s  Adjacency Gate 2: %s", iter->first.GetGate().GetIDString(), iter->first.GetGate().GetIDString() );
		if (nGateIdx == iter->GetFirstGateIdx())
		{
			int nAdjGateIdx = iter->GetSecondGateIdx();
			if (nAdjGateIdx < 0 && iter->GetSecondConstraint())
			{
				CString strMsg;
				strMsg.Format(" The Stand: %s in Adjacent Constraint is not exists!",iter->GetSecondConstraint()->GetGate().GetIDString());
				AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
				continue;
			}

			if (nAdjGateIdx == nGateIdx)
				return true;

			std::vector<ARCFlight*> vAdjFlights;
			vAdjFlights.clear();

			vGateList[nAdjGateIdx]->GetFlightsInTimeRange(tStart,tEnd,vAdjFlights);

			if (!iter->IsFlightFitContraint(flight,vAdjFlights,nGateIdx,m_pInTerm))
				return false;
		}

		if (nGateIdx == iter->GetSecondGateIdx())
		{
			int nAdjGateIdx = iter->GetFirstGateIdx();
			if (nAdjGateIdx < 0 && iter->GetSecondConstraint())
			{
				CString strMsg;
				strMsg.Format(" The Stand: %s in Adjacent Constraint is not exists!",iter->GetSecondConstraint()->GetGate().GetIDString());
				AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
				continue;
			}

			if (nAdjGateIdx == nGateIdx)
				return true;

			std::vector<ARCFlight*> vAdjFlights;
			vAdjFlights.clear();

			vGateList[nAdjGateIdx]->GetFlightsInTimeRange(tStart,tEnd,vAdjFlights);

			if (!iter->IsFlightFitContraint(flight,vAdjFlights,nGateIdx,m_pInTerm))
				return false;
		}
	}


	return true;
}

void AdjacencyGateConstraintList::InitConstraintGateIdx(std::vector<ref_ptr_bee<CAssignGate> >& vGateList)
{
	int nConstraint = GetCount();
	int nGateNum = vGateList.size();

	for (int i = 0; i < nConstraint; i++)
	{
		AdjacencyGateConstraint& constraint = GetItem(i);
		for (int j = 0; j < nGateNum; j++)
		{
			if (constraint.GetFirstGateIdx() <0 && vGateList[j]->GetName() == constraint.GetFirstConstraint()->GetGate().GetIDString())
				constraint.SetFirstGateIdx(j);

			if (constraint.GetSecondGateIdx() <0 && vGateList[j]->GetName() == constraint.GetSecondConstraint()->GetGate().GetIDString())
				constraint.SetSecondGateIdx(j);
			
			if (constraint.GetFirstGateIdx() >=0 && constraint.GetSecondGateIdx() >=0)
				break;
		}
	}
}