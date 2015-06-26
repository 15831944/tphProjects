#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include "ConflictFirstInATaxiway.h"
#include "AirsideImportExportManager.h"
#include "..\common\AirportDatabase.h"

CFirstInATaxiway::CFirstInATaxiway()
:DBElement()
{
}

CFirstInATaxiway::~CFirstInATaxiway()
{
}

void CFirstInATaxiway::InitFromDBRecordset(CADORecordset& recordset)
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

void CFirstInATaxiway::GetInsertSQL(int nParentID,CString& strSQL) const
{
	char szFltTypeA[1024] = {0};
	m_fltTypeA.WriteSyntaxStringWithVersion(szFltTypeA);
	char szFltTypeB[1024] = {0};
	m_fltTypeB.WriteSyntaxStringWithVersion(szFltTypeB);
	strSQL.Format(_T("INSERT INTO IN_CONFLICT_FIRSTINATAXIWAY\
					 (CONFLICTID, FLTTYPEA, FLTTYPEB)\
					 VALUES (%d,'%s','%s')"),
					 nParentID,szFltTypeA,szFltTypeB);
}

void CFirstInATaxiway::GetUpdateSQL(CString& strSQL) const
{
	char szFltTypeA[1024] = {0};
	m_fltTypeA.WriteSyntaxStringWithVersion(szFltTypeA);
	char szFltTypeB[1024] = {0};
	m_fltTypeB.WriteSyntaxStringWithVersion(szFltTypeB);
	strSQL.Format(_T("UPDATE IN_CONFLICT_FIRSTINATAXIWAY\
					 SET FLTTYPEA ='%s', FLTTYPEB ='%s'\
					 WHERE (ID = %d)"),szFltTypeA,szFltTypeB,m_nID);

}

void CFirstInATaxiway::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_CONFLICT_FIRSTINATAXIWAY\
					 WHERE (ID = %d)"),m_nID);
}

void CFirstInATaxiway::ExportData(CAirsideExportFile& exportFile)
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
void CFirstInATaxiway::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);

	//importFile.getFile().getField(m_strFltTypeA.GetBuffer(1024),1024);
	//importFile.getFile().getField(m_strFltTypeB.GetBuffer(1024),1024);
	CString strFltTypeA;
	importFile.getFile().getField(strFltTypeA.GetBuffer(1024),1024);
	CString strFltTypeB;
	importFile.getFile().getField(strFltTypeB.GetBuffer(1024),1024);

	importFile.getFile().getLine();

	ImportSaveData(m_nConflictResolutionID,strFltTypeA,strFltTypeB);
}
void CFirstInATaxiway::ImportSaveData(int nParentID,CString strFltTypeA,CString strFltTypeB)
{
	CString strSQL;
	strSQL.Format(_T("INSERT INTO IN_CONFLICT_FIRSTINATAXIWAY\
					 (CONFLICTID, FLTTYPEA, FLTTYPEB)\
					 VALUES (%d,'%s','%s')"),
					 nParentID,strFltTypeA,strFltTypeB);
	if (strSQL.IsEmpty())
		return;

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
/////////////////////////////////////////////////////////////////////////////////////////
//
CFirstInATaxiwayList::CFirstInATaxiwayList()
:BaseClass()
{

}
CFirstInATaxiwayList::~CFirstInATaxiwayList()
{

}

void CFirstInATaxiwayList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, FLTTYPEA, FLTTYPEB\
					 FROM IN_CONFLICT_FIRSTINATAXIWAY\
					 WHERE (CONFLICTID = %d)"), nParentID);

}
void CFirstInATaxiwayList::ReadData(int nParentID)
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
		CFirstInATaxiway* element = new CFirstInATaxiway;
		element->SetAirportDB(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}
void CFirstInATaxiwayList::ExportCFirstInATaxiways(CAirsideExportFile& exportFile)
{
	size_t itemCount = GetElementCount();
	exportFile.getFile().writeInt(static_cast<int>(itemCount));
	for (size_t i =0; i < itemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);	
	}

}
void CFirstInATaxiwayList::ImportCFirstInATaxiways(CAirsideImportFile& importFile)
{
	int nFirstInATaxiway = 0;
	importFile.getFile().getInteger(nFirstInATaxiway);
	for (int i =0; i < nFirstInATaxiway; ++i)
	{
		CFirstInATaxiway  FirstInATaxiway;
		FirstInATaxiway.SetConflictResolutionID(m_nConflictResolutionID);
		FirstInATaxiway.ImportData(importFile);
	}
}

void CFirstInATaxiwayList::DoNotCall()
{
	CFirstInATaxiwayList list;
	list.AddNewItem(NULL);
}