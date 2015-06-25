#pragma once
#include "airsidebasereport.h"
#include <vector>
#include "../Results/AirsideFlightLog.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../Results/OutputAirside.h"
#include "../common/FLT_CNST.H"
class CAirsideOperationResult;
class CAirsideReportBaseResult;


class  AIRSIDEREPORT_API CAirsideOperationReport :
	public CAirsideBaseReport
{

public:
	const static CString strFltOperationType[];
	enum FltOperationType
	{
		FltOperationType_Arr = 0,
		FltOperationType_Dep, 
		FltOperationType_TA
	};
	class AirsideFltOperationItem
	{

	public :
		AirsideFltOperationItem(CAirportDatabase* m_AirportDB)
		{
			operationType = FltOperationType_Arr;	
			actualArrTime = 0;
			actualDepTime = 0;
			arrDelay = 0;
			DepDelay = 0;
			fltcons.SetAirportDB(m_AirportDB) ;
		}
		~AirsideFltOperationItem()
		{
		}
	public:
		CString strFltID;
		FltOperationType operationType;

		AirsideFlightDescStruct fltDesc;

		FlightConstraint fltcons;

		long actualArrTime;
		long actualDepTime;
		long arrDelay;
		long DepDelay;
	public:
		BOOL ExportFile(ArctermFile& _file) ;
		BOOL ImportFile(ArctermFile& _file) ;
	protected:
	private:
	};

public:
	CAirsideOperationReport(CBGetLogFilePath pFunc);
	virtual ~CAirsideOperationReport(void);

public:
	void GenerateReport(CParameters * parameter);
	void RefreshReport(CParameters * parameter);
	int GetReportType(){ return Airside_AircraftOperation;} 
	CAirsideReportBaseResult *GetReportResult(){ return m_pAirsideOperationResult;}
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	//void HandleFlightLog(AirsideFlightLogEntry& fltlog,CAirsideNodeDelayReportParameter * parameter);
	//bool IsValidTime(AirsideFlightDescStruct& fltlog,CAirsideNodeDelayReportParameter * parameter);
	//bool IsFitConstraint(AirsideFlightDescStruct& fltlog,CAirsideNodeDelayReportParameter * parameter, FlightConstraint& fltConstraint);

	//const std::vector<AirsideFltOperationItem>& GetResult(){ return m_vOperationData;}
protected:
	bool fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons);
	FltOperationType getOperationType(const AirsideFlightDescStruct& fltDesc);
	long GetMaxTime();
	long GetMinTime();

	//std::vector<long> GetCount(long start ,long end);

protected:
	std::vector<AirsideFltOperationItem> m_vOperationData;
	CAirsideReportBaseResult              *m_pAirsideOperationResult;
public:
	BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
protected:
	 BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;
};