#include "StdAfx.h"
#include ".\contour.h"
#include "ALTAirport.h"
#include "AirsideImportExportManager.h"
#include "Topography.h"
#include "ALTObjectDisplayProp.h"

Contour::Contour(void)
{
	m_dAlt      = 0;
	m_nParentID = -1;
}

Contour::~Contour(void)
{
	//for (vector<Contour::RefPtr>::iterator iter=m_pChild.begin(); iter!=m_pChild.end(); iter++)
	//{
	//	Contour *pContour = (*iter).get();
	//	delete pContour;
	//}
	m_pChild.clear();
}

const CPath2008& Contour::GetPath()const
{
	return m_path;
}

CPath2008& Contour::GetPath()
{
	return m_path;
}

void Contour::SetPath(const CPath2008& _path)
{
	m_path.init(_path);
}

void Contour::SetAltude(double alt)
{
	m_dAlt = alt;
}

double Contour::GetAltitude()
{
	return m_dAlt;
}

void Contour::SetParentID(int nParentID)
{
	m_nParentID = nParentID;
}

int Contour::GetParentID(void)
{
	return m_nParentID;
}

int Contour::GetChildCount()
{
	return (int)m_pChild.size();
}

Contour* Contour::GetChildItem(int nIndex)
{
	ASSERT(nIndex < GetChildCount());

	return m_pChild[nIndex].get();
}

//NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCATIONID, POINTCOUNT, \
//LOCATION, ALTITUDE, TSTAMP
void Contour::ReadObject(int nObjID)
{
	m_nObjID = nObjID;
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	m_pChild.clear();
	if (!adoRecordset.IsEOF())
	{	
		ALTObject::ReadObject(adoRecordset);

		adoRecordset.GetFieldValue(_T("LOCATIONID"),m_nPathID);
		int nPointCount = -1;
		adoRecordset.GetFieldValue(_T("POINTCOUNT"),nPointCount);
		adoRecordset.GetFieldValue(_T("LOCATION"),nPointCount,m_path);

		adoRecordset.GetFieldValue(_T("ALTITUDE"),m_dAlt);
		adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);

		ReadChildObject(m_nObjID);
	}
}

void Contour::ReadChildObject(int nParentObjID)
{
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectChildScript(nParentObjID),nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		Contour *pChildContour = new Contour;

		int nChildID;
		adoRecordset.GetFieldValue(_T("ID"), nChildID);

		pChildContour->ReadObject(nChildID);

		m_pChild.push_back(pChildContour);

		adoRecordset.MoveNextData();
	}
}

int Contour::SaveObject(int nAirportID)
{	
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_CONTOUR);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	//save child contour
	for (vector<Contour::RefPtr>::iterator iter=m_pChild.begin(); iter!=m_pChild.end(); iter++)
	{
		Contour *pChildContour = (*iter).get();
		pChildContour->SetParentID(nObjID);
		pChildContour->SaveObject(nAirportID);
	}

	return nObjID;
}

void Contour::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);

	//Update child path
	//for (vector<Contour::RefPtr>::iterator iter=m_pChild.begin(); iter!=m_pChild.end(); iter++)
	//{
	//	Contour *pChildContour = (*iter).get();

	//	pChildContour->UpdatePath();
	//}
}

void Contour::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);

	//update child Object
	//for (vector<Contour::RefPtr>::iterator iter=m_pChild.begin(); iter!=m_pChild.end(); iter++)
	//{
	//	Contour *pChildContour = (*iter).get();

	//	pChildContour->UpdateObject(pChildContour->getID());
	//}
}

void Contour::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);

	CString strUpdateScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);

	//delete child Object
	for (vector<Contour::RefPtr>::iterator iter=m_pChild.begin(); iter!=m_pChild.end(); iter++)
	{
		Contour *pChildContour = (*iter).get();

		pChildContour->DeleteObject(pChildContour->getID());
	}
}

CString Contour::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	
	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, LOCATIONID, POINTCOUNT, \
		LOCATION, ALTITUDE, PARENTID\
		FROM CONTOUR_VIEW\
		WHERE ID = %d"),nObjID);
	return strSQL;
}

CString Contour::GetSelectChildScript(int nParentObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("SELECT ID, APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, LOCATIONID, POINTCOUNT, \
					 LOCATION, ALTITUDE\
					 FROM CONTOUR_VIEW\
					 WHERE PARENTID = %d"),nParentObjID);
	return strSQL;
}

CString Contour::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("INSERT INTO CONTOURPROP \
		(OBJID, LOCATION, ALTITUDE, PARENTID)\
		VALUES (%d,%d,%f, %d)"),nObjID,m_nPathID,m_dAlt, m_nParentID);
	return strSQL;

}

CString Contour::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("UPDATE CONTOURPROP \
		SET ALTITUDE = %f\
		WHERE OBJID = %d"),m_dAlt, nObjID);
	return strSQL;
}

CString Contour::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("DELETE FROM CONTOURPROP\
		WHERE OBJID = %d"),nObjID);
	return strSQL;
}

void Contour::ExportContours(CAirsideExportFile& exportFile)
{
	std::vector<int> vContourID;
	Topography::GetRootContourList(exportFile.GetTopographyID(),vContourID);
	int nContourCount = static_cast<int>(vContourID.size());

	//export contour
	for (int i =0; i < nContourCount;++i)
	{
		Contour contour;
		contour.ReadObject(vContourID[i]);
		contour.ExportContour(exportFile);
	}
}

void Contour::ExportContour(CAirsideExportFile& exportFile)
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

	//altitude
	exportFile.getFile().writeDouble(m_dAlt);
	//child count
	exportFile.getFile().writeInt((int)m_pChild.size());

	exportFile.getFile().writeLine();

	//child contour
	for (int i=0; i<(int)m_pChild.size(); i++)
	{
		m_pChild[i]->ExportContour(exportFile);
	}
}

void Contour::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.getNewAirspaceID();

	//path
	//m_path.a

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();

	for (int i = 0; i < nPointCount; ++i)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	//altitude
	importFile.getFile().getFloat(m_dAlt);
	//child count
	int nChidCount;
	importFile.getFile().getInteger(nChidCount);

	//insert to database
	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);

	importFile.getFile().getLine();

	for (int i=0; i<nChidCount; i++)
	{
		//read child
		int nObjType;
		importFile.getFile().getInteger(nObjType);

		Contour contour;
		contour.SetParentID(nNewObjID);
		contour.ImportObject(importFile);
	}
}

bool Contour::CopyData( const ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if (pObj->GetType() == GetType())
	{
		Contour* pOtherContour = (Contour*)pObj;
		m_dAlt = pOtherContour->GetAltitude();
		m_path = pOtherContour->GetPath();

		m_bLocked = pOtherContour->GetLocked();
		return true;
	}
	ASSERT(FALSE);
	return false;
}

const GUID& Contour::getTypeGUID()
{
	// {3B3C03EA-E947-40d0-97A4-07166FE145F8}
	static const GUID name = 
	{ 0x3b3c03ea, 0xe947, 0x40d0, { 0x97, 0xa4, 0x7, 0x16, 0x6f, 0xe1, 0x45, 0xf8 } };

	return name;

}

ALTObjectDisplayProp* Contour::NewDisplayProp()
{
	return new ContourDisplayProp();
}

