#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\flighttyperestrictionlist.h"
#include "AirsideImportExportManager.h"


CFlightTypeRestrictionList::CFlightTypeRestrictionList(void)
{
	m_nProjID = 0;
	m_pAirportDatabase = NULL;
}

CFlightTypeRestrictionList::~CFlightTypeRestrictionList(void)
{
}

void CFlightTypeRestrictionList::SetProjID(int nProjID)
{
   m_nProjID = nProjID;
}

int  CFlightTypeRestrictionList::GetProjID() const
{
	return m_nProjID;
}

void CFlightTypeRestrictionList::SetAirportDatabase(CAirportDatabase* pAirportDatabase)
{
	m_pAirportDatabase = pAirportDatabase;
}
void CFlightTypeRestrictionList::GetSelectElementSQL(int nParentID, CString &strSQL)
{

	strSQL.Format(_T("SELECT ID, FLIGHTTYPE, TAXIWAYID, FROMID, TOID\
					 FROM IN_TAXIWAYFLIGHTTYPERESTRICTONS\
					 WHERE (PROJID = %d)"), nParentID); 

}

void CFlightTypeRestrictionList::ReadData(int nProjectID)
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
}

void CFlightTypeRestrictionList::InitFromDBRecordset(CADORecordset& recordset)
{
	CFlightTypeRestriction *pFlightTypeRestriction = new CFlightTypeRestriction();
	pFlightTypeRestriction->SetAirportDatabase(m_pAirportDatabase);
	pFlightTypeRestriction->InitFromDBRecordset(recordset);
	AddNewItem(pFlightTypeRestriction);
}

void CFlightTypeRestrictionList::ImportTaxiwayFlightTypeRestriction(CAirsideImportFile& importFile)
{
	CFlightTypeRestriction FlightTypeRestriction;	
	FlightTypeRestriction.ImportData(importFile);
}


void CFlightTypeRestrictionList::ExportTaxiwayFlightTypeRestriction(CAirsideExportFile& exportFile)
{
	CFlightTypeRestriction FlightTypeRestriction;
	FlightTypeRestriction.ExportData(exportFile);
	exportFile.getFile().endFile();
}

