#include "StdAfx.h"
#include ".\intersections.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "Stretch.h"

Intersections::Intersections(void)
{ 
}

Intersections::Intersections( const Intersections& other)
{
	*this = other;
}
Intersections::~Intersections(void)
{
	m_vrInsecObjectPart.clear();
}

void Intersections::ReadObject(int nObjID)
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
			//if(ALTObject::ReadObjectByID(iopInsecObjectPart.nALTObjectID))
			{
				m_vrInsecObjectPart.push_back(iopInsecObjectPart);
			}
			adoRecordset.MoveNextData();
		}
	}
}
int Intersections::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_INTERSECTIONS);
	
	//CString strSQL = GetInsertScript(nObjID);
	CString strSQL = _T("");
	//CString strTemp = _T("");
	for(InsecObjectPartVector::const_iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		strSQL.Format(_T("INSERT INTO ROAD_INTERSECTIONS (OBJID,INSECOBJID,PART,POS) \
						  VALUES (%d,%d,%d,%f);\n"), \
						  nObjID,(*itrOBJPart).nALTObjectID,(*itrOBJPart).part,(*itrOBJPart).pos);

		CADODatabase::ExecuteSQLStatement(strSQL);
		//strSQL += strTemp;
	}


	return nObjID;
}
void Intersections::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);


	CString strSQL = _T("");
	//CString strTemp = _T("");
	for(InsecObjectPartVector::const_iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		strSQL.Format(_T("INSERT INTO ROAD_INTERSECTIONS (OBJID,INSECOBJID,PART,POS) \
						 VALUES (%d,%d,%d,%f);\n"), \
						 nObjID,(*itrOBJPart).nALTObjectID,(*itrOBJPart).part,(*itrOBJPart).pos);

		CADODatabase::ExecuteSQLStatement(strSQL);
		//strSQL += strTemp;
	}
	//CString strUpdateScript = GetInsertScript(nObjID);
	//CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void Intersections::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString Intersections::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	CString strTemp = _T("");
	for(InsecObjectPartVector::const_iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		strTemp.Format(_T("INSERT INTO ROAD_INTERSECTIONS (OBJID,INSECOBJID,PART,POS) \
						  VALUES (%d,%d,%d,%f);\n"), \
						  nObjID,(*itrOBJPart).nALTObjectID,(*itrOBJPart).part,(*itrOBJPart).pos);
		strSQL += strTemp;
	}

	return strSQL;
}
CString Intersections::GetUpdateScript(int nObjID) const
{
	CString strSQL  = _T("");
	return strSQL;
}
CString Intersections::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ROAD_INTERSECTIONS WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString Intersections::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM ROAD_INTERSECTIONS WHERE OBJID = %d"),nObjID);

	return strSQL;
}

const InsecObjectPartVector & Intersections::GetIntersectionsPart(void)const
{
	return (m_vrInsecObjectPart);
}

InsecObjectPartVector & Intersections::GetIntersectionsPart( void )
{
	return (m_vrInsecObjectPart);
}

void Intersections::SetIntersectionsPart(const InsecObjectPartVector & iopvrSet)
{
	m_vrInsecObjectPart.clear();
	for(InsecObjectPartVector::const_iterator ctitrOBJPart = iopvrSet.begin();ctitrOBJPart != iopvrSet.end();++ctitrOBJPart)
	{
		m_vrInsecObjectPart.push_back(*ctitrOBJPart);
	}
}

void Intersections::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);

	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeInt((int)(m_vrInsecObjectPart.size()) );
	for(InsecObjectPartVector::iterator itrOBJPart = m_vrInsecObjectPart.begin();itrOBJPart != m_vrInsecObjectPart.end();++itrOBJPart)
	{
		exportFile.getFile().writeInt((*itrOBJPart).nALTObjectID);
		exportFile.getFile().writeInt((*itrOBJPart).part);
		exportFile.getFile().writeDouble((*itrOBJPart).pos);
	}

	exportFile.getFile().writeLine();
}

void Intersections::ExportIntersections(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vIntersectionsID;
	ALTAirport::GetRoadIntersectionsIDs(nAirportID,vIntersectionsID);
	int nIntersectionsCount = static_cast<int>(vIntersectionsID.size());
	for (int i =0; i < nIntersectionsCount;++i)
	{
		Intersections intersectionsObj;
		intersectionsObj.ReadObject(vIntersectionsID[i]);
		intersectionsObj.ExportObject(exportFile);
	}
}

void Intersections::ImportObject(CAirsideImportFile& importFile)
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
		int oldObjID = -1;
		importFile.getFile().getInteger(oldObjID);
		iopInsecObjectPart.nALTObjectID = importFile.GetObjectNewID(oldObjID);
		importFile.getFile().getInteger(iopInsecObjectPart.part);
		importFile.getFile().getFloat(iopInsecObjectPart.pos);
		m_vrInsecObjectPart.push_back(iopInsecObjectPart);
	}

	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);
}

Intersections& Intersections::operator=( const Intersections& other)
{
	ALTObject::operator =(other);
	m_vrInsecObjectPart = other.m_vrInsecObjectPart;
	return *this;
}

CPoint2008 Intersections::GetPosition()
{
	CPoint2008 ret = CPoint2008(0,0,0);
	for(int i = 0;i< (int)m_vrInsecObjectPart.size();i++)
	{
		InsecObjectPart& thepart = m_vrInsecObjectPart[i];

		if(thepart.GetObject() && thepart.GetObject()->GetType() == ALT_STRETCH)
		{
			Stretch* pStretch  = (Stretch*) thepart.GetObject();
			const CPath2008& stretchpath = pStretch->GetPath();

			if( thepart.pos * 2 > stretchpath.getCount()-1 )
			{
				int pcount = (int) stretchpath.getCount();
				ret += stretchpath.getPoint(stretchpath.getCount()-1);
			}
			else 
			{
				ret += stretchpath.getPoint(0);
			}
		}
	}

	if( m_vrInsecObjectPart.size()) 
	{
		ret *= (1.0/m_vrInsecObjectPart.size());
	}
	return ret;
}

bool Intersections::HasObject( int nObjID ) const
{
	for(int i=0;i<(int)m_vrInsecObjectPart.size();i++)
	{
		InsecObjectPart thePart = m_vrInsecObjectPart[i];
		if(thePart.GetObjectID() == nObjID)
			return true;
	}
	return false;
}

const GUID& Intersections::getTypeGUID()
{
	// {74E3130C-8039-4e12-B3D7-F4E946A2919E}
	static const GUID name = 
	{ 0x74e3130c, 0x8039, 0x4e12, { 0xb3, 0xd7, 0xf4, 0xe9, 0x46, 0xa2, 0x91, 0x9e } };

	return name;
}