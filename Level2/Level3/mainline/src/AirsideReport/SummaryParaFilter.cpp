#include "StdAfx.h"
#include ".\summaryparafilter.h"
#include "../Common/ALTObjectID.h"
#include "FlightConflictSummaryData.h"

ReportDataFilter::ReportDataFilter(long lStart, long lEnd, int nConType, int nAreaID, int nLocType, int nOpType, int nActType)
:m_lTimeStart(lStart)
,m_lTimeEnd(lEnd)
,m_nConflictType(nConType)
,m_nAreaID(nAreaID)
,m_nLocationType(nLocType)
,m_nOperationType(nOpType)
,m_nActionType(nActType)
{
}

ReportDataFilter::~ReportDataFilter()
{
}

bool ReportDataFilter::IsFit(FlightConflictReportData::ConflictDataItem* pItem)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
{
	long lTime = pItem->m_tTime.asSeconds();
	if (lTime < m_lTimeStart || lTime > m_lTimeEnd)
		return false;

	if (pItem->m_nConflictType == m_nConflictType && pItem->m_nAreaID == m_nAreaID && pItem->m_nLocationType == m_nLocationType 
		&& pItem->m_nOperationType == m_nOperationType && pItem->m_nActionType == m_nActionType)
		return true;

	return false;

}

/////////////////////////////////////////////////////////////////////////////////////////////
SummaryDataFilter::SummaryDataFilter( int nConType, CString strArea, int nLocType, int nOpType, int nActType)
:m_nConflictType(nConType)
,m_strArea(strArea)
,m_nLocationType(nLocType)
,m_nOperationType(nOpType)
,m_nActionType(nActType)
{
}

SummaryDataFilter::~SummaryDataFilter()
{
}

bool SummaryDataFilter::IsFit(FlightConflictSummaryData* pData)
{
	if ((m_nConflictType == -1 || pData->m_pParaFilter->m_nConflictType == m_nConflictType)
		&& (m_nLocationType == -1 || pData->m_pParaFilter->m_nLocationType == m_nLocationType)
		&& (m_nOperationType == -1 || pData->m_pParaFilter->m_nOperationType == m_nOperationType)
		&& (m_nActionType == -1 || pData->m_pParaFilter->m_nActionType == m_nActionType))
	{
		ALTObjectID altName(m_strArea);
		ALTObjectID altobj(pData->m_strAreaName);
		if (altobj.idFits(altName))
			return true;
	}

	return false;
}