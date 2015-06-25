#pragma  once
#include "../Results/AirsideFlightLogEntry.h"
#include <vector>
#include "CARC3DChart.h"
#include "../MFCExControl/XListCtrl.h"
#include "airsidebasereport.h"
class CParameters;
class CAirsideFlightSummaryActivityReportBaseResult;
class CAirsideReportBaseResult;

enum FLTCNSTR_MODE;

/////////////////////////////////////////////////////////////////////////////////////////////
class AIRSIDEREPORT_API CFlightActivityBaseResult
{
public:
	CFlightActivityBaseResult();
	virtual ~CFlightActivityBaseResult();

public:
	enum subReportType
	{
		FAR_SysEntryDist = 0,
		FAR_SysExitDist,
		FAR_COUNT,

		ACTIVITY_RESULT_VT,
		ACTIVITY_RESULT_DT,
		ACTIVITY_RESULT_AT
	};

	virtual void RefreshReport(CParameters * parameter) = 0;
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL) = 0;
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter) = 0;
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg) = 0 ;
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg) = 0 ;
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter) = 0;
	virtual BOOL WriteReportData(ArctermFile& _file) = 0;
	virtual BOOL ReadReportData(ArctermFile& _file) = 0;
	virtual void SetReportResult(CFlightActivityBaseResult* pResult) = 0;
	virtual enumASReportType_Detail_Summary GetReportType() = 0;

	void setLoadFromFile(bool bLoad);
	bool IsLoadFromFile();
	void SetCBGetFilePath(CBGetLogFilePath pFunc);
	CBGetLogFilePath m_pCBGetLogFilePath;
protected:
	bool m_bLoadFromFile;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class AIRSIDEREPORT_API CFlightSummaryActivityResult : public CFlightActivityBaseResult
{
public:
	CFlightSummaryActivityResult();
	virtual ~CFlightSummaryActivityResult();
public:
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	void RefreshReport(CParameters * parameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg);
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;

public:
	class VTItem
	{
	public:
		VTItem()
			:tTime(0L)
			,dDistance(0.0)
			,dSpeed(0.0)
		{

		}

		~VTItem()
		{

		}

		long tTime;
		double dDistance;
		double dSpeed;

	public:
		BOOL ExportFile(ArctermFile& _file);
		BOOL ImportFile(ArctermFile& _file);
	};

	class ATItem
	{
	public:
		ATItem()
			:dAcc(0.0)
		{

		}
		~ATItem()
		{

		}

		long startTime;
		long endTime;
		double dAcc;
	public:
		BOOL ExportFile(ArctermFile& _file);
		BOOL ImportFile(ArctermFile& _file);
	};
	class FlightActItem
	{
	public:
		FlightActItem()
			:ID(-1)
			,bArrOrDeplActivity(false)
			,startTime(0L)
			,endTime(0L)
			,StartPosition(-1)
			,EndPostion(-1)
		{

		}

		~FlightActItem()
		{

		}
	
		int ID;
		bool bArrOrDeplActivity;
		AirsideFlightDescStruct fltDesc;
		long startTime;
		long endTime;
		int StartPosition;
		int EndPostion;
		std::vector<VTItem> vItem;//record VT information
		std::vector<ATItem> vATItem;//record Acceleration and Deceleration information

	public:
		BOOL ExportFile(ArctermFile& _file);
		BOOL ImportFile(ArctermFile& _file);
		bool operator < (const FlightActItem& item)const;
	};

public:
	std::vector<FlightActItem>&  GetResult()        {    return m_vResult;   }
	static CString GetObjName(int nObjectID);
	bool fits(CParameters * ,const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons,FLTCNSTR_MODE& fltConMode);
	void AddToResult(CParameters* param, FlightActItem& fltActItem);
	float CalcFlightRunDistance(int nFirst, int nSecond,AirsideFlightLogEntry& logEntry);
	bool IsInvalid(int nLastEvent,AirsideFlightLogEntry& logEntry);
	int GetAccEventCount(int nEvent,AirsideFlightLogEntry& logEntry,int nAcc);

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

	void SetReportResult(CFlightActivityBaseResult* pResult);
	enumASReportType_Detail_Summary GetReportType() {return ASReportType_Summary;}
private:
	CAirsideFlightSummaryActivityReportBaseResult* m_pChartResult;
	std::vector<FlightActItem> m_vResult;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class AIRSIDEREPORT_API CFlightDetailActivityResult : public CFlightActivityBaseResult
{
public:
	CFlightDetailActivityResult();
	virtual ~CFlightDetailActivityResult();
public:
	virtual void GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
public:
	class FltActTraceItem
	{
	public:
		FltActTraceItem()
			:nObjID(-1)
			,strName(_T(""))
			,entryTime(0L)
			,nObjType(0)
		{
		}
		~FltActTraceItem()
		{
		}


		int nObjType;//0,object;1,intersection node
		int nObjID;
		CString strName;
		long entryTime;
	public:
		BOOL ExportFile(ArctermFile& _file);
		BOOL ImportFile(ArctermFile& _file);
	};
	class FltActItem
	{
	public:
		FltActItem()
			:entryTime(0L)
			,exitTime(0L)
			,strFltType(_T(""))
			, ID(-1)
		{
		}
		~FltActItem()
		{
		}

		int ID;
		AirsideFlightDescStruct fltDesc;
		long entryTime;
		long exitTime;
		long durationTime;
		CString strFltType;

		std::vector<FltActTraceItem> vTrace;
	public:
		BOOL ExportFile(ArctermFile& _file);
		BOOL ImportFile(ArctermFile& _file);
	};

 public:
	void RefreshReport(CParameters * parameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
 	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
 
 	std::vector<FltActItem>&  GetResult()        {    return m_vResult;   }
 
 	bool fits(CParameters * ,const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons);
 	void AddToResult(CParameters* param, FltActItem& fltActItem);
 
protected:
 	std::vector<FltActItem> m_vResult;
 
 	CAirsideReportBaseResult *m_pResult;
public:
 	long GetMaxTime(subReportType subType);
 	long GetMinTime(subReportType subType);
 	//nIndex,get the data in the m_vResult at the specific position
 	long GetCount(subReportType subType,long start ,long end);
public:
 	BOOL ExportReportData(ArctermFile& _file,CString& Errmsg);
 	BOOL ImportReportData(ArctermFile& _file,CString& Errmsg);

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

	void SetReportResult(CFlightActivityBaseResult* pResult);
	enumASReportType_Detail_Summary GetReportType() {return ASReportType_Detail;}
 protected:
 	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;
	CString GetObjectName(int objectType,int objID);
};