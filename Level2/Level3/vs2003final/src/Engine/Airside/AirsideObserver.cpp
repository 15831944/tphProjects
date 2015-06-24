#include "StdAfx.h"
#include "../EngineDll.h"
#include "AirsideObserver.h"

#include <algorithm>
bool CAirsideObserver::OberverSubject( CAirsideSubject* pSubject )
{
	if( !IsLoopWith(pSubject) )
	{
		SubjectListType::iterator itr = std::find(m_vSubjectList.begin(),m_vSubjectList.end(),pSubject);
		if(itr==m_vSubjectList.end())
			m_vSubjectList.push_back(pSubject);
		pSubject->AddObserver(this);
		return true;
	}
	return false;
}

void CAirsideObserver::RemoveSubject( CAirsideSubject* pSubject )
{
	pSubject->RemoveObserver(this);
	SubjectListType::iterator itr = std::find(m_vSubjectList.begin(),m_vSubjectList.end(),pSubject);
	if(itr!=m_vSubjectList.end())
		m_vSubjectList.erase(itr);
}

void CAirsideObserver::ClearSubjects()
{
	for(SubjectListType::iterator itr = m_vSubjectList.begin();itr!=m_vSubjectList.end();itr++)
	{
		(*itr)->RemoveObserver(this);
	}
	m_vSubjectList.clear();
}

bool CAirsideObserver::IsLoopWith( CAirsideSubject*pSubject )
{
	if( pSubject->m_vObserverList.size() ==0 )
		return false;


	return false;
}
//////////////////////////////////////////////////////////////////////////
void CAirsideSubject::RemoveObserver( CAirsideObserver* pObserver )
{
	ObserverListType::iterator itr = std::find(m_vObserverList.begin(),m_vObserverList.end(),pObserver);
	if(itr!=m_vObserverList.end())
		m_vObserverList.erase(itr);
}

void CAirsideSubject::AddObserver( CAirsideObserver* pObserver )
{
	ObserverListType::iterator itr = std::find(m_vObserverList.begin(),m_vObserverList.end(),pObserver);
	if(itr==m_vObserverList.end())
		m_vObserverList.push_back(pObserver);	
}

void CAirsideSubject::NotifyObservers( const ElapsedTime& time )
{
	ObserverListType vObservers = m_vObserverList;
	for(ObserverListType::iterator itr= vObservers.begin();itr!= vObservers.end();itr++)
	{
		(*itr)->Update(time);
	}
	m_vObserverList.clear();
}


