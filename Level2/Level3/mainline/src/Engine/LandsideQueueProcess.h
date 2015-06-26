#pragma once
#include "Common/Path2008.h"
#include "Common/Point2008.h"
#include "Common/containr.h"
#include "EVENT.H"
#include "PaxLandsideBehavior.h"
#include "MOVEVENT.H"

class LandsideQueueProcess;
class LandsidePaxQueueProcess;
class LandsideQueueSystemProcess;
class LandsideQueue;
class LandsidePaxSericeInSim;

#define QUEUE_OVERFLOW			0xff
#define  QUEUE_APPROACHING		-1
#define  QUEUE_HEAD						0
#define  QUEUE_LAST_SEGMENT    (m_qCorners.getCount()-1)

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

	PaxLandsideBehavior *getHead (void) const
	{
		 return (getCount())? getItem (QUEUE_HEAD): NULL;
	}
    PaxLandsideBehavior *getTail (void) const
	{
		return (getCount())? getItem (getCount()-1): NULL;
	}

	PaxLandsideBehavior *removeHead (void)
	{
		PaxLandsideBehavior *obj = getItem (QUEUE_HEAD);
		removeItem (QUEUE_HEAD);
		return obj;
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
	LandsideQueue (const CPath2008& path) 
		: LandsideQueueBase(path) 
	{

	}
	virtual ~LandsideQueue () {}
	
	int hasWait ( LandsideQueueSystemProcess* pLandsideQueueSys);
	bool isHead(PaxLandsideBehavior* pLandsideBehavior);
	 int isTail (int index,LandsideQueueSystemProcess* pLandsideQueueSys);
	PaxLandsideBehavior* PeekWait(int idx,LandsideQueueSystemProcess* pLandsideQueueSys);
	PaxLandsideBehavior* PeekApproach(int idx);
	int FindApproach(PaxLandsideBehavior* pLandsideBehavior);

	int getNextIndex (int index,LandsideQueueSystemProcess* pLandsideQueueSys);
	int getTailIndex (LandsideQueueSystemProcess* pLandsideQueueSys);

	ARCVector3 GetNextStep(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys);
	bool StepItValid(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys);

	

	PaxLandsideBehavior* GetLandsideBehavior(int idx)const;
	PaxLandsideBehavior* GetHeadBehavior()const;
	PaxLandsideBehavior* GetTailBehavior()const;
	void RemoveWait(PaxLandsideBehavior* pLandsideBehavior);
	


	//Do the same logic as terminal queue
	void addToQueue (LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void arriveAtQueue(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void arriveAtWaitPoint (LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void leaveQueue (LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void relayAdvance (LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void continueAdvance (LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void approachQueue (LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	void updateApproaching(LandsideQueueSystemProcess* pLandsideQueueSys,const ElapsedTime& time);
	ARCVector3 getLocation(LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys);
	int onTailSegment (LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys) ;
	void getQueuePosition (LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys);
	void setPersonCurrentPosition( Person *aPerson,const ElapsedTime& arriveTime );
	int getSegment (LandsidePaxQueueProcess* pQueueProcess,LandsideQueueSystemProcess* pLandsideQueueSys) ;
	void generateAdvanceEvent (int p_ndx, ElapsedTime p_time,LandsideQueueSystemProcess* pLandsideQueueSys,DistanceUnit p_maxSpeed);
	void releaseNext (ElapsedTime p_time,LandsideQueueSystemProcess* pLandsideQueueSys);
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

	int getQueuePathSegmentIndex()const {return m_nSegment;}
	void setQueuePathSegmentIndex(int nSegment) {m_nSegment = nSegment;}

	void setCurrentIndex(int nIndex){m_nCurrentIndex = nIndex;}
	int getCurrentIndex()const {return m_nCurrentIndex;}

	void setNextIndex(int nIndex);
	int getNextIndex()const {return m_nNextIndex;}

	void updateIndex() {m_nCurrentIndex = m_nNextIndex;}

	void ArrivalQueue(bool bArrival) {m_bArrivalAtQueue = bArrival;}
	bool HasArrivalQueue()const {return m_bArrivalAtQueue;}

	void GenerateQueueEvent(const ElapsedTime& time);

	void SetQueueState(QueueState state) {m_queueState = state;}
	QueueState GetQueueState()const {return m_queueState;}

	void WriteQueuePaxLog(const ElapsedTime& p_time);


	void SetWaitTag(bool bWaitTag){m_bHasWaitTag = bWaitTag;}
	bool GetWaitTag()const {return m_bHasWaitTag;}
	void decQueuePathSegmentIndex(){m_nSegment--;}

	void moveThroughQueue(const ElapsedTime& time);
	void arriveAtQueue (const ElapsedTime& time);
	void notifyQueueAdvance(const ElapsedTime& time);
	void leaveQueue (const ElapsedTime& time );
	void processPerson (const ElapsedTime& time );

	DistanceUnit GetNormalSpeed()const {return m_dSpeed;}
private:
	int m_endState;
	int m_nSegment;
	int m_nCurrentIndex;
	int m_nNextIndex;
	DistanceUnit m_dSpeed;
	QueueState m_queueState;
	bool  m_bHasWaitTag;
	bool  m_bArrivalAtQueue;
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
	bool ExsitPaxQueueProcess(LandsidePaxQueueProcess* pQueueProcess);

	LandsidePaxQueueProcess* GetTailQueueProcess();

	LandsideQueue* GetlandsideQueue()const;

	void FlushOnVehiclePaxLog(CARCportEngine* pEngine,const ElapsedTime& t);

private:
	void Release(PaxLandsideBehavior* pLandsideBehavior);
	void Clear();
private:
	LandsideQueue* m_pQueue;
	LandsidePaxSericeInSim* m_pPaxServiceInSim;
	std::vector<LandsidePaxQueueProcess*> m_vPaxQueueProc;
};

class LandsideQueueEvent :public MobileElementMovementEvent
{
public:
	LandsideQueueEvent(LandsidePaxQueueProcess *pQueueProc);
	~LandsideQueueEvent(void);


	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;


	void SetLandsideQueueSystem(LandsideQueueSystemProcess* pLandsideQueueSys);

protected:
	LandsidePaxQueueProcess *m_pQueueProcess;
	LandsideQueueSystemProcess* m_pLandsideQueueSys;
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