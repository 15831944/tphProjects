#include "StdAfx.h"
#include ".\calloutdatamanager.h"
#include "CallOutBaseData.h"
#include <algorithm>
#include "CallOutProcessorThroughPutData.h"
CCallOutDataManager::CCallOutDataManager(CalloutDispSpecs* _CallOutSpecs,CTermPlanDoc* _Doc)
:m_CallOutSpecs(_CallOutSpecs),
m_TermPlanDoc(_Doc)
{
}
CCallOutDataManager::~CCallOutDataManager(void)
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;
}
void CCallOutDataManager::GenerateCallOutData() 
{
	ClearCallOutData() ;
		CCallOutBaseData* PData = NULL ;
	for (int ndx = 0 ; ndx < (int)m_CallOutSpecs->GetElementCount() ;ndx++)
	{
		CalloutDispSpecItem* _PCallOutSpec = m_CallOutSpecs->GetItem(ndx) ;
		for (int ItemCount = 0 ; ItemCount < (int)_PCallOutSpec->GetData().GetElementCount();ItemCount++)
		{
			CalloutDispSpecDataItem* _PSpecDataItem = _PCallOutSpec->GetData().GetItem(ItemCount) ;
			if(!_PSpecDataItem->GetEnabled())
				continue ;
			switch(_PSpecDataItem->GetEnvMode())
			{
			case EnvMode_Terminal:
				PData = GenerateTerminalCallOutData(_PSpecDataItem) ;
				break;
			case EnvMode_AirSide:
				PData = GenerateAirsideCallOutData(_PSpecDataItem) ;
				break;
			case EnvMode_LandSide:
				PData = GenerateLandSideCallOutData(_PSpecDataItem) ;
				break ;
			case EnvMode_OnBoard:
				PData = GenerateOnBoardCallOutData(_PSpecDataItem) ;
				break ;
			default:
				{
					ASSERT(0);
				}
			}
			if(PData)
				m_Data.push_back(PData) ;
		}
	}
	SetCallOutPositon() ;
}
CCallOutBaseData* CCallOutDataManager::GenerateTerminalCallOutData(CalloutDispSpecDataItem* _PSpaceItem)
{
	CCallOutBaseData* PData = NULL ;
	switch(_PSpaceItem->GetFacilityType())
	{
	case CalloutDispSpecDataItem::Facility_Processor:
		{
			PData = GenerateTerminalCallOutProcessorData(_PSpaceItem) ;
			break ;
		}
	}
	return PData ;
}
CCallOutBaseData* CCallOutDataManager::GenerateTerminalCallOutProcessorData(CalloutDispSpecDataItem* _PSpaceItem)
{
	CCallOutBaseData* PData = NULL ;
	switch(_PSpaceItem->GetMessure())
	{
	case CalloutDispSpecDataItem::Throughput:
		{
			PData = new CCallOutProcessorThroughPutData(_PSpaceItem->GetStartTime(),
				_PSpaceItem->GetEndTime(),
				_PSpaceItem->GetIntervalTime(),
				_PSpaceItem->GetFacilityData().GetProcessorID(),
				m_TermPlanDoc,
				_PSpaceItem->GetMobileElementType(),
				_PSpaceItem->GetConnection() == CalloutDispSpecDataItem::Connection_Family) ;
		}
		break ;
	}
	return PData;
}

CCallOutBaseData* CCallOutDataManager::GenerateAirsideCallOutData(CalloutDispSpecDataItem* _PSpaceItem)
{
	return NULL ;
}
CCallOutBaseData* CCallOutDataManager::GenerateLandSideCallOutData(CalloutDispSpecDataItem* _PSpaceItem)
{
	return NULL ;
}
CCallOutBaseData* CCallOutDataManager::GenerateOnBoardCallOutData(CalloutDispSpecDataItem* _PSpaceItem)
{
	return NULL ;
}

void CCallOutDataManager::DestoryTheCallOutData(CCallOutBaseData* _Data)
{
	std::vector<CCallOutBaseData*>::iterator iter = std::find(m_Data.begin(),m_Data.end(),_Data) ;

	if(iter != m_Data.end())
	{
		m_Data.erase(iter) ;
		delete _Data ;
	}
}
void CCallOutDataManager::NoticeTheTimeHasUpdated(const ElapsedTime& _currentTime)
{
	CCallOutBaseData* PBaseData = NULL ;
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		PBaseData = m_Data[i] ;
		PBaseData->NoticeTheDialogUpdateTheTime(_currentTime) ;
	}
}


void CCallOutDataManager::ShowAllTheCallOutDialog()
{
	CCallOutBaseData* PBaseData = NULL ;
	m_CallOutLineData.ClearAllTheData() ;
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		PBaseData = m_Data[i] ;
		PBaseData->DoModel() ;
		m_CallOutLineData.RegisterTheDlg(PBaseData,PBaseData->GetProcessorID(),&(m_TermPlanDoc->GetTerminal())) ;
	}
}
void CCallOutDataManager::SetAnimationBeginTime(const ElapsedTime& _time)
{
	CCallOutBaseData* PBaseData = NULL ;
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		PBaseData = m_Data[i] ;
		PBaseData->SetAnimiationBeginTime(_time);
	}
}
void CCallOutDataManager::ShutDownAllTheCallOutDialog()
{
	CCallOutBaseData* PBaseData = NULL ;
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		PBaseData = m_Data[i] ;
		PBaseData->DestroyCallOutDialog() ;
	}
	m_CallOutLineData.ClearAllTheData() ;
}

void CCallOutDataManager::ClearCallOutData()
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;
}
void CCallOutDataManager::SetCallOutPositon()
{
	int m_nScreenX = GetSystemMetrics(SM_CXSCREEN); // You get the width of the users monitor
	int m_nScreenY = GetSystemMetrics(SM_CYSCREEN); // You get the height of the users monitor
	CCallOutBaseData* PBaseData = NULL ;
	int line = 0 ; 
	int column = 0 ;
	CPoint point ;
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		PBaseData = m_Data[i] ;

		point.x = 341 * column ;
		point.y = 164 * line ;
		PBaseData->SetDlgPosition(point) ;
		if(164 * (line + 1) > m_nScreenY)
		{
			line = 0 ;
			column++ ;
		}else
		{
			line ++ ;
		}
		if(341 * (column + 1 )> m_nScreenX)
		{
			line =0 ;
			column = 0 ;
		}
		
	}
}