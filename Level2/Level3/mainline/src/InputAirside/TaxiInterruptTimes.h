#pragma once
#include "../Common/elaptime.h"
#include "../Common/FLT_CNST.H"
#include "../Common/ProDistrubutionData.h"
#include "InputAirsideAPI.h"
#include "TaxiLinkedRunwayData.h"

class INPUTAIRSIDE_API CTaxiInterruptTimeItem
{
public:
     CTaxiInterruptTimeItem() ;
	 ~CTaxiInterruptTimeItem() ;

public:
	int GetID() { return m_ID ;} ;
	void  SetID(int _id) { m_ID = _id ;} ;

	int GetFlightTyID() { return m_FlightTyID ;} ;
	void SetFlightTyID(int _id) { m_FlightTyID = _id ;} ;

	int GetShortLineID() { return m_ShortLineID ;} ;
	void SetShortLineID(int _id) { m_ShortLineID = _id ;} ;

	ElapsedTime GetStartTime() { return m_Startime ;} ;
	void SetStartTime(ElapsedTime _time ) { m_Startime = _time ;} ;

	ElapsedTime GetEndTime() { return m_EndTime ;} ;
	void SetEndTime(ElapsedTime _time) { m_EndTime = _time ;} ;
    
	 CProDistrubution* GetHoldTime() { return &m_HoldTime ;} ;
	void SetHoldTime(CProbDistEntry* pPDEntry) { m_HoldTime.SetProDistrubution(pPDEntry) ;} ;

	int GetDirection() { return m_Direction ;} ;
	void SetDirection(int _direct) { m_Direction = _direct ;} ;

	void SetLinkedRunway(bool bLinked);
	bool IsLinkedRunway();

	CTaxiLinkedRunwayData* GetLinkedRunwayDataByIdx(int idx);
	int GetLinkedRunwayDataCount();

	CTaxiLinkedRunwayData* GetLinkedRunwayData(int nRwyID, int nRwyMark);

	void AddLinkedRunwayData(CTaxiLinkedRunwayData* pData);
	void DelLinkedRunwayData(CTaxiLinkedRunwayData* pData);

public:
	void SaveData(int FlightTyID);
	void DelData() ;
	void UpdateData();

	static void ReadData(std::vector<CTaxiInterruptTimeItem*>& _vectorData , int FlightTyID) ;

private:
	void ReadLinkedRunwayData();
	
private:
	int m_ID ;
	int m_FlightTyID ;
	int m_ShortLineID ;
	ElapsedTime m_Startime ;
	ElapsedTime m_EndTime ;
	CProDistrubution m_HoldTime ;
	int m_Direction ;
	bool m_bLinkedRunway;

	std::vector<CTaxiLinkedRunwayData*> m_vLinkedRunwayData;
	std::vector<CTaxiLinkedRunwayData*> m_vDelLinkedRunwayData;
};

class INPUTAIRSIDE_API CTaxiInterruptFlightData
{
public:
	typedef std::vector<CTaxiInterruptTimeItem*> TY_DATA ;
	typedef TY_DATA::iterator INTERRUPTTIMEITER ;
public:
     CTaxiInterruptFlightData() ;
	 ~CTaxiInterruptFlightData() ;
private:
	int m_FlightTyID ;
	FlightConstraint m_FlightTy ;
	TY_DATA m_DataSet ;
	TY_DATA m_DelDataSet ;
protected:
	void ClearDelData() ;
	void ClearAllData() ;
public:
	int GetCount()const;
	CTaxiInterruptTimeItem* GetItem(int idx)const;
	FlightConstraint* GetFlightTy(); 

	int GetFlightTyID() { return m_FlightTyID ;} 
	void SetFlightTyID(int _id) { m_FlightTyID = _id ;} 

	void ReadData() ;
	void SaveData() ;
	void DelData();
	
	void AddTaxiInterruptTimeItem(CTaxiInterruptTimeItem* _item) ;
	void DelTaxiInterruptTimeItem(CTaxiInterruptTimeItem* _item) ;

	void DelAllData() ;
};

class INPUTAIRSIDE_API CTaxiInterruptTimes
{
public:
	CTaxiInterruptTimes(CAirportDatabase *airportDB);
	~CTaxiInterruptTimes(void);
public:
	typedef std::vector<CTaxiInterruptFlightData*> TY_DATA_FLIGHT ;
	typedef TY_DATA_FLIGHT::iterator TY_DATA_FLIGHT_ITER ;
public:
    TY_DATA_FLIGHT m_DataSet ;
	TY_DATA_FLIGHT m_DelDataSet ;
	CAirportDatabase * m_airportDB ;
public:
	void ReadData() ;
	void SaveData() ;

	int GetCount()const;
	CTaxiInterruptFlightData* GetItem(int idx){ return m_DataSet.at(idx); }
    

	void AddInterruptTimes(CTaxiInterruptTimeItem* _item  , FlightConstraint flightTy) ;
	void DelInterruptTimes(CTaxiInterruptTimeItem* _item , FlightConstraint flightTy) ;

	CTaxiInterruptFlightData* AddFlightTy(FlightConstraint _constraint ) ;
	void DelFlightData(  CTaxiInterruptFlightData* _pair) ;

	CTaxiInterruptFlightData* FindByFlightID(  FlightConstraint flightTy) ;

protected:
	void clearDelData() ;
	void clearAllData() ;

};
