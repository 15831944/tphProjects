#pragma once
class AirsideFlightInSim;
class CAirsidePaxBusInSim;
class ElapsedTime;
class AirsidePassengerBusContext;
class Person;

class AirsidePassengerBusStrategy
{
public:
	AirsidePassengerBusStrategy(AirsideFlightInSim* pAirsideFlightInSim);
	virtual ~AirsidePassengerBusStrategy(void);

	virtual void PassengerBusArrive(const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus,AirsidePassengerBusContext* pPaxBusContext) = 0;
	virtual void FlightArriveStand(const ElapsedTime& time,AirsidePassengerBusContext* pPaxBusContext) = 0;
	virtual bool Arrival()const = 0;

protected:
	AirsideFlightInSim* m_pAirsideFlightInSim;
};

class AirsideArrPassengerBusStrategy : public AirsidePassengerBusStrategy
{
public:
	AirsideArrPassengerBusStrategy(AirsideFlightInSim* pAirsideFlightInSim);
	virtual ~AirsideArrPassengerBusStrategy();

	virtual void PassengerBusArrive(const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus,AirsidePassengerBusContext* pPaxBusContext);
	virtual void FlightArriveStand(const ElapsedTime& time,AirsidePassengerBusContext* pPaxBusContext);

	virtual bool Arrival()const;
};

class AirsideDepPassengerBusStrategy : public AirsidePassengerBusStrategy
{
public:
	AirsideDepPassengerBusStrategy(AirsideFlightInSim* pAirsideFlightInSim);
	virtual ~AirsideDepPassengerBusStrategy();

	virtual void PassengerBusArrive(const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus,AirsidePassengerBusContext* pPaxBusContext);
	virtual void FlightArriveStand(const ElapsedTime& time,AirsidePassengerBusContext* pPaxBusContext);
	virtual bool Arrival()const;

private:
	virtual int WakeupHoldPassenger(const ElapsedTime& time ,int nPaxCount);
};

class AirsidePassengerBusContext
{
public:
	AirsidePassengerBusContext();
	~AirsidePassengerBusContext();

	virtual void PassengerBusArrive(const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus);
	virtual void PassengerBusLeave(CAirsidePaxBusInSim* pPaxBus);
	virtual void FlightArriveStand(const ElapsedTime& time);
	virtual void FlightLeaveStand();

	CAirsidePaxBusInSim* GetAvailablePassengerBus(Person* pPerson);
	void SetPaxBusStrategy(AirsidePassengerBusStrategy* pPaxBusStrategy);

	int GetPaxBusCount()const;
	CAirsidePaxBusInSim* GetPaxBus(int idx);

	void SetPaxListInFlight(const std::vector<Person*>& vPaxList);
	void AddPax(Person* pPerson);
	bool HaveGenerateMobileElment()const;
	void GenerateMobileElement();

	void ProcessPassengerTakeonBus(const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus);
private:
	bool m_bGenerateMobile;
	std::vector<Person*> m_vPerson;
	AirsidePassengerBusStrategy* m_pPaxBusStrategy;
	std::vector<CAirsidePaxBusInSim*> m_vArrPaxBusList;
	std::vector<CAirsidePaxBusInSim*> m_vDepPaxBusList;
};