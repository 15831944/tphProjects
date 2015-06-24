#include "stdafx.h"
#include "Observer.h"


void IObserveSubject::NotifyObservers( int nMsg )
{
	cpputil::TPtrVector<IObserver> observerlist = m_observerlist;
	ClearObserver();
	cpputil::TPtrVector<IObserver>::const_iterator itr = observerlist.begin();
	for(;itr!=observerlist.end();++itr)
	{
		IObserver* obs = *itr;
		obs->OnNotify(this,nMsg);
	}
}

void IObserveSubject::NotifyObservers( const ElapsedTime& t, int nMsg /*= 0*/ )
{
	m_notifyEvet.PlanNotify(this, t, nMsg);
}

void IObserveSubject::SubjectNotifyEvent::PlanNotify( IObserveSubject* pSubject,const ElapsedTime& t, int nMsg )
{
	m_pSubject = pSubject;
	setTime(t);
	addEvent();
}

void IObserveSubject::SubjectNotifyEvent::doProcess( CARCportEngine* pEngine )
{
	if(m_pSubject)
	{ 
		m_pSubject->NotifyObservers(m_nMsg);
	}
}
