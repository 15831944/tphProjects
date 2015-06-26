#include "StdAfx.h"
#include "FlightTypeDetailsItem.h"
#include "../Common/AirportDatabase.h"
#include "AirsideImportExportManager.h"


FlightTypeDetailsItem::FlightTypeDetailsItem(CAirportDatabase* pAirportDB, int nProjID)
:m_nID(-1)
,m_nProjID(nProjID)
,m_pAirportDB(pAirportDB)
{
}

FlightTypeDetailsItem::~FlightTypeDetailsItem(void)
{
}

FlightTypeDimensions& FlightTypeDetailsItem::GetDimensions()	
{
	return m_dimensions; 
}

FlightTypeServiceLoactions& FlightTypeDetailsItem::GetRelativeLocations()
{
	return m_serviceLocations; 
}

FlightConstraint FlightTypeDetailsItem::GetFlightConstraint()
{
	return m_fltType; 
}

RingRoad& FlightTypeDetailsItem::GetRingRoad()
{
	return m_ringRoad; 
}

CPath2008 FlightTypeDetailsItem::GetRingRoadPath()
{
	return m_ringRoad.GetPath();
}

int FlightTypeDetailsItem::GetID()
{
	return m_nID; 
}

FlightConstraint FlightTypeDetailsItem::GetFltType() 
{
	return m_fltType; 
}

void FlightTypeDetailsItem::SetFltType(FlightConstraint fltCons)
{
	m_fltType = fltCons; 
}

void FlightTypeDetailsItem::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	CString strFltType;
	char szFltType[1024] = {0};
	adoRecordset.GetFieldValue(_T("FlightType"),strFltType);
	strcpy_s(szFltType,strFltType);
	m_fltType.SetAirportDB(m_pAirportDB);
	m_fltType.setConstraintWithVersion(szFltType);


	m_dimensions.ReadData(adoRecordset);
	m_serviceLocations.ReadData(m_nID);
	m_ringRoad.ReadData(m_nID);
}

void FlightTypeDetailsItem::SaveData()
{
	if(m_nID != -1)
	{
		UpdateData();
		return;
	}

	char strFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(strFltType);
	m_nID = m_dimensions.SaveData(m_nProjID, strFltType);

	m_serviceLocations.SaveData(m_nID);
	m_ringRoad.SaveData(m_nID);
}

void FlightTypeDetailsItem::InitDefault()
{
	//dimensions
	char strFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(strFltType);
	m_nID = m_dimensions.SaveData(m_nProjID, strFltType);

	//Service location
	m_serviceLocations.InitDefault(m_nProjID, m_nID);

	//ring road
	m_ringRoad.SaveData(m_nID);
}

void FlightTypeDetailsItem::UpdateData()
{
	char strFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(strFltType);
	m_dimensions.UpdateData(m_nID, m_nProjID, strFltType);

	m_serviceLocations.UpdateData(m_nID);
	m_ringRoad.UpdateData(m_nID);
}


void FlightTypeDetailsItem::DeleteData()
{
	m_dimensions.DeleteData(m_nID);
	m_serviceLocations.DeleteData(m_nID);
	m_ringRoad.DeleteData();
}

void FlightTypeDetailsItem::SaveImportData(CString& strFltType)
{
	m_nID = m_dimensions.SaveData(m_nProjID, strFltType);
}

void FlightTypeDetailsItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	char strFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(strFltType);
	exportFile.getFile().writeField(strFltType);

	m_dimensions.ExportData(exportFile);
	m_serviceLocations.ExportData(exportFile, m_nID);
	m_ringRoad.ExportData(exportFile, m_nID);

	exportFile.getFile().writeLine();

}

void FlightTypeDetailsItem::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	CString strFltType;
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

	m_dimensions.ImportData(importFile);

	m_nProjID = importFile.getNewProjectID();
	SaveImportData(strFltType);

	m_serviceLocations.ImportData(importFile, m_nID);
	m_ringRoad.ImportData(importFile, m_nID);

	importFile.getFile().getLine();
}

