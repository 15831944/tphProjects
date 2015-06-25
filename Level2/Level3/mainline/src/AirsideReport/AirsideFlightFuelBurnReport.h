#pragma once
#include "AirsideBaseReport.h"
#include "../Common/FLT_CNST.H"
#include <vector>
#include <algorithm>
#include "../Engine/Airside/CommonInSim.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../Results/OutputAirside.h"
#include "AirsideReportNode.h"
#include "AirsideReportBaseResult.h"
#include "../Common/elaptime.h"

class CAirportDatabase ;
class CAirsideFlightFuelBurnParmater;
class CAirsideFlightFuelBurnReportResult;
class CFlightFuelBurnPerformancejudgment
{
public:
	CFlightFuelBurnPerformancejudgment() {} ;
	virtual ~CFlightFuelBurnPerformancejudgment() {} ;
protected:
	std::vector<AirsideMobileElementMode> m_ModeData ;
public:
	BOOL IsAtThisArea(int _mobileElementMode) 
	{
		return std::find(m_ModeData.begin(),m_ModeData.end(),(AirsideMobileElementMode)_mobileElementMode) != m_ModeData.end() ;
	}
};
//flight crusie state
class CFlightCrusieFuelBurnState : public CFlightFuelBurnPerformancejudgment
{
public:
	CFlightCrusieFuelBurnState():CFlightFuelBurnPerformancejudgment()
		{
			m_ModeData.push_back(OnCruiseThrough) ;
			m_ModeData.push_back(OnCruiseArr) ;
			m_ModeData.push_back(OnCruiseDep) ;
		}
};
//flight terminal
class CFlightTerminalFuelBurnState : public CFlightFuelBurnPerformancejudgment
{
public:
CFlightTerminalFuelBurnState():CFlightFuelBurnPerformancejudgment()
							 {
								 m_ModeData.push_back(OnTerminal) ;
							 }

};
//flight approach to land
class CFlightApproachToLandFuelBurnState : public CFlightFuelBurnPerformancejudgment
{
public:
CFlightApproachToLandFuelBurnState():CFlightFuelBurnPerformancejudgment()
								   {
									   m_ModeData.push_back(OnApproach);
									   m_ModeData.push_back(OnFinalApproach);
								   }
};
//flight landing
class CFlightLandingFuelBurnState : public CFlightFuelBurnPerformancejudgment
{
public:	
	CFlightLandingFuelBurnState():CFlightFuelBurnPerformancejudgment()
		{
			m_ModeData.push_back(OnLanding);
			m_ModeData.push_back(OnLASHold);
			m_ModeData.push_back(OnExitRunway) ;
		}
};
//taxi inbound
class CFlightTaxiInBound : public CFlightFuelBurnPerformancejudgment
{
public:
	CFlightTaxiInBound():CFlightFuelBurnPerformancejudgment()
		{
					m_ModeData.push_back(OnTaxiToTempParking) ;
					   m_ModeData.push_back(OnEnterTempStand) ;
					   m_ModeData.push_back(OnEnterTempParking) ;
					   m_ModeData.push_back(OnHeldAtTempParking) ;
					   m_ModeData.push_back(OnExitTempStand);
					   m_ModeData.push_back(OnTaxiToStand) ;
		 }
};
//stand server
class CFlightStandServerFuelBurnState : public CFlightFuelBurnPerformancejudgment
{
public:
		CFlightStandServerFuelBurnState():CFlightFuelBurnPerformancejudgment()
		{
			m_ModeData.push_back(OnEnterStand) ;
			m_ModeData.push_back(OnHeldAtStand) ;
			m_ModeData.push_back(OnExitStand) ;
		}
};
//pushback/tow
class CFlightPushBackTowFuelBurnState : public CFlightFuelBurnPerformancejudgment
{
public:
	CFlightPushBackTowFuelBurnState():CFlightFuelBurnPerformancejudgment()
	{
		m_ModeData.push_back(OnTowToDestination) ;
	}
};
//taxi outbound
class CFlightTaxiOutboundFuelBurnState :public CFlightFuelBurnPerformancejudgment
{
public:
	CFlightTaxiOutboundFuelBurnState():CFlightFuelBurnPerformancejudgment()
	{
		m_ModeData.push_back(OnTaxiToRunway) ;
		m_ModeData.push_back(OnQueueToRunway) ;
		m_ModeData.push_back(OnTakeOffWaitEnterRunway) ;
	}
};
//take off
class CFlightTakeoffFuelBurnState : public CFlightFuelBurnPerformancejudgment
{
public:
	CFlightTakeoffFuelBurnState():CFlightFuelBurnPerformancejudgment()
	{
		m_ModeData.push_back(OnTakeOffEnterRunway) ;
		m_ModeData.push_back(OnPreTakeoff);
		m_ModeData.push_back(OnTakeoff) ;
	} 
};
//climbout
class CFlightClimoutFuelBurnState : public CFlightFuelBurnPerformancejudgment
{

public:
	CFlightClimoutFuelBurnState():CFlightFuelBurnPerformancejudgment()
	{
		m_ModeData.push_back(OnClimbout) ;
	}
};
class CFlightFuelBurnReportItem;
class CFlightFuelBurningCalculator
{
public:
	enum FlightFuelState {STATE_UNKNOW ,STATE_crusie,STATE_terminal,STATE_Approachtoland,STATE_landing,STATE_inbound,STATE_server,STATE_Pushback_tow,STATE_outbound,STATE_takeoff,STATE_climbout};
public:
	double CalculateFuelBurnByEvenFlightLog(  AirsideFlightLogEntry& logEntry ,int _fromIndx,int _toIndex ,AirsideFlightDescStruct& _flttype,CFlightFuelBurnReportItem& _reportItem) ;
	double GetFuelBurnAtEachStateByFlightType(FlightFuelState _state,AirsideFlightDescStruct& _fltType) ;
protected:
	FlightFuelState JuedgeWhichStateBelongTo(AirsideMobileElementMode _mode) ;
protected:
	CAirportDatabase* m_AirportDB ;
	int m_proID ;
public:
	CFlightFuelBurningCalculator(CAirportDatabase* _airportDB,int _proid) ;
};

class CFlightFuelBurnReportItem
{
public:
	CString m_Airline ;
	CString m_ArrivalID ;
	CString m_DepID ;
	CString m_Actype ;
	ElapsedTime m_ArrFromTime ;
	ElapsedTime m_ArrToTime ;
	ElapsedTime m_ArrDuration ;
	ElapsedTime m_DepFromTime ;
	ElapsedTime m_DepToTime ;
	ElapsedTime m_DepDuration ;
	double m_FuleBurnDep ;
	double m_FuelBurnArrival ;
	AirsideFlightDescStruct	m_fltDesc ;

	CFlightFuelBurnReportItem()
	{
		 m_FuleBurnDep = 0;
		 m_FuelBurnArrival = 0;
	}
	void ExportFile(ArctermFile& _file)
	{
		TCHAR Th[1024] = {0} ;
		strcpy(Th,m_Airline) ;
		_file.writeField(Th) ;

		strcpy(Th,m_ArrivalID) ;
		_file.writeField(Th) ;

		strcpy(Th,m_DepID) ;
		_file.writeField(Th) ;

		strcpy(Th,m_Actype) ;
		_file.writeField(Th) ;

		_file.writeTime( m_ArrFromTime )  ;
		_file.writeTime(  m_ArrToTime );
		_file.writeTime( m_ArrDuration );
		_file.writeTime( m_DepFromTime );
		_file.writeTime( m_DepToTime );
		_file.writeTime( m_DepDuration );
		_file.writeDouble( m_FuleBurnDep)  ;
		_file.writeDouble( m_FuelBurnArrival );
		_file.writeLine();
		m_fltDesc.ExportFile(_file) ;
	}
	void ImportFile(ArctermFile& _file)
	{
		TCHAR th[1024] = {0} ;
		_file.getField(th,1024) ;
		m_Airline.Format(_T("%s"),th) ;

		_file.getField(th,1024) ;
		m_ArrivalID.Format(_T("%s"),th) ;

		_file.getField(th,1024) ;
		m_DepID.Format(_T("%s"),th) ;

		_file.getField(th,1024) ;
		m_Actype.Format(_T("%s"),th) ;

		_file.getTime( m_ArrFromTime )  ;
		_file.getTime(  m_ArrToTime );
		_file.getTime( m_ArrDuration );
		_file.getTime( m_DepFromTime );
		_file.getTime( m_DepToTime );
		_file.getTime( m_DepDuration );
		_file.getFloat( m_FuleBurnDep)  ;
		_file.getFloat( m_FuelBurnArrival );
		_file.getLine() ;
		m_fltDesc.ImportFile(_file) ;
		m_fltDesc.dAprchSpeed = 0 ;
		_file.getLine() ;
	}
};
class CFlightDetailFuelBurnReportItem
{
public:
	CString m_Airline ;
	CString m_FlightID ;
	CString m_Actype ;
	CString m_Operate ;
	ElapsedTime m_fromTime ;
	ElapsedTime m_toTime ;
	ElapsedTime m_Duration ;
	double m_FuelBurn ;

	CFlightDetailFuelBurnReportItem()
	{
		m_FuelBurn =0 ;
	}
	void ExportFile(ArctermFile& _file) 
	{
		TCHAR Th[1024] = {0} ;
		strcpy(Th,m_Airline) ;
		_file.writeField(Th) ;

		strcpy(Th,m_FlightID) ;
		_file.writeField(Th) ;

		strcpy(Th,m_Actype) ;
		_file.writeField(Th) ;

		strcpy(Th,m_Operate) ;
		_file.writeField(Th) ;

		_file.writeTime(m_fromTime) ;
		_file.writeTime(m_toTime) ;
		_file.writeTime(m_Duration) ;
		_file.writeDouble(m_FuelBurn) ;
		_file.writeLine() ;
	}
	void ImportFile(ArctermFile& _file)
	{
		_file.getField(m_Airline.GetBuffer(),1024) ;
		_file.getField(m_FlightID.GetBuffer(),1024) ;
		_file.getField(m_Actype.GetBuffer(),1024) ;
		_file.getField(m_Operate.GetBuffer(),1024) ;
		_file.getTime(m_fromTime) ;
		_file.getTime(m_toTime) ;
		_file.getTime(m_Duration) ;
		_file.getFloat(m_FuelBurn) ;
		_file.getLine() ;
	}
};
class CFlightSummaryFuelBurnReportItem
{
public:
	FlightConstraint m_FlightType ;
	int m_NumOfAircraft ;
	double m_TotalFuelBurn ;
	double m_minFuelBurn ;
	double m_MaxFuelBurn ;
	double m_AvgFuelBurn ;
	double m_Q1 ;
	double m_Q2 ;
	double m_Q3 ;
	double m_P1 ;
	double m_P5 ;
	double m_p10 ;
	double m_p90 ;
	double m_P95 ;
	double m_P99 ;
	CFlightSummaryFuelBurnReportItem()
	{
		 m_NumOfAircraft = 0;
		 m_TotalFuelBurn = 0 ;
		 m_minFuelBurn = 0 ;
		 m_MaxFuelBurn = 0 ;
		 m_AvgFuelBurn = 0 ;
		 m_Q1  = 0;
		 m_Q2  = 0;
		 m_Q3 = 0 ;
		 m_P1 = 0 ;
		 m_P5  = 0;
		 m_p10 = 0 ;
		 m_p90 = 0 ;
		 m_P95 = 0 ;
		 m_P99 = 0 ;
	}
	void ExportFile(ArctermFile& _file)
	{
		m_FlightType.writeConstraint(_file) ;
		_file.writeInt( m_NumOfAircraft );
		_file.writeDouble( m_TotalFuelBurn );
		_file.writeDouble( m_minFuelBurn );
		_file.writeDouble( m_MaxFuelBurn );
		_file.writeDouble( m_AvgFuelBurn );
		_file.writeDouble( m_Q1 );
		_file.writeDouble( m_Q2 );
		_file.writeDouble( m_Q3 );
		_file.writeDouble( m_P1 );
		_file.writeDouble( m_P5 );
		_file.writeDouble( m_p10 );
		_file.writeDouble( m_p90 );
		_file.writeDouble( m_P95 );
		_file.writeDouble( m_P99 );
		_file.writeLine() ;
	}
	void ImportFile(ArctermFile& _file)
	{
		m_FlightType.readConstraint(_file) ;
		_file.getInteger( m_NumOfAircraft );
		_file.getFloat( m_TotalFuelBurn );
		_file.getFloat( m_minFuelBurn );
		_file.getFloat( m_MaxFuelBurn );
		_file.getFloat( m_AvgFuelBurn );
		_file.getFloat( m_Q1 );
		_file.getFloat( m_Q2 );
		_file.getFloat( m_Q3 );
		_file.getFloat( m_P1 );
		_file.getFloat( m_P5 );
		_file.getFloat( m_p10 );
		_file.getFloat( m_p90 );
		_file.getFloat( m_P95 );
		_file.getFloat( m_P99 );
		_file.getLine() ;
	}
};
class CAirsideFlightFuelBurnReport :
	public CAirsideBaseReport
{
	class CDataSummary
	{
	public:
		FlightConstraint m_FltType ;
		std::vector<CFlightFuelBurnReportItem> m_Data ;
	};
public:
	CAirsideFlightFuelBurnReport(CBGetLogFilePath pFunc,CAirsideFlightFuelBurnParmater* _parameter,CAirportDatabase* _airportDB,int _proid):m_BaseResult(NULL),m_airportDB(_airportDB),m_proid(_proid),m_Parameter(_parameter),CAirsideBaseReport(pFunc){};
	~CAirsideFlightFuelBurnReport(void);
protected:
	CAirsideFlightFuelBurnParmater* m_Parameter ;
	CAirportDatabase* m_airportDB ;
	int m_proid;
	std::vector<CFlightFuelBurnReportItem> m_ReportItemData ;
	std::vector<CFlightDetailFuelBurnReportItem> m_DetailData ;
	std::vector<CFlightSummaryFuelBurnReportItem> m_SummaryData ;
	std::vector<CDataSummary*> m_SummaryMiddleData ;
	CAirsideFlightFuelBurnReportResult* m_BaseResult ;
public:
	CAirsideFlightFuelBurnReportResult* GetAirsideReportResult() { return m_BaseResult ;} ;
public:
	void GenerateReport(CParameters * parameter) ;
	void RefreshReport(CParameters * parameter);

	int GetReportType() { return Airside_FlightFuelBurning ;};

	void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);

	void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
protected:
	bool IsFitConstraint(AirsideFlightDescStruct& fltDesc,CParameters * parameter);
	bool IsValidTime(AirsideFlightDescStruct& fltDesc,CParameters * parameter);
protected:
	BOOL GetFitFlightEventLogByStartAndEndPostion(AirsideFlightLogEntry& logEntry,int& _startPos ,int& _endPos ) ;

	void GenerateDetailReport() ;
	void GenerateSummaryReport() ;
	void FillDetailListContent (CXListCtrl& cxListCtrl);
	void FillSummaryListContent(CXListCtrl& cxListCtrl);
	double GetFuelBurnByCurrentUnitWeight(double _fuelBurn);
	void CalculateSummaryData(CDataSummary* PDataSummary);
	void InitDetailListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);
	void InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);
public:
public:
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail)  ;
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail)  ;

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

	CString GetReportFileName();
protected:
	void ClearAllData() ;
};
