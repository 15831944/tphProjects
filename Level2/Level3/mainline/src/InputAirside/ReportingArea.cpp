#include "StdAfx.h"
#include ".\reportingarea.h"
#include "../common/ARCUnit.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "ALTObjectDisplayProp.h"
CReportingArea::CReportingArea(void)
{
	m_nPathID = -1;
}

CReportingArea::~CReportingArea(void)
{
	m_path.clear();
}

void CReportingArea::ReadObject(int nObjID)
{
	m_nObjID = nObjID;

	CADORecordset adoRecordset;
	long nRecordCount = -1;

	CADODatabase::ExecuteSQLStatement(ALTObject::GetSelectScript(nObjID),nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
		ALTObject::ReadObject(adoRecordset);

	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{	
		adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID );
		m_path.clear();
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
	}

}
int CReportingArea::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_REPORTINGAREA);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void CReportingArea::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void CReportingArea::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString CReportingArea::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO AS_REPORTINGAREA (OBJID,PATHID ) \
					 VALUES (%d,%d)"), \
					 nObjID,m_nPathID);

	return strSQL;
}
CString CReportingArea::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AS_REPORTINGAREA SET PATHID = %d \
					 WHERE OBJID = %d"),m_nPathID,nObjID);

	return strSQL;
}
CString CReportingArea::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AS_REPORTINGAREA WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString CReportingArea::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AS_REPORTINGAREA WHERE OBJID = %d"),
		nObjID);

	return strSQL;
}
const CPath2008& CReportingArea::GetPath()const
{
	return (m_path);
}


void CReportingArea::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}

void CReportingArea::ExportObject(CAirsideExportFile& exportFile)
{

}

void CReportingArea::ExportVehiclePoolParking(int nAirportID,CAirsideExportFile& exportFile)
{

}

void CReportingArea::ImportObject(CAirsideImportFile& importFile)
{

}

ALTObject * CReportingArea::NewCopy()
{
	CReportingArea * pArea  = new CReportingArea;
	*pArea = *this;
	return pArea;
}

ALTObjectDisplayProp* CReportingArea::NewDisplayProp()
{
	return new ReportingAreaDisplayProp();
}
