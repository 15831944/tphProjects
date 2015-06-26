#pragma once
#include "..\Database\DBElement.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "Common/FLT_CNST.H"
#include "Common/AirportDatabase.h"
#include "../Common/ALTObjectID.h"
#include "../Common/elaptime.h"
#include "../Common/AirsideFlightType.h"
#include "InputAirsideAPI.h"
#include "../Common/DBData.h"
#include "../Common/TimeRange.h"
class CAirsideImportFile;
class CAirsideExportFile;
class CAirportDatabase;


// the enumeration to identify
//     whether we build a NULL data and then read from database
//     or just new a Non-NULL data for saving into database if necessary
enum TowOffDataContentNonNull
{
	ContentNull = 0,
	ContentNonNull,
};

class INPUTAIRSIDE_API CTowOffStandReturnToStandEntry : public DBElement
{
public:
	CTowOffStandReturnToStandEntry(void);
	~CTowOffStandReturnToStandEntry(void);

	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	int GetStandID() const { return m_nStandID; }
	void SetStandID(int val) { m_nStandID = val; InitStandName(); }
	ALTObjectID GetStandName() const;
	CString GetStandNameString() const;
	int GetTimeBeforeEmplanement() const { return m_nTimeBeforeEmplanement; }
	void SetTimeBeforeEmplanement(int val) { m_nTimeBeforeEmplanement = val; }

protected:
	void InitStandName();
private:
	int				m_nStandID;
	ALTObjectID		m_StandName;
	int				m_nTimeBeforeEmplanement;
};


class INPUTAIRSIDE_API CTowOffStandReturnToStandList : public DBElementCollection<CTowOffStandReturnToStandEntry>
{
public:
	CTowOffStandReturnToStandList(TowOffDataContentNonNull NullOrNonNull = ContentNull);
	~CTowOffStandReturnToStandList(void);

	//DBElementCollection
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
};



class INPUTAIRSIDE_API CTowOffPriorityEntry : public DBElement
{
public:
	CTowOffPriorityEntry(TowOffDataContentNonNull NullOrNonNull = ContentNull);
	~CTowOffPriorityEntry(void);

	void SaveData(int nParentID);
	void DeleteData();

	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	int GetStandID() const { return m_nStandID; }
	void SetStandID(int val) { m_nStandID = val; InitStandName(); }
	ALTObjectID GetStandName() const;
	CString GetStandNameString() const;
	CTowOffStandReturnToStandList& GetListTowOffStandReturnToStand();

	CTowOffStandReturnToStandEntry* FindReturnToStandItem(int nReturnToStandID);
	CTowOffStandReturnToStandEntry* AddReturnToStandItem(int nReturnToStandID);
	void AddReturnToStandItem(CTowOffStandReturnToStandEntry* pReturnToStandEntry);
	bool SetReturnToStandItemData(int nPreReturnToStandID,int nCurReturnToStandID);
	int GetReturnToStandCout();
	CTowOffStandReturnToStandEntry* GetReturnToStandItem(int nIndex);
	void RemoveReturnToStandItem(CTowOffStandReturnToStandEntry* pReturnToStandEntry);

protected:
	void InitStandName();
private:
	int								m_nStandID;
	ALTObjectID						m_StandName;
	CTowOffStandReturnToStandList	m_listTowOffStandReturnToStand;
};


class INPUTAIRSIDE_API CTowOffPriorityList : public DBElementCollection<CTowOffPriorityEntry>
{
public:
	CTowOffPriorityList(TowOffDataContentNonNull NullOrNonNull = ContentNull);
	~CTowOffPriorityList(void);

	//DBElementCollection
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
};

// Please Reserve, Benny, 2009-07-30
// 
// class INPUTAIRSIDE_API CTowOffDepStandPriorityEntry : public DBElement
// {
// public:
// 	CTowOffDepStandPriorityEntry(void);
// 	~CTowOffDepStandPriorityEntry(void);
// 
// 	//DBElement
// 	virtual void InitFromDBRecordset(CADORecordset& recordset);
// 	virtual void GetInsertSQL(int nParentID, CString& strSQL) const;
// 	virtual void GetUpdateSQL(CString& strSQL) const;
// 	virtual void GetDeleteSQL(CString& strSQL) const;
// 
// 	int GetStandID() const;
// 	void SetStandID(int val);
// 	ALTObjectID GetStandName() const;
// 	CString GetStandNameString() const;
// 
// protected:
// 	void InitStandName();
// private:
// 	int								m_nStandID;
// 	ALTObjectID						m_StandName;
// };
// 
// 
// class INPUTAIRSIDE_API CTowOffDepStandPriorityList : public DBElementCollection<CTowOffDepStandPriorityEntry>
// {
// public:
// 	CTowOffDepStandPriorityList(TowOffDataContentNonNull NullOrNonNull = ContentNull);
// 	~CTowOffDepStandPriorityList(void);
// 
// 	//DBElementCollection
// 	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
// };
class INPUTAIRSIDE_API PreferTowoffFlightType : public CDBData
{
public:
	PreferTowoffFlightType();
	~PreferTowoffFlightType();

	void SetFlightType(const FlightConstraint& fltType);
	const FlightConstraint& GetFlightType()const;
	const CString& GetFlightTypeString()const;
	void SetAirportDatabase(CAirportDatabase* pAirportDatabase);

	virtual CString GetTableName()const;
	virtual void GetDataFromDBRecordset(CADORecordset& recordset);
	virtual bool GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual bool GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;

private:
	CAirportDatabase*	m_pAirportDatabase;	
	FlightConstraint m_fltType;
	CString m_strFlightType;
};

class INPUTAIRSIDE_API PreferrdTowoffStandData : public CDBDataCollection<PreferTowoffFlightType>
{
public:
	PreferrdTowoffStandData();
	~PreferrdTowoffStandData();

	void UseTimeRange(bool bUse);
	bool WhetherUseTimeRange()const;

	void SetTimeRange(const ElapsedTime& eStart,const ElapsedTime& eEnd);
	const TimeRange& GetTimeRange()const;

	CString GetTimeRangeString()const;

	void SetOperationNum(int nNum);
	int GetOperationNum()const;

	void SetTimeMins(int nMins);
	int GetTimeMins()const;

	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	virtual void ReadData(int nParentID);
	virtual void SaveData(int nParentID);

private:
	int m_nID;
	bool  m_bTimeWindow;
	TimeRange m_timeRange;
	int m_nOperation;
	int m_nMins;
};

enum INTERMEDIATESTANDSTYPE
{
	INTERMEDIATESTANDS_STAND = 0,
	INTERMEDIATESTANDS_TEMPORARYPARKING 
};

class INPUTAIRSIDE_API CTowOffStandAssignmentData :
	public DBElement
{
public:
	CTowOffStandAssignmentData(TowOffDataContentNonNull NullOrNonNull = ContentNull);
	~CTowOffStandAssignmentData(void);

	const ALTObjectID& GetStandALTObjectID();
	CString GetParStandNameString();
	void SetFlightType(const AirsideFlightType& flightType);
	void SetFlightType(CString& strFlightType);
	const AirsideFlightType& GetFlightType()const;

	int GetDeplanementTime() const;// { return m_nDePlanementTime; }
	void SetDeplanementTime(int val) { m_nDePlanementTime = val; }

	int GetEnplanementTime() const;// { return m_nEnPlanementTime; }
	void SetEnplanementTime(int val) { m_nEnPlanementTime = val; }

	ElapsedTime getFltDeplaneTime();
	ElapsedTime getFltEnplaneTime();

	int GetTowOffCriteria() const;// { return m_nTowOffCriteria; }
	void SetTowOffCriteria(int val) { m_nTowOffCriteria = val; }

	int GetParkStandsID() const;// { return m_nParkStandID; }
	void SetParkStandsID(int val);
	void InitStandName();

	BOOL IsTow() const;
	void SetTow(BOOL val) { m_bTowoff = val; }

	BOOL GetRepositionOrFreeup() const;
	void SetRepositionOrFreeup(BOOL val);

	int GetTimeLeaveArrStand() const;
	void SetTimeLeaveArrStand(int val);

	BOOL GetBackToArrivalStand() const;
	void SetBackToArrivalStand(BOOL val);

	void SetPreferredTowoff(bool bPrefer);
	bool GetPreferredTowoff()const;

	PreferrdTowoffStandData* GetPreferredTowoffData();

	void ClearPrioirtyItem();
	int GetPriorityCount();
	CTowOffPriorityEntry* GetPriorityItem(int nIndex);
	bool RemovePriorityItem(int nStandID);

	enum TimeLeaveArrStandType
	{
		BeforeDeparture = 0,
		AfterArrival,
	};
	TimeLeaveArrStandType GetTimeLeaveArrStandType() const;
	void SetTimeLeaveArrStandType(TimeLeaveArrStandType val);

// 	CTowOffDepStandPriorityList& GetListDepStandPriority();

	CTowOffPriorityList& GetListPriorities();
	int FindPriorityItemIndex(int nStandID);
	CTowOffPriorityEntry* FindPriorityItem(int nStandID);
	CTowOffPriorityEntry* AddPriorityItem(int nStandID);
	void AddPriorityItem(CTowOffPriorityEntry* pPriorityEntry);
	bool SetPriorityItemData(int nPrePriorityID,int nCurPriorityID);  // update the item's Stand ID

	CTowOffStandReturnToStandEntry* FindReturnToStandItem(int nPriorityStandID, int nReturnToStandID);
	CTowOffStandReturnToStandEntry* AddReturnToStandItem(int nPriorityStandID, int nReturnToStandID);
	bool SetReturnToStandItemData(int nPriorityStandID, int nPreReturnToStandID,int nCurReturnToStandID);

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase);

	void SaveData(int nParentID);
	void DeleteData();

	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

protected:
	void UpdatePriorityToDataBaseType();
private:
	AirsideFlightType				m_FlightType;					// flight type
	CString							m_strFlightType;				// flight type string
	int								m_nDePlanementTime;				// deplanement time
	int								m_nEnPlanementTime;				// Enplanement time
	int								m_nParkStandID;					// Parked stand ID
	ALTObjectID						m_ParkingStandName;				// Parked stand name
	BOOL							m_bTowoff;						// "Yes" or "No"
	BOOL							m_bRepositionOrFreeup;			// "Reposition for Departure" or "Free up Stand"
	int								m_nTimeLeaveArrStand;			// time to leave arrival stand before departure
	TimeLeaveArrStandType			m_enumTimeLeaveArrStandType;	// "Before Departure" or "After Arrival"
// 	CTowOffDepStandPriorityList		m_listDepStandPriority;			// Tow off Departure Stand Priorities
	int								m_nTowOffCriteria;				// tow off criteria when "Free Up Stand" selected -- "(> mins TA):"
	BOOL							m_bBackToArrivalStand;			// back to arrival stand: Yes/No
 	CTowOffPriorityList				m_listPriorities;				// all the priorities

	bool								m_bPrefer; // whether preferred tow off
	PreferrdTowoffStandData  m_preferredTowoffData;
	CAirportDatabase*				m_pAirportDatabase;				//
};
