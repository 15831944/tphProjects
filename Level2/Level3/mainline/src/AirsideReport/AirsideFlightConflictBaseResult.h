#pragma once
#include "AirsideBaseReport.h"
#include "AirsideReportBaseResult.h"

class CParameters;
class ArctermFile;
class CAirportDatabase;
class FlightConflictReportData;
class AirsideFlightConflictPara;
class AIRSIDEREPORT_API AirsideFlightConflictBaseResult
{
public:
	AirsideFlightConflictBaseResult(CParameters * parameter);
	virtual ~AirsideFlightConflictBaseResult(void);

public:
	virtual void SetReportData( FlightConflictReportData* pReportData);

	void SetCBGetFilePath(CBGetLogFilePath pFunc);

	CBGetLogFilePath m_pCBGetLogFilePath;

	CAirsideReportBaseResult *GetReportResult();

	void setLoadFromFile(bool bLoad);
	bool IsLoadFromFile();

	void SetAirportDB(CAirportDatabase* _AirportDB) { m_AirportDB = _AirportDB ;}

public:
	virtual void InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);

	virtual void FillListContent( CXListCtrl& cxListCtrl);

	virtual void RefreshReport(CParameters * parameter );

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

protected:
	CAirportDatabase* m_AirportDB ;
	CAirsideReportBaseResult *m_pChartResult;
	FlightConflictReportData* m_pReportData;
	AirsideFlightConflictPara *m_pParameter;
	bool m_bLoadFromFile;

};
