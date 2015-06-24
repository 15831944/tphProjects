#pragma once
#include "..\..\Common\elaptime.h"
#include "CallOutBaseData.h"
#include "..\..\Common\TERM_BIN.H"
#include "..\..\Inputs\MobileElemConstraint.h"
class ProcLogEntry ;
class Terminal ;

class CLogDataSet
{
public:
	int m_CurrentNdx ; // the current index which time goes to in Prolog.
	std::vector<ProcEventStruct> m_ProLog ;
	int m_ThroughtPut ;
	CString m_ProID ;
	CLogDataSet():m_CurrentNdx(0),m_ThroughtPut(0) {} ;
	~CLogDataSet();
};
class AxisYFormater :public CFromatValueString
{
public:

	virtual CString Format(double dVal)const
	{
		int val = (int)dVal ;
		CString strval ;
		strval.Format(_T("%d"),val) ;
		return strval ;
	}
};
class CCallOutProcessorThroughPutData : public CCallOutBaseData
{
public:
	CCallOutProcessorThroughPutData
		(const ElapsedTime& _startTime,
		const ElapsedTime& _endtime,
		const ElapsedTime& _interval,
		const ProcessorID& _Id,
		CTermPlanDoc* _Doc,
		CMobileElemConstraint& _FltTy ,
		BOOL _isFamily);
	~CCallOutProcessorThroughPutData(void);
protected:
	BOOL m_IsFamily ;
	CMobileElemConstraint m_FltTy;
	typedef std::map<int,BOOL> TY_MAP ;
	TY_MAP m_FltS ;
	int m_totalData ;
	AxisYFormater m_YFormat ;
protected:
	BOOL m_IntervalSet[1024] ;
	std::vector<CLogDataSet*> m_ProLog ;
public:
	virtual BOOL RefreshChartData(const ElapsedTime& _time) ; 
	virtual void InitOriginalChartData();
protected:
	int CalculatorTheThroughPut(const ElapsedTime& _time , CLogDataSet* _LogDataSet);
	void FilterTheLogByFlightType(ProcLogEntry* logEntry,CLogDataSet* DataSet);
	BOOL CheckTheEventFlightType(int _id);
	BOOL RefreshFamilyData(const ElapsedTime& _time,int ndx);
};
 