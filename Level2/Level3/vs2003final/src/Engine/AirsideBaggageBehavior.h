#pragma once
#include "airsidemobelementbasebehavior.h"

class AirsideBaggageCartInSim;

class AirsideBaggageBehavior :
	public AirsideMobElementBaseBehavior
{
public:
	AirsideBaggageBehavior(Person *pBaggage);
	AirsideBaggageBehavior(Person* pBaggage ,int curentstate);

	~AirsideBaggageBehavior(void);

	inline CPoint2008 GetOffSetInBus()const {return _offSetInBus ;}
	void SetOffsetInBus(const CPoint2008& pos ){ _offSetInBus = pos; }


public:
	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);

	void SetFollowerDestination(const CPoint2008& _ptCurrent, const CPoint2008& _ptDestination, float* _pRotation );

	//It writes next track to ElementTrack log.
	// in:
	// _bBackup: if the log is backwards.
	//virtual void writeLog (ElapsedTime time, bool _bBackup, bool bOffset = true );

	void MoveToCartFromPusher(AirsideBaggageCartInSim *pBagCart, ElapsedTime& eTime);

protected:
	CPoint2008 _offSetInBus ;


protected:

};
