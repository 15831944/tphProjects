#include "StdAfx.h"
#include ".\onboardpaxcellmovetoseatevent.h"
#include "OnboardCellSeatGivePlaceLogic.h"

OnboardPaxCellMoveToSeatEvent::OnboardPaxCellMoveToSeatEvent( OnboardCellSeatGivePlaceLogic *pLogic, PaxOnboardBaseBehavior* pPax, const ElapsedTime& eTime )
:m_pPaxLogic(pLogic)
,m_pPaxBehavior(pPax)
,m_bCancel(false)
{
	setTime(eTime);
}

OnboardPaxCellMoveToSeatEvent::~OnboardPaxCellMoveToSeatEvent(void)
{
}
int OnboardPaxCellMoveToSeatEvent::process( CARCportEngine* pEngine )
{
	if(m_pPaxLogic && !m_bCancel)
		return m_pPaxLogic->ProcessEvent(m_pPaxBehavior,getTime());

	ASSERT(0);
	return FALSE;
}

int OnboardPaxCellMoveToSeatEvent::kill( void )
{
	m_bCancel = true;
	return 0;

}

const char * OnboardPaxCellMoveToSeatEvent::getTypeName( void ) const
{
	return _T("OnboardPaxCellMoveToSeatEvent");
}

int OnboardPaxCellMoveToSeatEvent::getEventType( void ) const
{
	return 	OnboardPaxMoveToSeatEvent;
}

ElapsedTime OnboardPaxCellMoveToSeatEvent::removeMoveEvent( PaxOnboardBaseBehavior *anElement )
{
	ElapsedTime eTime;
	int nCount = m_pEventList->getCount();
	for( int i=0; i<nCount; i++ )
	{
		Event* pEventInList = m_pEventList->getItem( i );

		if( pEventInList->getEventType() == OnboardPaxMoveToSeatEvent)
		{

			OnboardPaxCellMoveToSeatEvent *pMoveEvent = (OnboardPaxCellMoveToSeatEvent *)pEventInList;
			if( pMoveEvent->m_pPaxBehavior == anElement)
			{
				eTime = pMoveEvent->time;
				//				m_pEventList->deleteItem( nIdx );
				m_pEventList->deleteItem( i );
				break;
			}
		}
	}
	return eTime;
}