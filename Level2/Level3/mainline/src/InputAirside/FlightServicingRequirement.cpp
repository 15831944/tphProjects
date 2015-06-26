#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\flightservicingrequirement.h"
#include "ServicingRequirement.h"
#include "VehicleSpecifications.h"
#include "AirsideImportExportManager.h"



CFlightServicingRequirement::CFlightServicingRequirement(void)
{
	m_pVehicleSpecification = NULL;
}

CFlightServicingRequirement::CFlightServicingRequirement(CString& strFlightType, CVehicleSpecifications *pVehicleSpec)
: m_pVehicleSpecification(pVehicleSpec)
{
	ASSERT(pVehicleSpec);

	SetFltType(strFlightType);

	//Add vehicle specifications
	int nCount = (int)pVehicleSpec->GetElementCount();
	for (int i=0; i<nCount; i++)
	{
		CVehicleSpecificationItem *pVehicleSpecItem = pVehicleSpec->GetItem(i);
		if (pVehicleSpecItem->getBaseType() == VehicleType_TowTruck||pVehicleSpecItem->getBaseType() == VehicleType_JointBagTug)
			continue;

		CServicingRequirement *pServicingRequirement = new CServicingRequirement;

		pServicingRequirement->SetServicingRequirementNameID(pVehicleSpecItem->GetID());
		SetDefaultValue(pServicingRequirement, pVehicleSpecItem);

		AddNewItem(pServicingRequirement);
	}
}

CFlightServicingRequirement::CFlightServicingRequirement(FlightConstraint& fltConstraint, CVehicleSpecifications *pVehicleSpec)
: m_pVehicleSpecification(pVehicleSpec)
{
	ASSERT(pVehicleSpec);

	SetFltType(fltConstraint);

	//Add vehicle specifications
	int nCount = (int)pVehicleSpec->GetElementCount();
	for (int i=0; i<nCount; i++)
	{
		CVehicleSpecificationItem *pVehicleSpecItem = pVehicleSpec->GetItem(i);
		if (pVehicleSpecItem->getBaseType() == VehicleType_TowTruck||pVehicleSpecItem->getBaseType() == VehicleType_JointBagTug)
			continue;

		CServicingRequirement *pServicingRequirement = new CServicingRequirement;

		pServicingRequirement->SetServicingRequirementNameID(pVehicleSpecItem->GetID());
		SetDefaultValue(pServicingRequirement, pVehicleSpecItem);

		AddNewItem(pServicingRequirement);
	}
}

void CFlightServicingRequirement::SetDefaultValue(CServicingRequirement* pServiceRequirement,CVehicleSpecificationItem *pVehiSepcItem)
{
	ASSERT(pServiceRequirement);
	ASSERT(pVehiSepcItem);

	CString strCondition;

	enumVehicleBaseType enumBaseType = pVehiSepcItem->getBaseType();
	enumVehicleUnit enumUnit = pVehiSepcItem->getUnit();

	if (enumUnit == Liters)
	{
		pServiceRequirement->SetConditionType(enumVehicleTypeCondition_Per100Liters);
	}
	else if (enumUnit == Pax)
	{
		pServiceRequirement->SetConditionType(enumVehicleTypeCondition_Per10Pax);

	}
	//else if (enumBaseType == VehicleType_BaggageTug)
	//{
	//	pServiceRequirement->SetConditionType(enumVehicleTypeCondition_DurationOfCarts);

	//}
	else if (enumUnit == Bag || enumUnit == Carts)
	{
		pServiceRequirement->SetConditionType(enumVehicleTypeCondition_Per10Bags);

	}
	//else if (enumUnit == PerVehicle && enumBaseType == VehicleType_TowTruck)
	//{
	//	pServiceRequirement->SetConditionType(enumVehicleTypeCondition_PriorPushToPush);

	//}
	else
	{
		pServiceRequirement->SetConditionType(enumVehicleTypeCondition_PerVehicle);
	}
}

CFlightServicingRequirement::~CFlightServicingRequirement(void)
{
}

void CFlightServicingRequirement::SetFltType(FlightConstraint& fltType)
{
	m_FlightType = fltType;
}

void CFlightServicingRequirement::SetFltType(CString& strFltType)
{
	if (m_pAirportDatabase)
	{
		m_FlightType.SetAirportDB(m_pAirportDatabase);
		m_FlightType.setConstraintWithVersion(strFltType);
	}
}

FlightConstraint CFlightServicingRequirement::GetFltType()
{
	return m_FlightType;
}

void CFlightServicingRequirement::SetAirportDatabase(CAirportDatabase* pAirportDatabase)
{
	m_pAirportDatabase = pAirportDatabase;
}

//DBElementCollection
void CFlightServicingRequirement::SaveData(int nProj)
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(nProj, strSQL);

		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		m_nFltTypeID = m_nID;
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	for (std::vector<CServicingRequirement *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetFltTypeID(m_nFltTypeID);
		(*iter)->SaveDataToDB();
	}
}

void CFlightServicingRequirement::DeleteData()
{

}

//DBElement
void CFlightServicingRequirement::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"), m_nID);

	//CString strFltType;
	//recordset.GetFieldValue(_T("FLTTYPEID"), strFltType);

	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT * \
						   FROM IN_SERVICINGREQUIREMENT \
						   WHERE (FLTTYPEID = %d)"),
						   m_nFltTypeID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nVehicleID = -1;
		adoRecordset.GetFieldValue(_T("SERVICEREQUIREMENTNAMEID"),nVehicleID);

		CVehicleSpecificationItem *pVehiSepcItem = m_pVehicleSpecification->GetVehicleItemByID(nVehicleID);
		
		if (pVehiSepcItem == NULL || pVehiSepcItem->getBaseType() == VehicleType_TowTruck||pVehiSepcItem->getBaseType() == VehicleType_JointBagTug)
		{
			adoRecordset.MoveNextData();
			continue;
		}

		CServicingRequirement *pServicingRequirement = new CServicingRequirement();
		pServicingRequirement->SetAirportDB(m_pAirportDatabase);
	    
		pServicingRequirement->InitFromDBRecordset(adoRecordset);

		AddNewItem(pServicingRequirement);

		adoRecordset.MoveNextData();
	}
}

//DBElement
void CFlightServicingRequirement::GetInsertSQL(int nProjID, CString& strSQL) const
{
	CString strFltType;
    m_FlightType.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));
	
	strSQL.Format(_T("INSERT INTO IN_SERVICINGREQUIREMENTFLIGHTTYPE\
					 (PROJID, FLTTYPE)\
					 VALUES (%d, '%s')"),\
					 nProjID,
					 strFltType);
}

void CFlightServicingRequirement::GetUpdateSQL(CString& strSQL) const
{
	CString strFltType;
	m_FlightType.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));
	
	strSQL.Format(_T("UPDATE IN_SERVICINGREQUIREMENTFLIGHTTYPE\
					 SET FLTTYPE = '%s' \
					 WHERE (ID =%d)"),\
					 strFltType,
					 m_nFltTypeID);
}

void CFlightServicingRequirement::GetDeleteSQL(CString& strSQL) const
{
		strSQL.Format(_T("DELETE FROM IN_SERVICINGREQUIREMENTFLIGHTTYPE\
						 WHERE (ID = %d)"),m_nFltTypeID);
}

void CFlightServicingRequirement::DeleteDataFromDB()
{
	for (std::vector<CServicingRequirement *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	CString strSelectSQL;
	GetDeleteSQL(strSelectSQL);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);
}

void CFlightServicingRequirement::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	CString strFltType;

	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

	ImportSaveData(importFile.getNewProjectID(),strFltType);

	importFile.getFile().getLine();

	int nCount = 0;
	importFile.getFile().getInteger(nCount);
	for (int i =0; i < nCount; ++i)
	{
		CServicingRequirement data;
		data.SetFltTypeID(m_nFltTypeID);
		data.ImportData(importFile);
	}
}

void CFlightServicingRequirement::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);

	CString strFltType;
	m_FlightType.WriteSyntaxStringWithVersion(strFltType.GetBuffer(1024));
	exportFile.getFile().writeField(strFltType);
	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt((int)GetElementCount());

	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CFlightServicingRequirement::ImportSaveData(int nProj,CString strFltType)
{
	CString strSQL;

	ImportGetInsertSQL(nProj, strSQL, strFltType);

	if (strSQL.IsEmpty())
		return;

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	m_nFltTypeID = m_nID;
}


void CFlightServicingRequirement::ImportGetInsertSQL(int nProjID, CString& strSQL,CString strFltType) const
{
	strSQL.Format(_T("INSERT INTO IN_SERVICINGREQUIREMENTFLIGHTTYPE\
					 (PROJID, FLTTYPE)\
					 VALUES (%d, '%s')"),\
					 nProjID,
					 strFltType);
}

void CFlightServicingRequirement::SetVehicleSpecification( CVehicleSpecifications* pVehicleSpecification )
{
	m_pVehicleSpecification = pVehicleSpecification;
}