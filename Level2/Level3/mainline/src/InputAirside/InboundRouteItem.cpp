#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\inboundrouteitem.h"
#include "ALTObject.h"
#include "AirsideImportExportManager.h"

CInboundRouteItem::CInboundRouteItem(void)
: m_nLevel(-3)
, m_nParentID(-3)
, m_nInboundRouteID(-3)
, m_nAltObjectID(-3)
, m_nIntersectNodeID(-3)
, m_strItemName("")
{

}

CInboundRouteItem::~CInboundRouteItem(void)
{

}

void CInboundRouteItem::SetParentID(int nParentID)
{
	m_nParentID = nParentID;
}

int CInboundRouteItem::GetParentID()
{
	return m_nParentID;
}

void CInboundRouteItem::SetInboundRouteID(int nInboundRouteID)
{
	m_nInboundRouteID = nInboundRouteID;
}

int CInboundRouteItem::GetInboundRouteID()
{
	return m_nInboundRouteID;
}

void CInboundRouteItem::SetALTObjectID(int nALTObjectID)
{
	m_nAltObjectID = nALTObjectID;
}

int CInboundRouteItem::GetALTObjectID()
{
	return m_nAltObjectID;
}

void CInboundRouteItem::SetIntersectNodeID(int nINodeID)
{
	m_nIntersectNodeID = nINodeID;
}

int CInboundRouteItem::GetIntersectNodeID()
{
	return m_nIntersectNodeID;
}

void CInboundRouteItem::SetItemName(CString strName)
{
	m_strItemName = strName;
}

CString CInboundRouteItem::GetItemName()
{
	return m_strItemName;
}

void CInboundRouteItem::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("NLEVEL"), m_nLevel);
	recordset.GetFieldValue(_T("PARENTID"), m_nParentID);
	recordset.GetFieldValue(_T("INBOUNDROUTEID"), m_nInboundRouteID);
	recordset.GetFieldValue(_T("OBJID"), m_nAltObjectID);
	recordset.GetFieldValue(_T("INTERSECTNODEID"), m_nIntersectNodeID);
	recordset.GetFieldValue(_T("ITEMNAME"), m_strItemName);

	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT * \
						   FROM IN_INBOUNDROUTE_DATA \
						   WHERE (INBOUNDROUTEID = %d AND PARENTID = %d)"),
						   m_nInboundRouteID, m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CInboundRouteItem *pInboundRouteItem = new CInboundRouteItem();
		pInboundRouteItem->InitFromDBRecordset(adoRecordset);

		AddNewItem(pInboundRouteItem);

		adoRecordset.MoveNextData();
	}
}

void CInboundRouteItem::GetInsertSQL(CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_INBOUNDROUTE_DATA\
					 (INBOUNDROUTEID, PARENTID, NLEVEL, OBJID, INTERSECTNODEID, ITEMNAME)\
					 VALUES (%d, %d, %d, %d, %d, '%s')"),\
					 m_nInboundRouteID,
					 m_nParentID,
					 m_nLevel,
					 m_nAltObjectID,
					 m_nIntersectNodeID,
					 m_strItemName);
}

void CInboundRouteItem::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_INBOUNDROUTE_DATA \
					 SET INBOUNDROUTEID = %d, \
					     PARENTID = %d, \
						 NLEVEL = %d, \
						 OBJID = %d, \
						 INTERSECTNODEID = %d, \
						 ITEMNAME = '%s' \
						 WHERE (ID =%d)"),\
					 m_nInboundRouteID,
					 m_nParentID,
					 m_nLevel,
					 m_nAltObjectID,
					 m_nIntersectNodeID,
					 m_strItemName,
					 m_nID);
}

void CInboundRouteItem::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_INBOUNDROUTE_DATA\
					 WHERE (ID = %d)"),m_nID);
}

void CInboundRouteItem::SaveDataToDB()
{
	CString strSQL;

	//not exist
	if (m_nID < 0)
	{
		GetInsertSQL(strSQL);

		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	for (std::vector<CInboundRouteItem *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetParentID(m_nID);
		(*iter)->SetInboundRouteID(m_nInboundRouteID);
		(*iter)->SaveDataToDB();
	}

	for (std::vector<CInboundRouteItem *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CInboundRouteItem::DeleteData()
{
	//delete content
	for (std::vector<CInboundRouteItem *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteData();
		m_deleteddataList.push_back(*iter);
	}

	m_dataList.clear();
}

void CInboundRouteItem::DeleteDataFromDB()
{
	ASSERT(0 == (int)m_dataList.size());

	for (std::vector<CInboundRouteItem *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();

	CString strSQL;
	GetDeleteSQL(strSQL);

	if (strSQL.IsEmpty())
	{
		return;
	}

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CInboundRouteItem::GetTailRouteItems(INBOUNDROUTEITEMLIST *pInboundRouteItemList)
{
	ASSERT(pInboundRouteItemList);

	if (0 == m_dataList.size())
	{
		pInboundRouteItemList->push_back(this);
		return;
	}

	for (std::vector<CInboundRouteItem *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->GetTailRouteItems(pInboundRouteItemList);
	}
}

void CInboundRouteItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nLevel);
	exportFile.getFile().writeInt(m_nParentID);
	exportFile.getFile().writeInt(m_nInboundRouteID);
	exportFile.getFile().writeInt(m_nAltObjectID);
	exportFile.getFile().writeInt(m_nIntersectNodeID);
	exportFile.getFile().writeField(m_strItemName);
	exportFile.getFile().writeInt((int)GetElementCount());
	exportFile.getFile().writeLine();

	for (int i=0; i<(int)GetElementCount(); i++)
	{
		GetItem(i)->ExportData(exportFile);
	}
}

void CInboundRouteItem::ImportData(CAirsideImportFile& importFile)
{
	int nOldID;
	importFile.getFile().getInteger(nOldID);

	importFile.getFile().getInteger(m_nLevel);

	int nOldParentID;
	importFile.getFile().getInteger(nOldParentID);
	if (-1 == nOldParentID)
	{
		m_nParentID = nOldParentID;
	}
	int nOldInboundRouteID;
	importFile.getFile().getInteger(nOldInboundRouteID);
	importFile.getFile().getInteger(m_nAltObjectID);
	m_nAltObjectID = importFile.GetObjectNewID(m_nAltObjectID);
	importFile.getFile().getInteger(m_nIntersectNodeID);
	m_nIntersectNodeID = importFile.GetIntersectionIndexMap(m_nIntersectNodeID);
	importFile.getFile().getField(m_strItemName.GetBuffer(1024),1024);

	int nChildItem;
	importFile.getFile().getInteger(nChildItem);
	importFile.getFile().getLine();

	CString strSQL = _T("");
	GetInsertSQL(strSQL);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	for (int i=0; i<nChildItem; i++)
	{
		CInboundRouteItem inboundRouteItem;
		inboundRouteItem.SetParentID(m_nID);
		inboundRouteItem.SetInboundRouteID(m_nInboundRouteID);
		inboundRouteItem.ImportData(importFile);
	}
}