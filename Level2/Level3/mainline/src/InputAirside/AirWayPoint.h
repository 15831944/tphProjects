#pragma once
#include "altobject.h"
#include "..\Common\point2008.h"
#include "../Common/LatLongConvert.h"

#include "InputAirsideAPI.h"

class ALTAirport;
class CLatitude;
class CLongitude;

class INPUTAIRSIDE_API AirWayPoint :
	public ALTObject
{
friend class CWayPoint3D;
public:
	typedef ref_ptr<AirWayPoint> RefPtr;
public:
	AirWayPoint(void);
	virtual ~AirWayPoint(void);
	
	virtual ALTOBJECT_TYPE GetType()const { return ALT_WAYPOINT; } ;
	virtual CString GetTypeName()const  { return "WayPoint"; };


	// {72F8E5F4-F38D-4340-BAEC-1ACF9D617F29}
	//static const GUID class_guid;
	static const GUID& getTypeGUID();
private:
	//share
	int m_nflag;

	//method0 WayPointByPosition
	CPoint2008 m_serviceLocation;
	CLatitude m_lat;
	CLongitude m_long;
	//method1 WayPointByWayPointBearingDistance
	int m_nOtherWayPointID;
	AirWayPoint::RefPtr m_pOtherWayPoint;
	double m_dBearing;
	double m_dDistance;
	//method2 WayPointByWayPointBearingAndWayPointBearing
	int m_nOtherWayPoint1ID;
	AirWayPoint::RefPtr m_pOtherWayPoint1;
	double m_dBearing1;
	int m_nOtherWayPoint2ID;
	AirWayPoint::RefPtr m_pOtherWayPoint2;
	double m_dBearing2;

	//share
	double m_dlowlimit;
	double m_dhightlimit;
	int m_nAptID;
	//std::vector<AirWayPoint> m_vWaypoint;
	//database operation
public:
	void SetFlag(int nflag){ m_nflag = nflag;}
	int GetFlag(){ return m_nflag;}

	void SetOtherWayPointID(int nOtherWayPointID){ m_nOtherWayPointID = nOtherWayPointID;}
	int GetOtherWayPointID(){ return m_nOtherWayPointID;}
    AirWayPoint * GetOtherWayPoint();
	void SetOtherWayPoint(AirWayPoint * pWayPt){ m_pOtherWayPoint = pWayPt; }
	void SetBearing(double dBearing){ m_dBearing = dBearing;}
	double GetBearing(){ return m_dBearing;}
	void SetDistance(double dDistance){ m_dDistance = dDistance;}
	double GetDistance(){ return m_dDistance;}
	void WayPointBearingDistanceClear();

	void SetOtherWayPoint1ID(int nOtherWayPoint1ID){ m_nOtherWayPoint1ID = nOtherWayPoint1ID;}
	int GetOtherWayPoint1ID(){ return m_nOtherWayPoint1ID;}
	AirWayPoint * GetOtherWayPoint1();
	void SetOtherWayPoint1(AirWayPoint * pWayPt ){ m_pOtherWayPoint1 = pWayPt; }
	void SetBearing1(double dBearing1){ m_dBearing1 = dBearing1;}
	double GetBearing1(){ return m_dBearing1;}

	void SetOtherWayPoint2ID(int nOtherWayPoint2ID){ m_nOtherWayPoint2ID = nOtherWayPoint2ID;}
	int GetOtherWayPoint2ID(){ return m_nOtherWayPoint2ID;}
	AirWayPoint * GetOtherWayPoint2();
	void SetOtherWayPoint2(AirWayPoint * pWaypoint){ m_pOtherWayPoint2 = pWaypoint; }
	void SetBearing2(double dBearing2){ m_dBearing2 = dBearing2;}
	double GetBearing2(){ return m_dBearing2;}

	void WayPointBearingAndWayPointBearingClear();

	void SetLowLimit(double dLimit){ m_dlowlimit = dLimit;}
	double GetLowLimit() const; 

	void SetHighLimit(double dLimit){ m_dhightlimit = dLimit;}
	double GetHighLimit() const; 

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();
	virtual bool CopyData(const ALTObject* pObj);

	//
	static void GetWayPointList(int nAirspaceID , std::vector<AirWayPoint>& list);
	
	//for
	CPoint2008 GetLocation(const ALTAirport& refARP);
	void SetLatLong(const CLatitude& latitude , const CLongitude& longtitude);

	void SetServicePoint(const CPoint2008& _pt);
	CPoint2008 GetServicePoint();	
	void WayPointPositionClear(){m_serviceLocation.setPoint(0,0,0);}

public:
	static void ExportWaypoints(CAirsideExportFile& exportFile);

	void ExportWaypoint(CAirsideExportFile& exportFile);
	bool IsInwaypointListOrder(ALTObjectUIDList* pwaypointListOrder,int nID);
	void ImportObject(CAirsideImportFile& importFile);

protected:
//	int m_nSvcptID;

	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;

};
