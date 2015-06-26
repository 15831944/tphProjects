#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\inboundrouteassignment.h"
#include "AirsideImportExportManager.h"

CInboundRouteAssignment::CInboundRouteAssignment(void)
: m_nRouteAssignmentStatus(ROUTEASSIGNMENT_SHORTPATHTOSTAND),TimeRangeID(-1)
{
}

CInboundRouteAssignment::~CInboundRouteAssignment(void)
{
}

void CInboundRouteAssignment::SetRouteAssignmentStatus(ROUTEASSIGNMENTSTATUS raStatus)
{
	m_nRouteAssignmentStatus = raStatus;
}

ROUTEASSIGNMENTSTATUS CInboundRouteAssignment::GetRouteAssignmentStatus()
{
	return m_nRouteAssignmentStatus;
}

void CInboundRouteAssignment::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, ROUTEASSIGNMENT \
					 FROM IN_INBOUNDROUTEASSIGNMENT \
					 WHERE PROJID = %d and TIMERANGEID = %d"), nProjectID,TimeRangeID);
}

void CInboundRouteAssignment::ReadData(int nProjectID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nProjectID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		InitFromDBRecordset(adoRecordset);
		adoRecordset.MoveNextData();
	}
}
void CInboundRouteAssignment::DeleteData()
{
	CString SQL ;
	GetDeleteSQL(SQL) ;
	SQL.Trim() ;
	if(SQL.IsEmpty())
		return ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL);
		DelAllItemFromDB() ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
	}

}
void CInboundRouteAssignment::InitFromDBRecordset(CADORecordset& recordset)
{
	int nRouteAssignmentStatus;

	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("ROUTEASSIGNMENT"), nRouteAssignmentStatus);
	m_nRouteAssignmentStatus = (ROUTEASSIGNMENTSTATUS)(nRouteAssignmentStatus);

	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT ID, INBOUNDROUTEASSIGNMENTID, RUNWAYID, MARKINDEX, TAXIWAYID, STANDGROUPID, RUNWAYEXITID, \
					 RUNWAYALLEXIT \
					 FROM IN_INBOUNDROUTE \
					 WHERE (INBOUNDROUTEASSIGNMENTID = %d)"),
					 m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CInboundRoute *pInboundRoute = new CInboundRoute();
		pInboundRoute->InitFromDBRecordset(adoRecordset);

		AddNewItem(pInboundRoute);

		adoRecordset.MoveNextData();
	}
}

void CInboundRouteAssignment::GetInsertSQL(int nProjID, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_INBOUNDROUTEASSIGNMENT\
					 (PROJID, ROUTEASSIGNMENT,TIMERANGEID)\
					 VALUES (%d, %d,%d)"),\
					 nProjID,
					 (int)m_nRouteAssignmentStatus,
					 TimeRangeID);
}

void CInboundRouteAssignment::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_INBOUNDROUTEASSIGNMENT\
					 SET ROUTEASSIGNMENT = %d \
					 WHERE (ID =%d and TIMERANGEID =%d)"),\
					 (int)m_nRouteAssignmentStatus,
					 m_nID,
					 TimeRangeID);
}

void CInboundRouteAssignment::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_INBOUNDROUTEASSIGNMENT\
					 WHERE (ID = %d and TIMERANGEID = %d)"),m_nID,TimeRangeID);
}

void CInboundRouteAssignment::SaveData(int nProjID)
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(nProjID, strSQL);

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

	for (std::vector<CInboundRoute *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetInboundRouteAssignmentID(m_nID);
		(*iter)->SaveData(m_nID);
	}

	for (std::vector<CInboundRoute *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CInboundRouteAssignment::DeleteItemFromIndex(size_t nIndex)
{
	if(nIndex < GetElementCount())
	{
		CInboundRoute *pInboundRoute = GetItem(nIndex);
		pInboundRoute->DeleteData();

		m_deleteddataList.push_back(*(m_dataList.begin() + nIndex));
		m_dataList.erase(m_dataList.begin() + nIndex);
	}
}
void CInboundRouteAssignment::DelAllItemFromDB()
{
   std::vector<CInboundRoute *>::iterator iter = m_dataList.begin();
   for( ; iter != m_dataList.end() ;++iter)
   {
	   (*iter)->DeleteDataFromDB() ;
   }
   for (std::vector<CInboundRoute *>::iterator iter = m_deleteddataList.begin();
	   iter != m_deleteddataList.end(); ++iter)
   {
	   (*iter)->DeleteDataFromDB();
   }
m_dataList.clear() ;
   	m_deleteddataList.clear();
}
void CInboundRouteAssignment::ExportInboundRouteAssignment(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("InboundRouteAssignmentID,");
	exportFile.getFile().writeField("InboundRouteAssignmentStatus,");
	exportFile.getFile().writeField("ProjID,");

	exportFile.getFile().writeField("RunwayID,");
	exportFile.getFile().writeField("RunwayMarkingID,");
	exportFile.getFile().writeField("RunwayExitID,");
	exportFile.getFile().writeField("StandGroupID,");
	exportFile.getFile().writeField("RunwayExitAllFlag,");

	exportFile.getFile().writeField("Level");
	exportFile.getFile().writeField("ParentID");
	exportFile.getFile().writeField("InboundRouteID");
	exportFile.getFile().writeField("AltObjID");
	exportFile.getFile().writeField("IntersectNodeID");
	exportFile.getFile().writeField("RouteItemName");

	exportFile.getFile().writeLine();

	CInboundRouteAssignment inboundRouteAssignment;
	inboundRouteAssignment.ReadData(exportFile.GetProjectID());
	inboundRouteAssignment.ExportData(exportFile);

	exportFile.getFile().endFile();
}

void CInboundRouteAssignment::ImportInboundRouteAssignment(CAirsideImportFile& importFile)
{
	CInboundRouteAssignment inboundRouteAssignment;
	inboundRouteAssignment.ImportData(importFile);
}

void CInboundRouteAssignment::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	exportFile.getFile().writeInt((int)m_nRouteAssignmentStatus);
	exportFile.getFile().writeInt((int)GetElementCount());
	exportFile.getFile().writeLine();

	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CInboundRouteAssignment::ImportData(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		int nOldID;
		importFile.getFile().getInteger(nOldID);
		int nOldProjID;
		importFile.getFile().getInteger(nOldProjID);

		int nRouteAssignmentStatus;
		importFile.getFile().getInteger(nRouteAssignmentStatus);
		m_nRouteAssignmentStatus = (ROUTEASSIGNMENTSTATUS)nRouteAssignmentStatus;

		int nChildCount;
		importFile.getFile().getInteger(nChildCount);
		importFile.getFile().getLine();

		CString strSQL = _T("");
		GetInsertSQL(importFile.getNewProjectID(), strSQL);

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

		for (int i=0; i<nChildCount; i++)
		{
			CInboundRoute inboundRoute;
			inboundRoute.SetInboundRouteAssignmentID(m_nID);
			inboundRoute.ImportData(importFile);
		}
	}
}