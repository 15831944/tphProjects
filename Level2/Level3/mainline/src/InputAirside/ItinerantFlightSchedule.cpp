#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include "ItinerantFlightSchedule.h"
#include "AirsideImportExportManager.h"
#include "..\common\AirportDatabase.h"
#include "common\ArrFlightOperation.h"
#include "common\DepFlightOperation.h"
#include "ALTObject.h"
#include "ALTObjectGroup.h"

//------------------------------------------------------------------
//ItinerantFlightScheduleItem
ItinerantFlightScheduleItem::ItinerantFlightScheduleItem()
:m_nEntryID(-1)
,m_nExitID(-1)
,m_nEntryFlag(0)
,m_nExitFlag(0)
,m_pAirportDB(NULL)
,m_bEnRoute(FALSE)
{
	serviceTime.SetMinute(30);
}

ItinerantFlightScheduleItem::~ItinerantFlightScheduleItem()
{

}

void ItinerantFlightScheduleItem::InitFromDBRecordset(CADORecordset& recordset)
{
	CString strID = _T("");
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("NAME"),strID);
	m_Airline = strID;

	recordset.GetFieldValue(_T("ENTRYID"),m_nEntryID);
	recordset.GetFieldValue(_T("ENTRYFLAG"),m_nEntryFlag);
	recordset.GetFieldValue(_T("EXITID"),m_nExitID);
	recordset.GetFieldValue(_T("EXITFLAG"),m_nExitFlag);

	strID = _T("");
	recordset.GetFieldValue(_T("ARRID"),strID);
	m_pArrFlightOp->SetArrFlightID(strID);

	strID = _T("");
	recordset.GetFieldValue(_T("DEPID"),strID);
	m_pDepFlightOp->SetDepFlightID(strID);

	long nArrTime = 0;
	recordset.GetFieldValue(_T("ARRTIME"),nArrTime);
	m_pArrFlightOp->SetArrTime(nArrTime);

	long nDepTime = 0;
	recordset.GetFieldValue(_T("DEPTIME"),nDepTime);
	m_pDepFlightOp->SetDepTime(nDepTime);

	strID = _T("");
	recordset.GetFieldValue(_T("ARRSTAND"),strID);
	ALTObjectID arrObjectID(strID);
	m_pArrFlightOp->SetArrStand(arrObjectID);

	strID = _T("");
	recordset.GetFieldValue(_T("DEPSTAND"),strID);
	ALTObjectID depObjectID(strID);
	m_pDepFlightOp->SetDepStand(depObjectID);

	strID = _T("");
	recordset.GetFieldValue(_T("ACTYPE"),strID);
	m_AcType = strID;

	strID = _T("");
	recordset.GetFieldValue(_T("ORIGIN"),strID);
	m_pArrFlightOp->SetOriginAirport(strID);
	
	strID = _T("");
	recordset.GetFieldValue(_T("DEST"),strID);
	m_pDepFlightOp->SetDestinationAirport(strID);

	int bEnRoute = 0;
	recordset.GetFieldValue(_T("ENROUTE"),bEnRoute);
	m_bEnRoute = (BOOL)bEnRoute;
}

void ItinerantFlightScheduleItem::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_ITINERANTFLIGHT_SCHEDULE\
					 (PROJID, NAME, ENTRYID,ENTRYFLAG, EXITID, EXITFLAG, ARRTIME,DEPTIME,ARRSTAND,DEPSTAND,ARRID,DEPID,ACTYPE,ORIGIN,DEST,ENROUTE)\
					 VALUES (%d,'%s',%d,%d,%d,%d,%d,%d,'%s','%s','%s','%s','%s','%s','%s',%d)"),
					 nParentID,m_Airline.GetValue(),m_nEntryID,m_nEntryFlag,m_nExitID,m_nExitFlag
					 ,m_pArrFlightOp->GetArrTime(),m_pDepFlightOp->GetDepTime(),m_pArrFlightOp->GetArrStand().GetIDString()\
					 ,m_pDepFlightOp->GetDepStand().GetIDString()\
					 ,m_pArrFlightOp->GetArrFlightID().GetValue()\
					 ,m_pDepFlightOp->GetDepFlightID().GetValue(),m_AcType.GetValue(),m_pArrFlightOp->GetOriginAirport().GetValue()\
					 ,m_pDepFlightOp->GetDestinationAirport().GetValue(),m_bEnRoute);
}

void ItinerantFlightScheduleItem::GetUpdateSQL(CString& strSQL) const
{
//CAN NOT UPDATE NOW, TODO
	strSQL.Format(_T("UPDATE IN_ITINERANTFLIGHT_SCHEDULE\
					 SET NAME = '%s',ENTRYID = %d,ENTRYFLAG=%d,EXITID=%d,EXITFLAG=%d,ARRTIME=%d,DEPTIME=%d,\
					 ARRSTAND='%s',DEPSTAND='%s',ARRID='%s',DEPID='%s',ACTYPE='%s',ORIGIN='%s',DEST='%s',ENROUTE=%d\
					 WHERE (ID = %d)"),m_Airline.GetValue(),m_nEntryID,m_nEntryFlag,m_nExitID,m_nExitFlag
					 ,m_pArrFlightOp->GetArrTime(),m_pDepFlightOp->GetDepTime(),m_pArrFlightOp->GetArrStand().GetIDString()\
					 ,m_pDepFlightOp->GetDepStand().GetIDString()\
					 ,m_pArrFlightOp->GetArrFlightID().GetValue()\
					 ,m_pDepFlightOp->GetDepFlightID().GetValue(),m_AcType.GetValue(),m_pArrFlightOp->GetOriginAirport().GetValue()\
					 ,m_pDepFlightOp->GetDestinationAirport().GetValue(),m_bEnRoute,m_nID);
}

void ItinerantFlightScheduleItem::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_ITINERANTFLIGHT_SCHEDULE\
					 WHERE (ID = %d)"),m_nID);
}

void ItinerantFlightScheduleItem::ExportData(CAirsideExportFile& exportFile)
{
// 	exportFile.getFile().writeInt(m_nID);
// 	exportFile.getFile().writeInt(exportFile.GetProjectID());
// 	exportFile.getFile().writeField(m_strName);
// 	char szFltType[1024] = {0};
// 	m_fltType.WriteSyntaxString(szFltType);
// 	exportFile.getFile().writeField(szFltType);
// 
// 	exportFile.getFile().writeInt(m_nEntryID);
// 	exportFile.getFile().writeInt(m_nEntryFlag);
// 
// 	exportFile.getFile().writeInt(m_nExitID);
// 	exportFile.getFile().writeInt(m_nExitFlag);
// 	
// 	
// 	exportFile.getFile().writeInt(m_arrTime.asSeconds());
// 	exportFile.getFile().writeInt(m_nFltID);
}

void ItinerantFlightScheduleItem::ImportDataSave(int nNewProjID, CString& strFltType)
{
	CString strSQL;
	strSQL.Format(_T("INSERT INTO IN_ITINERANTFLIGHT_SCHEDULE\
					 (PROJID, NAME, ENTRYID,ENTRYFLAG, EXITID, EXITFLAG, ARRTIME,DEPTIME,ARRSTAND,DEPSTAND,FLIGHTID,ACTYPE,ORIGIN,DEST)\
					 VALUES (%d,'%s',%d,%d,%d,%d,%d,'%s','%s','%s','%s','%s','%s','%s')"),
					 nNewProjID,m_Airline.GetValue(),m_nEntryID,m_nEntryFlag,m_nExitID,m_nExitFlag
					 ,m_pArrFlightOp->GetArrTime(),m_pDepFlightOp->GetDepTime(),m_pArrFlightOp->GetArrStand().GetIDString()\
					 ,m_pDepFlightOp->GetDepStand().GetIDString(),m_pArrFlightOp->GetArrFlightID().GetValue()\
					 ,m_pDepFlightOp->GetDepFlightID().GetValue(),m_AcType.GetValue(),m_pArrFlightOp->GetOriginAirport().GetValue()\
					 ,m_pDepFlightOp->GetDestinationAirport().GetValue(),m_bEnRoute);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void ItinerantFlightScheduleItem::ImportData(CAirsideImportFile& importFile)
{
// 	int nOldID = -1;
// 	importFile.getFile().getInteger(nOldID);
// 	int nOldProjID = -1;
// 	importFile.getFile().getInteger(nOldProjID);
// 
// 	importFile.getFile().getField(m_strName.GetBuffer(1024),1024);
// 	CString strFltType;
// 	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);
// 
// 
// 	int nOldEntryID = -1;
// 	importFile.getFile().getInteger(nOldEntryID);
// 	m_nEntryID = importFile.GetObjectNewID(nOldEntryID);
// 
// 	importFile.getFile().getInteger(m_nEntryFlag);
// 
// 	int nOldExitID = -1;
// 	importFile.getFile().getInteger(nOldExitID);
// 	m_nExitID = importFile.GetObjectNewID(nOldExitID);
// 
// 	importFile.getFile().getInteger(m_nExitFlag);
// 
// 
// 	long lArrTime = 0L;
// 	importFile.getFile().getInteger(lArrTime);
// 	m_arrTime.set(lArrTime);
// 
// 	int nFlightID = -1;
// 	importFile.getFile().getInteger(nFlightID);
// 	m_nFltID = nFlightID;
// 
// 	ImportDataSave(importFile.getNewProjectID(), strFltType);

}

//------------------------------------------------------------------
//ItinerantFlightSchedule
ItinerantFlightSchedule::ItinerantFlightSchedule(void)
:m_pAirportDB(NULL)
{
}

ItinerantFlightSchedule::~ItinerantFlightSchedule(void)
{
}

void ItinerantFlightSchedule::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT *\
					 FROM IN_ITINERANTFLIGHT_SCHEDULE\
					 WHERE (PROJID = %d)"), nParentID);
}

void ItinerantFlightSchedule::ReadData(int nParentID)
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
		ItinerantFlightScheduleItem* element = new ItinerantFlightScheduleItem;
		element->SetAirportDB(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}

void ItinerantFlightSchedule::ExportItinerantFlightSchedule(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
	exportFile.getFile().writeField(_T("ID,PROJID, NAME, FLTTYPE, ENTRYID,ENTRYFLAG, EXITID, EXITFLAG, ARRTIME,FLIGHTID"));
	exportFile.getFile().writeLine();
	ItinerantFlightSchedule ItinerantFltSchedule;
	ItinerantFltSchedule.SetAirportDB(pAirportDB);

	ItinerantFltSchedule.ReadData( exportFile.GetProjectID());
	ItinerantFltSchedule.ExportData( exportFile );

	exportFile.getFile().endFile();
}

void ItinerantFlightSchedule::ImportItinerantFlightSchedule(CAirsideImportFile& importFile)
{
	ItinerantFlightSchedule ItinerantFltSchedule;
	ItinerantFltSchedule.ImportData(importFile);

}

void ItinerantFlightSchedule::DoNotCall()
{
	ItinerantFlightSchedule sch;
	sch.AddNewItem(NULL);
}












