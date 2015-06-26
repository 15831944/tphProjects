#pragma once


#include <string>
#include <vector>


#include "./AirWayPoint.h"
#include "./ALTAirport.h"
#include "../Common/Point2008.h"
#include "../Common/Path2008.h"
#include "InputAirsideAPI.h"
#include "AirRoutePoint.h"

class Runway;
class AirRouteIntersectionList;
class CAirRouteIntersection;

using namespace std;
typedef pair <int,int> LogicRunway_Pair;

class INPUTAIRSIDE_API CAirRoute  : public ALTObject
{
public:

	
	static const GUID& getTypeGUID();

	enum RouteType{  STAR = 0, SID, EN_ROUTE ,MIS_APPROACH, CIRCUIT,TYPECOUNT, };
	const static char *  RouteTypeStr[TYPECOUNT];
	typedef ref_ptr<CAirRoute> RefPtr;


public:
	CAirRoute(int nAirRouteID = -1);
	CAirRoute(const CAirRoute& otherAirRoute);
	~CAirRoute();
	
	virtual ALTOBJECT_TYPE GetType()const {return ALT_AIRROUTE;} 
	virtual CString GetTypeName()const { return _T("AirRoute");} 


	virtual ALTObjectDisplayProp* NewDisplayProp();
private:
	//int m_nID;
	CString m_strName;
	std::vector<ARWaypoint *> m_vARWaypoint;
	std::vector<LogicRunway_Pair> m_vConnectRunway;
	RouteType m_enumType;
	
	ALTObject m_runway;

	ALTAirport m_airportInfo;

	//Runway* m_pRunway;

	int m_nRunwayMarkIndex;//0, Mark1;1,Mark2

	CPath2008 m_path;

	//ref_ptr<ALTObject> m_pRunway;
public:
	//void setRunwayObj(const ALTObject& altobj){ m_runway = altobj;}
	//const ALTObject& getRunwayObj() const;

	//int getRunwayID();
	//void setRunwayID(int nRunwayID,int nMarkIndex);

	//int getRunwayMarkIndex();
	int GetConnectRunwayCount();
	LogicRunway_Pair getLogicRunwayByIdx(int nIdx);
	bool IsConnectRunway(int nRwyID, int nRwyMark);

	void SetConnectRunway(std::vector<LogicRunway_Pair>& vRunways);
	void setRouteType(RouteType routType){ m_enumType = routType;}
	RouteType getRouteType() const;

	void setName(const CString& strName){ m_strName = strName;}
	const CString& getName() const;

	const std::vector<ARWaypoint *>& getARWaypoint() const;
	std::vector<ARWaypoint *>& getARWaypoint();

	int GetWayPointCount()const;
	ARWaypoint * GetWayPointByIdx(int idx); 

	ARWaypoint *getARWaypoint( int nArwpID);

	void ReadDataByARWaypointID(int nArwpID);

	void ReadAirportInfo();
	double GetAngleCosValueWithARPCoordinateBaseOnRefPoint(const CLatitude& latitude , const CLongitude& longtitude );

	bool HeadingValid(int nCurSel)const;
	int ExistHeading()const;

public:
	void ClearData();
	void ReadData(int nAirRouteID);

	void ReadARWaypoint();
	void ReadLogicRunways();

	void SaveData(int nAirspaceID);
	void UpdateData();
	void DeleteData();

	void DeleteARwayPoint();
	void DeleteLogicRunways();

	void UpdateARWaypoint();
	void UpdateLogicRunways();

public:
	static void ExportAirRoutes(CAirsideExportFile& exportFile);

	void ExportAirRoute(CAirsideExportFile& exportFile);

	void ImportObject(CAirsideImportFile& importFile);

public:
	int HasIntersection(CAirRoute * pAirRoute);

	//is way point id, not arwaypoint id
	bool IsWaypointExists(int nWaypointID);

	void CalWaypointExtentPoint();
	void InitPath();
	const CPath2008& getPath()const{ return m_path;}
	ARWaypoint* getFrontWaypoint(CPoint2008 pointPos);
	ARWaypoint* getAfterWaypoint(ARWaypoint* pForeWaypoint);

private:
	bool IsRunwayExist(int nRwy);


};


class INPUTAIRSIDE_API AirRouteList
{
public:
	AirRouteList();
	~AirRouteList();
	AirRouteList(class AirRouteList const &);
protected:
	std::vector<CAirRoute *> m_vAirRoute;
private:

public:
	int GetAirRouteCount()const;
	CAirRoute *GetAirRoute(int nIndex)const;
	CAirRoute* GetAirRouteByID(int nAirRouteID);
public:
	void ReadData(int nAirspaceID);
	void DeleteData();
	CAirRoute * DeleteAirRoute(int nIndex);
	void AddAirRoute(CAirRoute *pAirRoute);

	void SaveData(int nAirspaceID);

	void CalAirRouteIntersectionListByIdx(int idx);

	void CalAirRouteIntersections();
	void ReCalAirRouteIntersectionsByIdx(int idx);

	AirRouteIntersectionList* GetAirRouteIntersectionList();

public:
	std::vector<CAirRoute *> GetIntersectionAirRoute(CAirRoute * pAirRoute);

protected:
	AirRouteIntersectionList* m_pIntersectionList;

};











