#include "StdAfx.h"
#include ".\lsresourcequeueparam.h"

LSResourceQueueParam::LSResourceQueueParam(void)
{
	m_lIntervalDistance = 10; 
}

LSResourceQueueParam::~LSResourceQueueParam(void)
{
}
CString LSResourceQueueParam::GetReportParamName()
{
	return  _T("\\LandsideResourceQueue\\Param.pac");
}