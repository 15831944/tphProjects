#include "StdAfx.h"
#include "AirsideMultipleRunReport.h"
#include "Parameters.h"
#include "AirsideMultipleRunReportAgent.h"
#include "Engine/TERMINAL.H"
#include "AirsideFlightMutiRunDelayReport.h"
#include "MFCExControl/XListCtrl.h"
#include "CARC3DChart.h"
#include "AirsideStandMultiRunOperatinResult.h"
#include "AirsideAircraftMultiRunTakeoffProcessResult.h"
#include "AirsideAircraftMutiRunRunwayOperationResult.h"
#include "AirsideFlightOperationalMultiRunResult.h"
#include "Common/TERMFILE.H"
#include "AirsideRunwayDelayMultiRunResult.h"

CAirsideMultipleRunReport::CAirsideMultipleRunReport(void)
{
}


CAirsideMultipleRunReport::~CAirsideMultipleRunReport(void)
{
	ClearMutipleRunResult();
}


void CAirsideMultipleRunReport::GenerateReport( Terminal* pTerminal,CBGetLogFilePath pFunc,CBSetCurrentSimResult pSimFunc,CBCScheduleStand pScheduleFunc,const CString& strPorjectPath,reportType _reportType,CParameters * parameter )
{
	if (parameter == NULL)
		return;
	
	std::vector<int> vReportRuns;

	if(!parameter->GetReportRuns(vReportRuns))
		return;

	AddReportWhatToGen(_reportType,parameter);
	m_mapMutiRunResult[_reportType]->ClearSimReport();
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
		reportAgent.SetCBSecheduleStand(pScheduleFunc);
		CAirsideBaseReport* pReport = reportAgent.AddReportWhatToGen(_reportType,parameter, pTerminal);
		reportAgent.GenerateReport(pReport,parameter);

		m_mapMutiRunResult[_reportType]->AddSimReport(strSimResultFolderName,pReport);
	}
	
	m_mapMutiRunResult[_reportType]->LoadMultipleRunReport(parameter);
	//save result to default folder
	CString strFilePath;
	strFilePath.Format(_T("%s\\SimResult\\AirsideMultipleRunReport\\%s"),strPorjectPath,m_mapMutiRunResult[_reportType]->GetReportFileName());
	if (PathFileExists(strFilePath) == FALSE)//doesn't exist
	{
		CString strFileName = m_mapMutiRunResult[_reportType]->GetReportFileName();
		int iPos = strFileName.ReverseFind('\\');
		if (iPos != -1)
		{
			CString strSubFolder = strFileName.Left(iPos);
			CString strDefaultFolder;
			strDefaultFolder.Format(_T("%s\\SimResult\\AirsideMultipleRunReport\\%s"),strPorjectPath,strSubFolder);
			if (FileManager::IsDirectory(strDefaultFolder) == 0)
			{
				FileManager::MakePath(strDefaultFolder);
			}
		}
	}

	ArctermFile _file;
	if (_file.openFile(strFilePath,WRITE))
	{
		m_mapMutiRunResult[_reportType]->WriteReportData(_file);
		_file.endFile();
	}
}

CString CAirsideMultipleRunReport::GetReportFilePath( reportType _reportType,const CString& strPorjectPath) 
{
	CString strFilePath;
	CAirsideMultipleRunResult* pMultRunResult = m_mapMutiRunResult[ _reportType ];
	if (pMultRunResult)
	{
		strFilePath.Format(_T("%s\\SimResult\\AirsideMultipleRunReport\\%s"),strPorjectPath,m_mapMutiRunResult[_reportType]->GetReportFileName());
	}
	return strFilePath;
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
		case Airside_TakeoffProcess:
			m_mapMutiRunResult[_reportType] = new CAirsideAircraftMultiRunTakeoffProcessResult;
			break;
        case Airside_RunwayOperaitons:
            m_mapMutiRunResult[_reportType] = new CAirsideAircraftMutiRunRunwayOperationResult;
            break;
		case Airside_AircraftOperational:
			m_mapMutiRunResult[_reportType] = new CAirsideFlightOperationalMultiRunResult;
			break;
        case Airside_RunwayDelay:
            m_mapMutiRunResult[_reportType] = new CAirsideRunwayDelayMultiRunResult;
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

BOOL CAirsideMultipleRunReport::WriteReportData( reportType _reportType,ArctermFile& _file )
{
	AddReportWhatToGen(_reportType,NULL);
	CAirsideMultipleRunResult* pMultRunResult = m_mapMutiRunResult[ _reportType ];
	if (pMultRunResult )
	{
		pMultRunResult->WriteReportData(_file);
	}
	return TRUE;
}

BOOL CAirsideMultipleRunReport::ReadReportData( reportType _reportType,ArctermFile& _file )
{
	AddReportWhatToGen(_reportType,NULL);
	CAirsideMultipleRunResult* pMultRunResult = m_mapMutiRunResult[ _reportType ];
	if (pMultRunResult )
	{
		pMultRunResult->ReadReportData(_file);
	}
	return TRUE;
}
