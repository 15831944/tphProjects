#pragma once
#include "..\Common\elaptime.h"

class AirsideFlightInSim;
class AirsideBaggageTrainInSim;
class Person;


class AirsideFlightBaggageManager
{
public:
	AirsideFlightBaggageManager(void);
	~AirsideFlightBaggageManager(void);


public:
	void SetFlight(AirsideFlightInSim *pFight);

	bool LoadBagFromFlightAtStand(AirsideBaggageTrainInSim *pBaggageTrain, ElapsedTime time , ElapsedTime& eFinishedTime);

	void GenerateBaggageIfNeed(const ElapsedTime& time);
	void FlightArriveStand(bool bArrival, const ElapsedTime& time);

	bool IsGenerateGaggage()const;
protected:
	AirsideFlightInSim *m_pFlight;

	std::vector<AirsideBaggageTrainInSim *> m_vAssignedTrain;
	bool m_bBaggageGenerated;

	std::vector<Person *> m_vArrivalBag;


	//
	int m_nArrBagLoadedByCart;
};
