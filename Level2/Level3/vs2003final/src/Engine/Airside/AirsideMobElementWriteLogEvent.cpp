#include "StdAfx.h"
#include ".\airsidemobelementwritelogevent.h"
#include "AirsideMobElementWriteLogItem.h"

CAirsideMobElementWriteLogEvent::CAirsideMobElementWriteLogEvent(AirsideMobElementWriteLogItem* pLogItem)
{
	m_pLogItem = pLogItem;
}

CAirsideMobElementWriteLogEvent::~CAirsideMobElementWriteLogEvent(void)
{
}

int CAirsideMobElementWriteLogEvent::Process()
{
	if(m_pLogItem)
		m_pLogItem->Execute();

	return 1;
}

const char * CAirsideMobElementWriteLogEvent::getTypeName( void ) const
{
	return "AirsideMobElementWriteLogEvent";
}

int CAirsideMobElementWriteLogEvent::getEventType( void ) const
{
	return AirsideMobElementWriteLogEvent;
}