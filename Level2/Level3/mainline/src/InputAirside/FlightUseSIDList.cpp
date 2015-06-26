#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\flightusesidlist.h"
#include "Common/AirportDatabase.h"

CFlightUseSIDList::CFlightUseSIDList()
:m_FromTime(0l)
, m_ToTime(0l)
, m_pAirportDatabase(NULL)
{
}

CFlightUseSIDList::~CFlightUseSIDList(void)
{
}

void CFlightUseSIDList::SetFromTime(ElapsedTime estFromTime)
{
	m_FromTime = estFromTime;
}

ElapsedTime CFlightUseSIDList::GetFromTime(void)
{
	return m_FromTime;
}

void CFlightUseSIDList::SetToTime(ElapsedTime estToTime)
{
	m_ToTime = estToTime;
}

ElapsedTime CFlightUseSIDList::GetToTime(void)
{
	return m_ToTime;
}

void CFlightUseSIDList::GetItemByFlightTypeAndSidID(CString strFlightType, int nSidID, CFlightUseSID **ppFlightUseSID)
{
	if (NULL == ppFlightUseSID)
	{
		return;
	}

	*ppFlightUseSID = NULL;

	size_t nCount = GetElementCount();
	for (size_t i=0; i<nCount; i++)
	{
		CFlightUseSID *pFlightUseSID = GetItem(i);

		if (!pFlightUseSID->GetFlightTypeString().Compare(strFlightType)
			&& (pFlightUseSID->GetSidID()==nSidID))
		{
			*ppFlightUseSID = pFlightUseSID;

			return;
		}
	}
}

void CFlightUseSIDList::InitFromDBRecordset(CADORecordset& recordset)
{
	CFlightUseSID *pFlightUseSID = new CFlightUseSID;
	pFlightUseSID->SetAirportDatabase(m_pAirportDatabase);
	pFlightUseSID->InitFromDBRecordset(recordset);

	AddNewItem(pFlightUseSID);
}


void CFlightUseSIDList::Save(int nParentID, int nRunwayID, int nMarking)
{
	for (std::vector<CFlightUseSID *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->Save(nParentID, nRunwayID, nMarking, m_FromTime, m_ToTime);
	}

	for (std::vector<CFlightUseSID *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	m_deleteddataList.clear();
}

void CFlightUseSIDList::ExportDataEx(CAirsideExportFile& exportFile, int nRunwayID, int nMarkingID)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportDataEx(exportFile, nRunwayID, nMarkingID, m_FromTime.asSeconds(), m_ToTime.asSeconds());
	}
}

bool FlightTypeCompare(CFlightUseSID* fItem,CFlightUseSID* sItem)
{
	if (fItem->GetFlightType().fits(sItem->GetFlightType()))
		return false;
	else if(sItem->GetFlightType().fits(fItem->GetFlightType()))
		return true;

	return false;
}

void CFlightUseSIDList::SortFlightType()
{
	std::sort(m_dataList.begin(),m_dataList.end(),FlightTypeCompare);
}
