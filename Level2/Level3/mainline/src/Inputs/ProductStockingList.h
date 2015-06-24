#pragma once
#include "..\common\dbdata.h"
#include ".\productstocking.h"
class ProcessorID;
class ProductStockingList:public CDBDataCollection<ProductStockingSpecification>
{
public:
	ProductStockingList(void);
	~ProductStockingList(void);
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const ;
	ProductStockingSpecification *GetProductByName(CString &strName);

	//-------------------------------------------------------------------------
	//Summary:
	//		retrieve shop section processor product type and count
	//-------------------------------------------------------------------------
	bool RetrieveShopSectionProduct(std::map<int,int>& productMap,const ProcessorID& procID  )const;

	//-------------------------------------------------------------------------
	bool GetProductName(int nProductID, CString& sProductName)const;
};
