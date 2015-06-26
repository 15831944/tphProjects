#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include "DirectRouting.h"
#include "AirsideImportExportManager.h"


CDirectRouting::CDirectRouting()
:DBElement()
,m_nARWaypointStartID(-1)
,m_nARWaypointEndID(-1)
,m_nARWaypointMaxID(-1)
{
}

CDirectRouting::~CDirectRouting()
{
}

void CDirectRouting::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("ARWAYPOINTSTARTID"),m_nARWaypointStartID);
	//	recordset.GetFieldValue(_T("PROJID"),m_nProjectID);
	recordset.GetFieldValue(_T("ARWAYPOINTENDID"),m_nARWaypointEndID);
	recordset.GetFieldValue(_T("ARWAYPOINTMAXID"),m_nARWaypointMaxID);
}

void CDirectRouting::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_DIRECTROUTING\
					 (PROJID, ARWAYPOINTSTARTID, ARWAYPOINTENDID, ARWAYPOINTMAXID)\
					 VALUES (%d,%d,%d,%d)"),
					 nParentID,m_nARWaypointStartID,m_nARWaypointEndID,m_nARWaypointMaxID);
}

void CDirectRouting::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_DIRECTROUTING\
					 SET ARWAYPOINTSTARTID =%d, ARWAYPOINTENDID =%d, ARWAYPOINTMAXID =%d\
					 WHERE (ID = %d)"),m_nARWaypointStartID,m_nARWaypointEndID,m_nARWaypointMaxID,m_nID);

}

void CDirectRouting::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_DIRECTROUTING\
					 WHERE (ID = %d)"),m_nID);
}

void CDirectRouting::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	exportFile.getFile().writeInt(m_nARWaypointStartID);
	exportFile.getFile().writeInt(m_nARWaypointEndID);
	exportFile.getFile().writeInt(m_nARWaypointMaxID);
	exportFile.getFile().writeLine();
}
void CDirectRouting::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);

	int nARWaypointStartID = -1;
	importFile.getFile().getInteger(nARWaypointStartID);
	m_nARWaypointStartID = importFile.GetObjectNewID(nARWaypointStartID);
	
	int nARWaypointEndID = -1;
	importFile.getFile().getInteger(nARWaypointEndID);
	m_nARWaypointEndID = importFile.GetObjectNewID(nARWaypointEndID);

	int nARWaypointMaxID = -1;
	importFile.getFile().getInteger(nARWaypointMaxID);
	m_nARWaypointMaxID = importFile.GetObjectNewID(nARWaypointMaxID);

	importFile.getFile().getLine();

	SaveData(importFile.getNewProjectID());
}

/////////////////////////////////////////////////////////////////////////////////////////
//
CDirectRoutingList::CDirectRoutingList()
:BaseClass()
{

}
CDirectRoutingList::~CDirectRoutingList()
{

}

void CDirectRoutingList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, ARWAYPOINTSTARTID, ARWAYPOINTENDID, ARWAYPOINTMAXID\
					 FROM IN_DIRECTROUTING\
					 WHERE (PROJID = %d)"), nParentID);

}
void CDirectRoutingList::ExportDirectRoutings(CAirsideExportFile& exportFile)
{
	CDirectRoutingList CDirectRoutingList;
	CDirectRoutingList.ReadData(exportFile.GetProjectID());

	exportFile.getFile().writeField("DirectRouting");
	exportFile.getFile().writeLine();

	size_t itemCount = CDirectRoutingList.GetElementCount();

	for (size_t i =0; i < itemCount; ++i)
	{
		CDirectRoutingList.GetItem(i)->ExportData(exportFile);	
	}
	exportFile.getFile().endFile();
}
void CDirectRoutingList::ImportDirectRoutings(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CDirectRouting  CDirectRouting;
		CDirectRouting.ImportData(importFile);
	}
}

void CDirectRoutingList::DoNotCall()
{
	CDirectRoutingList list;
	list.AddNewItem(NULL);
}