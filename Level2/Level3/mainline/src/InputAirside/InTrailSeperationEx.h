#pragma once
#include <vector>
#include "..\common\ProbabilityDistribution.h"
#include "AircraftClassifications.h"

#include "InputAirsideAPI.h"

using namespace std;

class CADORecordset;
enum PhaseType
{
	AllPhase = 0,
	CruisePhase,
	TerminalPhase,
    ApproachPhase
};
/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CInTrailSeparationDataEx
{
public:
	CInTrailSeparationDataEx();
	~CInTrailSeparationDataEx();
public:
	void setID(int nID){ m_nID = nID; }
	int	 getID(){ return m_nID; }

	void setProjID(int nProj) { m_nProjID = nProj;}
	int getProjID() { return m_nProjID;}

	void setInTrailSepID(int nInTrailSepID){ m_nInTrailSepID = nInTrailSepID; }
	int	 getInTrailSepID(){ return m_nInTrailSepID; }

	void setClsTrailItem(AircraftClassificationItem clsTrailItem);
	AircraftClassificationItem* getClsTrailItem();

	void setCategoryType(FlightClassificationBasisType emCategoryType);
	FlightClassificationBasisType getCategoryType();

	void setClsLeadItem(AircraftClassificationItem clsLeadItem);
	AircraftClassificationItem* getClsLeadItem();

	void setMinDistance(long nMinDistance);
	int getMinDistance();

	void setMinTime(long nMinTime);
	int getMinTime();

	void setSpatialConverging(long nSpatialConverging);
	long getSpatialConverging();

	void setSpatialDiverging(long nSpatialDiverging);
	long getSpatialDiverging();

	void setTrailDistName(CString strTrailDistName);
	CString& getTrailDistName();

	void setProbTrailType(ProbTypes emProbType);
	ProbTypes getProbTrailType();

	void setTrailPrintDist(CString strTraiPrintDist);
	CString& getTrailPrintDist();

	void setTimeDistName(CString strTimeDistName);
	CString& getTimeDistName();

	void SetProbTimeType(ProbTypes emProbType);
	ProbTypes getProbTimeType();

	void setTimePrintDist(CString strTimePrintDist);
	CString& getTimePrintDist();

	void setSpatialDistName(CString strSpatialDistName);
	CString& getSpatialDistName();

	void setProbSpatialType(ProbTypes emProbType);
	ProbTypes getProbSpatialType();

	void setSpatialPrintDist(CString strSPatialPrintDist);
	CString& getSpatialPrintDist();

	ProbabilityDistribution* GetTrailProbDistribution(void);
	ProbabilityDistribution* GetTimeProbDistribution(void);
	ProbabilityDistribution* GetSpatialDisProbDistribution(void);

	void InitNewData(FlightClassificationBasisType emCategoryType,int nID);
private:
	int m_nID;
	int m_nProjID;
	int m_nInTrailSepID;

	AircraftClassificationItem m_clsTrailItem;
	AircraftClassificationItem m_clsLeadItem;
	int m_nMinDistance;
	int m_nMinTime;
	long m_nSpatialConverging;
	long m_nSpatialDiverging;	
	FlightClassificationBasisType m_emFltBasisType;
	
	CString m_strTrailDistName;
	ProbTypes m_emProbTrailType;
	CString m_strTrailPrintDist;
	ProbabilityDistribution* m_pTrailDistribution;
	
	CString m_strTimeDistName;
	ProbTypes m_emProbTimeType;
	CString m_strTimePrintDist;
	ProbabilityDistribution* m_pTimeDistribution;
	
	CString m_strSpatialDistName;
	ProbTypes m_emProbSpatialType;
	CString m_strSpatialPrintDist;
	ProbabilityDistribution* m_pSpatialDisDistribution;

public:
	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nInTrailSepID);
	void UpdateData();
	void DeleteData();
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
};


/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CInTrailSeparationEx
{
public:
	CInTrailSeparationEx();
	CInTrailSeparationEx(PhaseType emPahseType,int nSectorID);
	CInTrailSeparationEx(int nProjID);
	~CInTrailSeparationEx();
public:
	void setID(int nID){ m_nID = nID; }
	int	 getID(){ return m_nID; }

	void setProjID(int nProjID){ m_nProjID = nProjID; }
	int	 getProjID(){ return m_nProjID; }

	void setPhaseType(PhaseType emPahseType);
	PhaseType getPhaseType();

	void setCategoryType(FlightClassificationBasisType emCategoryType);
	FlightClassificationBasisType getCategoryType();
	
	void setConvergentUnder(long nConvergentUnder);
	long getConvergentUnder();

	void setDivergentOver(long nDivergentOver);
	long getDivergentOver();

	void setRadiusofConcer(long nRadiusofConcer);
	long getRadiusofConcer();

	void setSectorID(int nSectorID);
	int getSectorID();

	int GetItemCount()const;
	CInTrailSeparationDataEx* GetItem(int nIndex)const;

	void AddItem(CInTrailSeparationDataEx* pInTrailSepItem);
	void Deltem(int nIndex);	

	void DelDataByClass();
	CInTrailSeparationDataEx * GetItemByACClass(const AircraftClassificationItem* trailItem, const AircraftClassificationItem* leadItem)const;

	void InitNewData(int SectorID,PhaseType emPashType,FlightClassificationBasisType emCategoryType);
	void CompleteData(FlightClassificationBasisType emCategoryType);//check data availability and fix it
private:
	int m_nID;
	int m_nProjID;
	int m_nSectorID;
	long m_nConvergentUnder;
	long m_nDivergentOver;
	long m_nRadiusofConcer;
	
	PhaseType m_emPahseType;
	FlightClassificationBasisType m_emCategoryType;
	std::vector<CInTrailSeparationDataEx *> m_vInTrailSepItem;
	std::vector<CInTrailSeparationDataEx *> m_vInTrailSepItemNeedDel;

public:
	BOOL ReadData();
	BOOL SaveData(int nProjID);
	BOOL UpdateData();
	BOOL DeleteData();

public:
	void ReadData(CADORecordset& adoRecordset);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);

public:
	AircraftClassifications* m_pCurClassifications;
	AircraftClassifications* m_pClasNone;
	AircraftClassifications* m_pClasWakeVortexWightBased;
	AircraftClassifications* m_pClasWingspanBased;
	AircraftClassifications* m_pClasSurfaceBearingWeightBased;
	AircraftClassifications* m_pClasApproachSpeedBased;
};


/////////////////////////////////////////////////////////////////////
//
class INPUTAIRSIDE_API CInTrailSeparationListEx
{
public:
	CInTrailSeparationListEx();
	~CInTrailSeparationListEx();
public:
	void setProjID(int nID){ m_nProjID = nID; }
	int	 getProjID(){ return m_nProjID; }

	void setRadiusofConcer(long nRadiusofConcer);
	long getRadiusofConcer();

	int GetItemCount();
	CInTrailSeparationEx* GetItem(int nIndex);
	CInTrailSeparationEx* GetItemByType(int nSectorID, PhaseType emPahseType);
	CInTrailSeparationEx* GetItemByType(int nSectorID);
	CInTrailSeparationEx* GetSectorAllItemByPhaseType(PhaseType emPahseType);
	
	void AddItem(CInTrailSeparationEx* pInTrailSep);
	void Deltem(int nIndex);
	void DeltemBySector(int nSectorID);
	void MoidfyBySector(int nRSectorID,int nDSectorID);
	BOOL CheckBySector(int nSectorID);
	BOOL CheckByPhasType(PhaseType emPahseType,int nSectorID);
	void ModifyRadiusofConcer(long nRadiusofConcer);

	void IsAllSectorAndAllPhaseExist(BOOL& bIsAllSectorExist, BOOL& bIsAllPhaseExist);

public:
	BOOL ReadData(int nProjID);
	BOOL SaveData(int nProjID);
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	static void ImportInTrailSeparation(CAirsideImportFile& importFile);
	static void ExportInTrailSeparation(CAirsideExportFile& exportFile);

private:
	int m_nProjID;
	long m_nRadiusofConcer;

	std::vector<CInTrailSeparationEx *> m_vInTrailSeparation;
	std::vector<CInTrailSeparationEx *> m_vInTrailSepNeedDel;
};

