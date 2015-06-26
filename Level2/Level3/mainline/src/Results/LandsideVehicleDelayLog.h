#pragma once
#include "Common\Termfile.h"
#include <vector>


class LandsideVehicleDelayItem;
class ArctermFile;


class LandsideVehicleDelayLog
{
public:
	enum EnumDelayReason
	{
		DR_TrafficLight = 100,
		DR_YeildSign,
		DR_ForeheadCar,
		DR_ForCrossWalk,
		DR_NoReason,



		DR_Count
	};

	const static CString EnumDelayReasonName[];
	
	
	enum EnumDelayOperation
	{
		DO_Stop = 200,
		DO_SlowDown,
		DO_Normal,


		DO_Count
	};
	const static CString EnumDelayOperationName[];




public:
	LandsideVehicleDelayLog(void);
	~LandsideVehicleDelayLog(void);



	//open log file, if not exist create one , else clear old data;
	bool CreateLogFile( const CString& _csFullFileName);
	void LoadData(const CString& _csFullFileName);
	void SaveData(const CString& _csFullFileName);


	//write delay to log ,assert log file is open
	void WriteDelay(LandsideVehicleDelayItem* pDelay);


	void ClearData();
	int  GetItemCount()const{ return (int)m_vDelays.size(); }
	LandsideVehicleDelayItem* GetItem(int idx){ return m_vDelays.at(idx); }
	void AddItem(LandsideVehicleDelayItem* pDelay){ m_vDelays.push_back(pDelay); }
protected:
	ArctermFile m_LogFile;
	std::vector<LandsideVehicleDelayItem*> m_vDelays;

};


class LandsideVehicleDelayItem
{
public:
	LandsideVehicleDelayItem();
	~LandsideVehicleDelayItem();



public:

	static LandsideVehicleDelayItem* ReadLog(ArctermFile& inFile);

	void WriteLog(ArctermFile& outFile);


public:
	
	//vehicle
	int m_nVehicleID;
	int m_nVehicleType;
	
	//delay at the time start and end
	ElapsedTime m_eStartTime;
	ElapsedTime m_eEndTime;

	
	//the time delayed
	ElapsedTime m_eDelayTime;

	//Resource
	int m_nResourceID;
	CString m_strResName;

	//Reason
	LandsideVehicleDelayLog::EnumDelayReason m_enumReason;

	//Operation
	LandsideVehicleDelayLog::EnumDelayOperation m_enumOperation;

	//Description
	CString m_strDesc;





private:
};