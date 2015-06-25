#pragma once
#include "../InputAirside/InboundRouteAssignment.h"
#include "../common/TimeRange.h"
#define   INBOUD_ROUTE_FLIGHT    0x1000
#define   OUTBOUD_ROUTE_FLIGHT  0x1100
#define TIMERANGE_NODE  0x0011
class CNodeType
{
protected :
	int nodetype ; 
public:
	int GetType() { return nodetype ;} ;
	CNodeType(int nodeType) : nodetype(nodeType){};
	virtual ~CNodeType() {} ;
};
class CRouteAssTimeRange : public CNodeType
{
public:
	CRouteAssTimeRange(int m_pro);
	CRouteAssTimeRange();
	virtual ~CRouteAssTimeRange(void);
	CRouteAssTimeRange(int m_pro ,int flightID ,TimeRange* time_range) ;
	CRouteAssTimeRange(int m_pro ,int flightID ) ;
	virtual CString GetTimeRangeName() ;

	virtual void SetFlightTyID(int flight_id) ;
	virtual int  GetFlightTyID() ;

	virtual void SetTimeRangeID(int id) ;
	virtual int GetTimeRangeID() ;
	virtual void SetStartTime(ElapsedTime time) ;
	virtual void SetEndTime(ElapsedTime time ) ;
	virtual CInboundRouteAssignment* GetInboudRouteAssignment() ;
private:
	int m_flightty_ID ;
	int m_timerange_ID ;
	int m_proID ;
	TimeRange timeRange ; 
private:
	CInboundRouteAssignment Inboud_Route ;
public:
	virtual void SaveTimeRangeToDB() ;
	virtual void DelTimeRangeFromDB() ;
	//void ReadInboudRouteAssignment() ;
public:
	virtual void OnSaveData() ;
protected:
	int class_ty ;
};
class CInboudRouteAssTimeRange : public CRouteAssTimeRange
{
public:
	CInboudRouteAssTimeRange(int m_pro);
	CInboudRouteAssTimeRange();
	virtual ~CInboudRouteAssTimeRange(void);
    CInboudRouteAssTimeRange(int m_pro ,int flightID ,TimeRange* time_range) ;
    CInboudRouteAssTimeRange(int m_pro ,int flightID ) ;
};
class COutboundRouteAssTimeRange : public CRouteAssTimeRange
{
public:
	COutboundRouteAssTimeRange(int m_pro);
	COutboundRouteAssTimeRange();
	COutboundRouteAssTimeRange(int m_pro ,int flightID ,TimeRange* time_range) ;
	COutboundRouteAssTimeRange(int m_pro ,int flightID ) ;
	~COutboundRouteAssTimeRange() ;
};