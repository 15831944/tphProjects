// MathProcessEngine.cpp: implementation of the CMathProcessEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../Common/ProbabilityDistribution.h"
#include "MathProcessEngine.h"
#include "MathResultManager.h"
#include <math.h>

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const int MAX_TIMES = 170;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathProcessEngine::CMathProcessEngine()
{
	m_nServerCount			= -1;
	m_nPrePassengerCount	= 0;
	m_nSpecialRho			= -1;
	m_nBeginSpecialCount	= -1;
	m_nInterval				= 1800;

	m_pMathResultManager	= NULL;
	m_pServiceTimeProbDist	= NULL;
}

CMathProcessEngine::~CMathProcessEngine()
{
}

double CMathProcessEngine::Times(int iNum)
{
	// the range of double value is 1.7E +/- 308, 
	// times of 170 is 7.25742e+306
	assert (iNum >= 0 && iNum <= MAX_TIMES);
	double iTemp = 1.0;
	for (; iNum > 0; iNum--)
	{
		iTemp *= iNum;
	}
	return iTemp;
}

void CMathProcessEngine::AddArrTime(double dArrTime)
{
	m_vArrTime.push_back(dArrTime);
}

void CMathProcessEngine::SortArrTimes()
{
	std::sort(m_vArrTime.begin(), m_vArrTime.end());
}

bool CMathProcessEngine::Run()
{
	assert(m_pMathResultManager != NULL);
	bool bRet = false;
	if (m_vArrTime.empty())
		return bRet;
	
	ElapsedTime tmStart(m_vArrTime.front());
	ElapsedTime tmEnd(m_vArrTime.back());

	int nCount = ((tmEnd.asSeconds() - tmStart.asSeconds()) / m_nInterval) + 1;

	int iStartHalfHour = (int)(tmStart.asSeconds() / m_nInterval);
	
	for (int i = 0; (i + iStartHalfHour) * m_nInterval < tmEnd.asSeconds(); i++)
	{
		m_iStartTime = (i + iStartHalfHour) * m_nInterval;
		double iEnd = (i + iStartHalfHour + 1) * m_nInterval;

		vector<double> vArrTime;
		for (vector<double>::const_iterator iter = m_vArrTime.begin();
				iter != m_vArrTime.end();
				iter++)
		{
			if (*iter > m_iStartTime && *iter <= iEnd)
			{
				double dTemp = *iter;
				vArrTime.push_back(dTemp);
			}
		}

		if (i == 0)
		{
			m_nPrePassengerCount = 0;
		}
		// TRACE("%TimeRange: %d-%d\n", m_iStartTime, iEnd);
		
		if (Calculate(vArrTime))
			AddResult();
		else
			AddDefaultResult();

		m_nPrePassengerCount = m_nPassengerCount;
	}

	bRet = true;
	return bRet;
}

bool CMathProcessEngine::Calculate(const vector<double>& vArrTime)
{
	assert(m_pServiceTimeProbDist != NULL);
	bool bRet = false;
	if (vArrTime.empty() || m_nServerCount < 0 || m_nServerCount > 170)
		return bRet;

	int nArrTimeCount = static_cast<int>(vArrTime.size());
	double dArrMean = nArrTimeCount + m_nPrePassengerCount;
	double dRho = m_pServiceTimeProbDist->getMean() * dArrMean / m_nInterval;

	m_nSpecialRho = (int) dRho;
	
	if ((dRho  - (double)m_nServerCount) <= ZERO_PROBABILITY)
	{
		CalculateNormal(vArrTime, m_nServerCount);
		CalculateNormalLeaveTime(vArrTime, m_vLeaTime);
	}
	else
	{
		CalculateBusyLeaveTime(vArrTime, m_vLeaTime, m_nServerCount);
		if (!CalculateBusy(vArrTime, m_nServerCount))
			return false;
	}

	bRet = true;
	return bRet;
}

bool CMathProcessEngine::CalculateBusy(const vector<double>& vArrTime, int nServerCount)
{
	PumpMessages();
	vector<double> vLeaTime;
	CalculateBusyLeaveTime(vArrTime, vLeaTime, nServerCount);
	assert(vLeaTime.size() == vArrTime.size());

	int nArrTimeCount = static_cast<int>(vArrTime.size());
	double dTotalWaitTime = 0.0;
	for (int i = 0; i < nArrTimeCount; i++)
	{
        dTotalWaitTime += vLeaTime[i] - vArrTime[i];
	}

	m_dQTime = (dTotalWaitTime / (double)nArrTimeCount) - m_pServiceTimeProbDist->getMean();

	if (m_dQTime < 0)
	{
		m_nBeginSpecialCount = nServerCount;
		if (CalculateSpecial(vArrTime, nServerCount))
			return true;
	}

	m_dQTime = (dTotalWaitTime / (double)nArrTimeCount) - m_pServiceTimeProbDist->getMean();
	m_dQLength = m_dQTime * nArrTimeCount / m_nInterval;
	m_nPassengerCount = static_cast<int>(m_dQLength) + nServerCount;
	m_dThroughput = m_nInterval / m_pServiceTimeProbDist->getMean();
	m_dUtilization = 100.0;

	return true;
}

bool CMathProcessEngine::CalculateSpecial(const std::vector<double>& vArrTime, int nServerCount)
{
	PumpMessages();
	vector<double> vLeaTime;
	CalculateBusyLeaveTime(m_vArrTime, vLeaTime, nServerCount);
	if (m_nSpecialRho <= 0)
		return false;

	int nArrTimeCount = static_cast<int>(m_vArrTime.size());

	CalculateBusy(vArrTime, m_nBeginSpecialCount - 1);
	double dQLength1 = m_dQLength;

	if (m_nSpecialRho + 2 > MAX_TIMES)
		return false;

	CalculateNormal(vArrTime, m_nSpecialRho + 2);
	double dQLength2 = m_dQLength;

	m_dQLength = dQLength1 - ((dQLength1 - dQLength2) / (m_nSpecialRho - m_nBeginSpecialCount + 3) * (nServerCount - (m_nBeginSpecialCount - 1)));
	m_dQTime = m_dQLength / (nArrTimeCount / m_nInterval);
	m_nPassengerCount = static_cast<int>(m_dQLength) + nServerCount;
	m_dThroughput = m_nInterval / m_pServiceTimeProbDist->getMean();
	m_dUtilization = 100.0;
	
	return true;
}

void CMathProcessEngine::CalculateBusyLeaveTime(const vector<double>& vArrTime, 
											    vector<double>& vLeaTime,
												int nServerCount)
{
	assert(!vArrTime.empty());
	assert(m_pServiceTimeProbDist != NULL);
	
	int nLeaTimeCount = static_cast<int>(vLeaTime.size());
	srand((unsigned)time(NULL));
	double dRandServiceTime = 0.0;
	int nArrTimeCount = static_cast<int>(vArrTime.size());
	for (int i = 0; i < nArrTimeCount; i++)
	{
		dRandServiceTime = m_pServiceTimeProbDist->getRandomValue() / nServerCount;
		if (i == 0)
			vLeaTime.push_back(vArrTime.front() + dRandServiceTime);
		else
			vLeaTime.push_back(vLeaTime[nLeaTimeCount + i - 1] + dRandServiceTime);
	}
}

void CMathProcessEngine::AddResult()
{
	// result filter:
	// if the result is litter than 0,
	// then set it as 0;
	m_dQTime			= max(ZERO_PROBABILITY, m_dQTime);
	m_dQLength			= max(ZERO_PROBABILITY, m_dQLength);
	m_dThroughput		= max(ZERO_PROBABILITY, m_dThroughput);
	m_nPassengerCount	= static_cast<int>( max(ZERO_PROBABILITY, m_nPassengerCount) );
	m_dUtilization		= max(ZERO_PROBABILITY, m_dUtilization);


	// set the unit of queue time from second to minutes
	m_dQTime /= 60.0;

	AddResult(m_pMathResultManager->GetQTimeResult(), m_dQTime);
	AddResult(m_pMathResultManager->GetQLengthResult(), m_dQLength);
	AddResult(m_pMathResultManager->GetThroughputResult(), m_dThroughput, 0.0);
	AddResult(m_pMathResultManager->GetPaxCountResult(), (double)m_nPassengerCount);
	AddResult(m_pMathResultManager->GetUtilizationResult(), m_dUtilization, 0.0);
}

void CMathProcessEngine::AddDefaultResult()
{
	m_dQTime = 0.0;
	m_dQLength = 0.0;
	m_dThroughput = 0.0;
	m_nPassengerCount = 0;
	m_dUtilization = 0.0;

	AddResult();
}

void CMathProcessEngine::AddResult(MathResult& rs, const double dValue, const double dDefaultValue)
{
	int iCurProcIndex = rs.m_vstrProcess.size() - 1;
	assert(iCurProcIndex >= 0);

	// first, find the record int the result vector which time match the current time
	int indexRecord = -1;
	for (int i = 0; i < static_cast<int>(rs.m_vResult.size()); i++)
	{
		if (abs(rs.m_vResult[i].time.asSeconds() - m_iStartTime) < 5)
		{
			indexRecord = i;
			break;
		}
	}

	// if there's no record matched, then add a new record, 
	// and the record should been added value is the new one just added
	if (indexRecord < 0)
	{
		MathResultRecord rsRecord;
		rsRecord.time.set(m_iStartTime);
		rs.m_vResult.push_back(rsRecord);
		indexRecord = rs.m_vResult.size() - 1;
	}

	assert(indexRecord < static_cast<int>(rs.m_vResult.size()));
	MathResultRecord& rsCurRecord = rs.m_vResult.at(indexRecord);
	
	// if there's no value before the current process in this record, 
	// we should fill it with default value
	for (int j = rsCurRecord.vProcResults.size();
		 j < iCurProcIndex; j++)
	{
		rsCurRecord.vProcResults.push_back(dDefaultValue);
	}

	rsCurRecord.vProcResults.push_back(dValue);
}

void CMathProcessEngine::PumpMessages()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
