#include "StdAfx.h"
#include "TrafficLight.h"
#include "../common/ARCUnit.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "ALTObjectDisplayProp.h"
TrafficLight::TrafficLight(void)
{ 
	m_nPathID  = -1;
	m_dRotation = 0.0;
	m_nIntersect = -1;
}
TrafficLight::~TrafficLight(void)
{
	m_path.clear();
}

void TrafficLight::ReadObject(int nObjID)
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
		m_path.clear();
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
	}
}
int TrafficLight::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_TRAFFICLIGHT);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);
	
	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void TrafficLight::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void TrafficLight::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);
	
	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString TrafficLight::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO TRAFFICLIGHT (OBJID,PATHID,ROTATION,INTERSECTIONNODEID ) VALUES (%d,%d,%f,%d)"),nObjID,m_nPathID,m_dRotation,m_nIntersect);

	return strSQL;
}
CString TrafficLight::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE TRAFFICLIGHT SET PATHID = %d , ROTATION = %f, INTERSECTIONNODEID = %d WHERE OBJID = %d"),m_nPathID,m_dRotation,m_nIntersect,nObjID);

	return strSQL;
}
CString TrafficLight::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TRAFFICLIGHT WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString TrafficLight::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TRAFFICLIGHT WHERE OBJID = %d"),
		nObjID);

	return strSQL;
}
const CPath2008& TrafficLight::GetPath()const
{
	return (m_path);
}
void TrafficLight::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}

const int TrafficLight::GetIntersecID()const
{
	return m_nIntersect;
}

void TrafficLight::SetIntersectID(const int intersecID)
{
	m_nIntersect = intersecID;
}

double TrafficLight::GetRotation(void)const
{
	return (m_dRotation);
}

void TrafficLight::SetRotation(double dRotation)
{
	m_dRotation = dRotation;
}

void TrafficLight::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeInt(m_nPathID);
    exportFile.getFile().writeInt(m_nIntersect);
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

void TrafficLight::ExportTrafficLight(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vTrafficLightID;
	ALTAirport::GetTrafficLightsIDs(nAirportID,vTrafficLightID);
	int nTrafficLightCount = static_cast<int>(vTrafficLightID.size());
	for (int i =0; i < nTrafficLightCount;++i)
	{
		TrafficLight trafficLightObj;
		trafficLightObj.ReadObject(vTrafficLightID[i]);
		trafficLightObj.ExportObject(exportFile);
	}
}

void TrafficLight::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);
	importFile.getFile().getInteger(m_nPathID);

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();
	for (int i = 0; i < nPointCount; i++)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}
	importFile.getFile().getFloat(m_dRotation);
	importFile.getFile().getInteger(m_nIntersect);

	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);
}

ALTObject * TrafficLight::NewCopy()
{
	TrafficLight * pTrafficLight  = new TrafficLight;
	*pTrafficLight = *this;
	return pTrafficLight;
}

bool TrafficLight::CopyData(const  ALTObject* pObj )
{
	if(this == pObj)
		return true;

	if(pObj->GetType() == GetType() )
	{
		TrafficLight * pOther = (TrafficLight*)pObj;
		m_path = pOther->GetPath();
		m_dRotation = pOther->GetRotation();
		m_nIntersect = pOther->GetIntersecID();

		m_bLocked = pOther->GetLocked();
		return true;
	}
	return false;
}

const GUID& TrafficLight::getTypeGUID()
{
	// {2CC655BC-D74C-4cc7-8967-11A990C01EEF}
	static const GUID name = 
	{ 0x2cc655bc, 0xd74c, 0x4cc7, { 0x89, 0x67, 0x11, 0xa9, 0x90, 0xc0, 0x1e, 0xef } };
	
	return name;
}

ALTObjectDisplayProp* TrafficLight::NewDisplayProp()
{
	return new TrafficLightDisplayProp();
}
