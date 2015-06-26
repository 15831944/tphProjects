#include "stdafx.h"
#include "VehicleType.h"
#include "..\Database\DBElementCollection_Impl.h"

CVehicleType::CVehicleType(void)
{ 
	m_strVehicleName = _T("Undefined");
	m_bNotVehicleType = TRUE;
	m_bNotBirthTime   = TRUE;
	m_bNotDeathTime   = TRUE;
	m_bBirthTimeApplied = FALSE;
	m_bDeathTimeApplied = FALSE;
}

CVehicleType::~CVehicleType(void)
{
}

CString CVehicleType::ScreenPrint( BOOL _HasReturn /* = FALSE */) const
{
	CString csRetStr;

	static CString csNot = "NOT";
	static CString csAND = "AND";
	static CString csSpace = " ";

	CString csRetrun = "";
	if( _HasReturn )
		csRetrun = "\n";

	if( m_bNotVehicleType == FALSE )
		csRetStr += csNot + csSpace;

	csRetStr += "\"";
	CString szStr = _T("");
	szStr = m_vehicleType.getName(); 
	csRetStr += szStr;
	csRetStr = csRetStr +"\""+csRetrun;

	// BIRTH TIME
	if( m_bBirthTimeApplied )
	{
		csRetStr += csSpace + csAND + csSpace;
		if( m_bNotBirthTime == FALSE )
			csRetStr += csNot + csSpace;

		csRetStr += "\"" + m_odtStartBirthTime.Format("%H:%M") + 
			_T(" <= Birth Time <= ") + m_odtEndBirthTime.Format( "%H:%M" ) + "\""+csRetrun;
	}


	// DEATH TIME
	if( m_bDeathTimeApplied )
	{
		csRetStr += csSpace + csAND + csSpace;
		if( m_bNotDeathTime == FALSE )
			csRetStr += csNot + csSpace;

		csRetStr += "\"" + m_odtStartDeathTime.Format("%H:%M") + 
			_T(" <= Death Time <= ") + m_odtEndDeathTime.Format( "%H:%M" ) + "\""+csRetrun;

	}

	return csRetStr;
}