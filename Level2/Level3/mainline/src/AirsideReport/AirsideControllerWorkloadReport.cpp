#include "StdAfx.h"
#include ".\airsidecontrollerworkloadreport.h"
#include "AirsideControllerWorkloadResult.h"
#include "AirsideControllerWorkloadParam.h"



CAirsideControllerWorkloadReport::CAirsideControllerWorkloadReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
,m_pResult(NULL)
{
}

CAirsideControllerWorkloadReport::~CAirsideControllerWorkloadReport(void)
{
}

void CAirsideControllerWorkloadReport::GenerateReport( CParameters * parameter )
{
	m_pParam = (CAirsideControllerWorkloadParam *)parameter;

	delete m_pResult;
	m_pResult = NULL;

	m_pResult = new AirsideControllerWorkload::CReportResult();
	m_pResult->GenerateResult(m_pCBGetLogFilePath, (CAirsideControllerWorkloadParam *)parameter);
}

void CAirsideControllerWorkloadReport::RefreshReport( CParameters * parameter )
{
	m_pParam = (CAirsideControllerWorkloadParam *)parameter;


}

int CAirsideControllerWorkloadReport::GetReportType()
{
return Airside_ControllerWorkload;
}

void CAirsideControllerWorkloadReport::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	if(m_pResult)
		m_pResult->FillListContent( cxListCtrl, (CAirsideControllerWorkloadParam *)parameter );

}

BOOL CAirsideControllerWorkloadReport::ExportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /*= ASReportType_Detail*/ )
{

	return FALSE;
}

BOOL CAirsideControllerWorkloadReport::ImportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /*= ASReportType_Detail*/ )
{

	return FALSE;
}
void CAirsideControllerWorkloadReport::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC/*=NULL*/ ) 
{

	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Time interval"), LVCFMT_LEFT, 200);	
	cxListCtrl.InsertColumn(1, _T("Total Index"), LVCFMT_LEFT, 100);

	int nIndex = 2;
	for (int weight = CAirsideControllerWorkloadParam::weight_PushBacks;
		weight < CAirsideControllerWorkloadParam::weight_count; ++weight)
	{
		if(m_pParam)
		{
			CString strWeightName = m_pParam->getWeightName((CAirsideControllerWorkloadParam::enumWeight)weight);
			
			cxListCtrl.InsertColumn(nIndex, strWeightName, LVCFMT_LEFT, 80);
			nIndex += 1;

			cxListCtrl.InsertColumn(nIndex, _T("Contribution"), LVCFMT_LEFT, 80);
			nIndex += 1;
		}
	}
}
