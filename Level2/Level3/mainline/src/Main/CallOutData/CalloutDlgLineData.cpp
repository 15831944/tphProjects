#include "StdAfx.h"
#include ".\calloutdlglinedata.h"
#include "..\..\Inputs\IN_TERM.H"
#include "..\..\Engine\proclist.h"
#include "..\..\Engine\process.h"
#include "CallOutBaseData.h"
CCalloutDlgLineDataItem::CCalloutDlgLineDataItem(CCallOutBaseData* _dlgdata,Terminal* _terminal, const ProcessorID& _proID )
:m_CalloutBaseData(_dlgdata),m_Terminal(_terminal),m_proID(_proID)
{

}
void CCalloutDlgLineDataItem::InitTheToPoints()
{
	GroupIndex groupNdx = m_Terminal->GetProcessorList()->getGroupIndex(m_proID) ;
	Processor* pProcessor = NULL ;
	m_Lines.m_rect =  m_CalloutBaseData->GetCurrentDlgPosition() ;
	for (int ndx = groupNdx.start ; ndx <= groupNdx.end ;ndx++)
	{
		pProcessor = m_Terminal->GetProcessorList()->getProcessor(ndx) ;

		m_Lines.m_processorPoint.push_back(pProcessor->getServicePoint(0)) ;

	}
}
void CCalloutDlgLineDataItem::UpdateTheCurrentDlgPosition()
{
	m_Lines.m_rect = m_CalloutBaseData->GetCurrentDlgPosition() ;
}

BOOL CCalloutDlgLineDataItem::NeedDrawLine()
{ 
	return !m_CalloutBaseData->DlgIsClosed() ;
} 

CCalloutDlgLineData::CCalloutDlgLineData()
{

}
CCalloutDlgLineData::~CCalloutDlgLineData(void)
{
	ClearAllTheData() ;
}
void CCalloutDlgLineData::RegisterTheDlg(CCallOutBaseData* _data,const ProcessorID& _proID,Terminal* _terminal)
{
	if(_data == NULL)
		return ;
	CCalloutDlgLineDataItem* DlgItemData = FindTheDlgLineDataItem(_data) ;
	if(DlgItemData == NULL)
	{
		DlgItemData = new CCalloutDlgLineDataItem(_data,_terminal,_data->GetProcessorID()) ;

		DlgItemData->InitTheToPoints() ;
		m_Data.push_back(DlgItemData) ;
	}
}

CCalloutDlgLineDataItem* CCalloutDlgLineData::GetTheDlgLineData(CCallOutBaseData* _data)
{	
	return FindTheDlgLineDataItem(_data) ;
}
void CCalloutDlgLineData::ClearAllTheData()
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;
}
CCalloutDlgLineDataItem* CCalloutDlgLineData::FindTheDlgLineDataItem(CCallOutBaseData* _data) 
{
	CCalloutDlgLineDataItem* DlgItemData = NULL ;
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		DlgItemData =  m_Data[i] ;
		if(DlgItemData->GetCallOutBaseData() == _data )
			return DlgItemData ;
	}
	return NULL ;
}