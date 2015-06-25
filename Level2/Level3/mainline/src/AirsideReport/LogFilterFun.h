#pragma once
#include "../Common/Types.h"
//the base filter function interface for FlightConflictReportData
class CParameters;
class ARCEventLog;
class CBaseFilterFun
{
public:
	CBaseFilterFun();
	virtual ~CBaseFilterFun();
	virtual BOOL bFit(class_type* ct,const ARCEventLog* _evenlog);
};


class CDefaultFilterFun :public CBaseFilterFun
{
public:
	CDefaultFilterFun(CParameters* _paramter);
	virtual ~CDefaultFilterFun();

	virtual BOOL bFit(class_type* ct,const ARCEventLog* _evenlog);
protected:
	CParameters* m_paramter ;	
};

class CAreaFilterFun :public CDefaultFilterFun
{
public:
	CAreaFilterFun(CParameters* _pParameter);
	~CAreaFilterFun();

	virtual BOOL bFit(class_type* ct,const ARCEventLog* _evenlog);
};
