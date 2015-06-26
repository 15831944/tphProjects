#include "stdafx.h"
#include "TakeoffQueuesItem.h"
#include "AirsideImportExportManager.h"

TakeoffQueuesItem::TakeoffQueuesItem()
{
	m_nID = -1;
	m_nUniID = -1;
}

TakeoffQueuesItem::~TakeoffQueuesItem()
{

}
void TakeoffQueuesItem::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
//	adoRecordset.GetFieldValue(_T("TAKEOFFQUEUEID"),m_nTakeoffQueueID);
	adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);
	adoRecordset.GetFieldValue(_T("TAXIWAYID"),m_nTaxiwayID);
	adoRecordset.GetFieldValue(_T("UNIID"),m_nUniID);
	adoRecordset.GetFieldValue(_T("IDX"),m_idx);

}
void TakeoffQueuesItem::DeleteData() 
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_TAKEOFFQUEUENODE\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void TakeoffQueuesItem::SaveData(int nTakeoffQueueID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFQUEUENODE\
					 (TAKEOFFQUEUEID, PARENTID,TAXIWAYID, UNIID, IDX)\
					 VALUES (%d,%d,%d,%d,%d)"),nTakeoffQueueID,m_nParentID,m_nTaxiwayID, m_nUniID, m_idx);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void TakeoffQueuesItem::UpdateData(int nTakeoffQueueID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_TAKEOFFQUEUENODE\
					 SET TAKEOFFQUEUEID =%d, PARENTID =%d, TAXIWAYID =%d, UNIID=%d, IDX=%d\
					 WHERE (ID = %d)"),nTakeoffQueueID,m_nParentID,m_nTaxiwayID, m_nUniID, m_idx, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void TakeoffQueuesItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nParentID);
	exportFile.getFile().writeInt(m_nTaxiwayID);
	exportFile.getFile().writeInt(m_nUniID);
	exportFile.getFile().writeInt(m_idx);
	exportFile.getFile().writeLine();

	/*
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAKEOFFQUEUENODE\
	(TAKEOFFQUEUEID, PARENTID,TAXIWAYID, UNIID, IDX)\
	VALUES (%d,%d,%d,%d,%d)"),nTakeoffQueueID,m_nParentID,m_nTaxiwayID, m_nUniID, m_idx);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	*/
}

void TakeoffQueuesItem::ImportData(CAirsideImportFile& importFile,int nTakeoffQueueID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	importFile.getFile().getInteger(m_nParentID);

	int nOldTaixayID = -1;
	importFile.getFile().getInteger(nOldTaixayID);
	m_nTaxiwayID = importFile.GetObjectNewID(nOldTaixayID);

	importFile.getFile().getInteger(m_nUniID);
	importFile.getFile().getInteger(m_idx);
	SaveData(nTakeoffQueueID);
	importFile.getFile().getLine();
}