#pragma once
#include "..\Database\ADODatabase.h"
#include "..\Common\elaptime.h"
#include "..\Common\FLT_CNST.H"
#include "InputAirsideAPI.h"

class CAirportDatabase ;
class INPUTAIRSIDE_API CAbandonmentPoint
{
public:
	enum Point_Type { TY_Stand , TY_Intersection};
	CString m_Name ;
	int m_ID ;
	double m_Dis ;
	int m_SelType ;
	Point_Type m_PointType ; 

	CAbandonmentPoint();
};
class INPUTAIRSIDE_API CFollowMeConnectionStand
{
public:
	CFollowMeConnectionStand(); 
	~CFollowMeConnectionStand(); 
public:
	CString m_StandInSim ;
	std::vector<CString> m_MeetPointData ;
	CAbandonmentPoint m_AbandonmentPoint ;
public:
	static void ReadData(std::vector<CFollowMeConnectionStand*>& _data ,int _ParID) ;
	static void WriteData(std::vector<CFollowMeConnectionStand*>& _data ,int _ParID) ;
	static void DeleteData(std::vector<CFollowMeConnectionStand*>& _data) ;
public:
	void AddMeetingPoint(const CString&  _point) ;
	void RemoveMeetingPoint(CString _PointID) ;

	CAbandonmentPoint* GetAbandonmentPoint(); ;
protected:
	int m_ID ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;
	void ClearAll() ;
public:
	static CString GetSelectSQL(int _parID) ;
	static CString GetInsertSQL(CFollowMeConnectionStand* _stand ,int _parID) ;
	static CString GetUpdateSQL(CFollowMeConnectionStand* _stand ,int _parID) ;
	static CString GetDeleteSQL(CFollowMeConnectionStand* _stand) ;
protected:
	static void ReadMeetingPoint(CFollowMeConnectionStand* PStand ) ;
	static void WriteMeetingPoint(CFollowMeConnectionStand* pStand) ;
};

class INPUTAIRSIDE_API CFollowMeConnectionRunway
{
public:
	CFollowMeConnectionRunway(); 
	~CFollowMeConnectionRunway();
public:
	std::vector<CFollowMeConnectionStand*> m_Data ;
	std::vector<CFollowMeConnectionStand*> m_DeleteData ;
public:
	static void ReadData(std::vector<CFollowMeConnectionRunway*>& _data ,int _ParID) ;
	static void WriteData(std::vector<CFollowMeConnectionRunway*>& _data ,int _ParID) ;
	static void DeleteData(std::vector<CFollowMeConnectionRunway*>& _data) ;
protected:
	int m_ID ;
	int m_RunwayID ;
	int m_RunwayMarketID ;
	CString m_runwayName ;
public:
	int GetRunwayID() { return m_RunwayID ;};
	void SetRunwayID( int _str) { m_RunwayID = _str ;} ;

	void SetRunwayMarketID(int _id) { m_RunwayMarketID = _id ;} ;
	int GetRunwayMarkerID() { return m_RunwayMarketID ;} ;

	CString GetRunwayName() { return m_runwayName ;} ;
	void SetRunwayName(const CString& _str) { m_runwayName = _str ;} ;


	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;

	int GetRwyID();
	int GetRwyMark();
public:
	void ClearAll() ;
	void ClearDelData() ;
protected:
	static CString GetSelectSQL(int _parID) ;
	static CString GetInsertSQL(CFollowMeConnectionRunway* _item , int _parID) ;
	static CString GetUpdateSQL(CFollowMeConnectionRunway* _item , int _parID) ;
	static CString GetDeleteSQL(CFollowMeConnectionRunway* _item) ;
public:
	CFollowMeConnectionStand* AddStandItem(const CString& _stand) ;
	void RemoveStandItem(CFollowMeConnectionStand* _dataItem) ;
};

class INPUTAIRSIDE_API CFollowMeConnectionTimeRangeItem
{
public:
	CFollowMeConnectionTimeRangeItem() ;
	~CFollowMeConnectionTimeRangeItem() ;
public:
	std::vector<CFollowMeConnectionRunway*> m_Data ;
	std::vector<CFollowMeConnectionRunway*> m_DelData ;
protected:
	ElapsedTime m_startTime ;
	ElapsedTime m_EndTime ;
	int m_ID ;
public:
	int GetID() { return m_ID ;} ;
	void SetID(int _id) { m_ID = _id ;} ;
public:
	static void ReadData(std::vector<CFollowMeConnectionTimeRangeItem*>& _data ,int _parID) ;
	static void WriteData(std::vector<CFollowMeConnectionTimeRangeItem*>& _data ,int _parID) ;
	static void DeleteData(std::vector<CFollowMeConnectionTimeRangeItem*>& _data) ;
public:
	void ClearAll() ;
	void ClearDelData() ;
public:
	void SetStartTime(const ElapsedTime& _time) { m_startTime = _time ;};
	ElapsedTime GetStartTime();;

	void SetEndTime(const ElapsedTime& _time) { m_EndTime = _time ;};
	ElapsedTime GetEndTime();;
protected:
	static CString GetSelectSQL(int _parID) ;
	static CString GetInsertSQL(CFollowMeConnectionTimeRangeItem* _item ,int _parID) ;
	static CString GetUpdateSQL(CFollowMeConnectionTimeRangeItem* _item ,int _parID) ;
	static CString GetDeleteSQL(CFollowMeConnectionTimeRangeItem* _item) ;
public:
	CFollowMeConnectionRunway* AddRunwayItem(const CString& _runway ,int _runwayID ,int _runwayMarkID) ;
	void RemoveRunwayItem(CFollowMeConnectionRunway* _runway) ;
};

class INPUTAIRSIDE_API CFollowMeConnectionItem
{
public:
	CFollowMeConnectionItem(void):m_ID(-1) {};
	~CFollowMeConnectionItem() {ClearAll() ;} ;
protected:
	std::vector<CFollowMeConnectionTimeRangeItem*> m_Data ;
	std::vector<CFollowMeConnectionTimeRangeItem*> m_DelData ;
	FlightConstraint m_FlightConstraint ;
	int m_ID ;
public:
	static void ReadData(std::vector<CFollowMeConnectionItem*>& _data ,int _ParID,CAirportDatabase* _AirportDB) ;
	static void WriteData(std::vector<CFollowMeConnectionItem*>& _data , int _parID) ;
	static void DeleteData(std::vector<CFollowMeConnectionItem*>& _data) ;
public:
	void ClearAll() ;
	void ClearDeleteAll() ;
	void SetFlightConstraint(const FlightConstraint& _fltCon);;
	const FlightConstraint& GetFlightConstraint(); 
	int GetID() { return m_ID ;} ;
	void SetID(int _id) { m_ID = _id ;} ;
protected:
	static CString GetSelectSQL() ;
	static CString GetInsertSQL(CFollowMeConnectionItem* _item) ;
	static CString GetUpdateSQL(CFollowMeConnectionItem* _item) ;
	static CString GetDeleteSQL(CFollowMeConnectionItem* _item) ;
public:
	std::vector<CFollowMeConnectionTimeRangeItem*>* GetData(); ;
	std::vector<CFollowMeConnectionTimeRangeItem*> * GetDelData() { return &m_DelData ;} ;

	void RemoveRangeItem(CFollowMeConnectionTimeRangeItem* _data) ;
	CFollowMeConnectionTimeRangeItem* AddRangeItem(const ElapsedTime& _startTime ,const ElapsedTime& _endTime) ;
};
class INPUTAIRSIDE_API CFollowMeConnectionData
{
public:
	CFollowMeConnectionData(CAirportDatabase* _dataBase);;
	~CFollowMeConnectionData(void);;
protected:
	std::vector<CFollowMeConnectionItem*> m_Data ;
	std::vector<CFollowMeConnectionItem*> m_DeleteData ;
	CAirportDatabase* m_AirportDB ;
public:
	const std::vector<CFollowMeConnectionItem*>& GetData();
public:
	void ClearAll() ;
	void ReadData() ;
	void WriteData() ;
public:
	CFollowMeConnectionItem* AddNewItem(FlightConstraint& _fltCon) ;
	CFollowMeConnectionItem* FindItem(FlightConstraint& _fltCon) ;
	void DeleteItem(CFollowMeConnectionItem* _Pitem) ;
	void DeleteItem(FlightConstraint& _fltCon) ;
};
