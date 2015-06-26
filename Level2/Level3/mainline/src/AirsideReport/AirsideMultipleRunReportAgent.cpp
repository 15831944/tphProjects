#include "stdafx.h"
#include "AirsideMultipleRunReportAgent.h"
#include "AirsideReport/Parameters.h"
#include "AirsideReport/AirsideFlightDelayReport.h"
#include "AirsideReport/AirsideOperationReport.h"
#include "AirsideReport/AirsideTakeoffProcessReport.h"
#include "AirsideReport/AirsideRunwayOperationsReport.h"
#include "AirsideReport/AirsideFlightRunwayDelayReport.h"
#include "AirsideReport/AirsideFlightStandOperationReport.h"
#include "AirsideReport/AirsideRunwayCrossingsReport.h"

CAirsideMultipleRunReportAgent::CAirsideMultipleRunReportAgent(void)
{
}


CAirsideMultipleRunReportAgent::~CAirsideMultipleRunReportAgent(void)
{
	Clear();
}

void CAirsideMultipleRunReportAgent::AddReportWhatToGen(reportType _reportType,CParameters * parameter, Terminal* pTerm)
{
	CAirsideBaseReport* pReport = m_mapMutiRunPerformers[ _reportType ];
	if (pReport == NULL)
	{
		switch (_reportType)
		{
		case Airside_FlightDelay:
			m_mapMutiRunPerformers[ _reportType ] = new CAirsideFlightDelayReport(m_pGetLogFilePath);
			break;
		case Airside_AircraftOperation:
			m_mapMutiRunPerformers[ _reportType ] = new CAirsideOperationReport(m_pGetLogFilePath);
			break;
		case Airside_TakeoffProcess:
			m_mapMutiRunPerformers[ _reportType ] = new CAirsideTakeoffProcessReport(m_pGetLogFilePath);
			break;
		case Airside_RunwayOperaitons:
			m_mapMutiRunPerformers[ _reportType ] = new AirsideRunwayOperationsReport(m_pGetLogFilePath);
            ((AirsideRunwayOperationsReport*)m_mapMutiRunPerformers[_reportType])->SetAirportDB(pTerm->m_pAirportDB);
			break;
		case Airside_RunwayDelay:
			m_mapMutiRunPerformers[ _reportType ] = new AirsideFlightRunwayDelayReport(m_pGetLogFilePath,(AirsideFlightRunwayDelayReportPara*)parameter);
			break;
		case Airside_StandOperations:
			m_mapMutiRunPerformers[ _reportType ] = new CAirsideFlightStandOperationReport(m_pGetLogFilePath);
			break;
		case Airside_RunwayCrossings:
			m_mapMutiRunPerformers[ _reportType ] = new CAirsideRunwayCrossingsReport(m_pGetLogFilePath);
		default:
			break;
		}
	}
}

void CAirsideMultipleRunReportAgent::GenerateReport(reportType _reportType, CParameters * parameter )
{
	CAirsideBaseReport* pReport = m_mapMutiRunPerformers[ _reportType ];
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

void CAirsideMultipleRunReportAgent::Clear()
{
	std::map<reportType,CAirsideBaseReport*>::iterator iter = m_mapMutiRunPerformers.begin();
	for (; iter != m_mapMutiRunPerformers.end(); ++iter)
	{
		delete iter->second;
	}
	m_mapMutiRunPerformers.clear();
}

CString CAirsideMultipleRunReportAgent::GetSimResultPath( reportType _reportType )
{
	CAirsideBaseReport* pReport = m_mapMutiRunPerformers[ _reportType ];
	if (pReport)
	{
		return pReport->GetReportFilePath();
	}
	return CString("");
}
