#pragma once
#include "../common/Flt_cnst.h"

class ProbabilityDistribution;
class CAirRoute;
class ARWaypoint;

class AirRoute;
class AirRoutes;

class CFlightPlanRouteFix
{
public:
	CFlightPlanRouteFix();

	void setID(int nID){ m_nID = nID;}
	int	 getID(){ return m_nID;}

	//void setName(const CString& strName);
	//CString getName() const;

	//CString m_strName;

	//air Route Way point ID
	void setAirRouteWaypointID(int nID){ m_nAirRouteWaypointID = nID;}
	int getAirRouteWaypointID()const{ return m_nAirRouteWaypointID;}

	//flight plan id
	void setFlightPlanID(int nID){ m_nFlightPlanID = nID;}
	int getFlightPlanID() const{ return m_nFlightPlanID;}


	void setAltitude(const double& dAltitude);
	double getAltitude() const;

	void setSpeed(const double& dSpeed);
	double getSpeed() const;

	//save the flight plan rout fix data into database
	BOOL SaveData(int nOrder, int nfltplanID);
	BOOL UpdateData(int nOrder);
	BOOL DeleteData();

	ARWaypoint* getARWaypoint()const{ return m_pARWaypoint; }
	void setARWaypoint(ARWaypoint* pArwaypoint){ m_pARWaypoint = pArwaypoint;}

	void setOrder(int nOrder){ m_nOrder = nOrder;}

private:
	//the unique id in the table
	int m_nID;
	
	//flight plan id
	int m_nFlightPlanID;

	int m_nOrder;
	
	//air route way point id
	int m_nAirRouteWaypointID;

	double m_dAltitude;
	double m_dSpeed;

	ARWaypoint *m_pARWaypoint;
}; 

class CFlightPlanRoute
{
public:
	CFlightPlanRoute(CAirRoute* pAirRoute);
	BOOL ReadData();
	BOOL SaveData(int& nOrder,int nFltPlanID);
	BOOL DeleteData();

	int GetCount();
	CFlightPlanRouteFix* GetItem(int nIndex);

	CAirRoute* GetAirRoute() const { return m_pAirRoute; }
	ARWaypoint* GetArWaypoint(int nArwpID);
	CFlightPlanRouteFix * GetPlanRouteFix(int nArwpID);

	void AddItem(CFlightPlanRouteFix* pRouteFix) { m_lstRoute.push_back(pRouteFix); }

private:
	int m_nID;
	int m_nFlightPlanID;
	std::vector<CFlightPlanRouteFix*> m_lstRoute;
	CAirRoute* m_pAirRoute;
};

class CFlightPlan// : public CSQLExecutor
{
public:
	CFlightPlan(const FlightConstraint& FltType);
	~CFlightPlan();

public:
	void setID(int nID){ m_nID = nID;}
	int	 getID(){ return m_nID;}

	CString GetFltTypeStr() const;
	void SetFltType(const CString& strFltType); 

	ProbabilityDistribution* GetVerticalProfile() const { return m_verticalProfile; }
	void SetVerticalProfile(ProbabilityDistribution* vProfile) { m_verticalProfile = vProfile; }
	std::vector<CFlightPlanRoute*>& GetFltRouteList() { return m_vFltPlanRoute;	}
	
	CFlightPlanRoute* GetItem(int nIndex);
	void DeleteItem(int nIndex);
	void AddItem(CFlightPlanRoute* pFlightPlanRoute) { m_vFltPlanRoute.push_back(pFlightPlanRoute); }

	void AddFltPlanWaypoint( CFlightPlanRouteFix * pArWayPoint);

	FlightConstraint GetFltType()const{ return m_fltCnst; }

private:
	int m_nID; //unique id in table 
	int m_nProjID;// the project ID

	//the fltType
	//CString m_strFltType;

	//
	FlightConstraint m_fltCnst;
	
	//vertical profile distribution 
	ProbabilityDistribution* m_verticalProfile;

	//list of flight plane route
	std::vector<CFlightPlanRoute*> m_vFltPlanRoute;
	
	//the fltplan need to delete
	std::vector<CFlightPlanRoute*> m_vFltPlanRouteNeedDel;
	//std::vector<CFlightPlanRouteFix *> m_vWaypoint;

//virtual method
public:
	virtual CString GetSelectScript() const { return ""; }
	virtual CString GetUpdateScript() const;
	virtual CString GetInsertScript() const;
	virtual CString GetDeleteScript() const;
	virtual void Execute() {};

	BOOL ReadData();
	BOOL SaveData(int nProjID);
	BOOL UpdateData();
	BOOL DeleteData();
};
class CAirportDatabase;

class CFlightPlans
{
public:
	CFlightPlans();
	~CFlightPlans ();

public:
	BOOL ReadData();
	BOOL SaveData();

	void setID(int nID){ m_nID = nID;}
	int	 getID(){ return m_nID;}
	void setProjID(int nProjID) { m_nProjID = nProjID; }
	int getProjID()const { return m_nProjID; }
	//void setAirportDBID(int nDbID){ m_nAirportDBID = nDbID; }
	//int getAirportDBID()const{ return m_nAirportDBID; }
	void SetAirportDB(CAirportDatabase * pAirportDB){ m_pAirportDB = pAirportDB; }
	CAirportDatabase * getAirportDB(){ return m_pAirportDB; }

	void SetAirRoutes(AirRoutes* pAirRoutes) { m_pAirRoutes = pAirRoutes; }
	AirRoutes* GetAirRoutes() { return m_pAirRoutes; }

	BOOL AddItem(CFlightPlan *pFltPlan);
	BOOL DelItem(CFlightPlan* pFltPlan);
	int GetItemCount();
	CFlightPlan* GetItem(int nIndex);

private:
	int			m_nID;
	AirRoutes*	m_pAirRoutes;
	int			m_nProjID;
	CAirportDatabase*m_pAirportDB;

	std::vector<CFlightPlan*> m_vFlightPlan;
	std::vector<CFlightPlan*> m_vFlightPlanNeedDel;
};

