// PaxType.cpp: implementation of the CPaxType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../common/UnitsManager.h"
#include "engine\Terminal.h"
#include "results\fltlog.h"
#include "PaxType.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxType::CPaxType()
{
	m_csName = "Undefined";
	m_isPaxConAdd = TRUE;
	m_enumAreaPortalApply = ENUM_APPLY_NONE;
	m_isBirthTimeApplied = FALSE;
	m_isDeathTimeApplied = FALSE;
	m_isAreaPortalAdd = TRUE;
	m_isBirthTimeAdd = TRUE;
	m_isDeathTimeAdd = TRUE;
}

CPaxType::CPaxType(const CPaxType& _rhs)
{
	m_csName = _rhs.m_csName;
	m_isPaxConAdd = _rhs.m_isPaxConAdd;
	m_enumAreaPortalApply = _rhs.m_enumAreaPortalApply;
	m_isBirthTimeApplied = _rhs.m_isBirthTimeApplied;
	m_isDeathTimeApplied = _rhs.m_isDeathTimeApplied;
	m_isAreaPortalAdd = _rhs.m_isAreaPortalAdd;
	m_isBirthTimeAdd = _rhs.m_isBirthTimeAdd;
	m_isDeathTimeAdd = _rhs.m_isDeathTimeAdd;
	m_isTimeApplied = _rhs.m_isTimeApplied;
	m_area = _rhs.m_area;
	m_portal = _rhs.m_portal;
	m_oEndTime = _rhs.m_oEndTime;
	m_oStartTime = _rhs.m_oStartTime;
	m_oMaxBirthTime = _rhs.m_oMaxBirthTime;
	m_oMaxDeathTime = _rhs.m_oMaxDeathTime;
	m_oMinBirthTime = _rhs.m_oMinBirthTime;
	m_oMinDeathTime = _rhs.m_oMinDeathTime;
	m_paxCon = _rhs.m_paxCon; 
}

CPaxType& CPaxType::operator =(const CPaxType& _rhs)
{
	if(this == &_rhs)
		return *this;

	m_csName = _rhs.m_csName;
	m_isPaxConAdd = _rhs.m_isPaxConAdd;
	m_enumAreaPortalApply = _rhs.m_enumAreaPortalApply;
	m_isBirthTimeApplied = _rhs.m_isBirthTimeApplied;
	m_isDeathTimeApplied = _rhs.m_isDeathTimeApplied;
	m_isAreaPortalAdd = _rhs.m_isAreaPortalAdd;
	m_isBirthTimeAdd = _rhs.m_isBirthTimeAdd;
	m_isDeathTimeAdd = _rhs.m_isDeathTimeAdd;
	m_isTimeApplied = _rhs.m_isTimeApplied;
	m_area = _rhs.m_area;
	m_portal = _rhs.m_portal;
	m_oEndTime = _rhs.m_oEndTime;
	m_oStartTime = _rhs.m_oStartTime;
	m_oMaxBirthTime = _rhs.m_oMaxBirthTime;
	m_oMaxDeathTime = _rhs.m_oMaxDeathTime;
	m_oMinBirthTime = _rhs.m_oMinBirthTime;
	m_oMinDeathTime = _rhs.m_oMinDeathTime;
	m_paxCon = _rhs.m_paxCon;

	return *this;
}

CPaxType::~CPaxType()
{

}

bool CPaxType::fit(CPaxType& _rhs ) 
{
	return m_paxCon.fits(_rhs.GetPaxCon()) ? true: false;
}

BOOL CPaxType::Matches(const MobDescStruct& pds, Terminal* pTerminal) const
{
	BOOL bOK = TRUE;
	CMobileElemConstraint pc(pTerminal);
	pc.initMobType(pds);
	FlightLogEntry flightLogEntry;
    FlightDescStruct flightDescStruct;
/*
	if(pds.arrFlight != -1) {
		pTerminal->flightLog->getItem(flightLogEntry, pds.arrFlight);
		flightLogEntry.initStruct(flightDescStruct);
		pc.initFlightType(flightDescStruct, 'A');
	}
	else if(pds.depFlight != -1) {
		pTerminal->flightLog->getItem(flightLogEntry, pds.depFlight);
		flightLogEntry.initStruct(flightDescStruct);
		pc.initFlightType(flightDescStruct, 'D');
	}
	*/  // if transfer , the depture first
	if(pds.depFlight != -1) {
		pTerminal->flightLog->getItem(flightLogEntry, pds.depFlight);
		flightLogEntry.initStruct(flightDescStruct);
		pc.initFlightType(flightDescStruct, 'D');
	}
	else if(pds.arrFlight != -1) {
		pTerminal->flightLog->getItem(flightLogEntry, pds.arrFlight);
		flightLogEntry.initStruct(flightDescStruct);
		pc.initFlightType(flightDescStruct, 'A');
	}
    if(pds.intrinsic==4 && pds.arrFlight !=-1) 
	{
		pTerminal->flightLog->getItem(flightLogEntry, pds.arrFlight);
		flightLogEntry.initStruct(flightDescStruct);
		pc.initOtherFlightType(flightDescStruct);
	}
	//check pax constraint
	if(m_paxCon.fits(pc))
		bOK = m_isPaxConAdd;
	else
		bOK = !m_isPaxConAdd;

	if(!bOK) return FALSE;

	//check area/portal constraint
	//TODO:

	long nMin, nMax;
	//check birth time
	nMin = UNITSMANAGER->ConvertFromOleTime(m_oMinBirthTime);
	nMax = UNITSMANAGER->ConvertFromOleTime(m_oMaxBirthTime);
	if(m_isBirthTimeApplied) {
		if(nMin <= pds.startTime && pds.startTime <= nMax) //match
			bOK = m_isBirthTimeAdd;
		else
			bOK = !m_isBirthTimeAdd;
	}

	if(!bOK) return FALSE;

	//check death time
	nMin = UNITSMANAGER->ConvertFromOleTime(m_oMinDeathTime);
	nMax = UNITSMANAGER->ConvertFromOleTime(m_oMaxDeathTime);
	if(m_isDeathTimeApplied) {
		if(nMin <= pds.endTime && pds.endTime <= nMax) //match
			bOK = m_isDeathTimeAdd;
		else
			bOK = !m_isDeathTimeAdd;
	}	

	if(!bOK) return FALSE;
	
	return TRUE;
}

CString CPaxType::ScreenPrint( BOOL _HasReturn ) const
{
	CString csRetStr;

	static CString csNot = "NOT";
	static CString csAND = "AND";
	static CString csSpace = " ";

	CString csRetrun = "";
	if( _HasReturn )
		csRetrun = "\n";

	if( m_isPaxConAdd == FALSE )
		csRetStr += csNot + csSpace;

	csRetStr += "\"";
	CString szStr;//[256];
	//m_paxCon.screenPrint( szStr, 256 );
	m_paxCon.screenPrint( szStr, 0, 256 );
	csRetStr += szStr;//CString( szStr );
	csRetStr = csRetStr +"\""+csRetrun;

	if( m_enumAreaPortalApply == ENUM_APPLY_AREA )
	{
		csRetStr += csSpace + csAND + csSpace;
		if( m_isAreaPortalAdd == FALSE )
			csRetStr += csNot + csSpace;

		csRetStr += "\" Area: " + m_area.name;

		if( m_isTimeApplied )
			csRetStr += " in [" + m_oStartTime.Format( "%H:%M" ) + "-" + m_oEndTime.Format( "%H:%M" ) + "]";

		csRetStr = csRetStr +"\""+csRetrun;
	}
	else if( m_enumAreaPortalApply == ENUM_APPLY_PORTAL )
	{
		csRetStr += csSpace + csAND + csSpace;
		if( m_isAreaPortalAdd == FALSE )
			csRetStr += csNot + csSpace;

		csRetStr += "\" Portal: " + m_portal.name;

		if( m_isTimeApplied )
			csRetStr += " in [" + m_oStartTime.Format( "%H:%M" ) + "-" + m_oEndTime.Format( "%H:%M" ) + "]";

		csRetStr = csRetStr +"\""+csRetrun;
	}

	// BIRTH TIME
	if( m_isBirthTimeApplied )
	{
		csRetStr += csSpace + csAND + csSpace;
		if( m_isBirthTimeAdd == FALSE )
			csRetStr += csNot + csSpace;

		csRetStr += "\"" + m_oMinBirthTime.Format("%H:%M") + 
			" <= Birth Time <= " + m_oMaxBirthTime.Format( "%H:%M" ) + "\""+csRetrun;
	}


	// DEATH TIME
	if( m_isDeathTimeApplied )
	{
		csRetStr += csSpace + csAND + csSpace;
		if( m_isDeathTimeAdd == FALSE )
			csRetStr += csNot + csSpace;

		csRetStr += "\"" + m_oMinDeathTime.Format("%H:%M") + 
			" <= Death Time <= " + m_oMaxDeathTime.Format( "%H:%M" ) + "\""+csRetrun;

	}

	return csRetStr;
}


void CPaxType::SetStartTime(COleDateTime _dt)
{
	m_oStartTime = _dt;
}

void CPaxType::SetEndTime(COleDateTime _dt)
{
	m_oEndTime = _dt;
}

void CPaxType::SetMinBirthTime(COleDateTime _dt)
{
	m_oMinBirthTime = _dt;
}

void CPaxType::SetMaxBirthTime(COleDateTime _dt)
{
	m_oMaxBirthTime = _dt;
}

void CPaxType::SetMinDeathTime(COleDateTime _dt)
{
	m_oMinDeathTime = _dt;
}

void CPaxType::SetMaxDeathTime(COleDateTime _dt)
{
	m_oMaxDeathTime = _dt;
}

void CPaxType::SetIsPaxConAdd(BOOL _bAdd )
{
	m_isPaxConAdd = _bAdd;
}

void CPaxType::SetAreaportalApplyType( AREA_PORTAL_APPLY_TYPE _enumAreaPortalApply )
{
	m_enumAreaPortalApply = _enumAreaPortalApply;
}

void CPaxType::SetIsAreaPortalAdd(BOOL _bAdd )
{
	m_isAreaPortalAdd = _bAdd;
}

void CPaxType::SetIsTimeApplied(BOOL _bAdd )
{
	m_isTimeApplied = _bAdd;
}

void CPaxType::SetIsBirthTimeApplied(BOOL _bAdd )
{
	m_isBirthTimeApplied = _bAdd;
}

void CPaxType::SetIsBirthTimeAdd(BOOL _bAdd )
{
	m_isBirthTimeAdd = _bAdd;
}

void CPaxType::SetIsDeathTimeApplied(BOOL _bAdd )
{
	m_isDeathTimeApplied = _bAdd;
}

void CPaxType::SetIsDeathTimeAdd(BOOL _bAdd )
{
	m_isDeathTimeAdd = _bAdd;
}
void CPaxType::SetArea( const CArea& _area )
{
	m_area = _area;
}

void CPaxType::SetPortal( const CPortal& _portal)
{
	m_portal = _portal;
}