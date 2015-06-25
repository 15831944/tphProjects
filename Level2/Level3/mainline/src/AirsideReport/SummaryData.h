#pragma once
#include "../Common/elaptime.h"

class CSummaryData
{
public:
	CSummaryData(void);
	~CSummaryData(void);

public:
	ElapsedTime   m_estTotal;
	ElapsedTime   m_estMin;
	ElapsedTime   m_estAverage;
	ElapsedTime   m_estMax;
	ElapsedTime   m_estQ1;
	ElapsedTime   m_estQ2;
	ElapsedTime   m_estQ3;
	ElapsedTime   m_estP1;
	ElapsedTime   m_estP5;
	ElapsedTime   m_estP10;
	ElapsedTime   m_estP90;
	ElapsedTime   m_estP95;
	ElapsedTime   m_estP99;
	ElapsedTime   m_estSigma;	
};