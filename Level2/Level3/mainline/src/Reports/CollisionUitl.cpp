// CollisionUitl.cpp: implementation of the CCollisionUitl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CollisionUitl.h"
#include "common\ProgressBar.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollisionUitl::CCollisionUitl()
{

}

CCollisionUitl::~CCollisionUitl()
{

}
void CCollisionUitl::Init( ElapsedTime& _startTime, ElapsedTime& _endTime , Terminal* _pTerm )
{
	m_vTimeSegment.clear();
	ElapsedTime tTime = _startTime;
	int iCount = ( _endTime.asSeconds() - _startTime.asSeconds() ) / 3600;
	if ( (long)( _endTime.asSeconds() - _startTime.asSeconds() ) % 3600 == 0 )
	{		
		for( int i=0;i< iCount ; ++i )
		{
			CCollisonTimeSegment temp;
			temp.Init( tTime , tTime + 3600l , _pTerm );
			tTime += 3600l;
			m_vTimeSegment.push_back( temp );
		}
	}
	else
	{
		if ( iCount == 0 )
		{
			CCollisonTimeSegment temp;
			temp.Init( tTime , _endTime , _pTerm );			
			m_vTimeSegment.push_back( temp );
		}
		else
		{
			CCollisonTimeSegment temp;
			for( int i=0;i< iCount-1 ; ++i )
			{				
				CCollisonTimeSegment temp;
				temp.Init( tTime , tTime + 3600l , _pTerm );
				tTime += 3600l;
				m_vTimeSegment.push_back( temp );
			}
			
			temp.Init( tTime , _endTime , _pTerm );	
			m_vTimeSegment.push_back( temp );
		}
	}

	
}
void CCollisionUitl::AddPersonTrackInfo( int _iPersonIdx, TRACKPAIR& _trackPair ,ElapsedTime& _startTime,ElapsedTime& _endTime )
{
	TIMESEGMENT::iterator iter = m_vTimeSegment.begin();
	TIMESEGMENT::iterator iterEnd = m_vTimeSegment.end();
	for( ; iter != iterEnd ; ++iter)
	{
		//iter->AddPaxIfLiveInTime( _entryTime, _endTime, _iPaxIdx );
		iter->AddPersonTrackInfo( _iPersonIdx, _trackPair, _startTime,_endTime );
	}

}
void CCollisionUitl::CaculateAll( const CMobileElemConstraint& _paxType, ArctermFile& _file,float _threshold ,Pollygon& _region )
{
	CProgressBar bar1( "Generating Collision Log Report", 100, m_vTimeSegment.size(), TRUE ,1 );
	TIMESEGMENT::iterator iter = m_vTimeSegment.begin();
	TIMESEGMENT::iterator iterEnd = m_vTimeSegment.end();
	for( ; iter != iterEnd ; ++iter)
	{
		bar1.StepIt();
		iter->CaculateAll( _paxType,  _file, _threshold, _region );
	}
}
void CCollisionUitl::ClearAll()
{
	TIMESEGMENT::iterator iter = m_vTimeSegment.begin();
	TIMESEGMENT::iterator iterEnd = m_vTimeSegment.end();
	for( ; iter != iterEnd ; ++iter)
	{
		iter->ClearAll();
	}
	//m_vTimeSegment.clear();
}