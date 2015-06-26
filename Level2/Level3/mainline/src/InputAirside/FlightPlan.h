#pragma once
#include "AirRoute.h"
#include "InputAirsideAPI.h"

class CADORecordset;
class CAirportDatabase;
class CAirsideImportFile;
class CAirsideExportFile;


namespace ns_FlightPlan
{
	class FlightPlanGenericPhaseBase;

const static CString Phase_NameString[] =
{
	_T("cruise"),_T("terminal"), _T("approach"),_T("landing"), _T("takeoff"), _T("climbout")
};

class INPUTAIRSIDE_API WayPointProperty
{
public:
	enum Phase { Phase_cruise = 0,Phase_Terminal,Phase_approach,Phase_Landing, Phase_takeoff,  Phase_climbout,Phase_Count };

	
	WayPointProperty(const ARWaypoint& wayPoint, int nAirRoutePropID);
	explicit WayPointProperty(int nAirRoutePropID);
	WayPointProperty(CADORecordset& adoRecordset, CAirRoute& airRoute);

	void SetAltitude(double fAltitude) { m_fAltitude = fAltitude; }
	double GetAltitude() const;

	void SetDefaultAltitude();
	
	void SetSpeed(double fSpeed) { m_fSpeed = fSpeed; }
	double GetSpeed() const;

	void SetPhase(Phase phase) { m_phase = phase; }
	Phase GetPhase() const;

	CString GetPhaseNameString();

	const ARWaypoint& GetWayPoint() const;
	ARWaypoint& GetWayPoint();

	CPoint2008 GetExtendPoint() const { return m_ExtendPoint; }

	void Save();
	void DeleteFromDB();

	void SetAirRoutePropID(int nID) { m_nAirRoutePropID = nID; }

private:
	int			m_nTableID;
	int			m_nAirRoutePropID;
	ARWaypoint	m_wayPoint;
	double		m_fAltitude;
	double		m_fSpeed;
	Phase		m_phase;
	CPoint2008		m_ExtendPoint;

};

class INPUTAIRSIDE_API AirRouteProperty
{
public:
	AirRouteProperty(const CAirRoute& airRoute, int nFlightPlanID);
	AirRouteProperty(CADORecordset& adoRecordset);
	explicit AirRouteProperty(int nFlightPlanID);
	size_t GetWayPointCount() const;
	const WayPointProperty* GetItem(size_t nIndex) const;
	WayPointProperty* GetItem(size_t nIndex);

	const CAirRoute& GetAirRoute() const;
	CAirRoute& GetAirRoute();

	void DeleteFromPosToEnd(int nWayPointPos);
	void DeleteBeginToPos(int nWayPointPos);

	void AddItem(WayPointProperty* wayPointProperty);
	void DeleteItem(WayPointProperty* wayPointProperty);

	void Save();
	void DeleteFromDB();

	void SetFlightPlanID(int nID) { m_nFlightPlanID = nID; }
	int getTableID(){ return m_nTableID;}

private:
	int								m_nTableID;
	CAirRoute						m_airRoute;
	int								m_nFlightPlanID;
	std::vector<WayPointProperty *>	m_wayPointProperties;
	std::vector<WayPointProperty *>	m_wayPointPropertiesNeedDel;


};


class INPUTAIRSIDE_API FlightPlanGenericPhaseBase
{
public:
	FlightPlanGenericPhaseBase();
	virtual ~FlightPlanGenericPhaseBase();

	virtual void SaveData(int nFlightPlanID)					= 0;
	virtual void DeleteData()									= 0;
	virtual void InitFromDBRecordset(CADORecordset& recordset)	= 0;
};

//----------------------------------------------------------------------------------
//Summary:
//		set of data
//----------------------------------------------------------------------------------
class INPUTAIRSIDE_API FlightPlanGenericPhase : public FlightPlanGenericPhaseBase
{
public:
	enum Phase { Phase_cruise = 0,Phase_Terminal,Phase_approach,Phase_Landing, Phase_takeoff,  Phase_climbout,Phase_Count };

	FlightPlanGenericPhase();
	virtual ~FlightPlanGenericPhase();

	//--------------------------------------------------------------------------
	//Summary:
	//		Set or get operate
	//--------------------------------------------------------------------------
	void SetDistancetoRunway(double dDistance);
	double GetDistanceToRunway()const;

	void SetUpAltitude(double dAltitude);
	double GetUpAltitude()const;

	void SetDownAltitude(double dAltitude);
	double GetDownAltitude()const;

	void SetAltitudeSeparate(double dSep);
	double GetAltitudeSeparate()const;

	void SetUpSpeed(double dSpeed);
	double GetUpSpeed()const;

	void SetDownSpeed(double dSpeed);
	double GetDownSpeed()const;

	void SetPhase(Phase emPhase);
	Phase GetPhase()const;
	//---------------------------------------------------------------------------
	//Summary:
	//		database operation
	//---------------------------------------------------------------------------
	void SaveData(int nFlightPlanID);
	void UpdateData();
	void DeleteData();
	void InitFromDBRecordset(CADORecordset& recordset);
	//---------------------------------------------------------------------------


private:
	double		m_dDistToRunway;
	double		m_dUpAltitude;
	double		m_dDownAltitude;
	double		m_dAltitudeSep;
	double		m_dUpSpeed;
	double		m_dDownSpeed;
	Phase		m_phase;

	int			m_nTableID;
};
//-
//---------------------------------------------------------------------------------
//Summary:
//		vector of data
//---------------------------------------------------------------------------------
class INPUTAIRSIDE_API FlightPlanGenericPhaseList
{
public:
	FlightPlanGenericPhaseList();
	virtual ~FlightPlanGenericPhaseList();

	virtual void AddItem(FlightPlanGenericPhaseBase* pItem);
	virtual FlightPlanGenericPhaseBase* GetItem(int nIdx);
	virtual void DeleteItem(FlightPlanGenericPhaseBase* pItem);
	virtual int GetCount();

	//---------------------------------------------------------------------------
	//Summary:
	//		database operation
	//---------------------------------------------------------------------------
	void SaveData(int nFlightPlanID);
	void UpdateData();
	void DeleteData();
	void InitFromDBRecordset(CADORecordset& recordset);
	//---------------------------------------------------------------------------
private:
	std::vector<FlightPlanGenericPhaseBase*> m_vPhase;
	std::vector<FlightPlanGenericPhaseBase*> m_vDelPhase;
};

//--------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//Summary:
//		flight plan abstract class
//-----------------------------------------------------------------------------------
class INPUTAIRSIDE_API FlightPlanBase
{
public:
	enum OperationType	
	{ 
		takeoff = 0,
		landing,
		enroute,
		Circuit 
	};
	enum ProfileType
	{
		Generic_Profile = 0,
		Specific_Profile
	};

	FlightPlanBase();
	virtual ~FlightPlanBase();

	LPCTSTR GetFlightType() const;
	void SetFlightType(LPCTSTR lpszFlightType) { m_strFlightType = lpszFlightType; }

	OperationType GetOperationType() const;
	void SetOperationType(OperationType operationType) { m_operationType = operationType;}

	void SetAirportDB(CAirportDatabase* pAirportDB) {m_pAirportDB = pAirportDB;}

	bool operator < (const FlightPlanBase& flightProfile)const;
	virtual ProfileType GetProcfileType()const = 0;

	int GetID() const { return m_nTableID; }

	virtual void Save() = 0;
	virtual void DeleteFromDB() = 0;

protected:
	int								m_nTableID;
	int								m_nProjectID;
	CString							m_strFlightType;
	OperationType					m_operationType;
	CAirportDatabase*				m_pAirportDB;
};


//-----------------------------------------------------------------------------------
//Summary:
//		generic profile property
//----------------------------------------------------------------------------------
class INPUTAIRSIDE_API FlightPlanGeneric : public FlightPlanBase
{
public:
	FlightPlanGeneric(int nProjectID);
	virtual ~FlightPlanGeneric();

	virtual ProfileType GetProcfileType()const {return Generic_Profile;}

	virtual void AddItem(FlightPlanGenericPhase* pItem);
	virtual FlightPlanGenericPhase* GetItem(int nIdx) const;
	virtual void DeleteItem(FlightPlanGenericPhase* pItem);
	virtual int GetCount() const;
	//---------------------------------------------------------------------------
	//Summary:
	//		database operation
	//---------------------------------------------------------------------------
	void Save();
	void DeleteFromDB();

	void SaveData(int nForeignID);
	void UpdateData();
	void DeleteData();
	void InitFromDBRecordset(CADORecordset& recordset);
	//---------------------------------------------------------------------------

	int GetIndex(FlightPlanGenericPhaseBase* pGenericComponent);

	void FillDefaultValue();
	void FillLandingValue();
	void FillTakeoffValue();

public:
	double GetSpeed(double dDistToRunway) const;
	double GetMinSpeed(double dDistToRunway) const;
	double GetMaxSpeed(double dDistToRunway) const;

private:
	FlightPlanGenericPhaseList* m_pGenericPhases;
};	

class INPUTAIRSIDE_API FlightPlanSpecific : public FlightPlanBase
{
 public:

 	FlightPlanSpecific(int nProjectID);
 	FlightPlanSpecific(CADORecordset& adoRecordset);
 
 	size_t GetAirRouteCount() const;
 	const AirRouteProperty* GetItem(size_t nIndex) const;
 	AirRouteProperty* GetItem(size_t nIndex);
 
 
	virtual ProfileType GetProcfileType()const {return Specific_Profile;}
 	void AddItem(AirRouteProperty* airRouteProperty);
 	void DeleteItem(AirRouteProperty* airRouteProperty);
 	void DeleteItem(size_t nIndex);
 
 	void Save();
 	void DeleteFromDB();
 
 private:
 	std::vector<AirRouteProperty *>	m_airRouteProperties;
 	std::vector<AirRouteProperty *>	m_airRoutePropertiesNeedDel;
 
};

class INPUTAIRSIDE_API FlightPlans
{
public:
	FlightPlans(int nProjectID);
	FlightPlans(int nProjectID,CAirportDatabase* pAirportDB);
	~FlightPlans();

	void Read();
	void Save();


 	int GetCount()const;
 	FlightPlanBase* GetFlightPlan(int nidx);

 	void AddFlightPlan(FlightPlanBase* pFlightPlan);
 	void DeleteFlightPlan(FlightPlanBase* pFlightPlan);

	int GetIndex(FlightPlanBase* pFlightPlan);

private:
	void SortFlightPlan();
	static bool SortCompare(FlightPlanBase* pFlightPlanSource,FlightPlanBase* pFlightPlanDest);
private:
	int									m_nProjectID;
	CAirportDatabase*					m_pAirportDB;
	std::vector<FlightPlanBase *>		m_flightPlans;
	std::vector<FlightPlanBase *>		m_flightPlansNeedDel;

public:

};

}