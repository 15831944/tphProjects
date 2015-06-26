#include "stdafx.h"
#include "AirsideMultipleRunReportAgent.h"
#include "AirsideReport/Parameters.h"
#include "AirsideReport/AirsideFlightDelayReport.h"
#include "FlightOperationalReport.h"
#include "AirsideReport/AirsideTakeoffProcessReport.h"
#include "AirsideReport/AirsideRunwayOperationsReport.h"
#include "AirsideReport/AirsideFlightRunwayDelayReport.h"
#include "AirsideReport/AirsideFlightStandOperationReport.h"

CAirsideMultipleRunReportAgent::CAirsideMultipleRunReportAgent(void)
{
}


CAirsideMultipleRunReportAgent::~CAirsideMultipleRunReportAgent(void)
{
}

CAirsideBaseReport* CAirsideMultipleRunReportAgent::AddReportWhatToGen(reportType _reportType,CParameters * parameter, Terminal* pTerm)
{
	CAirsideBaseReport* pReport = NULL;
	switch (_reportType)
	{
	case Airside_FlightDelay:
		pReport = new CAirsideFlightDelayReport(m_pGetLogFilePath);
		break;
	case Airside_AircraftOperational:
		pReport = new CFlightOperationalReport(m_pGetLogFilePath);
		break;
	case Airside_TakeoffProcess:
		pReport = new CAirsideTakeoffProcessReport(m_pGetLogFilePath);
		break;
	case Airside_RunwayOperaitons:
		pReport = new AirsideRunwayOperationsReport(m_pGetLogFilePath);
		((AirsideRunwayOperationsReport*)pReport)->SetAirportDB(pTerm->m_pAirportDB);
		break;
	case Airside_RunwayDelay:
		pReport = new  AirsideFlightRunwayDelayReport(m_pGetLogFilePath,(AirsideFlightRunwayDelayReportPara*)parameter);
		break;
	case Airside_StandOperations:
		pReport = new CAirsideFlightStandOperationReport(m_pScheduleStand,m_pGetLogFilePath);
		break;
	default:
		break;
	}
	return pReport;
}

void CAirsideMultipleRunReportAgent::GenerateReport(CAirsideBaseReport* pReport, CParameters * parameter )
{
	if (pReport)
	{
		pReport->SetReportFileDir(m_strReportPath);
		pReport->GenerateReport(parameter);
	}
}

void CAirsideMultipleRunReportAgent::InitReportPath( const CString& strReportPath )
{
	m_strReportPath = strReportPath;
}

