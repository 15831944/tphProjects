#pragma once
#include "AirsideFlightDelayReport.h"
#include "../Engine/Airside/CommonInSim.h"
#include <iostream>
#include <vector>
using namespace std;

class CDetailComponentData
{
public:
	CDetailComponentData(void);
	~CDetailComponentData(void);

public:
	FltDelayReason      m_DelayReason;
	vector<int>         m_vData;

};
