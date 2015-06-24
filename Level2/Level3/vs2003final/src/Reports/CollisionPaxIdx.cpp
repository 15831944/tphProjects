// CollisionPaxIdx.cpp: implementation of the CCollisionPaxIdx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CollisionPaxIdx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollisionPaxIdx::CCollisionPaxIdx()
{

}

CCollisionPaxIdx::~CCollisionPaxIdx()
{

}
void CCollisionPaxIdx::AddPaxIfLiveInTime( ElapsedTime& _entryTime, ElapsedTime& _exitTime,  int _iPaxIdx )
{
	if( _entryTime <= m_endTime && _exitTime >= m_startTime )
		m_vPaxIdx.push_back( _iPaxIdx );
}
