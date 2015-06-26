#pragma once
#include "AirsideBaseReport.h"
#include "AirsideReportBaseResult.h"

class CParameters;
class ArctermFile;
class CAirportDatabase;
class AirsideFlightRunwayDelayData;
class AirsideFlightRunwayDelayReportPara;
class AIRSIDEREPORT_API AirsideFlightRunwayDelayBaseResult
{
public:
	AirsideFlightRunwayDelayBaseResult(CParameters * parameter);
	virtual ~AirsideFlightRunwayDelayBaseResult(void);

public:
	virtual void SetReportData( AirsideFlightRunwayDelayData* pReportData);

	void SetCBGetFilePath(CBGetLogFilePath pFunc);

	CBGetLogFilePath m_pCBGetLogFilePath;

	CAirsideReportBaseResult *GetReportResult();

	void setLoadFromFile(bool bLoad);
	bool IsLoadFromFile();

	void SetAirportDB(CAirportDatabase* _AirportDB) { m_AirportDB = _AirportDB ;}

public:
	virtual void InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL )=0;

	virtual void FillListContent( CXListCtrl& cxListCtrl);

	virtual void RefreshReport(CParameters * parameter );

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

protected:
	CAirportDatabase* m_AirportDB ;
	CAirsideReportBaseResult *m_pChartResult;
	AirsideFlightRunwayDelayData* m_pReportData;
	AirsideFlightRunwayDelayReportPara *m_pParameter;
	bool m_bLoadFromFile;

};
