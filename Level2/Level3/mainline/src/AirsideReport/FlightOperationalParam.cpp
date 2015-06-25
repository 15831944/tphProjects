#include "StdAfx.h"
#include ".\flightoperationalparam.h"

CFlightOperationalParam::CFlightOperationalParam(void)
: m_nSubType(0)
{
}

CFlightOperationalParam::~CFlightOperationalParam(void)
{
}

void CFlightOperationalParam::LoadParameter()
{

}

void CFlightOperationalParam::InitParameter(CXTPPropertyGrid * pGrid)
{

}

void CFlightOperationalParam::UpdateParameter()
{

}

CAirsideReportNode CFlightOperationalParam::GetStartObject()
{
	return m_startNode;
}

CAirsideReportNode CFlightOperationalParam::GetEndObject()
{
	return m_endNode;
}

CString CFlightOperationalParam::GetReportParamName()
{
	return  _T("FlightOperations\\FlightOperations.prm");
}

BOOL CFlightOperationalParam::ExportFile(ArctermFile& _file)
{
	if(!CParameters::ExportFile(_file))
		return FALSE ;
	_file.writeInt(m_nSubType) ;
	_file.writeLine() ;

	m_startNode.ExportFile(_file) ;
	m_endNode.ExportFile(_file) ;
	return TRUE ;

}
BOOL CFlightOperationalParam::ImportFile(ArctermFile& _file)
{
	if(!CParameters::ImportFile(_file))
		return FALSE ;

	_file.getInteger(m_nSubType) ;
	_file.getLine() ;

	m_startNode.ImportFile(_file) ;

	_file.getLine() ;
	m_endNode.ImportFile(_file) ;
	return TRUE ;
}

void CFlightOperationalParam::WriteParameter(ArctermFile& _file)
{
	CParameters::WriteParameter(_file);
	
	_file.writeInt(m_nSubType) ;
	_file.writeLine() ;

	m_startNode.ExportFile(_file) ;
	m_endNode.ExportFile(_file) ;
}

void CFlightOperationalParam::ReadParameter(ArctermFile& _file)
{
	CParameters::ReadParameter(_file);

	//_file.getLine() ;
	_file.getInteger(m_nSubType) ;
	_file.getLine() ;

	m_startNode.ImportFile(_file) ;

	_file.getLine(); 
	m_endNode.ImportFile(_file) ;
}