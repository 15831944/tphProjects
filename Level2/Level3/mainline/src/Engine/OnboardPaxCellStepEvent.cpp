#include "StdAfx.h"
#include ".\onboardpaxcellstepevent.h"
#include "../Common/elaptime.h"
#include "OnboardCellFreeMoveLogic.h"

OnboardPaxCellStepEvent::OnboardPaxCellStepEvent( OnboardCellFreeMoveLogic *pPaxLogic, const ElapsedTime& eTime )
:m_pPaxLogic(pPaxLogic)

{
	setTime(eTime);
}
OnboardPaxCellStepEvent::~OnboardPaxCellStepEvent(void)
{
}

int OnboardPaxCellStepEvent::process( CARCportEngine* pEngine )
{
	if(m_pPaxLogic)
	{
		int nret = m_pPaxLogic->Step3(getTime());
		return nret;
	}

	ASSERT(0);
	return FALSE;
}

int OnboardPaxCellStepEvent::kill( void )
{
    return 0;
}

const char * OnboardPaxCellStepEvent::getTypeName( void ) const
{
	return _T("OnboardPaxCellStepEvent");
}

int OnboardPaxCellStepEvent::getEventType( void ) const
{
	return 	OnboardPaxStepEvent;
}