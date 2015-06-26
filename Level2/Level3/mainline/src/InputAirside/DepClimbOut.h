#pragma once
//#include "SQLExecutor.h"
#include "..\Common\Flt_cnst.h"
#include "InputAirsideAPI.h"

class InputTerminal;
class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
class DepClimbOut
{
public:
	DepClimbOut()
	: m_nID(-1)
	, m_nMinHorAccel(5.0)
	, m_nMaxHorAccel(15.0)
	, m_nMinVerticalSpeedToEntoute(500.0)
	, m_nMaxVerticalSpeedToEntoute(3000.0)
	, m_fuelBurn(3000.0)
	, m_cost(2000.0)
	{}

	int m_nID;
	FlightConstraint m_fltType;
	//CString m_fltType;
	double m_nMinHorAccel;
	double m_nMaxHorAccel;
	double m_nMinVerticalSpeedToEntoute;
	double m_nMaxVerticalSpeedToEntoute;
	double m_fuelBurn;
	double m_cost;
};


class INPUTAIRSIDE_API DepClimbOuts
{
public:
	DepClimbOuts(CAirportDatabase* pAirportDB);
	~DepClimbOuts();

	virtual void ReadData(int nProjID);
	void InsertDepClimbOut(DepClimbOut&);
	void UpdateDepClimbOut(DepClimbOut&);
	void DeleteDepClimbOut(DepClimbOut&);
	std::vector<DepClimbOut>& GetDepClimbOuts()	{ return m_vDepClimbOuts; }
	void SetProjectID(int nProjID) { m_nProjID = nProjID; }
	int GetProjectID() { return m_nProjID; }
	void SetInputTerminal(InputTerminal* pInputTerminal) { m_pInputTerminal = pInputTerminal; }
	void SortFlightType();

private:
	int m_nProjID;
	InputTerminal* m_pInputTerminal;
	std::vector<DepClimbOut> m_vDepClimbOuts;
	CAirportDatabase* m_pAirportDB;
public:
	static void ImportDepClimbOuts(CAirsideImportFile& importFile);
	static void ExportDepClimbOuts(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportInsertDepClimbOut(DepClimbOut& depClimbOut,CString strFltType);

	double GetFuelBurnByFlightType(AirsideFlightDescStruct& _flttype) ;
};
