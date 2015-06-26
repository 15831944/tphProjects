#pragma once
#include "ApproachSeparationType.h"
#include "AircraftClassifications.h" 
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;
class CADORecordset;

class AircraftClassificationItem;

class INPUTAIRSIDE_API ApproachSeparationItem
{
public:
	ApproachSeparationItem(int nProjID,ApproachSeparationType emType);
	virtual ~ApproachSeparationItem(void);

	//read record in m_pADORecordSet
	virtual void ReadData();

	//this base class always delete exist record match m_nProjID and m_emType;
	virtual void SaveData();

	ApproachSeparationType GetType(){	return m_emType; }
protected:
	int m_nProjID;
	ApproachSeparationType m_emType;//you need this variable to known which field is valid in DB table ApproachSeparationCriteria.
public:
	virtual void ImportData(CAirsideImportFile& importFile);
	virtual void ExportData(CAirsideExportFile& exportFile);
};

class INPUTAIRSIDE_API AircraftClassificationBasisItem
{
public:
	AircraftClassificationBasisItem()
		:m_nID(-1)
		,m_nProjID(-1)	
		,m_aciLead(WakeVortexWightBased)
		,m_aciTrail(WakeVortexWightBased)
		,m_emClassifiBasis(WakeVortexWightBased)
		,m_dMinSeparation(926000.0)
		,m_dTimeDiffInFltPathIntersecting(120.0)
		,m_dTimeDiffOnRunwayIntersecting(120.0)
		,m_dDistanceDiffInFltPathIntersecting(10000.0)
		,m_dDistanceDiffOnRunwayIntersecting(10000.0)
	{
	}

	~AircraftClassificationBasisItem(){}

public:
	int m_nID;
	int m_nProjID;
	FlightClassificationBasisType m_emClassifiBasis;//aircraft classification.	
	AircraftClassificationItem m_aciTrail;//trail flight
	AircraftClassificationItem m_aciLead;//lead flight
	double m_dMinSeparation;
	double m_dTimeDiffInFltPathIntersecting;
	double m_dTimeDiffOnRunwayIntersecting;
	double m_dDistanceDiffInFltPathIntersecting;
	double m_dDistanceDiffOnRunwayIntersecting;

public:
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	void SaveData(ApproachSeparationType emType);
};
typedef std::vector<AircraftClassificationBasisItem*>  ACBasisItemVector;
typedef std::vector<AircraftClassificationBasisItem*>::iterator ACBasisItemIter;

class INPUTAIRSIDE_API AircraftClassificationBasisApproachSeparationItem : public ApproachSeparationItem
{
public:
	AircraftClassificationBasisApproachSeparationItem(int nProjID, ApproachSeparationType emType,FlightClassificationBasisType emFltTypeDefault = (FlightClassificationBasisType)-1);
	~AircraftClassificationBasisApproachSeparationItem(void);

	//if table "AIRCRAFTCLASSIFICATION" has no record's id match  TRAILID or LEADID ,then ReadData() delete this record;
	void ReadData();

	void SaveData();

	//it reads record from table "AIRCRAFTCLASSIFICATION" , and make a data struct with emFltType ,
	//if had been ReadData(),it do nothing.
	bool ShowItemsByFltType(FlightClassificationBasisType emFltType);

	ACBasisItemVector * GetItemsVector(void){return(&m_vectClassfiBasis);}
	AircraftClassificationBasisItem * GetItem(const AircraftClassificationItem& leadAc, const AircraftClassificationItem& trailAC)const;
	FlightClassificationBasisType GetFltType(void){return (m_emFltClassBasis);} 
protected:
	void Clear();
	bool DeleteRecord(int nID);
	ACBasisItemVector m_vectClassfiBasis;
	FlightClassificationBasisType m_emFltClassBasis;//aircraft classification.	

public:
	virtual void ImportData(CAirsideImportFile& importFile);
	virtual void ExportData(CAirsideExportFile& exportFile);
};
