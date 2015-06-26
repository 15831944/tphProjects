#pragma once


#include "InputAirsideAPI.h"
#include "Database/DBElement.h"
#include "Common/FLT_CNST.H"
#include "Common/VectorBase.h"
#include "Database/DBElementCollection.h"
#include "Database/tb_ConflictResolution.h"

enum YesOrNo
{
	No = 0,
	Yes,
};

class CAtIntersectionOnRightList;
class CFirstInATaxiwayList;
class COnSpecificTaxiwaysList;
class CAirsideImportFile;
class CAirsideExportFile;
class CAirportDatabase;



class INPUTAIRSIDE_API RunwayCrossBuffer : public DBElement
{
public:
	enum Case
	{
		_Landing,
		_Takeoff,
	};

	RunwayCrossBuffer();
	void setCase(Case c){ m_case = c ;} 
	Case getCase()const{ return m_case; }

	DistanceUnit getDistanceNM()const;
	void setDistanceNM(DistanceUnit d);

	void setTime(const ElapsedTime& t){ m_time = t;} 
	ElapsedTime getTime()const{ return m_time; }

	void setFlightType(const FlightConstraint& c){ m_fltType = c; }
	const FlightConstraint& getFlightType()const{ return m_fltType; }
	void InitFromDBRecordset(CADORecordset& recordset);
	void Init(CAirportDatabase* p);
protected:
	FlightConstraint m_fltType;
	DistanceUnit m_distance;
	ElapsedTime m_time;
	Case m_case;

	CString m_sfltType;

	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
};

class RunwayCrossBufferList : public DBElementCollection<RunwayCrossBuffer>
{
public:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ReadData( int nParentID );
protected:
	CAirportDatabase* m_pAirportDB;

};

class INPUTAIRSIDE_API CConflictResolution
{

public:
	CConflictResolution();
	~CConflictResolution(void);

	void ReadData(int nProjID);
	void SaveData(int nProjID);
	void SetRadiusOfConcern(int nRadiusOfConcern){m_nRadiusOfConcern = nRadiusOfConcern;}
	int GetRadiusOfConcern(){return m_nRadiusOfConcern;}

	void SetRunwayCrossBuffer(int nSeconds);
	int  GetRunwayCrossBuffer();
	void SetAtIntersectionOnRight(YesOrNo enumAtIntersectionOnRight){m_enumAtIntersectionOnRight = enumAtIntersectionOnRight;}
	YesOrNo GetAtIntersectionOnRight(){return m_enumAtIntersectionOnRight;}
	void SetFirstInATaxiway(YesOrNo enumFirstInATaxiway){m_enumFirstInATaxiway = enumFirstInATaxiway;}
	YesOrNo GetFirstInATaxiway(){return m_enumFirstInATaxiway;}
	void SetOnSpecificTaxiways(YesOrNo enumOnSpecificTaxiways){m_enumOnSpecificTaxiways = enumOnSpecificTaxiways;}
	YesOrNo GetOnSpecificTaxiways(){return m_enumOnSpecificTaxiways;}
	CAtIntersectionOnRightList* GetAtIntersectionOnRightList(){return m_pAtIntersectionOnRightList;}
	CFirstInATaxiwayList* GetFirstInATaxiwayList(){return m_pFirstInATaxiwayList;}
	COnSpecificTaxiwaysList* GetOnSpecificTaxiwaysList(){return m_pOnSpecificTaxiwaysList;}
	RunwayCrossBufferList m_vCrossBuffers;

	ElapsedTime getRunwayAsTaxiwayApproachTime()const{ return m_tRunwayAsTaxiwayNoApporachTime; }
	DistanceUnit getRunwayAsTaxiwayApproachDistNM()const;
	ElapsedTime getRunwayAsTaxiwayTakeoffTime()const{ return m_tRuwnayAsTaxiwayNoTakeoffTime; }
	DistanceUnit getRunwayAsTaxiwayApproachDist()const{ return m_dRunwayAsTaxiwayNoApporachDistance; }
	
	void setRunwayAsTaxiwayApproachTime(const ElapsedTime& t){ m_tRunwayAsTaxiwayNoApporachTime  = t; }
	void setRunwayAsTaxiwayApproachDistNM(DistanceUnit d);
	void setRunwayAsTaxiwayTakeoffTime(const ElapsedTime&t ){ m_tRuwnayAsTaxiwayNoTakeoffTime  = t; }
private:
	int m_nID;
	int m_nRadiusOfConcern;
	int m_nRunwayCrossBuffer;


	ElapsedTime m_tRunwayAsTaxiwayNoApporachTime;
	DistanceUnit m_dRunwayAsTaxiwayNoApporachDistance;
	ElapsedTime m_tRuwnayAsTaxiwayNoTakeoffTime;

	YesOrNo m_enumAtIntersectionOnRight;
	YesOrNo m_enumFirstInATaxiway;
	YesOrNo m_enumOnSpecificTaxiways;
	CAtIntersectionOnRightList* m_pAtIntersectionOnRightList;
	CFirstInATaxiwayList* m_pFirstInATaxiwayList;
	COnSpecificTaxiwaysList* m_pOnSpecificTaxiwaysList;
	CAirportDatabase* m_pAirportDB;
public:
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	static void ExportConflictResolution(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportConflictResolution(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
};

