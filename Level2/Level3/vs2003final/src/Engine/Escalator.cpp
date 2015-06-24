// Escalator.cpp: implementation of the Escalator class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Escalator.h"
#include "engine\person.h"
#include "common\line.h"
#include "common\pollygon.h"
#include "inputs\miscproc.h"
#include "engine\procq.h"
#include "Engine\TerminalMobElementBehavior.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Escalator::Escalator()
{
	m_dWidth = 3 * SCALE_FACTOR;
	m_dMovingSpeed = 1.0;
	m_nCapacity = 50;	
}

Escalator::~Escalator()
{
}

//void Escalator::initServiceLocation (int pathCount, const Point *pointList)
//{
//	m_location.init (pathCount, pointList);
//}


//add by Mark Chen 02/04/2004
int Escalator::readSpecialField(ArctermFile& procFile)
{
	procFile.getFloat( m_dWidth );
	procFile.getFloat(m_dMovingSpeed );
	procFile.getInteger( m_nCapacity );
	return TRUE;
}
int Escalator::writeSpecialField(ArctermFile& procFile)const
{
	procFile.writeFloat( (float)m_dWidth );
	procFile.writeFloat( (float)m_dMovingSpeed);
	procFile.writeInt(m_nCapacity);
	return TRUE;
}
void Escalator::initServiceLocation (int pathCount, const Point *pointList)
{
	if (pathCount <= 1)
        throw new StringError ("Escalator processor must have at least 2 service points.");

	m_vEscalatorShape.clear();
	for( int i=0; i<pathCount; ++i )
	{
		PIPEPOINT tempPoint;
		tempPoint.m_point = pointList[ i ];
		tempPoint.m_width = m_dWidth;
		m_vEscalatorShape.push_back( tempPoint );
	}	
    m_location.init (pathCount, pointList);	
	UpdateMinMax();
}
void Escalator::SetWidth( DistanceUnit _dWidth ) 
{
	m_dWidth = _dWidth;
	int iSize = m_vEscalatorShape.size();
	for( int i=0; i<iSize; ++i )
	{
		m_vEscalatorShape[i].m_width = _dWidth;
	}

}

// fill the bisect line in the m_vEscalatorShape 
void Escalator::CalculateTheBisectLine()
{
	Point ptStart;
	Point ptMid;
	Point ptEnd;
	Point point1;
	Point point2;
	DistanceUnit dWidth;

	int nPointCount = m_vEscalatorShape .size();
	assert( nPointCount > 1 );

	// calculate the first line
	ptStart = m_vEscalatorShape [0].m_point;
	ptEnd = m_vEscalatorShape [1].m_point;
	Point centralLineVector( ptStart, ptEnd );
	Point perpVector1 = centralLineVector.perpendicularY();
	dWidth = m_vEscalatorShape [0].m_width;
	perpVector1.length( dWidth / 2.0 );
	point1 = perpVector1 + ptStart;
	Point perpVector2 = centralLineVector.perpendicular();
	perpVector2.length( dWidth / 2.0 );
	point2 = perpVector2 + ptStart;
	m_vEscalatorShape [0].m_bisectPoint1 = point1;
	m_vEscalatorShape [0].m_bisectPoint2 = point2;
	
	DistanceUnit dZ = ptStart.getZ();
	m_vEscalatorShape [0].m_bisectPoint1.setZ( dZ );
	m_vEscalatorShape [0].m_bisectPoint2.setZ( dZ );

	// calculate the middle line.
	for( int i=1; i<nPointCount-1; i++ )
	{
		ptStart = m_vEscalatorShape [i-1].m_point;
		ptMid = m_vEscalatorShape [i].m_point;
		dZ = ptMid.getZ();
		ptEnd = m_vEscalatorShape [i+1].m_point;
		Point vector1 = Point( ptMid, ptStart );
		Point vector2 = Point( ptMid, ptEnd );
		vector1.length( 1.0 );
		vector2.length( 1.0 );
		Point tempPt = ptMid + vector1;
		tempPt += vector2;
		Point resVector1 = Point( ptMid, tempPt );
		dWidth = m_vEscalatorShape [i].m_width;

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
		if( IsCrossOver( m_vEscalatorShape [i-1].m_bisectPoint1, m_vEscalatorShape [i-1].m_bisectPoint2, point1, point2 ) )
		{
			m_vEscalatorShape [i].m_bisectPoint1 = point2;
			m_vEscalatorShape [i].m_bisectPoint2 = point1;
		}
		else
		{
			m_vEscalatorShape [i].m_bisectPoint1 = point1;
			m_vEscalatorShape [i].m_bisectPoint2 = point2;
		}

		m_vEscalatorShape [i].m_bisectPoint1.setZ( dZ );
		m_vEscalatorShape [i].m_bisectPoint2.setZ( dZ );
	}
		

	// calculate the last line
	ptStart = m_vEscalatorShape [nPointCount-2].m_point;
	ptEnd = m_vEscalatorShape [nPointCount-1].m_point;
	dZ = ptEnd.getZ();
	centralLineVector = Point( ptStart, ptEnd );
	perpVector1 = centralLineVector.perpendicularY();
	dWidth = m_vEscalatorShape [nPointCount-1].m_width;
	perpVector1.length( dWidth / 2.0 );
	point1 = perpVector1 + ptEnd;
	perpVector2 = centralLineVector.perpendicular();
	perpVector2.length( dWidth / 2.0 );
	point2 = perpVector2 + ptEnd;
	m_vEscalatorShape [nPointCount-1].m_bisectPoint1 = point1;
	m_vEscalatorShape [nPointCount-1].m_bisectPoint2 = point2;

	m_vEscalatorShape [nPointCount-1].m_bisectPoint1.setZ( dZ );
	m_vEscalatorShape [nPointCount-1].m_bisectPoint2.setZ( dZ );
/*
	for(  i=0; i<nPointCount; ++i )
	{
		// TRACE("\n z1= %.2f; z2= %.2f; z=%.2f\n", m_vEscalatorShape [i].m_bisectPoint1.getZ(),m_vEscalatorShape [i].m_bisectPoint2.getZ(),m_vEscalatorShape [i].m_point.getZ() );
	}
	*/
}
// check if ( _pt11, _pt21 ) X ( _pt12, _pt22 )
bool Escalator::IsCrossOver( Point _pt11, Point _pt12, Point _pt21, Point _pt22 )
{
	Line line1( _pt11, _pt21 );
	Line line2( _pt12, _pt22 );
	return line1.intersects( line2) == TRUE ? true : false;
}

//get all region the conveyor covered, it consisted of a list of pollygon.
void Escalator::GetCoveredRegion( POLLYGONVECTOR& _regions ) const
{
	_regions.clear();
	int iPipePointCount = m_vEscalatorShape .size();
	if( iPipePointCount <2 )
			return ;

	//POLLYGONVECTOR tempVector;=
	PIPESHAPE::const_iterator iter = m_vEscalatorShape .begin();
	PIPESHAPE::const_iterator iterNext = iter;
	++iterNext;
	PIPESHAPE::const_iterator iterEnd = m_vEscalatorShape .end();
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

void Escalator::beginService (Person *_pPerson, ElapsedTime _curTime)
{
		
//	// TRACE ("\n%s\n",_curTime.printTime() );
	ElapsedTime eventTime = _curTime;
	int nCount = m_location.getCount();
	//// TRACE(_pPerson->getProcessor()->getID()->GetIDString());
	int nn = (int)m_location.getPoint( m_location.getCount() -1 ).getZ();
    //// for only one direction
//	if( _pPerson->GetSourceFloor() == m_location.getPoint(0).getZ() / SCALE_FACTOR )
//	{
		for( int i=0; i<nCount; ++i )
		{
//			_pPerson->setDestination( m_location.getPoint( i ) );
//			eventTime += _pPerson->moveTime();
//			_pPerson->writeLogEntry( eventTime );
			Point ptDest = m_location.getPoint( i );			
			_pPerson->setTerminalDestination( ptDest  );
			if (m_dMovingSpeed <= 0) 
			{
				eventTime += _pPerson->moveTime();
			}
			else
			{
				eventTime += _pPerson->moveTime( m_dMovingSpeed* SCALE_FACTOR, FALSE);
				
			}
			_pPerson->writeLogEntry( eventTime, false );
		}
// 	}

//	else if(_pPerson->GetSourceFloor() == m_location.getPoint( m_location.getCount() -1 ).getZ() / SCALE_FACTOR )
//	{
//		// TRACE("asf");
//		for( int i=nCount-1; i >= 0; --i )
//		{
////			_pPerson->setDestination( m_location.getPoint( i ) );
////			eventTime += _pPerson->moveTime();
////			_pPerson->writeLogEntry( eventTime );
//			Point ptDest = m_location.getPoint( i );			
//			_pPerson->setDestination( ptDest  );
//			if (m_dMovingSpeed <= 0) 
//			{
//				eventTime += _pPerson->moveTime();
//			}
//			else
//			{
//				eventTime += _pPerson->moveTime( m_dMovingSpeed, FALSE);
//				
//			}
//			_pPerson->writeLogEntry( eventTime );				
//		}
//	}
	//// TRACE ("\n%s\n",eventTime.printTime() );
	_pPerson->setState( LeaveServer );
	_pPerson->generateEvent( eventTime ,false);
}

void  Escalator::initSpecificMisc (const MiscData *miscData) 
{
	if( miscData )
	{
		MiscEscalatorData* pMiscData = (MiscEscalatorData*)miscData;
		m_dMovingSpeed = pMiscData->GetMovingSpeed();
		//m_nCapacity = (int)pMiscData->GetCapacity();
	}
	else
	{
		m_dMovingSpeed = 1.0 ;
		m_nCapacity = 5 ;
	}
	
}

Point Escalator::GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime)
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

bool Escalator::CheckIfOnTheSameFloor(int _iCurFloor)
{
	return ((getServicePoint( 0 ).getZ()/ SCALE_FACTOR ==_iCurFloor)
		||(getServicePoint(serviceLocationLength()-1).getZ() / SCALE_FACTOR == _iCurFloor));
}

Processor* Escalator::CreateNewProc()
{
	Processor* pProc = new Escalator;
	return pProc;
}

bool Escalator::CopyInConstraint(Processor* _pDestProc)
{
	int nPSLCount = _pDestProc->serviceLocationPath()->getCount();
	ASSERT(nPSLCount > 0);
	DistanceUnit zPos = _pDestProc->serviceLocationPath()->getPoint(0).getZ();

	int nPICCount = inConstraint()->getCount();
	if( nPICCount> 0)
	{
		Point* pPointIC = new Point[nPICCount];
		memcpy(pPointIC, inConstraint()->getPointList(), sizeof(Point)*nPICCount);
		for(int i = 0; i < nPICCount; i++)
			pPointIC[i].setZ(zPos);

		_pDestProc->initInConstraint( nPICCount, pPointIC);
		delete[] pPointIC;
	}

	return true;
}

bool Escalator::CopyOutconstraint(Processor* _pDestProc)
{
	int nPSLCount = _pDestProc->serviceLocationPath()->getCount();
	ASSERT(nPSLCount > 0);
	DistanceUnit zPos = _pDestProc->serviceLocationPath()->getPoint(nPSLCount-1).getZ();

	int nPOCCount = outConstraint()->getCount();
	if(nPOCCount > 0)
	{
		Point* pPointOC = new Point[nPOCCount];
		memcpy(pPointOC, outConstraint()->getPointList(), sizeof(Point)*nPOCCount);
		for(int i = 0; i < nPOCCount; i++)
			pPointOC[i].setZ(zPos);
		_pDestProc->initOutConstraint( nPOCCount, pPointOC);
		delete[] pPointOC;
	}

	return true;
}

bool Escalator::CopyQueue(Processor* _pDestProc)
{
	int nPSLCount = _pDestProc->serviceLocationPath()->getCount();
	ASSERT(nPSLCount > 0);
	DistanceUnit zPos = _pDestProc->serviceLocationPath()->getPoint(0).getZ();

	int nPQCount = queueLength();
	if(nPQCount> 0)
	{
		Point* pPointQ=new Point[nPQCount];
		memcpy(pPointQ, queuePath()->getPointList(), sizeof(Point)*nPQCount);
		for(int i = 0; i < nPQCount; i++)
			pPointQ[i].setZ(zPos);
		_pDestProc->initQueue( FALSE, nPQCount, pPointQ);
		delete[] pPointQ;
	}

	return true;
}

bool Escalator::CopyOtherData(Processor* _pDestProc)
{
	((Escalator*)_pDestProc)->SetWidth(GetWidth());

	return true;
}

//check the queue/in constraint and the first point of service location on the same floor
//check the out constraint and the end point of the service location on the same floor
//if not ,rectify it
void Escalator::CheckAndRectifyQueueInOutConstraint()
{
	CheckAndRectifyQueueLocation();
	CheckAndRectifyInConstraintLocation();
	CheckAndRectifyOutConstraintLocation();
}

bool Escalator::CheckAndRectifyQueueLocation()
{
	int nServiceFloor = 0;

	Path* pPath = serviceLocationPath();
	if(pPath)
	{
		if(pPath->getCount() <= 0)
			return true;
		nServiceFloor = int(pPath->getPoint(0).getZ() / SCALE_FACTOR);
	}
	

	int nQueueFloor = -1;
	pPath = queuePath();
	if( pPath )
	{
		for( int i=0; i<pPath->getCount(); i++ )
		{
			Point pt = pPath->getPoint(i);
			nQueueFloor = int(pt.getZ() / SCALE_FACTOR);

		}
	}
	else
	{
		return true;
	}
	
	if(nServiceFloor != nQueueFloor)
	{
		pPath = queuePath();
		if (pPath)
		{
			Point *pPoint = queuePath()->getPointList();
			if( pPoint )
			{
				for( int i=0; i<queuePath()->getCount(); i++ )
				{
					pPoint[i].setZ(nServiceFloor * SCALE_FACTOR);
				}
			}
		}
	}
	return true;
}
//check the in constraint and service location floor
bool Escalator::CheckAndRectifyInConstraintLocation()
{
	int nServiceFloor = 0;

	Path* pPath = serviceLocationPath();
	if(pPath)
	{
		if(pPath->getCount() <= 0)
			return true;
		nServiceFloor = int(pPath->getPoint(0).getZ() / SCALE_FACTOR);
	}



	int nInconstraintFloor = -1;

	pPath = inConstraint();
	if( pPath )
	{
		for( int i=0; i<pPath->getCount(); i++ )
		{
			Point pt = pPath->getPoint(i);
			nInconstraintFloor = int(pt.getZ() / SCALE_FACTOR);
		}
	}
	else
	{
		return true;
	}

	if(nServiceFloor != nInconstraintFloor)
	{
		pPath = inConstraint();
		if (pPath)
		{
			Point *pPoint = inConstraint()->getPointList();
			if( pPoint )
			{
				for( int i=0; i<inConstraint()->getCount(); i++ )
				{
					pPoint[i].setZ(nServiceFloor * SCALE_FACTOR);
				}
			}
		}
	}

	return true;
}
//check the out constraint and the serve loacation wether in the same floor
bool Escalator::CheckAndRectifyOutConstraintLocation()
{
	//get the service location floor
	int nServiceFloor = 0;
	
	Path* pPath = serviceLocationPath();
	if(pPath)
	{
		int nCount = pPath->getCount();
		if(nCount <= 0)
			return true;
		nServiceFloor = int(pPath->getPoint(nCount -1).getZ() / SCALE_FACTOR);
	}


	//get the out constraint floor
	int nOutConstraintFloor = -1;

	pPath = outConstraint();
	if( pPath )
	{
		for( int i=0; i<pPath->getCount(); i++ )
		{
			Point pt = pPath->getPoint(i);
			nOutConstraintFloor = int(pt.getZ() / SCALE_FACTOR);
		}
	}

	//verify
	if(nServiceFloor != nOutConstraintFloor)
	{
		pPath  = outConstraint();
		if (pPath)
		{
			Point *pPoint = outConstraint()->getPointList();
			if( pPoint )
			{
				for( int i=0; i<outConstraint()->getCount(); i++ )
				{
					pPoint[i].setZ(nServiceFloor * SCALE_FACTOR);
				}
			}
		}
	}

	return true;
}