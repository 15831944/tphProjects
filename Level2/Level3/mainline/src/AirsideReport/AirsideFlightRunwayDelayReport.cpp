#include "StdAfx.h"
#include ".\airsideflightrunwaydelayreport.h"
#include "AirsideFlightRunwayDelayReportPara.h"
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelayBaseResult.h"
#include "DetailRunwayDelayResult.h"
#include "SummaryRunwayDelayResult.h"

AirsideFlightRunwayDelayReport::AirsideFlightRunwayDelayReport(CBGetLogFilePath pFunc, AirsideFlightRunwayDelayReportPara* parameter)
:CAirsideBaseReport(pFunc)
,m_pParameter(parameter)
{
	m_pBaseResult = NULL;
	m_pReportData = NULL;

	m_pReportData = new AirsideFlightRunwayDelayData((AirsideFlightRunwayDelayReportPara*)parameter);
}

AirsideFlightRunwayDelayReport::~AirsideFlightRunwayDelayReport(void)
{
	if (m_pBaseResult)
	{
		delete m_pBaseResult;
		m_pBaseResult = NULL;
	}

	if (m_pReportData)
	{
		delete m_pReportData;
		m_pReportData = NULL;
	}

}

void AirsideFlightRunwayDelayReport::GenerateReport( CParameters * parameter )
{
	ASSERT(parameter != NULL);
	if(parameter == NULL)
		return;

	//delete old result if exist
	if (m_pBaseResult)
	{
		delete m_pBaseResult;
		m_pBaseResult = NULL;
	}

	if (m_pReportData)
	{
		delete m_pReportData;
		m_pReportData = NULL;
	}

	m_pReportData = new AirsideFlightRunwayDelayData((AirsideFlightRunwayDelayReportPara*)parameter);

	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	m_pReportData->LoadData(strDescFilepath,strEventFilePath);


	if(parameter->getReportType() == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new DetailRunwayDelayResult(parameter);
	}
	else
	{	//summary report
		m_pBaseResult = new SummaryRunwayDelayResult(parameter);
	}

	if(m_pBaseResult)
	{
		m_pBaseResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer
		m_pBaseResult->SetAirportDB(m_AirportDB);//
		m_pBaseResult->SetReportData(m_pReportData);
		SaveReportData();
	}

}

void AirsideFlightRunwayDelayReport::RefreshReport( CParameters * parameter )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->RefreshReport( parameter );
	}

}

int AirsideFlightRunwayDelayReport::GetReportType()
{
	return Airside_RunwayDelay;
}

BOOL AirsideFlightRunwayDelayReport::ExportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType)
{
	return FALSE;
}

BOOL AirsideFlightRunwayDelayReport::ImportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType)
{
	return FALSE;
}

void AirsideFlightRunwayDelayReport::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	if(m_pBaseResult)
	{
		m_pBaseResult->InitListHead(cxListCtrl, piSHC);
	}
}

void AirsideFlightRunwayDelayReport::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->FillListContent(cxListCtrl);
	}
}

CAirsideReportBaseResult* AirsideFlightRunwayDelayReport::GetReportResult()
{	
	if(m_pBaseResult)
	{
		return m_pBaseResult->GetReportResult();
	}
	return NULL;

}

CString AirsideFlightRunwayDelayReport::GetReportFileName()
{
	return _T("RunwayDelays\\RunwayDelays.rep");
}

BOOL AirsideFlightRunwayDelayReport::WriteReportData( ArctermFile& _file )
{
	if(m_pBaseResult)
	{
		//title
		_file.writeField("Flight Runway Delay Report");
		_file.writeLine();

		m_pParameter->WriteParameter(_file);
		return m_pBaseResult->WriteReportData(_file);
	}
	return FALSE;
}

BOOL AirsideFlightRunwayDelayReport::ReadReportData( ArctermFile& _file )
{
	if(m_pBaseResult != NULL)
		delete m_pBaseResult;
	m_pBaseResult = NULL;

	//_file.getLine();
	m_pParameter->ReadParameter(_file);

	int nDetailOrSummary = m_pParameter->getReportType();
	if(nDetailOrSummary == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new DetailRunwayDelayResult(m_pParameter);
	}
	else
	{	//summary report
		m_pBaseResult = new SummaryRunwayDelayResult(m_pParameter);		
	}

	if(m_pBaseResult)
	{
		m_pBaseResult->setLoadFromFile(true);
		m_pBaseResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer
		m_pBaseResult->SetAirportDB(m_AirportDB);//
		m_pBaseResult->SetReportData(m_pReportData);
		return m_pBaseResult->ReadReportData(_file);
	}

	return FALSE;
}
