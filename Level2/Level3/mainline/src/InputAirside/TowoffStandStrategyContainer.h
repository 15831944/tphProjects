#pragma once
#include "Common/DBData.h"
#include "TowOffStandAssignmentDataList.h"

class CADORecordset;

class INPUTAIRSIDE_API TowoffStandStrategy : public CDBData
{
public:
	TowoffStandStrategy();
	~TowoffStandStrategy();

	void SetStrategyName(const CString& sName);
	const CString& GetStrategyName()const;

	void SetAirportDatabase(CAirportDatabase* pAirportDB);

	virtual CString GetTableName()const;
	virtual void GetDataFromDBRecordset(CADORecordset& recordset);
	virtual bool GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual bool GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;

	void SaveData(int nParentID);
	void DeleteData();

	CTowOffStandAssignmentDataList* GetTowoffStandDataList();

private:
	CString m_strName;
	CTowOffStandAssignmentDataList m_towoffStandAssignList;
};

class INPUTAIRSIDE_API TowoffStandStrategyContainer : public CDBDataCollection<TowoffStandStrategy>
{
public:
	TowoffStandStrategyContainer(void);
	~TowoffStandStrategyContainer(void);

	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	virtual void ReadData(int nParentID);
	virtual void SaveData(int nParentID);

	void SetSelectStrategy(int Sel);
	int GetSelectStrategy()const;

	TowoffStandStrategy* GetActiveStrategy();

	void SetAirportDatabase(CAirportDatabase* pAirportDB);
private:
	int m_nSelectStrategy;
	CAirportDatabase*  m_pAirportDatabase;
};
