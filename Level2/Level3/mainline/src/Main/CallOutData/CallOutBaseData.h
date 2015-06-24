#pragma once

#include "..\..\Common\elaptime.h"
#include "..\resource.h"
#include <CommonData/procid.h>
#include "..\..\Main\TermPlanDoc.h"
#include "BaseCallOutDlg.h"
class TimerFormater :public CFromatValueString
{
public:

	virtual CString Format(double dVal)const
	{
		ElapsedTime _time ;
		_time.SetSecond((int)dVal) ;
		return _time.printTime() ;
	}
};

class CCallOutBaseData
{
public:
	CCallOutBaseData(const ElapsedTime& _startTime,const ElapsedTime& _endtime,const ElapsedTime& _interval,CTermPlanDoc* _Doc);
	virtual ~CCallOutBaseData(void);
protected:
	 ProcessorID m_ProID ; // the processor which been watched
	
	CTermPlanDoc* m_TermDoc ;
	int m_RefreshTime ; // the refresh frequency for chart, second
	CGLChartCtrl::CHART_TYPE m_ChartType ;	// the type chart will been shown.
	ElapsedTime m_LastRefreshTime ; // the last time which the chart refresh. use to control the refresh frequency
	
	CBaseCallOutDlg* m_PBaseDlg ; // the dialog for showing the chart.

	TimerFormater m_TimeFormater ; // the format for showing axis X 
	ElapsedTime m_AnimiationBeginTime ; // the time of animiation begin
	ElapsedTime m_StartTime ; //the start time
	ElapsedTime m_EndTime ;	// the end time
	ElapsedTime m_IntervalTime ; // the interval
	CString m_Caption ; //the caption for dialog
	CString m_Processor ; // the processor name
	CPoint m_DlgPoint ; // the position of dialog on the screen 
	std::vector<CString> m_ColumnNames ; //the Columns .such as the processor number for calculate 
	void InitChartAxisData(); //init chart axis X & Y ,Chart show Message
	

	virtual BOOL RefreshChartData(const ElapsedTime& _time) =0 ; //calculate the data for show in the chart at this time.if the value has no change ,return FALSE else return TUR
	
	virtual void InitOriginalChartData() = 0	;	// init the original data for RefreshChartData function use, 
						//this will been called before the animation start, so the data handle which cost more time should put in this function.
public:
	 CGLChartCtrl* GetChartCtrl() ;	//return the chart data.
	void SetRefreshTime(int _MilSec) ;
	int GetRefreshTime() ;
	
	void SetChartType(CGLChartCtrl::CHART_TYPE  _type) ;
	CGLChartCtrl::CHART_TYPE GetChartType() ;

	ElapsedTime GetStartTime() ;
	ElapsedTime GetEndTime() ;
	ElapsedTime GetIntervalTime() ;

	void SetDlgPosition(const CPoint& _point) { m_DlgPoint = _point ;} ;

	ProcessorID GetProcessorID() { return m_ProID ;}
 public:
	void DoModel() ;
	void DestroyCallOutDialog() ;
	void ShowBaseDialog(BOOL _res) ;

	void SetDlgCaption(const CString& _cap) ;
	
	//notice the chart refresh the data
	void NoticeTheDialogUpdateTheTime(const ElapsedTime& _time) ;
	
	void SetAnimiationBeginTime(const ElapsedTime& _time) { m_AnimiationBeginTime = _time ;} ;

	CRect GetCurrentDlgPosition() ;
	BOOL DlgIsClosed();
};
