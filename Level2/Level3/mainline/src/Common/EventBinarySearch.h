#pragma once
#include <algorithm>

template <typename EventListType>
static int LinearSearchEvent( const EventListType& eventList,long curTime)
{
	int nCount = eventList.size();
	for(int i=0;i<nCount;i++)
	{
		if( eventList[i].time >= curTime ) 
			return i;
	}
	return -1;
}

class EventTimeComparer
{
public:
	template <class EventType>
		bool operator()(const EventType& e,long curTime)
	{
		return e.time < curTime;
	}

	template <class EventType>
		bool operator()(const EventType& e,const EventType& t)
	{
		return e.time < t.time;
	}
};

template <typename EventListType>
static int BinarySearchEvent( const EventListType& eventList, long curTime )
{
	EventListType::const_iterator ite = std::lower_bound(eventList.begin(), eventList.end(), curTime, EventTimeComparer());
	if (ite != eventList.end())
	{
		return ite - eventList.begin();
	}
	return -1;
}
