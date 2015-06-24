// PipeIndexList.cpp: implementation of the CPipeIndexList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PipeIndexList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPipeIndexList::CPipeIndexList()
{
	m_iTypeOfUsingPipe = 0;
	m_iIsOneToOne = 0;
}

CPipeIndexList::~CPipeIndexList()
{

}

int CPipeIndexList::GetSize() const
{
	return m_vPipeList.size();
}


int CPipeIndexList::GetPipeIdx( int _nIdx ) const
{
	return m_vPipeList[_nIdx]; 
}


void CPipeIndexList::AddPipeIdx( int _iPipeIdx )
{
	m_vPipeList.push_back( _iPipeIdx );
}
void CPipeIndexList::WriteData( ArctermFile& p_file) const
{
	p_file.writeInt( m_iTypeOfUsingPipe );
	if( m_iTypeOfUsingPipe > 0 )
	{
		int iSize = m_vPipeList.size();
		p_file.writeInt( iSize );
		for( int i=0; i<iSize; ++i )
		{
			p_file.writeInt( m_vPipeList[i] );
		}
	}

	p_file.writeInt( m_iIsOneToOne );
	
	//p_file.writeLine();

}
void CPipeIndexList::ReadData( ArctermFile& p_file)
{
	p_file.getInteger( m_iTypeOfUsingPipe );
	if( m_iTypeOfUsingPipe > 0 )
	{
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
	p_file.getInteger( m_iIsOneToOne );	

}
void CPipeIndexList::DeletePipeOrAdjust( int _iPipeIdx )
{
	int iSize = m_vPipeList.size();
	for( int i=iSize-1; i>=0; --i )
	{
		if( m_vPipeList[i] > _iPipeIdx )
		{
			m_vPipeList[ i ] = m_vPipeList[ i ] - 1;
		}
		else if( m_vPipeList[i] == _iPipeIdx )
		{
			m_vPipeList.clear();
			m_iTypeOfUsingPipe = 0;
			return;
		}
	}
}
bool CPipeIndexList::IfUseThisPipe( int _iPipeIdx )const
{
	int iSize = m_vPipeList.size();
	for( int i=iSize-1; i>=0; --i )
	{
		if( m_vPipeList[i] == _iPipeIdx )
		{
			return true;
		}
	}

	return false;
}