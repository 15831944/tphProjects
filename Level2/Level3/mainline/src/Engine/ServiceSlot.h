// ServiceSlot.h: interface for the ServiceSlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICESLOT_H__55EA503C_4BE4_4B60_8507_BD2288C288B7__INCLUDED_)
#define AFX_SERVICESLOT_H__55EA503C_4BE4_4B60_8507_BD2288C288B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "common\point.h"
#include "common\elaptime.h"
#include "common\path.h"
class ServiceSlot  
{
	PointIdxPair m_ptServicePoint;
	ElapsedTime m_tNecessaryTimeToForward;
	Person* m_pBaggageOnThisSlot;
public:
	ServiceSlot();
	virtual ~ServiceSlot();

	Person* GetBaggageOnSlot()const { return m_pBaggageOnThisSlot;	}
	PointIdxPair GetServicePoint()const { return m_ptServicePoint; }
	ElapsedTime GetTimeToForward()const { return m_tNecessaryTimeToForward;	}

	
	void SetBaggageOnSlot( Person* _pPerson ) { m_pBaggageOnThisSlot = _pPerson;}
	void SetServicePoint( const PointIdxPair& _ptServicePoint ){ m_ptServicePoint = _ptServicePoint; 	}
	void SetTimeToForward( const ElapsedTime& _tTimeToForward ){ m_tNecessaryTimeToForward = _tTimeToForward; }

	

};

#endif // !defined(AFX_SERVICESLOT_H__55EA503C_4BE4_4B60_8507_BD2288C288B7__INCLUDED_)
