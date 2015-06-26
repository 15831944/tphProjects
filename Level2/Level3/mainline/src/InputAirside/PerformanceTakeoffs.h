#pragma once
#include "..\Common\Flt_cnst.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
class INPUTAIRSIDE_API PerformTakeOff
{
public:
	PerformTakeOff()
	: m_nID(-1)
	, m_nMinAcceleration(5.0)
	, m_nMaxAcceleration(10.0)
	, m_nMinLiftOff(150.0)
	, m_nMaxLiftOff(190.0)
	, m_nMinPositionTime(45.0)
	, m_nMaxPositionTime(120.0)
	, m_minTakeOffRoll(1100.0)
	, m_maxTakeOffRoll(3000.0)
	, m_fuelBurn(3000.0)
	, m_cost(2000.0)
	{}

public:
	int m_nID;
	FlightConstraint m_fltType;
	//CString m_fltType;
	double m_nMinAcceleration;
	double m_nMaxAcceleration;
	double m_nMinLiftOff;
	double m_nMaxLiftOff;
	double m_nMinPositionTime;
	double m_nMaxPositionTime;
	double m_minTakeOffRoll;
	double m_maxTakeOffRoll;
	double m_fuelBurn;
	double m_cost;
};

class INPUTAIRSIDE_API PerformTakeOffs
{
public:
	PerformTakeOffs(CAirportDatabase* pAirportDB)
	{m_pAirportDB = pAirportDB;}

	void ReadData(int nProjID);
	void InsertPerformTakeOff(PerformTakeOff& tf);
	void UpdatePerformTakeOff(PerformTakeOff& tf);
	void DeletePerformTakeOff(PerformTakeOff& tf);

	void SetProjectID(int nProjID) { m_nProjID = nProjID; }
	int GetProjectID() { return m_nProjID; }
	std::vector<PerformTakeOff>& GetTakeoffs() { return m_vTakeoffs; }
	void SortFlightType();

private:
	int m_nProjID;
	int m_nLastTakeoffID;
	std::vector<PerformTakeOff> m_vTakeoffs;
	CAirportDatabase* m_pAirportDB;
public:
	static void ImportPerformTakeOffs(CAirsideImportFile& importFile);
	static void ExportPerformTakeOffs(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportInsertPerformTakeOff(PerformTakeOff& tf,CString strFltType);

	double GetFuelBurnByFlightType(AirsideFlightDescStruct& _flttype) ;
};
