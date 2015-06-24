// MathProcess.cpp: implementation of the CMathProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MathProcess.h"
#include "../common/ProbDistManager.h"
#include "../Inputs/probab.h"
#include "../engine/terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathProcess::CMathProcess()
{
	m_pTerminal = NULL;
	m_qDiscipline = QD_FCFS;
	m_qConfig = QC_LINEAR;
	m_nCapacity = -1;
	m_nProcCount = 1;
}

CMathProcess::~CMathProcess()
{

}

const CString& CMathProcess::GetName() const
{
	return m_strName;
}

int CMathProcess::GetProcessCount()
{
	return m_nProcCount;
}

int CMathProcess::GetCapacity()
{
	return m_nCapacity;
}

ProbabilityDistribution* CMathProcess::GetProbDist()
{
//	if (m_pProbDist != NULL)
//		return m_pProbDist;

	if (m_pTerminal == NULL)
		return NULL;

	int nCount = _g_GetActiveProbMan(m_pTerminal)->getCount();
	for (int i = 0; i < nCount; i++)
	{
		CProbDistEntry* pde = _g_GetActiveProbMan(m_pTerminal)->getItem(i);
		if (pde->m_csName.CompareNoCase(m_strProbDist) == 0)
			return pde->m_pProbDist;
	}

	return NULL;
	
}

CString CMathProcess::GetProbDistName()
{
	return m_strProbDist;
}

QueueDiscipline	CMathProcess::GetQDiscipline()
{
	return m_qDiscipline;
}

QueueConfig	CMathProcess::GetQConfig()
{
	return m_qConfig;
}


void CMathProcess::SetName(const CString& strName)
{
	m_strName = strName;
}

void CMathProcess::SetProcessCount(int nCount)
{
	m_nProcCount = nCount;
}

void CMathProcess::SetCapacity(int nCapacity)
{
	m_nCapacity = nCapacity;
}
/*
void CMathProcess::SetProbDist(ProbabilityDistribution* pProbDist)
{
	m_pProbDist = pProbDist;
}
*/
void CMathProcess::SetProbDistName(CString strProbDist)
{
	m_strProbDist = strProbDist;
}

void CMathProcess::SetQDiscipline(QueueDiscipline qd)
{
	m_qDiscipline = qd;
}

void CMathProcess::SetQConfig(QueueConfig qc)
{
	m_qConfig = qc;
}

void CMathProcess::SetTerminal(Terminal* pTerminal)
{
	m_pTerminal = pTerminal;
}

CMathProcess& CMathProcess::operator =(const CMathProcess& _rhs)
{
	m_strName = _rhs.m_strName;
	m_nProcCount = _rhs.m_nProcCount;
	m_nCapacity = _rhs.m_nCapacity;
	m_strProbDist = _rhs.m_strProbDist;
//	m_pProbDist = _rhs.m_pProbDist;
	m_qDiscipline = _rhs.m_qDiscipline;
	m_qConfig = _rhs.m_qConfig;	

	return (*this);
}