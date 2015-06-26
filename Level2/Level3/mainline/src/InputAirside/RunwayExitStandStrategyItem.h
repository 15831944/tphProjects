#pragma once

#include <vector>
#include "common/FLT_CNST.H"
#include "InputAirsideAPI.h"

class TimeRangeRunwayExitStrategyItem;
class FlightConstraint;
class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API RunwayExitStandStrategyItem
{
public:
	typedef std::vector<TimeRangeRunwayExitStrategyItem*> TimeRangeStrategyItemVector;
	typedef TimeRangeStrategyItemVector::iterator TimeRangeStrategyItemIter;
	typedef TimeRangeStrategyItemVector::const_iterator TimeRangeStrategyItemIter_const;

public:
	RunwayExitStandStrategyItem(int nExitStrategyID);
	~RunwayExitStandStrategyItem(void);

	// DB
	void ReadData();
	void SaveData();
	void UpdateData();
	void DeleteData();

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile, int nExitStrategyID);


	// data member access
	int getID() const { return m_nID; }
	void setID(int val) { m_nID = val; }
	int getExitStrategyID() const { return m_nExitStrategyID; }
	void setExitStrategyID(int val) { m_nExitStrategyID = val; }
	int getStandFamilyID() const { return m_nStandFamilyID; }
	void setStandFamilyID(int val);
	CString getStandFamilyName() const;

	// access the list
	int GetTimeRangeStrategyItemCount();
	TimeRangeRunwayExitStrategyItem* GetTimeRangeStrategyItem(int nIndex);

	void AddTimeRangeRWAssignStrategyItem(TimeRangeRunwayExitStrategyItem* pItem);
	void DeleteTimeRangeRWAssignStrategyItem(TimeRangeRunwayExitStrategyItem* pItem);

	BOOL CheckExitPercent() const;

protected:
	void RemoveAll();
	void DeleteDataFromDatabase();

	void InitStandName();

private:
	int m_nID;
	int m_nExitStrategyID;
	int m_nStandFamilyID;
	CString m_strStandFamilyName;

	TimeRangeStrategyItemVector m_vectTimeRangeRunwayExitStrategy;
	TimeRangeStrategyItemVector m_vectNeedDelStrategy;
};
