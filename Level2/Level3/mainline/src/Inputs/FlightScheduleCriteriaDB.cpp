// FlightScheduleCriteriaDB.cpp: implementation of the FlightScheduleCriteriaDB class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "FlightScheduleCriteriaDB.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FlightScheduleCriteriaDB::FlightScheduleCriteriaDB():DataSet(FlightScheduleCriteriaFile)
,m_bReAssign( false ),m_bAutoAssign( false )
{

}

FlightScheduleCriteriaDB::~FlightScheduleCriteriaDB()
{
	clear();
}
void FlightScheduleCriteriaDB::readData (ArctermFile& p_file)
{
	p_file.getLine();
	int iCount = 0;

	p_file.getInteger( iCount );
	if( iCount == 1l )
	{
		m_bAutoAssign = true;
	}
	else
	{
		m_bAutoAssign = false;
	}

	p_file.getInteger( iCount );
	if( iCount == 1l )
	{
		m_bReAssign = true;
	}
	else
	{
		m_bReAssign = false;
	}
	p_file.getInteger( iCount );
	for( int i=0; i<iCount; ++i )
	{
		CFlightScheduleAddUtil* pTempUtil = new CFlightScheduleAddUtil(m_pInTerm);
		pTempUtil->ReadFromFile( p_file );
		m_vAllSchduleUtil.push_back( pTempUtil );
	}

}
void FlightScheduleCriteriaDB::writeData (ArctermFile& p_file) const
{
	if( m_bAutoAssign )
	{
		p_file.writeInt( 1l );
	}
	else
	{
		p_file.writeInt( 0l );
	}

	if( m_bReAssign )
	{
		p_file.writeInt( 1l );
	}
	else
	{
		p_file.writeInt( 0l );
	}
	int iCount = m_vAllSchduleUtil.size();
	p_file.writeInt( iCount );
	p_file.writeLine();
	for( int i=0; i<iCount; ++i )
	{
		m_vAllSchduleUtil[i]->WriteToFile( p_file );
	}
}
void FlightScheduleCriteriaDB::clear (void)
{
	int iCount = m_vAllSchduleUtil.size();
	for( int i=0; i<iCount; ++i )
	{
		 if( m_vAllSchduleUtil[i] )
		 {
			 delete m_vAllSchduleUtil[i] ;
		 }
	}
	m_vAllSchduleUtil.clear();
}

CFlightScheduleAddUtil* FlightScheduleCriteriaDB::GetItem( int _iIdx )
{
	ASSERT( _iIdx >= 0 && _iIdx < static_cast<int>(m_vAllSchduleUtil.size()) );
	return m_vAllSchduleUtil[ _iIdx ];
}
void FlightScheduleCriteriaDB::RemoveItem( int _iIdx )
{
	ASSERT( _iIdx >= 0 && _iIdx < static_cast<int>(m_vAllSchduleUtil.size()) );
	delete m_vAllSchduleUtil[ _iIdx ];
	m_vAllSchduleUtil.erase( m_vAllSchduleUtil.begin() + _iIdx );
}

void FlightScheduleCriteriaDB::RemoveItem( CFlightScheduleAddUtil* _pItem )
{
	std::vector<CFlightScheduleAddUtil*>::iterator iter = std::find( m_vAllSchduleUtil.begin(), m_vAllSchduleUtil.end(), _pItem );
	if( iter != m_vAllSchduleUtil.end() )
	{
		delete *iter;
		m_vAllSchduleUtil.erase( iter );
	}
}