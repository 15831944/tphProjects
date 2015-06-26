#include "StdAfx.h"
#include ".\towoperationroute.h"
#include "../Database/DBElementCollection_Impl.h"
#include "../Common/SqlScriptMaker.h"

//////////////////////////////////////////////////////////////////////////
//CTowOperationRouteItem
CTowOperationRouteItem::CTowOperationRouteItem()
{
	m_nRouteID = -1;//route id
	//parent item id
	m_nParentID = -1;
	m_pParentItem = NULL;
	m_enumType = ItemType_Unknown;

	m_nObjectID = -1;//according to ItemType
	//if runway ,runway id
	// taxiway, taxiway id
	// TowRoute, tow route id

	m_nIntersectionID = -1;
	m_strName = _T("");
	m_nRouteType = 0;
}

CTowOperationRouteItem::~CTowOperationRouteItem()
{

}

void CTowOperationRouteItem::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("ROUTEID"),m_nRouteID);
	recordset.GetFieldValue(_T("PARENTID"),m_nParentID);
	int nItemType = 0;
	recordset.GetFieldValue(_T("ITEMTYPE"),nItemType);
	if(nItemType >= ItemType_Unknown && nItemType < ItemType_Count)
	{
		m_enumType = (ItemType)nItemType;
	}
	recordset.GetFieldValue(_T("OBJECTID"),m_nObjectID);
	recordset.GetFieldValue(_T("INTNODEID"),m_nIntersectionID);
	//recordset.GetFieldValue(_T("ITEMNAME"),m_strName);
	recordset.GetFieldValue(_T("ROUTETYPE"),m_nRouteType);
}

void CTowOperationRouteItem::GetInsertSQL( CString& strSQL ) const
{
	SqlInsertScriptMaker sqlInsert(_T("AS_TOWOPFLTDATA_ROUTE"));
	sqlInsert.AddColumn(SqlScriptColumn(_T("ROUTEID"),m_nRouteID));
	sqlInsert.AddColumn(SqlScriptColumn(_T("PARENTID"),m_nParentID));
	sqlInsert.AddColumn(SqlScriptColumn(_T("ITEMTYPE"),(int)m_enumType));
	sqlInsert.AddColumn(SqlScriptColumn(_T("OBJECTID"),m_nObjectID));
	sqlInsert.AddColumn(SqlScriptColumn(_T("INTNODEID"),m_nIntersectionID));
	//sqlInsert.AddColumn(SqlScriptColumn(_T("ITEMNAME"),m_strName));
	sqlInsert.AddColumn(SqlScriptColumn(_T("ROUTETYPE"),m_nRouteType));

	strSQL = sqlInsert.GetScript();

}

void CTowOperationRouteItem::GetUpdateSQL( CString& strSQL ) const
{
	SqlUpdateScriptMaker sqlUpdate(_T("AS_TOWOPFLTDATA_ROUTE"));

	sqlUpdate.AddColumn(SqlScriptColumn(_T("ROUTEID"),m_nRouteID));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("PARENTID"),m_nParentID));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("ITEMTYPE"),(int)m_enumType));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("OBJECTID"),m_nObjectID));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("INTNODEID"),m_nIntersectionID));
	//sqlUpdate.AddColumn(SqlScriptColumn(_T("ITEMNAME"),m_strName));
	sqlUpdate.AddColumn(SqlScriptColumn(_T("ROUTETYPE"),m_nRouteType));
	CString strCondition;
	strCondition.Format(_T("ID = %d"),m_nID);
	sqlUpdate.SetCondition(strCondition);

	strSQL = sqlUpdate.GetScript();
}

void CTowOperationRouteItem::GetDeleteSQL( CString& strSQL ) const
{
	SqlDeleteScriptMaker sqlDelete(_T("AS_TOWOPFLTDATA_ROUTE"));
	
	CString strCondition;
	strCondition.Format(_T("ID = %d"),m_nID);
	sqlDelete.SetCondition(strCondition);

	strSQL = sqlDelete.GetScript();
}

void CTowOperationRouteItem::SaveData( int nParentID )
{
	if(m_nID >=0)//update
	{
		CString strSQL;
		GetUpdateSQL(strSQL);
		
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
	else//save
	{
		CString strSQL;
		GetInsertSQL(strSQL);

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);	

	}
	
	//DBElementCollection<CTowOperationRouteItem>::SaveData(m_nID);


	for (std::vector<CTowOperationRouteItem *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetParentID(m_nID);
		(*iter)->SetRouteID(m_nRouteID);
		(*iter)->SetRouteType(m_nRouteType);
		(*iter)->SaveData(m_nID);
	}

	for (std::vector<CTowOperationRouteItem *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	m_deleteddataList.clear();
}

void CTowOperationRouteItem::DeleteData()
{
	DBElementCollection<CTowOperationRouteItem>::DeleteData();
	if(m_nID >= 0)
	{
		CString strSQL;
		GetDeleteSQL(strSQL);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}


//////////////////////////////////////////////////////////////////////////
//CTowOperationRoute
CTowOperationRoute::CTowOperationRoute( int nFltTypeDataID,int nType )
{
	m_nID = nFltTypeDataID;
	m_nType = nType;
}
CTowOperationRoute::~CTowOperationRoute(void)
{
}

void CTowOperationRoute::DeleteDataFromDB()
{
	for (std::vector<CTowOperationRouteItem *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{

		(*iter)->DeleteData();
	}

	for (std::vector<CTowOperationRouteItem *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}
	m_dataList.clear();
	m_deleteddataList.clear();
}

void CTowOperationRoute::ReadData( int nFltTypeDataID )
{

	SqlSelectScriptMaker sqlSelect(_T("AS_TOWOPFLTDATA_ROUTE"));
	sqlSelect.AddColumn(_T("*"));
	CString strCondition;
	strCondition.Format(_T("ROUTEID = %d AND ROUTETYPE = %d"),nFltTypeDataID,m_nType);
	sqlSelect.SetCondition(strCondition);

	CADORecordset adoRecordset;
	long lCount = 0L;

	std::vector<CTowOperationRouteItem *> vRouteItems;
	CADODatabase::ExecuteSQLStatement(sqlSelect.GetScript(),lCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CTowOperationRouteItem *pNewItem = new CTowOperationRouteItem;
		pNewItem->InitFromDBRecordset(adoRecordset);
		vRouteItems.push_back(pNewItem);
		adoRecordset.MoveNextData();
	}

	ParserItemTree(vRouteItems);
}

void CTowOperationRoute::SaveData( int nFltTypeDataID )
{
	//DBElementCollection<CTowOperationRouteItem >::SaveData(nFltTypeDataID);

	for (std::vector<CTowOperationRouteItem *>::iterator iter = m_dataList.begin();
		iter != m_dataList.end(); ++iter)
	{
		(*iter)->SetRouteID(nFltTypeDataID);
		(*iter)->SetRouteType(m_nType);
		(*iter)->SaveData(m_nID);
	}

	for (std::vector<CTowOperationRouteItem *>::iterator iter = m_deleteddataList.begin();
		iter != m_deleteddataList.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	m_deleteddataList.clear();


}

void CTowOperationRoute::ParserItemTree( std::vector<CTowOperationRouteItem *>& vRouteItem )
{
	ParserHeadItem(vRouteItem);
	
	std::vector<CTowOperationRouteItem *>::iterator iter = vRouteItem.begin();
	for (;iter != vRouteItem.end(); ++iter)
	{
		int nParentID = (*iter)->GetParentID();
		CTowOperationRouteItem *pParnetItem = FindParentItem(nParentID,vRouteItem);
		if(pParnetItem)
		{
			pParnetItem->AddNewItem(*iter);
			(*iter)->SetParentItem(pParnetItem);
		}
	}

}

void CTowOperationRoute::ParserHeadItem( std::vector<CTowOperationRouteItem *>& vRouteItem )
{
	std::vector<CTowOperationRouteItem *>::iterator iter = vRouteItem.begin();
	for (;iter != vRouteItem.end(); ++ iter)
	{
		if((*iter)->GetParentID() == -1)//head
		{
			AddNewItem(*iter);
		}
	}
}

CTowOperationRouteItem * CTowOperationRoute::FindParentItem(int nParentID,std::vector<CTowOperationRouteItem *>& vRouteItem )
{
	std::vector<CTowOperationRouteItem *>::iterator iter = vRouteItem.begin();
	for (;iter != vRouteItem.end(); ++ iter)
	{
		if((*iter)->GetItemID() == nParentID)
			return *iter;
	}
	return NULL;
}































