// GateMappingDB.cpp: implementation of the CGateMappingDB class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GateMappingDB.h"



// read and write
void CGateMapping::readMapData( ArctermFile& p_file,StringDictionary* _pStrDict)
{
	char szBuf[256];
	p_file.getField( szBuf, 256 );
	strMapName = szBuf;
	
	standGate.SetStrDict( _pStrDict );
	standGate.readProcessorID( p_file );
	
	arrivalGate.SetStrDict( _pStrDict );
	arrivalGate.readProcessorID( p_file );
	
	departureGate.SetStrDict( _pStrDict );
	departureGate.readProcessorID( p_file );
}

void CGateMapping::writeMapData( ArctermFile& p_file ) const 
{
	p_file.writeField( strMapName );
	
	standGate.writeProcessorID( p_file );
	arrivalGate.writeProcessorID( p_file );
	departureGate.writeProcessorID( p_file );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGateMappingDB::CGateMappingDB():DataSet( GateMappingDBFile ) 
{

}

CGateMappingDB::~CGateMappingDB()
{

}

void CGateMappingDB::removeItem( int _index )
{
	ASSERT( _index>=0 && _index < static_cast<int>(m_vectMapDB.size()) );
	 
	m_vectMapDB.erase( m_vectMapDB.begin() + _index );
}

const CGateMapping& CGateMappingDB::getItem( int _index ) const
{
	ASSERT( _index>=0 && _index < static_cast<int>(m_vectMapDB.size()) );
	return m_vectMapDB[_index];
}

CGateMapping* CGateMappingDB::GetItem( int _index )
{
	ASSERT( _index >= 0 && _index < static_cast<int>(m_vectMapDB.size()) );
	return &m_vectMapDB[_index];
}

int CGateMappingDB::getIndex( const CString& strMapName ) const
{
	for( unsigned i=0; i<m_vectMapDB.size(); i++ )
	{
		if( m_vectMapDB[i].getMapName() == strMapName )
			return i;
	}

	return -1;
}

// clear data structure before load data from the file.
void CGateMappingDB::clear()
{
	m_vectMapDB.clear();
}

void CGateMappingDB::readData( ArctermFile& p_file )
{
	while( p_file.getLine() )
	{
		CGateMapping gatemap;
		gatemap.readMapData( p_file, m_pInTerm->inStrDict );
		m_vectMapDB.push_back( gatemap );
	}
}

void CGateMappingDB::writeData( ArctermFile& p_file ) const
{
	for( unsigned i=0; i<m_vectMapDB.size(); i++ )
	{
		m_vectMapDB[i].writeMapData( p_file );
		p_file.writeLine();
	}
}

void CGateMappingDB::initDefaultValues()
{
	m_vectMapDB.clear();
}

const char *CGateMappingDB::getTitle () const
{
	return "Gate mapping database";
}

const char *CGateMappingDB::getHeaders () const
{
	return "Mapping Name, Stand Gate ID, Arrival Gate ID, Departure Gate ID";
}


bool CGateMappingDB::ifExistInDB( const CString& _strName, int _iNoCheckIndex)
{
	for( unsigned i=0; i<m_vectMapDB.size(); i++ )
	{
		if( m_vectMapDB[i].getMapName() == _strName && i != _iNoCheckIndex )
			return true;
	}

	return false;
}