#pragma once
#include "Common/Path2008.h"
#include "Common/Point2008.h"
#include "Common/containr.h"
#include "EVENT.H"
#include "PaxLandsideBehavior.h"

class LandsideQueueProcess;
class LandsidePaxQueueProcess;
class LandsideQueueSystemProcess;
class LandsideQueue;
class LandsidePaxSericeInSim;

#define QUEUE_OVERFLOW			0xff
class QueueElementList : public UnsortedContainer<PaxLandsideBehavior>
{
public:
	// default array size 8, subscript 0, delta (inc) 8
	QueueElementList () : UnsortedContainer<PaxLandsideBehavior> (8, 0, 8) {};
	virtual ~QueueElementList () {};

	int findElement (long p_id)
	{
		int count = getCount();
		PaxLandsideBehavior *element;
		for (int i = 0; i < count; i++)
		{
			element = getItem (i);
			if (element->GetPersonID() == p_id)
				return i;
		}
		return INT_MAX;
	}
};

class LandsideQueueBase
{
public:
	LandsideQueueBase(const CPath2008 path);
	~LandsideQueueBase();

	// Returns point at segment number index.
	CPoint2008 corner (int index) const { return m_qCorners.getPoint(index); };

	//Returns number of points at path segment.
	int cornerCount (void) const { return m_qCorners.getCount(); };

	//Returns the queue points as a path
	CPath2008* corner() { return &m_qCorners; }

protected:
	// The list of Points of waiting positions along the queue that define the shape of the queue.
	CPath2008 m_qCorners;
};

class LandsideQueue : public LandsideQueueBase
{
public:
	
	//Copy constructor : only its parent's copy constructor is invoked.
	LandsideQueue (const CPath2008& path) : LandsideQueueBase(path) {};
	virtual ~LandsideQueue () {};
	
	
	bool isHead(PaxLandsideBehavior* pLandsideBehavior);
	bool isTail(PaxLandsideBehavior* pLandsideBehavior);
	PaxLandsideBehavior* PeekWait(int idx);
	PaxLandsideBehavior* PeekApproach(int idx);
	int FindWait(PaxLandsideBehavior* pLandsideBehavior);
	int FindApproach(PaxLandsideBehavior* pLandsideBehavior);

	ARCVector3 GetNextStep(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys);
	bool StepItValid(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys);

	PaxLandsideBehavior* GetLandsideBehavior(int idx)const;
	PaxLandsideBehavior* GetHeadBehavior()const;
	PaxLandsideBehavior* GetTailBehavior()const;
	void RemoveApproach(PaxLandsideBehavior* pLandsideBehavior);
	void RemoveWait(PaxLandsideBehavior* pLandsideBehavior);
	void AddApproach(PaxLandsideBehavior* pLandsideBehavior);

	void StartQueueMove(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void WaitInQueue(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void AddToQueue(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void MoveToQueue(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void AdvanceQueue(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);

	int CalculateSegment(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys);
private:
	void UpdateApproach(const ElapsedTime& time,LandsideQueueSystemProcess* pLandsideQueueSys);
private:
	QueueElementList m_approachList;
	QueueElementList m_waitList;
};

class LandsidePaxQueueProcess
{
public:
	enum QueueState
	{
		sp_startMove,
		sp_movetoQueue,
		sp_addtoQueue,
		sp_waitinQueue,
		sp_advanceQueue,
		sp_leaveQueue,
	};
	LandsidePaxQueueProcess(PaxLandsideBehavior* pLandsideBehavior);
	~LandsidePaxQueueProcess();

	void StartMove(ElapsedTime p_time);
	void ProcessMove(ElapsedTime p_time);
	void Initialize(int state,LandsideQueueSystemProcess* pQueueProcess);

	PaxLandsideBehavior* GetlandsideBehavior();

	int GetSegment()const;
	void SetSegment(int nSegment) {m_nSegment = nSegment;}

	void GenerateQueueEvent(const ElapsedTime& time);

	void SetQueueState(QueueState state) {m_queueState = state;}
	QueueState GetQueueState()const {return m_queueState;}

	void SetStuck(bool bStuck) {m_bStuck = bStuck;}
	bool GetStuck()const {return m_bStuck;}

	void SetDes(bool bDes) {m_bDes = bDes;}
	bool GetDes()const {return m_bDes;}

private:
	bool m_bDes;
	int m_endState;
	int m_nSegment;
	QueueState m_queueState;
	bool m_bStuck;
	PaxLandsideBehavior* m_pLandsideBehavior;
	LandsideQueueSystemProcess* m_pLandsideQueueSys;
};

class LandsideQueueSystemProcess
{
public:
	LandsideQueueSystemProcess(LandsidePaxSericeInSim* pPaxServiceInsim);
	~LandsideQueueSystemProcess(void);

	//------------------------------------------------------------------------------------------------------
	//Summary:
	//		person enter landside queue process
	//Parameter:
	//		state: passenger leave queue state
	//-------------------------------------------------------------------------------------------------------
	void EnterQueueProcess(PaxLandsideBehavior* pLandsideBehavior,int state,const ElapsedTime& p_time);
	void LeaveQueueProcess(PaxLandsideBehavior* pLandsideBehavior,int state,const ElapsedTime& p_time);
	void InvokeWaitQueue(const ElapsedTime& time);
	//invoke head passenger to leave by bus station
	void LeaveQueue(const ElapsedTime& p_time);

	LandsidePaxQueueProcess* FindPaxQueueProcess(PaxLandsideBehavior* pLandsideBehavior);
	LandsidePaxQueueProcess* GetTailBehavior();
	LandsidePaxQueueProcess* GetNextBehavior(PaxLandsideBehavior* pLandsideBehavior);

	bool OnTailSegment(LandsidePaxQueueProcess* pPaxQueueProc);

	bool LastSegment(int nSegment)const;

	ARCVector3 GetTailPoint()const;

	LandsideQueue* GetlandsideQueue()const;

private:
	void Release(PaxLandsideBehavior* pLandsideBehavior);
	void Clear();
private:
	LandsideQueue* m_pQueue;
	LandsidePaxSericeInSim* m_pPaxServiceInSim;
	std::vector<LandsidePaxQueueProcess*> m_vPaxQueueProc;
};

class LandsideQueueEvent :public Event
{
public:
	LandsideQueueEvent(LandsidePaxQueueProcess *pQueueProc, ElapsedTime eTime);
	~LandsideQueueEvent(void);


	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;

	void SetPerson(PaxLandsideBehavior* pPerson);
	void SetState(LandsidePaxQueueProcess::QueueState emState);

protected:
	LandsidePaxQueueProcess *m_pQueueProcess;
	PaxLandsideBehavior* m_pPerson;
	LandsidePaxQueueProcess::QueueState m_emState;
};

class LandsideQueueWakeUpEvent : public Event
{
public:
	LandsideQueueWakeUpEvent(LandsideQueueSystemProcess* pQueueProcessSys,ElapsedTime eTime);
	~LandsideQueueWakeUpEvent();

	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;

private:
	LandsideQueueSystemProcess* m_queueProcessSys;
};