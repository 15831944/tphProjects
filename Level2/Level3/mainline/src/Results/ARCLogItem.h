#pragma once

class ARCEventLogList;
class ARCEventLog;
#include "boost_text_serialize.h"
#include "../Common/ALT_TYPE.h"
#include "../Common/ALTObjectID.h"
class ARCLogItem
{
public:
	ARCLogItem(void);
	virtual ~ARCLogItem(void);


	//clear event list;
	void ClearEvents();		
	

	void AddEvent(ARCEventLog* pEvent);

	int nStartPos;   //event start pos in file
	long startTime;
	long endTime;
 
	ARCEventLogList* mpEventList;	//ref to a out event list
	
	DECLARE_SERIALIZE
};

class AirsideALTObjectLogItem
{
public:
	AirsideALTObjectLogItem();
	~AirsideALTObjectLogItem();

	void ClearEvents();	
	void AddEvent(ARCEventLog* pEvent);
	ALTObjectID m_altObject;
	ALTOBJECT_TYPE m_emType;

	DECLARE_SERIALIZE
};