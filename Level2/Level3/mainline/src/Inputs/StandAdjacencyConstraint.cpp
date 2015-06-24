#include "StdAfx.h"
#include "StandAdjacencyConstraint.h"
#include "../common/dbdata.h"


StandAdjacencyConstraint::StandAdjacencyConstraint()
:m_nID(-1)
,m_bUseOld(false)
{
}

StandAdjacencyConstraint::~StandAdjacencyConstraint()
{

}

void StandAdjacencyConstraint::readData()
{
	CString strSQL;
	strSQL.Format("select * from AIRSIDE_ADJACENCY_CONSTRAINTS");
	long lCount=0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,lCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue("ID",m_nID);
		int nUseOld = 0;
		adoRecordset.GetFieldValue("ISCHICK",nUseOld);
		m_bUseOld = nUseOld ? 1 : 0;
		SetUseFlag(m_bUseOld);
	}
}

void StandAdjacencyConstraint::writeData()
{
	CString strSQL;
	if (m_nID<0)
	{
		strSQL.Format(_T("INSERT INTO AIRSIDE_ADJACENCY_CONSTRAINTS (ISCHICK) VALUES (%d)"),m_bUseOld?1:0);
		m_nID=CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		strSQL.Format(_T("UPDATE AIRSIDE_ADJACENCY_CONSTRAINTS SET ISCHICK=%d WHERE ID=%d"),\
			m_bUseOld,m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

bool StandAdjacencyConstraint::StandAdjacencyFit( const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList )
{
	if (m_bUseOld)
	{
		return m_adjGateConstratins.IsFlightAndGateFit(flight,nGateIdx,tStart, tEnd,vGateList);
	}

	return m_adjConstratinSpec.IsFlightAndGateFit(flight,nGateIdx,tStart, tEnd,vGateList);
}

bool StandAdjacencyConstraint::StandAdjacencyFitErrorMessage( const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList,CString& strError )
{
	if (m_bUseOld)
	{
		return m_adjGateConstratins.FlightAndGateFitErrorMessage(flight,nGateIdx,tStart, tEnd,vGateList,strError);
	}

	return m_adjConstratinSpec.FlightAndGateFitErrorMessage(flight,nGateIdx,tStart, tEnd,vGateList,strError);
}

void StandAdjacencyConstraint::loadDataSet( const CString& _pProjPath,InputTerminal* _pInTerm )
{
	readData();
	m_adjConstratinSpec.SetInputTerminal(_pInTerm);
	m_adjConstratinSpec.loadDataSet(_pProjPath);
	m_adjGateConstratins.SetInputTerminal(_pInTerm);
	m_adjGateConstratins.loadDataSet(_pProjPath);
}

void StandAdjacencyConstraint::InitConstraintGateIdx( std::vector<ref_ptr_bee<CAssignGate> >& vGateList )
{
	m_adjGateConstratins.InitConstraintGateIdx(vGateList);
	m_adjConstratinSpec.InitConstraintGateIdx(vGateList);
}
