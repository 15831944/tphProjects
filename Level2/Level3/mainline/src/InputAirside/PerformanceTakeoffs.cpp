#include "StdAfx.h"
#include ".\PerformanceTakeoffs.h"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"

using namespace ADODB;

void PerformTakeOffs::ReadData(int nProjID)
{
	CString strSQL;

	m_nProjID = nProjID;

	strSQL.Format("SELECT * FROM FltPerformTakeOff Where ProjID = %d", m_nProjID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
	m_vTakeoffs.clear();

	while (!pRS->GetadoEOF())
	{

		PerformTakeOff tf;
		tf.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();

		CString strFltType = (char*)(_bstr_t)pRS->Fields->GetItem(_bstr_t("CompID"))->GetValue();
		tf.m_fltType.SetAirportDB(m_pAirportDB);
		tf.m_fltType.setConstraintWithVersion(strFltType);
		//tf.m_fltType = strFltType;

		tf.m_nMinAcceleration = (double)pRS->Fields->GetItem(_bstr_t("MinAcceleration"))->GetValue();
		tf.m_nMaxAcceleration = (double)pRS->Fields->GetItem(_bstr_t("MaxAcceleration"))->GetValue();
		tf.m_nMinLiftOff = (double)pRS->Fields->GetItem(_bstr_t("MinLiftOff"))->GetValue();
		tf.m_nMaxLiftOff = (double)pRS->Fields->GetItem(_bstr_t("MaxLiftOff"))->GetValue();
		tf.m_nMinPositionTime = (double)pRS->Fields->GetItem(_bstr_t("MinPositionTime"))->GetValue();
		tf.m_nMaxPositionTime = (double)pRS->Fields->GetItem(_bstr_t("MaxPositionTime"))->GetValue();
		tf.m_minTakeOffRoll = (double)pRS->Fields->GetItem(_bstr_t("MinTakeOffRoll"))->GetValue();
		tf.m_maxTakeOffRoll = (double)pRS->Fields->GetItem(_bstr_t("MaxTakeOffRoll"))->GetValue();
		tf.m_fuelBurn = (double)pRS->Fields->GetItem(_bstr_t("FuelBurn"))->GetValue();
		tf.m_cost = (double)pRS->Fields->GetItem(_bstr_t("Cost"))->GetValue();
		

		m_vTakeoffs.push_back(tf);

		pRS->MoveNext();
	}
	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();

	if(m_vTakeoffs.size() < 1)
	{
		PerformTakeOff tf;
		InsertPerformTakeOff(tf);
		GetTakeoffs().push_back(tf);
	}

}

void PerformTakeOffs::InsertPerformTakeOff(PerformTakeOff& tf)
{
	CString strSQL;
	char szFltType[1024] = {0};
	tf.m_fltType.WriteSyntaxStringWithVersion(szFltType);

	strSQL.Format("INSERT INTO FltPerformTakeOff \
				  (MinAcceleration, MaxAcceleration, MinLiftOff, MaxLiftOff,\
				  MinPositionTime, MaxPositionTime, MinTakeOffRoll, MaxTakeOffRoll, FuelBurn, Cost, CompID, ProjID) \
				  VALUES(%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, '%s', %d)",
				  tf.m_nMinAcceleration,tf.m_nMaxAcceleration,tf.m_nMinLiftOff,
				  tf.m_nMaxLiftOff, tf.m_nMinPositionTime, tf.m_nMaxPositionTime, tf.m_minTakeOffRoll,
				  tf.m_maxTakeOffRoll,  tf.m_fuelBurn, tf.m_cost,
				  szFltType, m_nProjID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	strSQL.Format("SELECT TOP 1 ID FROM FltPerformTakeOff ORDER BY ID DESC");
	pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (!pRS->GetadoEOF())
		tf.m_nID = (int)pRS->Fields->GetItem(_bstr_t("ID"))->GetValue();

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();
}

void PerformTakeOffs::UpdatePerformTakeOff(PerformTakeOff& tf)
{
	CString strSQL;
	char szFltType[1024] = {0};
	tf.m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("UPDATE FltPerformTakeOff \
				  SET MinAcceleration=%f ,MaxAcceleration=%f ,MinLiftOff=%f, MaxLiftOff=%f,\
				  MinPositionTime=%f, MaxPositionTime=%f, MinTakeOffRoll=%f, MaxTakeOffRoll=%f, FuelBurn=%f, Cost=%f,\
				  CompID='%s', ProjID=%d WHERE id = %d",
				  tf.m_nMinAcceleration,tf.m_nMaxAcceleration,tf.m_nMinLiftOff,
				  tf.m_nMaxLiftOff, tf.m_nMinPositionTime, tf.m_nMaxPositionTime,
				  tf.m_minTakeOffRoll, tf.m_maxTakeOffRoll, tf.m_fuelBurn, tf.m_cost,
				  szFltType, m_nProjID, tf.m_nID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);
	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();
}
void PerformTakeOffs::DeletePerformTakeOff(PerformTakeOff& tf)
{
	CString strSQL;
	strSQL.Format("Delete From FltPerformTakeOff WHERE ID = %d", tf.m_nID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();

}
//void PerformTakeOffs::SaveData()
//{
//
//	//CADODatabase::BeginTransaction()
//	//CADODatabase::ExecuteSQLStatementAndReturnScopeID()
//	
//
//
//}
void PerformTakeOffs::ExportPerformTakeOffs(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	PerformTakeOffs performTakeOffs(pAirportDB);
	performTakeOffs.ReadData(exportFile.GetProjectID());
	performTakeOffs.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void PerformTakeOffs::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("PerformTakeOffs");
	exportFile.getFile().writeLine();

	std::vector<PerformTakeOff>::iterator iter = m_vTakeoffs.begin();
	for (;iter != m_vTakeoffs.end(); ++iter)
	{
		exportFile.getFile().writeInt(iter->m_nID);
		char szFltType[1024] = {0};
		(iter->m_fltType).WriteSyntaxStringWithVersion(szFltType);
		exportFile.getFile().writeField(szFltType);
		exportFile.getFile().writeDouble(iter->m_nMinAcceleration);
		exportFile.getFile().writeDouble(iter->m_nMaxAcceleration);
		exportFile.getFile().writeDouble(iter->m_nMinLiftOff);
		exportFile.getFile().writeDouble(iter->m_nMaxLiftOff);
		exportFile.getFile().writeDouble(iter->m_nMinPositionTime);
		exportFile.getFile().writeDouble(iter->m_nMaxPositionTime);
		exportFile.getFile().writeDouble(iter->m_minTakeOffRoll);
		exportFile.getFile().writeDouble(iter->m_maxTakeOffRoll);
		exportFile.getFile().writeDouble(iter->m_fuelBurn);
		exportFile.getFile().writeDouble(iter->m_cost);
		exportFile.getFile().writeLine();
	}
}
void PerformTakeOffs::ImportPerformTakeOffs(CAirsideImportFile& importFile)
{
	PerformTakeOffs performTakeOffs(NULL);
	performTakeOffs.SetProjectID(importFile.getNewProjectID());
	while (!importFile.getFile().isBlank())
	{
		PerformTakeOff performTakeOff;

		int nOldID = -1;
		importFile.getFile().getInteger(nOldID);

		CString strFltType;
		importFile.getFile().getField(strFltType.GetBuffer(1024),1024);
		importFile.getFile().getFloat(performTakeOff.m_nMinAcceleration);
		importFile.getFile().getFloat(performTakeOff.m_nMaxAcceleration);
		importFile.getFile().getFloat(performTakeOff.m_nMinLiftOff);
		importFile.getFile().getFloat(performTakeOff.m_nMaxLiftOff);
		importFile.getFile().getFloat(performTakeOff.m_nMinPositionTime);
		importFile.getFile().getFloat(performTakeOff.m_nMaxPositionTime);
		importFile.getFile().getFloat(performTakeOff.m_minTakeOffRoll);
		importFile.getFile().getFloat(performTakeOff.m_maxTakeOffRoll);
		importFile.getFile().getFloat(performTakeOff.m_fuelBurn);
		importFile.getFile().getFloat(performTakeOff.m_cost);

		performTakeOffs.ImportInsertPerformTakeOff(performTakeOff,strFltType);
		importFile.getFile().getLine();

	}
}

void PerformTakeOffs::ImportInsertPerformTakeOff(PerformTakeOff& tf,CString strFltType)
{
	CString strSQL;

	strSQL.Format("INSERT INTO FltPerformTakeOff \
				  (MinAcceleration, MaxAcceleration, MinLiftOff, MaxLiftOff,\
				  MinPositionTime, MaxPositionTime, MinTakeOffRoll, MaxTakeOffRoll, FuelBurn, Cost, CompID, ProjID) \
				  VALUES(%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, '%s', %d)",
				  tf.m_nMinAcceleration,tf.m_nMaxAcceleration,tf.m_nMinLiftOff,
				  tf.m_nMaxLiftOff, tf.m_nMinPositionTime, tf.m_nMaxPositionTime, tf.m_minTakeOffRoll,
				  tf.m_maxTakeOffRoll,  tf.m_fuelBurn, tf.m_cost,
				  strFltType, m_nProjID);

	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	_RecordsetPtr pRS = conPtr->Execute(strSQL.AllocSysString(), NULL, adOptionUnspecified);

}
bool FlightTypeCompare(PerformTakeOff& fItem,PerformTakeOff& sItem)
{
	if (fItem.m_fltType.fits(sItem.m_fltType))
		return false;
	else if(sItem.m_fltType.fits(fItem.m_fltType))
		return true;

	return false;
}
void PerformTakeOffs::SortFlightType()
{
	std::sort(m_vTakeoffs.begin(),m_vTakeoffs.end(),FlightTypeCompare);
}

double PerformTakeOffs::GetFuelBurnByFlightType( AirsideFlightDescStruct& _flttype )
{
	PerformTakeOff defaultTakeoff ;
	BOOL HasDefault = FALSE ;
	for (int i = 0 ; i < (int)m_vTakeoffs.size() ;i++)
	{
		PerformTakeOff takeoffitem ;
		takeoffitem = m_vTakeoffs[i] ;
		if(takeoffitem.m_fltType.isDefault())
		{
			defaultTakeoff = takeoffitem ;
			HasDefault = TRUE ;
			continue ;
		}
		if(takeoffitem.m_fltType.fits(_flttype))
			return takeoffitem.m_fuelBurn ;
	}
	if(HasDefault)
		return defaultTakeoff.m_fuelBurn ;
	else
		return 0 ;
}