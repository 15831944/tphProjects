#include "stdafx.h"
#include "LandingBehindTakeoff.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "../Common/ARCUnit.h"

LandingBehindTakeoffSameRunway::LandingBehindTakeoffSameRunway(int nProjID)
:ApproachSeparationItem(nProjID,LandingBehindTakeoffOnSameRunway)
,m_nID(-1)
,m_dLeadDisFromThreshold(10000.0)
{
}

LandingBehindTakeoffSameRunway::~LandingBehindTakeoffSameRunway(void)
{
}

void LandingBehindTakeoffSameRunway::ReadData()
{
	CString strSQL;
	strSQL.Format("SELECT * FROM APPROACHSEPARATIONCRITERIA WHERE PROJID = %d AND APPSEPTYPE = %d ORDER BY ID",
		m_nProjID, (int)m_emType);

	CADORecordset classADORecordSet;
	long lEffect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,lEffect,classADORecordSet);

	while (!classADORecordSet.IsEOF())
	{
		classADORecordSet.GetFieldValue(_T("ID"),m_nID);
		classADORecordSet.GetFieldValue(_T("LEADDISFROMTHRESHOLD"),m_dLeadDisFromThreshold);
		classADORecordSet.MoveNextData(); 
	}	
}
void LandingBehindTakeoffSameRunway::SaveData()
{
	ApproachSeparationItem::SaveData();
	UpdateData();
}
void LandingBehindTakeoffSameRunway::UpdateData()
{
	double dTemp = 0;
	CString strSQL;
	strSQL.Format(_T("INSERT INTO APPROACHSEPARATIONCRITERIA \
					 (PROJID,\
					 APPSEPTYPE,\
					 ARCCLASSTYPE,\
					 TRAILID,\
					 LEADID,\
					 MINSEPARATION,\
					 RUNWAYID,\
					 RUNWAYPORT,\
					 INTRRUNWAYID,\
					 INTRRUNWAYPORT,\
					 TIMEDIFFINTRFLTPATH,\
					 TIMEDIFFINTRRUNWAY,\
					 DISDIFFINTRFLTPATH,\
					 DISDIFFINTRRUNWAY,\
					 LEADDISFROMTHRESHOLD) \
					 VALUES (%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f)"), \
					 m_nProjID,\
					 (int)m_emType,\
					 -1,\
					 -1,\
					 -1,\
					 -1.0,\
					 -1,\
					 -1,\
					 -1,\
					 -1,\
					 -1.0,\
					 -1.0,\
					 -1.0,\
					 -1.0,\
					 dTemp);
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void LandingBehindTakeoffSameRunway::ImportData(CAirsideImportFile& importFile)
{
	ApproachSeparationItem::ImportData(importFile);
	importFile.getFile().getFloat(m_dLeadDisFromThreshold);
	importFile.getFile().getLine();
}
void LandingBehindTakeoffSameRunway::ExportData(CAirsideExportFile& exportFile)
{
	double dTemp = 0;
	ApproachSeparationItem::ExportData(exportFile);
	exportFile.getFile().writeDouble(dTemp);
	exportFile.getFile().writeLine();
}
//////////////////////////////////////////////////////////////////////////
//LandingBehindTakeoffIntersectionRunway
LandingBehindTakeoffIntersectionRunway::LandingBehindTakeoffIntersectionRunway(int nProjID)
:AircraftClassificationBasisApproachSeparationItem(nProjID,LandingBehindTakeoffOnIntersectingRunway)
{
}

LandingBehindTakeoffIntersectionRunway::~LandingBehindTakeoffIntersectionRunway(void)
{
}