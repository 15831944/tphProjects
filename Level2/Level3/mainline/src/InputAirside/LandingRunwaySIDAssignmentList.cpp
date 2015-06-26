#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\landingrunwaysidassignmentlist.h"
#include "AirsideImportExportManager.h"
#include "LandingRunwaySIDAssignment.h"
#include "FlightUseSID.h"
#include "FlightUseSIDList.h"
#include "common/AirportDatabase.h"


CLandingRunwaySIDAssignmentList::CLandingRunwaySIDAssignmentList()
{
	m_pAirportDatabase = NULL;

}

CLandingRunwaySIDAssignmentList::~CLandingRunwaySIDAssignmentList(void)
{
}

void CLandingRunwaySIDAssignmentList::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, RUNWAYID, RUNWAYMARKING, FROMTIME, TOTIME, FLTTYPE, SIDID \
					 FROM IN_SIDASSIGNMENT\
					 WHERE (PROJID = %d)"),nProjectID);
}

void CLandingRunwaySIDAssignmentList::InitFromDBRecordset(CADORecordset& recordset)
{
	int nRunWayID;
	int nMarking;

	recordset.GetFieldValue(_T("RUNWAYID"), nRunWayID);
	recordset.GetFieldValue(_T("RUNWAYMARKING"), nMarking);

	CLandingRunwaySIDAssignment *pLandingRunwaySIDAssignment = NULL;
	GetItemByRunwayIDAndMarking(nRunWayID, nMarking, &pLandingRunwaySIDAssignment);

	if (NULL == pLandingRunwaySIDAssignment)
	{
		pLandingRunwaySIDAssignment = new CLandingRunwaySIDAssignment();
		pLandingRunwaySIDAssignment->SetLandingRunwayID(nRunWayID);
		pLandingRunwaySIDAssignment->SetRunwayMarkingIndex(nMarking);
		AddNewItem(pLandingRunwaySIDAssignment);
	}
	pLandingRunwaySIDAssignment->SetAirportDatabase(m_pAirportDatabase);
	pLandingRunwaySIDAssignment->InitFromDBRecordset(recordset);
}

void CLandingRunwaySIDAssignmentList::GetItemByRunwayIDAndMarking(int nRunwayID, int nMarking, 
																   CLandingRunwaySIDAssignment ** ppLandingRunwaySIDAssignment)
{
	ASSERT(NULL != ppLandingRunwaySIDAssignment);

	*ppLandingRunwaySIDAssignment = NULL;

	size_t runwayCount = GetElementCount();
	for (int i=0; i<(int)runwayCount; i++)
	{
		CLandingRunwaySIDAssignment *pLandingRunwaySIDAssignment = GetItem(i); 
		if ((pLandingRunwaySIDAssignment->GetLandingRunwayID() == nRunwayID)
			&& (pLandingRunwaySIDAssignment->GetRunwayMarkingIndex() == nMarking))
		{
			*ppLandingRunwaySIDAssignment = pLandingRunwaySIDAssignment;

			return;
		}
	}
}

void CLandingRunwaySIDAssignmentList::ReadData(int nProjectID)
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

void CLandingRunwaySIDAssignmentList::ExportLandingRunwaySIDAssignment(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("SIDAssignmentID,");
	exportFile.getFile().writeField("ProjectID,");

	exportFile.getFile().writeField("RunwayID,");
	exportFile.getFile().writeField("RunwayMarkingID,");
	exportFile.getFile().writeField("FromTime,");
	exportFile.getFile().writeField("ToTime,");
	exportFile.getFile().writeField("FlightType,");
	exportFile.getFile().writeField("SidID");
	exportFile.getFile().writeLine();

	CLandingRunwaySIDAssignmentList landingRunwaySIDAssignmentList;
	landingRunwaySIDAssignmentList.ReadData(exportFile.GetProjectID());
	landingRunwaySIDAssignmentList.ExportData(exportFile);

	exportFile.getFile().endFile();
}

void CLandingRunwaySIDAssignmentList::ImportLandingRunwaySIDAssignment(CAirsideImportFile& importFile)
{
	CLandingRunwaySIDAssignmentList landingRunwaySIDAssignmentList;
	landingRunwaySIDAssignmentList.ImportData(importFile);
}

void CLandingRunwaySIDAssignmentList::ExportData(CAirsideExportFile& exportFile)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CLandingRunwaySIDAssignmentList::ImportData(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CFlightUseSID flightUseSID;
		flightUseSID.ImportData(importFile);
	}
}

BOOL CLandingRunwaySIDAssignmentList::IsConflictTime(int nRunwayID, CFlightUseSIDList * pFlightUseSIDList)
{
	size_t runwayCount = GetElementCount();

	for (int i=0; i<(int)runwayCount; i++)
	{
		CLandingRunwaySIDAssignment *pLandingRunwaySIDAssignment = GetItem(i); 

		if (nRunwayID == pLandingRunwaySIDAssignment->GetLandingRunwayID())
		{
			if (!pLandingRunwaySIDAssignment->IsConflictTime(pFlightUseSIDList))
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

void CLandingRunwaySIDAssignmentList::DeleteItemByRunwayID(int nRunwayID)
{
	size_t runwayCount = GetElementCount();

	for (int i=0; i<(int)runwayCount; i++)
	{
		CLandingRunwaySIDAssignment *pLandingRunwaySIDAssignment = GetItem(i); 

		if (nRunwayID == pLandingRunwaySIDAssignment->GetLandingRunwayID())
		{
			pLandingRunwaySIDAssignment->DeleteData();
		}
	}
}