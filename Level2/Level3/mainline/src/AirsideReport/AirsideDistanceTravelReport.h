#pragma once
#include "AirsideBaseReport.h"
//#include "../common/elaptime.h"
#include "../Results/AirsideFlightLog.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../Results/OutputAirside.h"
#include "../Common/FLT_CNST.H"
//#include "AirsideFlightDistanceTravelReportBaseResult.h"

class CParameters;
class CAirsideFlightDistanceTravelReportBaseResult;
class CAirsideReportBaseResult;

class CAirsideDistanceTravelReport :
	public CAirsideBaseReport
{
public:
	class  DistanceTravelReportItem
	{
	public:
		AirsideFlightDescStruct fltDesc;
		double distance;

		DistanceTravelReportItem(CAirportDatabase* m_AirportDB)
		{
			distance = 0.0f;
			memset(&fltDesc,0,sizeof(AirsideFlightDescStruct));
			fltCons.SetAirportDB(m_AirportDB) ;
		}

		FlightConstraint fltCons;

		BOOL ExportT0File(ArctermFile& _file) ;
		BOOL ImportFromFile(ArctermFile& _file) ;
	};

public:
	CAirsideDistanceTravelReport(CBGetLogFilePath pFunc);
	virtual ~CAirsideDistanceTravelReport();
	//members
public:
	//the member that store the pointer of the first item;
	const std::vector<DistanceTravelReportItem>& GetResult(){ return m_vResult;}
	virtual int GetReportType(){ return Airside_DistanceTravel;}
protected:
	std::vector<DistanceTravelReportItem> m_vResult;
	
	CAirsideFlightDistanceTravelReportBaseResult * m_pResult;

	CParameters *m_pParameter;
public:
	virtual void GenerateReport(CParameters * parameter);
	virtual void RefreshReport(CParameters * parameter);
	double GetMaxDisantce();
	double GetMinDisantce();
    long GetCount(double start ,double end);

	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);

	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
	CAirsideReportBaseResult *GetReportResult(){ return (CAirsideReportBaseResult *)m_pResult;}
private:
	//calculate the distance ,called by GenerateReport()
	void CalcDistance();
	//calculate the distance between two points 
	double distancePoint(double x1,double y1,double z1 , double x2 ,double y2 ,double z2);
	//get the count

	void ClearDistanceTravelReportItems();
	bool IsFitConstraint(AirsideFlightDescStruct& fltDesc,CParameters * parameter, FlightConstraint& fltConstraint);

	CString FormatDoubleValueToString(double dValue);
public:
	 //void Draw3DChartFromFile(CARC3DChart& chartWnd, ArctermFile& _file,int isdetail) ;
	 //void ExportFileFrom3DChart(CARC3DChart& chartWnd,  ArctermFile& _file, CParameters *pParameter) ;
public:
	 BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
	 BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
protected:
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;
};
