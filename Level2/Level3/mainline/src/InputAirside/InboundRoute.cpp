#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\inboundroute.h"
#include "AirsideImportExportManager.h"
#include "ALTObjectGroup.h"

CInboundRoute::CInboundRoute(void)
{
}

CInboundRoute::~CInboundRoute(void)
{
}

void CInboundRoute::SetInboundRouteAssignmentID(int nInboundRouteAssignment)
{
	m_nInboundRouteAssignmentID = nInboundRouteAssignment;
}

int CInboundRoute::GetInboundRouteAssignmentID()
{
	return m_nInboundRouteAssignmentID;
}

void CInboundRoute::SetRunwayID(int nRunwayID)
{
	m_nRunwayID = nRunwayID;
}

int CInboundRoute::GetRunwayID()
{
	return m_nRunwayID;
}

void CInboundRoute::SetMarkingIndexID(int nMarkingIndexID)
{
	m_nMarkingIndexID = nMarkingIndexID;
}

int CInboundRoute::GetMarkingIndexID()
{
	return m_nMarkingIndexID;
}

//void CInboundRoute::SetTaxiwayID(int nTaxiwayID)
//{
//	m_nTaxiwayID = nTaxiwayID;
//}
//
//int CInboundRoute::GetTaxiwayID()
//{
//	return m_nTaxiwayID;
//}

void CInboundRoute::SetStandGroupID(int nStandGroupID)
{
	m_nStandGroupID = nStandGroupID;
}

int CInboundRoute::GetStandGroupID()
{
	return m_nStandGroupID;
}

void CInboundRoute::SetRunwayExitID(int nRunwayExitID)
{
	m_nRunwayExitID = nRunwayExitID;
}

int CInboundRoute::GetRunwayExitID()
{
	return m_nRunwayExitID;
}

void CInboundRoute::SetRunwayAllExitFlag(BOOL bRunwayAllExitFlag)
{
	m_bRunwayExitAll = bRunwayAllExitFlag;
}

BOOL CInboundRoute::GetRunwayAllExitFlag()
{
	return m_bRunwayExitAll;
}

void CInboundRoute::SaveData(int nID)
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(strSQL);

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

	for (std::vector<CInboundRouteHead *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	for (std::vector<CInboundRouteHead *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CInboundRoute::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"), m_nID);
    recordset.GetFieldValue(_T("INBOUNDROUTEASSIGNMENTID"), m_nInboundRouteAssignmentID);
	recordset.GetFieldValue(_T("RUNWAYID"), m_nRunwayID);
	recordset.GetFieldValue(_T("MARKINDEX"), m_nMarkingIndexID);
	//recordset.GetFieldValue(_T("TAXIWAYID"), m_nTaxiwayID);
	recordset.GetFieldValue(_T("STANDGROUPID"), m_nStandGroupID);
	recordset.GetFieldValue(_T("RUNWAYEXITID"), m_nRunwayExitID);

	int nRunwayExitAllFalg;
	recordset.GetFieldValue(_T("RUNWAYALLEXIT"), nRunwayExitAllFalg);
	m_bRunwayExitAll = (BOOL)nRunwayExitAllFalg;


	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT * \
							FROM IN_INBOUNDROUTE_DATA \
							WHERE (INBOUNDROUTEID = %d AND PARENTID = -1)"),
							m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CInboundRouteHead *pInboundRouteHead = new CInboundRouteHead();
		pInboundRouteHead->InitFromDBRecordset(adoRecordset);

		AddNewItem(pInboundRouteHead);

		adoRecordset.MoveNextData();
	}
}

void CInboundRoute::GetInsertSQL(CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_INBOUNDROUTE\
					 (INBOUNDROUTEASSIGNMENTID, RUNWAYID, MARKINDEX, STANDGROUPID, \
					  RUNWAYEXITID, RUNWAYALLEXIT) \
					 VALUES (%d, %d, %d, %d, %d, %d)"),\
					 m_nInboundRouteAssignmentID,
					 m_nRunwayID,
					 m_nMarkingIndexID,
					 m_nStandGroupID,
					 m_nRunwayExitID,
					 (int)m_bRunwayExitAll);
}

void CInboundRoute::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_INBOUNDROUTE\
					 SET INBOUNDROUTEASSIGNMENTID = %d, \
					     RUNWAYID = %d, \
                         MARKINDEX = %d, \
                         STANDGROUPID = %d, \
						 RUNWAYEXITID = %d, \
						 RUNWAYALLEXIT = %d \
					 WHERE (ID =%d)"),
					 m_nInboundRouteAssignmentID,
					 m_nRunwayID,
					 m_nMarkingIndexID,
					 m_nStandGroupID,
					 m_nRunwayExitID,
					 (int)m_bRunwayExitAll,
					 m_nID);
}

void CInboundRoute::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_INBOUNDROUTE\
					 WHERE (ID = %d)"),m_nID);
}

void CInboundRoute::DeleteData()
{
	//delete content
	for (std::vector<CInboundRouteHead *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteData();
		m_deleteddataList.push_back(*iter);
	}

	m_dataList.clear();
}

void CInboundRoute::DeleteDataFromDB()
{
	for (std::vector<CInboundRouteHead *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();

	CString strSQL;
	GetDeleteSQL(strSQL);
	if (strSQL.IsEmpty())
		return;

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CInboundRoute::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nInboundRouteAssignmentID);
	exportFile.getFile().writeInt(m_nRunwayID);
	exportFile.getFile().writeInt(m_nMarkingIndexID);
	exportFile.getFile().writeInt(m_nRunwayExitID);
	//exportFile.getFile().writeInt(m_nStandGroupID);
	ALTObjectGroup altObjGroup;
	altObjGroup.ReadData(m_nStandGroupID);
	exportFile.getFile().writeInt((int)altObjGroup.getType());
	ALTObjectID altObjID = altObjGroup.getName();
	exportFile.getFile().writeField(altObjID.at(0).c_str());
	exportFile.getFile().writeField(altObjID.at(1).c_str());
	exportFile.getFile().writeField(altObjID.at(2).c_str());
	exportFile.getFile().writeField(altObjID.at(3).c_str());
	
	exportFile.getFile().writeInt((int)m_bRunwayExitAll);

	exportFile.getFile().writeInt((int)GetElementCount());
	exportFile.getFile().writeLine();

	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CInboundRoute::ImportData(CAirsideImportFile& importFile)
{
	int nOldID;
	importFile.getFile().getInteger(nOldID);
	int nOldInboundrouteAssignmentID;
	importFile.getFile().getInteger(nOldInboundrouteAssignmentID);

	importFile.getFile().getInteger(m_nRunwayID);
	m_nRunwayID = importFile.GetObjectNewID(m_nRunwayID);
	importFile.getFile().getInteger(m_nMarkingIndexID);
	importFile.getFile().getInteger(m_nRunwayExitID);
	m_nRunwayExitID = importFile.GetRunwayExitIndexMap(m_nRunwayExitID);

	//stand group
	int standGroupType;
	importFile.getFile().getInteger(standGroupType);
	CString strFirst;
	importFile.getFile().getField(strFirst.GetBuffer(1024), 1024);
	CString strSecond;
	importFile.getFile().getField(strSecond.GetBuffer(1024), 1024);
	CString strThird;
	importFile.getFile().getField(strThird.GetBuffer(1024), 1024);
	CString strFour;
	importFile.getFile().getField(strFour.GetBuffer(1024), 1024);
	CString standGroupName;
	standGroupName.Format("%s-%s-%s-%s", strFirst, strSecond,strThird, strFour);
	ALTObjectID altObjName((LPCSTR)standGroupName);
	ALTObjectGroup standGroup;
	standGroup.setType((ALTOBJECT_TYPE)standGroupType);
	standGroup.setName(altObjName);
	m_nStandGroupID = standGroup.InsertData(importFile.getNewProjectID());

	int nRunwayExitAll;
	importFile.getFile().getInteger(nRunwayExitAll);
	m_bRunwayExitAll = (BOOL)nRunwayExitAll;

	int nChildCount;
	importFile.getFile().getInteger(nChildCount);
	importFile.getFile().getLine();

	CString strSQL = _T("");
	GetInsertSQL(strSQL);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	for (int i=0; i<nChildCount; i++)
	{
		CInboundRouteItem inboundRouteItem;
		inboundRouteItem.SetInboundRouteID(m_nID);
		inboundRouteItem.ImportData(importFile);		
	}
}