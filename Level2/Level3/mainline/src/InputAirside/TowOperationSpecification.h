#pragma once
#include "ALT_BIN.h"
#include "deicepad.h"
#include "HoldShortLines.h"
#include "Runway.h"
#include "../Common/FLT_CNST.H"
#include "TowOperationRoute.h"
#include "ALTObjectGroup.h"
#include "VehicleDefine.h"

#include "InputAirsideAPI.h"

class Runway;
class CAirportDatabase;

//Tow operation type definition

const CString TOWOPERATIONTYPENAME[] = 
{
	_T("Push back to taxiway"),
	_T("Push & Tow"),
	_T("Pull & Tow"),
	_T("Push back to release point"),

	//add new one according to enumeration TOWOPERATIONTYPE
	_T("Count")
};
//Tow operation destination definition
enum TOWOPERATIONDESTINATION
{
	TOWOPERATIONDESTINATION_STAND,
	TOWOPERATIONDESTINATION_DEICINGPAD,
	TOWOPERATIONDESTINATION_TAXIWAYHOLDSHORTLINE,
	TOWOPERATIONDESTINATION_RUNWAYTHRESHOLD,
	TOWOPERATIONDESTINATION_STARTPOSITION,


	//NOTE:add new type before this line
	TOWOPERATIONDESTINATION_COUNT// the tow operation destination count

};

const CString TOWOPERATIONDESTINATIONNAME[] =
{
	_T("Stand"),
	_T("Deicing pad"),
	_T("Hold short line of taxiway"),
	_T("Threshold of runway"),
	_T("Start position"),


	//add new according to enumeration TOWOPERATIONDESTINATION
	_T("Count")
};
enum TOWOPERATIONROUTETYPE
{
	TOWOPERATIONROUTETYPE_SHORTESTPATH = 0,
	TOWOPERATIONROUTETYPE_USERDEFINE,


	//NOTE: add new type before this line
	TOWOPERATIONROUTETYPE_COUNT// the tow operation destination type
};

const CString TOWOPERATIONROUTETYPENAME[] = 
{
	_T("Shortest path"),
	_T("User define"),


	//add new according to TOWOPERATIONROUTETYPE
	_T("Count")
};

class INPUTAIRSIDE_API CReleasePoint: public DBElement
{
	friend class CTowOperationSpecification;
	friend class CTowOperationSpecFltTypeData;
public:
	CReleasePoint();
	~CReleasePoint();

public:
	//objID according to TOWOPERATIONDESTINATION
	virtual void InitFromDBRecordset(CADORecordset& recordset);

public:

	TOWOPERATIONDESTINATION GetEnumDestType() const { return m_enumDestType; }
	void SetEnumDestType(TOWOPERATIONDESTINATION type) { m_enumDestType = type; }


	int GetObjectID() const { return m_nObjectID; }
	void SetObjectID(int nID) { m_nObjectID = nID; }

	RUNWAY_MARK GetRunwayMark() const { return m_runwayMark; }
	void SetRunwayMark(RUNWAY_MARK runwayMark) { m_runwayMark = runwayMark; }

	TOWOPERATIONROUTETYPE GetEnumOutRouteType() const { return m_enumOutRouteType; }
	void SetEnumOutRouteType(TOWOPERATIONROUTETYPE type) { m_enumOutRouteType = type; }


	CTowOperationRoute* GetOutBoundRoute();

	TOWOPERATIONROUTETYPE GetEnumRetRouteType() const { return m_enumRetRouteType; }
	void SetEnumRetRouteType(TOWOPERATIONROUTETYPE type) { m_enumRetRouteType = type; }

	CTowOperationRoute* GetReturnRoute();

	void SaveData(int nFltID);
	void DeleteData();

	CString getName(){return m_strName;}
	void setName(CString strName){ m_strName = strName; }

protected:
	virtual void GetInsertSQL(int StandGroupID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

protected:
	int m_nObjectID;

	TOWOPERATIONDESTINATION m_enumDestType;

	//ALTObjectGroup m_objgroup;	// if m_enumOperartionType == TOWOPERATIONDESTINATION_STAND or TOWOPERATIONDESTINATION_DEICINGPAD

	////TOWOPERATIONDESTINATION_TAXIWAYHOLDSHORTLINE
	//CHoldShortLineNode m_holdShortLineNode;


	////TOWOPERATIONDESTINATION_RUNWAYTHRESHOLD
	//Runway m_runway;
	RUNWAY_MARK m_runwayMark;
	CString m_strName;

	TOWOPERATIONROUTETYPE m_enumOutRouteType;
	CTowOperationRoute m_outBoundRoute;//only valid while m_enumTowRouteType == TOWOPERATIONROUTETYPE_USERDEFINE

	TOWOPERATIONROUTETYPE m_enumRetRouteType;
	CTowOperationRoute m_retRoute;

};

class INPUTAIRSIDE_API CTowOperationSpecFltTypeData: public DBElementCollection<CReleasePoint> 
{
	friend class CTowOperationSpecification;

public:
	CTowOperationSpecFltTypeData();
	~CTowOperationSpecFltTypeData();

public:
	void SaveData(int StandGroupID);
	void DeleteData();
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	void SetAirportDatabase(CAirportDatabase* pAirportDatabase) { m_pAirportDatabase = pAirportDatabase; }

	TOWOPERATIONTYPE getOperationType(){ return m_enumOperartionType; }
	

protected:
	void GetInsertSQL(int StandGroupID,CString& strSQL) const;
	void GetUpdateSQL(CString& strSQL) const;
	void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

public:
	FlightConstraint& GetFltConstraint() { return m_fltConstraint; }
	void SetFltConstraint(const FlightConstraint& fltCons) { m_fltConstraint = fltCons;}

	TOWOPERATIONTYPE GetEnumOperartionType() const { return m_enumOperartionType; }
	void SetEnumOperartionType(TOWOPERATIONTYPE type) { m_enumOperartionType = type; }

protected:
	CAirportDatabase* m_pAirportDatabase;
	FlightConstraint m_fltConstraint;

	TOWOPERATIONTYPE m_enumOperartionType;
	int m_nID;
};

class INPUTAIRSIDE_API CTowOperationSpecStand: public DBElementCollection<CTowOperationSpecFltTypeData>
{
	friend class CTowOperationSpecification;
public:
	CTowOperationSpecStand();
	~CTowOperationSpecStand();


public:

	void SaveData();
	void DeleteData();

	virtual void InitFromDBRecordset(CADORecordset& recordset);

public:

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase) { m_pAirportDatabase = pAirportDatabase; }
	int GetProjID() const { return m_nProjID; }
	void SetProjID(int nProjID) { m_nProjID = nProjID; }

	ALTObjectGroup GetStandGroup() const { return m_standGroup; }
	void SetStandGroup(ALTObjectGroup standGroup) { m_standGroup = standGroup; }
protected:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	void UpdateData();

	//int m_nID;//id in 
	ALTObjectGroup m_standGroup;

	int m_nProjID;

	CAirportDatabase* m_pAirportDatabase;

	
};

class INPUTAIRSIDE_API CTowOperationSpecification
{
public:
	CTowOperationSpecification(int nProjID,CAirportDatabase* pAirportDatabase);
	~CTowOperationSpecification(void);

public:
	void ReadData();
	void SaveData();

	CTowOperationSpecFltTypeData* GetTowOperationSpecFltTypeData(const FlightConstraint& fltType, const ALTObjectGroup& standGroup);
	void AddNewItem(CTowOperationSpecStand *);

	void DeleteItem(CTowOperationSpecStand *);

	int GetItemCount();

	CTowOperationSpecStand * GetItem(int nIndex);
	
protected:
	std::vector<CTowOperationSpecStand *> m_vStandData;
	std::vector<CTowOperationSpecStand *> m_vDelStandData; 

	CAirportDatabase* m_pAirportDatabase;
	int m_nProjID;

};





























