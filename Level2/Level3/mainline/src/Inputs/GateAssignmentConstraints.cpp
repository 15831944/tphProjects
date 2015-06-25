#include "StdAfx.h"
#include "GateAssignmentConstraints.h"
#include "../InputAirside/CParkingStandBuffer.h"
#include "../Common/ARCFlight.h"
#include "Main/TermPlanDoc.h"

GateAssignmentConstraintList::GateAssignmentConstraintList()
 : DataSet(GateAssignConFile)
 ,m_pParkingStandBufferList(NULL)
{

}

GateAssignmentConstraintList::~GateAssignmentConstraintList()
{

}

void GateAssignmentConstraintList::clear()
{
	m_constraints.clear();
}

void GateAssignmentConstraintList::readData(ArctermFile& p_file)
{
	while (p_file.getLine())
	{
		GateAssignmentConstraint gateConstraint;
		//gateConstraint.GetGate().SetStrDict(DataSet::m_pInTerm->inStrDict);
		gateConstraint.GetGate().readALTObjectID(p_file);

		int conType;
		if (!p_file.getInteger(conType))
			return;
		gateConstraint.SetConType(GateAssignConType(conType));

		float value;
		if (!p_file.getFloat(value))
			return;
		gateConstraint.SetMinValue(value);

		if (!p_file.getFloat(value))
			return;
		gateConstraint.SetMaxValue(value);

		m_constraints.push_back(gateConstraint);
	}
}


void GateAssignmentConstraintList::SetParkingStandBufferList(ParkingStandBufferList* pStandBufferList)
{
	ASSERT(pStandBufferList != NULL);
	m_pParkingStandBufferList = pStandBufferList;
}

void GateAssignmentConstraintList::writeData(ArctermFile& p_file) const
{
	size_t nCount = m_constraints.size();
	for(size_t i = 0; i < nCount; i++)
	{
		GateAssignmentConstraint gateConstraint = m_constraints[i];
		gateConstraint.GetGate().writeALTObjectID(p_file);
		p_file.writeInt(gateConstraint.GetConType());
		p_file.writeFloat(gateConstraint.GetMinValue());
		p_file.writeFloat(gateConstraint.GetMaxValue());

		p_file.writeLine();
	}
}

int GateAssignmentConstraintList::GetCount()
{
	return (int)m_constraints.size();
}

void GateAssignmentConstraintList::AddItem(const GateAssignmentConstraint& gateAssignCon)
{
	m_constraints.push_back(gateAssignCon);
}

void GateAssignmentConstraintList::DeleteItem(int nIndex)
{
	ASSERT(nIndex > -1 && nIndex < GetCount());
	m_constraints.erase(m_constraints.begin() + nIndex);
}

GateAssignmentConstraint& GateAssignmentConstraintList::GetItem(int nIndex)
{
	ASSERT(nIndex > -1 && nIndex < GetCount());
	return m_constraints[nIndex];
}

bool GateAssignmentConstraintList::IsFlightAndGateFit(const ARCFlight* flight, const ALTObjectID& gate)
{
	//TRACE("\nCurrent Gate: %s", gate.GetIDString());
	if (!m_ACTypeStandConstraints.IsFlightFit(flight,gate))
		return false;

	for(std::vector<GateAssignmentConstraint>::iterator iter = m_constraints.begin(); iter != m_constraints.end(); ++iter)
	{
		//TRACE("\nGate In Constraints: %s", iter->GetGate().GetIDString());
		if(gate.idFits(iter->GetGate()))
		{
			if (!iter->IsFlightFit(*flight, m_pInTerm))
				return false;
		}
	}

	return true;
}

bool GateAssignmentConstraintList::IsFlightAndGateFit( const ARCFlight* pFlight, const ALTObjectID& gate, CString& strError )
{
	if (!m_ACTypeStandConstraints.IsFlightFit(pFlight,gate))
	{
		char strAC[64];
		pFlight->getACType(strAC);
		strError.Format(_T("Aircraft type: %s and Stand: %s doesn't satisfy aircraft type constraint"),strAC,gate.GetIDString());
		return false;
	}

	for(std::vector<GateAssignmentConstraint>::iterator iter = m_constraints.begin(); iter != m_constraints.end(); ++iter)
	{
		//TRACE("\nGate In Constraints: %s", iter->GetGate().GetIDString());
		if(gate.idFits(iter->GetGate()))
		{
			if (!iter->IsFlightFit(*pFlight, m_pInTerm))
			{
				char strAC[64];
				pFlight->getACType(strAC);
				strError.Format(_T("Aircraft type: %s and Stand: %s doesn't satisfy stand constraint"),strAC,gate.GetIDString());
				return false;
			}
		}
	}

	return true;
}

void GateAssignmentConstraintList::LoadataStandConstraint(CTermPlanDoc* Doc)
{
	SetInputTerminal(&Doc->GetTerminal());
	loadDataSet(Doc->m_ProjInfo.path);

	m_adjaconstrain.GetAdjConstraintSpec().SetInputTerminal(&Doc->GetTerminal());
	m_adjaconstrain.GetAdjConstraintSpec().loadDataSet(Doc->m_ProjInfo.path);
	m_adjaconstrain.GetAdjConstraints().SetInputTerminal(&Doc->GetTerminal());
	m_adjaconstrain.GetAdjConstraints().loadDataSet(Doc->m_ProjInfo.path);
	m_adjaconstrain.readData();
	m_ACTypeStandConstraints.ReadData();
}
