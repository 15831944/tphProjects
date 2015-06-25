// ProbDist.cpp: implementation of the CProbDistEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProbDistEntry.h"
#include "../Common/ProbabilityDistribution.h"

CProbDistEntry::CProbDistEntry( CString _csName, ProbabilityDistribution* _pProbDist )
{
	m_csName = _csName;
	m_pProbDist = _pProbDist;
}

CProbDistEntry::~CProbDistEntry()
{
	delete m_pProbDist;
}

