#include "StdAfx.h"
#include ".\arrivaletaoffsetlist.h"
#include "..\Database\DBElementCollection_Impl.h"

CArrivalETAOffsetList::CArrivalETAOffsetList(void)
{
}

CArrivalETAOffsetList::~CArrivalETAOffsetList(void)
{
	m_dataList.clear();

	m_deleteddataList.clear();
}

//DBElementCollection
void CArrivalETAOffsetList::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT * \
					 FROM IN_DISTANCEANDTIMEFROMARPLIST\
					 WHERE (PROJID = %d)"),nProjectID);
}

void CArrivalETAOffsetList::ReadData(int nProjectID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nProjectID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		InitFromDBRecordset(adoRecordset);
		adoRecordset.MoveNextData();
	}

	if(GetElementCount() ==0 ) //add default element
	{
		CArrivalETAOffset *pArrivalETAOffset = new CArrivalETAOffset;
		CString strFlightType(_T("DEFAULT"));
		pArrivalETAOffset->SetFlightType(strFlightType);
		AddNewItem(pArrivalETAOffset);
		CDistanceAndTimeFromARP *pDistanceAndTimeFromARP = new CDistanceAndTimeFromARP;
		pArrivalETAOffset->AddNewItem(pDistanceAndTimeFromARP);

		try
		{
			CADODatabase::BeginTransaction();
			SaveData(nProjectID);
			CADODatabase::CommitTransaction();

		}
		catch (CADOException &e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
		}
	}

}

void CArrivalETAOffsetList::SaveData(int nParentID)
{

	for (std::vector<CArrivalETAOffset *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SaveData(nParentID);
	}

	for (std::vector<CArrivalETAOffset *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

//DBElement
void CArrivalETAOffsetList::InitFromDBRecordset(CADORecordset& recordset)
{
	CArrivalETAOffset* pArrivalETAOffset = new CArrivalETAOffset;
	pArrivalETAOffset->SetAirportDatabase(m_pAirportDatabase);
	pArrivalETAOffset->InitFromDBRecordset(recordset);

	AddNewItem(pArrivalETAOffset);
}

void CArrivalETAOffsetList::DeleteItemFromIndex(size_t nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_dataList.size());

	std::vector<CArrivalETAOffset *>::iterator iter = m_dataList.begin() + nIndex;
	(*iter)->DeleteData();
	m_deleteddataList.push_back(*iter);
	m_dataList.erase(iter);
}

void CArrivalETAOffsetList::ExportArrivalETAOffset(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB)
{
	exportFile.getFile().writeField("FlightType ID,");
	exportFile.getFile().writeField("ProjectID,");
	exportFile.getFile().writeField("FlightType,");

	exportFile.getFile().writeLine();

	CArrivalETAOffsetList arrivalETAOffsetList;
	arrivalETAOffsetList.SetAirportDatabase(pAirportDB);
	arrivalETAOffsetList.ReadData(exportFile.GetProjectID());
	arrivalETAOffsetList.ExportData(exportFile);

	exportFile.getFile().endFile();
}
	
void CArrivalETAOffsetList::ImportArrivalETAOffset(CAirsideImportFile& importFile)
{
	CArrivalETAOffsetList arrivalETAOffsetList;
	arrivalETAOffsetList.ImportData(importFile);
}

void CArrivalETAOffsetList::ExportData(CAirsideExportFile& exportFile)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CArrivalETAOffsetList::ImportData(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CArrivalETAOffset arrivalETAOffset;
		arrivalETAOffset.ImportData(importFile);
	}
}

struct FlightTypeLess
{
	int operator()(CArrivalETAOffset * pOffsetA, CArrivalETAOffset * pOffsetB)const
	{
		return pOffsetB->GetFlightType().fits( pOffsetA->GetFlightType());
	}
};
void CArrivalETAOffsetList::SortByFlightType()
{
	std::sort(m_dataList.begin(),m_dataList.end(),FlightTypeLess());
}

CArrivalETAOffset * CArrivalETAOffsetList::FindBestMatchETAOffset( const AirsideFlightDescStruct& fltDesc )const
{
	std::vector<CArrivalETAOffset * > vArrivalOffset =  m_dataList;

	std::sort( vArrivalOffset.begin(), vArrivalOffset.end(), FlightTypeLess() );

	for(int i=0;i< (int)vArrivalOffset.size();i++)
	{
		CArrivalETAOffset * pTheOffset = (CArrivalETAOffset*)vArrivalOffset[i];
		if(pTheOffset && pTheOffset->GetFlightType().fits(fltDesc) )
			return pTheOffset;
	}
	return NULL;
}