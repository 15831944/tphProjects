#pragma once
#include "airsidebasereport.h"
#include "../Common/AIRSIDE_BIN.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../common/FLT_CNST.H"
#include <vector>
using namespace std;
class CAirsideReportBaseResult;
class CFlightOperationalParam;

class FltOperationalItem
{
public:
	FltOperationalItem(CAirportDatabase* _AirportDB)
		:m_ID(-1)
		,m_strFltType(_T(""))
		, m_fAirDis(0.0)
		, m_fGroundDis(0.0)
		, m_lStayTime(0l)
		, m_lAirTime(0L)
		, m_lGroundTime(0L)
		, m_fAirFuel(0.0)
		, m_fGroundFuel(0.0)
		, m_fOperatingCost(0.0)
		, m_strLandRunway(_T(""))
		, m_fLandDist(0.0)
		, m_strRunwayExit(_T(""))
		, m_strArrStand(_T(""))
		, m_strIntStand(_T(""))
		, m_strDepStand(_T(""))
		, m_strTakeoffRunway(_T(""))
		, m_strTakeoffIntersection(_T(""))
		, m_fTakeoffDistance(0.0)
		, m_fMaxAirSpeed(0.0)
		, m_fMaxTaxiSpeed(0.0)
		, m_fAvgTaxiSpeed(0.0)
	{
		m_fltCons.SetAirportDB(_AirportDB) ;
	}
	~FltOperationalItem()
	{
	}

	int m_ID;
	AirsideFlightDescStruct m_fltDesc;
	bool					m_bArrival;
	FlightConstraint m_fltCons;
	CString m_strFltType;              //flight type
	float   m_fAirDis;                 //air distance(km)
	float   m_fGroundDis;              //ground distance(m)
	long	m_lStayTime;				//stay time(second)
	long    m_lAirTime;                //air times(second)
    long    m_lGroundTime;             //ground time(second)
	float   m_fAirFuel;                //air fuel(kg)
	float   m_fGroundFuel;             //ground fuel(kg)
	float   m_fOperatingCost;          //operating cost($)
	CString m_strLandRunway;           //land runway
	float   m_fLandDist;               //land dist(m)
	CString m_strRunwayExit;           //runway exit
	CString m_strArrStand;             //arr stand
	CString m_strIntStand;             //int stand
	CString m_strDepStand;             //Dep stand
	CString m_strTakeoffRunway;        //takeoff runway
	CString m_strTakeoffIntersection;  //takeoff intersection
	float   m_fTakeoffDistance;        //takeoff distance(m)
	float   m_fMaxAirSpeed;            //max air speed(kts)
	float   m_fMaxTaxiSpeed;           //max taxi speed(kts)
	float   m_fAvgTaxiSpeed;           //avg taxi speed(kts)

public:
	BOOL ExportFile(ArctermFile& _file) ;
	BOOL ImportFile(ArctermFile& _file) ;
};

class CFlightOperationalReport :
	public CAirsideBaseReport
{
public:
	CFlightOperationalReport(CBGetLogFilePath pFunc);
	virtual ~CFlightOperationalReport(void);

	enum subReportType
	{
		OPERATIONAL_FLIGHTTYPEVSAIRDISTANCE = 0,
		OPERATIONAL_FLIGHTTYPEVSGROUNDDISTANCE,

		//add item before this line
		OPERATIONAL_COUNT
	};

public:
	virtual void GenerateReport(CParameters * parameter);
	virtual int GetReportType();

	void RefreshReport(CParameters * parameter);
	CAirsideReportBaseResult *GetReportResult()         {    return m_pResult;          }
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
    vector<FltOperationalItem>&  GetResult()        {    return m_vResult;   }

private:
	bool fits(CParameters * ,const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons,FLTCNSTR_MODE& fltConMode);
	void AddToResult(CParameters* param, FltOperationalItem& fltOperationalItem);
	//return true if point on air, return false if point not on air
	bool IsPointOnAir(AirsideFlightEventStruct& eventStruct);
	//return true if point on ground, return false if point not on ground
	bool IsPointOnGround(AirsideFlightEventStruct& eventStruct);
	//return true if point between OnLanding and OnExitRunway
	bool IsPointBetweenOnLandingAndOnExitRunway(AirsideFlightEventStruct& eventStruct);
	//return true if point between OnTakeoff and OnClimbOut
	bool IsPointBetweenOnTakeoffAndOnClimbOut(AirsideFlightEventStruct& eventStruct);
	//return true if point on taxiway , return false if point isn't on taxiway
	bool IsPointOnTaxiway(AirsideFlightEventStruct& eventStruct);
	bool IsPassObject(AirsideFlightLogEntry& logEntry,CFlightOperationalParam * pParam,int& startpos,int& endpos,bool bArrival);
	void CalculateOperationResult(AirsideFlightLogEntry& logEntry,CFlightOperationalParam * pParam,AirsideFlightDescStruct& fltDesc,int nStartPos,int nEndPos,bool bArrival);

	void CalcAirDistance(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcGroundDistance(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcAirTime(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcGourndTime(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcAirFuel(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcGroundFuel(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcOperatingCost(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcLandRunway(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcLandDist(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcRunwayExit(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcArrStand(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcIntStand(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcDepStand(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, AirsideFlightDescStruct&	fltDesc, FltOperationalItem& fltOperationalItem);
	void CalcTakeoffRunway(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, AirsideFlightDescStruct& fltDesc, FltOperationalItem& fltOperationalItem);
	void CalcTakeoffIntersection(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, AirsideFlightDescStruct& fltDesc, FltOperationalItem& fltOperationalItem);
	void CalcTakeoffDistance(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, AirsideFlightDescStruct& fltDesc, FltOperationalItem& fltOperationalItem);
	void CalcMaxAirSpeed(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcMaxTaxiSpeed(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalcAvgTaxiSpeed(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	long CalTowingTime(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	void CalStayTime(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);
	float CalTowAndPushbackDistance(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem);

	int GetIntersectionNodeID(const AirsideFlightEventStruct& event)const;
protected:
	std::vector<FltOperationalItem> m_vResult;
	CAirsideReportBaseResult *m_pResult;
	CAirsideReportBaseResult *m_pNodeDelayResult;
public:
	BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
protected:
	BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;
};
