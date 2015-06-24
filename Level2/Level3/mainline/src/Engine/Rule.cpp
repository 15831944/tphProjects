// Rule.cpp: implementation of the CRule class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "Rule.h"

#include "terminal.h"
#include "person.h"

#include "inputs\schedule.h"
#include "Engine\ARCportEngine.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkipProcRule::CSkipProcRule(int _nSkipTime): m_nSkipTime(_nSkipTime)
{
}
CSkipProcRule::CSkipProcRule(const CSkipProcRule& _other): m_nSkipTime(_other.m_nSkipTime)
{
}
bool CSkipProcRule::Vote(const Person& _man) const
{
	//arrival passenger need not check skip processor.
	if(_man.getType().GetFltConstraintMode()== ENUM_FLTCNSTR_MODE_ARR ) return true;

	const ElapsedTime& curTime = _man.GetPrevEventTime();//current time

	//get flight departure time
	char fullID[FULL_ID_LEN] = "";
	_man.getType().getFullID(fullID);
	int nFltIndex = _man.getEngine()->m_simFlightSchedule.getFlightIndex( fullID, ENUM_FLTCNSTR_MODE_DEP );
	ASSERT( nFltIndex!=-1);
	const ElapsedTime& depTime = _man.getEngine()->m_simFlightSchedule.getItem( nFltIndex )->getDepTime();
	//check skip time
	if(  depTime.asMinutes() - curTime.asMinutes() > m_nSkipTime) return true;//not skip
	
	return false;//skip
}

CRule* CSkipProcRule::Clone() const
{
	return new CSkipProcRule(m_nSkipTime);
}