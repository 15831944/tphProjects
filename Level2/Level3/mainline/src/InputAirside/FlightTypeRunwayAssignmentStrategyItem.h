#pragma once

#include <vector>
#include "Common/FLT_CNST.H"
#include "../Common/TimeRange.h"
#include "InputAirsideAPI.h"

class TimeRangeRunwayAssignmentStrategyItem;
class FlightConstraint;
class CAirsideImportFile;
class CAirsideExportFile;
class CAirportDatabase;
class CTakeOffTimeRangeRunwayAssignItem ;
enum RunwayAssignmentType;
class INPUTAIRSIDE_API FlightTypeRunwayAssignmentStrategyItem
{
public:
	typedef std::vector<TimeRangeRunwayAssignmentStrategyItem*> TimeRangeStrategyItemVector;
	typedef std::vector<TimeRangeRunwayAssignmentStrategyItem*>::iterator TimeRangeStrategyItemIter;

public:
	FlightTypeRunwayAssignmentStrategyItem(int nProjID);
	virtual ~FlightTypeRunwayAssignmentStrategyItem(void);

	void SetProjectID(int nProjectID){	m_nProjectID = nProjectID; }

	virtual void ReadData();
	void SaveData(const RunwayAssignmentType& rwAssignType);
	void UpdateData();
	void DeleteData();

	int GetTimeRangeStrategyItemCount();
	TimeRangeRunwayAssignmentStrategyItem* GetTimeRangeStrategyItem(int nIndex);

	int getID(){	return m_nID; }
	void setID(int nID){ m_nID = nID;}

	CString getFlightTypeString(){ return m_strFltType;}
	FlightConstraint GetFlightType(){ return m_FlightType;}
	void setFlightType(const CString& strFltType);

	void AddTimeRangeRWAssignStrategyItem(TimeRangeRunwayAssignmentStrategyItem* pItem);
	void DeleteTimeRangeRWAssignStrategyItem(TimeRangeRunwayAssignmentStrategyItem* pItem);
	void UpdateTimeRangeRWAssignStrategyItem(TimeRangeRunwayAssignmentStrategyItem* pItem);

	void RemoveAll();

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDatabase = pAirportDatabase;}
	

	bool operator> ( FlightTypeRunwayAssignmentStrategyItem& element);  
	//{ return m_FlightType < element.GetFlightType() ; }

private:
	int m_nID;
	int m_nProjectID;
	CString m_strFltType;
	FlightConstraint m_FlightType;

	TimeRangeStrategyItemVector m_vectTimeRangeRunwayAssignmentStrategy;

	void DeleteDataFromDatabase();
	TimeRangeStrategyItemVector m_vectNeedDelStrategy;

	CAirportDatabase* m_pAirportDatabase;

public:
	void ExportFlightTypeRunwayAssignment(CAirsideExportFile& exportFile,int nAssignType);
	void ImportFlightTypeRunwayAssignment(CAirsideImportFile& importFile);
    
};
class INPUTAIRSIDE_API  CTakeoffFlightTypeRunwayAssignment 
{
public:
	CTakeoffFlightTypeRunwayAssignment():m_ID(-1) {} ;
	~CTakeoffFlightTypeRunwayAssignment(){ RemoveAllData() ;} ;
public:
	typedef std::vector<CTakeOffTimeRangeRunwayAssignItem*> TY_DATA ;
	typedef TY_DATA::iterator TY_DATA_ITER ;
public:
	TY_DATA m_TimeRange ;
	TY_DATA m_DelTimeRange ;
protected:
	FlightConstraint m_FltType ;
	int m_ID ;
public:
	TY_DATA* GetTimeRangeData() { return &m_TimeRange ;} ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;
public:
	static CString GetSelectSQL(int _parID) ;
	static CString GetInsertSQL(CTakeoffFlightTypeRunwayAssignment* _timeRange ,int _ParID) ;
	static CString GetUpdateSQL(CTakeoffFlightTypeRunwayAssignment* _timeRange ,int _ParID) ;
	static CString GetDeleteSQL(CTakeoffFlightTypeRunwayAssignment* _timeRange) ;

	static void ReadData(std::vector<CTakeoffFlightTypeRunwayAssignment*>& _data, int _ParID,CAirportDatabase* _pAirportDatabase) ;
	static void WriteData(std::vector<CTakeoffFlightTypeRunwayAssignment*>& _data ,int _parID) ;
	static void DeleteData(std::vector<CTakeoffFlightTypeRunwayAssignment*>& _data) ;

	void setFlightType(const FlightConstraint& FltType)  { m_FltType = FltType ;};
	FlightConstraint& GetFlightType() { return m_FltType ;} ;
public:
	CTakeOffTimeRangeRunwayAssignItem* AddTakeoffTimeRangeeItem(TimeRange& _flt) ;
	CTakeOffTimeRangeRunwayAssignItem* FindTakeoffTimeRangeeItem( TimeRange& _flt) ;
	void RemoveTakeoffTimeRangeeItem(CTakeOffTimeRangeRunwayAssignItem* _data) ;
public:
	void RemoveAllData () ;
	void RemoveDeleteData() ;
};
