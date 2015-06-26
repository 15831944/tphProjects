#include "StdAfx.h"
#include ".\servicingrequirementlist.h"
#include "AirsideImportExportManager.h"
#include "ProjectDB.h"
#include "ServicingRequirement.h"

CServicingRequirementList::CServicingRequirementList(void)
{
}

CServicingRequirementList::~CServicingRequirementList(void)
{
}

void CServicingRequirementList::GetSelectElementSQL(int nProjectID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT *\
					 FROM IN_SERVICINGREQUIREMENT\
					 WHERE (PROJID = %d)"),nProjectID);
}

void CServicingRequirementList::ReadData(int nProjectID)
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

void CServicingRequirementList::InitFromDBRecordset(CADORecordset& recordset)
{
	CServicingRequirement *pServicingRequirement = new CServicingRequirement;
	pServicingRequirement->InitFromDBRecordset(recordset);

	AddNewItem(pServicingRequirement);
}

void CServicingRequirementList::ExportServicingRequirement(CAirsideExportFile& exportFile)
{
	CServicingRequirementList servicingRequirementList;

	servicingRequirementList.ReadData(exportFile.GetProjectID());
	servicingRequirementList.ExportData(exportFile);
}

void CServicingRequirementList::ImportServicingRequirement(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CServicingRequirement servicingRequirement;
		servicingRequirement.ImportData(importFile);
	}
}

void CServicingRequirementList::ExportData(CAirsideExportFile& exportFile)
{
	size_t nItemCount = GetElementCount();
	for (size_t i =0; i < nItemCount; ++i)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CServicingRequirementList::ImportData(CAirsideImportFile& importFile)
{

}