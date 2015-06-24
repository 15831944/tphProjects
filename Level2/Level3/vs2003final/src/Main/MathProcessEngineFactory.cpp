// MathProcessEngineFactory.cpp: implementation of the CMathProcessEngineFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../Common/ProbabilityDistribution.h"
#include "MathProcessEngineFactory.h"
#include "MathProcessEngineMMN.h"
#include "MathProcessEngineMGN.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathProcessEngineFactory::CMathProcessEngineFactory()
{
	m_pServiceTimeProbDist = NULL;
	m_pMathProcessEngine = NULL;
}

CMathProcessEngineFactory::CMathProcessEngineFactory(ProbabilityDistribution* pServiceTimeProbDist)
{
	ASSERT(pServiceTimeProbDist != NULL);
	m_pServiceTimeProbDist = pServiceTimeProbDist;
	m_pMathProcessEngine = NULL;
}

void CMathProcessEngineFactory::SetServiceTimeProbDist(ProbabilityDistribution* pServiceTimeProbDist)
{
	ASSERT(pServiceTimeProbDist != NULL);
	m_pServiceTimeProbDist = pServiceTimeProbDist;
}


CMathProcessEngineFactory::~CMathProcessEngineFactory()
{

}

CMathProcessEngine* CMathProcessEngineFactory::CreateInstance()
{
	if (m_pServiceTimeProbDist == NULL)
		return NULL;

	switch(m_pServiceTimeProbDist->getProbabilityType())
	{
	case EXPONENTIAL:
		m_pMathProcessEngine = new CMathProcessEngineMMN;
		break;

	case NORMAL:
	case HISTOGRAM:
	case UNIFORM:
		m_pMathProcessEngine = new CMathProcessEngineMGN;
		break;
	}

	if (m_pMathProcessEngine != NULL)
		m_pMathProcessEngine->m_pServiceTimeProbDist = m_pServiceTimeProbDist;

	return m_pMathProcessEngine; 
}

