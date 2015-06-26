#include "StdAfx.h"
#include ".\ctrlinterventionspecdata.h"
#include "AirsideImportExportManager.h"
#include "..\Database\ADODatabase.h"

CCtrlInterventionSpecData::CCtrlInterventionSpecData(void)
 :DBElement()
 ,m_RunwayID(0)
 ,m_MarkingIndex(1)
 ,m_PriorityOne(IM_SPEEDCTRL)
 ,m_PriorityTwo(IM_SIDESTEP)
 ,m_PriorityThree(IM_GOAROUND)
{
}

CCtrlInterventionSpecData::~CCtrlInterventionSpecData(void)
{
}

void CCtrlInterventionSpecData::SetRunwayID(int nRunwayID)
{
	m_RunwayID = nRunwayID;
}

int CCtrlInterventionSpecData::GetRunwayID()const
{
	return m_RunwayID;
}

void CCtrlInterventionSpecData::SetMarkingIndex(int nMarkingIndex)
{
	m_MarkingIndex = nMarkingIndex;
}

int CCtrlInterventionSpecData::GetMarkingIndex()
{
	return m_MarkingIndex;
}

void CCtrlInterventionSpecData::SetPriorityOne(INTERVENTIONMETHOD enumInterventionMethod)
{
	m_PriorityOne = enumInterventionMethod;
}

INTERVENTIONMETHOD CCtrlInterventionSpecData::GetPriorityOne()const
{
	return m_PriorityOne;
}

void CCtrlInterventionSpecData::SetPriorityTwo(INTERVENTIONMETHOD enumInterventionMethod)
{
	m_PriorityTwo = enumInterventionMethod;
}

INTERVENTIONMETHOD CCtrlInterventionSpecData::GetPriorityTwo()const
{
	return m_PriorityTwo;
}

void CCtrlInterventionSpecData::SetPriorityThree(INTERVENTIONMETHOD enumInterventionMethod)
{
	m_PriorityThree = enumInterventionMethod;
}

INTERVENTIONMETHOD CCtrlInterventionSpecData::GetPriorityThree()const
{
	return m_PriorityThree;
}

void CCtrlInterventionSpecData::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	exportFile.getFile().writeInt(m_RunwayID);
	exportFile.getFile().writeInt(m_MarkingIndex);
	exportFile.getFile().writeInt((int)m_PriorityOne);
	exportFile.getFile().writeInt((int)m_PriorityTwo);
	exportFile.getFile().writeInt((int)m_PriorityThree);
	exportFile.getFile().writeLine();
}

void CCtrlInterventionSpecData::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldID);
	importFile.getFile().getInteger(nOldProjID);
	importFile.getFile().getInteger(m_RunwayID);
	importFile.getFile().getInteger(m_MarkingIndex);

	int nPriority;
	importFile.getFile().getInteger(nPriority);
	m_PriorityOne = (INTERVENTIONMETHOD)nPriority;
	importFile.getFile().getInteger(nPriority);
	m_PriorityTwo = (INTERVENTIONMETHOD)nPriority;
	importFile.getFile().getInteger(nPriority);
	m_PriorityThree = (INTERVENTIONMETHOD)nPriority;

	importFile.getFile().getLine();

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_CTRLINTERVENTIONSPEC\
					 (PROJID, RUNWAYID, RUNWAYMARKING, PRIORITYONE, PRIORITYTWO, PRIORITYTHREE) \
					 VALUES (%d,%d,%d,%d,%d,%d)"),
					 importFile.getNewProjectID(),
					 m_RunwayID,
					 m_MarkingIndex,
					 (int)m_PriorityOne,
					 (int)m_PriorityTwo,
					 (int)m_PriorityThree);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CCtrlInterventionSpecData::Save(int nParentID)
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(nParentID,strSQL);

		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

//DBElement
void CCtrlInterventionSpecData::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("RUNWAYID"),m_RunwayID);
	recordset.GetFieldValue(_T("RUNWAYMARKING"),m_MarkingIndex);
	int nPriorityOne;
	recordset.GetFieldValue(_T("PRIORITYONE"),nPriorityOne);
	m_PriorityOne = (INTERVENTIONMETHOD)nPriorityOne;
	int nPriorityTwo;
	recordset.GetFieldValue(_T("PRIORITYTWO"),nPriorityTwo);
	m_PriorityTwo = (INTERVENTIONMETHOD)nPriorityTwo;
	int nPriorityThree;
	recordset.GetFieldValue(_T("PRIORITYTHREE"),nPriorityThree);
	m_PriorityThree = (INTERVENTIONMETHOD)nPriorityThree;
}

void CCtrlInterventionSpecData::GetInsertSQL(int nProjID, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_CTRLINTERVENTIONSPEC\
					 (PROJID, RUNWAYID, RUNWAYMARKING, PRIORITYONE, PRIORITYTWO, PRIORITYTHREE) \
					 VALUES (%d,%d,%d,%d,%d,%d)"),
					 nProjID,
					 m_RunwayID,
					 m_MarkingIndex,
					 (int)m_PriorityOne,
					 (int)m_PriorityTwo,
					 (int)m_PriorityThree);
}

void CCtrlInterventionSpecData::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_CTRLINTERVENTIONSPEC \
					 SET PRIORITYONE =%d, PRIORITYTWO =%d, PRIORITYTHREE =%d \
					 WHERE (ID =%d)"),
					 (int)m_PriorityOne,
					 (int)m_PriorityTwo,
					 (int)m_PriorityThree,
					 m_nID);
}

void CCtrlInterventionSpecData::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_CTRLINTERVENTIONSPEC\
					 WHERE (ID = %d)"),m_nID);
}

int CCtrlInterventionSpecData::GetPriority( INTERVENTIONMETHOD enumInterventionMethod ) const
{
	if( GetPriorityOne() == enumInterventionMethod) return 0;
	if( GetPriorityTwo() == enumInterventionMethod )return 1;
	if( GetPriorityThree() == enumInterventionMethod )return 2;
	return 0;
}