#pragma once
#include <iostream>
#include <vector>
#include "../common/FLT_CNST.H"
using namespace std;

class CDetailFlightDelayData
{
public:
	CDetailFlightDelayData(void);
	~CDetailFlightDelayData(void);

public:
    FlightConstraint m_fltCons;
	vector<long>      m_vData;           
};
