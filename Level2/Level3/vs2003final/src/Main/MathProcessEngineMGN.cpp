// MathProcessEngineMGN.cpp: implementation of the CMathProcessEngineMGN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "MathProcessEngine.h"
#include "MathProcessEngineMGN.h"
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

CMathProcessEngineMGN::CMathProcessEngineMGN()
{
}

CMathProcessEngineMGN::~CMathProcessEngineMGN()
{
}

void CMathProcessEngineMGN::CalculateNormal(const vector<double>& vArrTime, int nServerCount)
{
	ASSERT(m_pServiceTimeProbDist != NULL);
	PumpMessages();

	int nArrTimeCount = static_cast<int>(vArrTime.size());
	double dArrMean = nArrTimeCount + m_nPrePassengerCount;
	double dRho = m_pServiceTimeProbDist->getMean() * dArrMean / m_nInterval;
	dRho = dRho / m_nServerCount;

	double dVariance = m_pServiceTimeProbDist->getVarience() / m_nInterval;
	double dMean = m_pServiceTimeProbDist->getMean() / m_nInterval;

	m_dQLength = (pow((dArrMean / m_nServerCount),2) * (pow(dMean,2) + dVariance)) / (2*(1-(dArrMean / m_nServerCount) * dMean));
	m_dQTime = m_dQLength / dArrMean * m_nInterval;
	m_nPassengerCount = static_cast<int>( (dArrMean / m_nServerCount) * dMean + m_dQLength );
	m_dThroughput = dArrMean;
	m_dUtilization = 100 * (1.0 - dRho / m_nServerCount);
}

void CMathProcessEngineMGN::CalculateNormalLeaveTime(const vector<double>& vArrTime, vector<double>& vLeaTime)
{
	int nArrCount = static_cast<int>(vArrTime.size());
	m_vLeaTime.clear();

	double dDelayTime = 0.0;
	srand((unsigned)time(NULL));
	for (int i = 0; i < nArrCount; i++)
	{
		dDelayTime = m_pServiceTimeProbDist->getRandomValue();
		vLeaTime.push_back(m_vArrTime.at(i) + dDelayTime);
	}
}