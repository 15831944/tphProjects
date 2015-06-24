#include "StdAfx.h"
#include ".\mobileelementexitsystemevent.h"
#include "ARCportEngine.h"


MobileElementExitSystemEvent::MobileElementExitSystemEvent(MobileElement *pMobElement, ElapsedTime eTime)
:m_pMobileElement(pMobElement)
{
	time = eTime;
	m_nMobID = pMobElement->getID();
}

MobileElementExitSystemEvent::~MobileElementExitSystemEvent(void)
{
}

int MobileElementExitSystemEvent::process( CARCportEngine* pEngine )
{


	ASSERT(pEngine != NULL && m_pMobileElement != NULL);
	if(pEngine != NULL && m_pMobileElement != NULL)
		pEngine->m_simBobileelemList.Unregister(m_pMobileElement);

	return 1;
}

int MobileElementExitSystemEvent::kill( void )
{
	return 0;
}

const char * MobileElementExitSystemEvent::getTypeName( void ) const
{
	return _T("Mobile Exit System");
}

int MobileElementExitSystemEvent::getEventType( void ) const
{
	return MobElementExitEvent;
}

