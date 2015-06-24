#include "StdAfx.h"
#include ".\productstocking.h"

ProductStockingSpecification::ProductStockingSpecification(void)
{
	m_strProductName=_T("Please Edit");
}

ProductStockingSpecification::~ProductStockingSpecification(void)
{
}
void ProductStockingSpecification::GetSelectElementSQL(int nParentID,CString& strSQL) const 
{
	strSQL.Format(_T("SELECT * FROM IN_PRODECTSTOCKING_DETAIL WHERE PARENTID = %d"),m_nID);
}
bool ProductStockingSpecification::GetInsertSQL(int nParentID, CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_PRODECTSTOCKING (RPODUCTNAME) VALUES ('%s')"),m_strProductName);
	return true;
}
bool ProductStockingSpecification::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_PRODECTSTOCKING SET RPODUCTNAME='%s' WHERE ID=%d"),m_strProductName,m_nID);
	return true;
}
void ProductStockingSpecification::GetDataFromDBRecordset(CADORecordset& recordset) 
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("RPODUCTNAME"),m_strProductName);
	CDBDataCollection<ProductStockingItem>::ReadData(-1);
}

void ProductStockingSpecification::DeleteData()
{
	CDBDataCollection<ProductStockingItem>::DeleteData();
	CDBDataCollection<ProductStockingItem>::SaveData(m_nID);
	CDBData::DeleteData();
}
void ProductStockingSpecification::SaveData(int nParentID)
{
	CDBData::SaveData(-1);
	CDBDataCollection<ProductStockingItem>::SaveData(m_nID);
}

bool ProductStockingSpecification::RetrieveShopSectionProduct( std::map<int,int>& productMap, const ProcessorID& procID ) const
{
	ProcessorID key = procID;

	ProductStockingItem* pItem = NULL;
	while(true)
	{
		pItem = FindEntry (key.GetIDString());

		if (pItem)
		{
			std::map<int,int>::iterator iter = productMap.find(pItem->GetID());
			if(iter != productMap.end())
			{
				(*iter).second += static_cast<int>(pItem->GetInventory());
			}
			else
			{
				int nCount = static_cast<int>(pItem->GetInventory());
				productMap.insert(std::make_pair(GetID(),nCount));
			}
			return true;
		}

		// if not found, search its next ancestor
		if (!key.getSuperGroup())
			return false;
	}
}

ProductStockingItem* ProductStockingSpecification::FindEntry( const CString& procID )const
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		ProductStockingItem* pItem = GetItem(i);
		if (procID == pItem->GetShopName())
		{
			return pItem;
		}
	}
	return NULL;
}