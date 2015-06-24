#include "StdAfx.h"
#include ".\landsideconflictmoveevent.h"
#include "ARCportEngine.h"
#include "LandsideTrafficSystem.h"
#include "CrossWalkInSim.h"

LandsideConflictMoveEvent::LandsideConflictMoveEvent(CFreeMovingLogic* pPaxLogic)
:m_pPaxLogic(pPaxLogic)
{

}

LandsideConflictMoveEvent::~LandsideConflictMoveEvent(void)
{
}
int LandsideConflictMoveEvent::process( CARCportEngine* pEngine )
{
	if(m_pPaxLogic)
	{
		m_pPaxLogic->StepConflictEvent(time);
		return TRUE;
	}

	return FALSE;
}

int LandsideConflictMoveEvent::kill( void )
{
	return 0;
}

const char * LandsideConflictMoveEvent::getTypeName( void ) const
{
	return _T("LandsideConflictEvent");
}

int LandsideConflictMoveEvent::getEventType( void ) const
{
	return 	LandsideConflictEvent;
}

LandsideLeaveCrossWalkEvent::LandsideLeaveCrossWalkEvent( CFreeMovingLogic* pPaxLogic,CCrossWalkInSim* pCrossWalkInSim,const ElapsedTime& time )
:m_pPaxLogic(pPaxLogic)
,m_pCrossWalkInSim(pCrossWalkInSim)
{
	setTime(time);
}

LandsideLeaveCrossWalkEvent::~LandsideLeaveCrossWalkEvent( void )
{

}

int LandsideLeaveCrossWalkEvent::process( CARCportEngine* pEngine )
{
	if(m_pPaxLogic)
	{
		m_pPaxLogic->ReleaseCrossWalk(m_pCrossWalkInSim,time);
		return TRUE;
	}

	return FALSE;
}

int LandsideLeaveCrossWalkEvent::kill( void )
{
	return 0;
}

const char * LandsideLeaveCrossWalkEvent::getTypeName( void ) const
{
	return _T("LandsideFreeMoveEvent");
}

int LandsideLeaveCrossWalkEvent::getEventType( void ) const
{
	return LandsideFreeMoveEvent;
}