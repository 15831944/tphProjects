#pragma once

#include <boost/noncopyable.hpp>
#include <vector>
#include ".\boost_text_serialize.h"
#include <common\Types.h>
//#include <boost/serialization/vector.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
class ARCEventLog
{
	DynamicClass
public:
	long time;
	ARCEventLog();
	virtual ~ARCEventLog();
	
	DECLARE_SERIALIZE
	
};



//Event List
class ARCEventLogList :  public boost::noncopyable
{
public:
	ARCEventLogList(){}
	~ARCEventLogList(){ clear(); }
	typedef std::vector<ARCEventLog*> EventLogList;
	void clear();

	void addEvent(ARCEventLog* newLog){ mEventList.push_back(newLog); }

	//oder by time
	struct EventTimeSorter
	{
		bool operator()(ARCEventLog* pEvent1,ARCEventLog* pEvent2)const{
			if(pEvent1 && pEvent2)
				return pEvent1->time < pEvent2->time;
			return false;
		}
	};
	//sort by time order
	void Sort();

	EventLogList mEventList;
	
	DECLARE_SERIALIZE
};

#include <boost/serialization/version.hpp>
#define EventLogVersion 8
BOOST_CLASS_VERSION(ARCEventLogList, EventLogVersion)


