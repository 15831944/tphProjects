#pragma once
#include "AirsideBaseReport.h"
#include "../Results/AirsideRunwayOperationLog.h"
#include "../Results/AirsideFlightLog.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../Results/OutputAirside.h"
#include "AirsideRunwayUtilizationReportParam.h"


class CParameters;
class CAirsideReportBaseResult;
class ArctermFile;


namespace AirsideRunwayUtilizationReport
{	
	class AIRSIDEREPORT_API DetailFlightRunwayResult
	{
	public:
		DetailFlightRunwayResult()
		{
			m_nRunwayID = -1;
			m_enumRunwayMark =  RUNWAYMARK_FIRST;
			m_enumRunwayOperation =  CAirsideRunwayUtilizationReportParam::Landing;

		}
		~DetailFlightRunwayResult();

		//file operations, load save report
	public:
		virtual BOOL WriteReportData(ArctermFile& _file);
		virtual BOOL ReadReportData(ArctermFile& _file);
	public:
		//flight information
		CString m_strAirline;
		CString m_strFlightID;
		CString m_strFlightType;

		//runway information
		int m_nRunwayID;
		RUNWAY_MARK m_enumRunwayMark;
		CString m_strMarkName;

		CAirsideRunwayUtilizationReportParam::RunwayOperation m_enumRunwayOperation;

		//airroute passed
		CString m_strAirRoute;

		//occupancy time and exit time
		ElapsedTime m_eTimeStart;
		ElapsedTime m_eTimeEnd;

		ElapsedTime m_eTimeOccupancy;

		//enter exit at the object
		CString m_strEnterObject;
		CString m_strExitObject;

		//possible exit
		CString m_strPossibleExit;
		ElapsedTime m_eMinimumTime;

		ElapsedTime m_eTimeWait;

	};
};
class CAirsideRunwayUtilizationReportBaseResult
{
public:
	CAirsideRunwayUtilizationReportBaseResult(void);
	~CAirsideRunwayUtilizationReportBaseResult(void);


public:
	virtual void GenerateResult(CParameters *pParameter);
	virtual void Draw3DChart();


	void SetCBGetFilePath(CBGetLogFilePath pFunc);

	CBGetLogFilePath m_pCBGetLogFilePath;

	CAirsideReportBaseResult *GetReportResult();

	void setLoadFromFile(bool bLoad);
	bool IsLoadFromFile();

	void SetProjID(int nProjID);
protected:
	CAirportDatabase* m_AirportDB ;
	CAirsideReportBaseResult *m_pChartResult;

	bool m_bLoadFromFile;
	int m_nProjID;

public:
	void SetAirportDB(CAirportDatabase* _AirportDB) { m_AirportDB = _AirportDB ;}

public:
	virtual void InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);

	virtual void FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter );

	virtual void RefreshReport(CParameters * parameter);

	//initialize m_vRunwayParamMark
	void GetAllRunwayMark(CParameters *pParameter);

protected:
	std::vector<CAirsideReportRunwayMark> m_vRunwayParamMark;
	AirsideRunwayOperationLog m_airsideOperationLog;

	//the flag stand for the parameter is defualt
	bool m_bParameterDefault;

	bool fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc, AirsideRunwayOperationLog::RunwayLogItem *pOperationItem);
	void Clear();

	//file operation, save and read data
public:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

public:
	std::vector<AirsideRunwayUtilizationReport::DetailFlightRunwayResult *> m_vDetailResult;

};
