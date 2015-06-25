#pragma once
#include "../../Common/Path2008.h"
#include "../EngineDll.h"

class AirsideFlightInSim;
class IntersectionNodeInSim;
class RunwayInSim;
class ENGINE_TRANSFER HoldPositionInSim
{
public:
	HoldPositionInSim(int nDBID ,int nTaxiwayID, int nNodeID, DistanceUnit distOffNode);

public:
	int GetTaxiwayID() const { return m_nTaxiwayID; }
	void SetTaxiwayID(int val) { m_nTaxiwayID = val; }
	
	int GetNodeID() const { return m_nNodeID; }
	void SetNodeID(int val) { m_nNodeID = val; }

	DistanceUnit GetDistOffNode() const { return m_dDistOffNode; }
	void SetDistOffNode(DistanceUnit val) { m_dDistOffNode = val; }

	int GetID() const { return m_nID; }
	void SetID(int val) { m_nID = val; }
	
	IntersectionNodeInSim * GetNode() { return m_pNode; }
	void SetNode(IntersectionNodeInSim * pNode) { m_pNode = pNode; }

	RunwayInSim* GetRunwayInSim(){return m_pRunwayInSim;}
	void SetRunwayInSim(RunwayInSim *pRunwayInSim){m_pRunwayInSim = pRunwayInSim;}
public://logical
	//get the queue length of the runway
	int GetQueueLength()const{ return static_cast<int>(m_vInQueueFlights.size()); }
	double GetTakeoffQueueLength()const;
	AirsideFlightInSim *GetFlightInQueue(int nIndex);
	//the first flight in the queue of the hold short position
	AirsideFlightInSim* GetFirstFlightOfQueue()const;

	BOOL IsQueueFull()const;

	//queue	
	void AddFlightToQueue(AirsideFlightInSim* pFlight, bool bMoveToHead);
	void RemoveFlightInQueue(AirsideFlightInSim* pFlight);	

	void SetQueueFull(bool bful){ m_bQueueFull = bful; }

	bool IsWaveCrossHold() const { return m_bWaveCrossHold; }
	void SetWaveCrossHold(bool bWaveCross) { m_bWaveCrossHold = bWaveCross; }

	//int GetWaveCrossFlightCount() const { return m_nWaveCrossFlightCount; }
	void SetWaveCrossFlightCount(int val) { m_nWaveCrossFlightCount = val; }
	void DecreaseWaveCrossFlight(int nCount = 1){m_nWaveCrossFlightCount -= nCount;}
	bool HasFinishWaveCross() const{ return m_nWaveCrossFlightCount <= 0;}

	void AddEnrouteWaitList(AirsideFlightInSim* pFlight);
	void WakeupEnrouteWaitingFlight(const ElapsedTime& eTime);

	bool IsFlightInQueue(AirsideFlightInSim* pFlight)const;
	bool IsFlightInEnRouteWaitinglist(AirsideFlightInSim* pFlight)const;
protected:
	std::vector<AirsideFlightInSim*> m_vInQueueFlights;
	std::vector<AirsideFlightInSim*> m_vEnrouteFlights;


public:
	int m_nTaxiwayID;
	int m_nNodeID;
	bool m_bQueueFull;
	DistanceUnit m_dDistOffNode;

	int m_nID;//database id
	IntersectionNodeInSim *m_pNode;
	RunwayInSim *m_pRunwayInSim;
	
	//the hold is wave crossing hold.
	//has wave crossing setting
	bool m_bWaveCrossHold;

	//the count of flight that need to do wave cross
	int m_nWaveCrossFlightCount;

};
