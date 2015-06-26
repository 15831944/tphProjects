#include "StdAfx.h"
#include ".\roundabout.h"
#include "../common/ARCUnit.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"

Roundabout::Roundabout(void)
{
	m_dLaneWidth = 8;
	m_iLaneNum = 1;
}

Roundabout::~Roundabout(void)
{
	m_vrInsecObjectPart.clear();
}

void Roundabout::ReadObject(int nObjID)
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
		adoRecordset.GetFieldValue(_T("LANENUM"),m_iLaneNum); 

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
int Roundabout::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_ROUNDABOUT);	

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void Roundabout::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);
	CString strUpdateScript = GetInsertScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void Roundabout::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString Roundabout::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");

	CString strTemp = _T("");
	for(InsecObjectPartVector::const_iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		strTemp.Format(_T("INSERT INTO ROUNDABOUT (LANEWIDTH,LANENUM,OBJID,INSECOBJID,PART,POS) VALUES (%d,%f,%d,%d);\n"), \
		ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM),m_iLaneNum, nObjID,(*itrOBJPart).nALTObjectID,(*itrOBJPart).part,(*itrOBJPart).pos);

		strSQL += strTemp;
	}
	
	return strSQL;
}
CString Roundabout::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");

	return strSQL;
}
CString Roundabout::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ROUNDABOUT WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString Roundabout::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM ROUNDABOUT WHERE OBJID = %d"),nObjID);

	return strSQL;
}


const InsecObjectPartVector & Roundabout::GetIntersectionsPart(void)
{
	return (m_vrInsecObjectPart);
}

void Roundabout::SetIntersectionsPart(const InsecObjectPartVector & iopvrSet)
{
	m_vrInsecObjectPart.clear();
	for(InsecObjectPartVector::const_iterator ctitrOBJPart = iopvrSet.begin();ctitrOBJPart != iopvrSet.end();++ctitrOBJPart)
	{
		m_vrInsecObjectPart.push_back(*ctitrOBJPart);
	}
}

double Roundabout::GetLaneWidth(void)
{
	return (m_dLaneWidth);
}

void Roundabout::SetLaneWidth(double dLaneWidth)
{
	m_dLaneWidth = dLaneWidth;
}
int Roundabout::GetLaneNumber(void)
{
	return (m_iLaneNum );
}
void Roundabout::SetLaneNumber(int iLaneNum)
{
	m_iLaneNum = iLaneNum;
}

void Roundabout::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);

	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeDouble(ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM));
	exportFile.getFile().writeInt(m_iLaneNum);

	exportFile.getFile().writeInt((int)m_vrInsecObjectPart.size());
	for(InsecObjectPartVector::iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		exportFile.getFile().writeInt((*itrOBJPart).nALTObjectID);
		exportFile.getFile().writeInt((*itrOBJPart).part);
		exportFile.getFile().writeDouble((*itrOBJPart).pos);
	}

	exportFile.getFile().writeLine();
}

void Roundabout::ExportRoundabout(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vRoundaboutID;
	ALTAirport::GetRoundaboutsIDs(nAirportID,vRoundaboutID);
	int nRoundaboutCount = static_cast<int>(vRoundaboutID.size());
	for (int i =0; i < nRoundaboutCount;++i)
	{
		Roundabout roundaboutObj;
		roundaboutObj.ReadObject(vRoundaboutID[i]);
		roundaboutObj.ExportObject(exportFile);
	}
}

void Roundabout::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);

	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);

	importFile.getFile().getFloat(m_dLaneWidth);
	m_dLaneWidth = ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::CM,ARCUnit::M);
	importFile.getFile().getInteger(m_iLaneNum);

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