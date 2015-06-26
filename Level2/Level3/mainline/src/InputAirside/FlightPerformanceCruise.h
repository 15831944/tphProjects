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
class INPUTAIRSIDE_API CFlightPerformanceCruise //:public CSQLExecutor
{
public:
	CFlightPerformanceCruise();
	~CFlightPerformanceCruise();

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

public:
	int getID(){return m_id;}
	void setID(int nID){m_id = nID;}

	int getProjID(){return m_nProjID;}
	void setProjID(int nProjID){ m_nProjID =nProjID;}

	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(FlightConstraint& fltType) { m_fltType = fltType; }
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

class INPUTAIRSIDE_API vFlightPerformanceCruise:public std::vector<CFlightPerformanceCruise*>
{
public:
	vFlightPerformanceCruise(CAirportDatabase* pAirportDB);
	~vFlightPerformanceCruise();
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
	static void ImportFlightPerformanceCruises(CAirsideImportFile& importFile);
	static void ExportFlightPerformanceCruises(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);

	void ExportData(CAirsideExportFile& exportFile);

	double GetFuelBurnByFlightType(AirsideFlightDescStruct& _constraint) ;
};
}