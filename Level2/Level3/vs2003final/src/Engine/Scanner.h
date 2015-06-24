// Scanner.h: interface for the Scanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANNER_H__2238AEBA_FA27_41AE_BEA7_628CB4D7C9D9__INCLUDED_)
#define AFX_SCANNER_H__2238AEBA_FA27_41AE_BEA7_628CB4D7C9D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SimpleConveyor.h"

class Scanner : public CSimpleConveyor  
{
public:
	Scanner();
	virtual ~Scanner();
public:
	//	init processor data before engine run
	virtual void InitData( const MiscConveyorData * _pMiscData );

	// It is called by Person::processServerArrival
    // The different processor types serve clients in different ways.
	virtual void beginService (Person *aPerson, ElapsedTime curTime);

};

#endif // !defined(AFX_SCANNER_H__2238AEBA_FA27_41AE_BEA7_628CB4D7C9D9__INCLUDED_)
