#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\ctrlinterventionspecdatalist.h"
#include "AirsideImportExportManager.h"
#include "CtrlInterventionSpecData.h"

CCtrlInterventionSpecDataList::CCtrlInterventionSpecDataList(void)
 : BaseClass()
{
}

CCtrlInterventionSpecDataList::~CCtrlInterventionSpecDataList(void)
{
}

void CCtrlInterventionSpecDataList::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, RUNWAYID, RUNWAYMARKING, PRIORITYONE, PRIORITYTWO, PRIORITYTHREE \
					 FROM IN_CTRLINTERVENTIONSPEC\
					 WHERE (PROJID = %d)"),nProjectID);
}

void CCtrlInterventionSpecDataList::ReadData(int nProjectID)
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

void CCtrlInterventionSpecDataList::InitFromDBRecordset(CADORecordset& recordset)
{
	CCtrlInterventionSpecData *pCtrlInterventionSpecData = new CCtrlInterventionSpecData();
	pCtrlInterventionSpecData->InitFromDBRecordset(recordset);

	AddNewItem(pCtrlInterventionSpecData);
}

void CCtrlInterventionSpecDataList::ExportCtrlInterventionSpec(CAirsideExportFile& exportFile)
{
	CCtrlInterventionSpecDataList ctrlInterventionSpecDataList;
	ctrlInterventionSpecDataList.ReadData(exportFile.GetProjectID());
	ctrlInterventionSpecDataList.ExportData(exportFile);
}

void CCtrlInterventionSpecDataList::ImportCtrlInterventionSpec(CAirsideImportFile& importFile)
{
	CCtrlInterventionSpecDataList ctrlInterventionSpecDataList;
	ctrlInterventionSpecDataList.ImportData(importFile);
}

void CCtrlInterventionSpecDataList::ExportData(CAirsideExportFile& exportFile)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CCtrlInterventionSpecDataList::ImportData(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CCtrlInterventionSpecData ctrlInterventionSpecData;;
		ctrlInterventionSpecData.ImportData(importFile);
	}
}

bool CCtrlInterventionSpecDataList::IsSideStepPreGoAround( int nRunwayID, int rw_mark ) const
{	
	for(int i=0;i< (int) GetElementCount();i++)
	{
		CCtrlInterventionSpecData*  pItem = (CCtrlInterventionSpecData* )GetItem(i);
		if( pItem->GetRunwayID() == nRunwayID && pItem->GetMarkingIndex() == rw_mark )
		{
			return pItem->GetPriority(IM_SIDESTEP) < pItem->GetPriority(IM_GOAROUND); 
		}
	}
	return true;
}