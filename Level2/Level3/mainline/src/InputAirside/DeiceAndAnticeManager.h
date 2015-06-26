#pragma once
#include <CommonData/procid.h>
#include "../Database/ADODatabase.h"
#include "AircraftDeicingAndAnti_icingsSettings.h"
#include "InputAirsideAPI.h"

enum PostionMethod
{
	POWER_IN = 0,
	TOWTONOSE_IN,
	TOWTOTAIL
};

enum EngineState
{
	Running = 0,
	Off
};

enum PriorityType
{
	NONTYPE = 0,
	DeicePad_Type,
	DepStand_Type,
	LeadOut_Type,
	RemoteStand_Type
};
struct IceVehicles
{
	IceVehicles()
		:m_nNumber(0)
		,m_nFlowRate(0)
	{

	}
	int m_nNumber;
	int m_nFlowRate;
};
struct DeicePadPriority
{
	DeicePadPriority()
		:m_nPriority(1)
		,m_deicePad(-1)
		,m_nDeicePadQueue(0)
		,m_nWaittime(0)
		,m_emType(POWER_IN)
		,m_emEngineState(Running)
	{

	}
	int m_nPriority;
	int m_deicePad;
	int m_nDeicePadQueue;
	int m_nWaittime;
	PostionMethod m_emType;
	EngineState m_emEngineState;
};

struct DepStandPriority
{
	DepStandPriority()
		:m_nPriority(2)
		,m_Stand(-1)
		,m_nStandWithinTime(0)
		,m_nWaitFluidTime(0)
		,m_nWaitInspectionTime(0)
		,m_emEngineState(Running)
	{

	}
	int m_nPriority;
	int m_Stand;
	int m_nStandWithinTime;
	int m_nWaitFluidTime;
	int m_nWaitInspectionTime;
	EngineState m_emEngineState;
};

struct LeadOutPriority
{
	LeadOutPriority()
		:m_nPriority(3)
		,m_nDistance(0)
		,m_nStandWithinTime(0)
		,m_nWaitFluidTime(0)
		,m_nWaitInspectionTime(0)
		,m_emEngineState(Running)
	{

	}
	int m_nPriority;
	int m_nDistance;
	int m_nStandWithinTime;
	int m_nWaitFluidTime;
	int m_nWaitInspectionTime;
	EngineState m_emEngineState;
};

struct RemoteStandPriority
{
	RemoteStandPriority()
		:m_nPriority(4)
		,m_Stand(-1)
		,m_nStandWithinTime(0)
		,m_nWaitFluidTime(0)
		,m_nWaitInspectionTime(0)
		,m_emType(POWER_IN)
		,m_emEngineState(Running)
	{

	}
	int m_nPriority;
	int m_Stand;
	int m_nStandWithinTime;
	int m_nWaitFluidTime;
	int m_nWaitInspectionTime;
	PostionMethod m_emType;
	EngineState m_emEngineState;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///////////CDeiceAndAnticeInitionalSurfaceCondition
class InputTerminal;
class INPUTAIRSIDE_API CDeiceAndAnticeInitionalSurfaceCondition
{
public:
	CDeiceAndAnticeInitionalSurfaceCondition(AircraftSurfacecondition emType);
	~CDeiceAndAnticeInitionalSurfaceCondition();
	
public:
	IceVehicles* getDeiceVehicle(){return &m_deIceVehicles;}
	IceVehicles* getAntiVehicle(){return &m_antiIceVehicles;}
	DeicePadPriority* getDeicePad(){return &m_deicePadPriority;}
	DepStandPriority* getDepStand(){return &m_depStandPriority;}
	LeadOutPriority* getLeadOut(){return &m_leadOutPriority;}
	RemoteStandPriority* getRemoteStand(){return & m_remoteStandPriority;}
	const AircraftSurfacecondition getType()const{return m_emType;}
	PriorityType getPriotity(int  );
	int getID(){return m_nID;}

	void SaveData();
	void ReadData(int nDeiceAndAnticeInitionalSurfaceConditionID);
	void RemoveData();
	void UpdateData();

private:
	IceVehicles m_deIceVehicles;
	IceVehicles m_antiIceVehicles;
	DeicePadPriority m_deicePadPriority;
	DepStandPriority m_depStandPriority;
	LeadOutPriority m_leadOutPriority;
	RemoteStandPriority m_remoteStandPriority;
	AircraftSurfacecondition m_emType;
	int m_nID;
};

////////////////////////////////////////////////////////////////////////////////////////////
//////CDeiceAndAnticePrecipitionType
class INPUTAIRSIDE_API CDeiceAndAnticePrecipitionType
{
public:
	CDeiceAndAnticePrecipitionType(Precipitationtype emType);
	~CDeiceAndAnticePrecipitionType();
	
public:
	CDeiceAndAnticeInitionalSurfaceCondition* getClear(){return &m_clear;}
	CDeiceAndAnticeInitionalSurfaceCondition* getLightFrost(){return &m_lightFrost;}
	CDeiceAndAnticeInitionalSurfaceCondition* getIceLayer(){return &m_icelayer;}
	CDeiceAndAnticeInitionalSurfaceCondition* getDryPowerSnow(){return &m_dryPowderSnow;}
	CDeiceAndAnticeInitionalSurfaceCondition* getWetSnow(){return &m_wetSnow;}
	const Precipitationtype getType()const {return m_emType;}
	int getID(){return m_nID;}

	void ReadData(int nDeiceAndAnticePrecipitionTypeID);
	void RemoveData();
	void UpdateData();
	void SaveData();

	CDeiceAndAnticeInitionalSurfaceCondition* GetData(AircraftSurfacecondition surf);

private:
	CDeiceAndAnticeInitionalSurfaceCondition m_clear;
	CDeiceAndAnticeInitionalSurfaceCondition m_lightFrost;
	CDeiceAndAnticeInitionalSurfaceCondition m_icelayer;
	CDeiceAndAnticeInitionalSurfaceCondition m_dryPowderSnow;
	CDeiceAndAnticeInitionalSurfaceCondition m_wetSnow;
	Precipitationtype m_emType;
	InputTerminal* m_pInterm;
	int m_nID;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////CDeiceAndAnticeStand
class INPUTAIRSIDE_API CDeiceAndAnticeStand
{
public:
	CDeiceAndAnticeStand();
	~CDeiceAndAnticeStand();

public:
	CDeiceAndAnticePrecipitionType* getNonFreezing(){return &m_nonFreezing;}
	CDeiceAndAnticePrecipitionType* getFreezingDrizzle(){return &m_freezingDrizzle;}
	CDeiceAndAnticePrecipitionType* getLightSnow(){return &m_lightSnow;}
	CDeiceAndAnticePrecipitionType* getMediumSnow(){return &m_mediumSnow;}
	CDeiceAndAnticePrecipitionType* getHeavySnow(){return &m_heavySnow;}

	void setStand(int standID){m_standID = standID;}
	int getStand()const{return m_standID;}

	void SaveData(int nParentID);
	void ReadData();
	void InitFromDBRecordset(CADORecordset& recordset);
	void RemoveData();
	void UpdateData();

	CDeiceAndAnticeInitionalSurfaceCondition* GetData(Precipitationtype preT,AircraftSurfacecondition surf);

	
private:
	CDeiceAndAnticePrecipitionType m_nonFreezing;
	CDeiceAndAnticePrecipitionType m_freezingDrizzle;
	CDeiceAndAnticePrecipitionType m_lightSnow;
	CDeiceAndAnticePrecipitionType m_mediumSnow;
	CDeiceAndAnticePrecipitionType m_heavySnow;
	int m_standID;
	int m_nID;
};


/////////////////////////////////////////////////////////////////////////////////////////////
///CDeiceAndAnticeFlightType
class ALTObjectID;
class INPUTAIRSIDE_API CDeiceAndAnticeFlightType
{
public:
	CDeiceAndAnticeFlightType();
	~CDeiceAndAnticeFlightType();

	int getCount();
	void addItem(CDeiceAndAnticeStand* pItem);
	void removeItem(CDeiceAndAnticeStand* pItem);
	CDeiceAndAnticeStand* getItem(int ndx);

	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }

	void SaveData(int nParentID);
	void ReadData();
	void InitFromDBRecordset(CADORecordset& recordset);
	void RemoveData();
	void UpdateData();

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	CDeiceAndAnticeInitionalSurfaceCondition* GetData(const ALTObjectID& stand, Precipitationtype preT,AircraftSurfacecondition surf);
private:
	CAirportDatabase* m_pAirportDB;
	FlightConstraint m_fltType;
	std::vector<CDeiceAndAnticeStand*>m_vStand;
	std::vector<CDeiceAndAnticeStand*>m_vDelStand;
	int m_nID;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
///////CDeiceAndAnticeManager
class INPUTAIRSIDE_API CDeiceAndAnticeManager
{
public:
	CDeiceAndAnticeManager(CAirportDatabase* pAirportDB);
	~CDeiceAndAnticeManager();

	void addItem(CDeiceAndAnticeFlightType* pItem);
	void removeItem(CDeiceAndAnticeFlightType* pItem);
	CDeiceAndAnticeFlightType* getItem(int ndx);
	bool findItem(CDeiceAndAnticeFlightType* pItem);
	int getCount();

	////////database
	void SaveData(int nProjID);
	void ReadData(int nProjID);

	void SortByFltType();
private:
	CAirportDatabase* m_pAirportDB;
	std::vector<CDeiceAndAnticeFlightType*> m_vFlightType;
	std::vector<CDeiceAndAnticeFlightType*> m_vDelFlightType;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///////CXMLFileGenerator
class CXMLFileGenerator
{
public:
protected:
private:
};