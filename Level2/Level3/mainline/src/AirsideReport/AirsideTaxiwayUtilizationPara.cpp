#include "StdAfx.h"
#include "AirsideTaxiwayUtilizationPara.h"

CAirsideTaxiwayUtilizationPara::CAirsideTaxiwayUtilizationPara()
:m_pCurrentSelectTaxiway(NULL)
{

}

CAirsideTaxiwayUtilizationPara::~CAirsideTaxiwayUtilizationPara()
{

}

void CAirsideTaxiwayUtilizationPara::LoadParameter()
{

}

void CAirsideTaxiwayUtilizationPara::UpdateParameter()
{

}

CString CAirsideTaxiwayUtilizationPara::GetReportParamName()
{
	return _T("TaxiwayUtilization\\TaxiwayUtilization.prm");
}

BOOL CAirsideTaxiwayUtilizationPara::ImportFile(ArctermFile& _file)
{
	if(!CParameters::ImportFile(_file))
		return FALSE;

	m_vTaxiwayItems.ReadParameter(_file);
	return TRUE;
}

BOOL CAirsideTaxiwayUtilizationPara::ExportFile(ArctermFile& _file)
{
	if(!CParameters::ExportFile(_file))
		return FALSE;
	m_vTaxiwayItems.WriteParameter(_file);
	return TRUE;
}

void CAirsideTaxiwayUtilizationPara::ReadParameter(ArctermFile& _file)
{
	CParameters::ReadParameter(_file);

	m_vTaxiwayItems.ReadParameter(_file);
}

void CAirsideTaxiwayUtilizationPara::WriteParameter(ArctermFile& _file)
{
	CParameters::WriteParameter(_file);

	m_vTaxiwayItems.WriteParameter(_file);
}
