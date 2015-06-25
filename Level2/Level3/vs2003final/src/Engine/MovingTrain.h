#pragma once
#include "mobile.h"

class CTrainLogEntry;
class Terminal;
class IntegratedStation;

class TrainCarInSim
{
public:
	TrainCarInSim(CCar* pCar);
	~TrainCarInSim();

	void AddPax(Person* pPerson, IntegratedStation *pDestStation );
	void DeletePax(Person* pPerson);
	int GetPaxCount()const;
	bool Valid()const;

	bool Exist(Person* pPerson)const;

	bool ClearThisStationPerson(IntegratedStation *thisStation);
private:
	CCar*  m_pCar;
	std::vector< std::pair< Person*, IntegratedStation *> > m_vPerson;
};
class MovingTrain :public MobileElement
{
public:
	MovingTrain(CTrainLogEntry& p_entry,Terminal* _pTerm);
	~MovingTrain(void);

protected:
	CTrainLogEntry m_TrainLogEntry;
	Terminal* m_pTerm;
	CSingleCarSchedule* m_pSingTrainFlow;
	CARFLOW m_TrainFlow;
	int m_nFlowIndex;
	std::vector<CViehcleMovePath> m_movePath;
	//	CAR m_cars;
	std::vector<TrainCarInSim*> m_vTrainCar;
public:
	virtual int move (ElapsedTime currentTime,bool bNoLog);
	TrainCarInSim* getCar(int nCarIndex);
	TrainCarInSim* getCar(Person* pPerson);
	void FlushLog();
private:
	bool GetSchedule( ElapsedTime _time );
	IntegratedStation* GetSourceStation(int nFlowIndex);
	void InitTrain();

	bool ClearThisStationPerson(IntegratedStation *thisStation);

};
