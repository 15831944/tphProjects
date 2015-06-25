// UnitsManager.cpp: implementation of the CUnitsManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../common/UnitsManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// internal units are centimeters for length, kilos for weight
static CString	LengthStr[8] = { _T("km"), _T("m"), _T("cm"), _T("mm"), _T("mi"), _T("ft"), _T("in"), _T("nm") };
static CString LengthLongStr[8] = { _T("kilometers"), _T("meters"), _T("centimeters"), _T("millimeters"), _T("miles"), _T("feet"), _T("inches"), _T("nautical miles") };
static double LengthConversion[8][2] = { // ExtToInt, IntToExt
		100000.0, 0.00001,
		100.0, 0.01,
		1.0, 1.0,
		0.1, 10.0,
		160934.4, 0.0000062137119,
		30.48, 0.03280839895,
		2.54, 0.3937,
		185200.0, 0.000005399568, 
};
static CString WeightStr[2] = { _T("kg"), _T("lbs") };
static CString WeightLongStr[2] = { _T("kilogram"), _T("pounds") };
static double WeightConversion[2][2] = { // ExtToInt, IntToExt
	1.0, 1.0,
	0.4536, 2.205
};

#define TIMETOHOURDIV	360000
#define TIMETOMINDIV	6000
#define TIMETOSECDIV	100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUnitsManager* CUnitsManager::m_pInstance = NULL;

CUnitsManager::CUnitsManager()
{
	//LoadSettings();
}

CUnitsManager::~CUnitsManager()
{
	//SaveSettings();
}

CUnitsManager* CUnitsManager::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CUnitsManager();
	}
	return m_pInstance;
}

void CUnitsManager::DeleteInstance()
{
	if(m_pInstance)
		delete m_pInstance;
	m_pInstance = NULL;
}

//void CUnitsManager::LoadSettings()
//{
//	// load length and weight units from registry
//	m_nLengthUnits = ((CTermPlanApp*)AfxGetApp())->GetProfileInt("UnitsManager", "LengthUnits", UM_LEN_METERS);
//	m_nWeightUnits = ((CTermPlanApp*)AfxGetApp())->GetProfileInt("UnitsManager", "WeightUnits", UM_WEIGHT_KILOS);
//}

//void CUnitsManager::SaveSettings()
//{
//	// save to registry
//	VERIFY(((CTermPlanApp*)AfxGetApp())->WriteProfileInt("UnitsManager", "LengthUnits", m_nLengthUnits));
//	VERIFY(((CTermPlanApp*)AfxGetApp())->WriteProfileInt("UnitsManager", "WeightUnits", m_nWeightUnits));
//}

CString CUnitsManager::GetWeightUnitString(WEIGHT_UNITS n)
{
	ASSERT(n>=UM_WEIGHT_FIRST && n<=UM_WEIGHT_LAST);
	return WeightStr[n - UM_WEIGHT_OFFSET];
}

CString CUnitsManager::GetWeightUnitString()
{
	return WeightStr[m_nWeightUnits - UM_WEIGHT_OFFSET];
}

CString CUnitsManager::GetWeightUnitLongString(WEIGHT_UNITS n)
{
	ASSERT(n>=UM_WEIGHT_FIRST && n<=UM_WEIGHT_LAST);
	return WeightLongStr[n - UM_WEIGHT_OFFSET];
}

CString CUnitsManager::GetLengthUnitString(LENGTH_UNITS n)
{
	ASSERT(n>=UM_LEN_FIRST && n<=UM_LEN_LAST);
	return LengthStr[n - UM_LEN_OFFSET];
}

CString CUnitsManager::GetLengthUnitString()
{
	return LengthStr[m_nLengthUnits - UM_LEN_OFFSET];
}

CString CUnitsManager::GetLengthUnitLongString(LENGTH_UNITS n)
{
	ASSERT(n>=UM_LEN_FIRST && n<=UM_LEN_LAST);
	return LengthLongStr[n - UM_LEN_OFFSET];
}

double CUnitsManager::ConvertLength(double d)
{
	return d * LengthConversion[m_nLengthUnits - UM_LEN_OFFSET][1];
}

int CUnitsManager::ConvertLength(int i)
{
	return (int)(i*LengthConversion[m_nLengthUnits - UM_LEN_OFFSET][1]);
}

double CUnitsManager::UnConvertLength(double d)
{
	return d * LengthConversion[m_nLengthUnits - UM_LEN_OFFSET][0];
}

double CUnitsManager::ConvertLength(LENGTH_UNITS fromUnits, LENGTH_UNITS toUnits, double d)
{
	double m = d * LengthConversion[fromUnits - UM_LEN_OFFSET][0];
	return m * LengthConversion[toUnits - UM_LEN_OFFSET][1];
}

long CUnitsManager::ConvertFromOleTime(COleDateTime odt)
{
	return odt.GetHour()*TIMETOHOURDIV + odt.GetMinute()*TIMETOMINDIV + odt.GetSecond()*TIMETOSECDIV;
}

COleDateTime CUnitsManager::ConvertToOleTime(long nTime)
{
	COleDateTime ret = COleDateTime::GetCurrentTime();
	int nHour = nTime/TIMETOHOURDIV;
	int nMin = (nTime-nHour*TIMETOHOURDIV)/TIMETOMINDIV;
	int nSec = (nTime-nHour*TIMETOHOURDIV-nMin*TIMETOMINDIV)/TIMETOSECDIV;
	ret.SetTime(nHour,nMin,nSec);
	return ret;
}
