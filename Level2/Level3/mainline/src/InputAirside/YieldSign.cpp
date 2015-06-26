#include "StdAfx.h"
#include "YieldSign.h"
#include "../common/ARCUnit.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "ALTObjectDisplayProp.h"

YieldSign::YieldSign(void)
{ 
	m_nPathID  = -1;
	m_dRotation = 0.0;
	m_nIntersect = -1;
	m_nStretchID = -1;
}
YieldSign::~YieldSign(void)
{
	m_path.clear();
}

void YieldSign::ReadObject(int nObjID)
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
		adoRecordset.GetFieldValue(_T("INTERSECTIONNODEID"),m_nIntersect);
		adoRecordset.GetFieldValue(_T("STRETCHID"),m_nStretchID);
		m_path.clear();
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
	}
}
int YieldSign::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_YIELDSIGN);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);
	
	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void YieldSign::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void YieldSign::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);
	
	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString YieldSign::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO YIELDSIGN (OBJID,PATHID,ROTATION,INTERSECTIONNODEID,STRETCHID ) VALUES (%d,%d,%f,%d,%d)"),nObjID,m_nPathID,m_dRotation,m_nIntersect,m_nStretchID);

	return strSQL;
}
CString YieldSign::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE YIELDSIGN SET PATHID = %d , ROTATION = %f ,INTERSECTIONNODEID = %d ,STRETCHID = %d WHERE OBJID = %d"),m_nPathID,m_dRotation,m_nIntersect,m_nStretchID, nObjID);

	return strSQL;
}
CString YieldSign::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM YIELDSIGN WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString YieldSign::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM YIELDSIGN WHERE OBJID = %d"),
		nObjID);

	return strSQL;
}
const CPath2008& YieldSign::GetPath()const
{
	return (m_path);
}
void YieldSign::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}

const int YieldSign::GetIntersectionNodeID()const
{
	return m_nIntersect;
}

void YieldSign::SetIntersectionNodeID(const int nIntersect)
{
	m_nIntersect = nIntersect;
}

const int YieldSign::GetStretchID()const
{
	return m_nStretchID;
}

void YieldSign::SetStretchID(const int nStretchID)
{
	m_nStretchID = nStretchID;
}

double YieldSign::GetRotation(void)const
{
	return (m_dRotation);
}

void YieldSign::SetRotation(double dRotation)
{
	m_dRotation = dRotation;
}

void YieldSign::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeInt(m_nPathID);
	exportFile.getFile().writeInt(m_nIntersect);
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

void YieldSign::ExportYieldSign(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vYieldSignID;
	ALTAirport::GetYieldSignsIDs(nAirportID,vYieldSignID);
	int nYieldSignCount = static_cast<int>(vYieldSignID.size());
	for (int i =0; i < nYieldSignCount;++i)
	{
		YieldSign yieldSignObj;
		yieldSignObj.ReadObject(vYieldSignID[i]);
		yieldSignObj.ExportObject(exportFile);
	}
}

void YieldSign::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);
	importFile.getFile().getInteger(m_nPathID);
	importFile.getFile().getInteger(m_nIntersect);
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

ALTObject * YieldSign::NewCopy()
{
	YieldSign * pYieldSign  = new YieldSign;
	*pYieldSign = *this;
	return pYieldSign;
}

bool YieldSign::CopyData(const  ALTObject* pObj )
{
	if(this == pObj)
		return true;

	if(pObj->GetType() == GetType() )
	{
		YieldSign * pOther = (YieldSign*)pObj;
		m_path = pOther->GetPath();
        m_dRotation = pOther->GetRotation();
		m_nStretchID = pOther->GetStretchID();
        m_nIntersect = pOther->GetIntersectionNodeID();

		m_bLocked = pOther->GetLocked();
		return true;
	}
	return false;
}

const GUID& YieldSign::getTypeGUID()
{
	// {DEB7A9B8-7098-43fe-B1E9-3FE14344B35B}
	static const GUID name = 
	{ 0xdeb7a9b8, 0x7098, 0x43fe, { 0xb1, 0xe9, 0x3f, 0xe1, 0x43, 0x44, 0xb3, 0x5b } };
	return name;
}

