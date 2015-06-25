// StateTimePoint.h: interface for the CStateTimePoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATETIMEPOINT_H__5D2D5776_B47F_4DE8_A3DF_9F14A3D17F16__INCLUDED_)
#define AFX_STATETIMEPOINT_H__5D2D5776_B47F_4DE8_A3DF_9F14A3D17F16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TimePoint.h"

class CStateTimePoint : public CTimePoint  
{
public:
	CStateTimePoint();
	virtual ~CStateTimePoint();

	CStateTimePoint( const CStateTimePoint& _timePoint );

	void SetState( char _state );

	char GetState() const;

protected:
	char m_state;

};

#endif // !defined(AFX_STATETIMEPOINT_H__5D2D5776_B47F_4DE8_A3DF_9F14A3D17F16__INCLUDED_)
