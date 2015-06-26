#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include "ConflictAtIntersectionOnRight.h"
#include "AirsideImportExportManager.h"
#include "..\common\AirportDatabase.h"

CAtIntersectionOnRight::CAtIntersectionOnRight()
:DBElement()
{
}

CAtIntersectionOnRight::~CAtIntersectionOnRight()
{
}

void CAtIntersectionOnRight::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	//recordset.GetFieldValue(_T("FLTTYPEA"),m_strFltTypeA);
	//recordset.GetFieldValue(_T("FLTTYPEB"),m_strFltTypeB);

	CString strFltTypeA;
	recordset.GetFieldValue(_T("FLTTYPEA"),strFltTypeA);
	m_fltTypeA.SetAirportDB(m_pAirportDB);
	m_fltTypeA.setConstraintWithVersion(strFltTypeA);

	CString strFltTypeB;
	recordset.GetFieldValue(_T("FLTTYPEB"),strFltTypeB);
	m_fltTypeB.SetAirportDB(m_pAirportDB);
	m_fltTypeB.setConstraintWithVersion(strFltTypeB);
}

void CAtIntersectionOnRight::GetInsertSQL(int nParentID,CString& strSQL) const
{
	char szFltTypeA[1024] = {0};
	m_fltTypeA.WriteSyntaxStringWithVersion(szFltTypeA);
	char szFltTypeB[1024] = {0};
	m_fltTypeB.WriteSyntaxStringWithVersion(szFltTypeB);
	strSQL.Format(_T("INSERT INTO IN_CONFLICT_ATINTERSECTIONONRIGHT\
					 (CONFLICTID, FLTTYPEA, FLTTYPEB)\
					 VALUES (%d,'%s','%s')"),
					 nParentID,szFltTypeA,szFltTypeB);
}

void CAtIntersectionOnRight::GetUpdateSQL(CString& strSQL) const
{
	char szFltTypeA[1024] = {0};
	m_fltTypeA.WriteSyntaxStringWithVersion(szFltTypeA);
	char szFltTypeB[1024] = {0};
	m_fltTypeB.WriteSyntaxStringWithVersion(szFltTypeB);
	strSQL.Format(_T("UPDATE IN_CONFLICT_ATINTERSECTIONONRIGHT\
					 SET FLTTYPEA ='%s', FLTTYPEB ='%s'\
					 WHERE (ID = %d)"),szFltTypeA,szFltTypeB,m_nID);

}

void CAtIntersectionOnRight::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_CONFLICT_ATINTERSECTIONONRIGHT\
					 WHERE (ID = %d)"),m_nID);
}

void CAtIntersectionOnRight::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	char szFltTypeA[1024] = {0};
	m_fltTypeA.WriteSyntaxStringWithVersion(szFltTypeA);
	char szFltTypeB[1024] = {0};
	m_fltTypeB.WriteSyntaxStringWithVersion(szFltTypeB);
	exportFile.getFile().writeField(szFltTypeA);
	exportFile.getFile().writeField(szFltTypeB);
	exportFile.getFile().writeLine();
}
void CAtIntersectionOnRight::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);
	CString strFltTypeA;
	importFile.getFile().getField(strFltTypeA.GetBuffer(1024),1024);
	CString strFltTypeB;
	importFile.getFile().getField(strFltTypeB.GetBuffer(1024),1024);

	importFile.getFile().getLine();

	ImportSaveData(m_nConflictResolutionID,strFltTypeA,strFltTypeB);
}
void CAtIntersectionOnRight::ImportSaveData(int nParentID,CString strFltTypeA,CString strFltTypeB)
{
	CString strSQL;
	strSQL.Format(_T("INSERT INTO IN_CONFLICT_ATINTERSECTIONONRIGHT\
					 (CONFLICTID, FLTTYPEA, FLTTYPEB)\
					 VALUES (%d,'%s','%s')"),
					 nParentID,strFltTypeA,strFltTypeB);
	if (strSQL.IsEmpty())
		return;

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
/////////////////////////////////////////////////////////////////////////////////////////
//
CAtIntersectionOnRightList::CAtIntersectionOnRightList()
:BaseClass()
{

}
CAtIntersectionOnRightList::~CAtIntersectionOnRightList()
{
}

void CAtIntersectionOnRightList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, FLTTYPEA, FLTTYPEB\
					 FROM IN_CONFLICT_ATINTERSECTIONONRIGHT\
					 WHERE (CONFLICTID = %d)"), nParentID);

}
void CAtIntersectionOnRightList::ReadData(int nParentID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nParentID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CAtIntersectionOnRight* element = new CAtIntersectionOnRight;
		element->SetAirportDB(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}
void CAtIntersectionOnRightList::ExportCAtIntersectionOnRights(CAirsideExportFile& exportFile)
{
	size_t itemCount = GetElementCount();
	exportFile.getFile().writeInt(static_cast<int>(itemCount));
	for (size_t i =0; i < itemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);	
	}
}
void CAtIntersectionOnRightList::ImportCAtIntersectionOnRights(CAirsideImportFile& importFile)
{
	int nAtIntersectionOnRightCount = 0;
	importFile.getFile().getInteger(nAtIntersectionOnRightCount);

	for (int i =0; i < nAtIntersectionOnRightCount; ++i)
	{
		CAtIntersectionOnRight  AtIntersectionOnRight;
		AtIntersectionOnRight.SetConflictResolutionID(m_nConflictResolutionID);
		AtIntersectionOnRight.ImportData(importFile);
	}
}

void CAtIntersectionOnRightList::DoNotCall()
{
	CAtIntersectionOnRightList list;
	list.AddNewItem(NULL);
}