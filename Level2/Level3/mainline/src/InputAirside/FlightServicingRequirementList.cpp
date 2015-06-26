#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\flightservicingrequirementlist.h"
#include "../Common/FLT_CNST.H"

CFlightServicingRequirementList::CFlightServicingRequirementList(void)
{
}

CFlightServicingRequirementList::~CFlightServicingRequirementList(void)
{
}

//DBElementCollection
void CFlightServicingRequirementList::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, FLTTYPE \
					 FROM IN_SERVICINGREQUIREMENTFLIGHTTYPE\
					 WHERE (PROJID = %d)"),nProjectID);
}

bool FlightTypeCompare(CFlightServicingRequirement* fItem,CFlightServicingRequirement* sItem)
{
	if (fItem->GetFltType().fits(sItem->GetFltType()))
		return false;
	else if(sItem->GetFltType().fits(fItem->GetFltType()))
		return true;

	return false;
}

void CFlightServicingRequirementList::ReadData(int nProjectID, CVehicleSpecifications* pVehicleSpec)
{
	CString strSelectSQL;
	GetSelectElementSQL(nProjectID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		InitFromDBRecordset(adoRecordset, pVehicleSpec);
		adoRecordset.MoveNextData();
	}

	//if ((int)GetElementCount() == 0
	//	&& pVehicleSpec != NULL)
	//{
	//	CString strFlightType(_T("DEFAULT"));
	//	CFlightServicingRequirement *pFlightServiceingRequirement = new CFlightServicingRequirement(strFlightType, pVehicleSpec);

	//	AddNewItem(pFlightServiceingRequirement);

	//	SaveData(nProjectID);
	//}

	std::sort(m_dataList.begin(),m_dataList.end(),FlightTypeCompare);
}

void CFlightServicingRequirementList::SaveData(int nProjID)
{
	for (std::vector<CFlightServicingRequirement *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SaveData(nProjID);
	}

	for (std::vector<CFlightServicingRequirement *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

//DBElement
void CFlightServicingRequirementList::InitFromDBRecordset(CADORecordset& recordset, CVehicleSpecifications* pVehicleSpec)
{
	recordset.GetFieldValue(_T("ID"), m_nID);

	CString strFltType;
	recordset.GetFieldValue(_T("FLTTYPE"), strFltType);

	CFlightServicingRequirement *pFlightServicingRequirement = new CFlightServicingRequirement();

	pFlightServicingRequirement->SetAirportDatabase(m_pAirportDatabase);
	pFlightServicingRequirement->SetFltTypeID(m_nID);
	pFlightServicingRequirement->SetFltType(strFltType);

	AddNewItem(pFlightServicingRequirement);

	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT * \
						   FROM IN_SERVICINGREQUIREMENT \
						   WHERE (FLTTYPEID = %d)"),
						   m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		pFlightServicingRequirement->SetVehicleSpecification(pVehicleSpec);
		pFlightServicingRequirement->InitFromDBRecordset(adoRecordset);
	}	
}

void CFlightServicingRequirementList::SetAirportDatabase(CAirportDatabase* pAirportDB)
{ 
	m_pAirportDatabase = pAirportDB; 
}

void CFlightServicingRequirementList::ExportFlightServiceRequirement(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
	CFlightServicingRequirementList flightServicingRequirementList;
	flightServicingRequirementList.SetAirportDatabase(pAirportDB);
	flightServicingRequirementList.ReadData(exportFile.GetProjectID(),NULL);

	exportFile.getFile().writeField("FlightServicingRequirementList");
	exportFile.getFile().writeLine();

	size_t itemCount = flightServicingRequirementList.GetElementCount();	
	for (size_t i =0; i < itemCount; ++i)
	{
		flightServicingRequirementList.GetItem(i)->ExportData(exportFile);	
	}
	exportFile.getFile().endFile();
}

void CFlightServicingRequirementList::ImportFlightServiceRequirement(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CFlightServicingRequirement  flightServicingRequirement;
		flightServicingRequirement.ImportData(importFile);
	}
}

void CFlightServicingRequirementList::ExportData(CAirsideExportFile& exportFile)
{

}

void CFlightServicingRequirementList::ImportData(CAirsideImportFile& importFile)
{

}

//void CFlightServicingRequirementList::GetInsertSQL(int nProjID, CString& strSQL) const
//{
//	CString strFltType;
//	m_FlightType.printConstraint(strFltType.GetBuffer(1024));
//
//	strSQL.Format(_T("INSERT INTO IN_SERVICINGREQUIREMENTFLIGHTTYPE\
//					 (PROJID, FLTTYPE)\
//					 VALUES (%d, '%s')"),\
//					 nProjID,
//					 strFltType);
//}
//
//void CFlightServicingRequirementList::GetUpdateSQL(CString& strSQL) const
//{
//	CString strFltType;
//	m_FlightType.printConstraint(strFltType.GetBuffer(1024));
//
//	strSQL.Format(_T("UPDATE IN_SERVICINGREQUIREMENTFLIGHTTYPE\
//					 SET FLTTYPE = '%S' \
//					 WHERE (ID =%d)"),\
//					 strFltType,
//					 m_nID);
//}
//
//void CFlightServicingRequirementList::GetDeleteSQL(CString& strSQL) const
//{
//	strSQL.Format(_T("DELETE FROM IN_SERVICINGREQUIREMENTFLIGHTTYPE\
//					 WHERE (ID = %d)"),m_nID);
//}