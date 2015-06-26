#pragma once

#include <vector>
#include "common/FLT_CNST.H"
#include "InputAirsideAPI.h"

class TimeRangeRunwayExitStrategyItem;
class FlightConstraint;
class CAirsideExportFile;
class CAirsideImportFile;
class RunwayExitStandStrategyItem;

class INPUTAIRSIDE_API FlightTypeRunwayExitStrategyItem
{
public:
	typedef std::vector<RunwayExitStandStrategyItem*> RunwayExitStandStrategyItemVector;
	typedef RunwayExitStandStrategyItemVector::iterator RunwayExitStandStrategyItemIter;
	typedef RunwayExitStandStrategyItemVector::const_iterator RunwayExitStandStrategyItemIter_const;

public:
	FlightTypeRunwayExitStrategyItem(int nProjectID);
	~FlightTypeRunwayExitStrategyItem(void);


	// DB
	void ReadData();
	void SaveData();
	void UpdateData();
	void DeleteData();
	void ImportSaveData(CString strFltType);
	void ImportUpdateData(CString strFltType);

	// import & export
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	// data member access
	int getID(){ return m_nID; }
	void setID(int nID){ m_nID = nID;}
	void SetProjectID(int nProjectID){ m_nProjectID = nProjectID; }
	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDatabase = pAirportDatabase; }
	FlightConstraint& GetFltType() { return m_fltType; }

	const FlightConstraint& getFlightType()const{ return m_fltType;}
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }

	// do not access anymore
// 	CString GetFlightTypeString(){return m_strFltType;}
// 	void SetFltType(CString strFltType){m_strFltType = strFltType;}

	int GetRunwayExitStandStrategyCount(){ return (int)m_vectRunwayExitStandStrategyItems.size(); }
	RunwayExitStandStrategyItem* GetRunwayExitStandStrategyItem(int nIndex);
	void AddRunwayExitStandStrategyItem(RunwayExitStandStrategyItem* pItem);
	void DeleteRunwayExitStandStrategyItem(RunwayExitStandStrategyItem* pItem);

	BOOL CheckExitPercent() const;
protected:
	void RemoveAll();
	void DeleteDataFromDatabase();

private:
	int m_nID;
	int m_nProjectID;
// 	CString m_strFltType;

	FlightConstraint m_fltType;

	RunwayExitStandStrategyItemVector m_vectRunwayExitStandStrategyItems;
	RunwayExitStandStrategyItemVector m_vectNeedDelStrategyItems;

	CAirportDatabase* m_pAirportDatabase;
};
