#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\flightusestarlist.h"
#include "AirsideImportExportManager.h"
#include "Common/AirportDatabase.h"

CFlightUseStarList::CFlightUseStarList()
:m_FromTime(0l)
 , m_ToTime(0l)
 , m_pAirportDatabase(NULL)
{
}

CFlightUseStarList::~CFlightUseStarList(void)
{
}

void CFlightUseStarList::SetFromTime(ElapsedTime estFromTime)
{
	m_FromTime = estFromTime;
}

ElapsedTime CFlightUseStarList::GetFromTime(void)
{
	return m_FromTime;
}

void CFlightUseStarList::SetToTime(ElapsedTime estToTime)
{
	m_ToTime = estToTime;
}

ElapsedTime CFlightUseStarList::GetToTime(void)
{
	return m_ToTime;
}

void CFlightUseStarList::GetItemByFlightTypeAndStarID(CString strFlightType, int nStarID, CFlightUseStar **ppFlightUseStar)
{
	if (NULL == ppFlightUseStar)
	{
		return;
	}

	*ppFlightUseStar = NULL;

	size_t nCount = GetElementCount();
	for (size_t i=0; i<nCount; i++)
	{
		CFlightUseStar *pFlightUseStar = GetItem(i);

		if (!pFlightUseStar->GetFlightTypeString().Compare(strFlightType)
			&& (pFlightUseStar->GetStarID()==nStarID))
		{
			*ppFlightUseStar = pFlightUseStar;

			return;
		}
	}
}

void CFlightUseStarList::InitFromDBRecordset(CADORecordset& recordset)
{
	CFlightUseStar *pFlightUseStar = new CFlightUseStar;
	pFlightUseStar->SetAirportDatabase(m_pAirportDatabase);
	pFlightUseStar->InitFromDBRecordset(recordset);


	AddNewItem(pFlightUseStar);
}

void CFlightUseStarList::Save(int nParentID, int nRunwayID, int nMarking)
{
	for (std::vector<CFlightUseStar *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->Save(nParentID, nRunwayID, nMarking, m_FromTime, m_ToTime);
	}

	for (std::vector<CFlightUseStar *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	m_deleteddataList.clear();
}

void CFlightUseStarList::ExportDataEx(CAirsideExportFile& exportFile, int nRunwayID, int nMarkingID)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportDataEx(exportFile, nRunwayID, nMarkingID, m_FromTime.asSeconds(), m_ToTime.asSeconds());
	}
}

bool FlightTypeCompare(CFlightUseStar* fItem,CFlightUseStar* sItem)
{
	if (fItem->GetFlightType().fits(sItem->GetFlightType()))
		return false;
	else if(sItem->GetFlightType().fits(fItem->GetFlightType()))
		return true;

	return false;
}

void CFlightUseStarList::SortFlightType()
{
	std::sort(m_dataList.begin(),m_dataList.end(),FlightTypeCompare);
}