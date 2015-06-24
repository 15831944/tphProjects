// CollisionPaxIdx.h: interface for the CCollisionPaxIdx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLISIONPAXIDX_H__8B53DE31_0141_4743_95BA_933E1FBB117C__INCLUDED_)
#define AFX_COLLISIONPAXIDX_H__8B53DE31_0141_4743_95BA_933E1FBB117C__INCLUDED_

#include "common\elaptime.h"
#include<vector>
typedef std::vector<int > PAXIDEX;
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCollisionPaxIdx  
{
private:
	PAXIDEX m_vPaxIdx;
	ElapsedTime m_startTime;
	ElapsedTime m_endTime;
public:
	void SetStartTime( ElapsedTime& _time ){ m_startTime = _time;};
	void SetEndTime( ElapsedTime& _time ){ m_endTime = _time;};
	void AddPaxIfLiveInTime( ElapsedTime& _entryTime, ElapsedTime& _exitTime, int _iPaxIdx );
	const PAXIDEX & GetPaxIdxVector() const { return m_vPaxIdx;};
	CCollisionPaxIdx();
	virtual ~CCollisionPaxIdx();

};

#endif // !defined(AFX_COLLISIONPAXIDX_H__8B53DE31_0141_4743_95BA_933E1FBB117C__INCLUDED_)
