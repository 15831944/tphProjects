#pragma once 
#include "ApproachSeparationType.h"
#include "AircraftClassifications.h" 
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "..\common\ProbabilityDistribution.h"
#include "../InputAirside/ALT_BIN.h"
#include "RunwaySeparationRelation.h"
#include "InputAirsideAPI.h"

enum SEPSTD
{
	SEPSTD_NONE,//none
	SEPSTD_INTRAIL , //in trail
	SEPSTD_SPATIAL , //spatial
};

class CApproachTypeList;
class CApproachLindingBehindLandedList;
class CApproachLindingBehindTakeOffList;

class INPUTAIRSIDE_API CApproachSeparationCriteria
{
public:
	CApproachSeparationCriteria(int nProjID,const ALTObjectList& vRunways);
	virtual ~CApproachSeparationCriteria(void);
	
	virtual void ReadData(void);
	virtual void SaveData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
	static void ExportApproachSeparationCriteria(CAirsideExportFile& exportFile);
	static void ImportApproachSeparationCriteria(CAirsideImportFile& importFile);

	CApproachLindingBehindLandedList *GetLindingBehindLandedList();
	CApproachLindingBehindTakeOffList *GetLindingBehindTakeOffList();
protected:
	int m_nProjID;
public:
	std::vector<CApproachTypeList*> m_vrApproachTypeList;
	CRunwaySeparationRelation m_runwaySeparationRelation;
public:
	 CString m_OperatorLog ;
};

class CApproachRunwayNode;
class INPUTAIRSIDE_API CApproachTypeList
{
public:
	CApproachTypeList(int nProjID,AIRCRAFTOPERATIONTYPE emApproachType);
	virtual ~CApproachTypeList(void);

	virtual void ReadData(CString _logmsg);
	virtual void SaveData(CString _logmsg);
	virtual void DeleteData(void);
	virtual void ExportData(CAirsideExportFile& exportFile,CString _logmsg);
	virtual void ImportData(CAirsideImportFile& importFile,CString _logmsg);

	virtual int GetNodeCount(void);
	virtual CApproachRunwayNode* GetNode(int nIndex);
	CApproachRunwayNode* GetNode(int nTrailRunwayID, RUNWAY_MARK TrailRunWayMark, int nLeadRunwayID, RUNWAY_MARK LeadRunWayMark);
	virtual void DeleteNode(CApproachRunwayNode* pNodeDelete);
	virtual bool AddNode(CApproachRunwayNode* pNewNode);

	int GetProjectID(void);
	void GetRelatedLogicRunway(int nRunwayID, RUNWAY_MARK runWayMark, std::vector<int > vRunwayConcerned, std::vector<std::pair<int,RUNWAY_MARK> >& vRunway );

protected:
	void Clear(void);
public:
	AIRCRAFTOPERATIONTYPE m_emApproachType;
	int m_nProjID;
	std::vector<CApproachRunwayNode*> m_vrNode;
	std::vector<CApproachRunwayNode*> m_vrNodeDeleted;
public:
	CString m_log ;
};

class INPUTAIRSIDE_API CApproachLindingBehindLandedList:public CApproachTypeList
{
public:
	CApproachLindingBehindLandedList(int nProjID);
	virtual ~CApproachLindingBehindLandedList(void);
};

class INPUTAIRSIDE_API CApproachLindingBehindTakeOffList:public CApproachTypeList
{
public:
	CApproachLindingBehindTakeOffList(int nProjID);
	virtual ~CApproachLindingBehindTakeOffList(void);
};

class CApproachSeparationItem;
class INPUTAIRSIDE_API CApproachRunwayNode
{
public:
	CApproachRunwayNode(int nProjID,AIRCRAFTOPERATIONTYPE emApproachType);
	CApproachRunwayNode(CADORecordset &adoRecordset,CString _logmsg);
	virtual ~CApproachRunwayNode(void);
	virtual void SaveData(int nProjID,CString _logmsg);
	virtual void DeleteData(void);
	virtual void ReadData(int nID,CString _logmsg);
	virtual void ExportData(CAirsideExportFile& exportFile,CString _logmsg);
	virtual void ImportData(CAirsideImportFile& importFile,CString _logmsg);

	virtual int GetItemCount(void);
	virtual CApproachSeparationItem * GetItem(int nIndex);
	CApproachSeparationItem* GetItem(AircraftClassificationItem* pTrailFlightItem, AircraftClassificationItem* pLeadFlightItem);

	virtual void DeleteItem(CApproachSeparationItem *pItemDelete);

	void MakeDataValid(FlightClassificationBasisType emClassType);
	std::vector <CApproachSeparationItem *> GetItemCompounding(FlightClassificationBasisType emClassType);

	BOOL CheckTheItemCountAvailability() ;

	void RemoveClassType(FlightClassificationBasisType emClassType);
protected:
	void Clear(void);
protected:
	int m_nID;
	int m_nProjID;

	std::vector <CApproachSeparationItem *> m_vrAppSep;
	std::vector <CApproachSeparationItem *> m_vrAppSepDeleted;
public:
	RunwayRelationType m_emART;
	RunwayIntersectionType m_emAIT;
	double m_dRw1ThresholdFromInterNode;
	double m_dRw2ThresholdFromInterNode;
public:
	AIRCRAFTOPERATIONTYPE m_emApproachType;
	int m_nFirstRunwayID;
	RUNWAY_MARK m_emFirstRunwayMark;
	int m_nSecondRunwayID;
	RUNWAY_MARK m_emSecondRunwayMark;
	FlightClassificationBasisType m_emClassType;//classification type
	SEPSTD m_emSepStd;//separation standard
	bool m_bLAHSO;//true take the LAHSO,or false do nothing
public:
	CString m_log ;
};

class INPUTAIRSIDE_API CApproachSeparationItem
{
public:
	CApproachSeparationItem(void);
	CApproachSeparationItem(CADORecordset &adoRecordset);
	virtual ~CApproachSeparationItem(void);

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

	FlightClassificationBasisType m_emClassType;
	double m_dMinDistance;
	//¡ê¡§Distance Distribution¡ê?
	CString		m_strDisDistName;
	ProbTypes m_enumDisProbType;  // distribution
	CString m_strDisPrintDist;
	ProbabilityDistribution* m_pDisDistribution;

	double m_dMinTime;
	//¡ê¡§Time Distribution¡ê?
	CString		m_strTimeDistName;
	ProbTypes m_enumTimeProbType;  // distribution
	CString m_strTimePrintDist;
	ProbabilityDistribution* m_pTimeDistribution;
};