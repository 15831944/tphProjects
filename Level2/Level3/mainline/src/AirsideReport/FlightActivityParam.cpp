#include "StdAfx.h"
#include ".\flightactivityparam.h"

CFlightActivityParam::CFlightActivityParam(void)
:m_nSubType(0)
{
	AddFlightConstraint(FlightConstraint());
}

CFlightActivityParam::~CFlightActivityParam(void)
{
}
void CFlightActivityParam::LoadParameter()
{

}
//void CFlightActivityParam::InitParameter(CXTPPropertyGrid * pGrid)
//{
//
//	InitTimeRange(pGrid);
//	InitInterval(pGrid);
//}

CString CFlightActivityParam::GetReportParamName()
{
	return  _T("FlightActivity\\FlightActivity.prm");
}

void CFlightActivityParam::UpdateParameter()
{

}

BOOL CFlightActivityParam::ExportFile(ArctermFile& _file)
{
	if(!CParameters::ExportFile(_file))
		return FALSE ;
	_file.writeInt(m_nSubType) ;
	_file.writeLine() ;
	return TRUE ;
}
BOOL CFlightActivityParam::ImportFile(ArctermFile& _file)
{
	if(!CParameters::ImportFile(_file))
		return FALSE ;
	_file.getLine() ;
	_file.getInteger(m_nSubType) ;
	return TRUE ;
}
