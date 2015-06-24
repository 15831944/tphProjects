#pragma once
class Flight;

typedef std::vector<CString> FLIGHTIDLIST;
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
		void SetFilter(const CString& strFilter);
		CString GetFilter();
		bool containFlt(const char* pstrAirline ,const char* pstrFltID);//filter flight
		
	protected:
		void ParserFilter(const CString&  strFilter);
		bool Filter(const char* pstrData ,char* pstrFilter);

		CString m_strAirlineFilter;//Airline Filter, like AB*
		CString m_strADIDFilter; // Arrival ID, Or Departure ID Filter
	};


public:
	FlightGroup();
	~FlightGroup();
protected:

public:
	void setGroupName(CString strGroupName) { m_strGroupName = strGroupName;}
	CString getGroupName(){ return m_strGroupName;}


	void AddFilter(CFlightGroupFilter* groupFilter);
	void DelFilter(CFlightGroupFilter* groupFilter);
	void Clear();

	size_t GetFilterCount();
	CFlightGroupFilter* GetFilterByIndex(size_t nIndex);


	bool contain(const char* pstrAirline ,const char* pstrFltID);
	
	bool containFlt(Flight* pFlight, bool bArrival);
protected:
	CString m_strGroupName;
	std::vector<CFlightGroupFilter *> m_vFilter;
};

typedef std::vector<FlightGroup*> FLIGHTGROUPLIST;
///////////////////////////////////////////////////////////////////////////////////////////////
class COMMON_TRANSFER FlightManager		//Flight Group Manager
{
public:
	FlightManager();
	~FlightManager();
protected:

public:
	FLIGHTGROUPLIST* GetFlightGroups() { return &m_vGroups;}

	BOOL SaveGroupToFile();
	BOOL LoadData(const CString& _strDBPath);

	int findGroupItem(FlightGroup* pGroup);
	FlightGroup* getGroupItem(int nIdx){return m_vGroups[nIdx];} 

	void AddFlightGroup(FlightGroup* pGroup);
	void DelItem(FlightGroup* pGroup);
	BOOL CheckFlightGroup(FlightGroup* pGroup);
	int findFlightGroupItem(FlightGroup* pGroup);
private:
	FLIGHTGROUPLIST m_vGroups;
	CString  m_strDBPath;
};

FlightManager * _g_GetActiveFlightMan(CAirportDatabase * pAirportDB);
////////////////////////////////////////////////////////////////////////////////////////