#pragma once
#include "..\..\Common\CPPUtil.h"
#include "..\SEvent.H"



class ElapsedTime;
class IObserver;
class IObserveSubject
{
public:
	void NotifyObservers(int nMsg=0);
	void AddObserver(IObserver* obs){  m_observerlist.add(obs); }
	void RemoveObserver(IObserver* obs){ m_observerlist.remove(obs); }
	void ClearObserver(){ m_observerlist.clear(); }
	void NotifyObservers(const ElapsedTime& t, int nMsg = 0);
protected:
	class SubjectNotifyEvent: public SEvent
	{
	public:
		void PlanNotify(IObserveSubject* pSubject,const ElapsedTime& t, int nMsg);
		
	protected:
		IObserveSubject* m_pSubject;
		int m_nMsg;
		void doProcess(CARCportEngine* pEngine);
		virtual const char *getTypeName (void) const { return _T("Notify State Change"); }
		//It returns event type
		virtual int getEventType (void) const { return NormalEvent; }
	};

	cpputil::TPtrVector<IObserver> m_observerlist;
	SubjectNotifyEvent m_notifyEvet;
};


class IObserver
{
public:
	virtual void OnNotify(IObserveSubject* subjuct, int nMsg)=0;
};


