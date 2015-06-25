// TimePoint.h: interface for the CTimePoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEPOINT_H__2EA460BF_BFA3_4510_B849_45DBAE990412__INCLUDED_)
#define AFX_TIMEPOINT_H__2EA460BF_BFA3_4510_B849_45DBAE990412__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "POINT.H"
#include "elaptime.h"

class CTimePoint : public Point  
{
public:
	CTimePoint();
	virtual ~CTimePoint();

	CTimePoint( const CTimePoint& _timePoint );

	void SetTime( ElapsedTime _time );

	ElapsedTime GetTime() const;

	void SetPoint( Point _pt );

protected:
	ElapsedTime m_time;
};

#endif // !defined(AFX_TIMEPOINT_H__2EA460BF_BFA3_4510_B849_45DBAE990412__INCLUDED_)
