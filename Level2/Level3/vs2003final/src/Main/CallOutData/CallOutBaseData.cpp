#include "StdAfx.h"
#include ".\calloutbasedata.h"
CCallOutBaseData::CCallOutBaseData(const ElapsedTime& _startTime,const ElapsedTime& _endtime,const ElapsedTime& _interval,CTermPlanDoc* _Doc)
:m_RefreshTime(1),m_StartTime(_startTime),m_EndTime(_endtime),m_IntervalTime(_interval),m_PBaseDlg(NULL),m_TermDoc(_Doc)
{
	m_ChartType = CGLChartCtrl::TY_line ;
}

CCallOutBaseData::~CCallOutBaseData()
{
	if(m_PBaseDlg)
	{
		m_PBaseDlg->DestroyWindow() ;
		delete m_PBaseDlg ;
	}
}
void CCallOutBaseData::SetRefreshTime(int _MilSec)
{
	m_RefreshTime = _MilSec ;
}
int CCallOutBaseData::GetRefreshTime()
{
	return m_RefreshTime ;
}
void CCallOutBaseData::SetChartType(CGLChartCtrl::CHART_TYPE _type)
{
	m_ChartType = _type ;
}
CGLChartCtrl::CHART_TYPE CCallOutBaseData::GetChartType()
{
	return m_ChartType ;
}
ElapsedTime CCallOutBaseData::GetStartTime()
{
	return m_StartTime ;
}
ElapsedTime CCallOutBaseData::GetEndTime()
{
	return m_EndTime ;
}
ElapsedTime CCallOutBaseData::GetIntervalTime()
{
	return m_IntervalTime ;
}
void CCallOutBaseData::SetDlgCaption(const CString& _cap)
{
	m_Caption = _cap ;
}
BOOL CCallOutBaseData::DlgIsClosed()
{
	if(m_PBaseDlg)
		return m_PBaseDlg->IsClose() ;
	return FALSE ;
}
void CCallOutBaseData::DoModel()
{
	if(m_PBaseDlg)
		delete m_PBaseDlg ;
	m_PBaseDlg = new CBaseCallOutDlg(m_TermDoc) ;

	GetChartCtrl()->SetXFormater(&m_TimeFormater) ;
	if(m_PBaseDlg && m_PBaseDlg->GetSafeHwnd() == NULL)
	{
		InitOriginalChartData() ; // init the original data 
		InitChartAxisData() ;	//init the chart axis message 
		m_PBaseDlg->SetCaption(m_Caption);
		m_PBaseDlg->SetPosition(m_DlgPoint) ;
		m_PBaseDlg->SetStartTime(m_StartTime) ;
		m_PBaseDlg->SetEndTime(m_EndTime) ;
		m_PBaseDlg->SetIntervalTime(m_IntervalTime) ;
		m_PBaseDlg->SetProcessorName(m_Processor) ;
		m_PBaseDlg->Create(IDD_DIALOG_CALLOUT) ; // create the dialog.
		m_PBaseDlg->ShowWindow(SW_SHOW) ;
	}
}

void CCallOutBaseData::DestroyCallOutDialog()
{
	if(m_PBaseDlg)
	{
		m_PBaseDlg->DestroyWindow() ;
		delete m_PBaseDlg ;
	}
	m_PBaseDlg = NULL ;
}

void CCallOutBaseData::ShowBaseDialog(BOOL _res)
{
	if(m_PBaseDlg)
	{
		if(_res == TRUE)
			m_PBaseDlg->ShowWindow(SW_SHOW) ;
		else
			m_PBaseDlg->ShowWindow(SW_HIDE) ;
	}
}


void CCallOutBaseData::NoticeTheDialogUpdateTheTime(const ElapsedTime& _time)
{
	if(m_PBaseDlg == NULL)
		return ;
	if(_time >= m_EndTime || _time < m_StartTime)
		return ;

	GetChartCtrl()->SetActiveXVal(_time.asSeconds()) ;
	m_PBaseDlg->UpdateCurrentTime(_time) ;
	m_PBaseDlg->SendMessage(UPDATE_TIME,0,0) ;
	if(_time <= m_LastRefreshTime && _time.asSeconds() != 0)
	{
		GetChartCtrl()->UpdateData() ;
		GetChartCtrl()->Invalidate(FALSE) ;
		return ;
	}
	RefreshChartData(_time) ;

	m_LastRefreshTime = _time ;
}

void CCallOutBaseData::InitChartAxisData()
{
	CGLChartCtrl* PChartCtrl = GetChartCtrl() ;
	ASSERT(PChartCtrl) ;
	if(PChartCtrl == NULL)
		return ;
	//set axis X range 
	PChartCtrl->SetAxisXValueScale(m_StartTime.asSeconds(),m_EndTime.asSeconds()) ;

	PChartCtrl->SetAxisXInterval(m_IntervalTime.asSeconds()) ;
	//
	//set column 
	PChartCtrl->SetDataItemCount((int)m_ColumnNames.size()) ;
	for (int i = 0 ; i < (int)m_ColumnNames.size() ;i++)
	{
		PChartCtrl->SetDataItemName(i,m_ColumnNames[i]) ;
	}
	//set Chart Type 
	PChartCtrl->SetChartType(m_ChartType) ;

}

CGLChartCtrl* CCallOutBaseData::GetChartCtrl()
{
		if(m_PBaseDlg)
			return m_PBaseDlg->GetChartCtrl() ;
		return NULL ;
}

CRect CCallOutBaseData::GetCurrentDlgPosition()
{
	CRect _rect ;
	if(m_PBaseDlg)
		return m_PBaseDlg->GetPositionRect() ;
	return _rect ;
}