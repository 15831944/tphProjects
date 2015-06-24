#include "StdAfx.h"
#include ".\onboardcellfreemoveevent.h"
#include "OnboardCellFreeMoveLogic.h"

OnboardCellFreeMoveEvent::OnboardCellFreeMoveEvent( OnboardCellFreeMoveLogic *pFreeMoveLogic )
:m_pFreeMoveLogic(pFreeMoveLogic)
{

}
OnboardCellFreeMoveEvent::~OnboardCellFreeMoveEvent(void)
{

}

int OnboardCellFreeMoveEvent::process( CARCportEngine* pEngine )
{
	if(m_pFreeMoveLogic)
		m_pFreeMoveLogic->StartMove(getTime());


	return 0;
}

int OnboardCellFreeMoveEvent::kill( void )
{
	return 0;
}

const char * OnboardCellFreeMoveEvent::getTypeName( void ) const
{
	return _T("Onboard Free Move Event");
}

int OnboardCellFreeMoveEvent::getEventType( void ) const
{
	return 	OnboardPaxFreeMoveEvent;
}