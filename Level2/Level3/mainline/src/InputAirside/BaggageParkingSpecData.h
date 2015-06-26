#pragma once
#include "Common/path.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API BaggageCartParkingItem : public DBElement
{

public:
	BaggageCartParkingItem();

	void SetParkArea(const Path& path);
	Path GetParkArea()const;
	
	CString GetPushers()const{ return m_strPushers;}
	BaggageCartParkingItem& SetPushers(const CString& strPushers);


public:
	void DeleteData();
	void SaveData(int nPrjID);

	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

public:
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
	

protected:
	CString m_strPushers;
	Path m_parkArea;
	int m_nPathId;

};



class INPUTAIRSIDE_API BaggageCartParkingSpecData : public DBElementCollection<BaggageCartParkingItem> 
{
public:
	BaggageCartParkingSpecData(void){}
	~BaggageCartParkingSpecData(void){}

	void GetSelectElementSQL(int nParentID,CString& strSQL)const;

	static void ImportBagCartParking(CAirsideImportFile& importFile);
	static void ExportBagCartParking(CAirsideExportFile& exportFile);
protected:
	
private:
	//export dll symboles
	void DoNotCall();
};
