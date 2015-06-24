// StationLayout.cpp: implementation of the CStationLayout class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StationLayout.h"
#include <algorithm>
#include "engine\Hold.h"
#include <iterator>
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CStationLayout::CStationLayout()
{
	m_preBoardingArea=new HoldingArea();
	m_dCarLength= 10*SCALE_FACTOR;
	m_dCarWidth = 5*SCALE_FACTOR;
	m_dPlatformLength = 60*SCALE_FACTOR;
	m_dPlatformWidth = 20*SCALE_FACTOR;

	m_nCarNumber = 0;
		
	
	m_nEntryDoorNum =0;
	m_nExitDoorNum = 0;
	m_nCarCapacity = 100;

	m_nExitDoorType = 0;
	
}

CStationLayout::CStationLayout(const CStationLayout& _anotherLayout)
{
	
	m_preBoardingArea=new HoldingArea( *(_anotherLayout.m_preBoardingArea) );
	m_adjustPoint=_anotherLayout.m_adjustPoint;
    m_port1Path.init( _anotherLayout.m_port1Path );
	m_port2Path.init( _anotherLayout.m_port2Path );
	
	m_dCarWidth = _anotherLayout.m_dCarWidth;
	m_dCarLength = _anotherLayout.m_dCarLength;
	m_dPlatformLength = _anotherLayout.m_dPlatformLength;
	m_dPlatformWidth = _anotherLayout.m_dPlatformWidth;


	m_nCarNumber = _anotherLayout.m_nCarNumber;
	m_nEntryDoorNum = _anotherLayout.m_nEntryDoorNum;
	m_nExitDoorNum = _anotherLayout.m_nExitDoorNum;
	m_nCarCapacity = _anotherLayout.m_nCarCapacity;

	m_nExitDoorType = _anotherLayout.m_nExitDoorType;
	CAR::const_iterator iter=_anotherLayout.m_cars.begin();
	CAR::const_iterator iterLast=_anotherLayout.m_cars.end();
	for( ; iter!=iterLast; ++iter)
	{
		CCar* pTemp=new CCar( (**iter) );
		m_cars.push_back( pTemp );
	}
	m_pInTerm = _anotherLayout.m_pInTerm;
	m_strPath = _anotherLayout.m_strPath;
}

CStationLayout& CStationLayout::operator = ( const CStationLayout& _anotherLayout )
{
	if( this!= &_anotherLayout)
	{
		delete m_preBoardingArea;
		m_preBoardingArea= new HoldingArea(*(_anotherLayout.m_preBoardingArea));
		m_adjustPoint=_anotherLayout.m_adjustPoint;
		m_port1Path.init( _anotherLayout.m_port1Path );
		m_port2Path.init( _anotherLayout.m_port2Path );

		m_dCarWidth = _anotherLayout.m_dCarWidth;
		m_dCarLength = _anotherLayout.m_dCarLength;
		m_dPlatformLength = _anotherLayout.m_dPlatformLength;
		m_dPlatformWidth = _anotherLayout.m_dPlatformWidth;


		m_nCarNumber = _anotherLayout.m_nCarNumber;
		m_nEntryDoorNum = _anotherLayout.m_nEntryDoorNum;
		m_nExitDoorNum = _anotherLayout.m_nExitDoorNum;
		m_nCarCapacity = _anotherLayout.m_nCarCapacity;
		m_nExitDoorType = _anotherLayout.m_nExitDoorType;

		m_strPath = _anotherLayout.m_strPath;
		m_pInTerm = _anotherLayout.m_pInTerm;

		CAR::const_iterator iter=m_cars.begin();
		CAR::const_iterator iterLast=m_cars.end();
		for( ; iter!=iterLast; ++iter)
		{
			delete *iter;
		}
		m_cars.clear();

		iter=_anotherLayout.m_cars.begin();
		iterLast=_anotherLayout.m_cars.end();
		for(; iter!=iterLast; ++iter)
		{
			CCar* pTemp=new CCar( (**iter) );
			m_cars.push_back( pTemp );
		}

	}
	return *this;
}

CStationLayout::~CStationLayout()
{
	if (m_preBoardingArea)
	{
		delete m_preBoardingArea;
		m_preBoardingArea = NULL;
	}	

	CAR::const_iterator iter=m_cars.begin();
	CAR::const_iterator iterLast=m_cars.end();
	for( ; iter!=iterLast; ++iter)
	{
		delete *iter;
	}
	m_cars.clear();

}

void CStationLayout::writeData (ArctermFile& p_file) const
{
	//p_file.writeLine();
	p_file.writePoint(m_adjustPoint);
	p_file.writeLine();

	p_file.writeInt((int)m_cars.size());
	p_file.writeLine();

	assert( m_pInTerm );
	if(m_cars.size() > 0 )
	{
		CAR::const_iterator iter=m_cars.begin();
		CAR::const_iterator iterLast=m_cars.end();

		for(;iter!=iterLast;++iter)
		{
			(*iter)->m_pTerm=(Terminal*)m_pInTerm ;
			(*iter)->WriteAllProcessorInfo(p_file);
			//p_file.writeLine();
		}

		m_preBoardingArea->SetTerminal( (Terminal*)m_pInTerm );
		m_preBoardingArea->writeProcessor(p_file);
		p_file.writeLine();

		int iPointCount= m_port1Path.getCount();
		p_file.writeInt( iPointCount );
		p_file.writeLine();

		for( int i=0; i<iPointCount; ++i)
		{
			p_file.writePoint( m_port1Path.getPoint( i ) );
		}

		p_file.writeLine();

		iPointCount= m_port2Path.getCount();
		p_file.writeInt( iPointCount );
		p_file.writeLine();

		for( int ii=0; ii<iPointCount; ++ii)
		{
			p_file.writePoint( m_port2Path.getPoint( ii ) );
		}
		p_file.writeLine();
	}

	

	p_file.writeFloat( (float)m_dPlatformLength );	
	p_file.writeFloat( (float)m_dPlatformWidth );
	p_file.writeFloat( (float)m_dCarLength );
	p_file.writeFloat( (float)m_dCarWidth );
	p_file.writeInt( m_nCarNumber );	
	p_file.writeInt( m_nEntryDoorNum );
	p_file.writeInt( m_nExitDoorNum );
	p_file.writeInt( m_nCarCapacity );
	p_file.writeInt( m_nExitDoorType );
	
	p_file.writeLine();
	
	
	

}

void CStationLayout::readData (ArctermFile& p_file)
{
	//char temp[256];
	//p_file.getField(temp,256);
    //p_file.getLine();

	p_file.getLine();
	p_file.getPoint(m_adjustPoint);

	p_file.getLine();
	int iCarCount=0;
	p_file.getInteger(iCarCount);
	if( iCarCount > 0 )
	{
		for( int i=0; i<iCarCount; ++i )
		{
			CCar* pTemp=new CCar();
			pTemp->m_pTerm=(Terminal*)this->m_pInTerm;
			//p_file.getLine();
			pTemp->ReadAllProcessorInfo( p_file );
			m_cars.push_back( pTemp );
		}
		m_preBoardingArea->SetTerminal( (Terminal*)this->m_pInTerm );

		p_file.getLine();
		m_preBoardingArea->ReadProcessor(p_file);


		int iPointCount=0;
		p_file.getLine();
		p_file.getInteger( iPointCount );

		Point* pPointList= new Point[ iPointCount ];
		p_file.getLine();
		for(int ii=0; ii<iPointCount; ++ii)
		{
			p_file.getPoint( pPointList[ ii ] ) ;
		}
		m_port1Path.init( iPointCount, pPointList );
		delete pPointList;

		p_file.getLine();
		p_file.getInteger( iPointCount );

		p_file.getLine();
		pPointList= new Point[ iPointCount ];
		for( int i3=0; i3<iPointCount; ++i3)
		{
			p_file.getPoint( pPointList[ i3 ] ) ;
		}
		m_port2Path.init( iPointCount, pPointList );
		delete pPointList;
	}

	
	p_file.getLine();
	p_file.getFloat( m_dPlatformLength );	
	p_file.getFloat( m_dPlatformWidth );
	p_file.getFloat( m_dCarLength );
	p_file.getFloat( m_dCarWidth );
	p_file.getInteger( m_nCarNumber );	
	p_file.getInteger( m_nEntryDoorNum );
	p_file.getInteger( m_nExitDoorNum );
	p_file.getInteger( m_nCarCapacity );
	p_file.getInteger( m_nExitDoorType );

	if( iCarCount > 0 )
	{
		ComputeLayoutInfo();
	}
	

}

void CStationLayout::DoOffSet( DistanceUnit _X, DistanceUnit _Y )
{
	//m_adjustPoint.DoOffset( _X, _Y ,0 );
	if( m_preBoardingArea)
		m_preBoardingArea->DoOffset( _X, _Y );

	if( m_port1Path.getCount())
		m_port1Path.DoOffset( _X, _Y,0 );
	if(  m_port2Path.getCount())
		m_port2Path.DoOffset( _X, _Y, 0 );
	CAR::const_iterator iter=m_cars.begin();
	CAR::const_iterator iterLast=m_cars.end();

	for(;iter!=iterLast;++iter)
	{
		if(*iter)
			(*iter)->DoOffest( _X, _Y );
	}
	
}

void CStationLayout::GetAllCars( CAR& _allCar) const
{
	_allCar.clear();
	copy( m_cars.begin(), m_cars.end(), back_inserter( _allCar) );
}

void CStationLayout::clear (void)
{
	return;
}

void CStationLayout::DeleteCar( int _nIdx )
{
	ASSERT( _nIdx>=0 && (UINT)_nIdx<m_cars.size() );
	CAR::iterator iter = m_cars.begin() + _nIdx;
	if(*iter)
		delete *iter;
	m_cars.erase( iter );
}

BOOL CStationLayout::ComputeLayoutInfo()
{
	Point portPoint;
	//port1
	portPoint.setX( m_adjustPoint.getX() - m_dPlatformLength/2 );
	portPoint.setY( m_adjustPoint.getY() );
	m_port1Path.init(1, &portPoint );

	////port2
	portPoint.setX( m_adjustPoint.getX() + m_dPlatformLength/2 );
	portPoint.setY( m_adjustPoint.getY() );
	m_port2Path.init(1, &portPoint );

	int nCarNumDelta = m_nCarNumber - m_cars.size();
	if( nCarNumDelta > 0 )
	{
		for( int i=1; i<=nCarNumDelta; ++i )
		{
			CCar *pNewCar = new CCar;
			pNewCar->m_pTerm = (Terminal *)m_pInTerm;
			m_cars.push_back( pNewCar);
		}
	}
	else if ( nCarNumDelta < 0 )
	{
		int nLowIndex = m_cars.size()+nCarNumDelta;
		for( int i=m_cars.size()-1;i>=nLowIndex; --i)
		{
			delete m_cars[i];
			m_cars.erase( m_cars.begin() + i );
		}
	}

	if( m_cars.size()<=0 )
	{

		CCar *pNewCar = new CCar;
		pNewCar->m_pTerm = (Terminal *)m_pInTerm;
		m_cars.push_back( pNewCar);
		//ProcessorID id;
		//id.SetStrDict( this->m_pInTerm->inStrDict );
		//id.setID("CAR0-CARAREA");
		//m_cars.at(0)->m_carArea->init( id );
		m_nCarNumber = 1;
	}
	
	//all car area
	int nCarNum = m_cars.size();
	Point rectangleArea[4];
	if( nCarNum % 2 == 0 )
	{
		rectangleArea[0].setX( m_adjustPoint.getX() - (nCarNum/2)*m_dCarLength );
		rectangleArea[0].setY( m_adjustPoint.getY() + m_dCarWidth/2 );

		rectangleArea[1].setX( m_adjustPoint.getX() - (nCarNum/2-1)*m_dCarLength );
		rectangleArea[1].setY( m_adjustPoint.getY() + m_dCarWidth/2 );

		rectangleArea[2].setX( m_adjustPoint.getX() - (nCarNum/2-1)*m_dCarLength );
		rectangleArea[2].setY( m_adjustPoint.getY() - m_dCarWidth/2 );

		rectangleArea[3].setX( m_adjustPoint.getX() - (nCarNum/2)*m_dCarLength );
		rectangleArea[3].setY( m_adjustPoint.getY() - m_dCarWidth/2 );

	}
	else
	{
		rectangleArea[0].setX( m_adjustPoint.getX() - (nCarNum/2+0.5)*m_dCarLength );
		rectangleArea[0].setY( m_adjustPoint.getY() + m_dCarWidth/2 );

		rectangleArea[1].setX( m_adjustPoint.getX() - (nCarNum/2-0.5)*m_dCarLength );
		rectangleArea[1].setY( m_adjustPoint.getY() + m_dCarWidth/2 );

		rectangleArea[2].setX( m_adjustPoint.getX() - (nCarNum/2-0.5)*m_dCarLength );
		rectangleArea[2].setY( m_adjustPoint.getY() - m_dCarWidth/2 );

		rectangleArea[3].setX( m_adjustPoint.getX() - (nCarNum/2+0.5)*m_dCarLength );
		rectangleArea[3].setY( m_adjustPoint.getY() - m_dCarWidth/2 );
	}
	
	
	m_cars[0]->SetCarCapacity( m_nCarCapacity );
	m_cars[0]->GetCarArea()->SetTerminal((Terminal *)m_pInTerm);
	m_cars[0]->GetCarArea()->initServiceLocation( 4, rectangleArea );
	m_cars[0]->ComputeDoorInfo( m_nEntryDoorNum,m_nExitDoorNum, m_nExitDoorType, m_dCarLength,m_dPlatformWidth );
	

	for( int i=1; i<nCarNum ;++i)
	{
		rectangleArea[0].DoOffset( m_dCarLength , 0,0 );
		rectangleArea[1].DoOffset( m_dCarLength , 0,0 );
		rectangleArea[2].DoOffset( m_dCarLength , 0,0 );
		rectangleArea[3].DoOffset( m_dCarLength , 0,0 );
		m_cars[i]->SetCarCapacity( m_nCarCapacity );
		m_cars[i]->GetCarArea()->SetTerminal((Terminal *)m_pInTerm);
		m_cars[i]->GetCarArea()->initServiceLocation( 4, rectangleArea );
		m_cars[i]->ComputeDoorInfo( m_nEntryDoorNum,m_nExitDoorNum, m_nExitDoorType, m_dCarLength,m_dPlatformWidth );
	}
	

	// platform area
	Point platFormBase;
	platFormBase.setX( m_adjustPoint.getX());
	double _y=m_adjustPoint.getY() - m_dCarWidth/2 - PLATFORM_CAR_DISTANCE ;
	platFormBase.setY( _y);

	rectangleArea[0].setX( platFormBase.getX() - m_dPlatformLength/2 );
	rectangleArea[0].setY( platFormBase.getY());

	rectangleArea[1].setX( platFormBase.getX() + m_dPlatformLength/2 );
	rectangleArea[1].setY( platFormBase.getY());

	rectangleArea[2].setX( platFormBase.getX() + m_dPlatformLength/2 );
	rectangleArea[2].setY( platFormBase.getY() - m_dPlatformWidth );

	
	rectangleArea[3].setX( platFormBase.getX() - m_dPlatformLength/2 );
	rectangleArea[3].setY( platFormBase.getY() - m_dPlatformWidth );

	m_preBoardingArea->initServiceLocation( 4, rectangleArea );

	return true;
}

void CStationLayout::SetCarCapacity( int _nNumber )
{
	m_nCarCapacity = _nNumber;
	CAR::const_iterator iter=m_cars.begin();
	CAR::const_iterator iterLast=m_cars.end();
	for(;iter!=iterLast;++iter)
	{
		(*iter)->SetCarCapacity( _nNumber );
	}
}