#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\landingrunwaystarassignment.h"
#include "FlightUseStarList.h"
#include "AirsideImportExportManager.h"
#include "common/AirportDatabase.h"

CLandingRunwayStarAssignment::CLandingRunwayStarAssignment()
: m_LandingRunwayID(0)
, m_RunwayMarkingIndex(0)
, m_pAirportDatabase(NULL)
{

}

CLandingRunwayStarAssignment::~CLandingRunwayStarAssignment(void)
{
}

void CLandingRunwayStarAssignment::SetLandingRunwayID(int nLandingRunwayID)
{
	m_LandingRunwayID = nLandingRunwayID;
}

int CLandingRunwayStarAssignment::GetLandingRunwayID(void)
{
	return m_LandingRunwayID;
}

void CLandingRunwayStarAssignment::SetRunwayMarkingIndex(int nRunwayMarkingIndex)
{
	m_RunwayMarkingIndex = nRunwayMarkingIndex;
}

int CLandingRunwayStarAssignment::GetRunwayMarkingIndex(void)
{
	return m_RunwayMarkingIndex;
}

void CLandingRunwayStarAssignment::GetItemByTime(ElapsedTime estFromTime, ElapsedTime estToTime, CFlightUseStarList **ppFlightUseStarList)
{
	if (NULL == ppFlightUseStarList)
	{
		return;
	}

	*ppFlightUseStarList = NULL;

	size_t nFlightUseStarListCount = GetElementCount();
	for (size_t i=0; i<nFlightUseStarListCount; i++)
	{
		CFlightUseStarList * pFlightUseStarLst = GetItem(i);

		if (pFlightUseStarLst->GetFromTime() == estFromTime
			&& pFlightUseStarLst->GetToTime() == estToTime)
		{
			*ppFlightUseStarList = pFlightUseStarLst;

			return;
		}
	}
}

void CLandingRunwayStarAssignment::DeleteItemByTime(ElapsedTime estFromTime, ElapsedTime estToTime)
{
	size_t nFlightUseStarListCount = GetElementCount();
	for (size_t i=0; i<nFlightUseStarListCount; i++)
	{
		CFlightUseStarList * pFlightUseStarLst = GetItem(i);

		if (pFlightUseStarLst->GetFromTime() == estFromTime
			&& pFlightUseStarLst->GetToTime() == estToTime)
		{
			DeleteItemFromIndex(i);

			return;
		}
	}
}

void CLandingRunwayStarAssignment::DeleteItemFromIndex(size_t nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_dataList.size());

	std::vector<CFlightUseStarList *>::iterator iter = m_dataList.begin() + nIndex;

	(*iter)->DeleteData();

	m_deleteddataList.push_back(*iter);
	m_dataList.erase(iter);
}

void CLandingRunwayStarAssignment::InitFromDBRecordset(CADORecordset& recordset)
{
	int nFromTime;
	int nToTime;

	recordset.GetFieldValue(_T("FROMTIME"), nFromTime);
	recordset.GetFieldValue(_T("TOTIME"), nToTime);

	ElapsedTime estFromTime((long)nFromTime);
	ElapsedTime estToTime((long)nToTime);

	CFlightUseStarList *pFlightUseStarList = NULL;
	GetItemByTime(estFromTime, estToTime, &pFlightUseStarList);

	if (NULL == pFlightUseStarList)
	{
		pFlightUseStarList = new CFlightUseStarList();
		pFlightUseStarList->SetFromTime(estFromTime);
		pFlightUseStarList->SetToTime(estToTime);
		AddNewItem(pFlightUseStarList);
	}
	pFlightUseStarList->SetAirportDatabase(m_pAirportDatabase);

	pFlightUseStarList->InitFromDBRecordset(recordset);
	pFlightUseStarList->SortFlightType();

}

void CLandingRunwayStarAssignment::SaveData(int nParentID)
{
	std::vector<CFlightUseStarList *>::iterator iter = m_dataList.begin();
	for (;iter != m_dataList.end(); ++iter)
	{
		(*iter)->Save(nParentID, m_LandingRunwayID, m_RunwayMarkingIndex);
	}

	for (iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->Save(nParentID, m_LandingRunwayID, m_RunwayMarkingIndex);
	}

	m_deleteddataList.clear();
}

void CLandingRunwayStarAssignment::ExportData(CAirsideExportFile& exportFile)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++ i)
	{
		GetItem(i)->ExportDataEx(exportFile, m_LandingRunwayID, m_RunwayMarkingIndex);
	}
}

BOOL CLandingRunwayStarAssignment::IsConflictTime(CFlightUseStarList *pFlightUseStarList)
{
	if (NULL == pFlightUseStarList)
	{
		//no conflict
		return FALSE;
	}

	ElapsedTime estFromTime = pFlightUseStarList->GetFromTime();
	ElapsedTime estToTime = pFlightUseStarList->GetToTime();

	for (std::vector<CFlightUseStarList *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		//don't compare with self
		if (pFlightUseStarList == *iter)
		{
			continue;
		}

		ElapsedTime estFromTimeTwo = (*iter)->GetFromTime();
		ElapsedTime estToTimeTwo = (*iter)->GetToTime();

		if ((estFromTime<=estFromTimeTwo && estToTime<=estFromTimeTwo)
			|| (estFromTimeTwo<=estFromTime && estToTimeTwo<=estFromTime))
		{
			//no conflict
			continue; 
		}
		else
		{
			//conflict
			return TRUE;
		}
	}

	//no conflict
	return FALSE;
}

void CLandingRunwayStarAssignment::DeleteData()
{
	size_t nFlightUseStarListCount = GetElementCount();
	for (size_t i=0; i<nFlightUseStarListCount; i++)
	{
		DeleteItemFromIndex(i);
	}
}