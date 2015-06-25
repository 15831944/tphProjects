#include "StdAfx.h"
#include ".\logfilterfun.h"
#include "Parameters.h"
#include "../Results/AirsideFlightEventLog.h"
#include "AirsideFlightConflictPara.h"

CBaseFilterFun::CBaseFilterFun()
{

}

CBaseFilterFun::~CBaseFilterFun()
{

}

BOOL CBaseFilterFun::bFit(class_type* ct,const ARCEventLog* _evenlog)
{
	if (!_evenlog->IsA(ct))
		return FALSE ;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////
CDefaultFilterFun::CDefaultFilterFun(CParameters* _paramter)
:CBaseFilterFun()
,m_paramter(_paramter)
{

}

CDefaultFilterFun::~CDefaultFilterFun()
{

}

BOOL CDefaultFilterFun::bFit(class_type* ct,const ARCEventLog* _evenlog)
{
	if (CBaseFilterFun::bFit(ct,_evenlog) == FALSE)
		return FALSE;

	AirsideConflictionDelayLog* pConflictLog = (AirsideConflictionDelayLog*)_evenlog;
	ElapsedTime tTime ;
	tTime.setPrecisely(_evenlog->time);
	if (tTime < m_paramter->getStartTime() || tTime > m_paramter->getEndTime())
		return FALSE ;

	return TRUE ;
}
///////////////////////////////////////////////////////////////////////////////////////
CAreaFilterFun::CAreaFilterFun(CParameters* _paramter)
:CDefaultFilterFun(_paramter)
{

}

CAreaFilterFun::~CAreaFilterFun()
{

}

BOOL CAreaFilterFun::bFit(class_type* ct,const ARCEventLog* _evenlog)
{
	if (CDefaultFilterFun::bFit(ct,_evenlog) == FALSE)
		return FALSE;

	ALTObjectID objName(((AirsideConflictionDelayLog*)_evenlog)->sAreaName.c_str());
	if (!((AirsideFlightConflictPara*)m_paramter)->IsAreaFit(objName))
		return FALSE;

	return TRUE;
}
