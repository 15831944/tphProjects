#include "StdAfx.h"
#include ".\airsideflightburnparmater.h"


CString CAirsideFlightFuelBurnParmater::GetReportParamName()
{
	return  _T("FlightFuelBurn\\FlightFuelBurn.prm");
}

void CAirsideFlightFuelBurnParmater::WriteParameter(ArctermFile& _file)
{
	CParameters::WriteParameter(_file);
	
	m_FromNode.ExportFile(_file);
	m_ToNode.ExportFile(_file);
}

void CAirsideFlightFuelBurnParmater::ReadParameter(ArctermFile& _file)
{
	CParameters::ReadParameter(_file);
	m_FromNode.ImportFile(_file);
	_file.getLine() ;
	m_ToNode.ImportFile(_file);	
}

BOOL CAirsideFlightFuelBurnParmater::ExportFile(ArctermFile& _file)
{
	if(!CParameters::ExportFile(_file))
		return FALSE;

	m_FromNode.ExportFile(_file);
	m_ToNode.ExportFile(_file);
	
	return TRUE;
}

BOOL CAirsideFlightFuelBurnParmater::ImportFile(ArctermFile& _file)
{
	if(!CParameters::ImportFile(_file))
		return FALSE;

	m_FromNode.ImportFile(_file);
	_file.getLine() ;
	m_ToNode.ImportFile(_file);

	return TRUE;
}
