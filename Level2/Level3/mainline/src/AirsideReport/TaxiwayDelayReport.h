#pragma once
#include "airsidebasereport.h"
#include <vector>
#include <algorithm>
#include "TaxiwayDelayParameters.h"
#include "FlightConflictReportData.h"
class CTaxiwayDelayReportResult ;
#include "../Results/AirsideFlightLogItem.h"
#include "../Results/ARCBaseLog.h"
#include "LogFilterFun.h"

typedef FlightConflictReportData::ConflictDataItem  CONFIICTREPORTDATA_ITEM ;

class CFunSortByDelayTime
{
public:
	bool operator() (const CONFIICTREPORTDATA_ITEM* _first ,const CONFIICTREPORTDATA_ITEM* _second )const
	{
		return _first->m_tTime > _second->m_tTime ;
	}
};
//typedef std::map<CONFIICTREPORTDATA_ITEM*,ElapsedTime,CFunSortByDelayTime> TY_TAXIWAY ;
//typedef typedef std::vector<CONFIICTREPORTDATA_ITEM*>::iterator TY_TAXIWAY_MAP_ITER ;
typedef std::pair<ElapsedTime, ElapsedTime> TAXIWYSEGTIME;

typedef struct  
{
	ElapsedTime tEnter;
	ElapsedTime tLeave;
	CString strEnterNode;
	CString strLeaveNode;

}TAXIWAYSEGINFO;

////////////////////////////////////////////////////////////////////////////////////
class ConflictDataItem ;
class CTaxiwayFligthDelayItem  //one flight delay
{
protected:
	CString m_FlightID ;
	int m_FlightUniqeID ;
	BOOL m_IsDep ;
	std::vector<CONFIICTREPORTDATA_ITEM*> m_TaxiwayDelayIndex ;
public:
	CTaxiwayFligthDelayItem(){} ;
	~CTaxiwayFligthDelayItem() {};
public:
	void SetFlightUniqeID(int _id) { m_FlightUniqeID = _id ;} ;
	int GetFlightUniqeID() { return m_FlightUniqeID ;} ;
	void SetFlightID(CString _id) { m_FlightID = _id ;} ;
	CString GetFlightID() { return m_FlightID ;} ;

	void AddDelayLogData(CONFIICTREPORTDATA_ITEM* _conflictItem) ;

	//void GetDelaylogDatabyIntervalTime(ElapsedTime _start , ElapsedTime _end , std::vector<CONFIICTREPORTDATA_ITEM*>& _logData) ;

	std::vector<CONFIICTREPORTDATA_ITEM*>* GetDelayData() { return &m_TaxiwayDelayIndex ;} ;

	void IsDeparture(BOOL Res) { m_IsDep = Res ;} ;
	BOOL IsDeparture() {return m_IsDep ;} ;
	void GetTaxiwayDelayTimeByIntervalTime(ElapsedTime _start , ElapsedTime _end , std::vector<CONFIICTREPORTDATA_ITEM*>& _lastDelay);
};

/////////////////////////////////////////////////////////////////////////////////
class CTaxiwaySegmentItem  //each segment has many flight delay ,
{
protected:
	//int m_FromNodeID ;
	//int m_ToNodeID ;
	//CString m_strFromName ;
	//CString m_strToName ;
	CReportTaxiwaySegment m_ReportSegment;
	std::vector<CTaxiwayFligthDelayItem*> m_FlightDelay ;
public:
	void ClearData()
	{
		for (int i = 0 ; i < (int)m_FlightDelay.size() ;i++)
		{
			delete m_FlightDelay[i] ;
		}
		m_FlightDelay.clear() ;
	}
	//void SetFromIntersectionID(int _id) { m_FromNodeID = _id ;} ;
	//int GetFromIntersectionID() { return m_FromNodeID ;} ;

	//void SetToInsectionID(int _id) { m_ToNodeID  = _id ;} ;
	//int GetToInsectionID() { return m_ToNodeID ;} ;

	//void SetFromInsectionName(const CString& _name) { m_strFromName = _name ;} ;
	//CString GetFromInsectionName() { return m_strFromName ;} ;

	//void SetToInsectionName(const CString& _name) { m_strToName = _name ;} ;
	//CString GetToInsectionName() { return m_strToName ;} ;

	const CReportTaxiwaySegment& GetReportSegment(){ return m_ReportSegment; }

	void AddDelayLogData(FlightConflictReportData::ConflictDataItem* _conflictItem) ;
public:
	CTaxiwaySegmentItem(const CReportTaxiwaySegment& reportSeg); 
	~CTaxiwaySegmentItem();
protected:
	CTaxiwayFligthDelayItem*  FindFlightDelayItemByFlightID(int _flightId) ;
public:
	CTaxiwayFligthDelayItem* AddFlightDelayItem(int _flightID) ;

	std::vector<CTaxiwayFligthDelayItem*>* GetFlightDelayData() { return &m_FlightDelay ;} ;
};

//////////////////////////////////////////////////////////////////////////////////////////
class CTaxiwayDelayReportDataItem  //each taxi way has many segment 
{
protected:
	int m_Taxiway ;
	CString m_Taxiwayname ;
	std::vector<CTaxiwaySegmentItem*> m_SegmentData ;
public:
	void SetTaxiwayId(int _id) { m_Taxiway = _id ;} ;
	int GetTaxiwayID() { return m_Taxiway ;} ;

	void SetTaxiwayName(const CString& _Name) { m_Taxiwayname = _Name ;} ;
	CString GetTaxiwayName() { return m_Taxiwayname ;} ;

	std::vector<CTaxiwaySegmentItem*>* GetSegmentData() { return &m_SegmentData ;} ;
	void ClearData() 
	{
		for (int i = 0 ; i < (int)m_SegmentData.size() ;i++)
		{
			delete m_SegmentData[i] ;
		}
		m_SegmentData.clear() ;
	}
public:
	CTaxiwayDelayReportDataItem(int _taxiID):m_Taxiway(_taxiID){}  ;
	~CTaxiwayDelayReportDataItem() {  ClearData() ;};
public:

	void InitIntersectionSegment(CTaxiwaySelect* _TaxiwaySelect) ;

	void AddDelayLogData(FlightConflictReportData::ConflictDataItem* _conflictItem) ;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class CTaxiwayDelayReportData   // has many taxiway 
{
protected:
	std::vector<CTaxiwayDelayReportDataItem*> m_logData ;
public:
	void ClearData() 
	{
		for (int i = 0 ; i < (int)m_logData.size() ;i++)
		{
			delete m_logData[i] ;
		}
		m_logData.clear() ;
	}
public:
	CTaxiwayDelayReportData(CTaxiwayDelayParameters* _parameter) ;
	~CTaxiwayDelayReportData() { ClearData() ;};
	std::vector<CTaxiwayDelayReportDataItem*>* GetDataSet() { return &m_logData ;} ;
public:
	CTaxiwayDelayReportDataItem* AddTaxiwayItem(int _taxiID) ;
public:
	void AddDelayLogData(FlightConflictReportData::ConflictDataItem* _conflictItem) ;
};

////////////////////////////////////////////////////////////////////////////////////////////////
class CTaxiwayDelayReportDetailDataItem
{
public:
	ElapsedTime m_startTime ;
	ElapsedTime m_EndTime ;
	CString m_TaxiwayName ;
	CString m_FromIntersection ;
	CString m_ToIntersection ;
	CString m_FlightID ;
	ElapsedTime m_EnterSegmentTime ;
	ElapsedTime m_ExistSegmentTime ;
	ElapsedTime m_UnInterruptTime ;
	ElapsedTime m_DelayTime ;
	BOOL m_IsDep ;
	int m_numberofdelay ;
	CString m_delay1Reason ; 
	ElapsedTime m_delay1 ;
	CString m_delay2Reason ;
	ElapsedTime m_delay2 ;
	CString m_delay3Reason ;
	ElapsedTime m_delay3 ;
public:
	CTaxiwayDelayReportDetailDataItem();
	void CalculateDelayTime(ElapsedTime _dataItem);
	void ExportFile(ArctermFile& _file)
	{
		_file.writeTime(m_startTime) ;
		_file.writeTime(m_EndTime) ;
		_file.writeField(m_TaxiwayName) ;
		_file.writeField(m_FromIntersection) ;
		_file.writeField(m_ToIntersection) ;
		_file.writeField(m_FlightID) ;
		_file.writeTime(m_EnterSegmentTime) ;
		_file.writeTime( m_ExistSegmentTime) ;
		_file.writeTime( m_UnInterruptTime) ;
		_file.writeTime( m_DelayTime) ;
		_file.writeInt(m_IsDep)  ;
		_file.writeInt(m_numberofdelay)  ;
		_file.writeField( m_delay1Reason) ; 
		_file.writeTime( m_delay1) ;
		_file.writeField( m_delay2Reason) ;
		_file.writeTime( m_delay2) ;
		_file.writeField( m_delay3Reason) ;
		_file.writeTime( m_delay3) ;
		_file.writeLine() ;
	}
	void ImportFile(ArctermFile& _file)
	{
		
		TCHAR val[1024] = {0} ;
		_file.getTime(m_startTime) ;
		_file.getTime(m_EndTime) ;
		_file.getField(val,1024) ;
		m_TaxiwayName = val ;

		_file.getField(val,1024) ;
		m_FromIntersection =val ;

		_file.getField(val,1024) ;
		m_ToIntersection = val ;
		_file.getField(val,1024) ;
		m_FlightID =val ;
		_file.getTime(m_EnterSegmentTime) ;
		_file.getTime( m_ExistSegmentTime) ;
		_file.getTime( m_UnInterruptTime) ;
		_file.getTime( m_DelayTime) ;
		_file.getInteger(m_IsDep)  ;
		_file.getInteger(m_numberofdelay)  ;

		_file.getField( val,1024) ; 
		m_delay1Reason = val ;
		_file.getTime( m_delay1) ;

		_file.getField( val,1024) ;
		m_delay2Reason = val ;
		_file.getTime( m_delay2) ;

		_file.getField( val,1024 ) ;
		m_delay3Reason = val ;
		_file.getTime( m_delay3) ;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////
class CTaxiwayDelayReportSummaryDataItem
{
public:
	CString m_Taxiway ;
	CString m_FromNode ;
	CString m_ToNode ;
	int m_numofaircraft ;
	ElapsedTime m_totalDelay;
	ElapsedTime m_MinDelay ;
	ElapsedTime m_MeanDelay ;
	ElapsedTime m_MaxDelay ;
	CString m_FlightTyOfmaxDelay ;
	ElapsedTime m_MaxDelayStart ;
	ElapsedTime m_Q1 ;
	ElapsedTime m_Q2 ;
	ElapsedTime m_Q3 ;
	ElapsedTime m_P1 ;
	ElapsedTime m_P5 ;
	ElapsedTime m_P90 ;
	ElapsedTime m_P95 ;
	ElapsedTime m_P99 ;
public:
	void ExportFile(ArctermFile& _file)
	{
		_file.writeField(m_Taxiway)  ;
		_file.writeField( m_FromNode) ;
		_file.writeField( m_ToNode) ;
		_file.writeInt(m_numofaircraft)  ;
		_file.writeTime( m_totalDelay);
		_file.writeTime( m_MinDelay );
		_file.writeTime( m_MeanDelay );
		_file.writeTime( m_MaxDelay );
		_file.writeField( m_FlightTyOfmaxDelay) ;
		_file.writeTime( m_MaxDelayStart) ;
		_file.writeTime( m_Q1 );
		_file.writeTime( m_Q2 );
		_file.writeTime( m_Q3 );
		_file.writeTime( m_P1 );
		_file.writeTime( m_P5 );
		_file.writeTime( m_P90 );
		_file.writeTime( m_P95 );
		_file.writeTime( m_P99 );
		_file.writeLine() ;
	}
	void ImportFile(ArctermFile& _file)
	{
		
		TCHAR val[1024] = {0} ;

 		_file.getField(val ,1024)  ;
		m_Taxiway = val ;

		_file.getField(val ,1024 ) ;
		m_FromNode = val ;

		_file.getField( val ,1024) ;
		m_ToNode = val ;

		_file.getInteger(m_numofaircraft)  ;
		_file.getTime( m_totalDelay);
		_file.getTime( m_MinDelay );
		_file.getTime( m_MeanDelay );
		_file.getTime( m_MaxDelay );
		_file.getField(val ,1024 ) ;
		m_FlightTyOfmaxDelay =val ;
		_file.getTime( m_MaxDelayStart) ;
		_file.getTime( m_Q1 );
		_file.getTime( m_Q2 );
		_file.getTime( m_Q3 );
		_file.getTime( m_P1 );
		_file.getTime( m_P5 );
		_file.getTime( m_P90 );
		_file.getTime( m_P95 );
		_file.getTime( m_P99 );


	}
};
class CTaxiwayDelayReport :
	public CAirsideBaseReport
{
public:
	class CDelayLogFilterFunc : public CBaseFilterFun
	{
	protected:
		CTaxiwayDelayParameters* m_Parameter ;
	public:
		CDelayLogFilterFunc(CTaxiwayDelayParameters* _parameter) ;
		BOOL bFit(class_type* ct,const ARCEventLog* _evenlog);
	
	};
protected:
	CTaxiwayDelayReportData* m_ReportDelayData ;
	ARCBaseLog<AirsideFlightLogItem> mFlightLogData;
	CTaxiwayDelayReportResult* m_BaseResult ;
	
public:
	CTaxiwayDelayReportResult* GetBaseDelayReportResult() { return m_BaseResult ;} ;
	CTaxiwayDelayParameters* m_Paramer ;
public:
	CTaxiwayDelayReport(CBGetLogFilePath pFunc,CTaxiwayDelayParameters* _Paramer );
	~CTaxiwayDelayReport(void);
public:
	void GenerateReport(CParameters * parameter) ;
	void RefreshReport(CParameters * parameter);

	int GetReportType() { return Airside_TaxiwayDelay ;};

	void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);

	void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

protected:
	std::vector<CTaxiwayDelayReportDetailDataItem*> m_ReportDetailData ;
	std::vector<CTaxiwayDelayReportSummaryDataItem*> m_ReportSummaryData ;
protected:
	void InintListDetailHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL) ;
	void InitListSummaryHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL) ;
	void ClearDetailData() ;
	void ClearSummaryData() ;
protected:
	void GenerateDetailReportData(CTaxiwayDelayParameters* PtaxiwayDelayPar) ;
	void GenerateSummaryReportData(CTaxiwayDelayParameters* PtaxiwayDelayPar) ;

	//void SortByDelayTime(std::map<CONFIICTREPORTDATA_ITEM*,ElapsedTime>& _delaydata) ;
	void GetFlightEnterTimeAndLeaveTimeFormSegment(CString _fightID ,int _uniqeID,int _fromID ,int _ToID ,int _taxiway ,
													std::vector<TAXIWAYSEGINFO>& vSegInfo,CTaxiwayDelayParameters* PtaxiwayDelayPar);
protected:

	void FillDetailListContent(CXListCtrl& cxListCtrl, CParameters * parameter) ;
	void InsertDetailItem(CXListCtrl& cxListCtrl,CTaxiwayDelayReportDetailDataItem* detailDataItem) ;
protected:
	void FillSummaryListContent(CXListCtrl& cxListCtrl, CParameters * parameter) ;
	void AnalysisSummaryDelay(CTaxiwayDelayReportSummaryDataItem* _reportSummaryDataItem , CTaxiwaySegmentItem* _intersectionItem) ;
	void InsertSummaryDataItem(CXListCtrl& cxListCtrl,CTaxiwayDelayReportSummaryDataItem* _sunmmaryDataItem,int _index ) ;
public:
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail) ;
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail)  ;
public:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
	CString GetReportFileName();
};