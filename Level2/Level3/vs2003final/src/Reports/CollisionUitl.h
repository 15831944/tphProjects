// CollisionUitl.h: interface for the CCollisionUitl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLISIONUITL_H__0C932F73_A61C_4306_8FD1_09BA58AE209C__INCLUDED_)
#define AFX_COLLISIONUITL_H__0C932F73_A61C_4306_8FD1_09BA58AE209C__INCLUDED_

//#include "CollisionPaxIdx.h"
#include "CollisonTimeSegment.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//typedef std::vector<CCollisionPaxIdx> PAXIDEXVECTOR;
typedef std::vector<CCollisonTimeSegment> TIMESEGMENT;

class CCollisionUitl  
{
private:
	TIMESEGMENT m_vTimeSegment;
public:
	CCollisionUitl();
	virtual ~CCollisionUitl();
	//void Init( long _IntervalNum );
	void Init( ElapsedTime& _startTime, ElapsedTime& _endTime , Terminal* _pTerm );
//	void AddPax( ElapsedTime& _entryTime, ElapsedTime& _endTime, int _iPaxIdx );
	void AddPersonTrackInfo( int _iPersonIdx, TRACKPAIR& _trackPair ,ElapsedTime& _startTime,ElapsedTime& _endTime );
//	const PAXIDEXVECTOR& GetPaxIndexVector()const { return m_vPaxIdx;};
	void CaculateAll( const CMobileElemConstraint& _paxType, ArctermFile& _file,float _threshold ,Pollygon& _region );
	void ClearAll();
};

#endif // !defined(AFX_COLLISIONUITL_H__0C932F73_A61C_4306_8FD1_09BA58AE209C__INCLUDED_)
