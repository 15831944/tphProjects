#pragma once

#include <vector>

#include "../Common/TimeRange.h"
#include "StrategyType.h"
#include "Runway.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API RunwayAssignmentPercentItem
{
public: 
	RunwayAssignmentPercentItem(int nTimeRangeItemID)
		:m_nID(-1)
		,m_dPercent(0.0)
		,m_nRunwayMarkIndex(0)
		,m_nRunwayID(-1)
	{
		m_nTimeRangeItemID = nTimeRangeItemID;
	}
	~RunwayAssignmentPercentItem()
	{
	}
	
	int getID(){	return m_nID; }
	void SetTimeRangeID(int nID){	m_nTimeRangeItemID = nID; }
	
public:	

	int m_nTimeRangeItemID;
	int m_nID;
	double m_dPercent;
	int m_nRunwayID;
	int m_nRunwayMarkIndex;

public:
	void SaveData();
	void UpdateData();
	void DeleteData();

public:
	void ExportRunwayAssignmentPercent(CAirsideExportFile& exportFile);
	void ImportRunwayAssignmentPercent(CAirsideImportFile& importFile);
};

class INPUTAIRSIDE_API TimeRangeRunwayAssignmentStrategyItem
{
public:
	typedef std::vector<RunwayAssignmentPercentItem*> RunwayAssignPercentVector;
	typedef std::vector<RunwayAssignmentPercentItem*>::iterator RunwayAssignPercentIter;

public:
	TimeRangeRunwayAssignmentStrategyItem(int nFltTypeItemID);
	virtual ~TimeRangeRunwayAssignmentStrategyItem(void);

	RunwayAssignPercentVector& GetManagedAssignmentRWAssignPercent(){	return m_vectManagedAssignmentRWAssignPercent;}

	void ReadData();
	void SaveData();
	void UpdateData();
	void DeleteData();

	void setID(int nID){ m_nID = nID;}
	int getID(){ return m_nID;}

	void SetFltTypeItemID(int nID){ m_nFltTypeItemID = nID; }

	void setStrategyType(RunwayAssignmentStrategyType rwStrategyType){	m_emStrategyType = rwStrategyType; }
	RunwayAssignmentStrategyType getStrategyType(){	return m_emStrategyType; }

	
	TimeRange GetTimeRange(){	return m_dtTimeRange; }
	void SetTimeRange(TimeRange& val){	m_dtTimeRange = val; }

	void AddRunwayAssignPercentItem(RunwayAssignmentPercentItem* pItem);
	void DeleteRunwayAssignPercentItem(RunwayAssignmentPercentItem* pItem);
	void UpdateRunwayAssignPercentItem(RunwayAssignmentPercentItem* pItem);
	void UpdateRunwayAssignPercentItemType(RunwayAssignmentStrategyType rwType);

	void RemoveAll();


	//TODO move to timerange class memeber
	ElapsedTime getStartTime(){	return m_dtTimeRange.GetStartTime(); }
	ElapsedTime getEndTime(){	return m_dtTimeRange.GetEndTime(); }
	void setStartTime(ElapsedTime etStart){	m_dtTimeRange.SetStartTime(etStart); }
	void setEndTime(ElapsedTime etEnd){	m_dtTimeRange.SetEndTime(etEnd); }


protected:
	void RefreshRunwayAssignPercentItems(RunwayAssignPercentVector& vec, int nID);

private:
	int m_nID;
	int m_nFltTypeItemID;

	RunwayAssignmentStrategyType m_emStrategyType;
	TimeRange m_dtTimeRange;

	RunwayAssignPercentVector m_vectManagedAssignmentRWAssignPercent;

	void DeleteDataFromDatabase();
	RunwayAssignPercentVector m_vectNeedDelStrategy;

public:
	void ExportTimeRangeRunwayAssignment(CAirsideExportFile& exportFile);
	void ImportTimeRangeRunwayAssignment(CAirsideImportFile& importFile);
};

//////////////////////////////////////////////////////////////////////////
