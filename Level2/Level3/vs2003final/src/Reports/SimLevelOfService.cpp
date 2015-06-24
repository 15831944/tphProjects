// SimLevelOfService.cpp: implementation of the CSimLevelOfService class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include "assert.h"
#include "common\termfile.h"
#include "SimLevelOfService.h"
#include <CommonData/procid.h>

#include <algorithm>

CSimLevelOfServiceItem::CSimLevelOfServiceItem()
{

}

CSimLevelOfServiceItem::~CSimLevelOfServiceItem()
{

}

void CSimLevelOfServiceItem::setProcessorName( const CString& _strName )
{
	m_strProcessorsName = _strName;
}

const CString& CSimLevelOfServiceItem::GetProcessorName( void ) const
{
	return m_strProcessorsName;
}

const std::vector<Mob_ServicTime>& CSimLevelOfServiceItem::getSpecificPaxTypeService() const
{
	return m_vPaxTypeServices;
}

void CSimLevelOfServiceItem::setSpecificPaxTypeService( const std::vector<Mob_ServicTime>&  _vectorPaxTypeService )
{
	m_vPaxTypeServices = _vectorPaxTypeService;
	Sort();
}

void CSimLevelOfServiceItem::writeData( ArctermFile& _file )
{
	_file.writeField( m_strProcessorsName );
	_file.writeLine();
	
	int vsize = m_vPaxTypeServices.size();
	_file.writeInt( vsize );
	_file.writeLine();

	for( int i=0; i<vsize; i++)
	{
		Mob_ServicTime mob_st = m_vPaxTypeServices[i];
		mob_st.first.writeConstraint( _file );

		SERVICE_ITEMS temp = mob_st.second;
		_file.writeInt( temp.m_iUpperQueueLength );
		_file.writeInt( temp.m_iLowerQueueLength );
		_file.writeInt( temp.m_lUpperQueueWaitingSeconds );
		_file.writeInt( temp.m_lLowerQueueWaitingSeconds );
		_file.writeField( temp.m_strAreaName );
		_file.writeInt( temp.m_iUpperAreaDensity );		
		_file.writeInt( temp.m_iLowerAreaDensity );		
		_file.writeLine();
	}
}

void CSimLevelOfServiceItem::readData( ArctermFile& _file ,InputTerminal* _interm )
{
	char szBuf[256];

	_file.getLine();
	_file.getField( szBuf, 256 );
	m_strProcessorsName = CString( szBuf );

	int mapsize = 0;
	_file.getLine();
	_file.getInteger( mapsize );

	for( int i=0; i<mapsize; i++ )
	{
		_file.getLine();
		CMobileElemConstraint mob(_interm);
		//mob.SetInputTerminal( _interm );
		mob.readConstraint( _file );

		SERVICE_ITEMS temp;
		_file.getInteger( temp.m_iUpperQueueLength );
		_file.getInteger( temp.m_iLowerQueueLength );
		_file.getInteger( temp.m_lUpperQueueWaitingSeconds );
		_file.getInteger( temp.m_lLowerQueueWaitingSeconds );
		_file.getField( szBuf , 256 );
		temp.m_strAreaName = szBuf;
		_file.getInteger( temp.m_iUpperAreaDensity );	
		_file.getInteger( temp.m_iLowerAreaDensity );	
		
		Mob_ServicTime mob_st;
		mob_st.first = mob;
		mob_st.second = temp;
		m_vPaxTypeServices.push_back( mob_st );
	}

	Sort();
}

const SERVICE_ITEMS* CSimLevelOfServiceItem::getTheNearestPax( const CMobileElemConstraint& _mob )const
{
	for( unsigned i = 0; i< m_vPaxTypeServices.size(); i++ )
	{
		if( m_vPaxTypeServices[i].first.fits( _mob ) )
			return &(m_vPaxTypeServices[i].second);
	}
	
	return NULL;
}
const CMobileElemConstraint* CSimLevelOfServiceItem::getTheNearestPaxType( const CMobileElemConstraint& _mob )const
{
	for( unsigned i = 0; i< m_vPaxTypeServices.size(); i++ )
	{
		if( m_vPaxTypeServices[i].first.fits( _mob ) )
			return &(m_vPaxTypeServices[i].first);
	}

	return NULL;
}
void CSimLevelOfServiceItem::Sort()
{
	std::sort( m_vPaxTypeServices.begin(), m_vPaxTypeServices.end(),SortByMobileElemConstraint );
}

bool SortByMobileElemConstraint( Mob_ServicTime _m1, Mob_ServicTime _m2 )
{
	return  _m1.first<_m2.first ? true : false;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimLevelOfService::CSimLevelOfService( CStartDate _startDate )
:CSimGeneralPara( _startDate )
{
	m_bDynamicCreateProc = FALSE;
}

CSimLevelOfService::~CSimLevelOfService()
{

}

void CSimLevelOfService::WriteData( ArctermFile& _file )
{
	CSimGeneralPara::WriteData( _file );

	int vectorsize = m_vSimTLOSSetting.size();
	_file.writeInt( vectorsize );
	_file.writeLine();
	for( int i=0; i<vectorsize; i++ )
	{
		m_vSimTLOSSetting[i].writeData( _file );
	}
	_file.writeInt( m_bDynamicCreateProc );
	_file.writeLine();
}

void CSimLevelOfService::ReadData( ArctermFile& _file, InputTerminal* _interm ) 
{
	CSimGeneralPara::ReadData( _file, _interm );
	
	int vectorsize  = 0;
	_file.getLine();
	_file.getInteger( vectorsize );

	for( int i=0; i<vectorsize; i++ )
	{
		CSimLevelOfServiceItem item;
		item.readData( _file,_interm );
		m_vSimTLOSSetting.push_back( item );
	}

	if ( _file.getVersion() > 2.4 )
	{
		_file.getLine();
		_file.getInteger( m_bDynamicCreateProc );
	}

	
}

int CSimLevelOfService::getSettingCout( void ) const 
{
	return m_vSimTLOSSetting.size();
}

bool CSimLevelOfService::addSettingItem( const CSimLevelOfServiceItem& _item )
{
	if( ifExistInVector( _item ) )
		return false;

	m_vSimTLOSSetting.push_back( _item );
	return true;
}

CSimLevelOfServiceItem& CSimLevelOfService::getSetting( int _index )
{
	assert( _index>=0 && (unsigned)_index< m_vSimTLOSSetting.size() );

	return m_vSimTLOSSetting[ _index ];
}

const CSimLevelOfServiceItem& CSimLevelOfService::GetSetting( int _index ) const
{
	assert( _index>=0 && (unsigned)_index< m_vSimTLOSSetting.size() );
	
	return m_vSimTLOSSetting[ _index ];
}

std::vector< CSimLevelOfServiceItem >& CSimLevelOfService:: getSettingVector( void )
{
	return m_vSimTLOSSetting;
}


bool CSimLevelOfService::ifExistInVector(  const CSimLevelOfServiceItem& _item )
{
	CString strProcName = _item.GetProcessorName();

	for( unsigned i=0; i<m_vSimTLOSSetting.size(); i++ )
	{
		if( m_vSimTLOSSetting[i].GetProcessorName() == strProcName )
			return true;
	}

	return false;
}

const CSimLevelOfServiceItem* CSimLevelOfService::getTheNearestProc( const ProcessorID& _id )
{
	ProcessorID id = _id;
	do {
		CString strID = id.GetIDString();
		for( unsigned i=0; i<m_vSimTLOSSetting.size(); i++ )
		{
			if( strID == m_vSimTLOSSetting[i].GetProcessorName() )
				return &m_vSimTLOSSetting[i];
		}
	} while( id.getSuperGroup() );
	
	return NULL;
}

