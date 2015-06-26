#include "StdAfx.h"
#include ".\lsvehicleactivityreport.h"
#include "LSVehicleActivityParam.h"
#include "LSVehicleActivityResult.h"
#include "../Common/Termfile.h"



LSVehicleActivityReport::LSVehicleActivityReport(void)
{
	m_pParemeter = new LSVehicleActivityParam;
}

LSVehicleActivityReport::~LSVehicleActivityReport(void)
{
}

void LSVehicleActivityReport::GenerateReport( CBGetLogFilePath pFunc, InputLandside *pLandside)
{
	if(m_pParemeter == NULL)
		return;
	delete	m_pResult;
	m_pResult = NULL;


	//only detail report
	m_pResult = new LSVehicleActivityResult;




	if(m_pResult == NULL)
		return;

	m_pResult->GenerateResult(pFunc, m_pParemeter,pLandside);
}

int LSVehicleActivityReport::GetReportType() const
{
	return LANDSIDE_VEHICLE_ACTIVITY;
}

CString LSVehicleActivityReport::GetReportFileName()
{
	return _T("\\LandsideVehicleActivity\\Report.rep");
}

void LSVehicleActivityReport::RefreshReport()
{

}

BOOL LSVehicleActivityReport::WriteReportData( ArctermFile& _file )
{
	ASSERT(m_pParemeter != NULL);
	if(m_pParemeter == NULL)
		return FALSE;


	//header
	CString strColumn = _T("ID, Type, Entry Time, Exit Time, Max Speed, Distance, Pax Load");
	_file.writeField(strColumn);
	_file.writeLine();


	if(m_pResult)
		m_pResult->WriteReportData(_file);

	return TRUE;
}

BOOL LSVehicleActivityReport::ReadReportData( ArctermFile& _file )
{
	//clear result data
	delete m_pResult;
	m_pResult = NULL;

	m_pResult = new LSVehicleActivityResult;

	m_pResult->ReadReportData(_file);
	return TRUE;
}