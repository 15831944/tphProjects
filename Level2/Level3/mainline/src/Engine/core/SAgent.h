#pragma once
#include "..\SEvent.H"
#include "Observer.h"


class CARCportEngine;
class SAgent;

class SAgentActivator : public SEvent
{
public:
	SAgentActivator();
	void ActiveAgent(SAgent* pAgent, const ElapsedTime& t);
protected:
	SAgent* m_pAgent;
	void doProcess(CARCportEngine* pEngine);
	virtual int getEventType (void) const {	return AgentActiveEvent; }
	virtual const char *getTypeName (void) const{	return _T("Agent Active");	}
};

class SAgent : public IObserver, public IObserveSubject
{
public:
	static ElapsedTime curTime();
	virtual ~SAgent();
	virtual CString getTypeDesc()const;
public:
	virtual void OnActive(CARCportEngine*pEngine)=0;
	virtual void OnTerminate(CARCportEngine*pEngine)=0;

	virtual void OnNotify(IObserveSubject* subjuct, int nMsg);

public:
	void Activate(const ElapsedTime& nextT); //active agent next time
	void Terminate(const ElapsedTime& t); //terminate at specified time
	//void Terminate(); //terminate at current time
	void DeActive(); //make agent inactive remove all active event
protected:
	//message out box;	
	SAgent();	 
	SAgentActivator m_activator;
};


