#pragma once
#include <vector>
#include "..\InputAirside\AircraftClassifications.h"
#include "InTrailSeperationEx.h"
#include "InputAirsideAPI.h"

using   namespace   std;

class CADORecordset;
//enum PhaseType;
//{
//	PHASETYPE_UNKNOWN = 0,
//	CruisePhase,
//	TerminalPhase,
//    ApproachPhase,
//
//	
//	PHASETYPE_COUNT
//};
/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CInTrailSeparationData
{
public:
	CInTrailSeparationData();
	~CInTrailSeparationData();
public:
	void setID(int nID){ m_nID = nID; }
	int	 getID(){ return m_nID; }

	void setInTrailSepID(int nInTrailSepID){ m_nInTrailSepID = nInTrailSepID; }
	int	 getInTrailSepID(){ return m_nInTrailSepID; }

	void setClsTrailItem(AircraftClassificationItem clsTrailItem);
	AircraftClassificationItem* getClsTrailItem();

	void setClsLeadItem(AircraftClassificationItem clsLeadItem);
	AircraftClassificationItem* getClsLeadItem();

	void setMinDistance(long nMinDistance);
	int getMinDistance();

	void setMinTime(long nMinTime);
	int getMinTime();

private:
	int m_nID;
	int m_nInTrailSepID;

	AircraftClassificationItem m_clsTrailItem;
	AircraftClassificationItem m_clsLeadItem;
	long m_nMinDistance;
	long m_nMinTime;

public:
	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nInTrailSepID);
	void UpdateData();
	void DeleteData();
};


/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CInTrailSeparation
{
public:
	CInTrailSeparation();
	CInTrailSeparation(PhaseType emPahseType, FlightClassificationBasisType emCategoryType);
	~CInTrailSeparation();
public:
	void setID(int nID){ m_nID = nID; }
	int	 getID(){ return m_nID; }

	void setProjID(int nProjID){ m_nProjID = nProjID; }
	int	 getProjID(){ return m_nProjID; }

	void setPhaseType(PhaseType emPahseType);
	PhaseType getPhaseType();

	void setCategoryType(FlightClassificationBasisType emCategoryType);
	FlightClassificationBasisType getCategoryType();
	
	int GetItemCount()const;
	CInTrailSeparationData* GetItem(int nIndex)const;

	void AddItem(CInTrailSeparationData* pInTrailSepItem);
	void Deltem(int nIndex);	

	CInTrailSeparationData * GetItemByACClass(const AircraftClassificationItem* trailItem, const AircraftClassificationItem* leadItem)const;

	void SetIsActive(BOOL bIsActive);
	BOOL IsActive();
private:
	int m_nID;
	int m_nProjID;
	
	PhaseType m_emPahseType;
	FlightClassificationBasisType m_emCategoryType;
	BOOL m_bIsActive;    //add by adam 2007-04-07
	std::vector<CInTrailSeparationData *> m_vInTrailSepItem;
	std::vector<CInTrailSeparationData *> m_vInTrailSepItemNeedDel;

public:
	BOOL ReadData();
	BOOL SaveData(int nProjID);
	BOOL UpdateData();
	BOOL DeleteData();

public:
	void ReadData(CADORecordset& adoRecordset);
};


/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CInTrailSeparationList
{
public:
	CInTrailSeparationList();
	~CInTrailSeparationList();
public:
	void setProjID(int nID){ m_nProjID = nID; }
	int	 getProjID(){ return m_nProjID; }

	int GetItemCount();
	CInTrailSeparation* GetItem(int nIndex);
	CInTrailSeparation* GetItemByType(PhaseType emPahseType, FlightClassificationBasisType emCategoryType);
	CInTrailSeparation* GetItemByType(PhaseType emPahseType);
	void SetItemActive(PhaseType emPahseType, FlightClassificationBasisType emCategoryType, BOOL bActive);
	
	void AddItem(CInTrailSeparation* pInTrailSep);
	void Deltem(int nIndex);
	void DelItemByType(PhaseType emPahseType);

public:
	BOOL ReadData(int nProjID);
	BOOL SaveData(int nProjID);

private:
	int m_nProjID;

	std::vector<CInTrailSeparation *> m_vInTrailSeparation;
	std::vector<CInTrailSeparation *> m_vInTrailSepNeedDel;
};

