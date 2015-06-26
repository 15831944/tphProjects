#include "StdAfx.h"
#include ".\taxiinbound.h"
#include "../InputAirside/AirsideAircraft.h"
#include <algorithm>
//#include "./../Inputs/IN_TERM.H"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\Common\AirportDatabase.h"
using namespace ADODB;

/////////////////////////////////////////////////////////////
//
//CTaxiInboundNEC
//
/////////////////////////////////////////////////////////////
CTaxiInboundNEC::CTaxiInboundNEC(void)
{
	m_nID = -1;
	m_nProjID = -1;
	m_fNormalSpeed = 10.0f;
	m_fMaxSpeed = 20.0f;
	m_fIntersectionBuffer = 50.0f;
	m_fFuelBurn = 250.0f;
	m_fCostPerHour = 650.0f;
	m_fMinSeparation = 50.0f;
	m_fMaxSeparation = 150.0f;
	m_acceleration = 0.5 ;
	m_deceleration = 0.5 ;
	m_dLongitudinalSep = 200.0f;
	m_dStaggeredSep = 150.0f;
}

CTaxiInboundNEC::~CTaxiInboundNEC(void)
{
}

float CTaxiInboundNEC::GetLongitudinalSeperation()
{
	return m_dLongitudinalSep;
}

void CTaxiInboundNEC::SetLongitudinalSeperation( float fvalue )
{
	m_dLongitudinalSep = fvalue;
}

float CTaxiInboundNEC::GetStaggeredSeperation()
{
	return m_dStaggeredSep;
}

void CTaxiInboundNEC::SetStaggeredSeperation( float fvalue )
{
	m_dStaggeredSep = fvalue;
}
/////////////////////////////////////////////////////////////
//
//CTaxiInbound
//
/////////////////////////////////////////////////////////////
CTaxiInboundNECManager::CTaxiInboundNECManager()
{
	m_pInputTerminal = NULL;
}

CTaxiInboundNECManager::~CTaxiInboundNECManager()
{
}
CString CTaxiInboundNECManager::GetUpdateScript() const
{
	CString strSQL;
	//strSQL.Format("UPDATE TaxiInboundNEC \
	//			  SET MinSpeed = %.2f, MaxSpeed = %.2f, CostPerHour \
	//			  WHERE ID = %d", \
	//			  m_fTaxiway, m_fRunway, m_nID);
	return strSQL;
}

CString CTaxiInboundNECManager::GetSelectScript()const
{
//	m_nProjID = nProjID;
	CString strSQL;
	strSQL.Format("SELECT * \
				  FROM TaxiInboundNEC \
				  WHERE ProjID = %d", 
				  m_nProjID);
	return strSQL;
}

CString CTaxiInboundNECManager::GetInsertScript()const
{
	CString strSQL;
	//strSQL.Format("INSERT INTO \
	//			  TaxiInboundNEC(ProjID, NECID, MinSpeed, MaxSpeed, CostPerHour)\
	//			  VALUES (%d, %d, %.2f, %.2f, %.2f)", m_nProjID, m_fTaxiway, m_fRunway);

	return strSQL;
}

CString CTaxiInboundNECManager::GetDeleteScript()const
{
	CString strSQL;
	strSQL.Format("DELETE FROM TaxiInboundNEC WHERE ProjID = %d", m_nProjID);
	return strSQL;
}

void CTaxiInboundNECManager::ReadData(CAirportDatabase* pAirportDB)
{
//	m_nProjID = nProjID;
	m_vTaxiInboundNEC.clear();

	CString strSQL = GetSelectScript();

	_variant_t varCount;
	try
	{
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );

		while (!rsPtr->GetadoEOF())
		{
			CTaxiInboundNEC tiNec;

			_variant_t vValue;

			vValue = rsPtr->GetCollect(L"ID");
			if (vValue.vt != NULL)
				tiNec.SetID(vValue.intVal);

			vValue = rsPtr->GetCollect(L"ProjID");
			if (vValue.vt != NULL)
				tiNec.SetProjID(vValue.intVal);

			vValue = rsPtr->GetCollect(L"NECID");
			if(vValue.vt != NULL)
			{
				CString strFltType = (char*)(_bstr_t)(vValue);
				tiNec.GetFltType().SetAirportDB(pAirportDB);
				tiNec.GetFltType().setConstraintWithVersion(strFltType);
				//tiNec.GetFltType() = strFltType;
			}

			vValue = rsPtr->GetCollect(L"NormalSpeed");
			if (vValue.vt != NULL)
				tiNec.SetNormalSpeed((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"MaxSpeed");
			if (vValue.vt != NULL)
				tiNec.SetMaxSpeed((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"IntersectionBuffer");
			if (vValue.vt != NULL)
				tiNec.SetIntersectionBuffer((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"FuelBurn");
			if (vValue.vt != NULL)
				tiNec.SetFuelBurn((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"CostPerHour");
			if (vValue.vt != NULL)
				tiNec.SetCostPerHour((float)vValue.dblVal);
//////////////////////////////////////////////////////////////////////////
			vValue = rsPtr->GetCollect(L"ACCELERATION");
			if (vValue.vt != NULL)
				tiNec.SetAcceleration((float)vValue.dblVal);
			vValue = rsPtr->GetCollect(L"DECELERATION");
			if (vValue.vt != NULL)
				tiNec.SetDeceleration((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"MinSeparation");
			if (vValue.vt != NULL)
				tiNec.SetMinSeparation((float)vValue.dblVal);
			vValue = rsPtr->GetCollect(L"MaxSeparation");
			if (vValue.vt != NULL)
				tiNec.SetMaxSeparation((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"LongitudinalSeparation");
			if (vValue.vt != NULL)
				tiNec.SetLongitudinalSeperation((float)vValue.dblVal);
			vValue = rsPtr->GetCollect(L"StaggeredSeparation");
			if (vValue.vt != NULL)
				tiNec.SetStaggeredSeperation((float)vValue.dblVal);
//////////////////////////////////////////////////////////////////////////
			m_vTaxiInboundNEC.push_back(tiNec);

			rsPtr->MoveNext();
		}

		if (rsPtr)
			if (rsPtr->State == adStateOpen)
				rsPtr->Close();
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}

	if(m_vTaxiInboundNEC.size() < 1)
	{
		CTaxiInboundNEC tiNEC;
		tiNEC.SetProjID( GetProjID() );
		AddRecord(tiNEC);
	}
}

void CTaxiInboundNECManager::SaveData()
{
	CString strSQL = GetUpdateScript();

	//_variant_t varCount;
	//try
	//{
	//	_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
	//		&varCount,adOptionUnspecified );

	//	if (rsPtr)
	//		if (rsPtr->State == adStateOpen)
	//			rsPtr->Close();
	//}
	//catch (_com_error &e)
	//{
	//	CString strError = e.ErrorMessage();
	//	return;
	//}
}

class compareID
{
public:

	bool operator()(CTaxiInboundNEC& TaxiInboundNEC)
	{
	//	if (TaxiInboundNEC.GetFltType().isDefault())
			return true;
	//	return false;
	}
};

void CTaxiInboundNECManager::DeleteData()
{
	CString strSQL = GetDeleteScript();
	_variant_t varCount;
	try
	{
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );
		if (rsPtr)
			if (rsPtr->State == adStateOpen)
				rsPtr->Close();

		//int nCount = (int)m_vTaxiInboundNEC.size();
		//for(int i = 0; i < nCount; i++)
		//{
		//	if(iter->GetNECID() == -1)
		//	{
		//		m_vTaxiInboundNEC.erase(m_vTaxiInboundNEC.begin()+i);
		//		i--;
		//		nCount--;
		//	}
		//}

		TaxiInboundNECList::iterator iterLast = std::remove_if(m_vTaxiInboundNEC.begin(), m_vTaxiInboundNEC.end(), compareID());
		m_vTaxiInboundNEC.erase(iterLast, m_vTaxiInboundNEC.end());
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}
CTaxiInboundNEC* CTaxiInboundNECManager::GetRecordByID(int nID)
{
	int nCount = (int)m_vTaxiInboundNEC.size();
	if(nCount <= 0)
		return NULL;

	for(int i = 0; i < (int)m_vTaxiInboundNEC.size(); i++)
	{
		if(nID == m_vTaxiInboundNEC[i].GetID())
			return &m_vTaxiInboundNEC[i];
	}

	return NULL;
}

CTaxiInboundNEC* CTaxiInboundNECManager::GetRecordByIdx(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < (int)m_vTaxiInboundNEC.size());

	return &m_vTaxiInboundNEC[nIdx];
}


void CTaxiInboundNECManager::AddRecord(CTaxiInboundNEC& tiNEC)
{
	CString strSQL;
	char szFltType[1024] = {0};
	tiNEC.GetFltType().WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("INSERT INTO \
				  TaxiInboundNEC(ProjID, NECID, NormalSpeed, MaxSpeed, IntersectionBuffer, FuelBurn, CostPerHour,ACCELERATION,DECELERATION,MinSeparation, MaxSeparation, LongitudinalSeparation, StaggeredSeparation)\
				  VALUES (%d, '%s', %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)",
				  tiNEC.GetProjID(), szFltType, tiNEC.GetNormalSpeed(), 
				  tiNEC.GetMaxSpeed(), tiNEC.GetIntersectionBuffer(), tiNEC.GetFuelBurn(), tiNEC.GetCostPerHour(),
				  tiNEC.GetAcceleration(),tiNEC.GetDeceleration(),tiNEC.GetMinSeparation(),tiNEC.GetMaxSeparation(),
				  tiNEC.GetLongitudinalSeperation(), tiNEC.GetStaggeredSeperation());

	_variant_t varCount;
	try
	{
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );

		strSQL.Format("SELECT TOP 1 ID FROM TaxiInboundNEC ORDER BY ID DESC");

		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );

		_variant_t vValue;

		vValue = rsPtr->GetCollect(L"ID");
		if (vValue.vt != NULL)
			tiNEC.SetID(vValue.intVal);

		if (rsPtr->State == adStateOpen)
			rsPtr->Close();

		m_vTaxiInboundNEC.push_back(tiNEC);
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}

void CTaxiInboundNECManager::DeleteRecord(int nItem)
{
	if(nItem < 0 || m_vTaxiInboundNEC.empty())
		return ;

	int nID = m_vTaxiInboundNEC[nItem].GetID();

	CString strSQL;
	strSQL.Format("DELETE FROM TaxiInboundNEC WHERE ID = %d", nID);

	_variant_t varCount;
	try
	{
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );
		if (rsPtr)
			if (rsPtr->State == adStateOpen)
				rsPtr->Close();
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}

	m_vTaxiInboundNEC.erase(m_vTaxiInboundNEC.begin()+nItem);
}

void CTaxiInboundNECManager::UpdateItem(CTaxiInboundNEC& tiNEC)
{
	CString strSQL;
	char szFltType[1024] = {0};
	tiNEC.GetFltType().WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("UPDATE TaxiInboundNEC \
					SET NECID = '%s', NormalSpeed = %.2f, MaxSpeed = %.2f, IntersectionBuffer = %.2f, FuelBurn = %.2f,  CostPerHour = %.2f ,\
					ACCELERATION = %.2f ,DECELERATION = %.2f ,MinSeparation = %.2f , MaxSeparation = %.2f,\
					LongitudinalSeparation = %.2f, StaggeredSeparation = %.2f\
					WHERE ID = %d", \
					szFltType, tiNEC.GetNormalSpeed(), 
					tiNEC.GetMaxSpeed(), tiNEC.GetIntersectionBuffer(), tiNEC.GetFuelBurn(), tiNEC.GetCostPerHour(),
					tiNEC.GetAcceleration(),tiNEC.GetDeceleration(),tiNEC.GetMinSeparation(),tiNEC.GetMaxSeparation(),
					tiNEC.GetLongitudinalSeperation(), tiNEC.GetStaggeredSeperation(),
					tiNEC.GetID());
	
	try
	{
		_variant_t varCount;
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL, &varCount,adOptionUnspecified);
		if (rsPtr)
			if (rsPtr->State == adStateOpen)
				rsPtr->Close();

		TaxiInboundNECList::iterator iter = m_vTaxiInboundNEC.begin();
		for( ; iter != m_vTaxiInboundNEC.end(); iter++)
		{
			if(iter->GetID() == tiNEC.GetID())
			{
				//iter->SetFltType(tiNEC.GetFltType());
				//iter->SetMinSpeed(tiNEC.GetMinSpeed());
				//iter->SetMaxSpeed(tiNEC.GetMaxSpeed());
				//iter->SetCostPerHour(tiNEC.GetCostPerHour());
				*iter = tiNEC;
				break;
			}
		}	
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}

void CTaxiInboundNECManager::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(static_cast<int>(m_vTaxiInboundNEC.size()));
	std::vector<CTaxiInboundNEC>::iterator iter = m_vTaxiInboundNEC.begin();
	for (;iter != m_vTaxiInboundNEC.end(); ++iter)
	{
		exportFile.getFile().writeInt(iter->GetID());
		char szFltType[1024] = {0};
		(iter->GetFltType()).WriteSyntaxStringWithVersion(szFltType);
		exportFile.getFile().writeField(szFltType);
		exportFile.getFile().writeDouble(iter->GetMaxSpeed());
		exportFile.getFile().writeDouble(iter->GetNormalSpeed());
		exportFile.getFile().writeDouble(iter->GetIntersectionBuffer());
		exportFile.getFile().writeDouble(iter->GetFuelBurn());
		exportFile.getFile().writeDouble(iter->GetCostPerHour());

		exportFile.getFile().writeLine();
	}
}
void CTaxiInboundNECManager::ImportData(CAirsideImportFile& importFile)
{
	int nTaxiInboundNECCount = 0;
	importFile.getFile().getInteger(nTaxiInboundNECCount);
	for (int i =0; i < nTaxiInboundNECCount; ++i)
	{
		CTaxiInboundNEC taxiInboundNEC;

		int nOldID = -1;
		importFile.getFile().getInteger(nOldID);

		CString strFltType;
		importFile.getFile().getField(strFltType.GetBuffer(1024),1024);
		//taxiInboundNEC.SetFltType(strFltType);

		float fMaxSpeed;
		importFile.getFile().getFloat(fMaxSpeed);
		taxiInboundNEC.SetMaxSpeed(fMaxSpeed);

		float fNormalSpeed;
		importFile.getFile().getFloat(fNormalSpeed);
		taxiInboundNEC.SetNormalSpeed(fNormalSpeed);

		float fIntersectionBuffer;
		importFile.getFile().getFloat(fIntersectionBuffer);
		taxiInboundNEC.SetIntersectionBuffer(fIntersectionBuffer);

		float fFuelBurn;
		importFile.getFile().getFloat(fFuelBurn);
		taxiInboundNEC.SetFuelBurn(fFuelBurn);

		float fCostPerHour;
		importFile.getFile().getFloat(fCostPerHour);
		taxiInboundNEC.SetCostPerHour(fCostPerHour);

		taxiInboundNEC.SetProjID(importFile.getNewProjectID());

		ImportAddRecord(taxiInboundNEC,strFltType);

		importFile.getFile().getLine();
	}
}
void CTaxiInboundNECManager::ImportAddRecord(CTaxiInboundNEC& tiNEC,CString strFltType)
{
	CString strSQL;
	strSQL.Format("INSERT INTO \
				  TaxiInboundNEC(ProjID, NECID, NormalSpeed, MaxSpeed, IntersectionBuffer, FuelBurn, CostPerHour,ACCELERATION,DECELERATION,MinSeparation, MaxSeparation, LongitudinalSeparation, StaggeredSeparation)\
				  VALUES (%d, '%s', %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)",
				  tiNEC.GetProjID(), strFltType, tiNEC.GetNormalSpeed(), 
				  tiNEC.GetMaxSpeed(), tiNEC.GetIntersectionBuffer(), tiNEC.GetFuelBurn(), tiNEC.GetCostPerHour(),
				  tiNEC.GetAcceleration(),tiNEC.GetDeceleration(),tiNEC.GetMinSeparation(),tiNEC.GetMaxSeparation(),
				  tiNEC.GetLongitudinalSeperation(), tiNEC.GetStaggeredSeperation());

	_variant_t varCount;
	try
	{
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}

bool FlightTypeCompare(CTaxiInboundNEC& fItem,CTaxiInboundNEC& sItem)
{
	if (fItem.GetFltType().fits(sItem.GetFltType()))
		return false;
	else if (sItem.GetFltType().fits(fItem.GetFltType()))
		return true;

	return false;
}
void CTaxiInboundNECManager::SortFlightType()
{
	std::sort(m_vTaxiInboundNEC.begin(),m_vTaxiInboundNEC.end(),FlightTypeCompare);
}
/////////////////////////////////////////////////////////////
//
//CTaxiInbound
//
/////////////////////////////////////////////////////////////
CTaxiInbound::CTaxiInbound(CAirportDatabase* pAirportDB)
{
	m_nID = -1;
	m_nProjID = -1;
	m_fRunway = 0.0;
	m_fTaxiway = 0.0;
	m_pCompCatManager = NULL;
	m_pAirportDB = pAirportDB;
}

CTaxiInbound::CTaxiInbound(CAACompCatManager* pManager,bool flag)
{
	m_nID = -1;
	m_nProjID = -1;
	m_fRunway = 0.0;
	m_fTaxiway = 0.0;
	m_pCompCatManager = pManager;
}

CTaxiInbound::~CTaxiInbound(void)
{
}


CString CTaxiInbound::GetUpdateScript() const
{
	CString strSQL;
	strSQL.Format("UPDATE TaxiInbound \
				  SET TaxiwayIC = %.2f, RunwayIC = %.2f \
				  WHERE ID = %d", \
				  m_fTaxiway, m_fRunway, m_nID);
	return strSQL;
}

CString CTaxiInbound::GetSelectScript(int nProjID)const
{
//	m_nProjID = nProjID;
	CString strSQL;
	strSQL.Format("SELECT * \
				  FROM TaxiInbound \
				  WHERE ProjID = %d", 
				  m_nProjID);
	return strSQL;
}

CString CTaxiInbound::GetInsertScript()const
{
	CString strSQL;
	strSQL.Format("INSERT INTO \
				  TaxiInbound(ProjID, TaxiwayIC, RunwayIC)\
				  VALUES (%d, %.2f, %.2f)", m_nProjID, m_fTaxiway, m_fRunway);

	return strSQL;
}

CString CTaxiInbound::GetDeleteScript()const
{
	CString strSQL;
	strSQL.Format("DELETE FROM TaxiInbound WHERE ID = %d", m_nID);
	return strSQL;
}

void CTaxiInbound::ReadData(int nProjID)
{
	m_nProjID = nProjID;
	CString strSQL = GetSelectScript(nProjID);

	_variant_t varCount;
	try
	{
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );

		if (!rsPtr->GetadoEOF())
		{
			_variant_t vValue;

			vValue = rsPtr->GetCollect(L"ID");
			if (vValue.vt != NULL)
				m_nID = vValue.intVal;

			vValue = rsPtr->GetCollect(L"TaxiwayIC");
			if(vValue.vt != NULL)
				m_fTaxiway = (float)vValue.dblVal;

			vValue = rsPtr->GetCollect(L"RunwayIC");
			if (vValue.vt != NULL)
				m_fRunway = (float)vValue.dblVal;

			m_TaxiInboundNECManager.SetProjID(m_nProjID);
			m_TaxiInboundNECManager.SetCompCatManager(m_pCompCatManager);
			m_TaxiInboundNECManager.ReadData(m_pAirportDB);
		}
		else
		{
			strSQL.Empty();
			strSQL = GetInsertScript();

			DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
				&varCount,adOptionUnspecified );	
			//Init ID;
			ReadData(nProjID);
		}

		if (rsPtr)
			if (rsPtr->State == adStateOpen)
				rsPtr->Close();
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}

void CTaxiInbound::SaveData()
{
	CString strSQL = GetUpdateScript();

	_variant_t varCount;
	try
	{
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );

		if (rsPtr)
			if (rsPtr->State == adStateOpen)
				rsPtr->Close();
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}

void CTaxiInbound::DeleteData()
{
	CString strSQL = GetDeleteScript();
	_variant_t varCount;
	try
	{
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );
		if (rsPtr)
			if (rsPtr->State == adStateOpen)
				rsPtr->Close();
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}

void CTaxiInbound::AddRecord(CTaxiInboundNEC& tiNEC)
{
	tiNEC.SetProjID(m_nProjID);
	m_TaxiInboundNECManager.AddRecord(tiNEC);
}

void CTaxiInbound::DeleteRecord(int nItem)
{
	m_TaxiInboundNECManager.DeleteRecord(nItem);
}
void CTaxiInbound::ExportTaxiInbounds(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	CTaxiInbound TaxiInbound(pAirportDB);
	TaxiInbound.ReadData(exportFile.GetProjectID());
	TaxiInbound.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void CTaxiInbound::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("TaxiInbound");
	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeFloat(m_fTaxiway);
	exportFile.getFile().writeFloat(m_fRunway);

	exportFile.getFile().writeLine();

	m_TaxiInboundNECManager.ExportData(exportFile);
}
void CTaxiInbound::ImportTaxiInbounds(CAirsideImportFile& importFile)
{	
	while (!importFile.getFile().isBlank())
	{
		CTaxiInbound taxiInbound(NULL);
		taxiInbound.SetProjID(importFile.getNewProjectID());

		int nOldID = -1;
		importFile.getFile().getInteger(nOldID);

		importFile.getFile().getFloat(taxiInbound.m_fTaxiway);
		importFile.getFile().getFloat(taxiInbound.m_fRunway);

		_variant_t varCount;
		CString strSQL = taxiInbound.GetInsertScript();

		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );	

		importFile.getFile().getLine();

		taxiInbound.ImportData(importFile);
	}
}
void CTaxiInbound::ImportData(CAirsideImportFile& importFile)
{
	m_TaxiInboundNECManager.ImportData(importFile);
}

double CTaxiInbound::GetFuelBurnByFlightType( AirsideFlightDescStruct& _flttype )
{
	CTaxiInboundNECManager* pManager = GetTaxiInboundNECList();
	CTaxiInboundNEC* pTaxiInboundDefault = NULL ;
	for (int n = 0; n < pManager->GetCount(); n++)
	{
		CTaxiInboundNEC* pTaxiInboundNEC = pManager->GetRecordByIdx(n);

		if(pTaxiInboundNEC->GetFltType().isDefault())
		{
			pTaxiInboundDefault = pTaxiInboundNEC ;
			continue ;
		}
		if(pTaxiInboundNEC->GetFltType().fits(_flttype))
		{
			return pTaxiInboundNEC->GetFuelBurn() ;
		}
	}
	if(pTaxiInboundDefault)
		return pTaxiInboundDefault->GetFuelBurn() ;
	else
		return 0 ;
}