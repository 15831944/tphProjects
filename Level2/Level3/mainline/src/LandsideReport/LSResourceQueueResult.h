#pragma once
#include "landsidebaseresult.h"
#include "LandsideReportAPI.h"
#include "..\Results\LandsideResourceQueueLog.h"


#include <map>

class LANDSIDEREPORT_API LSResourceQueueResult :
	public LandsideBaseResult
{
public:
	class QueueItem
	{
	public:
		QueueItem()
		{
			m_nResourceID = -1;
			m_enumOperation = LandsideResourceQueueItem::QO_Entry;
		}

		~QueueItem(){};

	public:
	//Resource
	int m_nResourceID;
	CString m_strResName;

	ElapsedTime m_eTime;

	CString m_strVehicleType;
	int m_nVehicleID;//for debug
	//Reason
	LandsideResourceQueueItem::QueueOperation m_enumOperation;
		
	};

	class ResQueue
	{
	public:
		ResQueue(){m_nResourceID = -1;}
		~ResQueue(){}
	public:
		int m_nResourceID;
		CString m_strResName;

		std::vector<QueueItem> m_vItems;

	};

protected:
	class DetailTimeItem
	{
	public:
		DetailTimeItem(){m_nLen = 0;}
		~DetailTimeItem(){}

	public:
		ElapsedTime m_eTime;
		int m_nLen;
	};
	class DetailItem
	{
	public:
		DetailItem(){m_nObjID = 0;}
		~DetailItem(){}

	public:
		int m_nObjID;
		CString m_strObjName;
		CString m_strVehicleType;
		std::vector<DetailTimeItem> m_vTimeItems;


	private:
	};

protected:
	std:: vector<DetailItem> m_vResDetail;

public:
	LSResourceQueueResult(void);
	~LSResourceQueueResult(void);


	std::map<int, ResQueue> m_mapResQueue;
	
public:
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);

protected:
	void AddItem(QueueItem& dItem);


};
