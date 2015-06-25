#include "StdAfx.h"
#include ".\airsideflightconflictreport.h"
#include "AirsideFlightConflictBaseResult.h"
#include "AirsideFlightConflictDetailResult.h"
#include "AirsideFlightConflictPara.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictSummaryResult.h"
#include "LogFilterFun.h"

AirsideFlightConflictReport::AirsideFlightConflictReport(CBGetLogFilePath pFunc,AirsideFlightConflictPara* parameter)
:CAirsideBaseReport(pFunc)
,m_pParameter(parameter)
{
	m_pBaseResult = NULL;
	m_pReportConflictData = NULL;

	CAreaFilterFun * pFilterFun = new CAreaFilterFun(parameter);

	m_pReportConflictData = new FlightConflictReportData(parameter,pFilterFun);

}

AirsideFlightConflictReport::~AirsideFlightConflictReport(void)
{
	if (m_pBaseResult)
	{
		delete m_pBaseResult;
		m_pBaseResult = NULL;
	}


	if (m_pReportConflictData)
	{
		delete m_pReportConflictData;
		m_pReportConflictData = NULL;
	}

}

void AirsideFlightConflictReport::GenerateReport( CParameters * parameter )
{
	ASSERT(parameter != NULL);
	if(parameter == NULL)
		return;

	//delete old result if exist
	delete m_pBaseResult;
	m_pBaseResult = NULL;

	delete m_pReportConflictData;
	m_pReportConflictData = NULL;

	CAreaFilterFun * pFilterFun = new CAreaFilterFun(parameter);
	m_pReportConflictData = new FlightConflictReportData(parameter,pFilterFun);


	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	m_pReportConflictData->LoadData(strDescFilepath,strEventFilePath);


	if(parameter->getReportType() == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new AirsideFlightConflictDetailResult(parameter);
	}
	else
	{	//summary report
		m_pBaseResult = new AirsideFlightConflictSummaryResult(parameter);
	}

	if(m_pBaseResult)
	{
		m_pBaseResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer
		m_pBaseResult->SetAirportDB(m_AirportDB);//
		m_pBaseResult->SetReportData(m_pReportConflictData);
		SaveReportData();
	}

}

void AirsideFlightConflictReport::RefreshReport( CParameters * parameter )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->RefreshReport( parameter );
	}

}

int AirsideFlightConflictReport::GetReportType()
{
	return Airside_FlightConflict;
}

BOOL AirsideFlightConflictReport::ExportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType)
{
	return FALSE;
}

BOOL AirsideFlightConflictReport::ImportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType)
{
	return FALSE;
}

void AirsideFlightConflictReport::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->InitListHead(cxListCtrl, piSHC);
	}
}

void AirsideFlightConflictReport::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->FillListContent(cxListCtrl);
	}
}

CAirsideReportBaseResult* AirsideFlightConflictReport::GetReportResult()
{	
	if(m_pBaseResult)
	{
		return m_pBaseResult->GetReportResult();
	}
	return NULL;

}

CString AirsideFlightConflictReport::GetReportFileName()
{
	return _T("FlightConflicts\\FlightConflicts.rep");
}

BOOL AirsideFlightConflictReport::WriteReportData( ArctermFile& _file )
{
	if(m_pBaseResult)
	{
		_file.writeField("Flight Conflict Report");
		_file.writeLine();

		m_pParameter->WriteParameter(_file);
		return m_pBaseResult->WriteReportData(_file);
		
	}
	return FALSE;
}

BOOL AirsideFlightConflictReport::ReadReportData( ArctermFile& _file )
{
	if(m_pBaseResult != NULL)
		delete m_pBaseResult;
	m_pBaseResult = NULL;

	//_file.getLine();
	m_pParameter->ReadParameter(_file);

	int nDetailOrSummary = m_pParameter->getReportType();
	if(nDetailOrSummary == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new AirsideFlightConflictDetailResult(m_pParameter);
	}
	else
	{	//summary report
		m_pBaseResult = new AirsideFlightConflictSummaryResult(m_pParameter);		
	}

	if(m_pBaseResult)
	{
		m_pBaseResult->setLoadFromFile(true);
		m_pBaseResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer
		m_pBaseResult->SetAirportDB(m_AirportDB);//
		m_pBaseResult->SetReportData(m_pReportConflictData);
		return m_pBaseResult->ReadReportData(_file);

	}

	return FALSE;
}
