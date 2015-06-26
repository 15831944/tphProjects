#pragma once
#include "VehicleDefine.h"
#include "../Database/DBElement.h"
#include "../Database/DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"

class CDlgVehicularMovementAllowed;
class INPUTAIRSIDE_API CVehicularMovementAllowedTypeItemData: public DBElement
{
public:
	CVehicularMovementAllowedTypeItemData();
	CVehicularMovementAllowedTypeItemData(int nVehicleNameID);
	~CVehicularMovementAllowedTypeItemData();


public:
	void SetTaxiwayID(int nTaxiWayID);
	int GetTaxiwayID();

	void SetNodeFrom(int nNodeFrom);
	int GetNodeFrom();

	void SetNodeTo(int nNodeTo);
	int GetNodeTo();

	void SetVehicleNameID(int nNameID);
	int GetVehicleNameID();
	//init data from database recordset
	virtual void InitFromDBRecordset(CADORecordset& recordset);

	bool IsDataValid();
protected:

	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;

protected:
	//int m_nID;
	int m_nTaxiwayID;
	int m_NodeFrom;
	int m_NodeTo;
	int m_nVehicleNameID;

private:
};

class INPUTAIRSIDE_API CVehicularMovementAllowedTypeItem
{
public:
	CVehicularMovementAllowedTypeItem(int nVehicleNameID);
	~CVehicularMovementAllowedTypeItem();
public:
	void AddItem(CVehicularMovementAllowedTypeItemData& item);
	void DelItem(int nIndex);
	
	//get list type
	int getVehicleNameID();
	void SaveData( int nParentID );

	int GetItemCount();
	CVehicularMovementAllowedTypeItemData& GetItem(int nIndex);
	//make sure all the data is valid,
	//return the invalid data index
	int IsDataValid();
protected:
	std::vector<CVehicularMovementAllowedTypeItemData> m_vData;
	int m_nVehicleNameID;
	std::vector<CVehicularMovementAllowedTypeItemData> m_vDeleteData;
private:
};

class INPUTAIRSIDE_API CVehicularMovementAllowed
{
	friend class CDlgVehicularMovementAllowed;
public:
	CVehicularMovementAllowed(int nProjID);
	~CVehicularMovementAllowed(void);

public:
	virtual void ReadData(int nParentID);
	virtual void SaveData(int nParentID);
	
	CVehicularMovementAllowedTypeItem* GetItem(int nVehicleNameID);
	//all the type count
	int GetItemCount();
	//type list name
	//CString GetItemName(int nIndex);
	int GetItemNameID(int nIndex);

	//add data to type data list
	//it will add into the specified list according to its data
	void AddData(CVehicularMovementAllowedTypeItemData& pData);
	void DelData(CVehicularMovementAllowedTypeItemData& pData);
	
	//disable flag
	void SetDisable(bool bDisable);
	bool GetDiable();

protected:
	//read disable all settings flag
	void ReadDisableSettings();
	//save
	void SaveDisbaleSettings();

protected:
	//the sql script to get all the dta
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	std::vector<CVehicularMovementAllowedTypeItem* > m_vTypeData;
	int m_nProjID;
	//void Init();
	
	bool m_bDisable;

};
















