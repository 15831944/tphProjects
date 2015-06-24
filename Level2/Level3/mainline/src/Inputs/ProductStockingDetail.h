#pragma once
#include "..\common\dbdata.h"
#include "..\CommonData\procid.h"
class ProductStockingItem :
	public CDBData
{
public:
	ProductStockingItem(void);
	~ProductStockingItem(void);
	virtual CString GetTableName()const;
	virtual bool GetInsertSQL(int nParentID, CString& strSQL) const ;
	virtual bool GetUpdateSQL(CString& strSQL) const;
	virtual void GetDataFromDBRecordset(CADORecordset& recordset) ;

	CString GetShopName(){return m_strShopName;}
	void SetShopName(CString &strName){m_strShopName=strName;}
	double GetInventory(){return m_dInventory;}
	void SetInventory(double dInventory){m_dInventory=dInventory;}

private:
	//ProcessorID m_ShopID;
	CString m_strShopName;
	double m_dInventory;
};
