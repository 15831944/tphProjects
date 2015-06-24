// Flight.h: interface for the CFlight class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLIGHT_H__D55FBD75_D91B_496E_AC3C_81B8EE4F0B09__INCLUDED_)
#define AFX_FLIGHT_H__D55FBD75_D91B_496E_AC3C_81B8EE4F0B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#include <vector>

class CAircraftType;

class CFlight : public CObject
{
	DECLARE_SERIAL( CFlight )
public:
	CFlight();
	virtual ~CFlight();

	virtual void Serialize(CArchive& ar);

protected:
	CString m_sAirline;
	int m_nArrID;
	//CAirport* m_pOrigin;
	CTime m_tArrTime;
	int m_nDepID;
	//CAirport* m_pDest;
	CTime m_tDepTime;
	CAircraftType* m_pACType;


};

typedef std::vector<CFlight> CFLIGHTLIST;

#endif // !defined(AFX_FLIGHT_H__D55FBD75_D91B_496E_AC3C_81B8EE4F0B09__INCLUDED_)
