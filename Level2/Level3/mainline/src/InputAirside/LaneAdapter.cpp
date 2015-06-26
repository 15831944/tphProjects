#include "StdAfx.h"
#include ".\laneadapter.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"

LaneAdapter::LaneAdapter(void)
{
}

LaneAdapter::~LaneAdapter(void)
{
	m_vrInsecObjectPart.clear();
}

void LaneAdapter::ReadObject(int nObjID)
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
int LaneAdapter::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_LANEADAPTER);
	
	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void LaneAdapter::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);
	CString strUpdateScript = GetInsertScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void LaneAdapter::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString LaneAdapter::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	CString strTemp = _T("");
	for(InsecObjectPartVector::const_iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		strTemp.Format(_T("INSERT INTO LANEADAPTER (OBJID,INSECOBJID,PART,POS) \
						  VALUES (%d,%d,%d);\n"), \
						  nObjID,(*itrOBJPart).nALTObjectID,(*itrOBJPart).part,(*itrOBJPart).pos);
		strSQL += strTemp;
	}

	return strSQL;
}
CString LaneAdapter::GetUpdateScript(int nObjID) const
{
	CString strSQL  = _T("");
	return strSQL;
}
CString LaneAdapter::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM LANEADAPTER WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString LaneAdapter::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM LANEADAPTER WHERE OBJID = %d"),nObjID);

	return strSQL;
}

const InsecObjectPartVector & LaneAdapter::GetIntersectionsPart(void)
{
	return (m_vrInsecObjectPart);
}

void LaneAdapter::SetIntersectionsPart(const InsecObjectPartVector & iopvrSet)
{
	m_vrInsecObjectPart.clear();
	for(InsecObjectPartVector::const_iterator ctitrOBJPart = iopvrSet.begin();ctitrOBJPart != iopvrSet.end();++ctitrOBJPart)
	{
		m_vrInsecObjectPart.push_back(*ctitrOBJPart);
	}
}

void LaneAdapter::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeInt((int)m_vrInsecObjectPart.size());
	for(InsecObjectPartVector::iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		exportFile.getFile().writeInt((*itrOBJPart).nALTObjectID);
		exportFile.getFile().writeInt((*itrOBJPart).part);
		exportFile.getFile().writeDouble((*itrOBJPart).pos);
	}

	exportFile.getFile().writeLine();
}

void LaneAdapter::ExportLaneAdapter(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vLaneAdapterID;
	ALTAirport::GetLaneAdaptersIDs(nAirportID,vLaneAdapterID);
	int nLaneAdapterCount = static_cast<int>(vLaneAdapterID.size());
	for (int i =0; i < nLaneAdapterCount;++i)
	{
		LaneAdapter laneAdapterObj;
		laneAdapterObj.ReadObject(vLaneAdapterID[i]);
		laneAdapterObj.ExportObject(exportFile);
	}
}

void LaneAdapter::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);

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