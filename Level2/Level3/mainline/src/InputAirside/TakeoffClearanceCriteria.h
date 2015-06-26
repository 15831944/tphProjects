#pragma once  
#include "ApproachSeparationType.h"
#include "AircraftClassifications.h" 
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\common\ProbabilityDistribution.h"
#include "../InputAirside/ALT_BIN.h"
#include "RunwaySeparationRelation.h"

#include "InputAirsideAPI.h"


class INPUTAIRSIDE_API CTakeoffClaaranceSeparationItem
{
public:
	CTakeoffClaaranceSeparationItem(void);
	CTakeoffClaaranceSeparationItem(CADORecordset &adoRecordset);
	virtual ~CTakeoffClaaranceSeparationItem(void);

public:
	virtual void SaveData(int nParentID);
	virtual void DeleteData(void);
	virtual void ReadData(int nID);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

protected:
	ProbabilityDistribution* GetDistanceProbDistribution(void);
	ProbabilityDistribution* GetTimeProbDistribution(void);
protected:
	int m_nID;
	int m_nParentID;
public:
	AircraftClassificationItem m_aciTrail;//trail flight
	AircraftClassificationItem m_aciLead;//lead flight

	double m_dMinDistance;
	//£¨Distance Distribution£©
	CString		m_strDisDistName;
	ProbTypes m_enumDisProbType;  // distribution
	CString m_strDisPrintDist;
	ProbabilityDistribution* m_pDisDistribution;

	double m_dMinTime;
	//£¨Time Distribution£©
	CString		m_strTimeDistName;
	ProbTypes m_enumTimeProbType;  // distribution
	CString m_strTimePrintDist;
	ProbabilityDistribution* m_pTimeDistribution;
	FlightClassificationBasisType m_emClassType;//classification type
};

class AircraftClassificationItem;
class INPUTAIRSIDE_API CTakeoffClaaranceRunwayNode
{
public:
	CTakeoffClaaranceRunwayNode(int nProjID,AIRCRAFTOPERATIONTYPE emTakeoffClearanceType);
	CTakeoffClaaranceRunwayNode(CADORecordset &adoRecordset);
	virtual ~CTakeoffClaaranceRunwayNode(void);

	virtual void SaveData(int nProjID);
	virtual void DeleteData(void);
	virtual void ReadData(int nID);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

	virtual int GetItemCount(void);
	virtual CTakeoffClaaranceSeparationItem * GetItem(int nIndex);
	CTakeoffClaaranceSeparationItem* GetItem(AircraftClassificationItem* ptrailFlight, AircraftClassificationItem* pLeadFlight);

	virtual void DeleteItem(CTakeoffClaaranceSeparationItem *pItemDelete);

	std::vector <CTakeoffClaaranceSeparationItem *> GetItemCompounding(FlightClassificationBasisType emClassType);
	BOOL CheckTheItemCountAvailability() ;

	void MakeDataValid(FlightClassificationBasisType emClassType);
	void RemoveClassType(FlightClassificationBasisType emClassType);
protected:
	void Clear(void);
protected:
	int m_nID;
	int m_nProjID;

	std::vector <CTakeoffClaaranceSeparationItem *> m_vrTakeoffSep;
	std::vector <CTakeoffClaaranceSeparationItem *> m_vrTakeoffSepDeleted;
public:
	RunwayRelationType m_emRRT;
	RunwayIntersectionType m_emRIT;
	double m_dRw1ThresholdFromInterNode;
	double m_dRw2ThresholdFromInterNode;
public:
	AIRCRAFTOPERATIONTYPE m_emTakeoffClearanceType;
	int m_nFirstRunwayID;
	RUNWAY_MARK m_emFirstRunwayMark;
	int m_nSecondRunwayID;
	RUNWAY_MARK m_emSecondRunwayMark;
	FlightClassificationBasisType m_emClassType;//classification type

	bool m_bClearByTimeOrDistance;//true ,distance;false , time;
	double m_dLeadDistance;
	double m_dTimeAfter;

	bool m_bApproachByTimeOrDistance;//true , distance ; false , time;
	double m_dApproachDistance;
	double m_dApproachTime;

	double m_dDepartureTime;
};



class INPUTAIRSIDE_API CTakeoffClearanceTypeList
{
public:
	CTakeoffClearanceTypeList(int nProjID,AIRCRAFTOPERATIONTYPE emTakeoffClearanceType);
	virtual ~CTakeoffClearanceTypeList(void);

	virtual void ReadData(void);
	virtual void SaveData(void);
	virtual void DeleteData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

	virtual int GetNodeCount(void);
	virtual CTakeoffClaaranceRunwayNode* GetNode(int nIndex);
	virtual void DeleteNode(CTakeoffClaaranceRunwayNode* pNodeDelete);
	virtual bool AddNode(CTakeoffClaaranceRunwayNode* pNewNode);

	int GetProjectID(void);
protected:
	void Clear(void);
public:
	AIRCRAFTOPERATIONTYPE m_emTakeoffClearanceType;
	int m_nProjID;
	std::vector<CTakeoffClaaranceRunwayNode*> m_vrNode;
	std::vector<CTakeoffClaaranceRunwayNode*> m_vrNodeDeleted;
};

class INPUTAIRSIDE_API CTakeoffClaaranceTakeOffBehindLandedList: public CTakeoffClearanceTypeList
{
public:
	CTakeoffClaaranceTakeOffBehindLandedList(int nProjID);
	virtual ~CTakeoffClaaranceTakeOffBehindLandedList(void);
};

class INPUTAIRSIDE_API CTakeoffClaaranceTakeOffBehindTakeOffList: public CTakeoffClearanceTypeList
{
public:
	CTakeoffClaaranceTakeOffBehindTakeOffList(int nProjID);
	virtual ~CTakeoffClaaranceTakeOffBehindTakeOffList(void);
};


class INPUTAIRSIDE_API CTakeoffClearanceCriteria
{
public:
	CTakeoffClearanceCriteria(int nProjID,const ALTObjectList& vRunways);
	virtual ~CTakeoffClearanceCriteria(void);

	virtual void ReadData(void);
	virtual void SaveData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
	static void ExportTakeoffClearanceCriteria(CAirsideExportFile& exportFile);
	static void ImportTakeoffClearanceCriteria(CAirsideImportFile& importFile);

	ElapsedTime GetTakeoffSeparationTime(int nRunwayID, RUNWAY_MARK runWayMark,  AIRCRAFTOPERATIONTYPE aircraftOperType);
	double GetTakeoffSeparationDistance(int nRunwayID, RUNWAY_MARK runWayMark,  AIRCRAFTOPERATIONTYPE aircraftOperType);
	//time:return true  distance:return false
	BOOL IsTimeOrDistance(int nRunwayID, RUNWAY_MARK runWayMark,  AIRCRAFTOPERATIONTYPE aircraftOperType);

	CTakeoffClaaranceRunwayNode* GetTakeoffClearanceRunwayNode(int nTrailRunwayID, RUNWAY_MARK TrailRunWayMark, int nLeadRunwayID, RUNWAY_MARK LeadRunWayMark, AIRCRAFTOPERATIONTYPE aircraftOperType);
	void GetRelatedLogicRunway(int nRunwayID, RUNWAY_MARK runWayMark, std::vector<int > vRunwayConcerned,std::vector<std::pair<int,RUNWAY_MARK> >& vRunway, AIRCRAFTOPERATIONTYPE aircraftOperType );

public:
	CTakeoffClaaranceTakeOffBehindLandedList * GetTakeoffBehindLandingList();
	CTakeoffClaaranceTakeOffBehindTakeOffList * GetTakeoffBehindTakeOffList();
	BOOL IsLandingTimeOrDistance(int nRunwayID, RUNWAY_MARK runWayMark,  AIRCRAFTOPERATIONTYPE aircraftOperType);
	double GetLandingSeparationDistance(int nRunwayID, RUNWAY_MARK runWayMark, AIRCRAFTOPERATIONTYPE aircraftOperType);
	ElapsedTime GetLandingSeparationTime(int nRunwayID, RUNWAY_MARK runWayMark, AIRCRAFTOPERATIONTYPE aircraftOperType);

protected:
	int m_nProjID;
public:
	std::vector<CTakeoffClearanceTypeList*> m_vrTakeoffClearanceTypeList;
	CRunwaySeparationRelation m_runwaySeparationRelation;


};
