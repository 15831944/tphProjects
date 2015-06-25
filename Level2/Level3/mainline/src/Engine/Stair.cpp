// Stair.cpp: implementation of the Stair class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Stair.h"
#include "engine\person.h"
#include "common\line.h"
#include "common\pollygon.h"
#include "engine\procq.h"
#include "TerminalMobElementBehavior.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Stair::Stair()
{
	m_dWidth = 3 * SCALE_FACTOR;
}

Stair::~Stair()
{

}
int Stair::readSpecialField(ArctermFile& procFile)
{
	procFile.getFloat( m_dWidth );
	return TRUE;
}
int Stair::writeSpecialField(ArctermFile& procFile)const
{
	procFile.writeFloat( (float)m_dWidth );
	return TRUE;
}
void Stair::initServiceLocation (int pathCount, const Point *pointList)
{
	if (pathCount <= 1)
        throw new StringError ("Stair processor must have at least 2 service points.");

	m_vStairShape.clear();
	for( int i=0; i<pathCount; ++i )
	{
		PIPEPOINT tempPoint;
		tempPoint.m_point = pointList[ i ];
		tempPoint.m_width = m_dWidth;
		m_vStairShape.push_back( tempPoint );
	}	
    m_location.init (pathCount, pointList);	
}
void Stair::SetWidth( DistanceUnit _dWidth ) 
{
	m_dWidth = _dWidth;
	int iSize = m_vStairShape.size();
	for( int i=0; i<iSize; ++i )
	{
		m_vStairShape[i].m_width = _dWidth;
	}

}

// fill the bisect line in the m_vStairShape 
void Stair::CalculateTheBisectLine()
{
	Point ptStart;
	Point ptMid;
	Point ptEnd;
	Point point1;
	Point point2;
	DistanceUnit dWidth;

	int nPointCount = m_vStairShape .size();
	assert( nPointCount > 1 );

	// calculate the first line
	ptStart = m_vStairShape [0].m_point;
	ptEnd = m_vStairShape [1].m_point;
	Point centralLineVector( ptStart, ptEnd );
	Point perpVector1 = centralLineVector.perpendicularY();
	dWidth = m_vStairShape [0].m_width;
	perpVector1.length( dWidth / 2.0 );
	point1 = perpVector1 + ptStart;
	Point perpVector2 = centralLineVector.perpendicular();
	perpVector2.length( dWidth / 2.0 );
	point2 = perpVector2 + ptStart;
	m_vStairShape [0].m_bisectPoint1 = point1;
	m_vStairShape [0].m_bisectPoint2 = point2;
	
	DistanceUnit dZ = ptStart.getZ();
	m_vStairShape [0].m_bisectPoint1.setZ( dZ );
	m_vStairShape [0].m_bisectPoint2.setZ( dZ );

	// calculate the middle line.
	for( int i=1; i<nPointCount-1; i++ )
	{
		ptStart = m_vStairShape [i-1].m_point;
		ptMid = m_vStairShape [i].m_point;
		dZ = ptMid.getZ();
		ptEnd = m_vStairShape [i+1].m_point;
		Point vector1 = Point( ptMid, ptStart );
		Point vector2 = Point( ptMid, ptEnd );
		vector1.length( 1.0 );
		vector2.length( 1.0 );
		Point tempPt = ptMid + vector1;
		tempPt += vector2;
		Point resVector1 = Point( ptMid, tempPt );
		dWidth = m_vStairShape [i].m_width;

		// now calculate the width.
		double dCosVal = vector1.GetCosOfTwoVector( vector2 );
		double deg = acos( dCosVal );
		deg /= 2.0;
		dWidth /= sin( deg );

		resVector1.length( dWidth / 2.0 );
		Point resVector2 = Point( tempPt, ptMid );		
		resVector2.length( dWidth / 2.0 );
		point1 = ptMid + resVector1;
		point2 = ptMid + resVector2;
		// CHECK THE ORDER OF POINT 1 and POINT 2
		if( IsCrossOver( m_vStairShape [i-1].m_bisectPoint1, m_vStairShape [i-1].m_bisectPoint2, point1, point2 ) )
		{
			m_vStairShape [i].m_bisectPoint1 = point2;
			m_vStairShape [i].m_bisectPoint2 = point1;
		}
		else
		{
			m_vStairShape [i].m_bisectPoint1 = point1;
			m_vStairShape [i].m_bisectPoint2 = point2;
		}

		m_vStairShape [i].m_bisectPoint1.setZ( dZ );
		m_vStairShape [i].m_bisectPoint2.setZ( dZ );
	}
		

	// calculate the last line
	ptStart = m_vStairShape [nPointCount-2].m_point;
	ptEnd = m_vStairShape [nPointCount-1].m_point;
	dZ = ptEnd.getZ();
	centralLineVector = Point( ptStart, ptEnd );
	perpVector1 = centralLineVector.perpendicularY();
	dWidth = m_vStairShape [nPointCount-1].m_width;
	perpVector1.length( dWidth / 2.0 );
	point1 = perpVector1 + ptEnd;
	perpVector2 = centralLineVector.perpendicular();
	perpVector2.length( dWidth / 2.0 );
	point2 = perpVector2 + ptEnd;
	m_vStairShape [nPointCount-1].m_bisectPoint1 = point1;
	m_vStairShape [nPointCount-1].m_bisectPoint2 = point2;

	m_vStairShape [nPointCount-1].m_bisectPoint1.setZ( dZ );
	m_vStairShape [nPointCount-1].m_bisectPoint2.setZ( dZ );
/*
	for(  i=0; i<nPointCount; ++i )
	{
		//// TRACE("\n z1= %.2f; z2= %.2f; z=%.2f\n", m_vStairShape [i].m_bisectPoint1.getZ(),m_vStairShape [i].m_bisectPoint2.getZ(),m_vStairShape [i].m_point.getZ() );
	}
	*/
}
// check if ( _pt11, _pt21 ) X ( _pt12, _pt22 )
bool Stair::IsCrossOver( Point _pt11, Point _pt12, Point _pt21, Point _pt22 )
{
	Line line1( _pt11, _pt21 );
	Line line2( _pt12, _pt22 );
	return line1.intersects( line2) == TRUE ? true : false;
}

//get all region the conveyor covered, it consisted of a list of pollygon.
void Stair::GetCoveredRegion( POLLYGONVECTOR& _regions ) const
{
	_regions.clear();
	int iPipePointCount = m_vStairShape .size();
	if( iPipePointCount <2 )
			return ;

	//POLLYGONVECTOR tempVector;=
	PIPESHAPE::const_iterator iter = m_vStairShape .begin();
	PIPESHAPE::const_iterator iterNext = iter;
	++iterNext;
	PIPESHAPE::const_iterator iterEnd = m_vStairShape .end();
	for(int i=0; iterNext != iterEnd ; ++iter,++iterNext,++i )
	{
		Pollygon temp;
		Point vertex[4];
		vertex[0] = iter->m_bisectPoint1;
		vertex[1] = iter->m_bisectPoint2;
		vertex[2] = iterNext->m_bisectPoint2;
		vertex[3] = iterNext->m_bisectPoint1;
		temp.init( 4, vertex );
		//GetSiglePollygon( *iter, *iterNext, temp );
		_regions.push_back( temp );
	}
}

void Stair::beginService (Person *_pPerson, ElapsedTime _curTime)
{
		
//	// TRACE ("\n%s\n",_curTime.printTime() );
     

	ElapsedTime eventTime = _curTime;
	int nCount = m_location.getCount();
   // for bidirectional
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();

	if (spTerminalBehavior)
	{
		if( spTerminalBehavior->GetSourceFloor() == m_location.getPoint(0).getZ() / SCALE_FACTOR )
		{	//first point is arriving point
			_pPerson->setState( ArriveAtServer );
			for( int i=0; i<nCount; ++i )
			{
				_pPerson->setTerminalDestination( m_location.getPoint( i ) );
				eventTime += _pPerson->moveTime();
				_pPerson->writeLogEntry( eventTime, false );
				_pPerson->setState(FreeMoving);
			}
		}
		else if( spTerminalBehavior->GetSourceFloor() == m_location.getPoint( m_location.getCount() -1 ).getZ() / SCALE_FACTOR)
		{	
			_pPerson->setState( ArriveAtServer );
			for( int i=nCount-1; i >= 0; --i )
			{
				_pPerson->setTerminalDestination( m_location.getPoint( i ) );
				eventTime += _pPerson->moveTime();
				_pPerson->writeLogEntry( eventTime, false );
				_pPerson->setState(FreeMoving);
			}
		}
	}
	
	//// TRACE ("\n%s\n",eventTime.printTime() );
	_pPerson->setState( LeaveServer );
	_pPerson->generateEvent( eventTime,false );
}
Point Stair::GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime)
{
	if(getServicePoint( 0 ).getZ()/ SCALE_FACTOR ==_iCurFloor)
	{
		if( In_Constr.getCount() )
		    return In_Constr.getPoint( 0 );
		if( m_pQueue != NULL )
			return	m_pQueue->corner( m_pQueue->cornerCount() - 1 );
		return getServicePoint( 0 );
	}
	else if(getServicePoint(serviceLocationLength()-1).getZ() / SCALE_FACTOR == _iCurFloor)
	{
	  return getServicePoint(serviceLocationLength()-1);
	}
	else
	{
		throw new ARCFlowError (_strMobType, this->getID()->GetIDString(),"not the same Floor Problem! ",_strTime);
	}
}
bool Stair::CheckIfOnTheSameFloor(int _iCurFloor)
{
	return ((getServicePoint( 0 ).getZ()/ SCALE_FACTOR ==_iCurFloor)
		||(getServicePoint(serviceLocationLength()-1).getZ() / SCALE_FACTOR == _iCurFloor));
}

Processor* Stair::CreateNewProc()
{
	Processor* pProc = new Stair;
	return pProc;
}

bool Stair::CopyOtherData(Processor* _pDestProc)
{
	((Stair*)_pDestProc)->SetWidth(GetWidth());

	return true;
}

Point Stair::AcquireServiceLocation( Person* _pPerson )
{
	int nCount = m_location.getCount();
	// for bidirectional
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();

	if (spTerminalBehavior)
	{
		if( spTerminalBehavior->GetSourceFloor() == m_location.getPoint(0).getZ() / SCALE_FACTOR )
		{
			return m_location.getPoint( 0 ) ;
		}
		else if( spTerminalBehavior->GetSourceFloor() == m_location.getPoint( m_location.getCount() -1 ).getZ() / SCALE_FACTOR)
		{
			return m_location.getPoint( m_location.getCount() -1 ) ;
		}
	}

	return GetServiceLocation();
}
//Point Stair::GetPipeExitPoint(int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob)
//{
//	return GetEntryPoint(terminalMob->getPersonErrorMsg(),iCurFloor,_curTime);
//}