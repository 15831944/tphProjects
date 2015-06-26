#include "StdAfx.h"
#include "StandOperationData.h"
#include "../Common/termfile.h"
#include <Results/AirsideFlightEventLog.h>
/////data///////////////////////////////////////////////////////////////////////////////////////
CStandOperationReportData::CStandOperationReportData()
{
	m_lFlightIndex = -1;
	m_sSchedName = _T("");
	m_lSchedOn = 0;
	m_lSchedOff = 0;
	m_lSchedOccupancy = 0;
	m_lSchedAvailableOccupancy = 0;

	m_sActualName = _T("");
	m_lActualOn = 0;
	m_lActualOff = 0;
	m_lActualOccupancy = 0;
	m_lActualAvailableOccupancy = 0;

	m_lDelayEnter = 0;
	m_lDueStandOccupied = 0;

	m_lDelayLeaving = 0;
	m_lDueTaxiwayOccupied = 0;
	m_lDueGSE = 0;
	m_lDuePushbackClearance = 0;
}

CStandOperationReportData::~CStandOperationReportData()
{

}

bool CStandOperationReportData::IsConflict()
{
	if (!m_sSchedName.IsEmpty() && !m_sActualName.IsEmpty() && m_sSchedName != m_sActualName)
		return true;

	if (m_lDelayEnter > 0l)
		return true;

	return false;
}

bool CStandOperationReportData::IsDelay()
{
	if (m_lDelayEnter > 0l)
		return true;

	if (m_lDelayLeaving > 0l)
		return true;

	return false;
}

////////summary data///////////////////////////////////////////////////////////////////////////////////
CSummaryStandOperationData::CSummaryStandOperationData()
:m_dTotal(0)
{

}

CSummaryStandOperationData::~CSummaryStandOperationData()
{

}

