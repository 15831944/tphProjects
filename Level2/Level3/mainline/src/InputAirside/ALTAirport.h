#pragma once
#include <string>
#include "..\Common\point2008.h"

#include "..\Common\Referenced.h"
#include "..\Common\ref_ptr.h"
#include "..\Common\ARCVector.h"
#include "./ALTObject.h"
#include <map>
#include "../Common/Guid.h"
#include "../InputAirside/Runway.h"
#include "../InputAirside/DeicePad.h"
#include "../InputAirside/Taxiway.h"
#include "../InputAirside/Stand.h"
#include "../InputAirside/Stretch.h"
#include "../InputAirside/Intersections.h"
#include "../InputAirside/VehiclePoolParking.h"
#include "../InputAirside/TrafficLight.h"
#include "../InputAirside/TollGate.h"
#include "../InputAirside/StopSign.h"
#include "../InputAirside/YieldSign.h"
#include "../InputAirside/Heliport.h"
#include "../InputAirside/CircleStretch.h"
#include "../InputAirside/MeetingPoint.h"
#include "../InputAirside/StartPosition.h"
class CADORecordset;
class CAirsideExportFile;
class CAirsideImportFile;
class GroundRoutes;
class CAirsideImportExportManager;
class CLatitude;
class CLongitude;
class Runway;
class Taxiway;
class CReportingArea;

class Heliport  ;
class Stand  	;
class DeicePad 	;
class Stretch 	;
class Intersections 	;
class VehiclePoolParking 	;
class TrafficLight 	;
class TollGate ;
class StopSign 	;
class YieldSign;
class CircleStretch ;
class CStartPosition ;
class CMeetingPoint ;

enum MAGDIRECTION
{
	MAGEAST = 0,
	MAGWEST
};
enum Airsdie_Resource_ID
{
	AIRSIDE_RUNWAY = 0 ,
	AIRSIDE_TAXIWAY  ,
	AIRSIDE_STAND,
	AIRSIDE_VEHICLE_POOLPARKING,
	AIRSIDE_REPORTING_AREA,
	AIRSIDE_DEICEPAD
};
struct MagneticVariation
{

	double m_dval;
	int m_iEast_West;

	MagneticVariation(){ m_dval = 0; m_iEast_West = MAGEAST;}
	bool IsEastVar()const{ return m_iEast_West == MAGEAST; }
	double GetRealVariation()const{
		if( IsEastVar() )
		{
			return m_dval;
		}
		else{
			return -m_dval;
		}
	}
};

class INPUTAIRSIDE_API ALTAirport// : public Referenced
{
public:
	//Guid
	const CGuid& getGuid() const;



	ALTAirport(void);
	~ALTAirport(void);
	ALTAirport(const ALTAirport& );

	static const GUID& getTypeGUID();
private:
	CGuid m_guid;
private:
	std::string m_strName;
	//reference info
	long m_lLongitude;    // degrees
	long m_lLatitude;     // degrees
	double m_dElevation;    // ft;
	
	CPoint2008 m_refpoint;     // refer point to airport stuff 

	MagneticVariation m_magVar;


	//o
public:
	int getID() const;
	void setID(int nAirportID){m_nAirportID = nAirportID;}
	CString getName() const{ return m_strName.c_str();}
	void setName(CString& strName){ m_strName = (const char *)strName;}

	void setLongitude(long lValue){ m_lLongitude = lValue;}
	CString getLongtitude() const;
//	void setLongtitude(const CString& strLong);

	void setlatitude(long lValue){ m_lLatitude = lValue;}
	CString getLatitude() const;
//	void setLatitude(const CString& strLat);

	double getElevation() const;
	void setElevation(double dElevation){m_dElevation = dElevation; }

	CPoint2008 getRefPoint()const;
	void setRefPoint(const CPoint2008& refPoint){ m_refpoint = refPoint;}

	void ReadRecord(CADORecordset& adoRecordset);
	void ReadAirport(int nAirportID);
	void UpdateAirport(int nAirportID);
	void SaveAirport(int nProjID);
//	void DeleteAirport(int nAirportID);

	int GetAirportID() const{ return m_nAirportID;}
	
	void NewAirport(int nProjID);

	MagneticVariation& GetMagnectVariation();
	MagneticVariation GetMagnectVariation()const;
	void SetMagneticVariation(const MagneticVariation& magVar) { m_magVar = magVar; }
	CPoint2008 GetLatLongPos(const CLatitude& latitude , const CLongitude& longtitude )const;


	void GetPosLatLong(const CPoint2008& in_pt, CLatitude& out_latitude, CLongitude& out_longtitude )const;

	static void UpdateRunwayExitsNames(int nAirportID);
protected:
	int m_nAirportID;

	CString GetSelectScript(int nAirportID) const;
	CString GetInsertScript(int nAirportID) const;
	virtual CString GetUpdateScript(int nAirportID) const;
	virtual CString GetDeleteScript(int nAirportID) const;


public:
	static void GetMeetingPointIDs(int nAirportID, std::vector<int>& vMeetingPointID);
	static void GetStartPositionIDs(int nAirportID, std::vector<int>& vStartPositionID);
	static void GetNoseInParkingsIDs(int nAirportID,std::vector<int>& vrNoseInParkingID);
	static void GetYieldSignsIDs(int nAirportID,std::vector<int>& vrYieldSignID);
	static void GetStopSignsIDs(int nAirportID,std::vector<int>& vrStopSignID);
	static void GetTollGatesIDs(int nAirportID,std::vector<int>& vrTollGateID);
	static void GetTrafficLightsIDs(int nAirportID,std::vector<int>& vrTrafficLightID);
	static void GetLineParkingsIDs(int nAirportID,std::vector<int>& vrLineParkingID);
	static void GetLaneAdaptersIDs(int nAirportID,std::vector<int>& vrLaneAdapterID);
	static void GetTurnoffsIDs(int nAirportID,std::vector<int>& vrTurnoffID);
	static void GetRoundaboutsIDs(int nAirportID,std::vector<int>& vrRoundaboutID);
	static void GetRoadIntersectionsIDs(int nAirportID,std::vector<int>& vrRoadIntersectionsID);
	static void GetStretchsIDs(int nAirportID,std::vector<int>& vrStretchID);
	static void GetHeliportsIDs(int nAirportID,std::vector<int>& vrHeliportID);
	static void GetVehiclePoolParkingIDs(int nAirportID,std::vector<int>& vrVehiclePoolParkingID);
	static void GetPaxBusParkPosIDs(int nAirportID,std::vector<int>& vrSpotID );
	static void GetBagCartParkPosIDs(int nAirportID,std::vector<int>& vrSpotID);

	static void GetCircleStretchsIDs(int nAirportID,std::vector<int>& vrCircleStretchID);

	static void GetRunwaysIDs(int nAirportID, std::vector<int>& vRunwayID);
	static void GetTaxiwaysIDs(int nAirportID, std::vector<int>& vTaxiwayID);
	static void GetStandsIDs(int nAirportID, std::vector<int>& vStandList);
	static void GetDeicePadsIDs(int nAirportID, std::vector<int>& vDeicePadList);
	static void GetDeicePadsAsStandsIDs(int nAirportID, std::vector<int>& vDeicePadAsStandList);
	static void GetDeicePadsNotAsStandsIDs(int nAirportID, std::vector<int>& vDeicePadList);
	static void GetGroundRouteIDs(int nAirportID, std::vector<int>& vGroundRouteID);
	static void GetParkingPlaceIDs(int nAirportID, std::vector<int>& vParkingPlaceID);
	static void GetReportingAreaIDs(int nAirportID, std::vector<int>& vAreaID);
	
	//get the resource by type
	static void GetAirsideResourceObject(int nAirportID, ALTObjectList& vObjectList,Airsdie_Resource_ID _ResourceTy) ;
	static void GetVehiclePoolParkingList(int nAirportID, ALTObjectList& vObjectList);
	static void GetTaxiwayList(int nAirportID, ALTObjectList& vObjectList);
	static void GetRunwayList(int nAirportID, ALTObjectList& vObjectList);
	static void GetReportingAreaList(int nAirportID, ALTObjectList& vObjectList);
	static void GetMeetingPointList(int nAirportID, ALTObjectList& vObjectList);
	static void GetStartPositionList(int nAirportID, ALTObjectList& vObjectList);
	static void GetPaxBusParkSpotList(int nAirportID, ALTObjectList& vObjectList);

	//GetStandList: will get all stands and deice pads which used as stands
	static void GetStandList(int nAirportID, ALTObjectList& vObjectList);

	//GetDeicePadList: will get all deice pads except those used as stands
	static void GetDeicePadList(int nAirportID, ALTObjectList& vObjectList);

	static void GetContoursIDs(int nAirportID, std::vector<int>& vContourID);
	static void GetRootContoursIDs(int nAirportID, std::vector<int>& vContourID);

	static void GetAirportList(int nProjID,std::vector<ALTAirport>&);
	//if the project is not exist in database, create new default settings, and return the default airport ID
	//return airport id
	// -1, the project has existed
	static int InitializeAirside(const CString& strModelName, int& nAirportID);
	//return project id
	static int GetProjectIDByName(const CString& strModelName);
	//return project id
	static int NewProject(const CString& strModelName);
	//return airport id
	static int InitializeDefaultAirport(int nProjID);
	static void DeleteAirport(int nAirportID);

	static void DeleteAllObject(int nAirportID);
	static void UpdateName(const CString& strName, int nAirportID);


	static void GetAirportsName(int nPrjID, std::map<int, CString>& nameMap );
//helper class 
	ARCVector3 GetWorldLocation(long latitude, long longtitude, double elevation)const;

	static void ExportAirports(CAirsideExportFile& exportFile,int nProject);
	static void ImportAirports(CAirsideImportFile& importFile);
	void ExportAirport(CAirsideExportFile& exportFile);
	void ImportAirport(CAirsideImportFile& importFile);

	static void ExportObjects(int nAirportID,CAirsideExportFile& objectFile);
	static void ExportMiscObjects(CAirsideImportExportManager*);
	static void ImportMiscObjects(CAirsideImportExportManager*);
	static void sortAllStandByName(ALTObjectList& vObjectList);
	//data zone};

};










