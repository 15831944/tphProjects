#include "StdAfx.h"
#include ".\airsideintersectionreportparam.h"

CAirsideIntersectionReportParam::CAirsideIntersectionReportParam(void)
{
	m_nSummaryNodeID = -1;
	m_bAllTaxiway = true;
}

CAirsideIntersectionReportParam::~CAirsideIntersectionReportParam(void)
{
	ClearTaxiwayItems();
}

void CAirsideIntersectionReportParam::WriteParameter( ArctermFile& _file )
{
	CParameters::WriteParameter(_file);

	m_vTaxiwayItems.WriteParameter(_file);
}

void CAirsideIntersectionReportParam::ReadParameter( ArctermFile& _file )
{
	CParameters::ReadParameter(_file);
	
	m_vTaxiwayItems.ReadParameter(_file);
}

CString CAirsideIntersectionReportParam::GetReportParamName()
{
	return _T("IntersectionReport\\IntersectionReport.prm");
}

void CAirsideIntersectionReportParam::LoadParameter()
{

}

void CAirsideIntersectionReportParam::UpdateParameter()
{

}

bool CAirsideIntersectionReportParam::IsNodeSelected( int nNodeID )
{
	return m_vTaxiwayItems.IsNodeSelected(nNodeID);
}

BOOL CAirsideIntersectionReportParam::ExportFile( ArctermFile& _file )
{
	WriteParameter(_file);
	return TRUE;
}

BOOL CAirsideIntersectionReportParam::ImportFile( ArctermFile& _file )
{
	ReadParameter(_file);
	return TRUE;
}