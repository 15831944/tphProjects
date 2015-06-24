/*****
*
*   Class       MobileElement
*   Author      Lincoln Fellingham, Khacthu Nguyen
*               Aviation Research Corporation
*   Date        February 29, 1993
*
*   Purpose     Base class for all mobile elements in Arcterm
*               Has the key data and functions required for writing log
*               files for animation and analysis
*
*   Revisions   Planned: Move queuing functions from person subclass to
*               this class, as non-person mobile elements can also form
*               queues
*
*****/
#include "stdafx.h"
#include "mobile.h"
#include "movevent.h"
#include "common\point.h"
#include "common\elaptime.h"
#include "main\resource.h"
#include "main\termplandoc.h"
#include "../Common/ARCTracker.h"
#include "TerminalMobElementBehavior.h"
#include "AirsideMobElementBehavior.h"
#include "PaxOnboardBaseBehavior.h"

#include <stdlib.h>

MobileElement::~MobileElement () 
{
	//assert( M_DELIDX >= 0 );
	//if(m_spBehavior != NULL)
	//	delete m_spBehavior ;
	//if(m_spPrevBehavior != NULL)
	//	delete m_spPrevBehavior ;

	ClearBehaviors();
}

void MobileElement::setBehavior(MobElementBehavior* _behavior)
{
	ASSERT( _behavior != NULL);
	int xxx = getID();
	if(xxx == 15)
	{
		int xxx2 = 0;
	}
	//check the behavior is exist or not, only 1 behavior could be exist at same time
	int nBehaviorCount = (int)m_vBehavior.size();
	for (int nBehavior = 0; nBehavior < nBehaviorCount; ++nBehavior)
	{
		if(m_vBehavior[nBehavior] &&_behavior &&
			m_vBehavior[nBehavior]->getBehaviorType() == _behavior->getBehaviorType())
		{ //use old behavior
			delete _behavior;
			m_spPrevBehavior = m_spBehavior;
			m_spBehavior = m_vBehavior[nBehavior];
			return;
		}
	}
	MobElementBehavior::BehaviorType behaviorType = _behavior->getBehaviorType();
	m_vBehavior.push_back(_behavior);
	//if(m_spPrevBehavior != NULL)
	//	delete m_spPrevBehavior;
	m_spPrevBehavior = m_spBehavior;

	m_spBehavior = _behavior;
}

MobElementBehavior* MobileElement::getCurrentBehavior()const
{
	return m_spBehavior;
}

MobElementBehavior* MobileElement::getBehavior(MobElementBehavior::BehaviorType emType)
{
	//if(m_spBehavior == NULL || m_spBehavior->getBehaviorType() != emType)
	//	return NULL;
	//
	//return m_spBehavior;
	int nBehaviorCount = (int)m_vBehavior.size();
	for (int nBehavior = 0; nBehavior < nBehaviorCount; ++nBehavior)
	{
		if(m_vBehavior[nBehavior] &&
			m_vBehavior[nBehavior]->getBehaviorType() == emType)
			return m_vBehavior[nBehavior];
	}

	return NULL;
}

TerminalMobElementBehavior* MobileElement::getTerminalBehavior()const
{
	int nBehaviorCount = (int)m_vBehavior.size();
	for (int nBehavior = 0; nBehavior < nBehaviorCount; ++nBehavior)
	{
		if(m_vBehavior[nBehavior] &&
			m_vBehavior[nBehavior]->getBehaviorType() == MobElementBehavior::TerminalBehavior)
			return (TerminalMobElementBehavior*)m_vBehavior[nBehavior];
	}

	return NULL;
}

AirsideMobElementBehavior* MobileElement::getAirsideBehavior()const
{
	int nBehaviorCount = (int)m_vBehavior.size();
	for (int nBehavior = 0; nBehavior < nBehaviorCount; ++nBehavior)
	{
		if(m_vBehavior[nBehavior] &&
			m_vBehavior[nBehavior]->getBehaviorType() == MobElementBehavior::AirsideBehavior)
			return (AirsideMobElementBehavior*)m_vBehavior[nBehavior];
	}

	return NULL;
}

PaxOnboardBaseBehavior* MobileElement::getOnboardBehavior()const
{
	int nBehaviorCount = (int)m_vBehavior.size();
	for (int nBehavior = 0; nBehavior < nBehaviorCount; ++nBehavior)
	{
		if(m_vBehavior[nBehavior] &&
			m_vBehavior[nBehavior]->getBehaviorType() == MobElementBehavior::OnboardBehavior)
			return (PaxOnboardBaseBehavior*)m_vBehavior[nBehavior];
	}

	return NULL;
}
// The receiver's location will not actually be current until the passed eventTime.
void MobileElement::generateEvent (ElapsedTime eventTime ,bool bNoLog)
{
    static short testID = -1;
    MobileElementMovementEvent *aMove = new MobileElementMovementEvent;
    ElapsedTime destTime = aMove->removeMoveEvent (this);

//    if (id == testID)
 //       cout << '\r';

	if( m_prevEventTime > ElapsedTime(302400l) && eventTime < ElapsedTime(262800l) )
		assert( 0 );
    aMove->init (this, eventTime,bNoLog);
    aMove->addEvent();
	m_prevEventTime = aMove->getTime();

}

int MobileElement::move( ElapsedTime currentTime,bool bNoLog )
{
	PLACE_METHOD_TRACK_STRING();
	return 0;
}

LandsideBaseBehavior* MobileElement::getLandsideBehavior() const
{
	int nBehaviorCount = (int)m_vBehavior.size();
	for (int nBehavior = 0; nBehavior < nBehaviorCount; ++nBehavior)
	{
		if(m_vBehavior[nBehavior] &&
			m_vBehavior[nBehavior]->getBehaviorType() == MobElementBehavior::LandsideBehavior)
			return (LandsideBaseBehavior*)m_vBehavior[nBehavior];
	}

	return NULL;
}

void MobileElement::ClearBehaviors()
{
	int nBehaviorCount = (int)m_vBehavior.size();
	for (int nBehavior = 0; nBehavior < nBehaviorCount; ++nBehavior)
	{
		if(m_vBehavior[nBehavior] )
			delete m_vBehavior[nBehavior];
	}

	m_vBehavior.clear();
}
//Searches unsorted contain for a given mobile element and returns its index if found; otherwise returns INT_MAX.
int MobileElementList::findElement (long p_id)
{
    int count = getCount();
    MobileElement *element;
    for (int i = 0; i < count; i++)
    {
        element = getItem (i);
        if (element->getID() == p_id)
            return i;
    }
    return INT_MAX;
}

//Searches sorted contain for a given mobile element and returns its index if found; otherwise returns INT_MAX.
int SortedMobileElementList::findElement (long p_id)
{
    int count = getCount();
    MobileElement *element;
    for (int i = 0; i < count; i++)
    {
        element = getItem (i);
        if (element->getID() == p_id)
            return i;
    }
    return INT_MAX;
}

