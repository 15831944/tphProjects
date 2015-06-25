#pragma once
#include "AirsideBaseReport.h"
#include <vector>
#include "../Common/AIRSIDE_BIN.h"
#include "../Results/baselog.h"
#include "../Results/AirsideVehicleLog.h"
#include "../Results/AirsideVehicleLogEntry.h"
#include <algorithm>
#include "AirsideVehicleOpeDetailReportRes.h"
#include "AirsideVehicleOpeSummaryReportRes.h"
#include "../InputAirside/VehicleSpecificationItem.h"
#include "AirsideVehicleOperParameter.h"
class CVehicleOperaterDetailReportItem
{
public:
	int m_vehicleID ;
	CString m_VehicleType ;
	ElapsedTime m_StartServerTime ;
	ElapsedTime m_EndServerTime ;
	long m_TotalOnRouteTime ;
	long m_TotalIdelTime ;
	long m_ServerTime ;
	int  m_ServerNumber ;
	int m_NumberToPool ;
	double m_TotalDistance ;
	long m_FuelConsumed ;
	float m_AvgSpeed ;
	float m_MaxSpeed  ;
	std::vector<long> m_ServerSTimes ;
	void ExportFile(ArctermFile& _file)
	{
		_file.writeInt(m_vehicleID) ;
		_file.writeField(m_VehicleType) ;
		_file.writeTime(m_StartServerTime) ;
		_file.writeTime(m_EndServerTime) ;
		_file.writeInt(m_TotalOnRouteTime);
		_file.writeInt(m_TotalIdelTime);
		_file.writeInt(m_ServerTime);
		_file.writeInt(m_ServerNumber);
		_file.writeInt(m_NumberToPool);
		_file.writeDouble(m_TotalDistance);
		_file.writeInt(m_FuelConsumed);
		_file.writeDouble(m_AvgSpeed);
		_file.writeDouble(m_MaxSpeed);
		_file.writeLine() ;
		int size = m_ServerSTimes.size() ;
		_file.writeInt(size);
		_file.writeLine() ;
        for (int i = 0 ; i < (int)m_ServerSTimes.size() ;i++)
        {
			 _file.writeInt(m_ServerSTimes[i]);
        }
		_file.writeLine() ;
	}
	void ImportFile(ArctermFile& _file)
	{
		TCHAR  Name[1024] = {0} ; 
		_file.getInteger(m_vehicleID) ;

		_file.getField(Name,1024) ;
		m_VehicleType.Format(_T("%s"),Name) ;

		_file.getTime(m_StartServerTime) ;
		_file.getTime(m_EndServerTime) ;
		_file.getInteger(m_TotalOnRouteTime);
		_file.getInteger(m_TotalIdelTime);
		_file.getInteger(m_ServerTime);
		_file.getInteger(m_ServerNumber);
		_file.getInteger(m_NumberToPool);
		_file.getFloat(m_TotalDistance);
		_file.getInteger(m_FuelConsumed);
		_file.getFloat(m_AvgSpeed);
		_file.getFloat(m_MaxSpeed);
		_file.getLine() ;
		int size = 0 ;
		_file.getInteger(size);
		_file.getLine() ;
		long val = 0 ;
		for (int i = 0 ; i < size;i++)
		{
			_file.getInteger(val);
			m_ServerSTimes.push_back(val) ;
		}
	}
	CVehicleOperaterDetailReportItem()
	{
		 m_vehicleID = 0;
		 m_TotalOnRouteTime = 0;
		 m_TotalIdelTime = 0;
		 m_ServerTime = 0;
		  m_ServerNumber = 0;
		 m_NumberToPool = 0;
		 m_TotalDistance = 0;
		 m_FuelConsumed = 0;
		 m_AvgSpeed = 0 ;
		 m_MaxSpeed = 0 ;
	}
	CVehicleOperaterDetailReportItem(const CVehicleOperaterDetailReportItem& _data)
	{
		 m_vehicleID = _data.m_vehicleID;
		 m_VehicleType = _data.m_VehicleType;
		 m_StartServerTime = _data.m_StartServerTime;
		 m_EndServerTime = _data.m_EndServerTime;
		 m_TotalOnRouteTime = _data.m_TotalOnRouteTime;
		 m_TotalIdelTime = _data.m_TotalIdelTime;
		 m_ServerTime = _data.m_ServerTime;
		  m_ServerNumber = _data.m_ServerNumber;
		 m_NumberToPool = _data.m_NumberToPool;
		 m_TotalDistance = _data.m_TotalDistance;
		 m_FuelConsumed = _data.m_FuelConsumed;
		 m_AvgSpeed = _data.m_AvgSpeed;
		 m_MaxSpeed  = _data.m_MaxSpeed;
		 m_ServerSTimes.clear() ;
		 for (int i = 0 ; i < (int)_data.m_ServerSTimes.size() ; i++)
		 {
			 m_ServerSTimes.push_back(_data.m_ServerSTimes[i]) ;
		 }
	}
	CVehicleOperaterDetailReportItem& operator = (const CVehicleOperaterDetailReportItem& _data)
	{ 
		m_vehicleID = _data.m_vehicleID;
		m_VehicleType = _data.m_VehicleType;
		m_StartServerTime = _data.m_StartServerTime;
		m_EndServerTime = _data.m_EndServerTime;
		m_TotalOnRouteTime = _data.m_TotalOnRouteTime;
		m_TotalIdelTime = _data.m_TotalIdelTime;
		m_ServerTime = _data.m_ServerTime;
		m_ServerNumber = _data.m_ServerNumber;
		m_NumberToPool = _data.m_NumberToPool;
		m_TotalDistance = _data.m_TotalDistance;
		m_FuelConsumed = _data.m_FuelConsumed;
		m_AvgSpeed = _data.m_AvgSpeed;
		m_MaxSpeed  = _data.m_MaxSpeed;
		m_ServerSTimes.clear() ;
		for (int i = 0 ; i < (int)_data.m_ServerSTimes.size() ; i++)
		{
			m_ServerSTimes.push_back(_data.m_ServerSTimes[i]) ;
		}
			return *this ;
		}
};
class CVehicleOperaterSummaryReportSubItem
{
public:
	ElapsedTime m_StartTime ;
	ElapsedTime m_EndTime ;
	long  m_OnRoadTime ;
	float m_speed ;
	long m_serverTime ;
	double m_Distance ;
	float m_Fuel;
public:
	CVehicleOperaterSummaryReportSubItem():m_OnRoadTime(0),m_speed(0),m_serverTime(0),m_Distance(0),m_Fuel(0){} ;
};
class CVehicleOperaterSummaryReportItem
{
public:
	std::vector<CVehicleOperaterSummaryReportSubItem*> m_SubItem ;
	int m_vehicleType ;
	CVehicleOperaterSummaryReportItem():m_vehicleType(0){} ;
	~CVehicleOperaterSummaryReportItem()
	{
		for (int i= 0 ; i < (int)m_SubItem.size() ;i++)
		{
			delete m_SubItem[i] ;
		}
		m_SubItem.clear() ;
	}
	CVehicleOperaterSummaryReportItem(const CVehicleOperaterSummaryReportItem& _data)
	{
		m_SubItem.clear() ;
		for (int i = 0 ; i < (int)_data.m_SubItem.size() ;i++)
		{
			m_SubItem.push_back(_data.m_SubItem[i]) ;
		}
		m_vehicleType = _data.m_vehicleType ;
	}
    CVehicleOperaterSummaryReportItem& operator = (const CVehicleOperaterSummaryReportItem& _data)
	{
		m_SubItem.clear() ;
		for (int i = 0 ; i < (int)_data.m_SubItem.size() ;i++)
		{
			m_SubItem.push_back(_data.m_SubItem[i]) ;
		}
		m_vehicleType = _data.m_vehicleType ;	
		return *this ;
	}
};
class CVehicleEventLogAnalysis 
{
protected:
	class  CServerIndex  
	{
	public:
		int m_moveToServerIndex  ;
		int m_BeginServerIndex  ;
		int m_endServerIndex ;
        int m_OnRoadEndindex ; 
		BOOL m_ReturnPool ;
	public:
		CServerIndex():m_moveToServerIndex(-1),m_BeginServerIndex(-1),m_endServerIndex(-1),m_OnRoadEndindex(-1),m_ReturnPool(TRUE){} ;
	};

public:
		typedef EventLog<AirsideVehicleEventStruct> EVENLOG_TY ;
		CVehicleEventLogAnalysis( AirsideVehicleLogEntry* _vehicleEntry ,CAirsideVehicleOperParameter *pParam) ;
		~CVehicleEventLogAnalysis() {} ;

public:
	 virtual void AnalysisEventLog()  ;
public:
	void SetFlightLog(AirsideFlightLog* _fltLog ) { m_fltLog = _fltLog ;} ;
protected:
	virtual void AnalysisGeneralVehicle() ;
	virtual void AnalysisPaxVehicle() ;
	//---------------------------------------------------------------------------------
	//Summary:
	//		retrieve follow me car process information
	//---------------------------------------------------------------------------------
	virtual void AnalysisFollowmeCar();
protected:
	BOOL GetFlightDesStructByID(AirsideFlightDescStruct& destStruct , int _ID);
	AirsideFlightLog* m_fltLog ;
protected:
		AirsideVehicleDescStruct* m_descStruct ;
		std::vector<CServerIndex> m_ServerS ; 
		CAirsideVehicleOperParameter *m_pParam ;
		AirsideVehicleLogEntry* m_vehicleEntry ;
};


class CDetailAnalysis :public CVehicleEventLogAnalysis
{
protected:
	   virtual void CalculateStartTime()  ;
	   virtual void CalculateEndTime()  ;
	   virtual void CalculateTotalOnRoadTime() ;
	   virtual void CalculateIdelTime() ;
	   virtual void CalculateServerTime()  ;
	   virtual void CalculateNumberOfServer()  ;
	   virtual void CalculateNumberToPool()  ;
	   virtual void CalculateNumForGas()  ;
	   virtual void CalculateTotalDistance()  ;
	   virtual void CalculateFuelConsumed()  ;
	   virtual void CalculateMaxSpeed()  ;
	   virtual void CalculateAvgSpeed()  ;
	   void CalculateServerTimes();
protected:
	CVehicleOperaterDetailReportItem* m_DetailReportItem ;
public:
	   void AnalysisVehicleForDetail() ;
	   CDetailAnalysis(CVehicleOperaterDetailReportItem* _item , AirsideVehicleLogEntry* _vehicleEntry ,CAirsideVehicleOperParameter *pParam) ;
};
class CSummaryAnalysis : public CVehicleEventLogAnalysis
{
public:
	CSummaryAnalysis(CVehicleOperaterSummaryReportItem* _item  , AirsideVehicleLogEntry* _vehicleEntry ,CAirsideVehicleOperParameter *pParam) ;
	void AnalysisEventLog() ;
protected:
	int m_timeInterValNum ;
protected:
	void CalculateSpeed() ;
	void CalculateOnroadTime() ;
	void CalculateServerTime() ;
	void CalculateFuelConsumed() ;
	void CalculateDistance() ;
public:
	void SetIntervalNumber(int _num) { m_item->m_SubItem.resize(_num) ;};
protected:
	CVehicleOperaterSummaryReportItem* m_item ;
};
class CSummaryFinialResult 
{
public:
	ElapsedTime m_StartTime ;
	ElapsedTime m_EndTime ; 
	int m_VehicleType ;

	float m_MinSpeed ;
	float m_MaxSpeed ;
	float m_AvgSpeed ;
	float m_SpeedQ1 ;
	float m_SpeedQ2 ;
	float m_SpeedQ3 ;
	float m_SpeedP1 ;
	float m_SpeedP5 ;
	float m_SpeedP90 ;
	float m_SpeedP95 ;
	float m_SpeedP99 ;



	long m_MaxTimeInServer ;
	long m_MinTimeInServer ;
	long m_AvgTimeInServer ;
	long m_TimeInServerQ1 ;
	long m_TimeInServerQ2 ;
	long m_TimeInServerQ3 ;
	long m_TimeInServerP1 ;
	long m_TimeInServerP5 ;
	long m_TimeInServerP90 ;
	long m_TimeInServerP95 ;
	long m_TimeInServerP99 ;
	long m_totalTimeServer ;

	long m_MaxTimeInRoad ;
	long m_MinTimeInRoad ;
	long m_AvgTimeInRoad ;
	long m_TimeInRoadQ1 ;
	long m_TimeInRoadQ2 ;
	long m_TimeInRoadQ3 ;
	long m_TimeInRoadP1 ;
	long m_TimeInRoadP5 ;
	long m_TimeInRoadP90 ;
	long m_TimeInRoadP95 ;
	long m_TimeInRoadP99 ;
    long m_TotalInRoadTime ;

	double m_MinDis ;
	double m_MaxDis ;
	double m_AvgDis ;
	double m_DisQ1 ;
	double m_DisQ2 ;
	double m_DisQ3 ; 
	double m_DisP1 ;
	double m_Disp5 ;
	double m_DisP90 ;
	double m_DisP95 ;
	double m_DisP99 ;

	float m_MinFuel ;
	float m_MaxFuel ;
	float m_AvgFuel ;
	float m_FuelQ1 ;
	float m_FuelQ2 ;
	float m_FuelQ3 ;
	float m_FuelP1 ;
	float m_FuelP5 ;
	float m_FuelP90 ;
	float m_FuelP95 ;
	float m_FuelP99 ; 
    
	void ExportFile(ArctermFile& _file)
	{
		_file.writeTime(m_StartTime) ;
		_file.writeTime(m_EndTime) ;
		_file.writeInt(m_VehicleType) ;

		_file.writeFloat(m_MinSpeed );
		_file.writeFloat(m_MaxSpeed) ;
		_file.writeFloat( m_AvgSpeed) ;
		_file.writeFloat( m_SpeedQ1) ;
		_file.writeFloat( m_SpeedQ2) ;
		_file.writeFloat( m_SpeedQ3) ;
		_file.writeFloat( m_SpeedP1) ;
		_file.writeFloat( m_SpeedP5) ;
		_file.writeFloat( m_SpeedP90) ;
		_file.writeFloat( m_SpeedP95) ;
		_file.writeFloat( m_SpeedP99) ;


		_file.writeInt(m_MaxTimeInServer );
		_file.writeInt( m_MinTimeInServer) ;
		_file.writeInt( m_AvgTimeInServer) ;
		_file.writeInt( m_TimeInServerQ1) ;
		_file.writeInt( m_TimeInServerQ2) ;
		_file.writeInt( m_TimeInServerQ3) ;
		_file.writeInt( m_TimeInServerP1) ;
		_file.writeInt( m_TimeInServerP5) ;
		_file.writeInt( m_TimeInServerP90 );
		_file.writeInt( m_TimeInServerP95) ;
		_file.writeInt( m_TimeInServerP99 );
		_file.writeInt( m_totalTimeServer) ;

		_file.writeInt( m_MaxTimeInRoad) ;
		_file.writeInt( m_MinTimeInRoad) ;
		_file.writeInt( m_AvgTimeInRoad) ;
		_file.writeInt( m_TimeInRoadQ1) ;
		_file.writeInt( m_TimeInRoadQ2) ;
		_file.writeInt( m_TimeInRoadQ3) ;
		_file.writeInt( m_TimeInRoadP1) ;
		_file.writeInt( m_TimeInRoadP5 );
		_file.writeInt( m_TimeInRoadP90) ;
		_file.writeInt( m_TimeInRoadP95) ;
		_file.writeInt( m_TimeInRoadP99) ;
		_file.writeInt( m_TotalInRoadTime );


		_file.writeDouble( m_MinDis) ;
		_file.writeDouble( m_MaxDis) ;
		_file.writeDouble( m_AvgDis) ;
		_file.writeDouble( m_DisQ1) ;
		_file.writeDouble( m_DisQ2) ;
		_file.writeDouble( m_DisQ3 ); 
		_file.writeDouble( m_DisP1) ;
		_file.writeDouble( m_Disp5 );
		_file.writeDouble( m_DisP90) ;
		_file.writeDouble( m_DisP95 );
		_file.writeDouble( m_DisP99) ;

		_file.writeFloat( m_MinFuel) ;
		_file.writeFloat( m_MaxFuel );
		_file.writeFloat( m_AvgFuel) ;
		_file.writeFloat( m_FuelQ1 );
		_file.writeFloat( m_FuelQ2) ;
		_file.writeFloat( m_FuelQ3) ;
		_file.writeFloat( m_FuelP1 );
		_file.writeFloat( m_FuelP5) ;
		_file.writeFloat( m_FuelP90 );
		_file.writeFloat( m_FuelP95 );
		_file.writeFloat( m_FuelP99 ); 
		_file.writeLine() ;
	}
	void ImportFile(ArctermFile& _file)
	{
		_file.getTime(m_StartTime) ;
		_file.getTime(m_EndTime) ;
		_file.getInteger(m_VehicleType) ;

		_file.getFloat(m_MinSpeed );
		_file.getFloat(m_MaxSpeed) ;
		_file.getFloat( m_AvgSpeed) ;
		_file.getFloat( m_SpeedQ1) ;
		_file.getFloat( m_SpeedQ2) ;
		_file.getFloat( m_SpeedQ3) ;
		_file.getFloat( m_SpeedP1) ;
		_file.getFloat( m_SpeedP5) ;
		_file.getFloat( m_SpeedP90) ;
		_file.getFloat( m_SpeedP95) ;
		_file.getFloat( m_SpeedP99) ;


		_file.getInteger(m_MaxTimeInServer );
		_file.getInteger( m_MinTimeInServer) ;
		_file.getInteger( m_AvgTimeInServer) ;
		_file.getInteger( m_TimeInServerQ1) ;
		_file.getInteger( m_TimeInServerQ2) ;
		_file.getInteger( m_TimeInServerQ3) ;
		_file.getInteger( m_TimeInServerP1) ;
		_file.getInteger( m_TimeInServerP5) ;
		_file.getInteger( m_TimeInServerP90 );
		_file.getInteger( m_TimeInServerP95) ;
		_file.getInteger( m_TimeInServerP99 );
		_file.getInteger( m_totalTimeServer) ;

		_file.getInteger( m_MaxTimeInRoad) ;
		_file.getInteger( m_MinTimeInRoad) ;
		_file.getInteger( m_AvgTimeInRoad) ;
		_file.getInteger( m_TimeInRoadQ1) ;
		_file.getInteger( m_TimeInRoadQ2) ;
		_file.getInteger( m_TimeInRoadQ3) ;
		_file.getInteger( m_TimeInRoadP1) ;
		_file.getInteger( m_TimeInRoadP5 );
		_file.getInteger( m_TimeInRoadP90) ;
		_file.getInteger( m_TimeInRoadP95) ;
		_file.getInteger( m_TimeInRoadP99) ;
		_file.getInteger( m_TotalInRoadTime );


		_file.getFloat( m_MinDis) ;
		_file.getFloat( m_MaxDis) ;
		_file.getFloat( m_AvgDis) ;
		_file.getFloat( m_DisQ1) ;
		_file.getFloat( m_DisQ2) ;
		_file.getFloat( m_DisQ3 ); 
		_file.getFloat( m_DisP1) ;
		_file.getFloat( m_Disp5 );
		_file.getFloat( m_DisP90) ;
		_file.getFloat( m_DisP95 );
		_file.getFloat( m_DisP99) ;

		_file.getFloat( m_MinFuel) ;
		_file.getFloat( m_MaxFuel );
		_file.getFloat( m_AvgFuel) ;
		_file.getFloat( m_FuelQ1 );
		_file.getFloat( m_FuelQ2) ;
		_file.getFloat( m_FuelQ3) ;
		_file.getFloat( m_FuelP1 );
		_file.getFloat( m_FuelP5) ;
		_file.getFloat( m_FuelP90 );
		_file.getFloat( m_FuelP95 );
		_file.getFloat( m_FuelP99 ); 
	}
};

class CAirsideVehicleOperaterReport :
	public CAirsideBaseReport
{

public:
	CAirsideVehicleOperaterReport(CBGetLogFilePath pFunc,CAirsideVehicleOperParameter* _param);
	~CAirsideVehicleOperaterReport(void);
public:
	 void GenerateReport(CParameters * parameter) ;
	 void RefreshReport(CParameters * parameter);

	 int GetReportType() { return Airside_VehicleOperation ;};

	  void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);

	  void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
protected:
	void InitDetailListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL) ;
	void InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL) ;
protected:
	EventLog< AirsideVehicleEventStruct>  m_AirsideVehicleEventLog ;
	int m_IntervalSize  ;
	AirsideFlightLog m_fltLog; 
	CAirsideVehicleOpeReportResult* m_BaseResult ;
	AirsideVehicleLogEntry logEntry ;  
protected:
	std::vector<CVehicleOperaterDetailReportItem> m_DetailResult ;
	std::vector<CVehicleOperaterSummaryReportItem*> m_SummaryResult ;
	std::vector<CSummaryFinialResult> m_FinalResultSummary ;
	void GenerateDetailReport(CAirsideVehicleOperParameter *pParam ) ;
	void AnalysisDetailReportItem(AirsideVehicleDescStruct& _VehicleDesStruct,CAirsideVehicleOperParameter *pParam ) ;

	void GenerateSummaryReport(CAirsideVehicleOperParameter *pParam ) ;
	void AnalysisSummaryReportItem(AirsideVehicleDescStruct& _VehicleDesStruct,CAirsideVehicleOperParameter *pParam ) ;
	void CalSummaryInterval(CAirsideVehicleOperParameter *pParam );

	void InitDetaliList(CXListCtrl& cxListCtrl) ;
	void InitSummaryList(CXListCtrl& cxListCtrl) ;
	void AnalysisReslut(int Type , std::vector<CVehicleOperaterSummaryReportItem*>& _data) ;

public:
    enum subReportType
	{
		DETAIL_ONROAD_TIME ,
		DETAIL_IDEL_TIME ,
		DETAIL_TOTALTIME_INSERVER ,
		DETAIL_NUMBER_SERVER,
		DETAIL_NUMBER_TOPOOL ,
		DETAIL_NUMBER_FORGAS ,
		DETAIL_TIME_INLOTS ,
		DETAIL_FUEL_CONSUMED,
		DETAIL_AVG_SPEED ,
		DETAIL_MAX_SPEED ,
		DETAIL_VEHICLE_SERVICE , 

		SUMMARY_ON_ROAD ,
		SUMMARY_SPEED ,
		SUMMARY_IN_SERVER ,
		SUMMARY_FUEL ,
		SUMMARY_DISTANCE ,
		SUMMARY_UTILIZATION ,
	};
	CAirsideReportBaseResult* GetReportResult() { return m_BaseResult ;};
public:
	static CString GetVehicleTypeNameById(int _vehicleTy) {  return CVehicleSpecificationItem::GetVehicleTypeByID(_vehicleTy) ;};
	static int GetVehicleIDByName(CString& _vehicleTy) { return CVehicleSpecificationItem::GetVehicleIDByVehicleName(_vehicleTy) ;} ;
public:
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail)  ;
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail)  ;
protected:
	BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;
	void ExportListSummaryData(ArctermFile& _file,CParameters * parameter);
	void ExportListDetailData(ArctermFile& _file,CParameters * parameter);

	CAirsideVehicleOperParameter *m_pParam ;
public:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

	CString GetReportFileName();
};
