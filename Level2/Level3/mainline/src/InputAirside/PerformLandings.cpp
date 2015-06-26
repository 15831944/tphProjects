#include "StdAfx.h"
#include ".\PerformLandings.h"
//#include "./../Inputs/IN_TERM.H"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\Common\AirportDatabase.h"

using namespace ADODB;

//--------------------------------------------------------
//PerformLanding

void PerformLanding::ReadThresholds(int nID)
{
	m_nID = nID;
	CString strSQL;
	strSQL.Format("SELECT * FROM RunwayFlareDistance WHERE FltPerformlandingID = %d", m_nID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
	
	m_vRunwayThresholds.clear();

	while (!pRS->GetadoEOF())
	{
		RunwayThreshold rwItem;
		rwItem.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();
		rwItem.m_nFltPerformLandingID = (int)pRS->Fields->GetItem(_bstr_t("FltPerformlandingID"))->GetValue();
		CString strRWName = (LPCSTR)(_bstr_t)pRS->Fields->GetItem(_bstr_t("RWName"))->GetValue();
		strRWName.TrimLeft();
		strRWName.TrimRight();
		rwItem.m_strRunwayName = strRWName;

		rwItem.m_dDistance = (float)pRS->Fields->GetItem(_bstr_t("Distance"))->GetValue();

		m_vRunwayThresholds.push_back(rwItem);

		pRS->MoveNext();
	}

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();
}
void PerformLanding::InsertRunwayThreshold(RunwayThreshold& rwThreshold)
{
	CString strSQL;
	strSQL.Format("INSERT INTO RunwayFlareDistance \
				  (FltPerformlandingID,RWName, Distance ) \
				  VALUES( %d, '%s', %d)", 
				  rwThreshold.m_nFltPerformLandingID,
				  rwThreshold.m_strRunwayName,
				  rwThreshold.m_dDistance);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
	
	strSQL.Format("SELECT TOP 1 ID FROM RunwayFlareDistance ORDER BY ID DESC");
	pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (!pRS->GetadoEOF())
		rwThreshold.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();

}
void PerformLanding::UpdateRunwayThreshold(RunwayThreshold& rwThreshold)
{
	CString strSQL;
	strSQL.Format("UPDATE RunwayFlareDistance \
				   SET FltPerformlandingID = %d, RWName = '%s', Distance = %f WHERE ID = %d",
				   rwThreshold.m_nFltPerformLandingID, rwThreshold.m_strRunwayName,
				   rwThreshold.m_dDistance,rwThreshold.m_nID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();

}
void PerformLanding::DeleteRunwayThresholds(int nID)
{
	m_nID = nID;
	CString strSQL;
	strSQL.Format("DELETE FROM RunwayFlareDistance WHERE FltPerformlandingID = %d", m_nID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;
	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();
	
}
void PerformLanding::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(static_cast<int>(m_vRunwayThresholds.size()));
	std::vector<RunwayThreshold>::iterator iter = m_vRunwayThresholds.begin();
	for (; iter!=m_vRunwayThresholds.end(); iter++)
	{
		exportFile.getFile().writeInt(iter->m_nID);
		exportFile.getFile().writeField(iter->m_strRunwayName);
		exportFile.getFile().writeInt(iter->m_nFltPerformLandingID);
		exportFile.getFile().writeDouble(iter->m_dDistance);

		exportFile.getFile().writeLine();
	}
}
void PerformLanding::ImportData(CAirsideImportFile& importFile)
{
	int nRunwayThresholdCount = 0;
	importFile.getFile().getInteger(nRunwayThresholdCount);
	for (int i =0; i < nRunwayThresholdCount; ++i)
	{
		RunwayThreshold runwayThreshold;

		int nOldID = -1;
		importFile.getFile().getInteger(nOldID);

		importFile.getFile().getField((runwayThreshold.m_strRunwayName).GetBuffer(1024),1024);
		importFile.getFile().getInteger(runwayThreshold.m_nFltPerformLandingID);
		importFile.getFile().getFloat(runwayThreshold.m_dDistance);

		InsertRunwayThreshold(runwayThreshold);
		importFile.getFile().getLine();
	}
}
//---------------------------------------------------------
//PerformLandings
void PerformLandings::ReadData(int nProjID)
{
	m_nProjectID = nProjID;
	CString strSQL;
	strSQL.Format("SELECT * FROM FltPerformLanding WHERE ProjID = %d", m_nProjectID);

	//_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	//if (conPtr.GetInterfacePtr() == NULL)
	//	return;

	//_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
	m_vPerformLandings.clear();

	//while (!pRS->GetadoEOF())
	//{
	//	PerformLanding perfLand;
	//	perfLand.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		PerformLanding perfLand;
		adoRecordset.GetFieldValue("ID",perfLand.m_nID);
		CString strFltType;
		adoRecordset.GetFieldValue(_T("CompID"),strFltType);
		perfLand.m_fltType.SetAirportDB(m_pAirportDB);
		perfLand.m_fltType.setConstraintWithVersion(strFltType);
		//perfLand.m_fltType = strFltType;

		adoRecordset.GetFieldValue(_T("MinTouchDownSpeed"),perfLand.m_dMinTouchDownSpeed);
		adoRecordset.GetFieldValue(_T("MaxTouchDownSpeed"),perfLand.m_dMaxTouchDownSpeed);
		adoRecordset.GetFieldValue(_T("MinTouchDownDist"),perfLand.m_dMinTouchDownDist);
		adoRecordset.GetFieldValue(_T("MaxTouchDownDist"),perfLand.m_dMaxTouchDownDist);
		adoRecordset.GetFieldValue(_T("MaxDeceleration"),perfLand.m_dMaxDeceleration);
		adoRecordset.GetFieldValue(_T("MinLandDist"),perfLand.m_dMinLandDist);
		adoRecordset.GetFieldValue(_T("MaxLandDist"),perfLand.m_dMaxLandDist);
		adoRecordset.GetFieldValue(_T("MaxSpeedAcute"),perfLand.m_dMaxSpeedAcute);
		adoRecordset.GetFieldValue(_T("MaxSpeed90deg"),perfLand.m_dMaxSpeed90deg);
		adoRecordset.GetFieldValue(_T("MaxSpeedHighSpeed"),perfLand.m_dMaxSpeedHighSpeed);
		adoRecordset.GetFieldValue(_T("FuelBurn"),perfLand.m_dFuelBurn);
		adoRecordset.GetFieldValue(_T("Cost"),perfLand.m_dCost);
		adoRecordset.GetFieldValue(_T("NORMALDECELERATION"),perfLand.m_dNormalDeceleration);
		
		
		//perfLand.m_dMinTouchDownSpeed = 
		//	(double)pRS->Fields->GetItem(_bstr_t("MinTouchDownSpeed"))->GetValue();
		//perfLand.m_dMaxTouchDownSpeed = 
		//	(double)pRS->Fields->GetItem(_bstr_t("MaxTouchDownSpeed"))->GetValue();
		//perfLand.m_dMinTouchDownDist = 
		//	(double)pRS->Fields->GetItem(_bstr_t("MinTouchDownDist"))->GetValue();
		//perfLand.m_dMaxTouchDownDist = 
		//	(double)pRS->Fields->GetItem(_bstr_t("MaxTouchDownDist"))->GetValue();
		//perfLand.m_dMaxDeceleration = 
		//	(double)pRS->Fields->GetItem(_bstr_t("MaxDeceleration"))->GetValue();
		//perfLand.m_dMinLandDist = 
		//	(double)pRS->Fields->GetItem(_bstr_t("MinLandDist"))->GetValue();
		//perfLand.m_dMaxLandDist = 
		//	(double)pRS->Fields->GetItem(_bstr_t("MaxLandDist"))->GetValue();
		//perfLand.m_dMaxSpeedAcute = 
		//	(double)pRS->Fields->GetItem(_bstr_t("MaxSpeedAcute"))->GetValue();
		//perfLand.m_dMaxSpeed90deg= 
		//	(double)pRS->Fields->GetItem(_bstr_t("MaxSpeed90deg"))->GetValue();
		//perfLand.m_dMaxSpeedHighSpeed= 
		//	(double)pRS->Fields->GetItem(_bstr_t("MaxSpeedHighSpeed"))->GetValue();
		//perfLand.m_dFuelBurn = 
		//	(double)pRS->Fields->GetItem(_bstr_t("FuelBurn"))->GetValue();
		//perfLand.m_dCost = 
		//	(double)pRS->Fields->GetItem(_bstr_t("Cost"))->GetValue();

		//if (pRS->Fields->GetItem(_bstr_t("NORMALDECELERATION"))->GetValue())
		//	perfLand.m_dNormalDeceleration =
		//		(double)pRS->Fields->GetItem(_bstr_t("NORMALDECELERATION"))->GetValue();

		perfLand.ReadThresholds(perfLand.m_nID);

		m_vPerformLandings.push_back(perfLand);
		adoRecordset.MoveNextData();

		//pRS->MoveNext();
	}
	//if (pRS)
	//	if (pRS->State == adStateOpen)
	//		pRS->Close();

	if(m_vPerformLandings.size() < 1)
	{
		PerformLanding perfLand;
		InsertPerformLanding(perfLand);
		m_vPerformLandings.push_back(perfLand);
	}
}

void PerformLandings::InsertPerformLanding(PerformLanding& perfLand)
{
	CString strSQL;
	char szFltType[1024] = {0};
	perfLand.m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("INSERT INTO FltPerformLanding \
				  (MinTouchDownSpeed, MaxTouchDownSpeed, MinTouchDownDist,\
				   MaxTouchDownDist, MaxDeceleration,  MinLandDist, \
				   MaxLandDist, MaxSpeedAcute, MaxSpeed90deg, MaxSpeedHighSpeed, \
				   FuelBurn, Cost, NormalDeceleration, \
				   CompID, ProjID) \
				  VALUES(%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, '%s', %d)",
				  perfLand.m_dMinTouchDownSpeed,
				  perfLand.m_dMaxTouchDownSpeed,
				  perfLand.m_dMinTouchDownDist,
				  perfLand.m_dMaxTouchDownDist,
				  perfLand.m_dMaxDeceleration,
				  perfLand.m_dMinLandDist,
				  perfLand.m_dMaxLandDist,
				  perfLand.m_dMaxSpeedAcute,
				  perfLand.m_dMaxSpeed90deg,
				  perfLand.m_dMaxSpeedHighSpeed,
				  perfLand.m_dFuelBurn,
				  perfLand.m_dCost,
				  perfLand.m_dNormalDeceleration,
				  szFltType, m_nProjectID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	strSQL.Format("SELECT TOP 1 ID FROM FltPerformLanding ORDER BY ID DESC");
	pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (!pRS->GetadoEOF())
		perfLand.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();

	//if (pRS)
	//	if (pRS->State == adStateOpen)
	//		pRS->Close();
}
void PerformLandings::UpdatePerformLanding(PerformLanding& perfLand)
{
	CString strSQL;
	char szFltType[1024] = {0};
	perfLand.m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("UPDATE FltPerformLanding \
				  SET \
				  MinTouchDownSpeed = %f,MaxTouchDownSpeed = %f,MinTouchDownDist = %f,\
				  MaxTouchDownDist = %f,MaxDeceleration = %f,MinLandDist = %f,\
				  MaxLandDist = %f,MaxSpeedAcute = %f, MaxSpeed90deg = %f, MaxSpeedHighSpeed = %f, \
				  FuelBurn = %f, Cost = %f, NormalDeceleration = %f,\
				  CompID='%s', ProjID=%d WHERE id = %d",
				  perfLand.m_dMinTouchDownSpeed,
				  perfLand.m_dMaxTouchDownSpeed,
				  perfLand.m_dMinTouchDownDist,
				  perfLand.m_dMaxTouchDownDist,
				  perfLand.m_dMaxDeceleration,
				  perfLand.m_dMinLandDist,
				  perfLand.m_dMaxLandDist,
				  perfLand.m_dMaxSpeedAcute,
				  perfLand.m_dMaxSpeed90deg,
				  perfLand.m_dMaxSpeedHighSpeed,
				  perfLand.m_dFuelBurn,
				  perfLand.m_dCost,
				  perfLand.m_dNormalDeceleration,
				  szFltType, m_nProjectID, perfLand.m_nID);

	//_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	//if (conPtr.GetInterfacePtr() == NULL)
	//	return;

	DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL, NULL, adOptionUnspecified);
	//if (pRS)
	//	if (pRS->State == adStateOpen)
	//		pRS->Close();
}

void PerformLandings::DeletePerformLanding(PerformLanding& perfLand)
{
	CString strSQL;
	strSQL.Format("Delete From FltPerformLanding WHERE ID = %d", perfLand.m_nID);

	DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL, NULL, adOptionUnspecified);

	//if (pRS)
	//	if (pRS->State == adStateOpen)
	//		pRS->Close();
}
void PerformLandings::ExportPerformLandings(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	PerformLandings performLandings(pAirportDB);
	performLandings.ReadData(exportFile.GetProjectID());
	performLandings.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void PerformLandings::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("PerformLandings");
	exportFile.getFile().writeLine();

	std::vector<PerformLanding>::iterator iter = m_vPerformLandings.begin();
	for (;iter != m_vPerformLandings.end(); ++iter)
	{
		exportFile.getFile().writeInt(iter->m_nID);
		char szFltType[1024] = {0};
		(iter->m_fltType).WriteSyntaxStringWithVersion(szFltType);
		exportFile.getFile().writeField(szFltType);
		exportFile.getFile().writeDouble(iter->m_dMinTouchDownSpeed);
		exportFile.getFile().writeDouble(iter->m_dMaxTouchDownSpeed);
		exportFile.getFile().writeDouble(iter->m_dMinTouchDownDist);
		exportFile.getFile().writeDouble(iter->m_dMaxTouchDownDist);
		exportFile.getFile().writeDouble(iter->m_dMaxDeceleration);

		exportFile.getFile().writeDouble(iter->m_dNormalDeceleration);
		exportFile.getFile().writeDouble(iter->m_dMinLandDist);
		exportFile.getFile().writeDouble(iter->m_dMaxLandDist);
		exportFile.getFile().writeDouble(iter->m_dMaxSpeedAcute);
		exportFile.getFile().writeDouble(iter->m_dMaxSpeed90deg);

		exportFile.getFile().writeDouble(iter->m_dMaxSpeedHighSpeed);
		exportFile.getFile().writeDouble(iter->m_dFuelBurn);
		exportFile.getFile().writeDouble(iter->m_dCost);

		exportFile.getFile().writeLine();

		iter->ExportData(exportFile);
	}
}
void PerformLandings::ImportPerformLandings(CAirsideImportFile& importFile)
{
	PerformLandings performLandings(NULL);
	performLandings.SetProjectID(importFile.getNewProjectID());
	while (!importFile.getFile().isBlank())
	{
		PerformLanding performLanding;

		int nOldID = -1;
		importFile.getFile().getInteger(nOldID);

		CString strFltType;
		importFile.getFile().getField(strFltType.GetBuffer(1024),1024);
		importFile.getFile().getFloat(performLanding.m_dMinTouchDownSpeed);
		importFile.getFile().getFloat(performLanding.m_dMaxTouchDownSpeed);
		importFile.getFile().getFloat(performLanding.m_dMinTouchDownDist);
		importFile.getFile().getFloat(performLanding.m_dMaxTouchDownDist);
		importFile.getFile().getFloat(performLanding.m_dMaxDeceleration);

		importFile.getFile().getFloat(performLanding.m_dNormalDeceleration);
		importFile.getFile().getFloat(performLanding.m_dMinLandDist);
		importFile.getFile().getFloat(performLanding.m_dMaxLandDist);
		importFile.getFile().getFloat(performLanding.m_dMaxSpeedAcute);
		importFile.getFile().getFloat(performLanding.m_dMaxSpeed90deg);

		importFile.getFile().getFloat(performLanding.m_dMaxSpeedHighSpeed);
		importFile.getFile().getFloat(performLanding.m_dFuelBurn);
		importFile.getFile().getFloat(performLanding.m_dCost);


		performLandings.ImportInsertPerformLanding(performLanding,strFltType);
		importFile.getFile().getLine();

		performLanding.ImportData(importFile);
	}
}
void PerformLandings::ImportInsertPerformLanding(PerformLanding& perfLand,CString strFltType)
{
	CString strSQL;
	strSQL.Format("INSERT INTO FltPerformLanding \
				  (MinTouchDownSpeed, MaxTouchDownSpeed, MinTouchDownDist,\
				  MaxTouchDownDist, MaxDeceleration, MinLandDist, \
				  MaxLandDist, MaxSpeedAcute, MaxSpeed90deg, MaxSpeedHighSpeed, \
				  FuelBurn, Cost, NormalDeceleration, \
				  CompID, ProjID) \
				  VALUES(%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, '%s', %d)",
				  perfLand.m_dMinTouchDownSpeed,
				  perfLand.m_dMaxTouchDownSpeed,
				  perfLand.m_dMinTouchDownDist,
				  perfLand.m_dMaxTouchDownDist,
				  perfLand.m_dMaxDeceleration,
				  perfLand.m_dMinLandDist,
				  perfLand.m_dMaxLandDist,
				  perfLand.m_dMaxSpeedAcute,
				  perfLand.m_dMaxSpeed90deg,
				  perfLand.m_dMaxSpeedHighSpeed,
				  perfLand.m_dFuelBurn,
				  perfLand.m_dCost,
				  perfLand.m_dNormalDeceleration,
				  strFltType, m_nProjectID);

	//_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	//if (conPtr.GetInterfacePtr() == NULL)
	//	return;

	DATABASECONNECTION.GetConnection()->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

}


bool FlightTypeCompare(PerformLanding& fItem,PerformLanding& sItem)
{
	if (fItem.m_fltType.fits(sItem.m_fltType))
		return false;
	else if(sItem.m_fltType.fits(fItem.m_fltType))
		return true;

	return false;
}
void PerformLandings::SortFlightType()
{
	std::sort(m_vPerformLandings.begin(),m_vPerformLandings.end(),FlightTypeCompare);
}

double PerformLandings::GetFuelBurnByFlightType( AirsideFlightDescStruct& _Flttype )
{
	PerformLanding PFlightApproachLand  ;
	PerformLanding pDefaultFlightApproachLand ;
	BOOL m_HasDefault = FALSE ;
	for (int i = 0 ; i < (int)m_vPerformLandings.size() ;i++)
	{
		PFlightApproachLand = m_vPerformLandings.at(i) ;
		if(PFlightApproachLand.m_fltType.isDefault())
		{
			pDefaultFlightApproachLand = PFlightApproachLand ;
			m_HasDefault = TRUE ;
			continue ;
		}
		if(PFlightApproachLand.m_fltType.fits(_Flttype))
			return PFlightApproachLand.m_dFuelBurn ;
	}
	if(m_HasDefault)
		return pDefaultFlightApproachLand.m_dFuelBurn ;
	else
		return 0 ;
}