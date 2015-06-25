#pragma once
#include "AirsideBaseReport.h"

class CAirsideDurationReport :
	public CAirsideBaseReport
{
public:
	class AirsideDurationReportItem
	{
		public:
			char FlightID[255];
			long DurTime;
			char ArrID[128];
			char DepID[128];
			char ICaoCode[30];

			AirsideDurationReportItem *nextItem;
	public:
		AirsideDurationReportItem()
		{
			DurTime = -1;
			nextItem = NULL;
		}
	};

public:
	CAirsideDurationReport(CBGetLogFilePath pFunc);
	~CAirsideDurationReport(void);

public:
	AirsideDurationReportItem *m_pRecordset;
	virtual void GenerateReport(CParameters * parameter);
	virtual int GetReportType(){ return Airside_Duration;}
	double GetMaxDurTime()
	{
		return m_dMaxDurTime;
	};
	double GetMinDurTime()
	{
		return m_dMinDurTime;
	};
	long GetCount(double start ,double end);

private:
	double m_dMinDurTime;
	double m_dMaxDurTime;
};
