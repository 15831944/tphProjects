#include "StdAfx.h"
#include ".\pollygon2008.h"
#include "stdafx.h"
#include <stdlib.h>
#include "pollygon2008.h"
#include "point2008.h"
#include "line2008.h"
#include "util.h"

CPollygon2008::CPollygon2008(void)
{
}

CPollygon2008::~CPollygon2008(void)
{
}

// constructor that allocates CPoint2008 list and determines bounding box
void CPollygon2008::init (int pointCount, const CPoint2008 *pointList)
{
	CPath2008::init (pointCount, pointList);

	// calculate bounding box;
	SetBoundsPoint();

}
void CPollygon2008::SetBoundsPoint()
{
	bottomLeft = points[0];
	topRight = points[0];
	for (short i = 1; i < getCount(); i++)
	{
		bottomLeft = bottomLeft.getMinPoint (points[i]);
		topRight = topRight.getMaxPoint (points[i]);
	}
}

void CPollygon2008::GetBoundsPoint(CPoint2008& ptBottomLeft,CPoint2008& ptTopRight)const
{
	ptBottomLeft = bottomLeft;
	ptTopRight = topRight;
}

void CPollygon2008::scale (double sx, double sy, double sz )
{
	for (int i = 0; i < getCount(); i++)
	{
		points[ i ].scale( sx, sy, sz );
	}
}
// use even_odd rule to determine if the CPollygon2008 contains p
int CPollygon2008::contains (const CPoint2008& pnt) const
{  
	if (getCount() <= 2 || pnt.getZ() != bottomLeft.getZ())
		return 0;

	// first check bounding box
	CLine2008 testLine, polySegment;
	testLine.init (bottomLeft, topRight);
	if (!testLine.contains (pnt))
		return 0;

	// create a CLine2008 from pnt to outside of bounding box
	CPoint2008 tempPoint (topRight + CPoint2008 (100.0, 100.0, 0.0));
	CPoint2008 intersect;
	testLine.init (pnt, tempPoint);

	// count number of intersections with testLine and polygon sides
	int intersectCount = 0, cornerCount = 0;
	for (int i = 0; i < getCount() - 1; i++)
	{
		polySegment.init (points[i], points[i+1]);
		if (testLine.mayIntersect (polySegment))
		{
			intersect = testLine.intersection (polySegment);
			if (testLine.contains (intersect) && polySegment.contains (intersect))
			{
				if (testLine.isCornerPoint (intersect) || polySegment.isCornerPoint (intersect))
					cornerCount++;
				intersectCount++;
			}
		}
	}

	polySegment.init (points[getCount()-1], points[0]);
	if (testLine.mayIntersect (polySegment))
	{
		intersect = testLine.intersection (polySegment);
		if (testLine.contains (intersect) && polySegment.contains (intersect))
		{
			if (testLine.isCornerPoint (intersect) || polySegment.isCornerPoint (intersect))
				cornerCount++;
			intersectCount++;
		}
	}

	if (cornerCount & 1)
		return 1;

	if (cornerCount)
		intersectCount -= (int) (cornerCount / 2);

	// if intersectCount is odd, the Polgyon contains pnt
	return (intersectCount & 1);
}

int CPollygon2008::containsExcludeEdge( const CPoint2008& _pt) const
{
	if( contains( _pt) )
	{
		int _iSeg;
		return !isEdgePoint( _pt, _iSeg );
	}
	else
	{
		return 0;
	}
}

// test a point if is a edge point
bool CPollygon2008::isEdgePoint( const CPoint2008& _pt, int& _iSeg ) const
{
	CLine2008 _polySegment;
	for( int i=0; i<getCount(); i++ )
	{	
		int _iMod = (i+1)%getCount();
		_polySegment.init( points[i], points[_iMod] );
		if( _polySegment.comprisePt( _pt ) )
		{

			if( _pt.preciseCompare( points[ _iMod ] ) )
				_iSeg = _iMod;
			else
				_iSeg = i;

			return true;
		}
	}
	return false;
}

int CPollygon2008::intersects (const CPoint2008& p1, const CPoint2008& p2) const
{
	if (getCount() < 2 || p1.getZ() != bottomLeft.getZ())
		return 0;

	CLine2008 testLine (p1, p2);
	CLine2008 polySegment (points[0], points[getCount()-1]);

	// test closing segment
	if (getCount() > 2 && testLine.intersects (polySegment))
		return TRUE;

	// test each segment for intersection
	for (short i = 0; i < getCount() - 1; i++)
	{
		polySegment.init (points[i], points[i+1]);
		if (testLine.intersects (polySegment))
			return TRUE;
	}
	return FALSE;
}

//Returns the area of the polygon in meters squared.
double CPollygon2008::calculateArea (void)
{
	double area = getArea();
	if (area < 0){
		m_bcountClockwise=false;
		return -area;
	}else {
		m_bcountClockwise=true;
		return area;
	}

}

// returns a random point within the polygon
CPoint2008 CPollygon2008::getRandPoint (DistanceUnit) const
{
	CPoint2008 newPoint (0.0, 0.0, bottomLeft.getZ());
	double xProb, yProb;
	DistanceUnit width = topRight.getX() - bottomLeft.getX();
	DistanceUnit height = topRight.getY() - bottomLeft.getY();

	do {
		// get 2 random numbers between 0 and 1
		xProb = (double)rand() / RAND_MAX;
		yProb = (double)rand() / RAND_MAX;

		// create random point
		newPoint.setX (bottomLeft.getX() + (xProb * width));
		newPoint.setY (bottomLeft.getY() + (yProb * height));

		// if new point is not within polygon area, repeat
	} while (!contains (newPoint));

	return newPoint;
}


// do offset to all the position related data.
void CPollygon2008::DoOffset( DistanceUnit _xOffset,  DistanceUnit _yOffset,  DistanceUnit _zOffset )
{
	CPath2008::DoOffset( _xOffset, _yOffset, _zOffset );
	bottomLeft.DoOffset( _xOffset, _yOffset, _zOffset );
	topRight.DoOffset( _xOffset, _yOffset, _zOffset );
}
void CPollygon2008::Rotate (DistanceUnit degrees)
{
	CPath2008::Rotate( degrees );
	bottomLeft.rotate( degrees );
	topRight.rotate( degrees );
	SetBoundsPoint();
}
bool CPollygon2008::IsOverLap( const CPoint2008& p1, const CPoint2008& p2) const
{
	if( contains2( p1 ) )
		return true;
	if( contains2(p2) )
		return true;
	if( intersects( p1, p2 ) )
		return true;
	else
		return false;
}


bool CPollygon2008::contains ( const CPollygon2008& _another ) const
{
	int iAnotherPointCount = _another.getCount();
	if( iAnotherPointCount < 1 )
		return false;
	for( int i=0; i<iAnotherPointCount ; ++i )
	{
		if( !contains2( _another.getPoint( i ) ) )
			return false;
	}


	return true;
}
// test if this pollygon is a contex pollygon
bool CPollygon2008::IsAConvexPollygon()const
{
	ASSERT( getCount() >= 3 );
	int iPlusCount=0;
	int iMinusCount=0;
	for( int i=0,j=2; j< getCount(); ++j,++i )
	{
		CPoint2008 temp1;
		temp1.initVector( points[i], points[i+1]);

		CPoint2008 temp2;
		temp2.initVector( points[i+1], points[j]);
		CPoint2008 result;
		result.Vector1XVector2( temp1, temp2 );
		if( result.getZ() > 0 )
		{
			++iPlusCount;
		}
		else
		{
			++iMinusCount;
		}
		if( iPlusCount && iMinusCount )
		{
			return false;
		}
	}

	return true;
}
bool CPollygon2008::IfLineInsidePollygon( const CPoint2008& p1, const CPoint2008& p2 )const
{
	ASSERT( getCount() >= 3 );
	if( intersects( p1, p2 ) )
	{
		return false;	
	}
	else
	{
		CLine2008 tempLine( p1,p2 );
		CPoint2008 centerPoint;
		tempLine.GetCenterPoint( centerPoint );
		// don not think of Z 
		centerPoint.setZ( bottomLeft.getZ() );

		if( contains( centerPoint ) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
void CPollygon2008::DividAsConvexPollygons( std::vector<CPollygon2008>& _vResult )const
{
	ASSERT( getCount() >= 3 );
	if( IsAConvexPollygon() )
	{
		_vResult.push_back( *this );
	}
	else
	{
		CPollygon2008 reMainPollygon = *this;
		//reMainPollygon = reMainPollygon.init( this->getCount(), this->getpo );
		//reMainPollygon.
		//reMainPollygon.bottomLeft.setZ( points[0].getZ() );

		CPoint2008 tempTrianglePoint [ 3 ];
		CPollygon2008 tempTriangle;

		bool bNotEnd = true;
		int iStartPos =0;
		while( bNotEnd   )
		{
			if ( iStartPos >= reMainPollygon.getCount()-2 )
				return;

			tempTrianglePoint[0] = reMainPollygon.getPoint( iStartPos );
			tempTrianglePoint[1] = reMainPollygon.getPoint( iStartPos + 1 );
			tempTrianglePoint[2] = reMainPollygon.getPoint( iStartPos + 2 );

			if( reMainPollygon.IfLineInsidePollygon( tempTrianglePoint[0], tempTrianglePoint[2] )  )
			{
				tempTriangle.init( 3, tempTrianglePoint );
				_vResult.push_back( tempTriangle);
				reMainPollygon.RemovePointAt( iStartPos +1 );
				reMainPollygon.SetBoundsPoint();
				iStartPos = 0;
				if( reMainPollygon.IsAConvexPollygon() )
				{
					_vResult.push_back( reMainPollygon );
					bNotEnd = false;
				}
			}
			else
			{
				++iStartPos ;	

			}				
		}
	}
}

void CPollygon2008::Zoom(DistanceUnit _dX, DistanceUnit _dY, DistanceUnit _dZ  )
{
	if( getCount() > 3 )
	{
		int iIdx = getCount() - 2;
		CLine2008 temp1( points[0],points[ iIdx ] );
		CLine2008 temp2(  points[1],points[ iIdx +1] );
		CPoint2008 ptIntersection = temp1.intersection( temp2);
		DoOffset( ptIntersection.getX(),ptIntersection.getY(),0.0 );
		scale( _dX,_dY ,_dZ );
		DoOffset( -ptIntersection.getX(),-ptIntersection.getY(),0.0 );


		CLine2008 temp3( points[0],points[iIdx] );
		CLine2008 temp4(  points[1],points[iIdx+1] );
		CPoint2008 ptIntersection2 = temp3.intersection( temp4);

		DoOffset( ptIntersection.getX() - ptIntersection2.getX() ,ptIntersection.getY() -  ptIntersection2.getY(),0.0 );

	}
	else
	{
		CPoint2008 ptCenterPt12;
		DistanceUnit dX = points[1].getX() + ( points[2].getX() - points[1].getX() ) /2.0;
		DistanceUnit dY = points[1].getY() + ( points[2].getY() - points[1].getY() ) /2.0;
		ptCenterPt12.setPoint( dX, dY ,points[1].getZ() );

		CPoint2008 ptCenterPt20;
		dX = points[2].getX() + ( points[2].getX() - points[0].getX() ) /2.0;
		dY = points[2].getY() + ( points[2].getY() - points[0].getY() ) /2.0;

		ptCenterPt20.setPoint( dX, dY ,points[2].getZ() );

		CLine2008 temp1( points[0],ptCenterPt12 );
		CLine2008 temp2( points[1],ptCenterPt20 );
		CPoint2008 ptIntersection = temp1.intersection( temp2);
		DoOffset( ptIntersection.getX(),ptIntersection.getY(),0.0 );
		scale( _dX,_dY ,_dZ );
		DoOffset( -ptIntersection.getX(),-ptIntersection.getY(),0.0 );


		dX = points[1].getX() + ( points[2].getX() - points[1].getX() ) /2.0;
		dY = points[1].getY() + ( points[2].getY() - points[1].getY() ) /2.0;
		ptCenterPt12.setPoint( dX, dY ,points[1].getZ() );


		dX = points[2].getX() + ( points[2].getX() - points[0].getX() ) /2.0;
		dY = points[2].getY() + ( points[2].getY() - points[0].getY() ) /2.0;	
		ptCenterPt20.setPoint( dX, dY ,points[2].getZ() );

		CLine2008 temp3( points[0], ptCenterPt12 );
		CLine2008 temp4( points[1], ptCenterPt20 );

		CPoint2008 ptIntersection2 = temp3.intersection( temp4);

		DoOffset( ptIntersection.getX() - ptIntersection2.getX() ,ptIntersection.getY() -  ptIntersection2.getY(),0.0 );

	}

}

bool CPollygon2008::IfOverlapWithOtherPolygon( const CPollygon2008& _otherPlygon )const
{
	int iPointCount = _otherPlygon.getCount();
	for( int i=0,j=1; j<iPointCount; ++i,++j )
	{
		if( IsOverLap( _otherPlygon.getPoint( i ),_otherPlygon.getPoint( j ) ) )
			return true;
	}

	return false;
}

double CPollygon2008::getArea() const
{
	double area = 0.0;
	double ax, ay, bx, by;
	bx = points[0].getX() - points[getCount() - 1].getX();
	by = points[0].getY() - points[getCount() - 1].getY();
	for (short i = 0; i < getCount() - 2; i++)
	{
		ax = bx;
		ay = by;
		bx = points[i + 1].getX() - points[getCount() - 1].getX();
		by = points[i + 1].getY() - points[getCount() - 1].getY();
		area += (ax * by - ay * bx);    //twice area of triangle in cm^2
	}
	area *= 0.00005;    //divide by two, convert to m^2
	return area;
}

bool CPollygon2008::IsCountClock() const
{
	return getArea()>=0;
}

bool CPollygon2008::contains2( const CPoint2008& pt ) const
{
	if(getCount()<=2)
		return false;
	
	if(!(bottomLeft.x<=pt.x && pt.x <= topRight.x 
		&& bottomLeft.y <= pt.y && pt.y<= topRight.y ) )
	{
		return false;
	}

	int nIntersectCnt =0;
	for(int i =0;i<getCount();i++)
	{
		const CPoint2008& p1 = points[i];
		const CPoint2008& p2 = points[(i+1)%getCount()];
		double miny = MIN(p1.y,p2.y);
		double maxy = MAX(p1.y,p2.y);
		double dpy = maxy-miny;
		if( miny<pt.y && pt.y<=maxy )
		{
			double dr = (pt.y-p1.y)/(p2.y-p1.y);			
			{
				double rx = p1.x+ (p2.x-p1.x)*dr;
				if(rx>pt.x)
					nIntersectCnt++;
			}
		}		
	}
	

	return nIntersectCnt&1;
}