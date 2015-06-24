// MathProcessEngineMM1.cpp: implementation of the CMathProcessEngineMM1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "MathProcessEngineMMN.h"
#include "../Common/elaptime.h"
#include "../Common/ProbabilityDistribution.h"


using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathProcessEngineMMN::CMathProcessEngineMMN()
{
}

CMathProcessEngineMMN::~CMathProcessEngineMMN()
{
}

void CMathProcessEngineMMN::CalculateNormal(const vector<double>& vArrTime, int nServerCount)
{
	ASSERT(m_pServiceTimeProbDist != NULL);
	PumpMessages();
	int nArrTimeCount = static_cast<int>(vArrTime.size());
	double dArrMean = nArrTimeCount + m_nPrePassengerCount;
	double dRho = m_pServiceTimeProbDist->getMean() * dArrMean / m_nInterval;

	int i;
	m_vProbability.resize(nArrTimeCount + 1, 0.0);

	// calculate the vProbability[0]
	double dTemp1 = 0.0;
	for (i = 0; i < nServerCount; i++)
		dTemp1 += (pow(dRho, i) / Times(i));

	double dTemp2 = pow(dRho, nServerCount) / Times(nServerCount);
	dTemp2 = dTemp2 / (1 - dRho / nServerCount);

	m_vProbability.front() = 1 / (dTemp1 + dTemp2);

	// calculate vProbability[1] to vProbability[nArrTimeCount]
	for (i = 1; i < nServerCount; i++)
	{
		if (i > nArrTimeCount)
			break;
		m_vProbability.at(i) = m_vProbability.front() * pow(dRho, i) /Times(i);
	}

	for (i = nServerCount; i < nArrTimeCount + 1; i++)
	{
		double dCurPro = m_vProbability.front() * pow(dRho, i) 
			/ (pow(nServerCount, i - nServerCount) * Times(nServerCount));

		if (dCurPro < ZERO_PROBABILITY || dCurPro > 1.0)
			break;

		m_vProbability[i] = dCurPro;
	}

	//calculate queue length and time
	m_dQLength = pow(dRho, nServerCount + 1) * m_vProbability.front() 
		/ (Times(nServerCount - 1) * pow((nServerCount - dRho), 2));
	m_dQTime = m_dQLength / dArrMean * m_nInterval;

	// calculate utilization
	m_dUtilization = 100 * (1.0 - m_vProbability.front());

	// calculate throughput
	m_dThroughput = dArrMean;

	//calculate system length
	//at first,calculate the active server number
	int iTemp = 0;
	double dActServer = 0.0; //record the active server number
	for (i = 0; i < nArrTimeCount + 1; i++)
	{
		if (m_vProbability[i] < ZERO_PROBABILITY)
			break;

		iTemp = min(i, nServerCount);
		dActServer += iTemp * m_vProbability[i];
	}
	m_nPassengerCount = static_cast<int>(m_dQLength + dActServer);
}

// calculate the leave time from this process
void CMathProcessEngineMMN::CalculateNormalLeaveTime(const vector<double>& vArrTime, vector<double>& vLeaTime)
{
	// first calculate the accumulative probability
	assert(m_pServiceTimeProbDist != NULL && !m_vProbability.empty());
	int i = 0;
	int nProbCount = static_cast<int>(m_vProbability.size());
	int nArrTimeCount = static_cast<int>(vArrTime.size());
	vector<double> vAccProbability(nProbCount + 1, 0.0);
	for (i = 0; i < nProbCount + 1; i++)
	{
		if (i == 0)
			vAccProbability[i] = m_vProbability.front();
		else
			vAccProbability[i] += m_vProbability[i-1];
	}

	// calculate the initial delay time
	srand((unsigned)time(NULL));
	double dRandPro = 0.0;
	double dInitialDelay = 0.0;
	for (i = 0; i < nArrTimeCount; i++)
	{
		dRandPro = (double)rand()/(RAND_MAX+1);
		dInitialDelay = 0.0;

		for (int j = 0; j < nProbCount; j++)
		{
			if (dRandPro > vAccProbability[j] && dRandPro < vAccProbability[j+1])
			{
				for (int k = 0; k < j; k++)
					dInitialDelay += m_pServiceTimeProbDist->getRandomValue();

				break;
			}
		}
		vLeaTime.push_back(vArrTime.at(i) + dInitialDelay);
	}
}
