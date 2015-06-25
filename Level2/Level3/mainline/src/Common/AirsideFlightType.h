#pragma once
#include "FLT_CNST.H"
#include "ARCFlight.h"

class CAirportDatabase;
class AirsideFlightType
{
public:
	AirsideFlightType(void);
	~AirsideFlightType(void);

	bool IsFit(ARCFlight* pflight, char mode);	//for fit part of flight, arr or dep
	bool IsFit(ARCFlight* pflight);		//for fit flight

	//for compare whether include the other flight type if both arr --> dep or both not arr --> dep
	//otherwise compare whether is more detailed the other flight type
	bool IsFit(const AirsideFlightType& flttype);	

	CString getPrintString();
	CString getDBString();
	void FormatDBStringToFlightType(const CString& strFlightType);

	bool IsArrToDep();
	void SetArrToDep(bool bArrToDep);

	void SetFltConst1(const FlightConstraint& flt_cnst);
	void SetFltConst2(const FlightConstraint& flt_cnst);

	const FlightConstraint& GetFltConst1();
	const FlightConstraint& GetFltConst2();
	bool IsDefaultFlightType();
	void SetAirportDB(CAirportDatabase* pAirportDB);

private:
	bool m_bArrToDep;
	FlightConstraint m_fltConst1;		//if is arr -> dep,  m_fltConst1 is arr part constraint, m_fltConst2 is dep part constraint
	FlightConstraint m_fltConst2;		//if is not arr -> dep, only use m_fltConst1, m_fltConst2 not used
	CAirportDatabase* m_pAirportDB;
};
