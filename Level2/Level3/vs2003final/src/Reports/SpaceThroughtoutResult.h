// SpaceThroughtoutResult.h: interface for the CSpaceThroughtputResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPACETHROUGHTOUTRESULT_H__728409C4_5DDB_4E29_AD4A_113663B112FA__INCLUDED_)
#define AFX_SPACETHROUGHTOUTRESULT_H__728409C4_5DDB_4E29_AD4A_113663B112FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\elaptime.h"
#include <vector>
typedef struct
{
	ElapsedTime m_startTime;
	ElapsedTime m_endTime;
	long m_lThroughtout;
} 
THROUGHTOUTITEM;

typedef std::vector<THROUGHTOUTITEM> THROUGHTOUTRESULT;

class CSpaceThroughtputResult  
{
private:
	THROUGHTOUTRESULT m_vResult;
public:
	CSpaceThroughtputResult();
	virtual ~CSpaceThroughtputResult();
	void InitItem( ElapsedTime _startTime, ElapsedTime _intervalTime,int _iIntervalCount );
	void IncreaseCount( ElapsedTime& _time ,int nIncNum = 1);
	
	int GetResultItemCount () const { return m_vResult.size(); };
	THROUGHTOUTITEM& operator [] ( int _iIndex );
	
		

};

#endif // !defined(AFX_SPACETHROUGHTOUTRESULT_H__728409C4_5DDB_4E29_AD4A_113663B112FA__INCLUDED_)
