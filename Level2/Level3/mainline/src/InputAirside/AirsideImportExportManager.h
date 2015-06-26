#pragma once
#include "InputAirsideAPI.h"
#include <map>
#include <vector>
#include "Common\TERMFILE.H"


const static int ERRORINDEX = -1;
typedef   std::map<int,int> DBINDEXMAP;

class INPUTAIRSIDE_API CAirsideImportExportError: public CObject
{
public:
	CAirsideImportExportError(const CString& strError):m_strError(strError){}
	~CAirsideImportExportError(){}

public:
	CString ErrorMessage(){ return m_strError;}
protected:
	CString m_strError;
private:
};

enum enumAirsideFile
{
	IM_LOGFILE = 0,
	EX_LOGFILE,//1
	AS_PROJECT,//2
	AS_AIRPORT,//3
	AS_GROUND,//4
	AS_PATH,//5
	AS_OBJECT,//6
	AS_RUNWAYINTERSECTION,//7
	AS_HOLDPOSITION,//8
	AS_RUNWAYEXIT,//9

	AS_FLIGHTPLAN,//10
	AS_RUNWAYASSIGNMENT,//11
	
	AS_LRWSTARASSIGNMENT,//12
	AS_LRWSIDASSIGNMEN,//13
	AS_LRWINBOUNDROUTEASSIGNMEN,//14
	AS_LRWOUTBOUNDROUTEASSIGNMEN,//15
	AS_LRWEXITSTRATEGIES,//16
	AS_RWTAKEOFFPOSITION,//17
	AS_AIRCRAFTCLASSIFICATIONS,//18
	AS_GOAROUND,//19
	AS_PUSHBACKCLEARANCE,//20
	AS_TAKEOFFQUEUES,//21
	AS_CONFLICTRESOLUTION,//22
	AS_ITINERANTFLIGHTS,//23
	AS_SIDESTEPSPECIFICATION,//24
	AS_PARKINGSTANDBUFFER,//36
	AS_TAKEOFFSEQUENCING,//37
	AS_TEMPORARYPARKING,//38
	AS_OCCUPIEDASSIGNEDSTAND,//39
	AS_DIRECTROUTING,//40
	AS_FLIGHTPERFORMANCECRUISE,//41
	AS_FLIGHTPERFORMANCETERMINAL,//42
	AS_APPROACHLAND,//43
	AS_PERFORMLANDING,//44
	AS_TAXIINBOUND,//45
	AS_TAXIOUTBOUND,//46
	AS_APPROACHSEPARATION,//47	
	AS_TAKEOFFCLEARANCES,//48
	AS_PUSHBACK,//49
	AS_PERFORMANCETAKEOFF,//50
	AS_DEPCLIMBOUT,//51
	AS_VEHICLESPECIFICATION,//52	
	AS_VEHICLEPOOLPARKING,//53
	AS_TRAFFICLIGHT,//54
	AS_TOLLGATE,//55
	AS_STOPSIGN,//56
	AS_YIELDSIGN,//57
	AS_VEHICLEPOOLSANDDEPLOYMENT,//58
	AS_VEHICLEROUTE,//59
	AS_FLIGHTTYPEDETAILSMANAGER,//60
	AS_SERVICINGREQUIREMENT,//61
	AS_VEHICLEDISPPROP,//62	
	AS_DIRECTIONALITYCONS, //63
	AS_TAXIWAYFLITYPERESTIRCTION,//64
	AS_SLOTSPECIFICATION,//65
	AS_VEHICLETAGS,//66
	AS_PAXBUSPARKING,//67
	AS_TOWOFFSTANDASSIGNMENT,//68
	AS_BAGCARTPARKING,//69
	AS_TOWINGROUTE,//70
	AS_FILLETTAXIWAY,//71
    AS_STRETCHOPTION,//72
	AS_UNITMANAGERSETTING,//73
	AS_ITINERANTFLIGHTSCHEDULE,//74
	AS_ARRIVALSETAOFFSET, //75
	AS_AIRSIDESIMSETTINGCLOSURE,//76
	AS_HOLDSHORTLINES,//77
	AS_APPROACHSEPARATIONCRITERIA_NEW,//78
	AS_INTRAILSEPATATION, //79
    AS_FILEINTERSECTEDSTRETCH,//80
	AS_AIRROUTEINTERSECTION,//81

	//add new type before the last line
	AS_FILECOUNT,  
};


class INPUTAIRSIDE_API AirsideImportDBIndexMap
{
public:
	AirsideImportDBIndexMap();
	~AirsideImportDBIndexMap();


public:
	//add new item
	//nOldIndex: the old index read from files
	//nNewIndex; the new index in the database
	void AddIndexMap(int nOldIndex,int nNewIndex);

	//get the new index
	//nOldIndex, the old index read from files
	//return, success, return the id,
	//        fail,  return ERRORINDEX
	int GetNewIndex(int nOldIndex);


protected:
	DBINDEXMAP m_indexMap;
private:

};
class CAirportDatabase;
class CAirsideImportExportManager;

class CAirsideFile
{
public:
	CAirsideFile(CAirsideImportExportManager* pManager,
				 const CString& strDataFile, //data file
				 const CString& strLogFile,//log file
				 int p_op,
				 int nVersion
				);
	virtual ~CAirsideFile();

public:
	ArctermFile& getFile(){ return m_fileData;}
	void LogInfo(const CString& strInfo);

protected:
	ArctermFile m_fileData;
	ArctermFile m_fileLog;

	CString m_strLogFilePath;
	int m_nVersion;
	int m_fileType;

	CAirsideImportExportManager* m_pManager;
};

class INPUTAIRSIDE_API CAirsideImportFile: public CAirsideFile
{
public:
	CAirsideImportFile(CAirsideImportExportManager* pManager,
		const CString& strDataFile, //data file
		const CString& strLogFile,//log file
		int nVersion
		);
	~CAirsideImportFile();


public:
	int getVersion();
	void AddObjectIndexMap(int nOldIndex,int nNewIndex);
	int GetObjectNewID(int nOldObjectIndex);

	void AddAircraftClassificationsIndexMap(int nOldIndex,int nNewIndex);
	int GetAircraftClassificationsNewID(int nOldObjectIndex);

	void AddPathIndexMap(int nOldIndex,int nNewIndex);
	int GetPathNewID(int nOldPathIndex);

	void AddAirportIndexMap(int nOldIndex,int nNewIndex);
	int GetAirportNewIndex(int nOldIndex);

	void AddAirRouteIndexMap(int nOldIndex,int nNewIndex);
	int GetAirRouteNewIndex(int nOldIndex);

	void AddAirRouteARWaypointIndexMap(int nOldIndex,int nNewIndex);
	int GetAirRouteARWaypointNewIndex(int nOldIndex);

	void AddIntersecctionIndexMap(int nOldIndex,int nNewIndex);
	int GetIntersectionIndexMap(int nOldIndex);

	void AddRunwayExitIndexMap(int nOldIndex,int nNewIndex);
	int GetRunwayExitIndexMap(int nOldIndex);

	void AddTemporaryParkingIndexMap(int nOldIndex, int nNewIndex);
	int GetTemporaryParkingIndexMap(int nOldIndex);

	void SetNewProjectID(int nProjID);
	int getNewProjectID();

	int getNewAirspaceID();
	
	int getNewTopographyID();

	void AddTakeoffpositionRwmkdataIndexMap(int nOldIndex,int nNewIndex);
	int GetTakeoffpositionRwmkdataNewID(int nOldIndex);


	void AddVehicleDataSpecificationIndexMap(int nOldIndex,int nNewIndex);
	int GetVehicleDataSpecificationNewID(int nOldIndex);

	void AddIntersectItemIndexMap(int nOldIndex, int nNewIndex);
	int GetIntersectItemNewID(int nOldIndex);

	void AddStandLeadInLineItemIndexMap(int nOldIndex, int nNewIndex);
	int GetStandLeadInLineNewID(int nOldIndex);

	void AddStandLeadOutLineItemIndexMap(int nOldIndex, int nNewIndex);
	int GetStandLeadOutLineNewID(int nOldIndex);

protected:
	//AirsideImportDBIndexMap* m_pObjectIndexMap;
	//AirsideImportDBIndexMap* m_pPathIndexMap;
	//AirsideImportDBIndexMap* m_pAirportIndexMap;

private:
};

class INPUTAIRSIDE_API CAirsideExportFile: public CAirsideFile
{
public:
	CAirsideExportFile(	CAirsideImportExportManager* m_pManager,const CString& strDataFile,const CString& strLogFile,int nVersion);
	~CAirsideExportFile();	
public:

	void AddAirportID(int nAirportID);
	std::vector<int>& GetAirportIDList();

	int GetAirspaceID();

	int GetTopographyID();

	int GetProjectID();
private:
};

class CMasterDatabase;
class INPUTAIRSIDE_API CAirsideImportExportManager
{
public:
	CAirsideImportExportManager(const CString& strAirsideDir,const CString& strProjName);
	~CAirsideImportExportManager(void);
	
public:
	void ImportAirside(CMasterDatabase *pMasterDatabase,const CString& strAppPath,CString& strOldProjName,CString& strDatabaseName,bool bExistProject);
	void ExportAirside(const CString& strDatabaseName,int nVersion);

	CString GetFilePath(enumAirsideFile enumFile);


public:
	void AddObjectIndexMap(int nOldIndex,int nNewIndex);
	int GetObjectNewID(int nOldObjectIndex);

	void AddAircraftClassificationsIndexMap(int nOldIndex,int nNewIndex);
	int GetAircraftClassificationsNewID(int nOldObjectIndex);

	void AddPathIndexMap(int nOldIndex,int nNewIndex);
	int GetPathNewID(int nOldPathIndex);

	void AddAirportIndexMap(int nOldIndex,int nNewIndex);
	int GetAirportNewIndex(int nOldIndex);

	void SetNewProjectID(int nProjID);
	int getNewProjectID();

	void AddAirRouteIndexMap(int nOldIndex,int nNewIndex);
	int GetAirRouteNewIndex(int nOldIndex);

	void AddAirRouteARWaypointIndexMap(int nOldIndex,int nNewIndex);
	int GetAirRouteARWaypointNewIndex(int nOldIndex);


	void AddIntersecctionIndexMap(int nOldIndex,int nNewIndex);
	int GetIntersectionIndexMap(int nOldIndex);

	void AddRunwayExitIndexMap(int nOldIndex,int nNewIndex);
	int GetRunwayExitIndexMap(int nOldIndex);

	void AddTemporaryParkingIndexMap(int nOldIndex, int nNewIndex);
	int GetTemporaryParkingIndexMap(int nOldIndex);

	void AddAirportID(int nAirportID);
	std::vector<int>& GetAirportIDList();

	int getVersion();
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void AddTakeoffpositionRwmkdataIndexMap(int nOldIndex,int nNewIndex);
	int GetTakeoffpositionRwmkdataNewID(int nOldIndex);

	void AddVehicleDataSpecificationIndexMap(int nOldIndex,int nNewIndex);
	int GetVehicleDataSpecificationNewID(int nOldIndex);

	void AddIntersectItemIndexMap(int nOldIndex, int nNewIndex);
	int GetIntersectItemNewID(int nOldIndex);

	void AddLeadInLineIndexMap(int nOldIndex, int nNewIndex);
	int GetLeadInLineNewID(int nOldIndex);

	void AddLeadOutLineIndexMap(int nOldIndex, int nNewIndex);
	int GetLeadOutLineNewID(int nOldIndex);

protected:
	void ExportProjectInfo(const CString& strDatabaseName);
	void ImportProjectInfo(CString& strDatabaseName,CString& strOldProjName);
	void ExportAirportInfo();
	void ImportAirportInfo();
	void ExportObjects();
	void ImportObjects();

protected:
	int m_nVersion;
	CString m_strAirsideDir;
	CString m_strProjName;
	AirsideImportDBIndexMap m_objectIndexMap;
	AirsideImportDBIndexMap m_pathIndexMap;
	AirsideImportDBIndexMap m_airportIndexmap;
	AirsideImportDBIndexMap m_airRouteIndexmap;
	AirsideImportDBIndexMap m_intersectionNodeIndexMap;
	AirsideImportDBIndexMap m_airRouteARWaypointIndexmap;
	AirsideImportDBIndexMap m_runwayExitIndexMap;
	AirsideImportDBIndexMap m_takeoffpositionRwmkdataIndexMap;
	AirsideImportDBIndexMap m_aircraftclassificationsIndexMap;
	AirsideImportDBIndexMap m_vehicleDataSpecificationIndexMap;
	AirsideImportDBIndexMap m_temporaryParkingIndexMap;
	AirsideImportDBIndexMap m_intersectItemIndexMap;
	AirsideImportDBIndexMap m_standLeadInLineIndexMap;
	AirsideImportDBIndexMap m_standLeadOutLineIndexMap;

	int m_nProjectID;
	int m_nOldProjectID;
	//ArctermFile m_fileLog;
	//ArctermFile m_fileData;
	CAirportDatabase* m_pAirportDB;

protected:
	std::vector<int> m_vAirportID;
};
