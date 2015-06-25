// UnitsManager.h: interface for the CUnitsManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNITSMANAGER_H__4B3BB966_439D_11D4_930E_0080C8F982B1__INCLUDED_)
#define AFX_UNITSMANAGER_H__4B3BB966_439D_11D4_930E_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UM_LEN_FIRST		0
#define UM_LEN_KILOMETERS	0
#define UM_LEN_METERS		1
#define UM_LEN_CENTIMETERS	2
#define UM_LEN_MILLIMETERS	3
#define UM_LEN_MILE			4
#define UM_LEN_FEET			5
#define UM_LEN_INCHES		6
#define UM_LEN_NAUTMILE		7
#define UM_LEN_LAST			7
#define UM_LEN_OFFSET		0

#define UM_WEIGHT_FIRST		100
#define UM_WEIGHT_KILOS		100
#define UM_WEIGHT_POUNDS	101
#define UM_WEIGHT_LAST		101
#define UM_WEIGHT_OFFSET	100

#define UNITSMANAGER	CUnitsManager::GetInstance()
#define DESTROY_UNITSMANAGER	CUnitsManager::DeleteInstance();

class CUnitsManager  
{
public:
	typedef int WEIGHT_UNITS;
	typedef int LENGTH_UNITS;

private:
	static CUnitsManager* m_pInstance;
	WEIGHT_UNITS m_nWeightUnits;
	LENGTH_UNITS m_nLengthUnits;

	/*void LoadSettings();
	void SaveSettings();*/

protected:
	CUnitsManager();
	virtual ~CUnitsManager();

public:
	static CUnitsManager* GetInstance();
	static void DeleteInstance();

	BOOL SetWeightUnits(WEIGHT_UNITS n) { ASSERT(n>=UM_WEIGHT_FIRST && n<=UM_WEIGHT_LAST); m_nWeightUnits = n; return TRUE; };
	BOOL SetLengthUnits(LENGTH_UNITS n) { ASSERT(n>=UM_LEN_FIRST && n<=UM_LEN_LAST); m_nLengthUnits = n; return TRUE; };

	WEIGHT_UNITS GetWeightUnits() { return m_nWeightUnits; };
	LENGTH_UNITS GetLengthUnits() { return m_nLengthUnits; };

	CString GetWeightUnitString(WEIGHT_UNITS n);
	CString GetLengthUnitString(LENGTH_UNITS n);
	CString GetWeightUnitString();
	CString GetLengthUnitString();

	CString GetWeightUnitLongString(WEIGHT_UNITS n);
	CString GetLengthUnitLongString(LENGTH_UNITS n);

	//conversion
	double ConvertLength(double d); // converts the value from meters to current units.
	double UnConvertLength(double d); // converts the value from current units to meters.
	int ConvertLength(int i); // converts the value from meters to current units.
	double ConvertLength(LENGTH_UNITS fromUnits, LENGTH_UNITS toUnits, double d);

	long ConvertFromOleTime(COleDateTime odt);
	COleDateTime ConvertToOleTime(long nTime);


};

#endif // !defined(AFX_UNITSMANAGER_H__4B3BB966_439D_11D4_930E_0080C8F982B1__INCLUDED_)
