#pragma once

#include "InputAirsideAPI.h"

enum OrientationType
{	
	Orientation_unknown = 0,
	Longitudinal,
	Angled,

	Orientation_Count,
};

class CADORecordset;
class CAirsideImportFile;
class CAirsideExportFile;
/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CTemporaryParkingCriteriaData
{
public:
	CTemporaryParkingCriteriaData();
	~CTemporaryParkingCriteriaData();

public:
	void setID(int nID){ m_nID = nID; }
	int	 getID(){ return m_nID; }

	void SetStartID(int nStartID);
	int	 GetStartID();

	void SetEndID(int nEndID);
	int	 GetEndID();

	void SetEntryID(int nEntryID);
	int	 GetEntryID();

	void SetOrientationType(OrientationType emType);
	OrientationType	 GetOrientationType();

public:
	void ReadData(CADORecordset& adoRecordset);
	void UpdateData();
	void DeleteData();
	void SaveData(int nTempParkID);

private:
	int m_nID;	
	int m_nStartID;
	int m_nEndID;
	int m_nEntryID;
	OrientationType m_emType;

public:
	void ExportData(CAirsideExportFile& exportFile);

	void ImportData(CAirsideImportFile& importFile,int nTempParkID);
};


/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CTemporaryParkingCriteria
{
public:
	CTemporaryParkingCriteria();
	~CTemporaryParkingCriteria();

public:
	void setID(int nID){ m_nID = nID; }
	int	 getID(){ return m_nID; }

	void SetTaxiwayID(int nTaxiwayID);
	int	 GetTaxiwayID();

	void AddItem(CTemporaryParkingCriteriaData* pNewItem);
	void DelItem(CTemporaryParkingCriteriaData* pDelItem);

	CTemporaryParkingCriteriaData* GetItem(int nIndex);
	int GetItemCount();


public:
	void ReadData();
	void ReadData(CADORecordset& adoRecordset);
	void DeleteData();
	void UpdateData();
	void SaveData(int nprojID);
protected:
	void SaveParkingData();

private:
	int m_nID;
	int m_nTaxiwayID;

	std::vector<CTemporaryParkingCriteriaData *> m_vParkingData;
	std::vector<CTemporaryParkingCriteriaData *> m_vParkingDataNeedDel;


public:
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

};

/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CTemporaryParkingCriteriaList
{
public:
	CTemporaryParkingCriteriaList();
	~CTemporaryParkingCriteriaList();

public:
	void setProjID(int nProjID){ m_nProjID = nProjID; }
	int	 getProjID(){ return m_nProjID; }

	void AddItem(CTemporaryParkingCriteria* pNewItem);
	void DelItem(CTemporaryParkingCriteria* pDelItem);
	void DelItem(int nIndex);

	CTemporaryParkingCriteria *GetItemByID(int nID);

	CTemporaryParkingCriteria* GetItem(int nIndex);
	int GetItemCount();

private:
	int m_nProjID;

	std::vector<CTemporaryParkingCriteria *> m_vParking;
	std::vector<CTemporaryParkingCriteria *> m_vParkingNeedDel;

public:
	void ReadData(int nprojID);
	void SaveData(int nProjID);


public:
	static void ExportTemporaryParkingCriterias(CAirsideExportFile& exportFile);

	static void ImportTemporaryParkingCriterias(CAirsideImportFile& importFile);

	void ExportTemporaryParkingCriteria(CAirsideExportFile& exportFile);

	void ImportTemporaryParkingCriteria(CAirsideImportFile& importFile);

};