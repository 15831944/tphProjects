#pragma once
#include <vector>
#include "../../common/elaptime.h"
#include "../../Common/point.h"
#include "../EngineDll.h"
#include "../../Common/AIRSIDE_BIN.h"
#include "AirsideFlightState.h"
#include "AirsideVehicleState.h"

class CAirsideMobileElement;

//Base write log event class
class AirsideMobElementWriteLogItem
{

public:
	AirsideMobElementWriteLogItem(CAirsideMobileElement *pMobElement);
	virtual ~AirsideMobElementWriteLogItem(void);

public:
	
	//execute the write log event
	void Execute();

protected:
	//Flight or vehicle
	CAirsideMobileElement *m_pAirsideElement;
	//AirsideFlightEventStruct m_logItem;

public:

};

// flight write log event class
class AirsideFlightWriteLogItem : public AirsideMobElementWriteLogItem
{
public:
	AirsideFlightWriteLogItem(CAirsideMobileElement *pMobElement);
	~AirsideFlightWriteLogItem();
public:
	//Log structure
	AirsideFlightEventStruct m_logItem;
	//flight state of the time
	AirsideFlightState m_fltState;
};

//vehicle write log  item
class CAirsideVehicleWriteLogItem :public AirsideMobElementWriteLogItem
{
public:
	CAirsideVehicleWriteLogItem(CAirsideMobileElement *pMobElement);
	~CAirsideVehicleWriteLogItem();

public:
	//vehicle event description
	AirsideVehicleEventStruct m_logItem;
	CAirsideVehicleState m_state;
};

// the write log item list
//vector as container
class AirsideMobElementWriteLogItemList
{
public:
	AirsideMobElementWriteLogItemList();
	~AirsideMobElementWriteLogItemList();


public:
	void AddItem(AirsideMobElementWriteLogItem* item);

	//AirsideMobElementWriteLogItem, the data returned
	//return:	NOT NULL - the data returned is valid
	//			NULL -the data is not valid, the list is empty
	AirsideMobElementWriteLogItem* GetItem();

	int GetItemCount();


protected:
	std::vector<AirsideMobElementWriteLogItem *> m_itemList;

};