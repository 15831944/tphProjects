#include "StdAfx.h"
#include ".\steppableschedule.h"
#include <algorithm>
#include "ISteppable.h"
#include "ScheduleStepEvent.h"
SteppableSchedule::SteppableSchedule(void)
{
}

SteppableSchedule::~SteppableSchedule(void)
{
}

void SteppableSchedule::add( ISteppable* elem, const Key& key )
{
	m_SteppableList.push_back(DataValueType(key,elem));
	std::sort(m_SteppableList.begin(),m_SteppableList.end());
}

bool SteppableSchedule::step( const ElapsedTime& t,CARCportEngine& state )
{		// now change the time
	DataValueList currentSteps;	
	
	if( extractMin(t,currentSteps) )  // come out in reverse order
	{		
		// shuffle
		if (!currentSteps.empty()) 
		{
			int len = currentSteps.size();
			//try
			{
				for(int x=0;x<len;x++)  // if we're not being killed...
				{
					ISteppable* pStepable = currentSteps[x].second;
					pStepable->step(state);						
				}
			}			
		}
		return true;
		// execute			
		//time = minKey.time;
	}	
	return false;
}

bool SteppableSchedule::extractMin( const ElapsedTime& time, DataValueList& minList )
{
	if(m_SteppableList.empty())
		return false;
	
	DataValueList::iterator itrFind;
	for(itrFind = m_SteppableList.begin();itrFind!=m_SteppableList.end();++itrFind)
	{
		if(time < itrFind->first.time )
		{
			break;
		}

	}	
	minList = DataValueList(m_SteppableList.begin(),itrFind);
	m_SteppableList.erase(m_SteppableList.begin(),itrFind);
	return true;
}
bool SteppableSchedule::getTime( ElapsedTime& t ) const
{
	if(m_SteppableList.empty())
	{
		return false;
	}
	const Key& minKey = m_SteppableList.begin()->first;
	t  = minKey.time;
	return true;
}

bool SteppableSchedule::scheduleOnce( const Key& key, ISteppable* event )
{
	add(event,key);	
	//(new ScheduleStepEvent(this,key.time))->addEvent();
	return true;
}
int SteppableSchedule::Key::compareTo( const Key& o ) const
{
	const ElapsedTime& time2 = o.time;
	if (time.getPrecisely() == time2.getPrecisely())  // the most common situation
	{				
		int ordering2 = o.ordering;
		if (ordering == ordering2) return 0;  // the most common situation
		if (ordering < ordering2) return -1;
		// if (ordering > ordering2)  return 1;
	}
	// okay, so they're different times
	if (time < time2) return -1;
	/* if (time > time2) */ 
	return 1;
}