#include "StdAfx.h"
#include ".\airsideflightdelayparam.h"

CAirsideFlightDelayParam::CAirsideFlightDelayParam(void)
{
	m_nSubType = 0;
}

CAirsideFlightDelayParam::~CAirsideFlightDelayParam(void)
{
}
void CAirsideFlightDelayParam::LoadParameter()
{

}
void CAirsideFlightDelayParam::UpdateParameter()
{


}
CAirsideReportNode CAirsideFlightDelayParam::GetStartObject()
{
	return m_startNode;
}
CAirsideReportNode CAirsideFlightDelayParam::GetEndObject()
{
	return m_endNode;
}

BOOL CAirsideFlightDelayParam::ExportFile(ArctermFile& _file)
{
	if(!CParameters::ExportFile(_file))
		return FALSE ;

	m_startNode.ExportFile(_file) ;

	m_endNode.ExportFile(_file) ;

	_file.writeInt(m_nSubType) ;
	_file.writeLine() ;
	return TRUE ;
}
BOOL CAirsideFlightDelayParam::ImportFile(ArctermFile& _file)
{
	if(!CParameters::ImportFile(_file))
		return FALSE ;

	m_startNode.ImportFile(_file) ;

	_file.getLine();
	m_endNode.ImportFile(_file) ;

	_file.getInteger(m_nSubType) ;
	_file.getLine() ;
    
	return TRUE ;
}