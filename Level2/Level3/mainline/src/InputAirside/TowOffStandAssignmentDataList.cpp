#include "StdAfx.h"
#include "..\Database\DBElementCollection_Impl.h"
#include ".\towoffstandassignmentdatalist.h"
#include "../Common/ARCFlight.h"
#include "../Common/ALTObjectID.h"
#include "../Common/AirsideFlightType.h"

const int SERVICETIME_DEF = 30;
const int MAXPARKINGTIME_DEF = 180;

CTowOffStandAssignmentDataList::CTowOffStandAssignmentDataList(TowOffDataContentNonNull NullOrNonNull/* = ContentNull*/)
{
	m_pAirportDatabase = NULL;
	if (ContentNonNull == NullOrNonNull)
	{
		AddNewItem(new CTowOffStandAssignmentData(ContentNonNull));
	}
}

CTowOffStandAssignmentDataList::~CTowOffStandAssignmentDataList(void)
{
}

//DBElementCollection
void CTowOffStandAssignmentDataList::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT * \
					 FROM IN_TOWOFFSTANDASSIGNMENT\
					 WHERE (PARENTID = %d)"), nProjectID);
}

bool CompareTowoffDataByFlightType(CTowOffStandAssignmentData* pData1, CTowOffStandAssignmentData* pData2)
{
	const AirsideFlightType& flttype1 = pData1->GetFlightType();
	const AirsideFlightType& flttype2 = pData2->GetFlightType();

	if (flttype1.IsFit(flttype2))
		return false;

	if(flttype2.IsFit(flttype1))
		return true;

	return false;
}

void CTowOffStandAssignmentDataList::ReadData(int nParentID)
{
	//  
	CleanData();

	DBElementCollection<CTowOffStandAssignmentData>::ReadData(nParentID);
	size_t nCount = GetElementCount();
	for (size_t i=0;i<nCount;i++)
	{
		GetItem(i)->SetAirportDatabase(m_pAirportDatabase);
	}

	if (0 == nCount)
	{
		AddNewItem(new CTowOffStandAssignmentData(ContentNonNull));
	}
	std::sort(m_dataList.begin(), m_dataList.end(),CompareTowoffDataByFlightType);
}


void CTowOffStandAssignmentDataList::SetAirportDatabase(CAirportDatabase* pAirportDB)
{
	m_pAirportDatabase = pAirportDB;
	size_t nCount = GetElementCount();
	for (size_t i=0;i<nCount;i++)
	{
		GetItem(i)->SetAirportDatabase(m_pAirportDatabase);
	}
	std::sort(m_dataList.begin(), m_dataList.end(),CompareTowoffDataByFlightType);
}

void CTowOffStandAssignmentDataList::ExportTowOffStandAssignment(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
	CTowOffStandAssignmentDataList towOffStandAssignmentDataList;
	towOffStandAssignmentDataList.SetAirportDatabase(pAirportDB);
	towOffStandAssignmentDataList.ReadData(exportFile.GetProjectID());

	exportFile.getFile().writeField(_T("ID, ProjID, Flight Type, DePlanementTime, EnPlanementTime, TowOffCriteria, IntermediateStandSID, InterMediateStandsType"));
	exportFile.getFile().writeLine();

	size_t itemCount = towOffStandAssignmentDataList.GetElementCount();	
	for (size_t i =0; i < itemCount; ++i)
	{
		towOffStandAssignmentDataList.GetItem(i)->ExportData(exportFile);	
	}
	exportFile.getFile().endFile();
}

void CTowOffStandAssignmentDataList::ImportTowOffStandAssignment(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CTowOffStandAssignmentData  towOffStandAssignmentData;
		towOffStandAssignmentData.ImportData(importFile);
	}
}


CTowOffStandAssignmentData* CTowOffStandAssignmentDataList::GetFlightFitData(ARCFlight* pflight, const ALTObjectID& standID )
{
	std::sort(m_dataList.begin(), m_dataList.end(),CompareTowoffDataByFlightType);

	int nCount = (int)GetElementCount();
	bool bFitToTowOff = false;
	for (int m = 0; m < nCount; m++)
	{
		CTowOffStandAssignmentData* pData = GetItem(m);
		AirsideFlightType fltType = pData->GetFlightType();
		if (fltType.IsFit(pflight))
		{
			if (standID.idFits(pData->GetStandALTObjectID()))
			{

				if(pData->IsTow() == TRUE && pData->GetRepositionOrFreeup() == FALSE)		//free up stand
				{
					if (pflight->isTurnaround() && (pflight->getDepTime() - pflight->getArrTime()) < ElapsedTime(pData->GetTowOffCriteria()*60L))	//not satisfy the max parking time condition
						continue;

					if (!pflight->isTurnaround() && pflight->getServiceTime()< ElapsedTime(pData->GetTowOffCriteria()*60L))
						continue;
				}

				return pData;
			}
		}
	}


	return NULL;
}

CTowOffStandAssignmentData* CTowOffStandAssignmentDataList::GetFlightTowoffData( ARCFlight* pflight, const ALTObjectID& standID )
{
	std::sort(m_dataList.begin(), m_dataList.end(),CompareTowoffDataByFlightType);

	int nCount = (int)GetElementCount();
	bool bFitToTowOff = false;
	for (int m = 0; m < nCount; m++)
	{
		CTowOffStandAssignmentData* pData = GetItem(m);
		AirsideFlightType fltType = pData->GetFlightType();
		if (fltType.IsFit(pflight))
		{
			if (standID.idFits(pData->GetStandALTObjectID()))
			{
				if(pData->IsTow() == TRUE && pData->GetRepositionOrFreeup() == FALSE)		//free up stand
				{
					if (pflight->isTurnaround() && (pflight->getDepTime() - pflight->getArrTime()) < ElapsedTime(pData->GetTowOffCriteria()*60L))	//not satisfy the max parking time condition
						continue;

					if (!pflight->isTurnaround() && pflight->getServiceTime()< ElapsedTime(pData->GetTowOffCriteria()*60L))
						continue;
				}

				return pData;
			}
		}
	}


	return NULL;
}

void CTowOffStandAssignmentDataList::GetFlightTowoffDataList( ARCFlight* pFlight, const ALTObjectID& standName, std::map<CString,CTowOffStandAssignmentData*>& mapDataList )
{
 	CTowOffStandAssignmentData* pData = GetFlightTowoffData(pFlight,standName);
 	if (pData)
 	{
 		mapDataList.insert(std::map<CString,CTowOffStandAssignmentData*>::value_type(standName.GetIDString(),pData));
 	}

}

