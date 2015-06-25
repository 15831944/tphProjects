#pragma once
#include "airsidebasereport.h"
#include <vector>
class CRunwayExitParameter ;
class ARCEventLog;
class CRunwayExitResult;
class AirsideFlightLogDesc;
class CFlightFliterData
{
public:
	int m_RunwayID ;
	CString m_RunwayName;

	int m_RunwayExitID ;
	CString m_RunwayExitName;

	CString m_AcID ;
	CString m_FlightType ;

	ElapsedTime m_EntryTime ;
	CString m_Operation ;
public:
	CFlightFliterData():m_RunwayExitID(-1),m_RunwayID(-1) {} ;
public:
	BOOL ImportFile(ArctermFile& _file) 
	{
		TCHAR Th[1024] = {0} ;

		_file.getInteger(m_RunwayID) ;
		_file.getField(Th,1024) ;
		m_RunwayName.Format(_T("%s"),Th) ;

		_file.getInteger(m_RunwayExitID) ;
		_file.getField(Th,1024) ;
		m_RunwayExitName.Format(_T("%s"),Th) ;

		_file.getField(Th,1024) ;
		m_AcID.Format(_T("%s"),Th) ;

		_file.getField(Th,1024) ;
		m_FlightType.Format(_T("%s"),Th) ;

		_file.getTime(m_EntryTime) ;

		_file.getField(Th,1024) ;
		m_Operation.Format(_T("%s"),Th) ;
		_file.getLine() ;
		return TRUE ;
	}
	BOOL ExportFile(ArctermFile& _file) 
	{
		_file.writeInt(m_RunwayID) ;
		_file.writeField(m_RunwayName) ;
		_file.writeInt(m_RunwayExitID) ;
		_file.writeField(m_RunwayExitName);
		_file.writeField(m_AcID) ;
		_file.writeField(m_FlightType) ;
		_file.writeTime(m_EntryTime);
		_file.writeField(m_Operation) ;
		_file.writeLine() ;
		return TRUE ;
	}
};

class CTimeIntervalFlightData
{
public:
	ElapsedTime m_startTime ;
	ElapsedTime m_endTime ;
public:
	std::vector<CFlightFliterData> m_FlightData;
public:
	void AddData(const CFlightFliterData& _data) 
	{
		m_FlightData.push_back(_data) ;
	};

	void ExportFile(ArctermFile& _file) 
	{
		_file.writeTime(m_startTime) ;
		_file.writeTime(m_endTime) ;
		_file.writeLine() ;

		int size = (int)m_FlightData.size() ;

		_file.writeInt(size);
		_file.writeLine() ;
		for (int i =0 ; i < size ;i++)
		{
			m_FlightData[i].ExportFile(_file) ;
		}
	}
	void ImportFile(ArctermFile& _file) 
	{
		_file.getTime(m_startTime) ;
		_file.getTime(m_endTime) ;
		_file.getLine() ;

		int size = 0 ;
		if(!_file.getInteger(size))
			return ;
		_file.getLine() ;
		for (int i = 0 ; i < size ;i++)
		{
			CFlightFliterData dataItem ;
			dataItem.ImportFile(_file) ;
			m_FlightData.push_back(dataItem) ;
		}
	}
};
class CRunwayExitRepDataItem
{
public:
	int m_RunwayID ;
	CString m_RunwayName;
	
	int m_RunwayExitID ;
	CString m_RunwayExitName;
public:
	std::vector<CTimeIntervalFlightData*> m_RunwayExitData;

public:
	CRunwayExitRepDataItem(int _runwayID ,int _exitID):m_RunwayID(_runwayID),m_RunwayExitID(_exitID){} ;
	~CRunwayExitRepDataItem() ;
public:
	void InitTimeIntervalByParamter(CRunwayExitParameter* _parameter) ;

	void AddFlightEvenLogData(const CFlightFliterData& _data) ;

	std::vector<CTimeIntervalFlightData*>* GetData() { return &m_RunwayExitData ;} ;

public:
	int GetMaxUsager(CString& _whichsegment) ;
	int GetMinUsager(CString& _whichsegment) ;
	int GetMeanUsager() ;

	void ExportFile(ArctermFile& _file) 
	{
		_file.writeInt(m_RunwayID) ;
		_file.writeField(m_RunwayName) ;
		_file.writeInt(m_RunwayExitID) ;
		_file.writeField(m_RunwayExitName) ;
		_file.writeLine() ;

		_file.writeInt((int)m_RunwayExitData.size()) ;
		_file.writeLine() ;

		for (int i = 0 ; i < (int)m_RunwayExitData.size() ;i++)
		{
			m_RunwayExitData[i]->ExportFile(_file) ;
		}
	}
	void ImportFile(ArctermFile& _file) 
	{
		TCHAR TH[1024] = {0} ;
		_file.getInteger(m_RunwayID) ;
		_file.getField(TH,1024) ;
		m_RunwayName.Format(_T("%s"),TH) ;
		_file.getInteger(m_RunwayExitID) ;
		_file.getField(TH,1024) ;
		m_RunwayExitName.Format(_T("%s"),TH) ;
		_file.getLine() ;
		
		int size = 0 ;
		if(!_file.getInteger(size))
			return ;
		_file.getLine() ;
		CTimeIntervalFlightData*  Pdata = NULL ;
		for (int i = 0 ; i < size ;i++)
		{
			Pdata = new CTimeIntervalFlightData() ;
			Pdata->ImportFile(_file) ;
			m_RunwayExitData.push_back(Pdata) ;
		}
	}
};
class CRunwayExitRepData
{
public:
	CRunwayExitRepData() ;
	~CRunwayExitRepData() ;
protected:
	std::vector<CRunwayExitRepDataItem*> m_Data ;
public:
	void AddFlightEvenlog(const CFlightFliterData& _data,CRunwayExitParameter* _parameter) ;
	CRunwayExitRepDataItem* FindRunwayExitRepDataItem(int _runwayID ,int _exitID) ;

	std::vector<CRunwayExitRepDataItem*>* GetData() { return &m_Data ;} ;
public:
	void ClearAll() ;

	void ExportFile(ArctermFile& _file) ;
	void ImportFile(ArctermFile& _file) ;
};

class CRunwayExitSummaryDataItem
{
public:
	CString m_Runway ;
	CString m_ExitName;
	int m_MinUsager ;
	CString m_MinUsagerAt ;
	int m_MaxUsager ;
	CString m_MaxUsagerAt ;
	int m_MeanUsager ;

	int m_Q1 ;
	int m_Q2 ;
	int m_Q3 ;

	int m_P1 ;
	int m_p5 ;
	int m_p10 ;
	int m_p90 ;
	int m_P95 ;
	int m_p99 ;

	CRunwayExitSummaryDataItem()
	{
		 m_MinUsager = 0;
		 m_MaxUsager = 0;
		 m_MeanUsager = 0;
		 m_Q1 = 0;
		 m_Q2 = 0;
		 m_Q3 = 0;

		 m_P1 = 0;
		 m_p5 = 0;
		 m_p10 = 0;
		 m_p90 = 0;
		 m_P95 = 0;
		 m_p99 = 0;
	}
public:
	BOOL ExportFile(ArctermFile& _file) 
	{
		_file.writeField(m_Runway) ;
		_file.writeField( m_ExitName);
		_file.writeInt( m_MinUsager) ;
		_file.writeField( m_MinUsagerAt) ;
		_file.writeInt( m_MaxUsager) ;
		_file.writeField( m_MaxUsagerAt) ;
		_file.writeInt( m_MeanUsager) ;

		_file.writeInt( m_Q1 );
		_file.writeInt( m_Q2 );
		_file.writeInt( m_Q3 );

		_file.writeInt( m_P1) ;
		_file.writeInt( m_p5 );
		_file.writeInt( m_p10 );
		_file.writeInt( m_p90) ;
		_file.writeInt( m_P95 );
		_file.writeInt( m_p99) ;
		_file.writeLine() ;
		return TRUE ;
	}
	BOOL ImportFile(ArctermFile& _file) 
	{
		TCHAR val[1024] = {0} ;

		_file.getField(val,1024) ;
		m_Runway = val ;

		_file.getField(val,1024) ;
		m_ExitName = val ;

		_file.getInteger(m_MinUsager) ;
		_file.getField(val,1024) ;
		m_MinUsagerAt = val ;

		_file.getInteger(m_MaxUsager) ;
		_file.getField(val,1024) ;
		m_MaxUsagerAt = val ;

		_file.getInteger(m_MeanUsager) ;

		_file.getInteger( m_Q1 );
		_file.getInteger( m_Q2 );
		_file.getInteger( m_Q3 );

		_file.getInteger( m_P1) ;
		_file.getInteger( m_p5 );
		_file.getInteger( m_p10 );
		_file.getInteger( m_p90) ;
		_file.getInteger( m_P95 );
		_file.getInteger( m_p99) ;
		return  TRUE ;
	}
};
class CRunwayExitReport :
	public CAirsideBaseReport
{
protected:
	BOOL HandetheRunwayExitFlightLog(CRunwayExitParameter* _parameter,const ARCEventLog* _evenlog,AirsideFlightLogDesc& fltdesc) ;

	void AnalysisRunwayExitFlightLogForReport(CRunwayExitParameter* _parameter) ;
protected:
	std::vector<CFlightFliterData> m_FilterEventLog ;
	CRunwayExitRepData m_ReportBaseData ;
protected:
	std::vector<CRunwayExitSummaryDataItem> m_SummaryData ;
protected:
	CRunwayExitResult* m_BaseResult ;
	CRunwayExitParameter* m_parameter;
public:
	CRunwayExitResult* GetBaseDelayReportResult() { return m_BaseResult ;} ;
public:
	CRunwayExitReport(CBGetLogFilePath pFunc,CRunwayExitParameter* _parameter);
	~CRunwayExitReport(void);
public:
	void GenerateReport(CParameters * parameter) ;
	void RefreshReport(CParameters * parameter);

	int GetReportType() { return Airside_RunwayExit ;};

	void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);

	void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

protected:
	void GenerateDetailReportData(CRunwayExitParameter* _paramerer) ;
	void GenerateSummaryReportData(CRunwayExitParameter* _paramerer) ;
	void InitDetailListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);
	void InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);
	void FillDetailListContent(CXListCtrl& cxListCtrl);
	void FillSummaryListContent(CXListCtrl& cxListCtrl);
public:
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail)  ;
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail)  ;
public:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
	CString GetReportFileName();
};
