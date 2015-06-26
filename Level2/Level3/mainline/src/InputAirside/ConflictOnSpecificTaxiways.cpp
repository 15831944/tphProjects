#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include "ConflictOnSpecificTaxiways.h"
#include "AirsideImportExportManager.h"
#include "..\common\AirportDatabase.h"

COnSpecificTaxiways::COnSpecificTaxiways()
:DBElement()
,m_nTaxiwayID(-1)
{
}

COnSpecificTaxiways::~COnSpecificTaxiways()
{
}

void COnSpecificTaxiways::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	//recordset.GetFieldValue(_T("FLTTYPE"),m_strFltType);
	CString strFltType;
	recordset.GetFieldValue(_T("FLTTYPE"),strFltType);
	m_fltType.SetAirportDB(m_pAirportDB);
	m_fltType.setConstraintWithVersion(strFltType);

	recordset.GetFieldValue(_T("TAXIWAYID"),m_nTaxiwayID);
}

void COnSpecificTaxiways::GetInsertSQL(int nParentID,CString& strSQL) const
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format(_T("INSERT INTO IN_CONFLICT_ONSPECIFICTAXIWAYS\
					 (CONFLICTID, FLTTYPE, TAXIWAYID)\
					 VALUES (%d,'%s',%d)"),
					 nParentID,szFltType,m_nTaxiwayID);
}

void COnSpecificTaxiways::GetUpdateSQL(CString& strSQL) const
{
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format(_T("UPDATE IN_CONFLICT_ONSPECIFICTAXIWAYS\
					 SET FLTTYPE ='%s', TAXIWAYID =%d\
					 WHERE (ID = %d)"),szFltType,m_nTaxiwayID,m_nID);

}

void COnSpecificTaxiways::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_CONFLICT_ONSPECIFICTAXIWAYS\
					 WHERE (ID = %d)"),m_nID);
}

void COnSpecificTaxiways::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	exportFile.getFile().writeField(szFltType);
	exportFile.getFile().writeInt(m_nTaxiwayID);
	exportFile.getFile().writeLine();
}
void COnSpecificTaxiways::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);

	//importFile.getFile().getField(m_strFltType.GetBuffer(1024),1024);
	CString strFltType;
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

	int nTaxiwayID = -1;
	importFile.getFile().getInteger(nTaxiwayID);
	m_nTaxiwayID = importFile.GetObjectNewID(nTaxiwayID);

	importFile.getFile().getLine();

	ImportSaveData(m_nConflictResolutionID,strFltType);
}
void COnSpecificTaxiways::ImportSaveData(int nParentID,CString strFltType)
{
	CString strSQL;
	strSQL.Format(_T("INSERT INTO IN_CONFLICT_ONSPECIFICTAXIWAYS\
					 (CONFLICTID, FLTTYPE, TAXIWAYID)\
					 VALUES (%d,'%s',%d)"),
					 nParentID,strFltType,m_nTaxiwayID);
	if (strSQL.IsEmpty())
		return;

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
/////////////////////////////////////////////////////////////////////////////////////////
//
COnSpecificTaxiwaysList::COnSpecificTaxiwaysList()
:BaseClass()
{

}
COnSpecificTaxiwaysList::~COnSpecificTaxiwaysList()
{

}

void COnSpecificTaxiwaysList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, FLTTYPE, TAXIWAYID\
					 FROM IN_CONFLICT_ONSPECIFICTAXIWAYS\
					 WHERE (CONFLICTID = %d)"), nParentID);

}
void COnSpecificTaxiwaysList::ReadData(int nParentID)
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
		COnSpecificTaxiways* element = new COnSpecificTaxiways;
		element->SetAirportDB(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}
void COnSpecificTaxiwaysList::ExportCOnSpecificTaxiwayss(CAirsideExportFile& exportFile)
{
	size_t itemCount = GetElementCount();
	exportFile.getFile().writeInt(static_cast<int>(itemCount));
	for (size_t i =0; i < itemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);	
	}
}
void COnSpecificTaxiwaysList::ImportCOnSpecificTaxiwayss(CAirsideImportFile& importFile)
{
	int nOnSpecificTaxiwaysCount = 0;
	importFile.getFile().getInteger(nOnSpecificTaxiwaysCount);

	for (int i =0; i < nOnSpecificTaxiwaysCount; ++i)
	{
		COnSpecificTaxiways  OnSpecificTaxiways;
		OnSpecificTaxiways.SetConflictResolutionID(m_nConflictResolutionID);
		OnSpecificTaxiways.ImportData(importFile);
	}
}

void COnSpecificTaxiwaysList::DoNotCall()
{
	COnSpecificTaxiwaysList aList;
	aList.AddNewItem(NULL);
}