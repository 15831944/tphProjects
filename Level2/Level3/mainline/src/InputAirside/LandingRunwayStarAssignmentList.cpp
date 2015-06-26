#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\landingrunwaystarassignmentlist.h"
#include "AirsideImportExportManager.h"
#include "LandingRunwayStarAssignment.h"
#include "FlightUseStarList.h"
#include "FlightUseStar.h"
#include "common/AirportDatabase.h"

CLandingRunwayStarAssignmentList::CLandingRunwayStarAssignmentList()
{
	m_pAirportDatabase = NULL;
}

CLandingRunwayStarAssignmentList::~CLandingRunwayStarAssignmentList(void)
{
}

void CLandingRunwayStarAssignmentList::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, RUNWAYID, RUNWAYMARKING, FROMTIME, TOTIME, FLTTYPE, STARID \
					 FROM IN_STARASSIGNMENT\
					 WHERE (PROJID = %d)"),nProjectID);
}

void CLandingRunwayStarAssignmentList::InitFromDBRecordset(CADORecordset& recordset)
{
	int nRunWayID;
	int nMarking;

	recordset.GetFieldValue(_T("RUNWAYID"), nRunWayID);
	recordset.GetFieldValue(_T("RUNWAYMARKING"), nMarking);

	CLandingRunwayStarAssignment *pLandingRunwayStarAssignment = NULL;
	GetItemByRunwayIDAndMarking(nRunWayID, nMarking, &pLandingRunwayStarAssignment);

	if (NULL == pLandingRunwayStarAssignment)
	{
		pLandingRunwayStarAssignment = new CLandingRunwayStarAssignment();
		pLandingRunwayStarAssignment->SetLandingRunwayID(nRunWayID);
		pLandingRunwayStarAssignment->SetRunwayMarkingIndex(nMarking);
		AddNewItem(pLandingRunwayStarAssignment);
	}
	pLandingRunwayStarAssignment->SetAirportDatabase(m_pAirportDatabase);
	pLandingRunwayStarAssignment->InitFromDBRecordset(recordset);
}


void CLandingRunwayStarAssignmentList::GetItemByRunwayIDAndMarking(int nRunwayID, int nMarking, 
																   CLandingRunwayStarAssignment ** ppLandingRunwayStarAssignment)
{
	ASSERT(NULL != ppLandingRunwayStarAssignment);

	*ppLandingRunwayStarAssignment = NULL;

	size_t runwayCount = GetElementCount();
	for (int i=0; i<(int)runwayCount; i++)
	{
		CLandingRunwayStarAssignment *pLandingRunwayStarAssignment = GetItem(i); 
		if ((pLandingRunwayStarAssignment->GetLandingRunwayID() == nRunwayID)
			&& (pLandingRunwayStarAssignment->GetRunwayMarkingIndex() == nMarking))
		{
			*ppLandingRunwayStarAssignment = pLandingRunwayStarAssignment;

			return;
		}
	}
}

void CLandingRunwayStarAssignmentList::ReadData(int nProjectID)
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

void CLandingRunwayStarAssignmentList::ExportLandingRunwayStarAssignment(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("StarAssignmentID,");
	exportFile.getFile().writeField("ProjectID,");

	exportFile.getFile().writeField("RunwayID,");
	exportFile.getFile().writeField("RunwayMarkingID,");
	exportFile.getFile().writeField("FromTime,");
	exportFile.getFile().writeField("ToTime,");
	exportFile.getFile().writeField("FlightType,");
	exportFile.getFile().writeField("StarID");
	exportFile.getFile().writeLine();

	CLandingRunwayStarAssignmentList landingRunwayStarAssignmentList;
	landingRunwayStarAssignmentList.ReadData(exportFile.GetProjectID());
	landingRunwayStarAssignmentList.ExportData(exportFile);

	exportFile.getFile().endFile();
}

void CLandingRunwayStarAssignmentList::ImportLandingRunwayStarAssignment(CAirsideImportFile& importFile)
{
	CLandingRunwayStarAssignmentList landingRunwayStarAssignmentList;
	landingRunwayStarAssignmentList.ImportData(importFile);
}

void CLandingRunwayStarAssignmentList::ExportData(CAirsideExportFile& exportFile)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CLandingRunwayStarAssignmentList::ImportData(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CFlightUseStar flightUseStar;
		flightUseStar.ImportData(importFile);
	}
}

BOOL CLandingRunwayStarAssignmentList::IsConflictTime(int nRunwayID, CFlightUseStarList * pFlightUseStarList)
{
	size_t runwayCount = GetElementCount();

	for (int i=0; i<(int)runwayCount; i++)
	{
		CLandingRunwayStarAssignment *pLandingRunwayStarAssignment = GetItem(i); 

		if (nRunwayID == pLandingRunwayStarAssignment->GetLandingRunwayID())
		{
			if (!pLandingRunwayStarAssignment->IsConflictTime(pFlightUseStarList))
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
	}

	return FALSE;
}

void CLandingRunwayStarAssignmentList::DeleteItemByRunwayID(int nRunwayID)
{
	size_t runwayCount = GetElementCount();

	for (int i=0; i<(int)runwayCount; i++)
	{
		CLandingRunwayStarAssignment *pLandingRunwayStarAssignment = GetItem(i); 

		if (nRunwayID == pLandingRunwayStarAssignment->GetLandingRunwayID())
		{
			pLandingRunwayStarAssignment->DeleteData();
		}
	}
}