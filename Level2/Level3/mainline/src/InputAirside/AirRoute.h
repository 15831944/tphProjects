#pragma once


#include <string>
#include <vector>


#include "./AirWayPoint.h"
#include "./ALTAirport.h"
#include "../Common/Point2008.h"
#include "../Common/Path2008.h"
#include "InputAirsideAPI.h"

class Runway;
class AirRouteIntersectionList;
class CAirRouteIntersection;

using namespace std;

class INPUTAIRSIDE_API ARWaypoint
{
public:
    enum ARWaypointType
    {
        ARWayPoint_AirWayPoint = 0, // AirWayPoint
        ARWayPoint_Altitude = 1 // Altitude & Intercept angle.
    };

	enum DepartType{ NextWaypoint = 0, Heading };
	enum HeadingType{ Aligned = 0, Direct ,None };

    enum ToNextType{ ToNextWaitPoint = 0, ToNextAltitude };
    enum DirectType{ DirectType_Direct = 0, InterceptTrack };
public:

	ARWaypoint(int nID = -1);
	~ARWaypoint();

	ARWaypoint(const ARWaypoint&);
protected:
	//AirWayPoint m_pWaypoint;
	int m_nID;
	double m_lMinSpeed;
	double m_lMaxSpeed;
	double m_lMinHeight;
    double m_lMaxHeight;
    int m_nSequenceNum;

    ARWaypointType m_nWaypointType;
    // if m_nWaypointType is ARWayPoint_AirWayPoint, these members are used.
    AirWayPoint m_wayPoint;
	DepartType m_DepartType;
	HeadingType m_HeadingType;
	long m_lDegree;
	double m_lVisDistance;
	CPoint2008 m_ExtentPoint;

    // m_nWaypointType is ARWayPoint_Altitude, these members are used
    double m_fAltitude;
    double m_fAngle;
    ToNextType m_toNextPtType;
    DirectType m_directTpye;
    double m_inboundTrackAngle;
    double m_interceptAngle;

public:
	// get a copy of this object , change its name in sequence
	//virtual ALTObject * NewCopy();

	int getID()const;
	void setID(int nID);

	void setMinSpeed(double lSpeed);
	void setMaxSpeed(double lSpeed);
	
	void SetSequenceNum(int nSequenceNum);
	int GetSequenceNum(void);

	double getMinSpeed() const;
	double getMaxSpeed() const;

	void setMinHeight(double lMinHeight);
	void setMaxHeight(double lMaxHeight);

	double getMinHeight() const;
	double getMaxHeight() const;

	void setWaypoint(const AirWayPoint& altobj){ m_wayPoint = altobj;}
	AirWayPoint& getWaypoint();
	const AirWayPoint& getWaypoint() const;

	void setDepartType(DepartType type);
	void setHeadingType(HeadingType type);
	void setDegrees(long lDegrees);
	void setVisDistance(double lDistance);
	void setExtentPoint( CPoint2008 extentPoint);

	DepartType getDepartType();
	HeadingType getHeadingType();
	long getDegrees();
	double getVisDistance();
	CPoint2008 getExtentPoint() const;


	double getDefaultAltitude();

    ARWaypoint::ARWaypointType GetWaypointType() const { return m_nWaypointType; }
    void SetWaypointType(ARWaypoint::ARWaypointType val) { m_nWaypointType = val; }
    double GetAltitude() const { return m_fAltitude; }
    void SetAltitude(double val) { m_fAltitude = val; }
    double GetAngle() const { return m_fAngle; }
    void SetAngle(double val) { m_fAngle = val; }
    ARWaypoint::ToNextType GetNextPtType() const { return m_toNextPtType; }
    void SetNextPtType(ARWaypoint::ToNextType val) { m_toNextPtType = val; }
    ARWaypoint::DirectType GetDirectTpye() const { return m_directTpye; }
    void SetDirectTpye(ARWaypoint::DirectType val) { m_directTpye = val; }
    double GetInboundTrackAngle() const { return m_inboundTrackAngle; }
    void SetInboundTrackAngle(double val) { m_inboundTrackAngle = val; }
    double GetInterceptAngle() const { return m_interceptAngle; }
    void SetInterceptAngle(double val) { m_interceptAngle = val; }

	ARWaypoint&  operator =(const ARWaypoint& );

	//database operation
public:
	void UpdateData();
	void DeleteData();
	void SaveData(int nAirRouteID);

public:
	void ImportObject(CAirsideImportFile& importFile,int nAirRouteID);
	void ExportObject(CAirsideExportFile& exportFile);

};

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











