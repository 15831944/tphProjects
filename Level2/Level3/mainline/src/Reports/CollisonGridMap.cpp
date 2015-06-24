// CollisonGridMap.cpp: implementation of the CCollisonGridMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CollisonGridMap.h"
#include "common\Point.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollisonGridMap::CCollisonGridMap()
{

}

CCollisonGridMap::~CCollisonGridMap()
{
	m_vGridMap.clear();
}

void CCollisonGridMap::InitMapStruct(Point& _leftBottom, Point& _rightTop, DistanceUnit _gridWidth )
{
	m_leftBottom = _leftBottom;
	m_rightTop = _rightTop;
	DistanceUnit m_length = fabs( _rightTop.getX()  -  _leftBottom.getX() );
	DistanceUnit m_width = fabs( _rightTop.getY()  -  _leftBottom.getY() );
	m_gridWidth = _gridWidth;
	m_iXCount = (long)(m_length / m_gridWidth)+1;
	m_iYCount = (long)(m_width / m_gridWidth)+1;
	m_vGridMap.clear();
	m_vGridMap.reserve( m_iXCount*m_iYCount );
	for( long i=0; i < m_iYCount ; ++i )
	{
		for( long j=0; j < m_iXCount ; ++j )
		{
			CCollisonGrid temp;
			m_vGridMap.push_back( temp );
		}
	}

}
void CCollisonGridMap::AddPersonIntoGrid( PAXPOSE& _personInfo )
{
	long iX = (long)(( _personInfo.m_x - m_leftBottom.getX() ) / m_gridWidth);
	long iY = (long)(( _personInfo.m_y - m_leftBottom.getY() ) / m_gridWidth);
	ASSERT( iX>=0 && iX<m_iXCount && iY>=0 && iY<m_iYCount );
	long iIdx = iX + iY * m_iXCount ;
	ASSERT( iIdx >=0 && (unsigned)iIdx < m_vGridMap.size() );
	m_vGridMap[ iIdx ].AddPerson( _personInfo );

}

void CCollisonGridMap::ComputeAndWriteToFile( ArctermFile& p_file , ElapsedTime& _time ,const CMobileElemConstraint& _sPaxType,Terminal* _pTerm)
{
	long iSize = m_vGridMap.size();
	for( long i=0; i<iSize ; ++i )
	{
		m_vGridMap[i].WritePersonInGrid(p_file,_time,_sPaxType,_pTerm);
		if( (i+1) % m_iXCount != 0  && (i+1) < iSize )
		{
			m_vGridMap[i].WritePersonInOtherGrid( m_vGridMap[i+1] , p_file , _time , _sPaxType , _pTerm , m_gridWidth*2.0/sqrt(2.0) );
		}

		long iUpGridPos = ( i / m_iXCount +1 ) * m_iXCount + i % m_iXCount;
		if( iUpGridPos < iSize )
		{
			m_vGridMap[i].WritePersonInOtherGrid( m_vGridMap[iUpGridPos] , p_file , _time , _sPaxType , _pTerm , m_gridWidth*2.0/sqrt(2.0) );
		}

		if( (iUpGridPos+1) % m_iXCount != 0 && (iUpGridPos+1) < iSize )
		{
			m_vGridMap[i].WritePersonInOtherGrid( m_vGridMap[iUpGridPos+1] , p_file , _time , _sPaxType , _pTerm , m_gridWidth*2.0/sqrt(2.0) );
		}

		m_vGridMap[i].ClearGridPerson();//*****************************notice************//
		
	}
	
}

void CCollisonGridMap::ClearGridPerson()
{
	GRIDMAP::iterator iter = m_vGridMap.begin();
	GRIDMAP::iterator iterEnd = m_vGridMap.end();
	for( ; iter != iterEnd; ++iter )
	{
		iter->ClearGridPerson();
	}
}
