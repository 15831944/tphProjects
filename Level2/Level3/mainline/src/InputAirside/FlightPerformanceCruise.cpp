#include "StdAfx.h"
#include ".\flightperformancecruise.h"
//#include "./../Inputs/IN_TERM.H"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\Common\AirportDatabase.h"


using namespace ADODB;
using namespace ns_AirsideInput;
CFlightPerformanceCruise::CFlightPerformanceCruise(void)
{
	m_id = -1; //unique id in table 
	m_nProjID  = -1;// the project ID
	m_dFuelBurn = 1500.00;
	m_dMinSpeed = 200.00;
	m_dMaxSpeed = 340.00;
	m_dMaxAcceleration = 3.00;
	m_dMaxDeceleration = 4.00;
}

CFlightPerformanceCruise::~CFlightPerformanceCruise(void)
{
}

CString CFlightPerformanceCruise::GetUpdateScript() const
{
	CString strSQL;
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format(_T("UPDATE FlightperformanceCruise\
		SET ProjID =%d, CompID ='%s', FuelBurn =%f, MinSpeed =%f, MaxSpeed =%f, MaxAcceleration =%f, MaxDeceleration =%f \
		WHERE (ID = %d)"),m_nProjID,szFltType,m_dFuelBurn,m_dMinSpeed,m_dMaxSpeed,m_dMaxAcceleration,m_dMaxDeceleration,m_id);
	return strSQL;
}

CString CFlightPerformanceCruise::GetInsertScript() const
{
	CString strSQL;
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format(_T("INSERT INTO FlightperformanceCruise \
									 (ProjID, CompID, FuelBurn, MinSpeed, MaxSpeed, MaxAcceleration, MaxDeceleration)\
									 VALUES (%d,'%s',%f,%f,%f,%f,%f)"),
									 m_nProjID,szFltType,m_dFuelBurn,m_dMinSpeed,m_dMaxSpeed,m_dMaxAcceleration,m_dMaxDeceleration);

	return strSQL;
}

CString CFlightPerformanceCruise::GetDeleteScript() const
{
	CString strSQL;
	strSQL.Format(_T("	DELETE FROM FlightperformanceCruise \
		WHERE (ID = %d)"),m_id);
	return strSQL;
}

void CFlightPerformanceCruise::SaveData()
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
	strSQL.Format("SELECT TOP 1 ID FROM FlightperformanceCruise WHERE  ProjID= %d ORDER BY ID DESC",m_nProjID);

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

void CFlightPerformanceCruise::UpdateData()
{
	_variant_t varCount;
	DATABASECONNECTION.GetConnection()->Execute((_bstr_t)GetUpdateScript(),&varCount,adOptionUnspecified);
}

void CFlightPerformanceCruise::DeleteData()
{
	_variant_t varCount;
	DATABASECONNECTION.GetConnection()->Execute((_bstr_t)GetDeleteScript(),&varCount,adOptionUnspecified);
}

void CFlightPerformanceCruise::ImportData(CAirsideImportFile& importFile)
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

	ImportSaveData(strFltType);
	importFile.getFile().getLine();
}
void CFlightPerformanceCruise::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_id);

	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	//CString strFltType;
	//strFltType = szFltType;
	exportFile.getFile().writeField(szFltType);

	exportFile.getFile().writeDouble(m_dFuelBurn);
	exportFile.getFile().writeDouble(m_dMinSpeed);
	exportFile.getFile().writeDouble(m_dMaxSpeed);
	exportFile.getFile().writeDouble(m_dMaxAcceleration);
	exportFile.getFile().writeDouble(m_dMaxDeceleration);

	exportFile.getFile().writeLine();
}

void CFlightPerformanceCruise::ImportSaveData(CString strFltType)
{
	_variant_t varCount;
	DATABASECONNECTION.GetConnection()->Execute((_bstr_t)ImportGetInsertScript(strFltType),&varCount,adOptionUnspecified);
}
CString CFlightPerformanceCruise::ImportGetInsertScript(CString strFltType) const
{
	CString strSQL;
	strSQL.Format(_T("INSERT INTO FlightperformanceCruise \
					 (ProjID, CompID, FuelBurn, MinSpeed, MaxSpeed, MaxAcceleration, MaxDeceleration)\
					 VALUES (%d,'%s',%f,%f,%f,%f,%f)"),
					 m_nProjID,strFltType,m_dFuelBurn,m_dMinSpeed,m_dMaxSpeed,m_dMaxAcceleration,m_dMaxDeceleration);

	return strSQL;
}

//////////////////////////////////////////////////////////////////////////
//class vFlightPerformanceCruise

vFlightPerformanceCruise::vFlightPerformanceCruise(CAirportDatabase* pAirportDB)
{
	m_nProjID = -1;
	m_pAirportDB = pAirportDB;
}
vFlightPerformanceCruise::~vFlightPerformanceCruise()
{
	RemoveAll();
}
void vFlightPerformanceCruise:: ReadData(int nProjID)
{
	m_nProjID = nProjID;
	clear();

	CString strSQL;
	strSQL.Format(_T("SELECT * FROM FlightperformanceCruise \
		WHERE (ProjID = %d)"),m_nProjID);

	_variant_t varCount;
	_RecordsetPtr pRS = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified);
	
	while (!pRS->GetadoEOF())
	{ //ProjID, CompID, FuelBurn, MinSpeed, MaxSpeed, MaxAcceleration, MaxDeceleration
		
		CFlightPerformanceCruise *pCruise = new CFlightPerformanceCruise();

		_variant_t vID = pRS->GetCollect(L"ID");
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

		push_back(pCruise);

		pRS->MoveNext();
	}

	//default data
	if(this->size() < 1)
	{
		CFlightPerformanceCruise *pCruise = new CFlightPerformanceCruise();
		pCruise->setProjID(nProjID);
		pCruise->SaveData();

		push_back(pCruise);

	}
}
void vFlightPerformanceCruise:: SaveData()
{
	vFlightPerformanceCruise::iterator iter = begin();
	for (;iter != end(); ++iter)
	{
		(*iter)->setProjID(m_nProjID);
		(*iter)->SaveData();
	}
}
void vFlightPerformanceCruise::RemoveAll()
{
	vFlightPerformanceCruise::iterator iter = begin();
	for (;iter != end(); ++iter)
		delete *iter;
	clear();	
}
void vFlightPerformanceCruise::ExportFlightPerformanceCruises(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	vFlightPerformanceCruise vFlightPerformanceCruise(pAirportDB);
	vFlightPerformanceCruise.ReadData(exportFile.GetProjectID());
	vFlightPerformanceCruise.ExportData(exportFile);
	exportFile.getFile().endFile();
}
void vFlightPerformanceCruise::ExportData(CAirsideExportFile& exportFile)
{	
	exportFile.getFile().writeField("FlightPerformanceCruise");
	exportFile.getFile().writeLine();

	vFlightPerformanceCruise::iterator iter = begin();
	for (;iter != end(); ++iter)
	{
		(*iter)->ExportData(exportFile);
	}
}
void vFlightPerformanceCruise::ImportFlightPerformanceCruises(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CFlightPerformanceCruise flightPerformanceCruise;
		flightPerformanceCruise.setProjID(importFile.getNewProjectID());
		flightPerformanceCruise.ImportData(importFile);
	}
}
bool FlightTypeCompare(CFlightPerformanceCruise* fItem,CFlightPerformanceCruise* sItem)
{
	if (fItem->GetFltType().fits(sItem->GetFltType()))
		return false;
	else if(sItem->GetFltType().fits(fItem->GetFltType()))
		return true;

	return false;
}
void vFlightPerformanceCruise::SortFlightType()
{
	std::sort(begin(),end(),FlightTypeCompare);
}


double ns_AirsideInput::vFlightPerformanceCruise::GetFuelBurnByFlightType( AirsideFlightDescStruct& _constraint )
{
	CFlightPerformanceCruise* PFlightCruise = NULL ;
	CFlightPerformanceCruise* pDefaultFlightCruise = NULL ;
	for (int i = 0 ; i < (int)size() ;i++)
	{
		PFlightCruise = at(i) ;
		if(PFlightCruise->GetFltType().isDefault())
		{
			pDefaultFlightCruise = PFlightCruise ;
			continue ;
		}
		if(PFlightCruise->GetFltType().fits(_constraint))
			return PFlightCruise->getFuelBurn() ;
	}
	if(pDefaultFlightCruise != NULL)
		return pDefaultFlightCruise->getFuelBurn() ;
	else
		return 0 ;
}