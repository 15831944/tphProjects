#include "StdAfx.h"
#include "TollGate.h"
#include "../common/ARCUnit.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "ALTObjectDisplayProp.h"
TollGate::TollGate(void)
{ 
	m_nPathID  = -1;
	m_dRotation = 0.0;
}
TollGate::~TollGate(void)
{
	m_path.clear();
}

void TollGate::ReadObject(int nObjID)
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
		m_path.clear();
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
	}
}
int TollGate::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_TOLLGATE);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);
	
	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void TollGate::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void TollGate::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);
	
	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString TollGate::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO TOLLGATE (OBJID,PATHID,ROTATION ) VALUES (%d,%d,%f)"),nObjID,m_nPathID,m_dRotation);

	return strSQL;
}
CString TollGate::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE TOLLGATE SET PATHID = %d ,ROTATION = %f WHERE OBJID = %d"),m_nPathID,m_dRotation, nObjID);

	return strSQL;
}
CString TollGate::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TOLLGATE WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString TollGate::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TOLLGATE WHERE OBJID = %d"),
		nObjID);

	return strSQL;
}
const CPath2008& TollGate::GetPath()const
{
	return (m_path);
}
void TollGate::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}

double TollGate::GetRotation(void)const
{
	return (m_dRotation);
}

void TollGate::SetRotation(double dRotation)
{
	m_dRotation = dRotation;
}

void TollGate::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeInt(m_nPathID);

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

void TollGate::ExportTollGate(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vTollGateID;
	ALTAirport::GetTollGatesIDs(nAirportID,vTollGateID);
	int nTollGateCount = static_cast<int>(vTollGateID.size());
	for (int i =0; i < nTollGateCount;++i)
	{
		TollGate tollGateObj;
		tollGateObj.ReadObject(vTollGateID[i]);
		tollGateObj.ExportObject(exportFile);
	}
}

void TollGate::ImportObject(CAirsideImportFile& importFile)
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

	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);
}


ALTObject * TollGate::NewCopy()
{
	TollGate * pTollGate  = new TollGate;
	*pTollGate = *this;
	return pTollGate;
}

const GUID& TollGate::getTypeGUID()
{
	// {56F0DE03-3E52-4bc4-87CF-37599BF6269B}
	static const GUID name = 
	{ 0x56f0de03, 0x3e52, 0x4bc4, { 0x87, 0xcf, 0x37, 0x59, 0x9b, 0xf6, 0x26, 0x9b } };

	return name;
}

ALTObjectDisplayProp* TollGate::NewDisplayProp()
{
	return new TollGateDisplayProp();
}
