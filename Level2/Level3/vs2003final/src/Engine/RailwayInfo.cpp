// RailwayInfo.cpp: implementation of the RailwayInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RailwayInfo.h"
#include "..\common\termfile.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RailwayInfo::RailwayInfo()
{

}
RailwayInfo::RailwayInfo(const RailwayInfo& _anotherRail)
{

	m_railWay.init(_anotherRail.m_railWay);

	/*
	this->m_iFirstStationIndex=_anotherRail.m_iFirstStationIndex;
	this->m_iSecondStationIndex=_anotherRail.m_iSecondStationIndex;
	this->m_iTypeFromFirstViewPoint=_anotherRail.m_iTypeFromFirstViewPoint;
	*/
	m_key = _anotherRail.m_key;
	this->m_iTypeFromSecondViewPoint=_anotherRail.m_iTypeFromSecondViewPoint;
	m_iOwnerIndex=_anotherRail.m_iOwnerIndex;
}
RailwayInfo::~RailwayInfo()
{

}

RailwayInfo& RailwayInfo::operator=(const RailwayInfo& _anotherRail)
{
	if(this!=&_anotherRail)
	{
		m_railWay.init(_anotherRail.m_railWay);
		/*
		this->m_iFirstStationIndex=_anotherRail.m_iFirstStationIndex;
		this->m_iSecondStationIndex=_anotherRail.m_iSecondStationIndex;
		this->m_iTypeFromFirstViewPoint=_anotherRail.m_iTypeFromFirstViewPoint;
		*/
		m_key = _anotherRail.m_key;
		this->m_iTypeFromSecondViewPoint=_anotherRail.m_iTypeFromSecondViewPoint;
		m_iOwnerIndex=_anotherRail.m_iOwnerIndex;
	}
	
	return *this;

}

//read my info from file
void RailwayInfo::ReadDataFromFile(ArctermFile& p_file)
{
	p_file.getLine();

	int iPointCount;
	p_file.getInteger(iPointCount);
	Point* pPointList=new Point[iPointCount];

	for(int i=0;i<iPointCount;++i)
	{
		p_file.getLine();
		p_file.getPoint(pPointList[i]);
	}
	m_railWay.init(iPointCount,pPointList);
	delete [] pPointList;

	p_file.getLine();
	int iData=-1;

	p_file.getInteger(iData );
	m_key.SetFirstStationIndex( iData );

	
	p_file.getInteger(iData);
	m_key.SetTypeFromFirstViewPoint( iData );
	
	p_file.getInteger(iData);
	m_key.SetSecondStationIndex( iData );

	
	p_file.getInteger(m_iTypeFromSecondViewPoint);
	p_file.getInteger(m_iOwnerIndex);
		
}

// save my data to file
void RailwayInfo::SaveDataToFile(ArctermFile& p_file)
{
	int iPointCount=m_railWay.getCount();
	p_file.writeInt(iPointCount);
	p_file.writeLine();
	for(int i=0;i<iPointCount;++i)
	{
		p_file.writePoint(m_railWay.getPoint(i));
		p_file.writeLine();
	}

	p_file.writeInt(m_key.GetFirstStationIndex());
	p_file.writeInt(m_key.GetTypeFromFirstViewPoint());
	p_file.writeInt(m_key.GetSecondStationIndex());
	p_file.writeInt(m_iTypeFromSecondViewPoint);
	p_file.writeInt(m_iOwnerIndex);

	p_file.writeLine();

} 

bool RailwayInfo::operator == ( const RailwayInfo& _anotherRail ) const
{
	return m_key == _anotherRail.m_key ;
}

void RailwayInfo::InsertPoint( int _iAfterIdx, Point& _pt )
{
	ASSERT( _iAfterIdx>=0 && _iAfterIdx< m_railWay.getCount()-1 );
	int iTotalCount = m_railWay.getCount()+1;
	Point* pNewPointList = new Point[ iTotalCount ];
	for(int i=0; i<= _iAfterIdx ; ++i )
	{
		Point tempPoint = m_railWay.getPoint( i );
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}
	pNewPointList[ _iAfterIdx +1 ].setPoint( _pt.getX(), _pt.getY(), _pt.getZ() );

	for( i= _iAfterIdx+2; i< iTotalCount ; ++i )
	{
		Point tempPoint = m_railWay.getPoint( i-1 );
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}
	m_railWay.init( iTotalCount , pNewPointList );
	delete [] pNewPointList;
	
}
void RailwayInfo::GetPoint ( int _iIdx, Point & _pt ) const
{
	ASSERT( _iIdx>=0 && _iIdx< m_railWay.getCount());
	Point* pArray = m_railWay.getPointList();
	_pt.setPoint( pArray[_iIdx ].getX(), pArray[_iIdx ].getY(),pArray[_iIdx ].getZ());
}
void RailwayInfo::SetPoint ( int _iIdx, Point & _pt )
{
	ASSERT( _iIdx>=0 && _iIdx< m_railWay.getCount());
	Point* pArray = m_railWay.getPointList();
	pArray[ _iIdx ].setPoint( _pt.getX(), _pt.getY(), _pt.getZ() );
	
}
void RailwayInfo::DeletePoint( int _iIdx )
{
	ASSERT( _iIdx>=0 && _iIdx< m_railWay.getCount());
	int iTotalCount = m_railWay.getCount()-1;
	Point* pNewPointList = new Point[ iTotalCount ];
	for(int i=0; i< _iIdx ; ++i )
	{
		Point tempPoint = m_railWay.getPoint( i );
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}

	for( i= _iIdx; i< iTotalCount ; ++i )
	{
		Point tempPoint = m_railWay.getPoint( i+1 );
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}
	m_railWay.init( iTotalCount , pNewPointList );
	delete [] pNewPointList;
}