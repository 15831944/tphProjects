#include "StdAfx.h"
#include ".\calloutprocessorthroughputdata.h"
#include "..\..\Results\procntry.h"
#include "..\..\Results\proclog.h"
#include "..\..\Engine\terminal.h"
#include "..\..\Results\Outpax.h"
#include "BaseCallOutDlg.h"
#include "Common\STATES.H"
CCallOutProcessorThroughPutData::CCallOutProcessorThroughPutData
(const ElapsedTime& _startTime,
 const ElapsedTime& _endtime,
 const ElapsedTime& _interval,
 const ProcessorID& _Id,
 CTermPlanDoc* _Doc,
 CMobileElemConstraint& _FltTy,
 BOOL _isFamily)
:CCallOutBaseData(_startTime,_endtime,_interval,_Doc),m_FltTy(_FltTy),m_IsFamily(_isFamily)
{
	m_ProID = _Id ;
	m_ProID.SetStrDict(m_TermDoc->GetTerminal().outStrDict) ;
	m_Caption = _T("Processor ThroughPut") ;
	m_Processor = m_ProID.GetIDString( ) ;
	memset(m_IntervalSet,0,sizeof(BOOL)*125) ;
	m_totalData = 0 ;
}
CCallOutProcessorThroughPutData::~CCallOutProcessorThroughPutData()
{
	for (int i = 0 ; i < (int)m_ProLog.size() ;i++)
	{
		delete m_ProLog[i] ;
	}
}
void CCallOutProcessorThroughPutData::InitOriginalChartData()
{
	m_ProLog.clear() ;
	int procCount = m_TermDoc->GetTerminal().procLog->getCount();

	ProcLogEntry logEntry ;
	logEntry.SetOutputTerminal(&(m_TermDoc->GetTerminal()));
	logEntry.SetEventLog(m_TermDoc->GetTerminal().m_pProcEventLog);
	for (int i = 0; i < procCount; i++)
	{
		m_TermDoc->GetTerminal().procLog->getItem (logEntry, i);
		int idList[MAX_PROC_IDS];
		(logEntry).getIDs (idList);
		ProcessorID id;
		id.SetStrDict(m_TermDoc->GetTerminal().outStrDict) ;
		id.copyIDlist (idList);
		if(m_ProID.idFits(id))
		{
			CLogDataSet* DataSet = new CLogDataSet ;
			DataSet->m_ProID = id.GetIDString() ;
			FilterTheLogByFlightType(&logEntry,DataSet) ;
			m_ProLog.push_back(DataSet) ;
		}
	}
	if(m_IsFamily)
		m_ColumnNames.push_back(m_Processor) ;
	else
	{
		for (int ndx = 0 ; ndx < (int) m_ProLog.size() ; ndx++)
		{
			m_ColumnNames.push_back(m_ProLog[ndx]->m_ProID) ;
		}
	}
	m_totalData = 0 ;

	GetChartCtrl()->SetYFormater(&m_YFormat) ;
}
void CCallOutProcessorThroughPutData::FilterTheLogByFlightType(ProcLogEntry* logEntry,CLogDataSet* DataSet)
{
	for (int i = 0 ; i < logEntry->getCount() ;i++)
	{
		ProcEventStruct _ProEventItem ;
		_ProEventItem = logEntry->getEvent(i) ;

		if(CheckTheEventFlightType(_ProEventItem.element))
		{
			DataSet->m_ProLog.push_back(_ProEventItem) ;
		}
	}
	ElapsedTime _ProTime ;
	for (int i = 0 ; i < (int)DataSet->m_ProLog.size() ;i++)
	{
		ProcEventStruct _ProEventItem ;
		_ProEventItem = DataSet->m_ProLog[i] ;
		_ProTime.setPrecisely(_ProEventItem.time) ;
		if(_ProTime >= m_AnimiationBeginTime)
		{
			DataSet->m_CurrentNdx = i ;
			break ;
		}
	}
}
BOOL  CCallOutProcessorThroughPutData::CheckTheEventFlightType(int _id)
{
	TY_MAP::iterator iter = m_FltS.find(_id) ;
	if(iter != m_FltS.end())
	{
		return (*iter).second ;
	}
	OutputPaxEntry element;
	element.SetOutputTerminal(&m_TermDoc->GetTerminal()) ;
	long lPaxCount = m_TermDoc->GetTerminal().paxLog->getCount();
	for (long lPax = 0; lPax < lPaxCount; lPax++)
	{
		m_TermDoc->GetTerminal().paxLog->getItem (element, lPax);//pax elemen
		if(element.getID() != _id)
			continue ;
		if(element.fits(m_FltTy)== 0)
			m_FltS[_id] = FALSE ;
		else
			m_FltS[_id] = TRUE ;
		return m_FltS[_id] ;
	}
	return FALSE ;
}


BOOL CCallOutProcessorThroughPutData::RefreshChartData(const ElapsedTime& _time)
{
	int ndx = 0;
	ndx = (_time.asSeconds() - m_StartTime.asSeconds()) /m_IntervalTime.asSeconds() ;
	if(ndx > 1024)
		return FALSE;
	if(m_IsFamily)
		RefreshFamilyData(_time,ndx) ;
	else
	{
		int PData = 0 ;
		for (int i = 0 ; i < (int)m_ProLog.size() ;i++)
		{
			if(m_IntervalSet[ndx] == FALSE )
			{

				m_ProLog[i]->m_ThroughtPut = 0 ;
				GetChartCtrl()->AddPoint(m_StartTime.asSeconds() + ndx *m_IntervalTime.asSeconds(),0,i,TRUE) ;

			}
			if(_time.asSeconds()% m_IntervalTime.asSeconds() != 0)
			{
				PData = CalculatorTheThroughPut(_time,m_ProLog[i]) ;
				GetChartCtrl()->AddPoint(_time.asSeconds(),PData,i,FALSE) ;
			}
		}
	}
	if(!m_IntervalSet[ndx])
		m_IntervalSet[ndx] =TRUE ;
	m_PBaseDlg->SendMessage(UPDATE_CHART,0,0) ;
	//m_PBaseDlg->Invalidate(FALSE) ;
	return TRUE ;
}
BOOL CCallOutProcessorThroughPutData::RefreshFamilyData(const ElapsedTime& _time,int ndx)
{
	m_totalData = 0 ;
	if(m_IntervalSet[ndx] != FALSE )
	{
		for (int i = 0 ; i < (int)m_ProLog.size() ;i++)
		{

			if(_time.asSeconds()% m_IntervalTime.asSeconds() != 0)
			{
				m_totalData += CalculatorTheThroughPut(_time,m_ProLog[i]) ;
			}
		}
		GetChartCtrl()->AddPoint(_time.asSeconds(),m_totalData,0,FALSE) ;
	}else
	{
		for (int i = 0 ; i < (int)m_ProLog.size() ;i++)
		{

			m_ProLog[i]->m_ThroughtPut = 0 ;
		}
		GetChartCtrl()->AddPoint(_time.asSeconds(),m_totalData,0,TRUE) ;
	}
	
	return TRUE ;
}
int CCallOutProcessorThroughPutData::CalculatorTheThroughPut(const ElapsedTime& _time , CLogDataSet* _LogDataSet)
{
	if(_LogDataSet == NULL)
		return 0 ;
	int _CurrentNdx = _LogDataSet->m_CurrentNdx ;

	ProcEventStruct _ProEventItem ;
	ElapsedTime _ProTime ;
	for ( ; _CurrentNdx < (int)_LogDataSet->m_ProLog.size() ; _CurrentNdx++)
	{
		_ProEventItem = _LogDataSet->m_ProLog.at(_CurrentNdx) ;

		_ProTime.setPrecisely(_ProEventItem.time) ;
		if(_ProTime <= _time)
		{
			if(_ProEventItem.type == BeginService )
				_LogDataSet->m_ThroughtPut++ ;
			if(_CurrentNdx + 1 == (int)_LogDataSet->m_ProLog.size())
				_LogDataSet->m_CurrentNdx = _CurrentNdx+1 ;
		}else
		{
			_LogDataSet->m_CurrentNdx = _CurrentNdx ;
			break ;
		}
	}
	return _LogDataSet->m_ThroughtPut ;
}

CLogDataSet::~CLogDataSet()
{

}