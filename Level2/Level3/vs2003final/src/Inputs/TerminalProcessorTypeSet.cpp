// TerminalProcessorTypeSet.cpp: implementation of the CTerminalProcessorTypeSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TerminalProcessorTypeSet.h"


CTerminalProcessorTypeSet::CTerminalProcessorTypeSet()
 : DataSet(TerminalProcessorTypeSetFile)
{
	memset(m_bProcessorTypeVisiable, 1, 18*sizeof(BOOL));
}

CTerminalProcessorTypeSet::~CTerminalProcessorTypeSet()
{
}

void CTerminalProcessorTypeSet::readData (ArctermFile& p_file)
{
	p_file.getLine();
	for(int i=0; i<18; i++) {
		p_file.getInteger(m_bProcessorTypeVisiable[i]);
	}
}

void CTerminalProcessorTypeSet::writeData (ArctermFile& p_file) const
{
	for(int i=0; i<18; i++) {
		p_file.writeInt(m_bProcessorTypeVisiable[i]);
	}
	p_file.writeLine();
	
}

BOOL CTerminalProcessorTypeSet::IsProcessorTypeVisiable( int index )
{
	return m_bProcessorTypeVisiable[index];
}

void CTerminalProcessorTypeSet::SetProcessorTypeVisiable( int index, BOOL bAble )
{
	m_bProcessorTypeVisiable[index] = bAble;
}

void CTerminalProcessorTypeSet::clear( void )
{
	memset(m_bProcessorTypeVisiable, 1, 18*sizeof(BOOL));
}
