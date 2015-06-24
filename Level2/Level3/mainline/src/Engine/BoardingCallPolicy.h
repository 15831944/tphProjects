#pragma once
#include <vector>
#include <deque>
class Flight ;
class CAirsidePaxBusInSim ;
class ElapsedTime ;
class CARCportEngine;
class Person;


class CPolicyData
{
private:
	Flight* m_flight ;
	bool	m_bArrFlight;
	BOOL m_isBusArrived ;
	BOOL m_isFlightArrived ;
	std::vector<CAirsidePaxBusInSim*> m_Pbus ;
public:
	CPolicyData(bool bArrial) ;
	CPolicyData(Flight* _fli,bool bArrial) ;
	CPolicyData(const CPolicyData& _policydata) ;
	~CPolicyData() ;
public:
	void SetFlight(Flight* _fli) ;
	Flight* GetFlight() ;

	BOOL GetIsBusArrived() ;

	void SetIsFlightArrived(BOOL _res) ;
	BOOL GetIsFlightArrived() ;

	bool arrivalFlight()const;
	void arrivalFlight(bool bArrival);

	void SetArrivedBus(CAirsidePaxBusInSim* _bus) ;
	void RemoveArriveBus(CAirsidePaxBusInSim* _bus) ;
	std::vector<CAirsidePaxBusInSim*>* GetArrivedBus() ;
	CAirsidePaxBusInSim* GetCanServerBus(const Person* pPerson) ;
};
class CPolicyDataContainer
{
private:
	std::vector<CPolicyData*> m_Policy ;
public:
	CPolicyData* FindPolicyData(Flight* _Fli,bool bArrial) ;
public:
	virtual ~CPolicyDataContainer() ;
	CPolicyDataContainer() ;
};
class CBoardingCallPolicy : public CPolicyDataContainer
{

public:
	CBoardingCallPolicy();
	virtual ~CBoardingCallPolicy(void);
public:
	virtual void NoticeBusHasArrived(Flight* fli,bool bArrival,ElapsedTime time,CAirsidePaxBusInSim* p_bus, CARCportEngine*_pEngine) ;
	virtual void NoticeFlightArrived(Flight* fli,bool bArrival,ElapsedTime time, CARCportEngine*_pEngine) ;

	virtual void NoticeBusHasLeaved(Flight* fli,bool bArrival,CAirsidePaxBusInSim* p_bus) ;
	virtual void NoticeFlightHasLeaved(Flight* fli,bool bArrival) ;
public:
	BOOL FlightIsArrived(Flight* fli,bool bArrival) ;
	BOOL BusIsArrived(Flight* fli,bool bArrival) ;
	BOOL CanTakeOnTheFlight(Flight* fli, bool bArrival,CARCportEngine*_pEngine) ;
	BOOL HaveBusServer(Flight* fli, bool bArrival,CARCportEngine*_pEngine) ;
private:
	void HandleBusArrivalForDep(CPolicyData* _policydata,ElapsedTime curTime,CAirsidePaxBusInSim* _bus , CARCportEngine*_pEngine);
	BOOL HandleFlightArrivalForDep(CPolicyData* _policydata,ElapsedTime curTime, CARCportEngine*_pEngine);
};
class CPassengerTakeOffCallPolicy : public CPolicyDataContainer
{
public:
	class FlightMobileElmentInfo
	{
	public:
		FlightMobileElmentInfo(Flight* pFlight)
			:m_pFlight(pFlight)
		{

		}
		~FlightMobileElmentInfo()
		{
			
		}

	public:
		bool operator == (const FlightMobileElmentInfo& flightInfo)
		{
			if (m_pFlight == flightInfo.m_pFlight)
			{
				return true;
			}
			return false;
		}

		Person* front()
		{
			return m_vPerson.front();
		}

		void pop_front()
		{
			m_vPerson.pop_front();
		}

		void push_back(Person* pPerson)
		{
			m_vPerson.push_back(pPerson);
		}

		bool empty()const
		{
			return m_vPerson.empty();
		}

		const Flight* getFlight()const {return m_pFlight;}
	private:
		Flight* m_pFlight;
		std::deque<Person*>m_vPerson;//all person in flight
	};

public:
	CPassengerTakeOffCallPolicy() ;
	virtual ~CPassengerTakeOffCallPolicy() ;
    void NoticeBusHasArrived(Flight* fli,bool bArrival,ElapsedTime time,CAirsidePaxBusInSim* p_bus, CARCportEngine*_pEngine) ;
	void NoticeFlightArrived(Flight* fli,bool bArrival,ElapsedTime time, CARCportEngine*_pEngine) ;
	void NoticeBusHasLeaved(Flight* fli,bool bArrival,CAirsidePaxBusInSim* p_bus) ;
	void NoticeFlightHasLeaved(Flight* fli,bool bArrival) ;
protected:
	void HandBusHasArrived(CPolicyData* _policydata , ElapsedTime curTime,CAirsidePaxBusInSim* _arrivalBus, CARCportEngine*_pEngine);
	void HandleFlightArrived(CPolicyData* _policydata , ElapsedTime curTime, CARCportEngine*_pEngine);
	bool AlreadyGeneratePassengerforFlight(const Flight* pFlight)const;
	int GetIndexOfFlightInfo(const Flight* pFlight)const;

private:
	std::vector<FlightMobileElmentInfo*> m_vFlightPaxList;
};
