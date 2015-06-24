// Splitter.h: interface for the Splitter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITTER_H__08577EB7_64AD_4809_86A7_267720CFBD71__INCLUDED_)
#define AFX_SPLITTER_H__08577EB7_64AD_4809_86A7_267720CFBD71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "SimpleConveyor.h"

class Splitter : public CSimpleConveyor  
{
public:
	Splitter();
	virtual ~Splitter();
public:
	// It is called by Person::processServerArrival
    // The different processor types serve clients in different ways.
	virtual void beginService (Person *_pPerson, ElapsedTime _curTime);

private:
	// release the head person
	virtual bool releaseHeadPerson( Person* person, ElapsedTime time );

};

#endif // !defined(AFX_SPLITTER_H__08577EB7_64AD_4809_86A7_267720CFBD71__INCLUDED_)
