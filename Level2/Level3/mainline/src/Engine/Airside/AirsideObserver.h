#pragma once

class CAirsideMobileElement;
class ElapsedTime;
//observer pattern

#include <set>
/************************************************************************/
/* Observer                                                             */
/************************************************************************/
class CAirsideSubject;
class ENGINE_TRANSFER CAirsideObserver
{
public:
	virtual bool OberverSubject(CAirsideSubject* pSubject);
	virtual void Update(const ElapsedTime& tTime)=0;

	bool IsLoopWith(CAirsideSubject*pSubject);

protected:
	virtual void RemoveSubject(CAirsideSubject* pSubject);
	void ClearSubjects();

protected:
	typedef std::vector<CAirsideSubject*> SubjectListType;
	SubjectListType m_vSubjectList;
};

/************************************************************************/
/* subject                                                              */
/************************************************************************/
class ENGINE_TRANSFER CAirsideSubject
{
friend class CAirsideObserver;

public:
	virtual void NotifyObservers(const ElapsedTime& time);

protected:

	typedef std::vector<CAirsideObserver*> ObserverListType;
	ObserverListType m_vObserverList;	
	void AddObserver(CAirsideObserver* pObserver);
	void RemoveObserver(CAirsideObserver* pObserver);

};


