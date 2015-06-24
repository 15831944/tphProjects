// GroupFollowerInfo.cpp: implementation of the CGroupFollowerInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "GroupFollowerInfo.h"

#include "GroupLeaderInfo.h"
#include "person.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGroupFollowerInfo::CGroupFollowerInfo(  int _nGroupSize )
	:CGroupInfo( _nGroupSize )
{
}

CGroupFollowerInfo::~CGroupFollowerInfo()
{

}


// Only call this in OUT_GROUP
// if disAllow, continue gen event
// else, OUT_GROUP --> JOIN_GROUP ask leader check if all member 
enum EVENT_OPERATION_DECISTION CGroupFollowerInfo::ProcessEvent( ElapsedTime _eventTime, bool _bDisallowGroupInNextProc )
{
	enum EVENT_OPERATION_DECISTION enumRes = CONTINURE_GENERATE_EVENT;
	//if the next proc is ConveyProc - LOADER,

	//only used by LOADER proc,
	//it is the flag stand for the leader's next proc is a LOADER
	//it isn't waiting for group
	if( _bDisallowGroupInNextProc || m_pNextProc != NULL )
	{
		m_bWaitingForRegrouping = false;
	}
	else
	{
		// OUT_GROUP --> JOIN_GROUP
		m_bWaitingForRegrouping = true;
		if(m_pGroupLeader == NULL)
           return CONTINURE_GENERATE_EVENT ;
		if( ((CGroupLeaderInfo*)m_pGroupLeader->m_pGroupInfo)->AllReadytoRegroup() )
		{
			m_pGroupLeader->generateEvent( _eventTime,false );
		}
		return REMOVE_FROM_PROCESSOR_AND_NO_EVENT;
	}
	return enumRes;
}

bool CGroupFollowerInfo::IsSameGroup(Person* pPerson)
{
	if (pPerson == m_pGroupLeader)
		return true;

	CGroupLeaderInfo * pGroupLeaderInfo = (CGroupLeaderInfo*)m_pGroupLeader->m_pGroupInfo;
	if (pGroupLeaderInfo == NULL)
		return false;

	return pGroupLeaderInfo->IsSameGroup(pPerson);

}

int CGroupFollowerInfo::GetPosition(Person* pPerson)
{
	CGroupLeaderInfo* pLeaderGroup =(CGroupLeaderInfo*)GetGroupLeader();
	ASSERT(pLeaderGroup);

	int idx = pLeaderGroup->SearchPostion(pPerson);//index except group leader
	if (idx < 0) // not in same group
		return -1;

	return idx + 1;
}