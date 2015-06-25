#include "stdafx.h"
#include "AirsideOperationParam.h"

CAirsideOperationParam::CAirsideOperationParam(void)
{
		AddFlightConstraint(FlightConstraint());
}

CAirsideOperationParam::~CAirsideOperationParam(void)
{
}

void CAirsideOperationParam::LoadParameter()
{
}

//void  CAirsideOperationParam::InitParameter(CXTPPropertyGrid * pGrid)
//{
//	//time range
//	InitTimeRange(pGrid);
//
//	//interval
//	InitInterval(pGrid);
//}

void  CAirsideOperationParam::UpdateParameter()
{
}
BOOL CAirsideOperationParam::ExportFile(ArctermFile& _file)
{
	return CParameters::ExportFile(_file) ;
}
BOOL CAirsideOperationParam::ImportFile(ArctermFile& _file)
{
	return CParameters::ImportFile(_file) ;
}

CString CAirsideOperationParam::GetReportParamName()
{
	return _T("AirsideOperation\\AirsideOperation.prm");
}

void CAirsideOperationParam::ReadParameter(ArctermFile& _file)
{
	CParameters::ReadParameter(_file);
}

void CAirsideOperationParam::WriteParameter(ArctermFile& _file)
{
	CParameters::WriteParameter(_file);
}