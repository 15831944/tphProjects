#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\inboundroutehead.h"
#include "AirsideImportExportManager.h"

CInboundRouteHead::CInboundRouteHead(void)
: m_pFirstRouteItem(NULL)
{
}

CInboundRouteHead::~CInboundRouteHead(void)
{
	if (NULL != m_pFirstRouteItem)
	{
		delete m_pFirstRouteItem;
	}

	int nCount = (int)m_deleteddataList.size();
	for (int i=0; i<nCount; i++)
	{
		CInboundRouteItem *pItem = m_deleteddataList[i];
		ASSERT(pItem);
		delete pItem;
	}

	m_deleteddataList.clear();
}

void CInboundRouteHead::InitFromDBRecordset(CADORecordset& recordset)
{
	ASSERT(NULL == m_pFirstRouteItem);
	m_pFirstRouteItem = new CInboundRouteItem;

	m_pFirstRouteItem->InitFromDBRecordset(recordset);
}

void CInboundRouteHead::GetInsertSQL(int nInboundID, CString& strSQL) const
{
	//strSQL.Format(_T("INSERT INTO IN_INBOUNDROUTE_DATAHEAD\
	//				 (INBOUNDID, ROUTEHEADNAME) \
	//				 VALUES (%d, '%s')"),\
	//				 nInboundID,
	//				 m_strRouteheadName);
}

void CInboundRouteHead::GetUpdateSQL(CString& strSQL) const
{
	//strSQL.Format(_T("UPDATE IN_INBOUNDROUTE_DATAHEAD\
	//				 SET \
	//				 ROUTEHEADNAME = '%s' \
	//				 WHERE ID = %d"),\
	//				 m_strRouteheadName,
	//				 m_nID);
}

void CInboundRouteHead::GetDeleteSQL(CString& strSQL) const
{
	//strSQL.Format(_T("DELETE FORM IN_INBOUNDROUTE_DATAHEAD\
	//				 WHERE ID = %d"),\
	//				 m_nID);
}

void CInboundRouteHead::SaveData(int nInboundID)
{
	ASSERT(m_pFirstRouteItem);
	m_pFirstRouteItem->SetParentID(-1);
	m_pFirstRouteItem->SetInboundRouteID(nInboundID);
	m_pFirstRouteItem->SaveDataToDB();

	for (std::vector<CInboundRouteItem *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CInboundRouteHead::DeleteData()
{
	//delete content
	ASSERT(0==(int)m_dataList.size());

	m_pFirstRouteItem->DeleteData();
	m_deleteddataList.push_back(m_pFirstRouteItem);

	m_pFirstRouteItem = NULL;
}

void CInboundRouteHead::DeleteDataFromDB()
{
	ASSERT(0==(int)m_dataList.size());
	ASSERT(m_pFirstRouteItem==NULL);

	for (std::vector<CInboundRouteItem *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CInboundRouteHead::GetFirstRouteItem(CInboundRouteItem **ppInboundRouteItem)
{
	ASSERT(ppInboundRouteItem);

	*ppInboundRouteItem = m_pFirstRouteItem;
}

void CInboundRouteHead::SetFirstRouteItem(CInboundRouteItem *pInboundRouteItem)
{
	ASSERT(pInboundRouteItem);
	m_pFirstRouteItem = pInboundRouteItem;
}

void CInboundRouteHead::GetTailRouteItems(INBOUNDROUTEITEMLIST *pInboundRouteItemList)
{
	ASSERT(pInboundRouteItemList);
	ASSERT(m_pFirstRouteItem);

	m_pFirstRouteItem->GetTailRouteItems(pInboundRouteItemList);
}

void CInboundRouteHead::ExportData(CAirsideExportFile& exportFile)
{
	if (m_pFirstRouteItem != NULL)
	{
		m_pFirstRouteItem->ExportData(exportFile);
	}
}

void CInboundRouteHead::ImportData(CAirsideImportFile& importFile)
{
	if (m_pFirstRouteItem != NULL)
	{
		m_pFirstRouteItem->ImportData(importFile);
	}
}