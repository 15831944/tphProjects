#pragma once
#include "InputAirsideAPI.h"
#include "..\Common\FLT_CNST.H"
#include "..\Common\TimeRange.h"
#include "RunwayExit.h"
#include "TrainingFlightContainer.h"
#include "RunwayExitDiscription.h"

class ALTObjectID;
class TimeRange;

class INPUTAIRSIDE_API CTrainingTakeOffPosition
{
public:
	CTrainingTakeOffPosition();;
	~CTrainingTakeOffPosition();;
	enum PosOrList
	{
		TB_POSITION,
		TB_POSITIONLIST
	};
public:
	int m_nID;
	int	m_TakeOffPax;
	int m_ApproachPax;
	bool m_TakeOffSequence;
	bool m_ApproachSequence;
	std::vector<int> m_runwayIdList;
public:
	void ReadData(std::vector<int>&_data,int _id);
	void WriteData(std::vector<int>&_data,int _id);
	void DeleteDate(int _id);

	void ReadData(std::vector<CTrainingTakeOffPosition*>&_data ,int _parID);
	void WriteData(std::vector<CTrainingTakeOffPosition*>&_data ,int _parID);
	void DeleteDate(std::vector<CTrainingTakeOffPosition*>&_data);

	void RemoveData();
public:
	void SetID(int _id) { m_nID = _id;} 
	int GetID() { return m_nID;}
protected:
	CString GetSelectSQL(int _parID,PosOrList _Type);
	CString GetInsertSQL(int _id,int _parID);
	CString GetInsertSQL(CTrainingTakeOffPosition* _item ,int _parID);
	CString GetUpdateSQL(CTrainingTakeOffPosition* _item ,int _parID);
	CString GetDeleteSQL(int _id,PosOrList _type);	
};

class INPUTAIRSIDE_API CTrainingTimeRangeManage
{
public:

	CTrainingTimeRangeManage();;
	~CTrainingTimeRangeManage();;

public:

	typedef std::vector<CTrainingTakeOffPosition*> TY_DATA;
	typedef TY_DATA::iterator TY_DATA_ITER;

public:

	int m_ID;
	TY_DATA m_TakeOffPositionList;
	TY_DATA m_DelTakeOffPosition;	
	TimeRange m_dtTimeRange;
	CTrainingTakeOffPosition m_TakeOffPosition;
public:

	int GetID() { return m_ID;} 
	void SetID(int _id) { m_ID = _id;} 	
	TY_DATA* GetTakeOffPosData() { return &m_TakeOffPositionList;}
	void SetStartTime(ElapsedTime _range) { m_dtTimeRange.SetStartTime(_range);};
	ElapsedTime GetStartTime() { return m_dtTimeRange.GetStartTime();};

	void SetEndTime(ElapsedTime _time) { m_dtTimeRange.SetEndTime(_time);};
	ElapsedTime GetEndTime() { return m_dtTimeRange.GetEndTime();};
	void RemoveAllData();
	void RemoveDelData();
	CTrainingTakeOffPosition* AddTrainTakeoffPosition(const std::vector<RunwayExit> _exitlist);
	void RemoveTrainTakeoffPosition(CTrainingTakeOffPosition* postion);
	CTrainingTakeOffPosition* FindTakeoffPosition(const std::vector<RunwayExit>& exit);

public:

	void ReadData(std::vector<CTrainingTimeRangeManage*>& _data,int _ParID);
	void WriteData(std::vector<CTrainingTimeRangeManage*>& _data,int _parID);
	void DeleteData(std::vector<CTrainingTimeRangeManage*>& _data);

protected:

	CString GetSelectSQL(int _parID);
	CString GetInsertSQL(CTrainingTimeRangeManage* _timeRange ,int _parID);
	CString GetUpdateSQL(CTrainingTimeRangeManage* _timeRange ,int _parID);
	CString GetDeleteSQL(CTrainingTimeRangeManage* _timeRange);

};

class INPUTAIRSIDE_API  CTrainingFlightTypeManage
{
public:
	CTrainingFlightTypeManage():m_ID(-1) {} 
	~CTrainingFlightTypeManage(){ RemoveAllData();} 

public:

	typedef std::vector<CTrainingTimeRangeManage*> TY_DATA;
	typedef TY_DATA::iterator TY_DATA_ITER;

public:

	TY_DATA m_TimeRangeList;
	TY_DATA m_DelTimeRange;
	CTrainingTimeRangeManage m_TimeRange;
protected:
	int m_ID;
	int m_FlightID;
	CString m_FltType;
public:
	void SetFlightType(CString _flight) { m_FltType = _flight;};
	CString GetFlightType() {return m_FltType;};
	TY_DATA* GetTimeRangeData() { return &m_TimeRangeList;} 
public:

	void SetID(int _id) { m_FlightID = _id;} 
	int GetID() { return m_FlightID;} 
	int GetCount() const;
	CTrainingTimeRangeManage* GetItem( int idx ) const;

public:

	 CString GetSelectSQL(int _parID);
	 CString GetInsertSQL(CTrainingFlightTypeManage* _TrainFlight ,int _ParID);
	 CString GetUpdateSQL(CTrainingFlightTypeManage* _TrainFlight ,int _ParID);
	 CString GetDeleteSQL(CTrainingFlightTypeManage* _TrainFlight);

	 void ReadData(std::vector<CTrainingFlightTypeManage*>& _data, int _ParID);
	 void WriteData(std::vector<CTrainingFlightTypeManage*>& _data,int _ParID);
	 void DeleteData(std::vector<CTrainingFlightTypeManage*>& _data);

public:

	CTrainingTimeRangeManage* AddTrainingTimeRange(TimeRange& _flt);
	CTrainingTimeRangeManage* FindTrainingTimeRangee( TimeRange& _flt);
	void RemoveTrainingTimeRangeeItem(CTrainingTimeRangeManage* _data);

public:

	void RemoveAllData ();
	void RemoveDeleteData();
};

class INPUTAIRSIDE_API CTrainingFlightsManage
{
public:
	CTrainingFlightsManage(){};
	~CTrainingFlightsManage(void);

	typedef std::vector<CTrainingFlightTypeManage*> TY_DATA;
	typedef TY_DATA::iterator TY_DATA_ITER;
	CTrainingFlightTypeManage m_Flightyp;
public:

	TY_DATA m_FlightData;	

public:

	void ReadData();
	void WriteData();
	void ClearData();

public:

	std::vector<CTrainingFlightTypeManage*>* GetFlayTypeData() { return &m_FlightData;};
	CTrainingFlightTypeManage* AddTrainFlightType(CTrainingFlightData& flightType);
	CTrainingFlightTypeManage* FindFlightTypeItem(CTrainingFlightData& flightType);
	void RemoveAllData();

};