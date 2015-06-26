#include "StdAfx.h"
#include ".\Airsector.h"
#include "ALTAirspace.h"
#include "AirsideImportExportManager.h"
#include "ALTObjectDisplayProp.h"



AirSector::AirSector(void)
{
	m_dLowAlt = 30000;
	m_dHighAlt = 40000;
	m_dBandInterval =100;
}

AirSector::AirSector( const AirSector& othersector ) : ALTObject(othersector)
{
	CopyData(&othersector);
}
AirSector::~AirSector(void)
{
}

//NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOWALT, HIGHALT, INTVALALT, \
//TSTAMP, PATH, POINTCOUNT, PATHID

void AirSector::ReadObject(int nObjID)
{m_nObjID = nObjID;
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	

		ALTObject::ReadObject(adoRecordset);


		adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID);
		int nPointCount = -1;
		adoRecordset.GetFieldValue(_T("POINTCOUNT"),nPointCount);
		adoRecordset.GetFieldValue(_T("PATH"),nPointCount,m_path);

		adoRecordset.GetFieldValue(_T("LOWALT"),m_dLowAlt);
		adoRecordset.GetFieldValue(_T("HIGHALT"),m_dHighAlt);

		adoRecordset.GetFieldValue(_T("INTVALALT"),m_dBandInterval);
	}

}
int AirSector::SaveObject(int nAirportID)
{	
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_SECTOR);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;

}
void AirSector::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}
void AirSector::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}
void AirSector::DeleteObject(int nObjID)
{	
	ALTObject::DeleteObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);

	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);

}
CString AirSector::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, LOWALT, HIGHALT, INTVALALT, \
		TSTAMP, PATH, POINTCOUNT, PATHID\
		FROM SECTOR_VIEW\
		WHERE ID = %d"),nObjID);

	return strSQL;

}
CString AirSector::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO SECTORPROP\
		(OBJID, PATH, LOWALT, HIGHALT, INTVALALT)\
		VALUES (%d,%d,%f,%f,%f)"),
		nObjID,m_nPathID,m_dLowAlt,m_dHighAlt,m_dBandInterval);

	return strSQL;
}
CString AirSector::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE SECTORPROP \
		SET LOWALT =%f, HIGHALT =%f, INTVALALT = %f \
		WHERE OBJID = %d"),
		m_dLowAlt,m_dHighAlt,m_dBandInterval,nObjID);

	return strSQL;
}
CString AirSector::GetDeleteScript(int nObjID) const
{

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM SECTORPROP \
		WHERE OBJID = %d"),
		nObjID);

	return strSQL;
}

ALTObject * AirSector::NewCopy()
{
	AirSector * reslt = new AirSector;
	reslt->CopyData(this);
	return reslt;
}

void AirSector::SetLowAltitude(double dAlt)
{ 
	m_dLowAlt = dAlt;
}

double AirSector::GetLowAltitude()
{ 
	return m_dLowAlt;
}

void AirSector::SetHighAltitude(double dAlt)
{ 
	m_dHighAlt = dAlt;
}

double AirSector::GetHighAltitude()
{ 
	return m_dHighAlt;
}

const CPath2008& AirSector::GetPath()const
{ 
	return m_path;
}

void AirSector::ExportSectors(CAirsideExportFile& exportFile)
{
	ALTObjectUIDList vSectorID;
	ALTAirspace::GetSectorList(exportFile.GetAirspaceID(),vSectorID);

	for (ALTObjectUIDList::size_type i = 0; i < vSectorID.size(); ++i )
	{
		AirSector airSectorObj;
		airSectorObj.ReadObject(vSectorID[i]);
		airSectorObj.ExportSector(exportFile);
	}
}
//CPath2008 m_path;
//double m_dLowAlt;
//double m_dHighAlt;
//double m_dBandInterval;
void AirSector::ExportSector(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	//apt id
	exportFile.getFile().writeInt(m_nAptID);
	//path
	int nPathCount = m_path.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i =0; i < nPathCount; ++i)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}

	exportFile.getFile().writeDouble(m_dLowAlt);
	exportFile.getFile().writeDouble(m_dHighAlt);
	exportFile.getFile().writeDouble(m_dBandInterval);

	exportFile.getFile().writeLine();

	AirSectorDisplayProp dspProp;
	dspProp.ReadData(m_nObjID);
	dspProp.ExportDspProp(exportFile);


}
void AirSector::ImportObject(CAirsideImportFile& importFile)
{

	ALTObject::ImportObject(importFile);

	m_nAptID = importFile.getNewAirspaceID();
	int nOldAirspaceID = -1;
	importFile.getFile().getInteger(nOldAirspaceID);

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();

	for (int i = 0; i < nPointCount; ++i)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	importFile.getFile().getFloat(m_dLowAlt);
	importFile.getFile().getFloat(m_dHighAlt);
	importFile.getFile().getFloat(m_dBandInterval);
	
	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);

	importFile.getFile().getLine();

	AirSectorDisplayProp dspProp;
	dspProp.ImportDspProp(importFile,m_nObjID);
}

bool AirSector::CopyData(const ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if (pObj->GetType() == GetType())
	{
		AirSector* pOthersector = (AirSector*)pObj;
		m_path = pOthersector->m_path;
		m_dLowAlt = pOthersector->m_dLowAlt;
		m_dHighAlt = pOthersector->m_dHighAlt;
		m_dBandInterval = pOthersector->m_dBandInterval;	
	}
	return __super::CopyData(pObj);
}

const GUID& AirSector::getTypeGUID()
{
	// {7DE9A81B-5125-4ebf-998D-3053C9227AF8}
	static const GUID class_guid = 
	{ 0x7de9a81b, 0x5125, 0x4ebf, { 0x99, 0x8d, 0x30, 0x53, 0xc9, 0x22, 0x7a, 0xf8 } };
	
	return class_guid;


}

ALTObjectDisplayProp* AirSector::NewDisplayProp()
{
	return new AirSectorDisplayProp();
}
