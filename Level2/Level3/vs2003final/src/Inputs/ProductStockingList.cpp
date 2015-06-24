#include "StdAfx.h"
#include ".\productstockinglist.h"
#include "..\CommonData\procid.h"

ProductStockingList::ProductStockingList(void)
{
}

ProductStockingList::~ProductStockingList(void)
{
}
void ProductStockingList::GetSelectElementSQL(int nParentID,CString& strSQL) const 
{
	strSQL.Format(_T("SELECT * FROM IN_PRODECTSTOCKING"));
}
ProductStockingSpecification *ProductStockingList::GetProductByName(CString &strName)
{
	std::vector<ProductStockingSpecification *>::iterator iter;
	for(iter=m_vDatalist.begin();iter!=m_vDatalist.end();iter++)
	{
		if((*iter)->GetProductName()==strName)
		{
			return *iter;
		}
	}
	return NULL;
}

bool ProductStockingList::RetrieveShopSectionProduct( std::map<int,int>& productMap, const ProcessorID& procID ) const
{
	std::vector<ProductStockingSpecification *>::const_iterator iter = m_vDatalist.begin();
	for(;iter!= m_vDatalist.end(); ++iter)
	{
		(*iter)->RetrieveShopSectionProduct(productMap,procID);
	}
	return true;
}

bool ProductStockingList::GetProductName( int nProductID, CString& sProductName ) const
{
	std::vector<ProductStockingSpecification *>::const_iterator iter = m_vDatalist.begin();
	for(;iter!= m_vDatalist.end(); ++iter)
	{
		if ((*iter)->GetID() == nProductID)
		{
			sProductName = (*iter)->GetProductName();
			return true;
		}
	}
	return false;
}