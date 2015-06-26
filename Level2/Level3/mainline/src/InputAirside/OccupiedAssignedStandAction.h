#pragma once

#include <vector>
#include "../Database/ADODatabase.h"
#include "..\AirsideGUI\NodeViewDbClickHandler.h"
#include "../Common/Flt_cnst.h"
#include "InputAirsideAPI.h"
//#include "..\Engine\Airside\AirsideFlightInSim.h"
#include "..\Engine\Airside\RunwayInSim.h"


// enum StandPriority
// {
// 	NA = 0,
// 	Priority1,
// 	Priority2,
// 	Priority3,
// 	Priority4,
// 	Priority5
// };
enum StrategyType
{
	
	ToDynamicalStand= 0,
	ToIntersection,	
	ToTemporaryParking,
	ToStand,
	Delaytime ,

	StrategyNum//-should be the last
};
typedef int ObjID;
typedef vector<int> ObjIDList;
typedef vector<StrategyType> StrategyTypeList;

class INPUTAIRSIDE_API CirculateRoute : public  DBElementCollection<CirculateRoute>
{
public:
	CirculateRoute(void);
	virtual ~CirculateRoute(void);

	void SetParentID(int nParentID);
	int GetParentID()const;
	void SetRouteID(int nRouteID);
	int GetRouteID()const;
	void SetALTObjectID(int nALTObjectID);
	int GetALTObjectID()const;
	void SetIntersectNodeID(int nINodeID);
	int GetIntersectNodeID()const; // only use it in GUI, unreliable method if used in engine
	void SetItemName(CString strName);
	CString GetItemName()const;

public:
	void DeleteDataFromDB();
	void SaveDataToDB();
	void SaveData(int nParentID);

	//DBElementCollection
	virtual void DeleteData();
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;



private:
	int m_nParentID;
	int m_nRouteID;
	int m_nAltObjectID;      //taxiway id, ground group id
	int m_nIntersectNodeID;  //the node id which intersect with it's parent object, only use it in GUI, unreliable if used in engine
	CString m_strItemName;

};

class INPUTAIRSIDE_API CirclateRouteStrategy : public cpputil::noncopyable
{
public:
	CirclateRouteStrategy();
	~CirclateRouteStrategy();

	void SetIntersectionID(int nIntersectionID);
	int GetIntersectionID()const;

	void SetStrategyType(int iType);
	int GetStrategyType()const;

	void ReadData(int nParentID);
	void SaveDataToDB(int nParentID);
	void DeleteData();
	CirculateRoute* GetCirculateRoute();
private:
	int m_iType; //0 Intersection; 1  interrupt
	int m_nIntersectionID;
	CirculateRoute* m_pCirRoute;
};

//--------------------COccupiedAssignedStandStrategy--------------------
class INPUTAIRSIDE_API COccupiedAssignedStandStrategy
{
public:
	COccupiedAssignedStandStrategy();
	~COccupiedAssignedStandStrategy();

	void SetDelaytime(int nDelaytime);
	int GetDelaytime();
	void SetIntersectionID(int nIntersectionID);
	int GetIntersectionID();
	void SetStrategyType(int iType);
	int GetStrategyType()const;
	void SetTimes(int iTime);
	int GetTimes()const;

	ObjIDList &GetTmpParkingList();
	ObjIDList &GetTmpStandList();
	void SetTmpParkingList(const ObjIDList &parkingList);
	void SetTmpStandList(const ObjIDList &standList);
	const StrategyTypeList &GetOrder();
	void StrategyUp(int strategyIndex);
	void StrategyDown(int strategyIndex);
	void addTmpParking(ObjID tmpParking);
	void deleteTmpParking(int parkingIndex);
	void addTmpStand(ObjID tmpStand);
	void deleteTmpStand(int standIndex);
	COccupiedAssignedStandStrategy& operator =(COccupiedAssignedStandStrategy &strategy);
	
	void addDefaultData();
	void ReadData(CADORecordset& adoRecordset);
	void DeleteData();
	void SaveData(int nTimeWinID);

 	void CleanData();

	CirculateRoute* GetCirculateRoute();


// 	void SetPriority(StandPriority enumPriority){m_enumPriority = enumPriority;}
// 	StandPriority GetPriority(){return m_enumPriority;}
// 	void SetStrategyType(StrategyType enumStrategyType){m_enumStrategyType = enumStrategyType;}
// 	StrategyType GetStrategyType(){return m_enumStrategyType;}
//void SetIntersectionTaxiway1ID(int nIntersectionTaxiway1ID){m_nIntersectionTaxiway1ID = nIntersectionTaxiway1ID;}
//int GetIntersectionTaxiway1ID(){return m_nIntersectionTaxiway1ID;}
//void SetIntersectionTaxiway2ID(int nIntersectionTaxiway2ID){m_nIntersectionTaxiway2ID = nIntersectionTaxiway2ID;}
//int GetIntersectionTaxiway2ID(){return m_nIntersectionTaxiway2ID;}
//void SetIntersectionIndex(int nIntersectionIndex){m_nIntersectionIndex = nIntersectionIndex;}
//int GetIntersectionIndex(){return m_nIntersectionIndex;}


// 	void SetStandID(int nStandID){m_nStandID = nStandID;}
// 	int GetStandID(){return m_nStandID;}

// 	void SetTemporaryParkingID(int nTemporaryParkingID){m_nTemporaryParkingID = nTemporaryParkingID;}
// 	int GetTemporaryParkingID(){return m_nTemporaryParkingID;}
// 	void ExportData(CAirsideExportFile& exportFile);
// 	void ImportData(CAirsideImportFile& importFile,int nStandFltID);

private:
	int m_nID;
	int m_nDelaytime;
// 	int m_nIntersectionID;
	CirclateRouteStrategy m_cirRouteStrategy;
	ObjIDList m_vTmpParkingList;
// 	ObjIDList m_vDelTmpParkingList;
	ObjIDList m_vTmpStandList;
// 	ObjIDList m_vDelTmpStandList;
	StrategyTypeList m_vOrder;	//the order of Priorities
	int m_iTime;//N times
					
};

//-----------------CAirSideCreteriaTimeWin---------------
class INPUTAIRSIDE_API CAirSideCreteriaTimeWin
{
public:
	CAirSideCreteriaTimeWin();

	~CAirSideCreteriaTimeWin();

	void SetStartTime(ElapsedTime& tStart);

	ElapsedTime GetStartTime();

	void SetEndTime(ElapsedTime& tEnd);

	ElapsedTime GetEndTime();

	COccupiedAssignedStandStrategy &GetStrategy();

	void GetStrategy(COccupiedAssignedStandStrategy &strategy);

	bool inTimeWindow(ElapsedTime time);


	void ReadData(CADORecordset& adoRecordset);

	void DeleteData();

	void SaveData(int nFlightTypeID);

	void CleanData();


private:
	int m_nID;
	ElapsedTime m_tStartTime;
	ElapsedTime m_tEndTime;
	COccupiedAssignedStandStrategy m_Strategy;
};

//----------------CAirSideCriteriaFlightType--------------------

typedef vector<CAirSideCreteriaTimeWin *> TimeWindowList;

class INPUTAIRSIDE_API CAirSideCriteriaFlightType/*:public CDBDataCollection*/
{	
public:
	
	CAirSideCriteriaFlightType();
	~CAirSideCriteriaFlightType();
	FlightConstraint &getFlightType();
	void setFlightType(const CString strFlightType);
	void setFlightType(const FlightConstraint &flightType);
	TimeWindowList &getTimeWindowList();
	void addTimeWindow(CAirSideCreteriaTimeWin *timeWindow);
	void delTimeWindow(int timeWinIndex);
	CAirSideCreteriaTimeWin *getTimeWindow(int timeWinIndex);
	bool isDefaultFlightType();
	void SetAirportDB(CAirportDatabase* pAirportDB);

	CAirSideCreteriaTimeWin *addDefaultData();

	void ReadData(CADORecordset& adoRecordset);

	void DeleteData();

	void SaveData(int nStandsID);

	void CleanData();

protected:
private:
	int m_nID;
	FlightConstraint m_FlightType;
	TimeWindowList m_vTimeWinList;
	TimeWindowList m_vDelTimeWinList;
// 	CAirportDatabase* m_pAirportDB;
};
//----------------CAirSideCriteriaParkingStands-------------------
typedef vector<int> ObjIDList;
typedef vector<CAirSideCriteriaFlightType *> FlightTypeList;

class INPUTAIRSIDE_API CAirSideCriteriaParkingStands
{
public:
	CAirSideCriteriaParkingStands();
	~CAirSideCriteriaParkingStands();
	ObjIDList &getParkingStandList();
	FlightTypeList &getFlightTypeList();
	void addParkingStand(ObjID tmpParkingStand);
	void delParkingStand(int parkingStandIndex);
	void setParkingStandList(const ObjIDList &vParkingStandList);
	void addFlightType(CAirSideCriteriaFlightType *flightType);
	void delFlightType(int flightTypeIndex);
	CAirSideCriteriaFlightType *getFlightType(int flightTypeIndex); 
	void SetAirportDB(CAirportDatabase* pAirportDB);
	bool isAllParkingStand();
	CAirSideCriteriaFlightType *addDefaultData();
	int getStandMatch(ALTObjectID standID);
	int standMatch(ALTObjectID standID1,ALTObjectID standID2);

	void ReadData(CADORecordset& adoRecordset);

	void DeleteData();//delete data from database

	void SaveData(int nExitsID);

	void CleanData();//clean content


protected:
private:
	int m_nID;
	ObjIDList m_vParkingStandList;
	FlightTypeList m_vFlightTypeList;
	FlightTypeList m_vDelFlightTypeList;
	CAirportDatabase* m_pAirportDB;
};

//--------------------CAirSideCriteriaExits----------------------
typedef vector<CAirSideCriteriaParkingStands *> ParkingStandsList;
class INPUTAIRSIDE_API CAirSideCriteriaExits
{
public:
	CAirSideCriteriaExits();
	~CAirSideCriteriaExits();	
	ObjIDList &getExitList();
	ParkingStandsList &getParkingStandsList();
	void addExit(ObjID exit);
	void delExit(int exitIndex);
	void setExitList(const ObjIDList &vExitList);
// 	void setAllExit();
	void addParkingStands(CAirSideCriteriaParkingStands *parkingStands);
	void delParkingStands(int parkingStandsIndex);
	CAirSideCriteriaParkingStands *getParkingStands(int parkingStandsIndex);
	void SetAirportDB(CAirportDatabase* pAirportDB);
	bool IsAllRunwayExit();//m_vExitList is empty means all exits are selected

	CAirSideCriteriaParkingStands *addDefaultData();
	
	void ReadData(CADORecordset& adoRecordset);

	void DeleteData();//delete data from database

	void SaveData(int projectID,int parentID=0);

	void CleanData();//clean content



protected:
private:
	int m_nID;
	ObjIDList m_vExitList;
	ParkingStandsList m_vParkingStandsList;
	ParkingStandsList m_vDelParkingStandsList;
	CAirportDatabase* m_pAirportDB;

};

typedef vector<CAirSideCriteriaExits *> ExitsList;
class INPUTAIRSIDE_API COccupiedAssignedStandCriteria
{
public:
	COccupiedAssignedStandCriteria();
	~COccupiedAssignedStandCriteria();
	ExitsList &getExitsList();
	void addExits(CAirSideCriteriaExits* exits);
	void delExits(int exitsIndex);
	CAirSideCriteriaExits*getExits(int exitsIndex);
	void addDefaultData();
	void SaveData();
	void ReadData();
	void CleanData();
	void SetAirportDB(CAirportDatabase* pAirportDB);
	void SetPrjID(int prjID);
// 	COccupiedAssignedStandStrategy GetStrategyByFlightType(AirsideFlightInSim* pFlight);
	


protected:
private:
	CAirportDatabase* m_pAirportDB;
	int m_nPrjID;
	ExitsList m_vExitsList;
	ExitsList m_vDelExitsList;
};

//////--c--///////-----------should be deleted---------------------

//--------------------COccupiedAssignedStandFltType--------------------
/*
class FlightConstraint;
class CAirportDatabase;
class INPUTAIRSIDE_API COccupiedAssignedStandFltType
{
public:
	typedef std::vector<COccupiedAssignedStandPriority*> OccupiedAssignedStandPriorityVector;
	typedef std::vector<COccupiedAssignedStandPriority*>::iterator OccupiedAssignedStandPriorityIter;
public:
	COccupiedAssignedStandFltType();
	~COccupiedAssignedStandFltType();

	int GetID(){return m_nID;}
	void ReadData(CADORecordset& adoRecordset);
	void ReadPriorityData();
	void SaveData(int nStandActionID);
	void UpdateData();
	void DeleteData();

	int GetPriorityCount();
	COccupiedAssignedStandPriority* GetPriorityItem(int nIndex);

	void AddPriorityItem(COccupiedAssignedStandPriority* pItem);
	void DelPriorityItem(COccupiedAssignedStandPriority* pItem);

	void RemoveAll();

	FlightConstraint GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	void ImportSaveData(int nStandActionID,CString strFltType);

private:
	int m_nID;
	FlightConstraint m_fltType;
	CAirportDatabase* m_pAirportDB;
	OccupiedAssignedStandPriorityVector m_vPriority;
	OccupiedAssignedStandPriorityVector m_vNeedDelPriority;
};

//-------------------COccupiedAssignedStandAction---------------------
class INPUTAIRSIDE_API COccupiedAssignedStandAction
{
public:
	typedef std::vector<COccupiedAssignedStandFltType*> OccupiedAssignedStandFltTypeVector;
	typedef std::vector<COccupiedAssignedStandFltType*>::iterator OccupiedAssignedStandFltTypeIter;
public:
	 COccupiedAssignedStandAction();
	~COccupiedAssignedStandAction();

	void ReadData(int nProjID);
	void SaveData(int nProjID);

	int GetFltTypeCount();
	COccupiedAssignedStandFltType* GetFltTypeItem(int nIndex);

	void AddFltTypeItem(COccupiedAssignedStandFltType* pItem);
	void DelFltTypeItem(COccupiedAssignedStandFltType* pItem);
	void RemoveAll();
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	static void ExportOccupiedAssignedStandAction(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportOccupiedAssignedStandAction(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);

private:
	OccupiedAssignedStandFltTypeVector m_vFltType;
	OccupiedAssignedStandFltTypeVector m_vNeedDelFltType;			
	CAirportDatabase* m_pAirportDB;
};*/


/////////////////////--------------------------------------------
