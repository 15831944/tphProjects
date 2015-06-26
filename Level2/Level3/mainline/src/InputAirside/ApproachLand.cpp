#include "StdAfx.h"
#include ".\approachland.h"
//#include "../Inputs/IN_TERM.H"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\Common\AirportDatabase.h"

using namespace ADODB;

void ApproachLands::ReadData(int nProjID)
{
	m_nProjectID = nProjID;
	CString strSQL;
	strSQL.Format("SELECT * FROM FltPerformApproachLand WHERE ProjID = %d", m_nProjectID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
	m_vApproachLands.clear();

	while (!pRS->GetadoEOF())
	{
		ApproachLand appLand;
		appLand.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();
	
		CString strFltType = (char*)(_bstr_t)pRS->Fields->GetItem(_bstr_t("CompID"))->GetValue();
		appLand.m_fltType.SetAirportDB(m_pAirportDB);
		appLand.m_fltType.setConstraintWithVersion(strFltType);
		//appLand.m_fltType = strFltType;

		appLand.m_dMinLength = (double)pRS->Fields->GetItem(_bstr_t("MinLength"))->GetValue();
		appLand.m_dMinSpeed  = (double)pRS->Fields->GetItem(_bstr_t("MinSpeed"))->GetValue();
		appLand.m_dMaxSpeed  = (double)pRS->Fields->GetItem(_bstr_t("MaxSpeed"))->GetValue();
		appLand.m_dFuelBurn = (double)pRS->Fields->GetItem(_bstr_t("FuelBurn"))->GetValue();
		appLand.m_dCost = (double)pRS->Fields->GetItem(_bstr_t("Cost"))->GetValue();

		m_vApproachLands.push_back(appLand);

		pRS->MoveNext();
	}
	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();

	if(m_vApproachLands.size() < 1)
	{
		ApproachLand appLand;
		InsertApproachLand(appLand);
		GetApproachLands().push_back(appLand);
	}
}


void ApproachLands::InsertApproachLand(ApproachLand& appLand)
{
	CString strSQL;
	char szFltType[1024] = {0};
	appLand.m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("INSERT INTO FltPerformApproachLand \
				  (MinLength, MinSpeed, MaxSpeed, FuelBurn, Cost, CompID, ProjID)\
				  VALUES(%f, %f, %f, %f, %f, '%s', %d)",
			      appLand.m_dMinLength,appLand.m_dMinSpeed, appLand.m_dMaxSpeed, appLand.m_dFuelBurn, appLand.m_dCost,
				  szFltType, m_nProjectID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	strSQL.Format("SELECT TOP 1 ID FROM FltPerformApproachLand ORDER BY ID DESC");
	pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (!pRS->GetadoEOF())
		appLand.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();
}
void ApproachLands::UpdateApproachLand(ApproachLand& appLand)
{
	CString strSQL;
	char szFltType[1024] = {0};
	appLand.m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("UPDATE FltPerformApproachLand \
				  SET MinLength = %f, MinSpeed = %f, MaxSpeed = %f , FuelBurn = %f, Cost = %f,\
				  CompID='%s', ProjID=%d WHERE id = %d",
				  appLand.m_dMinLength,appLand.m_dMinSpeed, appLand.m_dMaxSpeed, appLand.m_dFuelBurn, appLand.m_dCost,
				  szFltType, m_nProjectID, appLand.m_nID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();
}

void ApproachLands::DeleteApproachLand(ApproachLand& appLand)
{
	CString strSQL;
	strSQL.Format("Delete From FltPerformApproachLand WHERE ID = %d", appLand.m_nID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();
}
void ApproachLands::ExportApproachLands(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	ApproachLands approachLands(pAirportDB);
	approachLands.ReadData(exportFile.GetProjectID());
	approachLands.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void ApproachLands::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("ApproachLands");
	exportFile.getFile().writeLine();

	std::vector<ApproachLand>::iterator iter = m_vApproachLands.begin();
	for (;iter != m_vApproachLands.end(); ++iter)
	{
		exportFile.getFile().writeInt(iter->m_nID);
		char szFltType[1024] = {0};
		(iter->m_fltType).WriteSyntaxStringWithVersion(szFltType);
		exportFile.getFile().writeField(szFltType);
		exportFile.getFile().writeDouble(iter->m_dMinLength);
		exportFile.getFile().writeDouble(iter->m_dMinSpeed);
		exportFile.getFile().writeDouble(iter->m_dMaxSpeed);
		exportFile.getFile().writeDouble(iter->m_dFuelBurn);
		exportFile.getFile().writeDouble(iter->m_dCost);

		exportFile.getFile().writeLine();
	}
}
void ApproachLands::ImportApproachLands(CAirsideImportFile& importFile)
{
	ApproachLands approachLands(NULL);
	approachLands.SetProjectID(importFile.getNewProjectID());
	while (!importFile.getFile().isBlank())
	{
		ApproachLand approachLand;

		int nOldID = -1;
		importFile.getFile().getInteger(nOldID);

		CString strFltType;
		importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

		importFile.getFile().getFloat(approachLand.m_dMinLength);
		importFile.getFile().getFloat(approachLand.m_dMinSpeed);
		importFile.getFile().getFloat(approachLand.m_dMaxSpeed);
		importFile.getFile().getFloat(approachLand.m_dFuelBurn);
		importFile.getFile().getFloat(approachLand.m_dCost);

		approachLands.ImportInsertApproachLand(approachLand,strFltType);
		importFile.getFile().getLine();

	}
}
void ApproachLands::ImportInsertApproachLand(ApproachLand& appLand,CString strFltType)
{
	CString strSQL;
	strSQL.Format("INSERT INTO FltPerformApproachLand \
				  (MinLength, MinSpeed, MaxSpeed, FuelBurn, Cost, CompID, ProjID)\
				  VALUES(%f, %f, %f, %f, %f, '%s', %d)",
				  appLand.m_dMinLength,appLand.m_dMinSpeed, appLand.m_dMaxSpeed, appLand.m_dFuelBurn, appLand.m_dCost,
				  strFltType, m_nProjectID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

}
bool FlightTypeCompare(ApproachLand& fItem,ApproachLand& sItem)
{
	if (fItem.m_fltType.fits(sItem.m_fltType))
		return false;
	else if(sItem.m_fltType.fits(fItem.m_fltType))
		return true;
	
	return false;
}
void ApproachLands::SortFlightType()
{
	std::sort(m_vApproachLands.begin(),m_vApproachLands.end(),FlightTypeCompare);
}

double ApproachLands::GetFuelBurnByFlightType( AirsideFlightDescStruct& _constraint )
{
	ApproachLand PFlightApproachLand  ;
	ApproachLand pDefaultFlightApproachLand ;
	BOOL m_HasDefault = FALSE ;
	for (int i = 0 ; i < (int)m_vApproachLands.size() ;i++)
	{
		PFlightApproachLand = m_vApproachLands.at(i) ;
		if(PFlightApproachLand.m_fltType.isDefault())
		{
			pDefaultFlightApproachLand = PFlightApproachLand ;
			m_HasDefault = TRUE ;
			continue ;
		}
		if(PFlightApproachLand.m_fltType.fits(_constraint))
			return PFlightApproachLand.m_dFuelBurn ;
	}
	if(m_HasDefault)
		return pDefaultFlightApproachLand.m_dFuelBurn ;
	else
		return 0 ;
}