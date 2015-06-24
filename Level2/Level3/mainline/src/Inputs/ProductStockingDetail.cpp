#include "StdAfx.h"
#include ".\productstockingdetail.h"

ProductStockingItem::ProductStockingItem(void)
{
	m_dInventory=0;
}

ProductStockingItem::~ProductStockingItem(void)
{
}

CString ProductStockingItem::GetTableName()const
{
	return _T("IN_PRODECTSTOCKING_DETAIL");
}
bool ProductStockingItem::GetInsertSQL(int nParentID, CString& strSQL) const 
{
	strSQL.Format(_T("INSERT INTO IN_PRODECTSTOCKING_DETAIL \
					 (PARENTID,SHOPID,INVENTORY) VALUES \
					 (%d,'%s',%f)"),nParentID,m_strShopName,m_dInventory);
	return true;
}
bool ProductStockingItem::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_PRODECTSTOCKING_DETAIL \
					 SET SHOPID='%s',INVENTORY=%f \
					 WHERE ID=%d"),m_strShopName,m_dInventory,m_nID);
	return true;
}
void ProductStockingItem::GetDataFromDBRecordset(CADORecordset& recordset) 
{
	recordset.GetFieldValue(_T("ID"),m_nID);	
	recordset.GetFieldValue(_T("SHOPID"),m_strShopName);
	recordset.GetFieldValue(_T("INVENTORY"),m_dInventory);
}