#include "StdAfx.h"
#include "AirsideMultipleRunReport.h"
#include "Parameters.h"
#include "AirsideMultipleRunReportAgent.h"
#include "Engine/TERMINAL.H"
#include "AirsideFlightMutiRunDelayReport.h"
#include "MFCExControl/XListCtrl.h"
#include "CARC3DChart.h"
#include "AirsideStandMultiRunOperatinResult.h"
CAirsideMultipleRunReport::CAirsideMultipleRunReport(void)
{
}


CAirsideMultipleRunReport::~CAirsideMultipleRunReport(void)
{
	ClearMutipleRunResult();
}


void CAirsideMultipleRunReport::GenerateReport( Terminal* pTerminal,CBGetLogFilePath pFunc,CBSetCurrentSimResult pSimFunc,const CString& strPorjectPath,reportType _reportType,CParameters * parameter )
{
	if (parameter == NULL)
		return;
	
	std::vector<int> vReportRuns;

	if(!parameter->GetReportRuns(vReportRuns))
		return;

	AddReportWhatToGen(_reportType,parameter);
	m_mapMutiRunResult[_reportType]->ClearResultPath();
	for (int nRun = 0; nRun < static_cast<int>(vReportRuns.size()); ++nRun)
	{
		CString strSimResult;
		int iReportRun = vReportRuns.at(nRun);

		pSimFunc(iReportRun);

		CString strSimResultFolderName;
		strSimResultFolderName.Format(_T("SimResult%d"),iReportRun);
		CString strReportFileDir;
		strReportFileDir.Format(_T("%s\\SimResult\\%s\\report"),strPorjectPath,strSimResultFolderName);
		parameter->SetReportFileDir(strReportFileDir);
		parameter->SaveParameterFile();
		CAirsideMultipleRunReportAgent reportAgent;
		reportAgent.InitReportPath(strReportFileDir);
		reportAgent.SetCBGetLogFilePath(pFunc);
		reportAgent.AddReportWhatToGen(_reportType,parameter);
		reportAgent.GenerateReport(_reportType,parameter);

		m_mapMutiRunResult[_reportType]->AddSimResultPath(strSimResultFolderName,reportAgent.GetSimResultPath(_reportType));
	}
	
	m_mapMutiRunResult[_reportType]->LoadMultipleRunReport(parameter);
}

void CAirsideMultipleRunReport::AddReportWhatToGen( reportType _reportType,CParameters * parameter )
{
	CAirsideMultipleRunResult* pMultRunResult = m_mapMutiRunResult[ _reportType ];
	if (pMultRunResult == NULL)
	{
		switch (_reportType)
		{
		case Airside_FlightDelay:
			m_mapMutiRunResult[_reportType] = new CAirsideFlightMutiRunDelayResult;
			break;
        case Airside_StandOperations:
            m_mapMutiRunResult[_reportType] = new CAirsideStandMultiRunOperatinResult;
            break;
		default:
			break;
		}
	}
}

void CAirsideMultipleRunReport::InitListHead( reportType _reportType,CXListCtrl& cxListCtrl, CParameters * parameter, int iType/*=0*/,CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	CAirsideMultipleRunResult* pMultRunResult = m_mapMutiRunResult[ _reportType ];
	if (pMultRunResult )
	{
		pMultRunResult->InitListHead(cxListCtrl,parameter,iType,piSHC);
	}
}

void CAirsideMultipleRunReport::FillListContent( reportType _reportType,CXListCtrl& cxListCtrl, CParameters * parameter,int iType/*=0*/ )
{
	CAirsideMultipleRunResult* pMultRunResult = m_mapMutiRunResult[ _reportType ];
	if (pMultRunResult )
	{
		pMultRunResult->FillListContent(cxListCtrl,parameter,iType);
	}
}

void CAirsideMultipleRunReport::Draw3DChart( reportType _reportType,CARC3DChart& chartWnd, CParameters *pParameter,int iType/*=0*/ )
{
	CAirsideMultipleRunResult* pMultRunResult = m_mapMutiRunResult[ _reportType ];
	if (pMultRunResult )
	{
		pMultRunResult->Draw3DChart(chartWnd,pParameter,iType);
	}
}

void CAirsideMultipleRunReport::ClearMutipleRunResult()
{
	std::map<reportType,CAirsideMultipleRunResult*>::iterator iter = m_mapMutiRunResult.begin();
	std::map<reportType,CAirsideMultipleRunResult*>::iterator iterEnd = m_mapMutiRunResult.end();
	for( ; iter!=iterEnd; ++iter )
	{
		delete iter->second;
	}
	m_mapMutiRunResult.clear();	
}
