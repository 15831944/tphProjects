#pragma once
//#include "SQLExecutor.h"
#include "..\Common\Flt_cnst.h"
#include "InputAirsideAPI.h"

class InputTerminal;
class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
class ApproachLand
{
public:
	ApproachLand(void)
		:m_nID(-1)
		,m_dMinLength(10.0)
		,m_dMinSpeed(110.0)
		,m_dMaxSpeed(160.0)
		,m_dFuelBurn(1000.0)
		,m_dCost(1900.0)
	{}

public:
	int m_nID;
	FlightConstraint m_fltType;
	//CString m_fltType;

	double m_dMinLength;
	double m_dMinSpeed;
	double m_dMaxSpeed;
	double m_dFuelBurn;
	double m_dCost;
};

class INPUTAIRSIDE_API ApproachLands
{
public:
	ApproachLands(CAirportDatabase* pAirportDB)
	: m_nProjectID(-1)
	, m_pInputTerminal(NULL)
	, m_pAirportDB(pAirportDB)
	{}

	void ReadData(int nProjID);
	void InsertApproachLand(ApproachLand& appLand);
	void UpdateApproachLand(ApproachLand& appLand);
	void DeleteApproachLand(ApproachLand& appLand);

	std::vector<ApproachLand>& GetApproachLands() {	return m_vApproachLands; }
	void SetProjectID(int nProjID) { m_nProjectID = nProjID; }
	int GetProjectID() { return m_nProjectID; }
	void SetInputTerminal(InputTerminal* pInputTerminal) { m_pInputTerminal = pInputTerminal; }
	void SortFlightType();

private:
	int				m_nProjectID;
	InputTerminal*	m_pInputTerminal;
	CAirportDatabase* m_pAirportDB;
	std::vector<ApproachLand> m_vApproachLands;
public:
	static void ImportApproachLands(CAirsideImportFile& importFile);
	static void ExportApproachLands(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportInsertApproachLand(ApproachLand& appLand,CString strFltType);
	double GetFuelBurnByFlightType(AirsideFlightDescStruct& _constraint) ;
};