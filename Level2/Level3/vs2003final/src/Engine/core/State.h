#pragma once
#include "..\EVENT.H"
#include "..\SEvent.H"



class IStateMachine;
class IState
{
public:
	IState(){}	
	virtual ~IState(){ }

	virtual const char* getDesc()const=0;
	virtual IStateMachine* getMachine()const=0;

	virtual void Entry(CARCportEngine* pEngine){   }
	virtual void Execute(CARCportEngine* pEngine){   }
	virtual void Exit(CARCportEngine* pEngine){  }	

	void TransitToState(IState* nextState,const ElapsedTime& t);	
	void TransitToState(IState* nextState,CARCportEngine* pEngine);
protected:
};


class ChangeStatevent : public SEvent
{
public:
	ChangeStatevent(){ m_pMachine= NULL; m_pnextState = NULL; }
	void StateChange(IStateMachine* pM, IState* pS,const ElapsedTime&t);
protected:
	virtual void doProcess(CARCportEngine* pEngine);
	IState* m_pnextState;
	IStateMachine* m_pMachine;
	//For shut down event
	//It returns event's name
	virtual const char *getTypeName (void) const{ return "Change State"; }
	//It returns event type
	virtual int getEventType (void) const{ return MobStateEvent; }
};



class IStateMachine 
{
public:
	IStateMachine():m_curState(NULL){}
	virtual ~IStateMachine();
	void ExecuteCurState(CARCportEngine* pEngine);

	virtual void ChangeState(IState* nextState,CARCportEngine* pEngine);
	virtual void ChangeState(IState* nextState,const ElapsedTime& t);	
protected:
	IState* m_curState;
	virtual void OnStateChanged(){}
	ChangeStatevent m_changeEvent;
};

