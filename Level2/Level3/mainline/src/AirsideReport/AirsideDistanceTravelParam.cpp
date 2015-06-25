#include "StdAfx.h"
#include "airsidedistancetravelparam.h"

using namespace ADODB;

CAirsideDistanceTravelParam::CAirsideDistanceTravelParam()
{
	m_lThreshold = 0L;
	m_enumUnit = 1;
	strcpy(m_modelName, "AM");
}

CAirsideDistanceTravelParam::~CAirsideDistanceTravelParam()
{

}
void CAirsideDistanceTravelParam::LoadParameter()
{
	return;
}

//void CAirsideDistanceTravelParam::InitParameter(CXTPPropertyGrid* pGrid)
//{
//	ASSERT(pGrid != NULL);
//
//
//	this->InitTimeRange(pGrid);
//	this->InitInterval(pGrid);
//
//	CXTPPropertyGridItem *pUnitsItem = pGrid->FindItem(_T("units"));
//	if (pUnitsItem != NULL)
//	{
//		long lunit = ((CXTPPropertyGridItemEnum *)pUnitsItem)->GetEnum();
//		if (lunit != GetUnit())
//		{
//			SetUnit(lunit);
//		}
//	}
//}

void CAirsideDistanceTravelParam::UpdateParameter()
{
	return;
}

bool CAirsideDistanceTravelParam::CreatePrameterTable()
{
	return false;
}

BOOL CAirsideDistanceTravelParam::ExportFile(ArctermFile& _file)
{
	if(!CParameters::ExportFile(_file) )
		return FALSE ;
	_file.writeInt(m_enumUnit) ;
	_file.writeInt(m_lThreshold) ;
	_file.writeLine() ;
	return TRUE ;
}
BOOL CAirsideDistanceTravelParam::ImportFile(ArctermFile& _file)
{
	if(!CParameters::ImportFile(_file))
		return FALSE ;
	_file.getLine() ;
	_file.getInteger(m_enumUnit) ;
	_file.getInteger(m_lThreshold) ;
	return TRUE ;
}






