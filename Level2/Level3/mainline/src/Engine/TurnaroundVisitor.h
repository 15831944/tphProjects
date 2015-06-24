// TurnaroundVisitor.h: interface for the TurnaroundVisitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TURNAROUNDVISITOR_H__3B1422E3_0EDE_4395_9454_E6B9E5E1859D__INCLUDED_)
#define AFX_TURNAROUNDVISITOR_H__3B1422E3_0EDE_4395_9454_E6B9E5E1859D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VISITOR.H"
#include "common\containr.h"

class CARCportEngine;

class TurnaroundVisitor : public PaxVisitor  
{
public:
	TurnaroundVisitor(MobLogEntry& _entry, CARCportEngine *_pEngine );
	virtual ~TurnaroundVisitor();

protected:
	// Gate index.
    int arrivingGate;
	
    // Gate index.
    int departingGate;
	
    // Passenger type.
    CMobileElemConstraint arrivingType;
	
    // Passenger type.
    CMobileElemConstraint departingType;
	
    ElapsedTime departureTime;
public:
	virtual const CMobileElemConstraint &getArrivalType(void) const {return arrivingType;}
};

#endif // !defined(AFX_TURNAROUNDVISITOR_H__3B1422E3_0EDE_4395_9454_E6B9E5E1859D__INCLUDED_)
