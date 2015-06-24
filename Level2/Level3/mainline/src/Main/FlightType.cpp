// PaxType.cpp: implementation of the CFlightType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlightType.h"
#include "../common/UnitsManager.h"

#include "engine\Terminal.h"
#include "results\fltlog.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlightType::CFlightType()
{
	m_csName = "Undefined";
	m_isFlightConstraintAdd = TRUE;
	m_isBirthTimeApplied = FALSE;
	m_isDeathTimeApplied = FALSE;
	m_isBirthTimeAdd = TRUE;
	m_isDeathTimeAdd = TRUE;
}

CFlightType::CFlightType(const CFlightType& _rhs)
{
	m_csName = _rhs.m_csName;
	m_isFlightConstraintAdd = _rhs.m_isFlightConstraintAdd;
	m_isBirthTimeApplied = _rhs.m_isBirthTimeApplied;
	m_isDeathTimeApplied = _rhs.m_isDeathTimeApplied;
	m_isBirthTimeAdd = _rhs.m_isBirthTimeAdd;
	m_isDeathTimeAdd = _rhs.m_isDeathTimeAdd;
	m_isTimeApplied = _rhs.m_isTimeApplied;
	m_oEndTime = _rhs.m_oEndTime;
	m_oStartTime = _rhs.m_oStartTime;
	m_oMaxBirthTime = _rhs.m_oMaxBirthTime;
	m_oMaxDeathTime = _rhs.m_oMaxDeathTime;
	m_oMinBirthTime = _rhs.m_oMinBirthTime;
	m_oMinDeathTime = _rhs.m_oMinDeathTime;
	m_flightConstraint = _rhs.m_flightConstraint; 
}

CFlightType& CFlightType::operator = (const CFlightType& _rhs)
{
	if(this == &_rhs)
		return *this;

	m_csName = _rhs.m_csName;
	m_isFlightConstraintAdd = _rhs.m_isFlightConstraintAdd;
	m_isBirthTimeApplied = _rhs.m_isBirthTimeApplied;
	m_isDeathTimeApplied = _rhs.m_isDeathTimeApplied;
	m_isBirthTimeAdd = _rhs.m_isBirthTimeAdd;
	m_isDeathTimeAdd = _rhs.m_isDeathTimeAdd;
	m_isTimeApplied = _rhs.m_isTimeApplied;
	m_oEndTime = _rhs.m_oEndTime;
	m_oStartTime = _rhs.m_oStartTime;
	m_oMaxBirthTime = _rhs.m_oMaxBirthTime;
	m_oMaxDeathTime = _rhs.m_oMaxDeathTime;
	m_oMinBirthTime = _rhs.m_oMinBirthTime;
	m_oMinDeathTime = _rhs.m_oMinDeathTime;
	m_flightConstraint = _rhs.m_flightConstraint;

	return *this;
}

CFlightType::~CFlightType()
{

}

BOOL CFlightType::Matches(const AirsideFlightDescStruct& fds,bool bARR, Terminal* pTerminal) const
{
	char mode = 'T';
	if (fds._arrID.HasValue() && bARR)
	{
		mode = 'A';
	}
	if (fds._depID.HasValue() && !bARR)
	{
		mode = 'D';
	}

	if(!m_flightConstraint.fits(fds,mode))
		return (FALSE);
	else
		return TRUE;
}

CString CFlightType::ScreenPrint( BOOL _HasReturn ) const
{
	CString csRetStr;

	static CString csNot = "NOT";
	static CString csAND = "AND";
	static CString csSpace = " ";

	CString csRetrun = "";
	if( _HasReturn )
		csRetrun = "\n";

	if( m_isFlightConstraintAdd == FALSE )
		csRetStr += csNot + csSpace;

	csRetStr += "\"";
	CString szStr;
	m_flightConstraint.screenPrint( szStr, 0, 256 );
	csRetStr += szStr;
	csRetStr = csRetStr +"\""+csRetrun;

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


void CFlightType::SetStartTime(COleDateTime _dt)
{
	m_oStartTime = _dt;
}

void CFlightType::SetEndTime(COleDateTime _dt)
{
	m_oEndTime = _dt;
}

void CFlightType::SetMinBirthTime(COleDateTime _dt)
{
	m_oMinBirthTime = _dt;
}

void CFlightType::SetMaxBirthTime(COleDateTime _dt)
{
	m_oMaxBirthTime = _dt;
}

void CFlightType::SetMinDeathTime(COleDateTime _dt)
{
	m_oMinDeathTime = _dt;
}

void CFlightType::SetMaxDeathTime(COleDateTime _dt)
{
	m_oMaxDeathTime = _dt;
}

void CFlightType::SetIsFlightConstraintAdd(BOOL _bAdd )
{
	m_isFlightConstraintAdd = _bAdd;
}

void CFlightType::SetIsTimeApplied(BOOL _bAdd )
{
	m_isTimeApplied = _bAdd;
}

void CFlightType::SetIsBirthTimeApplied(BOOL _bAdd )
{
	m_isBirthTimeApplied = _bAdd;
}

void CFlightType::SetIsBirthTimeAdd(BOOL _bAdd )
{
	m_isBirthTimeAdd = _bAdd;
}

void CFlightType::SetIsDeathTimeApplied(BOOL _bAdd )
{
	m_isDeathTimeApplied = _bAdd;
}

void CFlightType::SetIsDeathTimeAdd(BOOL _bAdd )
{
	m_isDeathTimeAdd = _bAdd;
}
