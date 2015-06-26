#include "StdAfx.h"
#include ".\turnoff.h"
#include "../common/ARCUnit.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"

Turnoff::Turnoff(void)
{
	m_dLaneWidth = 8;
	m_nPathID = -1;
}

Turnoff::~Turnoff(void)
{
	m_vrInsecObjectPart.clear();
	m_path.clear();
}

void Turnoff::ReadObject(int nObjID)
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

		adoRecordset.GetFieldValue(_T("LANEWIDTH"),m_dLaneWidth);
		m_dLaneWidth = ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::CM,ARCUnit::M);   

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
int Turnoff::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_TURNOFF);	

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void Turnoff::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetInsertScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void Turnoff::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString Turnoff::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");

	CString strTemp = _T("");
	for(InsecObjectPartVector::const_iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		strTemp.Format(_T("INSERT INTO TURNOFF (LANEWIDTH,OBJID,INSECOBJID,PART,POS,PATHID) VALUES (%d,%f,%d,%d);\n"), \
		ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM),nObjID,(*itrOBJPart).nALTObjectID,(*itrOBJPart).part,(*itrOBJPart).pos,m_nPathID);

		strSQL += strTemp;
	}
	
	return strSQL;
}
CString Turnoff::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");

	return strSQL;
}
CString Turnoff::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TURNOFF WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString Turnoff::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TURNOFF WHERE OBJID = %d"),nObjID);

	return strSQL;
}


const InsecObjectPartVector & Turnoff::GetIntersectionsPart(void)
{
	return (m_vrInsecObjectPart);
}

void Turnoff::SetIntersectionsPart(const InsecObjectPartVector & iopvrSet)
{
	m_vrInsecObjectPart.clear();
	for(InsecObjectPartVector::const_iterator ctitrOBJPart = iopvrSet.begin();ctitrOBJPart != iopvrSet.end();++ctitrOBJPart)
	{
		m_vrInsecObjectPart.push_back(*ctitrOBJPart);
	}
}

double Turnoff::GetLaneWidth(void)
{
	return (m_dLaneWidth);
}

void Turnoff::SetLaneWidth(double dLaneWidth)
{
	m_dLaneWidth = dLaneWidth;
}

const CPath2008& Turnoff::GetPath()const
{
	return (m_path);
}
void Turnoff::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}

void Turnoff::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);

	exportFile.getFile().writeInt(m_nAptID);

	exportFile.getFile().writeDouble(ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM));
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

void Turnoff::ExportTurnoff(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vTurnoffID;
	ALTAirport::GetTurnoffsIDs(nAirportID,vTurnoffID);
	int nTurnoffCount = static_cast<int>(vTurnoffID.size());
	for (int i =0; i < nTurnoffCount;++i)
	{
		Turnoff turnoffObj;
		turnoffObj.ReadObject(vTurnoffID[i]);
		turnoffObj.ExportObject(exportFile);
	}
}

void Turnoff::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);

	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);

	importFile.getFile().getFloat(m_dLaneWidth);
	m_dLaneWidth = ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::CM,ARCUnit::M);
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