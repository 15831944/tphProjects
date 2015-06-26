#include "stdafx.h"
#include "VehicleTagItem.h"
#include "..\Database\DBElementCollection_Impl.h"

//CVehicleTagItem::CVehicleTagItem(void)
//{
//	m_nID = -1;
//	m_nParentID = -1;
//	m_nProjID = -1;
//	m_bShowTags = FALSE;
//	m_dwTagInfo = 0x0;
//}

CVehicleTagItem::CVehicleTagItem(int nProjID):m_nProjID(nProjID)
{
	m_nID = -1;
	m_bShowTags = FALSE;
	m_dwTagInfo = 0x0;
}

CVehicleTagItem::~CVehicleTagItem(void)
{
}

void CVehicleTagItem::ReadData(int nID)
{
	if(nID == -1)return;
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM VEHICLETAGITEM WHERE (ID = %d);"),nID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	while (!adoRecordset.IsEOF()) {
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);

		int iValue = -1;
		adoRecordset.GetFieldValue(_T("VEHICLEID"),iValue);
		m_vehicleItem.ReadData(iValue);

		adoRecordset.GetFieldValue(_T("SHOWTAGFLAG"),iValue);
		m_bShowTags = (BOOL)iValue;

		adoRecordset.GetFieldValue(_T("VEHICLETAGFLAG"),iValue);
		m_dwTagInfo = iValue;

		adoRecordset.MoveNextData();
	}
}

void CVehicleTagItem::SaveData(int nParentID)
{
	CString strSQL = _T("");
	if(m_nID  == -1)
	{
		strSQL.Format(_T("INSERT INTO VEHICLETAGITEM (PROJID,PARENTID,VEHICLEID,SHOWTAGFLAG,VEHICLETAGFLAG) VALUES (%d,%d,%d,%d,%d);"),m_nProjID,m_nParentID = nParentID, m_vehicleItem.GetID(),m_bShowTags,m_dwTagInfo);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		strSQL.Format(_T("UPDATE VEHICLETAGITEM SET PROJID = %d,PARENTID = %d,VEHICLEID = %d,SHOWTAGFLAG = %d,VEHICLETAGFLAG = %d WHERE (ID = %d);"),m_nProjID,m_nParentID = nParentID,m_vehicleItem.GetID(),m_bShowTags,m_dwTagInfo, m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CVehicleTagItem::DeleteData(void)
{
	if(m_nID == -1)return;
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM VEHICLETAGITEM WHERE (ID = %d);"), m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);	
	m_nID = -1;
	m_nParentID = -1;
	m_nProjID = -1;
}

void CVehicleTagItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_nParentID);
	exportFile.getFile().writeInt(m_vehicleItem.GetID());
	exportFile.getFile().writeInt((int)m_bShowTags);
	exportFile.getFile().writeInt((int)m_dwTagInfo);
	exportFile.getFile().writeLine();
}

void CVehicleTagItem::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nID); 
	m_nID = -1;
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	importFile.getFile().getInteger(m_nParentID);
	m_nParentID = -1;

	int iValue = -1;
	importFile.getFile().getInteger(iValue);
	iValue = importFile.GetVehicleDataSpecificationNewID(iValue);
	m_vehicleItem.ReadData(iValue);

	importFile.getFile().getInteger(iValue);
	m_bShowTags = (BOOL)iValue;

	importFile.getFile().getInteger(iValue);
	m_dwTagInfo = (DWORD)iValue;

	importFile.getFile().getLine();
}

void CVehicleTagItem::GetVehicleTagItemList(int nParentID,std::vector<int> & vrVehicleTagItemID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM VEHICLETAGITEM WHERE (PARENTID = %d);"),nParentID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	vrVehicleTagItemID.clear();
	while (!adoRecordset.IsEOF()) {
		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID); 
		vrVehicleTagItemID.push_back(nID);
		adoRecordset.MoveNextData();
	} 
} 

int CVehicleTagItem::GetParentID(void)
{
	return (m_nParentID);
}

int CVehicleTagItem::GetID(void)
{
	return (m_nID);
}