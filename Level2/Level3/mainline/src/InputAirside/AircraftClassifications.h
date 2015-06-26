#pragma once
#include "ApproachSeparationType.h"
#include <vector>

#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API AircraftClassificationItem
{
public:
	AircraftClassificationItem(FlightClassificationBasisType emType = NoneBased);
	~AircraftClassificationItem();
	void setID(int nID);
	void setName(CString strName);
	void setMaxValue(int nMax);
	void setMinValue(int nMin);

	void SetProjectID(int nProjID);
	void SetType(FlightClassificationBasisType emType);

	CString getName(){	return m_strName; }

	//if return false , it means the record with nID is not exist.
	bool ReadData(int nID );

	int getID()const{	return m_nID; }

	void SaveData();
	void UpdateData();
	void DeleteData();

	bool IsFitIn(double dval)const;

	int m_nID;
	CString m_strName;
	int m_nMinValue;
	int m_nMaxValue;
	int m_nProjID;
	FlightClassificationBasisType m_emType;
	//type
	//ProjID
	
	bool operator == (const AircraftClassificationItem& item)const;
	bool operator < (const AircraftClassificationItem& item)const;
	bool operator > (const AircraftClassificationItem& item)const;
public:
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
};


class INPUTAIRSIDE_API AircraftClassifications
{
public: 
	typedef std::vector<AircraftClassificationItem*> AircraftCategoryList;
	typedef std::vector<AircraftClassificationItem*>::iterator AircraftCategoryIter;
	typedef std::vector<AircraftClassificationItem*>::const_iterator AircraftCategoryConstIter;

public:
	AircraftClassifications();
	explicit AircraftClassifications(int nProjID, FlightClassificationBasisType emType);
	~AircraftClassifications(void);

	void SetProjectID(int nProjID);
	void SetBasisType(FlightClassificationBasisType cType); 
	FlightClassificationBasisType GetBasisType();
	void ReadData();
	void SaveData();

	void InsertDefaultValueIfNeed();

	void AddItem(AircraftClassificationItem* pItem);
	void DeleteItem(AircraftClassificationItem* pItem);
	AircraftClassificationItem*  GetItem(int nIndex);

	AircraftClassificationItem*  GetItemByID(int nID);
	
	int GetCount();

	AircraftClassificationItem * GetFitInItem(double dval);
	int GetMaxValue(int idx);

	void sort();

protected:
	AircraftClassificationItem* GetWakeVortexFitInItem(double dval);
	AircraftClassificationItem* GetFitInItemExceptWakeVortex(double dval);
private:
	int m_nProjID;

	FlightClassificationBasisType m_emType;
	AircraftCategoryList m_vectClassification;
	AircraftCategoryList m_vectNeedToDel;


public:
	static void ExportData(CAirsideExportFile& exportFile);
	static void ImportData(CAirsideImportFile& importFile);

	void ExportAircraftClassificatins(CAirsideExportFile& exportFile);

};
