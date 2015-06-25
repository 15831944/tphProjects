#pragma once
#include "../common/FLT_CNST.H"
using namespace std;

class COperationalResultData
{
public:
	COperationalResultData(void);
	~COperationalResultData(void);

public:
	FlightConstraint m_fltCons;
	double           m_dData;    
};
