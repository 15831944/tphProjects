// SideMoveWalkProcData.cpp: implementation of the CSideMoveWalkProcData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SideMoveWalkProcData.h"
#include "in_term.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSideMoveWalkProcData::CSideMoveWalkProcData( const ProcessorID& id ):ProcessorDataElement( id )
{
	m_dMoveSpeed = 100.0;
	m_dWidth = 100.0;
	m_iCapacity = 0;
}

CSideMoveWalkProcData::~CSideMoveWalkProcData()
{

}
void CSideMoveWalkProcData::readData (ArctermFile& p_file)
{
	p_file.getLine();
	procID.SetStrDict( m_pInputTerm->inStrDict);
	procID.readProcessorID( p_file );
	double d;
	p_file.getFloat( d );
	m_dMoveSpeed = d * SCALE_FACTOR;
	p_file.getFloat( d );
	m_dWidth = d * SCALE_FACTOR;
	p_file.getInteger( m_iCapacity );
}
void CSideMoveWalkProcData::writeData (ArctermFile& p_file) const
{
	procID.writeProcessorID( p_file );
	p_file.writeFloat( (float)(m_dMoveSpeed / SCALE_FACTOR) );
	p_file.writeFloat( (float) (m_dWidth / SCALE_FACTOR) );
	p_file.writeInt( m_iCapacity );
	p_file.writeLine();
}