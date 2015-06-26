#include "StdAfx.h"
#include ".\noseinparking.h"
#include "../common/ARCUnit.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"

NoseInParking::NoseInParking(void)
{
	m_dSpotWidth = 8;
	m_iSpotNum = 1;
	m_dSpotSlopAngle = 0;
	m_nPathID = -1;
}

NoseInParking::~NoseInParking(void)
{
	m_vrInsecObjectPart.clear();
	m_path.clear();
}

void NoseInParking::ReadObject(int nObjID)
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
		m_vrInsecObjectPart.clear();
		InsecObjectPart iopInsecObjectPart;

		adoRecordset.GetFieldValue(_T("SPOTWIDTH"),m_dSpotWidth);
		m_dSpotWidth = ARCUnit::ConvertLength(m_dSpotWidth,ARCUnit::CM,ARCUnit::M);   
		adoRecordset.GetFieldValue(_T("SPOTNUM"),m_iSpotNum); 
		adoRecordset.GetFieldValue(_T("SPOTSLOPANGLE"),m_dSpotSlopAngle);

		adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID );
		m_path.clear();
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);

		while(!adoRecordset.IsEOF())
		{
			adoRecordset.GetFieldValue(_T("INSECOBJID"),iopInsecObjectPart.nALTObjectID); 
			adoRecordset.GetFieldValue(_T("PART"),iopInsecObjectPart.part);
			adoRecordset.GetFieldValue(_T("POS"),iopInsecObjectPart.pos);
			m_vrInsecObjectPart.push_back(iopInsecObjectPart);
			
			adoRecordset.MoveNextData();
		}
	}
}
int NoseInParking::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_NOSEINPARKING);	

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void NoseInParking::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetInsertScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void NoseInParking::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString NoseInParking::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");

	CString strTemp = _T("");
	for(InsecObjectPartVector::const_iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		strTemp.Format(_T("INSERT INTO NoseInParking (SPOTWIDTH,SPOTNUM,SPOTSLOPANGLE,OBJID,INSECOBJID,PART,POS,PATHID) VALUES (%d,%f,%d,%d);\n"), \
		ARCUnit::ConvertLength(m_dSpotWidth,ARCUnit::M,ARCUnit::CM),m_iSpotNum,m_dSpotSlopAngle,nObjID,(*itrOBJPart).nALTObjectID,(*itrOBJPart).part,(*itrOBJPart).pos,m_nPathID);

		strSQL += strTemp;
	}
	
	return strSQL;
}
CString NoseInParking::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");

	return strSQL;
}
CString NoseInParking::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM NoseInParking WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString NoseInParking::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM NoseInParking WHERE OBJID = %d"),nObjID);

	return strSQL;
}


const InsecObjectPartVector & NoseInParking::GetIntersectionsPart(void)
{
	return (m_vrInsecObjectPart);
}

void NoseInParking::SetIntersectionsPart(const InsecObjectPartVector & iopvrSet)
{
	m_vrInsecObjectPart.clear();
	for(InsecObjectPartVector::const_iterator ctitrOBJPart = iopvrSet.begin();ctitrOBJPart != iopvrSet.end();++ctitrOBJPart)
	{
		m_vrInsecObjectPart.push_back(*ctitrOBJPart);
	}
}

double NoseInParking::GetSpotWidth(void)
{
	return (m_dSpotWidth);
}

void NoseInParking::SetSpotWidth(double dSpotWidth)
{
	m_dSpotWidth = dSpotWidth;
}

double NoseInParking::GetSpotSlopAngle(void)
{
	return (m_dSpotSlopAngle);
}

void NoseInParking::SetSpotSlopAngle(double dSpotSlopAngle)
{
	m_dSpotSlopAngle = dSpotSlopAngle;
}

int NoseInParking::GetSpotNumber(void)
{
	return (m_iSpotNum);
}
void NoseInParking::SetSpotNumber(int iSpotNum)
{
	m_iSpotNum = iSpotNum;
}

const CPath2008& NoseInParking::GetPath()const
{
	return (m_path);
}
void NoseInParking::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}

void NoseInParking::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);

	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeDouble(ARCUnit::ConvertLength(m_dSpotWidth,ARCUnit::M,ARCUnit::CM));
	exportFile.getFile().writeInt(m_iSpotNum);
	exportFile.getFile().writeDouble(m_dSpotSlopAngle);
	exportFile.getFile().writeInt(m_nPathID);
	
	int nPathCount = m_path.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i = 0; i < nPathCount; i++)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}

	exportFile.getFile().writeInt((int)m_vrInsecObjectPart.size());
	for(InsecObjectPartVector::iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		exportFile.getFile().writeInt((*itrOBJPart).nALTObjectID);
		exportFile.getFile().writeInt((*itrOBJPart).part);
		exportFile.getFile().writeDouble((*itrOBJPart).pos);
	}

	exportFile.getFile().writeLine();
}

void NoseInParking::ExportNoseInParking(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vNoseInParkingID;
	ALTAirport::GetNoseInParkingsIDs(nAirportID,vNoseInParkingID);
	int nNoseInParkingCount = static_cast<int>(vNoseInParkingID.size());
	for (int i =0; i < nNoseInParkingCount;++i)
	{
		NoseInParking noseInParkingObj;
		noseInParkingObj.ReadObject(vNoseInParkingID[i]);
		noseInParkingObj.ExportObject(exportFile);
	}
}

void NoseInParking::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);

	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);
	importFile.getFile().getFloat(m_dSpotWidth);
	m_dSpotWidth = ARCUnit::ConvertLength(m_dSpotWidth,ARCUnit::CM,ARCUnit::M);
	importFile.getFile().getInteger(m_iSpotNum);
	importFile.getFile().getFloat(m_dSpotSlopAngle);
	importFile.getFile().getInteger(m_nPathID);

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();
	for (int i = 0; i < nPointCount; i++)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	InsecObjectPart iopInsecObjectPart;
	int nCount = -1;
	m_vrInsecObjectPart.clear();
	importFile.getFile().getInteger(nCount);
	for(int i = 0;i < nCount;i++)
	{
		importFile.getFile().getInteger(iopInsecObjectPart.nALTObjectID);
		importFile.getFile().getInteger(iopInsecObjectPart.part);
		importFile.getFile().getFloat(iopInsecObjectPart.pos);
		m_vrInsecObjectPart.push_back(iopInsecObjectPart);
	}

	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);
}