#include "StdAfx.h"
#include "..\Database\DBElementCollection_Impl.h"
#include ".\routeitem.h"
#include "ALTObject.h"
#include "AirsideImportExportManager.h"

CRouteItem::CRouteItem(void)
: m_nParentID(-3)
, m_nRouteID(-3)
, m_nAltObjectID(-3)
, m_nIntersectNodeID(-3)
, m_strItemName("")
, m_nPriorityID(-1)
{
}

CRouteItem::~CRouteItem(void)
{
}

void CRouteItem::SetParentID(int nParentID)
{
	m_nParentID = nParentID;
}

int CRouteItem::GetParentID()
{
	return m_nParentID;
}

void CRouteItem::SetRouteID(int nRouteID)
{
	m_nRouteID = nRouteID;
}

int CRouteItem::GetRouteID()
{
	return m_nRouteID;
}

void CRouteItem::SetALTObjectID(int nALTObjectID)
{
	m_nAltObjectID = nALTObjectID;
}

int CRouteItem::GetALTObjectID()
{
	return m_nAltObjectID;
}

void CRouteItem::SetIntersectNodeID(int nINodeID)
{
	m_nIntersectNodeID = nINodeID;
}

int CRouteItem::GetIntersectNodeID()
{
	return m_nIntersectNodeID;
}

void CRouteItem::SetItemName(CString strName)
{
	m_strItemName = strName;
}

CString CRouteItem::GetItemName()
{
	return m_strItemName;
}

void CRouteItem::SetPrioirtyID(int nPriorityID)
{
	m_nPriorityID = nPriorityID;
}

int CRouteItem::GetPriorityID()const
{
	return m_nPriorityID;
}

void CRouteItem::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("PARENTID"), m_nParentID);
	recordset.GetFieldValue(_T("OUTBOUNDROUTEID"), m_nRouteID);
	recordset.GetFieldValue(_T("OBJID"), m_nAltObjectID);
	recordset.GetFieldValue(_T("INTERSECTNODEID"), m_nIntersectNodeID);
	recordset.GetFieldValue(_T("ITEMNAME"), m_strItemName);
	recordset.GetFieldValue(_T("PRIORITYID"),m_nPriorityID);

	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT * \
						   FROM IN_OUTBOUNDROUTE_DATA \
						   WHERE (OUTBOUNDROUTEID = %d AND PARENTID = %d)"),
						   m_nRouteID, m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CRouteItem *pOutboundRouteItem = new CRouteItem();
		pOutboundRouteItem->InitFromDBRecordset(adoRecordset);

		AddNewItem(pOutboundRouteItem);

		adoRecordset.MoveNextData();
	}
}

void CRouteItem::GetInsertSQL(CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_OUTBOUNDROUTE_DATA\
					 (OUTBOUNDROUTEID, PARENTID, OBJID, INTERSECTNODEID, ITEMNAME,PRIORITYID)\
					 VALUES (%d, %d, %d, %d, '%s',%d)"),\
					 m_nRouteID,
					 m_nParentID,
					 m_nAltObjectID,
					 m_nIntersectNodeID,
					 m_strItemName,
					 m_nPriorityID);
}

void CRouteItem::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_OUTBOUNDROUTE_DATA \
					 SET OUTBOUNDROUTEID = %d, \
					 PARENTID = %d, \
					 OBJID = %d, \
					 INTERSECTNODEID = %d, \
					 ITEMNAME = '%s', \
					 PRIORITYID = %d\
					 WHERE (ID =%d)"),\
					 m_nRouteID,
					 m_nParentID,
					 m_nAltObjectID,
					 m_nIntersectNodeID,
					 m_strItemName,
					 m_nPriorityID,
					 m_nID);
}

void CRouteItem::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM IN_OUTBOUNDROUTE_DATA\
					 WHERE (ID = %d)"),m_nID);
}

void CRouteItem::SaveDataToDB()
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

	for (std::vector<CRouteItem *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetParentID(m_nID);
		(*iter)->SetRouteID(m_nRouteID);
		(*iter)->SetPrioirtyID(m_nPriorityID);
		(*iter)->SaveDataToDB();
	}

	for (std::vector<CRouteItem *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteDataFromDB();
	}

	m_deleteddataList.clear();
}

void CRouteItem::DeleteData()
{
	//delete content
	for (std::vector<CRouteItem *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->DeleteData();
		m_deleteddataList.push_back(*iter);
	}

	m_dataList.clear();
}

void CRouteItem::DeleteDataFromDB()
{
	ASSERT(0 == (int)m_dataList.size());

	for (std::vector<CRouteItem *>::iterator iter = m_deleteddataList.begin();
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

