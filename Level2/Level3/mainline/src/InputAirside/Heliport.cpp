#include "StdAfx.h"
#include ".\heliport.h"
#include "AirsideImportExportManager.h"
#include "../common/ARCUnit.h"
#include "ALTAirport.h"
#include "ALTObjectDisplayProp.h"

Heliport::Heliport(void)
{
	m_path.init(0,NULL);
	m_Radius = 8000;
}

Heliport::~Heliport(void)
{
}


void Heliport::SetPath(const CPath2008& path)
{
	m_path = path;
}

const CPath2008& Heliport::GetPath()const
{
	return m_path;
}

void Heliport::SetRadius(double radius)
{
	m_Radius = radius;
}

double Heliport::GetRadius()const
{
	return m_Radius;
}

CPath2008& Heliport::getPath()
{
	return m_path;
}



CString Heliport::GetSelectScript(int nObjID)const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM HELIPORTPROP WHERE OBJID =%d"),nObjID);
	return strSQL;
}

CString Heliport::GetUpdateScript(int nObjID)const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE HELIPORTPROP SET RADIUS = %f WHERE OBJID = %d"),m_Radius,nObjID);
	return strSQL;
}

CString Heliport::GetInsertScript(int nObjID)const
{
	CString strSQL;
	strSQL.Format(_T("INSERT INTO HELIPORTPROP(OBJID,PATHID,RADIUS)VALUES(%d,%d,%f)"),m_nObjID,m_nPathID,m_Radius);
	return strSQL;
}

CString Heliport::GetDeleteScript(int nObjID)const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM HELIPORTPROP WHERE OBJID = %d"),nObjID);

	return strSQL;
}

void Heliport::ReadObject(int nObjID)
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

		adoRecordset.GetFieldValue(_T("RADIUS"),m_Radius);
//		m_Radius = ARCUnit::ConvertLength(m_Radius,ARCUnit::CM,ARCUnit::M);
	}
}

int Heliport::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_HELIPORT);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void Heliport::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}
void Heliport::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void Heliport::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);

}

bool Heliport::CopyData(const  ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if(pObj->GetType() == GetType())
	{
		Heliport  *pOtherHeliPort = (Heliport*)pObj;
		m_path = pOtherHeliPort->GetPath();
		m_Radius = pOtherHeliPort->GetRadius();		
	}
	return __super::CopyData(pObj);
}
void Heliport::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeDouble(ARCUnit::ConvertLength(m_Radius,ARCUnit::M,ARCUnit::CM));
	exportFile.getFile().writeInt(m_nPathID);

	//path
	int nPathCount = m_path.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i = 0; i < nPathCount; i++)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}

	exportFile.getFile().writeLine();
}

void Heliport::ExportHeliport(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vHeliportID;
	ALTAirport::GetHeliportsIDs(nAirportID,vHeliportID);
	int nHeliportCount = static_cast<int>(vHeliportID.size());
	for (int i =0; i < nHeliportCount;++i)
	{
		Heliport HeliportObj;
		HeliportObj.ReadObject(vHeliportID[i]);
		HeliportObj.ExportObject(exportFile);
	}
}

void Heliport::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);

	importFile.getFile().getFloat(m_Radius);
	m_Radius = ARCUnit::ConvertLength(m_Radius,ARCUnit::CM,ARCUnit::M);
	importFile.getFile().getInteger(m_nPathID);

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();
	for (int i = 0; i < nPointCount; i++)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);
}

ALTObject * Heliport::NewCopy()
{
	Heliport * newHeliport = new Heliport;

	newHeliport->CopyData(this);

	return newHeliport;

}

const GUID& Heliport::getTypeGUID()
{
	// {3BAEF362-750D-4c25-896A-72E4479DB37E}
	static const GUID name = 
	{ 0x3baef362, 0x750d, 0x4c25, { 0x89, 0x6a, 0x72, 0xe4, 0x47, 0x9d, 0xb3, 0x7e } };

	return name;
}

ALTObjectDisplayProp* Heliport::NewDisplayProp()
{
	return new HeliportDisplayProp();
}

