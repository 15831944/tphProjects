#include "stdafx.h"
#include ".\topography.h"
#include "..\Database\ADODatabase.h"
#include "Surface.h"
#include "contour.h"
#include "Structure.h"
Topography::Topography(void)
{
}

Topography::~Topography(void)
{
}

void Topography::GetSurfaceList(int nTopographyID, std::vector<int>& vSurfaceID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM SURFACE_VIEW WHERE APTID = %d",nTopographyID);

	long nRecordCount;
	CADORecordset adoRs;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

	while (!adoRs.IsEOF())
	{
		int nID = -1;
		adoRs.GetFieldValue(_T("ID"),nID);
		vSurfaceID.push_back(nID);

		adoRs.MoveNextData();
	}
}

void Topography::GetStructureList(int nTopographyID, std::vector<int>& vStructrIDs )
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM STRUCTURE_VIEW WHERE APTID = %d",nTopographyID);

	long nRecordCount;
	CADORecordset adoRs;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

	while (!adoRs.IsEOF())
	{
		int nID = -1;
		adoRs.GetFieldValue(_T("ID"),nID);
		vStructrIDs.push_back(nID);

		adoRs.MoveNextData();
	}
}

//if failed,throw exception CADOException
void Topography::GetContourList(int nTopographyID, std::vector<int>& vContourID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM CONTOUR_VIEW WHERE APTID = %d",nTopographyID);

	long nRecordCount;
	CADORecordset adoRs;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

	while (!adoRs.IsEOF())
	{
		int nID = -1;
		adoRs.GetFieldValue(_T("ID"),nID);
		vContourID.push_back(nID);

		adoRs.MoveNextData();
	}
}

void Topography::GetRootContourList(int nTopographyID, std::vector<int>& vContourID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM CONTOUR_VIEW WHERE APTID = %d AND PARENTID = -1",nTopographyID);

	long nRecordCount;
	CADORecordset adoRs;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

	while (!adoRs.IsEOF())
	{
		int nID = -1;
		adoRs.GetFieldValue(_T("ID"),nID);
		vContourID.push_back(nID);

		adoRs.MoveNextData();
	}
}

void Topography::ExportObjects(CAirsideExportFile& exportFile)
{
	Surface::ExportSurfaces(exportFile);
	Contour::ExportContours(exportFile);
}

void Topography::LoadStructure(int nTopographyID)
{
	m_lstStructure.clear();

	ALTObjectUIDList objectIDs;
	Topography::GetStructureList(nTopographyID,objectIDs);

	int nCount = (int)objectIDs.size();
	for(int i =0 ;i < nCount; ++i)
	{
		Structure * pStructure = new Structure();
		pStructure->ReadObject(objectIDs.at(i));
		m_lstStructure.push_back(pStructure);
	}
}

void Topography::LoadSurface(int nTopographyID)
{
	m_lstSurface.clear();

	ALTObjectUIDList objectIDs;
	Topography::GetSurfaceList(nTopographyID,objectIDs);

	int nCount = (int)objectIDs.size();
	for(int i =0 ;i < nCount; ++i)
	{
		Surface * pSurface = new Surface();
		pSurface->ReadObject(objectIDs.at(i));
		m_lstSurface.push_back(pSurface);
	}
}

void Topography::LoadContour(int nTopographyID)
{
	m_lstContour.clear();

	ALTObjectUIDList objectIDs;
	Topography::GetContourList(nTopographyID,objectIDs);

	int nCount = (int)objectIDs.size();
	for(int i =0 ;i < nCount; ++i)
	{
		Contour* pContour = new Contour();
		pContour->ReadObject(objectIDs.at(i));
		m_lstContour.push_back(pContour);
	}
}

void Topography::LoadTopography(int nTopographyID)
{
	LoadStructure(nTopographyID);
	LoadSurface(nTopographyID);
	LoadContour(nTopographyID);
}

ALTObjectList* Topography::getAirStructureList()
{
	return &m_lstStructure;
}

ALTObjectList* Topography::getSurfaceList()
{
	return &m_lstSurface;
}

ALTObjectList* Topography::getContourList()
{
	return &m_lstContour;
}