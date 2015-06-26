#pragma once
#include <vector>

class FlightTypeRunwayExitStrategyItem;
class CAirportDatabase;
class CAirsideExportFile;
class CAirsideImportFile;

#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API RunwayExitStrategies
{
public:
	typedef std::vector<FlightTypeRunwayExitStrategyItem*> StrategyVector;
	typedef StrategyVector::iterator StrategyIter;
	typedef StrategyVector::const_iterator StrategyIter_const;

public:
	RunwayExitStrategies(int nProjectID);
	~RunwayExitStrategies(void);

public:

	void SetProjectID(int nProjID){	m_nProjID = nProjID; }
	void ReadData();
	void SaveData();

	int GetStrategyCount(){	return (int)m_vectStrategy.size(); }
	FlightTypeRunwayExitStrategyItem* GetStrategyItem(int nIndex);

	void DeleteStrategyItem(FlightTypeRunwayExitStrategyItem* pItem);
	void AddStrategyItem(FlightTypeRunwayExitStrategyItem* pStrategyItem);

	//void SetAirportDatabase(CAirportDatabase* pAirportDatabase) { m_pAirportDatabase = pAirportDatabase; }
	
	void RemoveAll();

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}


	static void ExportRunwayExitStrategies(CAirsideExportFile& exportFile);
	static void ImportRunwayExitStrategies(CAirsideImportFile& importFile);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	void ImportReadData();

	BOOL CheckExitPercent() const;
private:
	void DeleteDataFromDatabase();

private:
	StrategyVector m_vectStrategy;
	int m_nProjID;
	CAirportDatabase* m_pAirportDB;
	StrategyVector m_vectNeedDelStrategy;
	//CAirportDatabase* m_pAirportDatabase;
};
