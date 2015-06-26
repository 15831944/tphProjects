#include "StdAfx.h"
#include ".\airsideenroutequeuecapacity.h"
#include "..\Common\SqlScriptMaker.h"
#include "..\Database\ADODatabase.h"
#include "..\Database\DBElementCollection_Impl.h"

CAirsideEnrouteQueueCapacity::CAirsideEnrouteQueueCapacity(void)
{
}

CAirsideEnrouteQueueCapacity::~CAirsideEnrouteQueueCapacity(void)
{
	//Clear();
}

//int CAirsideEnrouteQueueCapacity::getCount() const
//{
//	return static_cast<int>(m_vEnrouteCapacity.size());
//}
//
//void CAirsideEnrouteQueueCapacity::ReadData( int nProjectID )
//{
//
//	SqlSelectScriptMaker sqlSelect(_T("AS_ENROUTEQCAPACITY_TIMERANGE"));
//	sqlSelect.AddColumn(_T("*"));
//
//	CString strSelectSQL = sqlSelect.GetScript();
//
//	long nRecordCount = -1;
//	CADORecordset adoRecordset;
//	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);
//
//	while (!adoRecordset.IsEOF())
//	{
//		AirsideEnrouteQCapacityTimeRange* pData = new AirsideEnrouteQCapacityTimeRange;
//		pData->InitFromDBRecordset(adoRecordset);
//		m_vEnrouteCapacity.push_back(element);
//		adoRecordset.MoveNextData();
//	}
//
//
//
//
//
//}
//AirsideEnrouteQCapacityTimeRange * CAirsideEnrouteQueueCapacity::getItem( int nIndex )
//{
//	ASSERT(nIndex >=0 && nIndex < static_cast<int>(m_vEnrouteCapacity.size()));
//	if (nIndex >=0 && nIndex < static_cast<int>(m_vEnrouteCapacity.size()))
//	{
//		return m_vEnrouteCapacity.at(nIndex);
//	}
//
//	return NULL;
//}
//
//void CAirsideEnrouteQueueCapacity::DeleteItem( AirsideEnrouteQCapacityTimeRange * pItem )
//{
//	std::vector<AirsideEnrouteQCapacityTimeRange *>::iterator iterFind =
//		std::find(m_vEnrouteCapacity.begin(), m_vEnrouteCapacity.end(), pItem);
//	if(iterFind != m_vEnrouteCapacity.end())
//	{
//		m_vEnrouteCapacity.erase(iterFind);
//	}
//
//	m_vDelEnrouteCapacity.push_back(pItem);
//}
//
//void CAirsideEnrouteQueueCapacity::Clear()
//{
//	std::vector<AirsideEnrouteQCapacityTimeRange *>::iterator iter = m_vEnrouteCapacity.begin();
//	for (; iter != m_vEnrouteCapacity.end(); ++iter)
//	{
//		delete *iter;
//	}
//	m_vEnrouteCapacity.clear();
//
//
//	iter = m_vDelEnrouteCapacity.begin();
//	for (; iter != m_vDelEnrouteCapacity.end(); ++iter)
//	{
//		delete *iter;
//	}
//	m_vDelEnrouteCapacity.clear();
//
//
//}

void CAirsideEnrouteQueueCapacity::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
		SqlSelectScriptMaker sqlSelect(_T("AS_ENROUTEQCAPACITY_TIMERANGE"));
		sqlSelect.AddColumn(_T("*"));
	
		strSQL = sqlSelect.GetScript();
}

AirsideEnrouteQCapacityTimeRange* CAirsideEnrouteQueueCapacity::AddNewItem( AirsideEnrouteQCapacityTimeRange* pTimeRangeData )
{
	return DBElementCollection< AirsideEnrouteQCapacityTimeRange >::AddNewItem(pTimeRangeData);

}


//////////////////////////////////////////////////////////////////////////
//AirsideEnrouteQCapacityItem

AirsideEnrouteQCapacityItem::AirsideEnrouteQCapacityItem()
{
	m_dMaxLength = 100000;
}
AirsideEnrouteQCapacityItem::~AirsideEnrouteQCapacityItem()
{

}
void AirsideEnrouteQCapacityItem::setTakeOffPosition( int nExitID )
{
	m_takeOffPosition.ReadData(nExitID);
}

RunwayExit * AirsideEnrouteQCapacityItem::getTakeOffPosition()
{
	return &m_takeOffPosition;
}

void AirsideEnrouteQCapacityItem::setMaxLength( double dMaxLength )
{
	//m -> cm
	m_dMaxLength = dMaxLength *100;
}

double AirsideEnrouteQCapacityItem::getMaxLength() const
{//cm ->m
	return m_dMaxLength/100;
}

void AirsideEnrouteQCapacityItem::InitFromDBRecordset( CADORecordset& adoRecordset )
{
	adoRecordset.GetFieldValue(_T("ID"), m_nID);

	int nExitID = -1;
	adoRecordset.GetFieldValue(_T("RUNWAYEXITID"), nExitID);
	adoRecordset.GetFieldValue(_T("MAXLENGTH"),m_dMaxLength);

	ASSERT(nExitID >=0);
	if(nExitID >= 0)
		setTakeOffPosition(nExitID);
}

void AirsideEnrouteQCapacityItem::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	SqlInsertScriptMaker sqlInsert(_T("AS_ENROUTEQCAPACITY_DATA"));
	sqlInsert.AddColumn(_T("TIMERANGEID"), nParentID);
	sqlInsert.AddColumn(_T("RUNWAYEXITID"), m_takeOffPosition.GetID());
	sqlInsert.AddColumn(_T("MAXLENGTH"), m_dMaxLength);

	strSQL = sqlInsert.GetScript();
}

void AirsideEnrouteQCapacityItem::GetUpdateSQL( CString& strSQL ) const
{
	SqlUpdateScriptMaker sqlUpdate("AS_ENROUTEQCAPACITY_DATA");
	sqlUpdate.AddColumn(_T("RUNWAYEXITID"), m_takeOffPosition.GetID());
	sqlUpdate.AddColumn(_T("MAXLENGTH"), m_dMaxLength);

	CString strCondition;
	strCondition.Format(_T(" ID = %d"), m_nID);
	sqlUpdate.SetCondition(strCondition);

	strSQL = sqlUpdate.GetScript();
}

void AirsideEnrouteQCapacityItem::GetDeleteSQL( CString& strSQL ) const
{
	SqlDeleteScriptMaker sqlDel(_T("AS_ENROUTEQCAPACITY_DATA"));

	CString strCondition;
	strCondition.Format(_T(" ID = %d"), m_nID);
	sqlDel.SetCondition(strCondition);

	strSQL = sqlDel.GetScript();
}

void AirsideEnrouteQCapacityItem::GetSelectSQL( int nID,CString& strSQL ) const
{
	ASSERT(0);
}


void AirsideEnrouteQCapacityTimeRange::SetTimeRange( ElapsedTime eStart, ElapsedTime eTimeEnd )
{
	m_timeRange.SetStartTime(eStart);
	m_timeRange.SetEndTime(eTimeEnd);
}

void AirsideEnrouteQCapacityTimeRange::InitFromDBRecordset( CADORecordset& adoRecordset )
{
	adoRecordset.GetFieldValue(_T("ID"), m_nID);
	

	int nStartTime;
	int nEndTime;
	adoRecordset.GetFieldValue(_T("STARTTIME"), nStartTime);
	adoRecordset.GetFieldValue(_T("ENDTIME"),nEndTime);

	ElapsedTime eStartTime;
	ElapsedTime eEndTime;
	eStartTime.setPrecisely(nStartTime);
	eEndTime.setPrecisely(nEndTime);


	m_timeRange.SetStartTime(eStartTime);
	m_timeRange.SetEndTime(eEndTime);

	//
	m_enRouteData.ReadData(m_nID);

}

void AirsideEnrouteQCapacityTimeRange::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	SqlInsertScriptMaker sqlInsert(_T("AS_ENROUTEQCAPACITY_TIMERANGE"));
	sqlInsert.AddColumn(_T("STARTTIME"), m_timeRange.GetStartTime().getPrecisely());
	sqlInsert.AddColumn(_T("ENDTIME"), m_timeRange.GetEndTime().getPrecisely());

	strSQL = sqlInsert.GetScript();
}

void AirsideEnrouteQCapacityTimeRange::GetUpdateSQL( CString& strSQL ) const
{
	SqlUpdateScriptMaker sqlUpdate(_T("AS_ENROUTEQCAPACITY_TIMERANGE"));
	sqlUpdate.AddColumn(_T("STARTTIME"), m_timeRange.GetStartTime().getPrecisely());
	sqlUpdate.AddColumn(_T("ENDTIME"), m_timeRange.GetEndTime().getPrecisely());

	CString strCondition;
	strCondition.Format(_T(" ID = %d"), m_nID);
	sqlUpdate.SetCondition(strCondition);

	strSQL = sqlUpdate.GetScript();
}

void AirsideEnrouteQCapacityTimeRange::GetDeleteSQL( CString& strSQL ) const
{

	SqlDeleteScriptMaker sqlDel(_T("AS_ENROUTEQCAPACITY_TIMERANGE"));

	CString strCondition;
	strCondition.Format(_T(" ID = %d"), m_nID);
	sqlDel.SetCondition(strCondition);

	strSQL = sqlDel.GetScript();
}

void AirsideEnrouteQCapacityTimeRange::GetSelectSQL( int nID,CString& strSQL ) const
{
	ASSERT(0);
}

void AirsideEnrouteQCapacityTimeRange::SaveData( int nParentID )
{
	DBElement::SaveData(nParentID);
	ASSERT(m_nID >=0);
	if(m_nID >= 0)
		m_enRouteData.SaveData(m_nID);
}

AirsideEnrouteQCapacityItemList* AirsideEnrouteQCapacityTimeRange::getEnrouteQCapacity()
{
	return &m_enRouteData;
}

AirsideEnrouteQCapacityItemList::AirsideEnrouteQCapacityItemList()
{

}

AirsideEnrouteQCapacityItemList::~AirsideEnrouteQCapacityItemList()
{

}

void AirsideEnrouteQCapacityItemList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	SqlSelectScriptMaker sqlSelect(_T("AS_ENROUTEQCAPACITY_DATA"));
	sqlSelect.AddColumn(_T("*"));

	CString strCondition;
	strCondition.Format(_T("TIMERANGEID = %d"), nParentID);
	sqlSelect.SetCondition(strCondition);

	strSQL = sqlSelect.GetScript();	
}

AirsideEnrouteQCapacityItem* AirsideEnrouteQCapacityItemList::AddNewItem( AirsideEnrouteQCapacityItem* pEnrouteQItem )
{
	return DBElementCollection< AirsideEnrouteQCapacityItem >::AddNewItem(pEnrouteQItem);
}
