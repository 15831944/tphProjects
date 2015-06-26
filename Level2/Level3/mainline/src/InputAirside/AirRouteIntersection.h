#pragma once
#include "Common/point2008.h"
#include "InputAirsideAPI.h"

class CAirRoute;
class ARWaypoint;
class CAirsideImportFile;
class CAirsideExportFile;
class  INPUTAIRSIDE_API IntersectedRouteSegment
{
public:
	IntersectedRouteSegment();
	IntersectedRouteSegment(int nIntersectionID);
	~IntersectedRouteSegment();
	IntersectedRouteSegment& operator = (const IntersectedRouteSegment& routeSeg);

	void setID(int id){ m_nID = id;}
	void setAirRouteID(int id) {m_nRouteID = id;}
	void setWaypoint1ID(int id) { m_nWaypoint1ID = id;}
	void setWaypoint2ID(int id) {m_nWaypoint2ID = id;}
	void setIntersectionID(int id){ m_nIntersectionID = id;}

	int getID()const{ return m_nID;}
	int getAirRouteID()const{ return m_nRouteID;}
	int getWaypoint1ID()const{ return m_nWaypoint1ID;}
	int getWaypoint2ID()const{ return m_nWaypoint2ID;}
	int getIntersectionID()const{ return m_nIntersectionID;}

public:
	void UpdateData();
	void DeleteData();
	void SaveData();

	void ImportRouteSeg(CAirsideImportFile& importFile);
	void ExportRouteSeg(CAirsideExportFile& exportFile);

protected:
	int m_nRouteID;
	int m_nWaypoint1ID;
	int m_nWaypoint2ID;
	int m_nIntersectionID;
	int m_nID;
};


class INPUTAIRSIDE_API CAirRouteIntersection
{
public:
	CAirRouteIntersection(void);
	~CAirRouteIntersection(void);
	CAirRouteIntersection& operator = (const CAirRouteIntersection& intersection);

	void setID(int id){ m_nID = id;}
	void setAirspaceID(int id){ m_nAirspaceID = id;}
	void setPos(CPoint2008 pos){ m_pos = pos;}
	void setAltHigh(double altHigh){ m_dAltHigh = altHigh;}
	void setAltLow(double altLow){ m_dAltLow = altLow;}
	void setName(CString strName){ m_strName = strName;}


	int getID()const { return m_nID;}
	CPoint2008 getPos()const { return m_pos;}
	double getAltHigh()const { return m_dAltHigh;}
	double getAltLow()const { return m_dAltLow;}
	CString getName(){ return m_strName;}
	int getIntersectedRouteCount(){ return int(m_vIntersectedRoutes.size());}
	IntersectedRouteSegment getIntersectedRouteSeg(CAirRoute* pRoute);
	IntersectedRouteSegment getIntersectedRouteSegByIdx(int idx){ return m_vIntersectedRoutes.at(idx);}
	bool IsIntersectionInRoute(CAirRoute* pRoute);
	bool IsSamePoint(CPoint2008 pointPos, double dAlthigh,double dAltlow);
	void AddIntersectedRouteSeg(IntersectedRouteSegment Seg){ m_vIntersectedRoutes.push_back(Seg); }
	void DeleteIntersectedRoute(CAirRoute* pRoute);
public:
	void ReadData();
	void DeleteData();
	void SaveData();

public:
	void ImportIntersection(CAirsideImportFile& importFile);
	void ExportIntersection(CAirsideExportFile& exportFile);

protected:
	void DeleteIntersectedRouteList();

protected:
	int m_nID;
	int m_nAirspaceID;
	CPoint2008 m_pos;
	double m_dAltLow;
	double m_dAltHigh;
	CString m_strName;

	std::vector<IntersectedRouteSegment> m_vIntersectedRoutes;
	std::vector<IntersectedRouteSegment> m_vIntersectedRoutesDel;
};

class INPUTAIRSIDE_API AirRouteIntersectionList
{
public:
	AirRouteIntersectionList();
	~AirRouteIntersectionList();

	void AddAirRouteIntersection(CAirRouteIntersection* pIntersection){ m_vAirRouteIntersectionList.push_back(pIntersection);}
	CAirRouteIntersection* GetAirRouteIntersectionByIdx(int idx){ return m_vAirRouteIntersectionList.at(idx);}
	std::vector<CAirRouteIntersection*> GetAirRouteIntersection(CAirRoute* pRoute);
	int GetAllAirRouteIntersectionCount(){ return int(m_vAirRouteIntersectionList.size());}
	void RemoveAllIntersections();
	void CalIntersectionInAirRoutes(CAirRoute* pRoute,CAirRoute*pOtherRoute);
	void UpdateIntersectionList();

	void ReadData(int nAirspaceID);
	void DeleteData(CAirRouteIntersection* pIntersection);
	void SaveData();
	void DeleteData();

	static void ExportAirRouteIntersections(CAirsideExportFile& exportFile);
	static void ImportAirRouteIntersections(CAirsideImportFile& importFile);

protected:
	void GetIntersectedPoints(CAirRoute* pRoute,CAirRoute* pOtherRoute,std::vector<CPoint2008>& PosList);
	double GetPointAltHigh(ARWaypoint* pForeWaypoint, ARWaypoint* pAfterWaypoint,CPoint2008 pointPos);
	double GetPointAltLow(ARWaypoint* pForeWaypoint, ARWaypoint* pAfterWaypoint,CPoint2008 pointPos);


protected:
	std::vector<CAirRouteIntersection*> m_vAirRouteIntersectionList;
	std::vector<CAirRouteIntersection*> m_vAirRouteIntersectionDelList;

};
