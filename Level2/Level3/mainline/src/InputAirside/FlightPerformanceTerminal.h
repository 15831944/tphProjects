#pragma once
//#include "..\AirsideInput\sqlexecutor.h"
#include "..\Common\Flt_cnst.h"

#include "InputAirsideAPI.h"

class InputTerminal;
class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
namespace ns_AirsideInput
{
class INPUTAIRSIDE_API CFlightPerformanceTerminal //:public CSQLExecutor
{
public:
	CFlightPerformanceTerminal(void);
	~CFlightPerformanceTerminal(void);
private:
	int m_id; //unique id in table 

	int m_nProjID;// the project ID
	FlightConstraint m_fltType;
	//CString m_fltType;
	double m_dFuelBurn;
	double m_dMinSpeed;
	double m_dMaxSpeed;
	double m_dMaxAcceleration;
	double m_dMaxDeceleration;

	double m_dMinSpeedHolding;
	double m_dMaxSpeedHolding;
	double m_dMaxDownVSpeed;
	double m_dMaxUpVSpeed;
	double m_dCost;

public:
	int getID(){return m_id;}
	void setID(int nID){m_id = nID;}

	int getProjID(){return m_nProjID;}
	void setProjID(int nProjID){ m_nProjID =nProjID;}

	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }
	//CString& GetFltType() { return m_fltType; }
	//void SetFltType(const CString& fltType) { m_fltType = fltType; }


	double getFuelBurn(){ return m_dFuelBurn;}
	void setFuelBurn(double dFuelBurn){m_dFuelBurn = dFuelBurn;}

	double getMinSpeed(){ return m_dMinSpeed;}
	void setMinSpeed(double dMinSpeed){m_dMinSpeed = dMinSpeed;}

	double getMaxSpeed(){ return m_dMaxSpeed;}
	void setMaxSpeed(double dMaxSpeed){m_dMaxSpeed = dMaxSpeed;}

	double getMaxAcceleration(){ return m_dMaxAcceleration;}
	void setMaxAcceleration(double dMaxAcceleration){m_dMaxAcceleration = dMaxAcceleration;}

	double getMaxDeceleration(){ return m_dMaxDeceleration;}
	void setMaxDeceleration(double dMaxDeceleration){m_dMaxDeceleration = dMaxDeceleration;}

	double getMinSpeedHolding(){ return m_dMinSpeedHolding;}
	void setMinSpeedHolding(double dMinSpeedHolding){m_dMinSpeedHolding = dMinSpeedHolding;}

	double getMaxSpeedHolding(){ return m_dMaxSpeedHolding;}
	void setMaxSpeedHolding(double dMaxSpeedHolding){m_dMaxSpeedHolding = dMaxSpeedHolding;}

	double getMaxDownVSpeed(){ return m_dMaxDownVSpeed;}
	void setMaxDownVSpeed(double dMaxDownVSpeed){m_dMaxDownVSpeed = dMaxDownVSpeed;}

	double getMaxUpVSpeed(){ return m_dMaxUpVSpeed;}
	void setMaxUpVSpeed(double dMaxUpVSpeed){m_dMaxUpVSpeed = dMaxUpVSpeed;}

	double getCost(){ return m_dCost;}
	void setCost(double dCost){m_dCost = dCost;}

	//virtual method
public:
	virtual CString GetSelectScript() const  {return "";}
	virtual CString GetUpdateScript() const ;
	virtual CString GetInsertScript() const ;
	virtual CString GetDeleteScript() const ;
	virtual void Execute() {};

public:
	void SaveData();
	void UpdateData();
	void DeleteData();

	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportSaveData(CString strFltType);
	CString ImportGetInsertScript(CString strFltType) const;
};

class INPUTAIRSIDE_API vFlightPerformanceTerminal:public std::vector<CFlightPerformanceTerminal *>
{
public:
	vFlightPerformanceTerminal(CAirportDatabase* pAirportDB);
	~vFlightPerformanceTerminal();
	void setProjID(int nProjID){m_nProjID = nProjID;}
	int getProjID(){return m_nProjID;}
	void ReadData(int nProjID);
	void SaveData();
	void RemoveAll();
	void SortFlightType();
	void SetInputTerminal(InputTerminal* pInputTerminal) { m_pInputTerminal = pInputTerminal; }

private:
	InputTerminal*	m_pInputTerminal;
	int m_nProjID;
	CAirportDatabase* m_pAirportDB;
public:
	static void ImportFlightPerformanceTerminals(CAirsideImportFile& importFile);
	static void ExportFlightPerformanceTerminals(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);

	void ExportData(CAirsideExportFile& exportFile);
	double GetFuelBurnByFlightTy(AirsideFlightDescStruct& _fltType) ;
};

}