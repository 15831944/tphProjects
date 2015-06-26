#include "StdAfx.h"
#include "StopSign.h"
#include "../common/ARCUnit.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "ALTObjectDisplayProp.h"

StopSign::StopSign(void)
{ 
	m_nPathID  = -1;
	m_dRotation = 0.0;
	m_nIntersection = -1;
	m_nStretchID = -1;
}
StopSign::~StopSign(void)
{
	m_path.clear();
}

void StopSign::ReadObject(int nObjID)
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
		adoRecordset.GetFieldValue(_T("ROTATION"),m_dRotation );
		adoRecordset.GetFieldValue(_T("INTERSECTIONNODEID"),m_nIntersection);
		adoRecordset.GetFieldValue(_T("STRETCHID"),m_nStretchID);
		m_path.clear();
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
	}
}
int StopSign::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_STOPSIGN);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);
	
	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void StopSign::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void StopSign::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);
	
	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString StopSign::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO STOPSIGN (OBJID,PATHID,ROTATION,INTERSECTIONNODEID,STRETCHID ) VALUES (%d,%d,%f,%d,%d)"),nObjID,m_nPathID,m_dRotation,m_nIntersection,m_nStretchID);

	return strSQL;
}
CString StopSign::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE STOPSIGN SET PATHID = %d , ROTATION = %f, INTERSECTIONNODEID = %d, STRETCHID = %d WHERE OBJID = %d"),m_nPathID,m_dRotation,m_nIntersection,m_nStretchID,nObjID);

	return strSQL;
}
CString StopSign::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM STOPSIGN WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString StopSign::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM STOPSIGN WHERE OBJID = %d"),
		nObjID);

	return strSQL;
}
const CPath2008& StopSign::GetPath()const
{
	return (m_path);
}
void StopSign::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}

const int StopSign::GetIntersectionNodeID()const
{
	return m_nIntersection;
}

void StopSign::SetIntersectionNodeID(const int nIntersection)
{
	m_nIntersection = nIntersection;
}

const int StopSign::GetStretchID()const
{
	return m_nStretchID;
}

void StopSign::SetStretchID(const int nStretchID)
{
	m_nStretchID = nStretchID;
}

double StopSign::GetRotation(void)const
{
	return (m_dRotation);
}

void StopSign::SetRotation(double dRotation)
{
	m_dRotation = dRotation;
}

void StopSign::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeInt(m_nPathID);
	exportFile.getFile().writeInt(m_nIntersection);
	exportFile.getFile().writeInt(m_nStretchID);

	//path
	int nPathCount = m_path.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i = 0; i < nPathCount; i++)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}
	exportFile.getFile().writeDouble(m_dRotation);

	exportFile.getFile().writeLine();
}

void StopSign::ExportStopSign(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vStopSignID;
	ALTAirport::GetStopSignsIDs(nAirportID,vStopSignID);
	int nStopSignCount = static_cast<int>(vStopSignID.size());
	for (int i =0; i < nStopSignCount;++i)
	{
		StopSign stopSignObj;
		stopSignObj.ReadObject(vStopSignID[i]);
		stopSignObj.ExportObject(exportFile);
	}
}

void StopSign::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);
	importFile.getFile().getInteger(m_nPathID);
	importFile.getFile().getInteger(m_nIntersection);
	importFile.getFile().getInteger(m_nStretchID);

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();
	for (int i = 0; i < nPointCount; i++)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}
	importFile.getFile().getFloat(m_dRotation);

	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);
}


ALTObject * StopSign::NewCopy()
{
	StopSign * pStopSign  = new StopSign;
	*pStopSign = *this;
	return pStopSign;
}

bool StopSign::CopyData(const  ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if(pObj->GetType() == GetType() )
	{
		StopSign * pOther = (StopSign*)pObj;
		m_path = pOther->GetPath();
		m_dRotation = pOther->GetRotation();
		m_nIntersection = pOther->GetIntersectionNodeID();
		m_nStretchID = pOther->GetStretchID();

		m_bLocked = pOther->GetLocked();
		return true;
	}
	return false;
}

const GUID& StopSign::getTypeGUID()
{
	// {3ABCE17A-0921-451e-AA20-118038037714}
	static const GUID name = 
	{ 0x3abce17a, 0x921, 0x451e, { 0xaa, 0x20, 0x11, 0x80, 0x38, 0x3, 0x77, 0x14 } };
	
	return name;
}

ALTObjectDisplayProp* StopSign::NewDisplayProp()
{
	return new StopSignDisplayProp();
}
