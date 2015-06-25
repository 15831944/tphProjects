#pragma once
#include "InboudRouteAssTimeRange.h"
class CRouteAssTimeRangeContainer
{
public:
	CRouteAssTimeRangeContainer(int proID ,int FlightTy_ID,int classTY);
	virtual ~CRouteAssTimeRangeContainer(void);
public:
	typedef std::vector<CRouteAssTimeRange*> TIMERANGE_VECTOR ;
	typedef TIMERANGE_VECTOR::iterator   TIMERANGE_ITER ;
	typedef TIMERANGE_VECTOR::const_iterator TIMERANGE_ITER_CONST ;

private:
	TIMERANGE_VECTOR list_TimeRange ;
	TIMERANGE_VECTOR del_list_TimeRange ;
	int  m_proID ;
	int m_flightTyID ;
public:
	void SetFlightID(int flightID) ;
	void AddTimeRange(CRouteAssTimeRange* P_timerange) ;
	void DelTimeRange(CRouteAssTimeRange* P_timeRange) ;
	TIMERANGE_VECTOR& GetTimeRanges() ;
	void OnSaveData();
	void DelAllTimeRangesFromDB() ;
	void ReadTimeRangeFromDB() ;
protected:
    int class_TY ;
	virtual void InitTimeRanges(CADORecordset& record) ;
};
class CInboudRouteAssTimeRangeContainer : public CRouteAssTimeRangeContainer
{
public:
	CInboudRouteAssTimeRangeContainer(int proID ,int FlightTy_ID);
	~CInboudRouteAssTimeRangeContainer(void);
};

class COutboundRouteAssTimeRangeContaniner : public CRouteAssTimeRangeContainer
{
public:
	COutboundRouteAssTimeRangeContaniner(int proID ,int FlightTy_ID);
	~COutboundRouteAssTimeRangeContaniner(void);
};