// PipeNameList.cpp: implementation of the CPipeNameList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PipeNameList.h"
#include "common\termfile.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPipeNameList::CPipeNameList()
{

}

CPipeNameList::~CPipeNameList()
{

}

int CPipeNameList::GetSize() const
{
	return m_vPipeList.size();
}


int CPipeNameList::GetPipeIdx( int _nIdx ) const
{
	return m_vPipeList[_nIdx]; 
}


void CPipeNameList::AddPipeIdx( int _iPipeIdx )
{
	m_vPipeList.push_back( _iPipeIdx );
}
void CPipeNameList::WriteData( ArctermFile& p_file) const
{
	int iSize = m_vPipeList.size();
	p_file.writeInt( iSize );
	for( int i=0; i<iSize; ++i )
	{
		p_file.writeInt( m_vPipeList[i] );
	}
	//p_file.writeLine();

}
void CPipeNameList::ReadData( ArctermFile& p_file)
{
	//p_file.getLine();
	int iSize = 0;
	p_file.getInteger( iSize );
	for( int i=0; i<iSize ; ++i )
	{
		int iIdx =-1;
		p_file.getInteger( iIdx );
		ASSERT( iIdx>=0 );
		m_vPipeList.push_back( iIdx );
	}

}