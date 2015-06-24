// GroupInfo.cpp: implementation of the CGroupInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "GroupInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGroupInfo::CGroupInfo( int _nGroupSize )
{ 
	m_nGroupSize = _nGroupSize; 
	m_pGroupLeader = NULL;
	m_bWaitingForRegrouping = false;
	m_pNextProc = NULL;
}

CGroupInfo::~CGroupInfo()
{
}
void CGroupInfo::SetNextProc(Processor *pNextProc)
{
	m_pNextProc = pNextProc;
}