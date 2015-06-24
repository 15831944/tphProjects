#pragma once
#include "Common/elaptime.h"
class ISteppable;
class CARCportEngine;
class SteppableSchedule
{
public:
	class Key
	{
	public:
		ElapsedTime time;
		int ordering;

		Key(){ ordering = 0; }
		Key(const ElapsedTime& _time, int _ordering)
		{
			time = _time;
			ordering = _ordering;
		}
		int compareTo(const Key& o)const;

		bool operator<(const Key& o)const{ return compareTo(o)<0; }
	};

	
	typedef std::pair<Key,ISteppable*> DataValueType;
	typedef std::vector<DataValueType> DataValueList;

	bool step(const ElapsedTime& t, CARCportEngine& state );
	bool extractMin(const ElapsedTime& time, DataValueList& minList);
	bool getTime(ElapsedTime& t)const;

	DataValueList m_SteppableList;
	bool scheduleOnce(const Key& key,  ISteppable* event);

	SteppableSchedule(void);
	~SteppableSchedule(void);
private:
	void add( ISteppable* elem, const Key& key );
};
