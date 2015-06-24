#pragma once
class AirsideFlightInSim;
class RunwayExitInSim;

/************************************************************************/
/*                                                                      */
/************************************************************************/
class ENGINE_TRANSFER InQueueItem
{
public:
	InQueueItem(AirsideFlightInSim* pFlight, const ElapsedTime& enterTime);

	AirsideFlightInSim* GetFlight()const{ return m_pFlight; }
	ElapsedTime GetEnterTime()const{ return m_enterTime; }
protected:
	AirsideFlightInSim* m_pFlight;
	ElapsedTime m_enterTime;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
class ENGINE_TRANSFER QueueToTakeoffPos
{
public:
	QueueToTakeoffPos(RunwayExitInSim* pTakeoffPos){ m_pTakeoffPos = pTakeoffPos; m_pHeadFlight = NULL;}	
	RunwayExitInSim* GetTakeoffPos()const{ return m_pTakeoffPos; }
	int GetItemCount()const{ return (int)m_vInQueueItems.size();}
	InQueueItem& ItemAt(int idx){ return m_vInQueueItems[idx]; }
	const InQueueItem& ItemAt(int idx)const{ return m_vInQueueItems[idx]; }

	void AddInQueueFlight(AirsideFlightInSim* pFlight, const ElapsedTime& enterTime);
	ElapsedTime GetAverageQueueTime(const ElapsedTime& curTime)const;
	bool IsFlightInQueue(AirsideFlightInSim* pFlight)const;
	void RemoveInQueueFlight(AirsideFlightInSim* pFlight);

	void SetHeadFlight(AirsideFlightInSim* pFlight){ m_pHeadFlight = pFlight; }
	AirsideFlightInSim* GetHeadFlight(const ElapsedTime& tCurTime);
	ElapsedTime GetFlightEnterTime(AirsideFlightInSim* pFlight);
	ElapsedTime GetQueueTime()const; //return  the earliest flight in queue time
	int GetAheadFlightInQ(ElapsedTime tEnterTime) const;
	
protected:
	RunwayExitInSim* m_pTakeoffPos;
	AirsideFlightInSim* m_pHeadFlight;
	std::vector<InQueueItem> m_vInQueueItems;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
class ENGINE_TRANSFER QueueToTakeoffPosList
{
public:
	
	QueueToTakeoffPos* GetAddQueue(RunwayExitInSim* pTakeoffPos);
	~QueueToTakeoffPosList();

	int GetAllFlightCount()const;
	ElapsedTime GetAvgInQueueTime(const ElapsedTime& tTime)const;
	void RemoveFlight(AirsideFlightInSim*pFlight);

	int GetQueueCount()const{ return (int)m_vQueues.size();}
	const QueueToTakeoffPos* GetQueueByIdx(int nIdx) const { return m_vQueues[nIdx]; }

protected:
	std::vector<QueueToTakeoffPos*> m_vQueues;
};