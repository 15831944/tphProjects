#pragma once

#include <vector>
class FlightTypeRunwayAssignmentStrategyItem;
class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;

#include "InputAirsideAPI.h"

enum RunwayAssignmentType
{
	RunwayAssignmentType_Landing = 0,
	RunwayAssignmentType_TakeOff,

	RunwayAssignmentType_Count
};
class INPUTAIRSIDE_API RunwayAssignmentStrategies
{
public:
	typedef std::vector<FlightTypeRunwayAssignmentStrategyItem*> StrategyVector;
	typedef std::vector<FlightTypeRunwayAssignmentStrategyItem*>::iterator StrategyIter;

public:
	RunwayAssignmentStrategies(int nProjID);
	virtual ~RunwayAssignmentStrategies(void);

public:
	RunwayAssignmentType m_enumRunwayAssignType;
	void SetProjectID(int nProjID){	m_nProjID = nProjID; }
	virtual void ReadData();
	void SaveData();

	int GetStrategyCount(){	return (int)m_vectStrategy.size(); }
	FlightTypeRunwayAssignmentStrategyItem* GetStrategyItem(int nIndex);

	RunwayAssignmentType GetRunwayAssignmentType(){	return m_enumRunwayAssignType; }

	void DeleteStrategyItem(FlightTypeRunwayAssignmentStrategyItem* pStrategyItem);
	void AddStrategyItem(FlightTypeRunwayAssignmentStrategyItem* pStrategyItem);

	void RemoveAll();

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDatabase = pAirportDatabase;}


	void Sort();

private:
	StrategyVector m_vectStrategy;
	int m_nProjID;

	void DeleteDataFromDatabase();
	StrategyVector m_vectNeedDelStrategy;
	CAirportDatabase* m_pAirportDatabase;

public:
	static void ExportRunwayAssignment(CAirsideExportFile& exportFile);
	static void ImportRunwayAssignment(CAirsideImportFile& importFile);

};





