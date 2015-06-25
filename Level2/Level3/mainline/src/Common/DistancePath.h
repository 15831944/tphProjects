// DistancePath.h: interface for the CDistancePath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISTANCEPATH_H__3BACD880_0E4E_467A_8EA4_BA71FA8924EF__INCLUDED_)
#define AFX_DISTANCEPATH_H__3BACD880_0E4E_467A_8EA4_BA71FA8924EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PATH.H"
#include <vector>
#include "StateTimePoint.h"
#include "elaptime.h"

class CDistancePath : public Path  
{
public:
	CDistancePath();
	virtual ~CDistancePath();

	void SetSpeed( DistanceUnit _dAccelerateSpeed, DistanceUnit _dDecelerateSpeed, DistanceUnit _dOrigSpeed );

	void Calculate();

	std::vector<CStateTimePoint>& GetResultList();


protected:

	// input
	DistanceUnit m_dAccelerateSpeed;
	DistanceUnit m_dDecelerateSpeed;
	DistanceUnit m_dOrigSpeed;

	// cal res
	DistanceUnit m_dTotalLength;
	DistanceUnit m_dAccLength;
	DistanceUnit m_dDeccLength;
	DistanceUnit m_dActualSpeed;
	ElapsedTime m_AccPtTime;
	ElapsedTime m_DecPtTime;
	ElapsedTime m_TotalTime;

	DistanceUnit* m_pDistance;
	std::vector<CStateTimePoint> m_resultList;	// the place keep the result points and their time, 
											// include ac/de points.


protected:

	virtual void Initialization();
	
	// Cal the overall length.
	DistanceUnit GetOverallLength();

	// Give length, Cal the Point
	bool GetPointOnPath( DistanceUnit _dLength, Point& _point );

	// Give V and Acc give length
	DistanceUnit GetAccLength( DistanceUnit _acc, DistanceUnit _v );

	// Give index find time.
	ElapsedTime GetHitTime( int _nIdx );

	// Give V and Acc give length
	ElapsedTime GetHitTime( DistanceUnit _dLength );

	};

#endif // !defined(AFX_DISTANCEPATH_H__3BACD880_0E4E_467A_8EA4_BA71FA8924EF__INCLUDED_)
