#include "StdAfx.h"
#include ".\flightperformanceterminal.h"
#include "../Database/ARCportDatabaseConnection.h"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\Common\AirportDatabase.h"

//#include "./../Inputs/IN_TERM.H"

using namespace ADODB;
using namespace ns_AirsideInput;

CFlightPerformanceTerminal::CFlightPerformanceTerminal(void)
{
	m_id = -1; //unique id in table 

	m_nProjID  = -1;// the project ID
	m_dFuelBurn = 1200.00;
	m_dMinSpeed = 180.00;
	m_dMaxSpeed = 250.00;
	m_dMaxAcceleration = 3.00;
	m_dMaxDeceleration = 4.00;

	m_dMinSpeedHolding = 180.00;
	m_dMaxSpeedHolding = 260.00;
	m_dMaxDownVSpeed = 2000.00;
	m_dMaxUpVSpeed = 2000.00;
	m_dCost = 1800.00;
}

CFlightPerformanceTerminal::~CFlightPerformanceTerminal(void)
{
}
CString CFlightPerformanceTerminal::GetUpdateScript() const
{
	CString strSQL;
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format(_T("UPDATE FlightPerformanceTerminal \
		SET ProjID =%d, CompID ='%s', FuelBurn =%f, MinSpeed =%f, MaxSpeed =%f, MaxAcceleration =%f, \
		MaxDeceleration =%f, MinSpeedHolding =%f, MaxSpeedHolding =%f, MaxDownVSpeed =%f, \
		MaxUpVSpeed =%f, Cost =%f \
		WHERE (ID = %d)"), m_nProjID,szFltType,m_dFuelBurn,m_dMinSpeed,m_dMaxSpeed,m_dMaxAcceleration,
		m_dMaxDeceleration,m_dMinSpeedHolding,m_dMaxSpeedHolding,m_dMaxDownVSpeed,m_dMaxUpVSpeed,m_dCost,m_id );

	return strSQL;
}
CString CFlightPerformanceTerminal::GetInsertScript() const
{
	CString strSQL;
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format(_T("INSERT INTO FlightPerformanceTerminal \
									 (ProjID, CompID, FuelBurn, MinSpeed, MaxSpeed, MaxAcceleration, MaxDeceleration, \
									 MinSpeedHolding, MaxSpeedHolding, MaxDownVSpeed, MaxUpVSpeed, Cost) \
									 VALUES (%d,'%s',%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)"),
									 m_nProjID,szFltType,m_dFuelBurn,m_dMinSpeed,m_dMaxSpeed,m_dMaxAcceleration,
									 m_dMaxDeceleration,m_dMinSpeedHolding,m_dMaxSpeedHolding,m_dMaxDownVSpeed,m_dMaxUpVSpeed,m_dCost );

	return strSQL;
}
CString CFlightPerformanceTerminal::GetDeleteScript() const
{
	CString strSQL;
	strSQL.Format("DELETE FROM FlightPerformanceTerminal \
		WHERE (ID = %d)",m_id);
	return strSQL;
}
void CFlightPerformanceTerminal::SaveData()
{
	if (m_id != -1)
	{
		UpdateData();
		return;
	}
	_variant_t varCount;
	DATABASECONNECTION.GetConnection()->Execute((_bstr_t)GetInsertScript(),&varCount,adOptionUnspecified);
	//get id
	CString strSQL;
	strSQL.Empty();
	strSQL.Format("SELECT TOP 1 ID FROM FlightPerformanceTerminal WHERE  ProjID= %d ORDER BY ID DESC",m_nProjID);

	_RecordsetPtr pRS =  DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	if (!pRS->GetadoEOF())
	{
		_variant_t vid;
		vid = pRS->GetCollect(L"ID");
		if (vid.vt != NULL)
		{
			m_id = (int)vid.intVal;
		}
	}
	if (pRS)
		if (pRS->State == adStateOpen)
			pRS->Close();

}
void CFlightPerformanceTerminal::UpdateData()
{
	_variant_t varCount;
	DATABASECONNECTION.GetConnection()->Execute((_bstr_t)GetUpdateScript(),&varCount,adOptionUnspecified);

}
void CFlightPerformanceTerminal::DeleteData()
{
	_variant_t varCount;
	DATABASECONNECTION.GetConnection()->Execute((_bstr_t)GetDeleteScript(),&varCount,adOptionUnspecified);

}
void CFlightPerformanceTerminal::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	CString strFltType;
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

	importFile.getFile().getFloat(m_dFuelBurn);
	importFile.getFile().getFloat(m_dMinSpeed);
	importFile.getFile().getFloat(m_dMaxSpeed);
	importFile.getFile().getFloat(m_dMaxAcceleration);
	importFile.getFile().getFloat(m_dMaxDeceleration);

	importFile.getFile().getFloat(m_dMinSpeedHolding);
	importFile.getFile().getFloat(m_dMaxSpeedHolding);
	importFile.getFile().getFloat(m_dMaxDownVSpeed);
	importFile.getFile().getFloat(m_dMaxUpVSpeed);
	importFile.getFile().getFloat(m_dCost);

	ImportSaveData(strFltType);
	importFile.getFile().getLine();
}
void CFlightPerformanceTerminal::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_id);

	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	exportFile.getFile().writeField(szFltType);

	exportFile.getFile().writeDouble(m_dFuelBurn);
	exportFile.getFile().writeDouble(m_dMinSpeed);
	exportFile.getFile().writeDouble(m_dMaxSpeed);
	exportFile.getFile().writeDouble(m_dMaxAcceleration);
	exportFile.getFile().writeDouble(m_dMaxDeceleration);

	exportFile.getFile().writeDouble(m_dMinSpeedHolding);
	exportFile.getFile().writeDouble(m_dMaxSpeedHolding);
	exportFile.getFile().writeDouble(m_dMaxDownVSpeed);
	exportFile.getFile().writeDouble(m_dMaxUpVSpeed);
	exportFile.getFile().writeDouble(m_dCost);

	exportFile.getFile().writeLine();
}
void CFlightPerformanceTerminal::ImportSaveData(CString strFltType)
{
	_variant_t varCount;
	DATABASECONNECTION.GetConnection()->Execute((_bstr_t)ImportGetInsertScript(strFltType),&varCount,adOptionUnspecified);
}
CString CFlightPerformanceTerminal::ImportGetInsertScript(CString strFltType) const
{
	CString strSQL;
	strSQL.Format(_T("INSERT INTO FlightPerformanceTerminal \
					 (ProjID, CompID, FuelBurn, MinSpeed, MaxSpeed, MaxAcceleration, MaxDeceleration, \
					 MinSpeedHolding, MaxSpeedHolding, MaxDownVSpeed, MaxUpVSpeed, Cost) \
					 VALUES (%d,'%s',%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)"),
					 m_nProjID,strFltType,m_dFuelBurn,m_dMinSpeed,m_dMaxSpeed,m_dMaxAcceleration,
					 m_dMaxDeceleration,m_dMinSpeedHolding,m_dMaxSpeedHolding,m_dMaxDownVSpeed,m_dMaxUpVSpeed,m_dCost );

	return strSQL;
}
//////////////////////////////////////////////////////////////////////////
//class vFlightPerformanceTerminal
vFlightPerformanceTerminal::vFlightPerformanceTerminal(CAirportDatabase* pAirportDB)
{
	m_nProjID = -1;
	m_pAirportDB = pAirportDB;
}
vFlightPerformanceTerminal::~vFlightPerformanceTerminal()
{
	RemoveAll();
}
void vFlightPerformanceTerminal::ReadData(int nProjID)
{
	m_nProjID = nProjID;
	CString strSQL;
	strSQL.Format(_T("SELECT *	FROM FlightPerformanceTerminal \
		WHERE (ProjID = %d)"), m_nProjID);

	_variant_t varCount;
	_RecordsetPtr pRS = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified);

	while (!pRS->GetadoEOF())
	{ //ProjID, CompID, FuelBurn, MinSpeed, MaxSpeed, MaxAcceleration, MaxDeceleration

		CFlightPerformanceTerminal *pCruise = new CFlightPerformanceTerminal();

		_variant_t vID =pRS->GetCollect(L"ID");
			if (vID.vt != NULL)
				pCruise->setID((int)vID.intVal);

		_variant_t vProjID =  pRS->GetCollect(L"ProjID");
		if(vProjID.vt != NULL)
			pCruise->setProjID((int)vProjID.intVal);

		_variant_t vCompID=  pRS->GetCollect(L"CompID");
		if(vCompID.vt != NULL)
		{
			CString strFltType = (char*)(_bstr_t)(vCompID);
			pCruise->GetFltType().SetAirportDB(m_pAirportDB);
			pCruise->GetFltType().setConstraintWithVersion(strFltType);
			//pCruise->GetFltType() = strFltType;
		}

		_variant_t vFuelBurn=  pRS->GetCollect(L"FuelBurn");
		if(vFuelBurn.vt != NULL)
			pCruise->setFuelBurn((double)vFuelBurn.dblVal);

		_variant_t vMinSpeed=  pRS->GetCollect(L"MinSpeed");
		if(vMinSpeed.vt != NULL)
			pCruise->setMinSpeed((double)vMinSpeed.dblVal);

		_variant_t vMaxSpeed=  pRS->GetCollect(L"MaxSpeed");
		if(vMaxSpeed.vt != NULL)
			pCruise->setMaxSpeed((double)vMaxSpeed.dblVal);

		_variant_t vMaxAcceleration=  pRS->GetCollect(L"MaxAcceleration");
		if(vMaxAcceleration.vt != NULL)
			pCruise->setMaxAcceleration((double)vMaxAcceleration.dblVal);

		_variant_t vMaxDeceleration=  pRS->GetCollect(L"MaxDeceleration");
		if(vMaxDeceleration.vt != NULL)
			pCruise->setMaxDeceleration((double)vMaxDeceleration.dblVal);
		
		//MinSpeedHolding, MaxSpeedHolding, MaxDownVSpeed, MaxUpVSpeed, Cost

		_variant_t vMinSpeedHolding=  pRS->GetCollect(L"MinSpeedHolding");
		if(vMinSpeedHolding.vt != NULL)
			pCruise->setMinSpeedHolding((double)vMinSpeedHolding.dblVal);

		_variant_t vMaxSpeedHolding=  pRS->GetCollect(L"MaxSpeedHolding");
		if(vMaxSpeedHolding.vt != NULL)
			pCruise->setMaxSpeedHolding((double)vMaxSpeedHolding.dblVal);

		_variant_t vMaxDownVSpeed=  pRS->GetCollect(L"MaxDownVSpeed");
		if(vMaxDownVSpeed.vt != NULL)
			pCruise->setMaxDownVSpeed((double)vMaxDownVSpeed.dblVal);

		_variant_t vMaxUpVSpeed=  pRS->GetCollect(L"MaxUpVSpeed");
		if(vMaxUpVSpeed.vt != NULL)
			pCruise->setMaxUpVSpeed((double)vMaxUpVSpeed.dblVal);

		_variant_t vCost=  pRS->GetCollect(L"Cost");
		if(vCost.vt != NULL)
			pCruise->setCost((double)vCost.dblVal);


		push_back(pCruise);

		pRS->MoveNext();
	}

	if(size()<1)
	{
		CFlightPerformanceTerminal *pTerminal = new CFlightPerformanceTerminal();
		pTerminal->setProjID(nProjID);

		pTerminal->SaveData();
		push_back(pTerminal);
	}

}

void vFlightPerformanceTerminal::SaveData()
{
	vFlightPerformanceTerminal::iterator iter = begin();
	for (;iter != end(); ++iter)
	{
		(*iter)->setProjID(m_nProjID);
		(*iter)->SaveData();
	}
}
void vFlightPerformanceTerminal::ExportFlightPerformanceTerminals(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	vFlightPerformanceTerminal vFlightPerformanceTerminal(pAirportDB);
	vFlightPerformanceTerminal.ReadData(exportFile.GetProjectID());
	vFlightPerformanceTerminal.ExportData(exportFile);
	exportFile.getFile().endFile();
}
void vFlightPerformanceTerminal::ExportData(CAirsideExportFile& exportFile)
{	
	exportFile.getFile().writeField("FlightPerformanceTerminal");
	exportFile.getFile().writeLine();

	vFlightPerformanceTerminal::iterator iter = begin();
	for (;iter != end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}
void vFlightPerformanceTerminal::ImportFlightPerformanceTerminals(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CFlightPerformanceTerminal flightPerformanceTerminal;
		flightPerformanceTerminal.setProjID(importFile.getNewProjectID());
		flightPerformanceTerminal.ImportData(importFile);
	}
}
void vFlightPerformanceTerminal::RemoveAll()
{
	vFlightPerformanceTerminal::iterator iter = begin();
	for (;iter != end(); ++iter)
		delete *iter;
}
bool FlightTypeCompare(CFlightPerformanceTerminal* fItem,CFlightPerformanceTerminal* sItem)
{
	if (fItem->GetFltType().fits(sItem->GetFltType()))
		return false;
	else if(sItem->GetFltType().fits(fItem->GetFltType()))
		return true;
	
	return false;
}
void vFlightPerformanceTerminal::SortFlightType()
{
	std::sort(begin(),end(),FlightTypeCompare);
}

double vFlightPerformanceTerminal::GetFuelBurnByFlightTy( AirsideFlightDescStruct& _fltType )
{
	CFlightPerformanceTerminal* PFlightTerminal = NULL ;
	CFlightPerformanceTerminal* pDefaultFlightTerminal = NULL ;
	for (int i = 0 ; i < (int)size() ;i++)
	{
		PFlightTerminal = at(i) ;
		if(PFlightTerminal->GetFltType().isDefault())
		{
			pDefaultFlightTerminal = PFlightTerminal ;
			continue ;
		}
		if(PFlightTerminal->GetFltType().fits(_fltType))
			return PFlightTerminal->getFuelBurn() ;
	}
	if(pDefaultFlightTerminal != NULL)
		return pDefaultFlightTerminal->getFuelBurn() ;
	else
		return 0 ;
}