#include "StdAfx.h"
#include ".\landsidereportmanager.h"
#include "LandsideBaseReport.h"
#include "MFCExControl/XListCtrl.h"
#include "../Common/termfile.h"
#include "LandsideBaseParam.h"
#include "../Results/paxlog.h"
#include "LSVehicleActivityReport.h"
#include "LSVehicleDelayReport.h"
#include "LSResourceQueueReport.h"
#include "LSVehicleThroughputReport.h"
#include "LSVehicleTotalDelayReport.h"




LandsideReportManager::LandsideReportManager(void)
{
	m_emReportType	= LANDSIDE_REPOERT_UNKOWNTYPE;
	m_pReport		= NULL;
	m_pCommonData	= NULL;
}

LandsideReportManager::~LandsideReportManager(void)
{
}
void LandsideReportManager::SetReportType( LandsideReportType rpType )
{
	m_emReportType =rpType;
}

LandsideReportType LandsideReportManager::GetReportType() const
{
	return m_emReportType;
}

void LandsideReportManager::SetCBGetLogFilePath( CBGetLogFilePath pFunc )
{
	m_pGetLogFilePath = pFunc;
}

CBGetLogFilePath LandsideReportManager::GetCBGetLogFilePath()
{
	return m_pGetLogFilePath;
}

void LandsideReportManager::SetReportPath( const CString& strReportPath )
{
	m_strReportFilePath = strReportPath;
}
//---------------------------------------------------------------------------------
//Summary:
//		onboard flight for generate result for all onboard report
//Parameter:
//		pOnBoardList[in]: user define onboard flight
//---------------------------------------------------------------------------------
void LandsideReportManager::InitUpdate( int nProjID )
{
	//clear old data
	if (m_pReport)
	{
		delete m_pReport;
		m_pReport = NULL;
	}

	switch (m_emReportType)
	{
	case LANDSIDE_VEHICLE_ACTIVITY:
		{
			m_pReport = new LSVehicleActivityReport;
		}
		break;
	case LANDSIDE_VEHICLE_DELAY:
		m_pReport = new LSVehicleDelayReport;
		break;
	case LANDSIDE_VEHICLE_TOTALDELAY:
		m_pReport = new LSVehicleTotalDelayReport;
		break;
	case  LANDSIDE_RESOURCE_QUEUELEN:
		m_pReport =new LSResourceQueueReport;
		break;
	case LANDSIDE_RESOURCE_THROUGHPUT:
		m_pReport = new LSVehicleThroughputReport;
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}

	if (m_pReport)
	{
		m_pReport->Initialize(m_pCommonData,m_strReportFilePath);
		m_pReport->LoadReportData();
	}
}

CString LandsideReportManager::GetCurrentReportName() const
{
	LandsideReportType rpType = GetReportType();
	if (rpType < LANDSIDE_REPORT_COUNT)
		return LandsideReportName[rpType];

	return "";
}

LandsideBaseParam * LandsideReportManager::GetParameters() const
{
	if(m_pReport != NULL)
	{
		return m_pReport->getParam();
	}

	return NULL;
}

//---------------------------------------------------------------------------------
//Summary:
//		retrieve current report
//--------------------------------------------------------------------------------
LandsideBaseReport * LandsideReportManager::GetReport()
{
	return m_pReport;
}

//---------------------------------------------------------------------------------
//Summary:
//		set list ctrl and fill
//---------------------------------------------------------------------------------
void LandsideReportManager::InitReportList( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	if (NULL == m_pReport)
	{
		return;
	}

	cxListCtrl.DeleteAllItems();
	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);


	m_pReport->InitListHead(cxListCtrl, piSHC);
}

void LandsideReportManager::SetReportListContent( CXListCtrl& cxListCtrl )
{
	if (NULL == m_pReport)
	{
		return;
	}
	cxListCtrl.DeleteAllItems();
	m_pReport->FillListContent(cxListCtrl);
}
//--------------------------------------------------------------------------------
BOOL LandsideReportManager::ExportListCtrlToCvsFile(ArctermFile& _file,CXListCtrl& cxListCtrl)
{
	//export the lisctrl head 
	int size = cxListCtrl.GetColumns() ;
	LVCOLUMN column ;
	column.mask = LVCF_TEXT ;
	TCHAR th[1024] = {0};
	column.pszText = th ;
	column.cchTextMax = 1024 ;
	for (int i = 0 ;i < size ;i++)
	{
		cxListCtrl.GetColumn(i,&column) ;
		_file.writeField(column.pszText) ;
	}
	_file.writeLine() ;
	return  ExportListData(_file,cxListCtrl) ;
}
BOOL LandsideReportManager::ExportListData(ArctermFile& _file,CXListCtrl& cxListCtrl) 
{
	CString val ; 
	for (int line = 0 ; line < cxListCtrl.GetItemCount() ;line++)
	{
		for (int colum = 0 ; colum < cxListCtrl.GetColumns() ; colum++)
		{
			val = cxListCtrl.GetItemText(line,colum) ;
			_file.writeField(val) ;
		}
		_file.writeLine() ;
	}
	return TRUE;
}

ProjectCommon * LandsideReportManager::GetCommonData() const
{
	return m_pCommonData;
}

void LandsideReportManager::SetCommonData( ProjectCommon * pCommon )
{
	m_pCommonData = pCommon;
}

void LandsideReportManager::GenerateReport(InputLandside *pLandside)
{
	if(GetParameters())
		GetParameters()->SaveParameterFile();


 	m_pReport->GenerateReport(m_pGetLogFilePath, pLandside);
	m_pReport->SaveReportData();
 	
}








