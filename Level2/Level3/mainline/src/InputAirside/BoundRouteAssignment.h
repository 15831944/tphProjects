#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "TaxiRoute.h"
#include "../Common/FLT_CNST.H"
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;
class CAirportDatabase;

enum BOUNDROUTEASSIGNMENTSTATUS
{
	BOUNDROUTEASSIGNMENT_SHORTPATH      = 0,
	BOUNDROUTEASSIGNMENT_TIMESHORTPATH,
	BOUNDROUTEASSIGNMENT_PREFERREDROUTE
};


class INPUTAIRSIDE_API CTimeRangeRouteAssignment : public DBElementCollection<CIn_OutBoundRoute>
{
public:
	CTimeRangeRouteAssignment();
	virtual ~CTimeRangeRouteAssignment(void);

public:
	void SetRouteAssignmentStatus(BOUNDROUTEASSIGNMENTSTATUS raStatus);
	BOUNDROUTEASSIGNMENTSTATUS GetRouteAssignmentStatus();

	void SetRouteType(BOUNDROUTETYPE eType);
	BOUNDROUTETYPE GetRouteType();

	//DBElementCollection
	virtual void SaveData(int nParentID);
	virtual void DeleteItemFromIndex(size_t nIndex);
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void DeleteData();
	virtual void DeleteItem(const CIn_OutBoundRoute* dbElement);

	void SetStartTime(ElapsedTime& tStart);
	ElapsedTime GetStartTime();

	void SetEndTime(ElapsedTime& tEnd);
	ElapsedTime GetEndTime(); 

	bool IsInTimeRange(ElapsedTime tTime);


protected:
	BOUNDROUTEASSIGNMENTSTATUS m_nRouteAssignmentStatus; //1:shortest path to stand  2:time shortest path to stand  3:preferred route
	ElapsedTime m_tStartTime;
	ElapsedTime m_tEndTime;
	BOUNDROUTETYPE m_eType;

};

class INPUTAIRSIDE_API CFlightTypeRouteAssignment : public DBElementCollection<CTimeRangeRouteAssignment>
{
public:
	CFlightTypeRouteAssignment();
	virtual ~CFlightTypeRouteAssignment();

	void SetRouteType(BOUNDROUTETYPE eType);
	BOUNDROUTETYPE GetRouteType();

	void SetAirportDB(CAirportDatabase* pDB);

	//DBElementCollection
	virtual void SaveData(int nParentID);
	virtual void DeleteItemFromIndex(size_t nIndex);
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void DeleteData();
	virtual void DeleteItem(const CTimeRangeRouteAssignment* dbElement);
	void SetFltType(const CString& strFltType);

	CString& GetFltTypeName();
	FlightConstraint& GetFltType();



protected:
	BOUNDROUTETYPE m_eType; 
	FlightConstraint m_FltType;
	CAirportDatabase* m_pAirportDB;
	CString m_strFltType;
};


class INPUTAIRSIDE_API CBoundRouteAssignment : public DBElementCollection<CFlightTypeRouteAssignment>
{
public:
	CBoundRouteAssignment(BOUNDROUTETYPE eType,CAirportDatabase* pAirportDB);
	virtual ~CBoundRouteAssignment();

	void SetRouteType(BOUNDROUTETYPE eType);
	BOUNDROUTETYPE GetRouteType();

	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;
	virtual void ReadData(int nProjectID);
	virtual void SaveData(int nProjID);
	virtual void DeleteItemFromIndex(size_t nIndex);
	virtual void DeleteItem(const CFlightTypeRouteAssignment* dbElement);

protected:
	BOUNDROUTETYPE m_eType; 
	CAirportDatabase* m_pAirportDB;
	int m_nPrjID;

};


