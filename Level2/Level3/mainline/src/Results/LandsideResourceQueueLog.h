#pragma once
#include "Common\Termfile.h"
#include <vector>

class ArctermFile;
class LandsideResourceQueueItem;


class LandsideResourceQueueLog
{
public:
	LandsideResourceQueueLog(void);
	~LandsideResourceQueueLog(void);


	//open log file, if not exist create one , else clear old data;
	bool CreateLogFile( const CString& _csFullFileName);
	void LoadData(const CString& _csFullFileName);
	void SaveData(const CString& _csFullFileName);


	//write delay to log ,assert log file is open
	void Write(LandsideResourceQueueItem* pDelay);


	void ClearData();
	int  GetItemCount()const{ return (int)m_vItems.size(); }
	LandsideResourceQueueItem* GetItem(int idx){ return m_vItems.at(idx); }
	void AddItem(LandsideResourceQueueItem* pDelay){ m_vItems.push_back(pDelay); }
protected:
	ArctermFile m_LogFile;
	std::vector<LandsideResourceQueueItem*> m_vItems;
};


class LandsideResourceQueueItem
{
public:
	enum QueueOperation
	{
		QO_Entry = 0,
		QO_Exit,


		QO_Count
	};


public:
	LandsideResourceQueueItem();
	~LandsideResourceQueueItem();

public:
	static LandsideResourceQueueItem* ReadLog(ArctermFile& inFile);

	void WriteLog(ArctermFile& outFile);
public:

	//Resource
	int m_nResourceID;
	CString m_strResName;

	//vehicle
	int m_nVehicleID;
//	int m_nVehicleType;
	CString m_strVehicleType;

	//delay at the time start and end
	ElapsedTime m_eTime;


	//Reason
	QueueOperation m_enumOperation;



private:
};