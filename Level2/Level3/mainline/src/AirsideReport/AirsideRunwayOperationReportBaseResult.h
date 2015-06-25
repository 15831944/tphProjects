#pragma once
#include "AirsideBaseReport.h"
class CParameters;
class CAirsideReportBaseResult;
class ArctermFile;
class CAirsideRunwayOperationReportBaseResult
{
public:
	CAirsideRunwayOperationReportBaseResult(void);
	virtual ~CAirsideRunwayOperationReportBaseResult(void);

public:
	virtual void GenerateResult(CParameters *pParameter);
	virtual void Draw3DChart();

	void SetCBGetFilePath(CBGetLogFilePath pFunc);

	CBGetLogFilePath m_pCBGetLogFilePath;

	CAirsideReportBaseResult *GetReportResult();

	void setLoadFromFile(bool bLoad);
	bool IsLoadFromFile();
protected:
	CAirportDatabase* m_AirportDB ;
	CAirsideReportBaseResult *m_pChartResult;

	bool m_bLoadFromFile;
public:
	void SetAirportDB(CAirportDatabase* _AirportDB) { m_AirportDB = _AirportDB ;}

public:
	virtual void InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);

	virtual void FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter );

	virtual void RefreshReport(CParameters * parameter);


	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
};
