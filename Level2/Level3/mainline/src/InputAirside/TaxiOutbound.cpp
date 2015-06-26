#include "StdAfx.h"
#include ".\taxioutbound.h"
#include "../InputAirside/AirsideAircraft.h"
#include <algorithm>
//#include "./../Inputs/IN_TERM.H"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\Common\AirportDatabase.h"

using namespace ADODB;


/////////////////////////////////////////////////////////////
//
//CTaxiOutboundNEC
//
/////////////////////////////////////////////////////////////
CTaxiOutboundNEC::CTaxiOutboundNEC()
{
	m_nID = -1;
	m_nProjID = -1;
	m_fNormalSpeed = 10.0f;
	m_fMaxSpeed = 20.0f;
	m_fIntersectionBuffer = 50.0f;
	m_fMinSeparation = 50.0f;
	m_fMaxSeparation = 150.0f;
	m_fFuelBurn = 250.0f;
	m_fCostPerHour = 650.0f;
	m_acceleration = 0.5 ;
	m_deceleration = 0.5 ;
	m_dLongitudinalSep = 200.0f;
	m_dStaggeredSep = 150.0f;
}

CTaxiOutboundNEC::~CTaxiOutboundNEC(void)
{
}

float CTaxiOutboundNEC::GetLongitudinalSeperation()
{
	return m_dLongitudinalSep;
}

void CTaxiOutboundNEC::SetLongitudinalSeperation( float fvalue )
{
	m_dLongitudinalSep = fvalue;
}

float CTaxiOutboundNEC::GetStaggeredSeperation()
{
	return m_dStaggeredSep;
}

void CTaxiOutboundNEC::SetStaggeredSeperation( float fvalue )
{
	m_dStaggeredSep = fvalue;
}
/////////////////////////////////////////////////////////////
//
//CTaxiInbound
//
/////////////////////////////////////////////////////////////


CTaxiOutbound::CTaxiOutbound(CAirportDatabase* pAirportDB)
{
	m_nProjID = -1;
	m_pInputTerminal = NULL;
	m_pAirportDB = pAirportDB;
}

CTaxiOutbound::~CTaxiOutbound()
{
}

CString CTaxiOutbound::GetSelectScript(int nProjID)const
{
//	m_nProjID = nProjID;
	CString strSQL;
	strSQL.Format("SELECT * \
				  FROM TaxiOutboundNEC \
				  WHERE ProjID = %d", 
				  m_nProjID);
	return strSQL;
}

CString CTaxiOutbound::GetUpdateScript() const
{
	CString strSQL = _T("");
	return strSQL;
}

CString CTaxiOutbound::GetInsertScript() const
{
	CString strSQL = _T("");
	return strSQL;
}

CString CTaxiOutbound::GetDeleteScript() const
{
	CString strSQL = _T("");
	return strSQL;
}

void CTaxiOutbound::ReadData(int nProjID)
{
	m_nProjID = nProjID;
	m_vTaxiOutboundNEC.clear();

	CString strSQL = GetSelectScript(nProjID);

	_variant_t varCount;
	try
	{
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );

		while (!rsPtr->GetadoEOF())
		{
			CTaxiOutboundNEC toNec;

			_variant_t vValue;

			vValue = rsPtr->GetCollect(L"ID");
			if (vValue.vt != NULL)
				toNec.SetID(vValue.intVal);

			vValue = rsPtr->GetCollect(L"ProjID");
			if (vValue.vt != NULL)
				toNec.SetProjID(vValue.intVal);

			vValue = rsPtr->GetCollect(L"NECID");
			if(vValue.vt != NULL)
			{
				CString strFltType = (char*)(_bstr_t)(vValue);
				toNec.GetFltType().SetAirportDB(m_pAirportDB);
				toNec.GetFltType().setConstraintWithVersion(strFltType);
				//toNec.GetFltType() = strFltType;

			}

			vValue = rsPtr->GetCollect(L"NormalSpeed");
			if(vValue.vt != NULL)
				toNec.SetNormalSpeed((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"MaxSpeed");
			if (vValue.vt != NULL)
				toNec.SetMaxSpeed((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"IntersectionBuffer");
			if (vValue.vt != NULL)
				toNec.SetIntersectionBuffer((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"MinSeparation");
			if (vValue.vt != NULL)
				toNec.SetMinSeparation((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"MaxSeparation");
			if (vValue.vt != NULL)
				toNec.SetMaxSeparation((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"FuelBurn");
			if (vValue.vt != NULL)
				toNec.SetFuelBurn((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"CostPerHour");
			if (vValue.vt != NULL)
				toNec.SetCostPerHour((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"ACCELERATION");
			if (vValue.vt != NULL)
				toNec.SetAcceleration((float)vValue.dblVal);
			vValue = rsPtr->GetCollect(L"DECELERATION");
			if (vValue.vt != NULL)
				toNec.SetDeceleration((float)vValue.dblVal);

			vValue = rsPtr->GetCollect(L"LongitudinalSeparation");
			if (vValue.vt != NULL)
				toNec.SetLongitudinalSeperation((float)vValue.dblVal);
			vValue = rsPtr->GetCollect(L"StaggeredSeparation");
			if (vValue.vt != NULL)
				toNec.SetStaggeredSeperation((float)vValue.dblVal);

			m_vTaxiOutboundNEC.push_back(toNec);

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

	if(m_vTaxiOutboundNEC.size() < 1)
	{
		CTaxiOutboundNEC toNEC;
		AddRecord(toNEC);
	}
}

void CTaxiOutbound::AddRecord(CTaxiOutboundNEC& toNEC)
{
	toNEC.SetProjID(m_nProjID);

	CString strSQL;
	char szFltType[1024] = {0};
	toNEC.GetFltType().WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("INSERT INTO \
				  TaxiOutboundNEC(ProjID, NECID, NormalSpeed, MaxSpeed, IntersectionBuffer, MinSeparation, MaxSeparation, FuelBurn, CostPerHour,ACCELERATION,DECELERATION, LongitudinalSeparation, StaggeredSeparation) \
				  VALUES (%d, '%s', %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)",
				  toNEC.GetProjID(), szFltType, toNEC.GetNormalSpeed(), toNEC.GetMaxSpeed(),
				  toNEC.GetIntersectionBuffer(), toNEC.GetMinSeparation(), toNEC.GetMaxSeparation(),
				  toNEC.GetFuelBurn(),  toNEC.GetCostPerHour(),toNEC.GetAcceleration(),toNEC.GetDeceleration(),
				  toNEC.GetLongitudinalSeperation(), toNEC.GetStaggeredSeperation());

	_variant_t varCount;
	try
	{
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );

		strSQL.Format("SELECT TOP 1 ID FROM TaxiOutboundNEC ORDER BY ID DESC");

		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );

		_variant_t vValue;

		vValue = rsPtr->GetCollect(L"ID");
		if (vValue.vt != NULL)
			toNEC.SetID(vValue.intVal);

		if (rsPtr->State == adStateOpen)
			rsPtr->Close();

		AddToList(toNEC);
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}

void CTaxiOutbound::AddToList(CTaxiOutboundNEC& toNEC)
{
	m_vTaxiOutboundNEC.push_back(toNEC);
}

void CTaxiOutbound::DeleteRecord(int nItem)
{
	ASSERT(nItem >=0 && nItem < GetCount());

	DeleteRecord(m_vTaxiOutboundNEC[nItem]);
}

void CTaxiOutbound::DeleteRecord(CTaxiOutboundNEC& toNEC)
{
	CString strSQL;
	strSQL.Format("DELETE FROM TaxiOutboundNEC WHERE ID = %d", toNEC.GetID());

	_variant_t varCount;
	try
	{
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );
		if (rsPtr)
			if (rsPtr->State == adStateOpen)
				rsPtr->Close();

		DeleteFromList(toNEC);
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}

void CTaxiOutbound::DeleteFromList(CTaxiOutboundNEC& toNEC)
{
	TaxiOutboundNECList::iterator iter = m_vTaxiOutboundNEC.begin();
	for( ; iter != m_vTaxiOutboundNEC.end(); iter++)
	{
		if(iter->GetID() == toNEC.GetID())
		{
			m_vTaxiOutboundNEC.erase(iter);			
			break;
		}
	}	
}

void CTaxiOutbound::UpdateRecord(CTaxiOutboundNEC& toNEC)
{
	CString strSQL;
	char szFltType[1024] = {0};
	toNEC.GetFltType().WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("UPDATE TaxiOutboundNEC \
				  SET NECID = '%s', NormalSpeed = %.2f, MaxSpeed = %.2f, IntersectionBuffer = %.2f,\
				  MinSeparation = %.2f, MaxSeparation = %.2f, FuelBurn = %.2f, CostPerHour = %.2f ,ACCELERATION = %.2f,DECELERATION = %.2f,\
				  LongitudinalSeparation = %.2f, StaggeredSeparation = %.2f\
				  WHERE ID = %d",
				  szFltType, toNEC.GetNormalSpeed(), toNEC.GetMaxSpeed(),
				  toNEC.GetIntersectionBuffer(), toNEC.GetMinSeparation(), toNEC.GetMaxSeparation(),
				  toNEC.GetFuelBurn(),  toNEC.GetCostPerHour(),toNEC.GetAcceleration(),toNEC.GetDeceleration(),
				  toNEC.GetLongitudinalSeperation(), toNEC.GetStaggeredSeperation(),
				  toNEC.GetID());

	_variant_t varCount;

	try
	{
		_RecordsetPtr rsPtr = DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,
			&varCount,adOptionUnspecified );
		if (rsPtr)
			if (rsPtr->State == adStateOpen)
				rsPtr->Close();

		UpdateFromList(toNEC);
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return;
	}
}

void CTaxiOutbound::UpdateFromList(CTaxiOutboundNEC& toNEC)
{
	TaxiOutboundNECList::iterator iter = m_vTaxiOutboundNEC.begin();
	for( ; iter != m_vTaxiOutboundNEC.end(); iter++)
	{
		if(iter->GetID() == toNEC.GetID())
		{
			*iter = toNEC;
			break;
		}
	}	
}

CTaxiOutboundNEC* CTaxiOutbound::GetRecordByID(int nID)
{
	for(int i = 0; i < GetCount(); i++)
	{
		if(nID == m_vTaxiOutboundNEC[i].GetID())
			return &m_vTaxiOutboundNEC[i];
	}

	return NULL;
}

CTaxiOutboundNEC* CTaxiOutbound::GetRecordByIdx(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < (int)m_vTaxiOutboundNEC.size());

	return &m_vTaxiOutboundNEC[nIdx];
}
void CTaxiOutbound::ExportTaxiOutbounds(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	CTaxiOutbound taxiOutbound(pAirportDB);
	taxiOutbound.ReadData(exportFile.GetProjectID());
	taxiOutbound.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void CTaxiOutbound::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("TaxiOutbound");
	exportFile.getFile().writeLine();

	std::vector<CTaxiOutboundNEC>::iterator iter = m_vTaxiOutboundNEC.begin();
	for (;iter != m_vTaxiOutboundNEC.end(); ++iter)
	{
		exportFile.getFile().writeInt(iter->GetID());

		char szFltType[1024] = {0};
		(iter->GetFltType()).WriteSyntaxStringWithVersion(szFltType);
		exportFile.getFile().writeField(szFltType);

		exportFile.getFile().writeDouble(iter->GetNormalSpeed());
		exportFile.getFile().writeDouble(iter->GetMaxSpeed());
		exportFile.getFile().writeDouble(iter->GetIntersectionBuffer());
		exportFile.getFile().writeDouble(iter->GetMinSeparation());
		exportFile.getFile().writeDouble(iter->GetMaxSeparation());
		exportFile.getFile().writeDouble(iter->GetFuelBurn());
		exportFile.getFile().writeDouble(iter->GetCostPerHour());
		exportFile.getFile().writeLine();
	}
}
void CTaxiOutbound::ImportTaxiOutbounds(CAirsideImportFile& importFile)
{
	CTaxiOutbound taxiOutbound(NULL);
	taxiOutbound.SetProjID(importFile.getNewProjectID());
	while (!importFile.getFile().isBlank())
	{
		CTaxiOutboundNEC taxiOutboundNEC;

		int nOldID = -1;
		importFile.getFile().getInteger(nOldID);

		CString strFltType;
		importFile.getFile().getField(strFltType.GetBuffer(1024),1024);
		//taxiOutboundNEC.SetFltType(strfltType);

		float fNormalSpeed;
		importFile.getFile().getFloat(fNormalSpeed);
		taxiOutboundNEC.SetNormalSpeed(fNormalSpeed);

		float fMaxSpeed;
		importFile.getFile().getFloat(fMaxSpeed);
		taxiOutboundNEC.SetMaxSpeed(fMaxSpeed);

		float fIntersectionBuffer;
		importFile.getFile().getFloat(fIntersectionBuffer);
		taxiOutboundNEC.SetIntersectionBuffer(fIntersectionBuffer);

		float fMinSeparation;
		importFile.getFile().getFloat(fMinSeparation);
		taxiOutboundNEC.SetMinSeparation(fMinSeparation);

		float fMaxSeparation;
		importFile.getFile().getFloat(fMaxSeparation);
		taxiOutboundNEC.SetMaxSeparation(fMaxSeparation);

		float fFuelBurn;
		importFile.getFile().getFloat(fFuelBurn);
		taxiOutboundNEC.SetFuelBurn(fFuelBurn);

		float fCostPerHour;
		importFile.getFile().getFloat(fCostPerHour);
		taxiOutboundNEC.SetCostPerHour(fCostPerHour);

		taxiOutbound.ImportAddRecord(taxiOutboundNEC,strFltType);
		importFile.getFile().getLine();

	}
}

void CTaxiOutbound::ImportAddRecord(CTaxiOutboundNEC& toNEC,CString strFltType)
{
	toNEC.SetProjID(m_nProjID);

	CString strSQL;
	char szFltType[1024] = {0};
	toNEC.GetFltType().WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format("INSERT INTO \
				  TaxiOutboundNEC(ProjID, NECID, NormalSpeed, MaxSpeed, IntersectionBuffer, MinSeparation, MaxSeparation, FuelBurn, CostPerHour,ACCELERATION,DECELERATION, LongitudinalSeparation, StaggeredSeparation) \
				  VALUES (%d, '%s', %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)",
				  toNEC.GetProjID(), strFltType, toNEC.GetNormalSpeed(), toNEC.GetMaxSpeed(),
				  toNEC.GetIntersectionBuffer(), toNEC.GetMinSeparation(), toNEC.GetMaxSeparation(),
				  toNEC.GetFuelBurn(),  toNEC.GetCostPerHour(),toNEC.GetAcceleration(),toNEC.GetDeceleration(),
				  toNEC.GetLongitudinalSeperation(), toNEC.GetStaggeredSeperation());

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
bool FlightTypeCompare(CTaxiOutboundNEC& fItem,CTaxiOutboundNEC& sItem)
{
	if (fItem.GetFltType().fits(sItem.GetFltType()))
		return false;
	else if(sItem.GetFltType().fits(fItem.GetFltType()))
		return true;

	return false;
}
void CTaxiOutbound::SortFlightType()
{
	std::sort(m_vTaxiOutboundNEC.begin(),m_vTaxiOutboundNEC.end(),FlightTypeCompare);
}

double CTaxiOutbound::GetFuelBurnByFlightType( AirsideFlightDescStruct& _flttype )
{

	CTaxiOutboundNEC pTaxiInboundDefault ;
	bool isDefault = FALSE ;
	for (int n = 0; n < (int)m_vTaxiOutboundNEC.size(); n++)
	{
		CTaxiOutboundNEC pTaxiInboundNEC =m_vTaxiOutboundNEC[n];

		if(pTaxiInboundNEC.GetFltType().isDefault())
		{
			pTaxiInboundDefault = pTaxiInboundNEC ;
			isDefault = TRUE ;
			continue ;
		}
		if(pTaxiInboundNEC.GetFltType().fits(_flttype))
		{
			return pTaxiInboundNEC.GetFuelBurn() ;
		}
	}
	if(isDefault)
		return pTaxiInboundDefault.GetFuelBurn() ;
	else
		return 0 ;
}