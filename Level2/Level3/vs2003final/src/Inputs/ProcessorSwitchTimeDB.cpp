// ProcessorSwitchTimeDB.cpp: implementation of the CProcessorSwitchTimeDB class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "inputs\ProcessorSwitchTimeDB.h"

void ProSwitchTime::SetDefaultValue()
{
	int mode = GetFltConstraintMode();
	if( mode == ENUM_FLTCNSTR_MODE_DEP )
		iStyle = 1;
	else if( mode == ENUM_FLTCNSTR_MODE_ARR )
		iStyle = 2;
	else if( mode == ENUM_FLTCNSTR_MODE_ALL )
		iStyle = 3;
	else
		iStyle = 0;

	DepOpenTime = -120l;
	DepCloseTime = -10l;
	ArrOpenTime = 10l;
	ArrCloseTime = 60l;
	m_proNum = 1 ;
	m_isDaily = true ;
	/*if( mode=='D'||mode=='\0' )
	{
		DepOpenTime  = 60l;
		DepCloseTime = 10l;
	}
	if( mode=='A'||mode=='\0' )
	{
		ArrOpenTime	 = 10l;
		ArrCloseTime = 60l; 
	}*/
}

void ProSwitchTime::SetDepDefValue()
{
	DepOpenTime = -120l;
	DepCloseTime = -10l;
}

void ProSwitchTime::SetArrDefValue()
{
	ArrOpenTime = 10l;
	ArrCloseTime = 60l;
}
void ProSwitchTime::readData( ArctermFile& p_file)
{
	m_MobType.readConstraint( p_file );
	p_file.getInteger( iStyle );
	p_file.getInteger( DepOpenTime );
	p_file.getInteger( DepCloseTime );
	p_file.getInteger( ArrOpenTime );
	p_file.getInteger( ArrCloseTime );
	if(p_file.getVersion() > 2.2)
	{
		p_file.getInteger(m_proNum) ;
		p_file.getInteger(m_isDaily) ;
	}
	/*int mode = getMode();
	if( mode == 'D' || mode == '\0')
	{
		p_file.getInteger( DepOpenTime );
		p_file.getInteger( DepCloseTime );
	}
	if( mode == 'A' || mode == '\0' )
	{
		p_file.getInteger( ArrOpenTime );
		p_file.getInteger( ArrCloseTime );
	}*/
}

void ProSwitchTime::writeData( ArctermFile& p_file) const
{
	m_MobType.writeConstraint(p_file);
	p_file.writeInt( iStyle );
	p_file.writeInt( DepOpenTime );
	p_file.writeInt( DepCloseTime );
	p_file.writeInt( ArrOpenTime );
	p_file.writeInt( ArrCloseTime );
	p_file.writeInt(m_proNum) ;
	p_file.writeInt(m_isDaily) ;
	/*int mode = getMode();
	if( mode == 'D' || mode == '\0')
	{
		p_file.writeInt( DepOpenTime );
		p_file.writeInt( DepCloseTime );
	}
	if( mode == 'A' || mode == '\0')
	{
		p_file.writeInt( ArrOpenTime );
		p_file.writeInt( ArrCloseTime );
	}*/
}

bool ProSwitchTime::operator == (ProSwitchTime pst) const
{
	if( !(m_MobType== pst.m_MobType) )
		return false;
	/*
	if( getMode()=='D' || getMode=='\0')
	{
		if(DepOpenTime != pst.DepOpenTime )
			return false;
		if(DepCloseTime != pst.DepCloseTime)
			return false;
	}
	if( getMode() == 'A' || getMode=='\0' )
	{
		if( ArrOpenTime != pst.ArrOpenTime)
			return false;
		if( ArrCloseTime != pst.ArrCloseTime)
			return false;
	}*/
	return true;
}

bool ProSwitchTime::isEqual( ProSwitchTime pst ) const
{
	if( !(m_MobType== pst.m_MobType) )
		return false;
	if( iStyle != pst.iStyle )
		return false;
	if(DepOpenTime != pst.DepOpenTime )
		return false;
	if(DepCloseTime != pst.DepCloseTime)
		return false;
	if( ArrOpenTime != pst.ArrOpenTime)
		return false;
	if( ArrCloseTime != pst.ArrCloseTime)
		return false;
	if(m_proNum != pst.m_proNum)
		return false ;
	if(m_isDaily != pst.m_isDaily)
		return false ;
	return true;	
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessorSwitchTimeDB::CProcessorSwitchTimeDB()
:DataSet( ProcessorSwitchTimeFile,(float)2.21 )
{
  
}

CProcessorSwitchTimeDB::~CProcessorSwitchTimeDB()
{
}

void CProcessorSwitchTimeDB::readData(ArctermFile& p_file)
{
	while( p_file.getLine() )
	{
		ProSwitchTime pst;
		pst.getMobType()->SetInputTerminal(m_pInTerm);
		pst.readData( p_file);
		m_data.push_back( pst );
	}
}

void CProcessorSwitchTimeDB::writeData(ArctermFile& p_file) const
{
	int nSize = m_data.size();
	for( int i=0; i<nSize; i++)
	{
		m_data.at(i).writeData( p_file );
		p_file.writeLine();
	}
}

const char* CProcessorSwitchTimeDB::getTitle () const
{
	return "Processor Switch Time Database";
}


const char* CProcessorSwitchTimeDB::getHeaders () const
{
	return "Time";
}


int CProcessorSwitchTimeDB::addItem( ProSwitchTime& pst)
{ 
	/*int pos;
	for(pos=0; pos<m_data.size(); pos++)
	{
		if( m_data[pos] == pst )
			return pos;
	}*/
	m_data.push_back( pst ); 

	return m_data.size()-1;
}

void CProcessorSwitchTimeDB::setItem( int _indx,ProSwitchTime& pst)
{
	assert( _indx>=0 && (UINT)_indx < m_data.size() );
	m_data[_indx] = pst;
}