#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\landingrunwaysidassignment.h"
#include "FlightUseSIDList.h"
#include "common/AirportDatabase.h"

CLandingRunwaySIDAssignment::CLandingRunwaySIDAssignment()
: m_LandingRunwayID(0)
, m_RunwayMarkingIndex(0)
, m_pAirportDatabase(NULL)
{

}

CLandingRunwaySIDAssignment::~CLandingRunwaySIDAssignment(void)
{
}

void CLandingRunwaySIDAssignment::SetLandingRunwayID(int nLandingRunwayID)
{
	m_LandingRunwayID = nLandingRunwayID;
}

int CLandingRunwaySIDAssignment::GetLandingRunwayID(void)
{
	return m_LandingRunwayID;
}

void CLandingRunwaySIDAssignment::SetRunwayMarkingIndex(int nRunwayMarkingIndex)
{
	m_RunwayMarkingIndex = nRunwayMarkingIndex;
}

int CLandingRunwaySIDAssignment::GetRunwayMarkingIndex(void)
{
	return m_RunwayMarkingIndex;
}

void CLandingRunwaySIDAssignment::GetItemByTime(ElapsedTime estFromTime, ElapsedTime estToTime, CFlightUseSIDList **ppFlightUseSIDList)
{
	if (NULL == ppFlightUseSIDList)
	{
		return;
	}

	*ppFlightUseSIDList = NULL;

	size_t nFlightUseSIDListCount = GetElementCount();
	for (size_t i=0; i<nFlightUseSIDListCount; i++)
	{
		CFlightUseSIDList * pFlightUseSIDLst = GetItem(i);

		if (pFlightUseSIDLst->GetFromTime() == estFromTime
			&& pFlightUseSIDLst->GetToTime() == estToTime)
		{
			*ppFlightUseSIDList = pFlightUseSIDLst;

			return;
		}
	}
}

void CLandingRunwaySIDAssignment::DeleteItemByTime(ElapsedTime estFromTime, ElapsedTime estToTime)
{
	size_t nFlightUseSIDListCount = GetElementCount();
	for (size_t i=0; i<nFlightUseSIDListCount; i++)
	{
		CFlightUseSIDList * pFlightUseSIDLst = GetItem(i);

		if ((pFlightUseSIDLst->GetFromTime() == estFromTime)
			&& (pFlightUseSIDLst->GetToTime() == estToTime))
		{
			DeleteItemFromIndex(i);

			return;
		}
	}
}

void CLandingRunwaySIDAssignment::DeleteItemFromIndex(size_t nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_dataList.size());

	std::vector<CFlightUseSIDList *>::iterator iter = m_dataList.begin() + nIndex;

	(*iter)->DeleteData();

	m_deleteddataList.push_back(*iter);
	m_dataList.erase(iter);
}

void CLandingRunwaySIDAssignment::InitFromDBRecordset(CADORecordset& recordset)
{
	int nFromTime;
	int nToTime;

	recordset.GetFieldValue(_T("FROMTIME"), nFromTime);
	recordset.GetFieldValue(_T("TOTIME"), nToTime);

	ElapsedTime estFromTime((long)nFromTime);
	ElapsedTime estToTime((long)nToTime);

	CFlightUseSIDList *pFlightUseSIDList = NULL;
	GetItemByTime(estFromTime, estToTime, &pFlightUseSIDList);

	if (NULL == pFlightUseSIDList)
	{
		pFlightUseSIDList = new CFlightUseSIDList();
		pFlightUseSIDList->SetFromTime(estFromTime);
		pFlightUseSIDList->SetToTime(estToTime);
		AddNewItem(pFlightUseSIDList);
	}
	pFlightUseSIDList->SetAirportDatabase(m_pAirportDatabase);
	pFlightUseSIDList->InitFromDBRecordset(recordset);
	pFlightUseSIDList->SortFlightType();
}

void CLandingRunwaySIDAssignment::SaveData(int nParentID)
{
	std::vector<CFlightUseSIDList *>::iterator iter = m_dataList.begin();
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

void CLandingRunwaySIDAssignment::ExportData(CAirsideExportFile& exportFile)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++ i)
	{
		GetItem(i)->ExportDataEx(exportFile, m_LandingRunwayID, m_RunwayMarkingIndex);
	}
}

BOOL CLandingRunwaySIDAssignment::IsConflictTime(CFlightUseSIDList *pFlightUseSIDList)
{
	if (NULL == pFlightUseSIDList)
	{
		//no conflict
		return FALSE;
	}

	ElapsedTime estFromTime = pFlightUseSIDList->GetFromTime();
	ElapsedTime estToTime = pFlightUseSIDList->GetToTime();

	for (std::vector<CFlightUseSIDList *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		//don't compare with self
		if (pFlightUseSIDList == *iter)
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

void CLandingRunwaySIDAssignment::DeleteData()
{
	size_t nFlightUseSIDListCount = GetElementCount();
	for (size_t i=0; i<nFlightUseSIDListCount; i++)
	{
		DeleteItemFromIndex(i);
	}
}