// ViehcleMovePath.cpp: implementation of the CViehcleMovePath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ViehcleMovePath.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViehcleMovePath::CViehcleMovePath()
{

}

CViehcleMovePath::~CViehcleMovePath()
{

}



CViehcleMovePath::CViehcleMovePath( const CViehcleMovePath& _movePath )
{
	m_arrTime = _movePath.m_arrTime;
	m_depTime = _movePath.m_depTime;
	m_vectStateTimePoint = _movePath.m_vectStateTimePoint;
}

int CViehcleMovePath::GetCount()
{
	return m_vectStateTimePoint.size();
}

void CViehcleMovePath::AddItem( CStateTimePoint& _item )
{
	m_vectStateTimePoint.push_back( _item );
}

void CViehcleMovePath::AddItem( Point _pt, ElapsedTime _time, char _state )
{
	CStateTimePoint timePoint;

	_time += m_depTime;
	timePoint.SetPoint( _pt );
	timePoint.SetTime( _time );
	timePoint.SetState( _state );
	m_vectStateTimePoint.push_back( timePoint );
}

CStateTimePoint CViehcleMovePath::GetItem( int _nIdx )
{
	return m_vectStateTimePoint[_nIdx];
}


void CViehcleMovePath::SetArrTime( ElapsedTime _time )
{
	m_arrTime = _time;	
}


ElapsedTime CViehcleMovePath::GetArrTime()
{
	return m_arrTime;
}


void CViehcleMovePath::DoTimeOffset( ElapsedTime _deltaTime )
{
	m_arrTime += _deltaTime;
	m_depTime += _deltaTime;

	int nCount = m_vectStateTimePoint.size();
	for( int i=0; i<nCount; i++ )
	{
		ElapsedTime time = m_vectStateTimePoint[i].GetTime();
		time += _deltaTime;
		m_vectStateTimePoint[i].SetTime( time );
	}
}


void CViehcleMovePath::WriteData( ArctermFile& _file ) const
{
	_file.writeTime( m_arrTime, TRUE );
	_file.writeTime( m_depTime, TRUE );
	_file.writeLine();

	int nCount = m_vectStateTimePoint.size();
	_file.writeInt( nCount );
	_file.writeLine();
	for( int i=0; i<nCount; i++ )
	{
		_file.writePoint( m_vectStateTimePoint[i] );
		_file.writeTime( m_vectStateTimePoint[i].GetTime(), TRUE );
		_file.writeChar( m_vectStateTimePoint[i].GetState() );
		_file.writeLine();
	}
}

void CViehcleMovePath::ReadData( ArctermFile& _file )
{
	_file.getLine();
	_file.getTime( m_arrTime, TRUE );
	_file.getTime( m_depTime, TRUE );

	int nCount;
	_file.getLine();
	_file.getInteger( nCount );
	for( int i=0; i<nCount; i++ )
	{
		_file.getLine();
		CStateTimePoint timePoint;
		_file.getPoint( timePoint );
		ElapsedTime time;
		_file.getTime( time, TRUE );
		timePoint.SetTime( time );
		char c;
		_file.getChar( c );
		timePoint.SetState( c );
		m_vectStateTimePoint.push_back( timePoint );
	}
}


void CViehcleMovePath::Clear()
{
	m_vectStateTimePoint.clear();
}

void CViehcleMovePath::SetDepTime( ElapsedTime _time )
{
	m_depTime = _time;	
}


ElapsedTime CViehcleMovePath::GetDepTime()
{
	return m_depTime;
}

ElapsedTime CViehcleMovePath::GetDestArrTime()
{
	int nCount = m_vectStateTimePoint.size();
	return m_vectStateTimePoint[nCount-1].GetTime();
}

