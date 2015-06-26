#pragma once
//#include "SQLExecutor.h"
#include "..\Common\Flt_cnst.h"

#include "InputAirsideAPI.h"

class InputTerminal;
class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
class INPUTAIRSIDE_API RunwayThreshold
{
public:
	RunwayThreshold()
		:m_nID(-1)
		,m_nFltPerformLandingID(-1)
		,m_dDistance(0.0)
	{
		m_strRunwayName.Empty();
	}

public:
	int m_nID;
	CString m_strRunwayName;
	int m_nFltPerformLandingID;
	double m_dDistance;
};

class INPUTAIRSIDE_API PerformLanding
{
public:
	PerformLanding()
		:m_nID(-1)
		,m_dMinTouchDownSpeed(80.0)		
		,m_dMaxTouchDownSpeed(100.0)
		,m_dMinTouchDownDist(100.0)
		,m_dMaxTouchDownDist(300.0)
		,m_dMaxDeceleration(10.0)
		,m_dNormalDeceleration(4.0)
		,m_dMinLandDist(900.0)
		,m_dMaxLandDist(2000.0)
		,m_dMaxSpeedAcute(10.0)
		,m_dMaxSpeed90deg(15.0)
		,m_dMaxSpeedHighSpeed(40.0)
		,m_dFuelBurn(250.0)
		,m_dCost(650.0)
	{}

public:
	void ReadThresholds(int nID);
	void InsertRunwayThreshold(RunwayThreshold& rwThreshold);
	void UpdateRunwayThreshold(RunwayThreshold& rwThreshold);
	void DeleteRunwayThresholds(int nID);

	std::vector<RunwayThreshold>& GetRunwayThresholds()
	{
		return m_vRunwayThresholds;
	}

	double GetRunwayThresholdFlareDistanceByRwName(CString& strRunwayName)
	{
		for(std::vector<RunwayThreshold>::iterator iter = m_vRunwayThresholds.begin();
			iter != m_vRunwayThresholds.end();
			iter++
			)
			if (0 == strRunwayName.CompareNoCase(iter->m_strRunwayName))
				return iter->m_dDistance;
		return double(0.0);
	}

	int GetRunwayThresholdIDByRwName(CString& strRunwayName)
	{
		for(std::vector<RunwayThreshold>::iterator iter = m_vRunwayThresholds.begin();
			iter != m_vRunwayThresholds.end();
			iter++
			)
			if (0 == strRunwayName.CompareNoCase(iter->m_strRunwayName))
				return iter->m_nID;
		return int(-1);
	}

	int GetRunwayThresholdIndexByRwName(CString& strRunwayName)
	{
		for(size_t i=0; i< m_vRunwayThresholds.size(); i++)
			if (0 == strRunwayName.CompareNoCase(m_vRunwayThresholds.at(i).m_strRunwayName))
				return (int)i;
		return int(-1);
	}

	int GetID()
	{
		return m_nID;
	}

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

public:
	int m_nID;
	FlightConstraint m_fltType;
	//CString m_fltType;
	std::vector<RunwayThreshold> m_vRunwayThresholds;
	double m_dMinTouchDownSpeed;
	double m_dMaxTouchDownSpeed;
	double m_dMinTouchDownDist;
	double m_dMaxTouchDownDist;
	double m_dMaxDeceleration;
	double m_dNormalDeceleration;
	double m_dMinLandDist;
	double m_dMaxLandDist;
	double m_dMaxSpeedAcute;
	double m_dMaxSpeed90deg;
	double m_dMaxSpeedHighSpeed;
	double m_dFuelBurn;
	double m_dCost;
};


class INPUTAIRSIDE_API PerformLandings
{
public:
	PerformLandings(CAirportDatabase* pAirportDB)
	: m_nProjectID(-1)
	, m_pInputTerminal(NULL)
	, m_pAirportDB(pAirportDB)
	{}
	void ReadData(int nProjID);
	void InsertPerformLanding(PerformLanding& perfLand);
	void UpdatePerformLanding(PerformLanding& perfLand);
	void DeletePerformLanding(PerformLanding& perfLand);

	std::vector<PerformLanding>& GetPerformLandings() { return m_vPerformLandings; }
	void SetProjectID(int nProjID) { m_nProjectID = nProjID; }
	int GetProjectID() { return m_nProjectID; }
	void SetInputTerminal(InputTerminal* pInputTerminal) { m_pInputTerminal = pInputTerminal; }

	void SortFlightType();

private:
	int m_nProjectID;
	InputTerminal*	m_pInputTerminal;
	std::vector<PerformLanding> m_vPerformLandings;
	CAirportDatabase* m_pAirportDB;
public:
	static void ImportPerformLandings(CAirsideImportFile& importFile);
	static void ExportPerformLandings(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportInsertPerformLanding(PerformLanding& perfLand,CString strFltType);
	double GetFuelBurnByFlightType(AirsideFlightDescStruct& _Flttype) ;
};