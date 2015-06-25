#pragma once
#include "InboudRouteAssTimeRange.h"
#include "../common/TimeRange.h"
#include "InboudRouteAssTimeRangeContainer.h"
#define FLIGHTTY_NODE   0x0010 
class CRouteAssFlightTY : public CNodeType
{
public:

	CRouteAssFlightTY(int m_projectID);
	virtual ~CRouteAssFlightTY(void);

	CString GetFlightType() ;
	void SetFlightType(const CString& name) ;

	void SetFlightTyID(int ID) ;
	int  GetFlightTyID() ;

	virtual CRouteAssTimeRange*  AddTimeRange(TimeRange* timerange) ;
	void DelTimeRange(CRouteAssTimeRange* timerange) ;
	void ReadTimeTanges() ;
	CRouteAssTimeRangeContainer::TIMERANGE_VECTOR& GetTimeRanges() ;


protected:
	CRouteAssTimeRangeContainer* P_timeranges ;
	int  FlightTy_id ;
	CString flightTy_Name ;
	int m_proID ;
public:
	CRouteAssFlightTY(int m_projectID,int flightTy_ID) ;
public:

	void DelFlightTyToDB() ;
	void OnSaveData() ;
protected:
	void SaveFlightTyToDB() ;
	int class_TY ;
};
class CInboudRouteAssFlightTY : public CRouteAssFlightTY
{
public:

	CInboudRouteAssFlightTY(int m_projectID);
	virtual ~CInboudRouteAssFlightTY(void);
    CInboudRouteAssFlightTY(int m_projectID,int flightTy_ID) ;

};
class COutboundRouteAssFilghtTY : public CRouteAssFlightTY
{
public:
     COutboundRouteAssFilghtTY(int m_projectID);
	 COutboundRouteAssFilghtTY(int m_projectID,int flightTy_ID) ;
	 ~COutboundRouteAssFilghtTY() ;
};