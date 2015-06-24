#pragma once
#include "..\common\dbdata.h"
#include ".\productstockingdetail.h"

class ProductStockingSpecification: public CDBDataCollection<ProductStockingItem>,public CDBData
{
public:
	ProductStockingSpecification(void);
	~ProductStockingSpecification(void);
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const ;

	virtual CString GetTableName()const{return _T("IN_PRODECTSTOCKING");}
	virtual bool GetInsertSQL(int nParentID, CString& strSQL) const;
	virtual bool GetUpdateSQL(CString& strSQL) const;
	virtual void GetDataFromDBRecordset(CADORecordset& recordset);

	void DeleteData();
	void SaveData(int nParentID);

	CString GetProductName(){return m_strProductName;}
	void SetProductName(CString name){m_strProductName=name;}

	
	ProductStockingItem* FindEntry(const CString& procID)const;
	bool RetrieveShopSectionProduct( std::map<int,int>& productMap, const ProcessorID& procID ) const;

private:
	CString m_strProductName;
};
