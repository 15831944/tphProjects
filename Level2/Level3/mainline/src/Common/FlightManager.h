#pragma once
class Flight;
class FlightGroup;
class ElapsedTime;
#include "../Database/ADODatabase.h"
typedef std::vector<CString> FLIGHTIDLIST;
typedef std::vector<FlightGroup*> FLIGHTGROUPLIST;
//////////////////////////////////////////////////////////////////////////////////////////////
class COMMON_TRANSFER FlightGroup
{
	
public:
	class CFlightGroupFilter
	{
	public:
		CFlightGroupFilter();
		~CFlightGroupFilter();
	
	public:
		//format : AA*-123*

		void SetAirline(const CString& strAirline);
		const CString& GetAirline()const;

		void SetFlightID(const CString& strFlightID);
		const CString& GetFlightID()const;

		void SetDay(const CString& strDay);
		const CString& GetDay()const;

		const CString& GetStartTime()const;
		void SetStartTime(const CString& strStartTime);

		const CString& GetEndTime()const;
		void SetEndTime(const CString& strEndTime);

		const CString& GetMinTurnaround()const;
		void SetMinTurnaround(const CString& strMinTime);

		const CString& GetMaxTurnaround()const;
		void SetMaxTurnaround(const CString& strMaxTime);

		void SetFilter(const CString& strFilter);
		CString GetFilter();

		bool timeWithInTurnaround( const ElapsedTime& tTime );
		bool timeWithIn(const ElapsedTime& tTime);
		bool containFlt(const char* pstrAirline ,const char* pstrFltID,const char* pstrFltDay);//filter flight
		bool containFlt(const char* pstrAirline ,const char* pstrFltID);
		bool operator ==(const CFlightGroupFilter& fliter);
	protected:
		void ParserFilter(const CString&  strFilter);
		bool Filter(const char* pstrData ,char* pstrFilter);

		CString m_strAirlineFilter;//Airline Filter, like AB*
		CString m_strADIDFilter; // Arrival ID, Or Departure ID Filter
		CString m_strDay;//Arrival Day, Or Departure Day 
		CString m_startTime;
		CString m_endTime;
		CString m_minTurnaroundTime;
		CString m_maxTurnaroundTime;
	};


public:
	FlightGroup();
	FlightGroup(const FlightGroup& flightGroup);
	~FlightGroup();
protected:

public:
	void setGroupName(CString strGroupName) { m_strGroupName = strGroupName;}
	CString getGroupName(){ return m_strGroupName;}


	void AddFilter(CFlightGroupFilter* groupFilter);
	void DelFilter(CFlightGroupFilter* groupFilter);
	void Clear();

	size_t GetFilterCount()const;
	CFlightGroupFilter* GetFilterByIndex(size_t nIndex)const;

	bool contain(const char* pstrAirline ,const char* pstrFltID,const char* pstrFltDay,const ElapsedTime& tTime,const ElapsedTime& tTurnaroundTime);
	bool contain(const char* pstrAirline ,const char* pstrFltID,const char* pstrFltDay);
	bool contain(const char* pstrAirline ,const char* pstrFltID);
	
	//bool containFlt(Flight* pFlight, bool bArrival);
protected:
	CString m_strGroupName;
	std::vector<CFlightGroupFilter *> m_vFilter;
//the interface for DB
//////////////////////////////////////////////////////////////////////////
protected:
	int m_ID ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;
public:
	static void ReadDataFromDB(FLIGHTGROUPLIST& _dataSet,int _airportDBID,DATABASESOURCE_TYPE type ) ;
	static void WriteDataToDB(FLIGHTGROUPLIST& _dataSet,int _airportDBID,DATABASESOURCE_TYPE type ) ;
	static void DeleteDataFromDB(FLIGHTGROUPLIST& _dataSet,int _airportDBID,DATABASESOURCE_TYPE type ) ;
	static void DeleteAllData(int _airportDBID,DATABASESOURCE_TYPE type) ;
protected:
	static void WriteData(FlightGroup* _group,int _airportDBID,DATABASESOURCE_TYPE type ) ;
	static void UpdateData(FlightGroup* _group,int _airportDBID,DATABASESOURCE_TYPE type ) ;
	static void DeleteData(FlightGroup* _group,int _airportDBID,DATABASESOURCE_TYPE type ) ;
protected:
	static void ReadGroupFliter(FlightGroup* _group ,DATABASESOURCE_TYPE type ) ;
	static void WriteGroupFliter(FlightGroup* _group ,DATABASESOURCE_TYPE type ) ;

// new version database read and save 
public:
	bool loadDatabase(CADORecordset& aodRecordset,CAirportDatabase* pAirportDatabase);
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool updateDatabase(CAirportDatabase* pAirportDatabase);

	bool loadGroupFilter(CAirportDatabase* pAirportDatabase);
	bool saveGroupFilter(CAirportDatabase* pAirportDatabase);
	bool deleteGroupFilter(CAirportDatabase* pAirportDatabase);
};


///////////////////////////////////////////////////////////////////////////////////////////////
class COMMON_TRANSFER FlightManager		//Flight Group Manager
{
public:
	FlightManager();
	~FlightManager();
protected:

public:
	FLIGHTGROUPLIST* GetFlightGroups() { return &m_vGroups;}

	BOOL LoadData(const CString& _strDBPath);
	//current version read and save
	bool loadDatabase(CAirportDatabase* pAirportDatabase); //save the data to new version database
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);

	int findGroupItem(FlightGroup* pGroup);
	FlightGroup* getGroupItem(int nIdx){return m_vGroups[nIdx];} 

	void AddFlightGroup(FlightGroup* pGroup);
	void DelItem(FlightGroup* pGroup);
	BOOL CheckFlightGroup(FlightGroup* pGroup);
	int findFlightGroupItem(FlightGroup* pGroup);
private:
	FLIGHTGROUPLIST m_vGroups;
	FLIGHTGROUPLIST m_DelvGroup ;
	CString  m_strDBPath;
//the interface for DB
//////////////////////////////////////////////////////////////////////////
public:
	void ReadDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	void WriteDataToDB(int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	void DeleteAllData(int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
public:
    void ResetAllID() ;
	void Clear() ;
	void ClearDelGroup() ;
};

FlightManager * _g_GetActiveFlightMan(CAirportDatabase * pAirportDB);
////////////////////////////////////////////////////////////////////////////////////////