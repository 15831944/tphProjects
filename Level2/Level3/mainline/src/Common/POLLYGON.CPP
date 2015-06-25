#include "stdafx.h"
#include <stdlib.h>
#include "common\pollygon.h"
#include "common\point.h"
#include "common\line.h"
#include "util.h"


// constructor that allocates Point list and determines bounding box
void Pollygon::init (int pointCount, const Point *pointList)
{
    Path::init (pointCount, pointList);

	// calculate bounding box;
	SetBoundsPoint();

}
void Pollygon::SetBoundsPoint()
{
	bottomLeft = points[0];
    topRight = points[0];
    for (short i = 1; i < numPoints; i++)
    {
        bottomLeft = bottomLeft.getMinPoint (points[i]);
        topRight = topRight.getMaxPoint (points[i]);
    }
}

void Pollygon::scale (double sx, double sy, double sz )
{
	for (int i = 0; i < numPoints; i++)
    {
        points[ i ].scale( sx, sy, sz );
    }
}
// use even_odd rule to determine if the Pollygon contains p
int Pollygon::contains (const Point& pnt) const
{  
	 if (numPoints <= 2 || pnt.getZ() != bottomLeft.getZ())
		return 0;

	// first check bounding box
    Line testLine, polySegment;
    testLine.init (bottomLeft, topRight);
    if (!testLine.contains (pnt))
		return 0;

    // create a Line from pnt to outside of bounding box
    Point tempPoint (topRight + Point (100.0, 100.0, 0.0));
    Point intersect;
    testLine.init (pnt, tempPoint);

    // count number of intersections with testLine and polygon sides
    int intersectCount = 0, cornerCount = 0;
    for (int i = 0; i < numPoints - 1; i++)
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

    polySegment.init (points[numPoints-1], points[0]);
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

int Pollygon::containsExcludeEdge( const Point& _pt) const
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
bool Pollygon::isEdgePoint( const Point& _pt, int& _iSeg ) const
{
	Line _polySegment;
	for( int i=0; i<numPoints; i++ )
	{	
		int _iMod = (i+1)%numPoints;
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

int Pollygon::intersects (const Point& p1, const Point& p2) const
{
    if (numPoints < 2 || p1.getZ() != bottomLeft.getZ())
		return 0;

    Line testLine (p1, p2);
    Line polySegment (points[0], points[numPoints-1]);

    // test closing segment
    if (numPoints > 2 && testLine.intersects (polySegment))
        return TRUE;

    // test each segment for intersection
    for (short i = 0; i < numPoints - 1; i++)
    {
        polySegment.init (points[i], points[i+1]);
        if (testLine.intersects (polySegment))
            return TRUE;
    }
    return FALSE;
}

//Returns the area of the polygon in meters squared.
double Pollygon::calculateArea (void)
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
Point Pollygon::getRandPoint (DistanceUnit) const
{
    Point newPoint (0.0, 0.0, bottomLeft.getZ());
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
void Pollygon::DoOffset( DistanceUnit _xOffset,  DistanceUnit _yOffset,  DistanceUnit _zOffset )
{
	Path::DoOffset( _xOffset, _yOffset, _zOffset );
    bottomLeft.DoOffset( _xOffset, _yOffset, _zOffset );
	topRight.DoOffset( _xOffset, _yOffset, _zOffset );
}
void Pollygon::Rotate (DistanceUnit degrees)
{
	Path::Rotate( degrees );
	bottomLeft.rotate( degrees );
	topRight.rotate( degrees );
	SetBoundsPoint();
}
bool Pollygon::IsOverLap( const Point& p1, const Point& p2) const
{
	if( contains( p1 ) )
		return true;
	if( contains(p2) )
		return true;
	if( intersects( p1, p2 ) )
		return true;
	else
		 return false;
}


bool Pollygon::contains ( const Pollygon& _another ) const
{
	int iAnotherPointCount = _another.getCount();
	if( iAnotherPointCount < 1 )
		return false;
	for( int i=0; i<iAnotherPointCount ; ++i )
	{
		if( !contains( _another.getPoint( i ) ) )
			return false;
	}


	return true;
}
// test if this pollygon is a contex pollygon
bool Pollygon::IsAConvexPollygon()const
{
	ASSERT( numPoints >= 3 );
	int iPlusCount=0;
	int iMinusCount=0;
	for( int i=0,j=2; j< numPoints; ++j,++i )
	{
		Point temp1;
		temp1.initVector( points[i], points[i+1]);

		Point temp2;
		temp2.initVector( points[i+1], points[j]);
		Point result;
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

void Pollygon::GetBoundsPoint(Point& ptBottomLeft,Point& ptTopRight)const
{
	ptBottomLeft = bottomLeft;
	ptTopRight = topRight;
}

bool Pollygon::IfLineInsidePollygon( const Point& p1, const Point& p2 )const
{
	ASSERT( numPoints >= 3 );
	if( intersects( p1, p2 ) )
	{
		return false;	
	}
	else
	{
		Line tempLine( p1,p2 );
		Point centerPoint;
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
void Pollygon::DividAsConvexPollygons( std::vector<Pollygon>& _vResult )const
{
	ASSERT( numPoints >= 3 );
	if( IsAConvexPollygon() )
	{
		_vResult.push_back( *this );
	}
	else
	{
		Pollygon reMainPollygon;
		reMainPollygon.init( this->numPoints, this->points );
		//reMainPollygon.
		//reMainPollygon.bottomLeft.setZ( points[0].getZ() );

		Point tempTrianglePoint [ 3 ];
		Pollygon tempTriangle;

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

void Pollygon::Zoom(DistanceUnit _dX, DistanceUnit _dY, DistanceUnit _dZ  )
{
	if( numPoints > 3 )
	{
		int iIdx = numPoints - 2;
		Line temp1( points[0],points[ iIdx ] );
		Line temp2(  points[1],points[ iIdx +1] );
		Point ptIntersection = temp1.intersection( temp2);
		DoOffset( ptIntersection.getX(),ptIntersection.getY(),0.0 );
		scale( _dX,_dY ,_dZ );
		DoOffset( -ptIntersection.getX(),-ptIntersection.getY(),0.0 );

		
		Line temp3( points[0],points[iIdx] );
		Line temp4(  points[1],points[iIdx+1] );
		Point ptIntersection2 = temp3.intersection( temp4);

		DoOffset( ptIntersection.getX() - ptIntersection2.getX() ,ptIntersection.getY() -  ptIntersection2.getY(),0.0 );

	}
	else
	{
		Point ptCenterPt12;
		DistanceUnit dX = points[1].getX() + ( points[2].getX() - points[1].getX() ) /2.0;
		DistanceUnit dY = points[1].getY() + ( points[2].getY() - points[1].getY() ) /2.0;
		ptCenterPt12.setPoint( dX, dY ,points[1].getZ() );

		Point ptCenterPt20;
		dX = points[2].getX() + ( points[2].getX() - points[0].getX() ) /2.0;
		dY = points[2].getY() + ( points[2].getY() - points[0].getY() ) /2.0;

		ptCenterPt20.setPoint( dX, dY ,points[2].getZ() );
		
		Line temp1( points[0],ptCenterPt12 );
		Line temp2( points[1],ptCenterPt20 );
		Point ptIntersection = temp1.intersection( temp2);
		DoOffset( ptIntersection.getX(),ptIntersection.getY(),0.0 );
		scale( _dX,_dY ,_dZ );
		DoOffset( -ptIntersection.getX(),-ptIntersection.getY(),0.0 );


		dX = points[1].getX() + ( points[2].getX() - points[1].getX() ) /2.0;
		dY = points[1].getY() + ( points[2].getY() - points[1].getY() ) /2.0;
		ptCenterPt12.setPoint( dX, dY ,points[1].getZ() );

		
		dX = points[2].getX() + ( points[2].getX() - points[0].getX() ) /2.0;
		dY = points[2].getY() + ( points[2].getY() - points[0].getY() ) /2.0;	
		ptCenterPt20.setPoint( dX, dY ,points[2].getZ() );
		
		Line temp3( points[0], ptCenterPt12 );
		Line temp4( points[1], ptCenterPt20 );
		
		Point ptIntersection2 = temp3.intersection( temp4);

		DoOffset( ptIntersection.getX() - ptIntersection2.getX() ,ptIntersection.getY() -  ptIntersection2.getY(),0.0 );

	}
	
}

bool Pollygon::IfOverlapWithOtherPolygon( const Pollygon& _otherPlygon )const
{
	int iPointCount = _otherPlygon.getCount();
	for( int i=0,j=1; j<iPointCount; ++i,++j )
	{
		if( IsOverLap( _otherPlygon.getPoint( i ),_otherPlygon.getPoint( j ) ) )
			return true;
	}

	return false;
}

void Pollygon::ReadDataFromDB(Pollygon* _polly,int _parentID)
{
	if(_polly == NULL || _parentID == -1)
		return ;
	CString SQL ;
	long count = 0 ;
	CADORecordset dataSet ;
	SQL.Format(_T("SELECT * FROM TB_DECK_POLLY WHERE REGINID = %d"),_parentID) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataSet) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	GetDataSet(dataSet,_polly) ;
}
void Pollygon::WriteDataToDB(Pollygon* _polly,int _parentID)
{
	if(_polly == NULL || _parentID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_DECK_POLLY WHERE REGINID = %d"),_parentID);
	CADODatabase::ExecuteSQLStatement(SQL) ;
	int count = _polly->getCount();
	for (int i = 0 ; i < count ;i++)
	{
		WriteData(_polly->getPoint(i),_parentID) ;
	}
}
void Pollygon::GetDataSet(CADORecordset& _set,Pollygon* _polly)
{
	if(_polly == NULL)
		return ;
	double x = 0 ;
	double y = 0 ;
	std::vector<Point> Pointlist ;
	while(!_set.IsEOF())
	{
		_set.GetFieldValue(_T("_X"),x) ;
		_set.GetFieldValue(_T("_Y"),y) ;
		Pointlist.push_back(Point(x,y,0.0)) ;
		_set.MoveNextData() ;
	}
	int nDataSize = Pointlist.size() ;
	Point* pPList = new Point[nDataSize];
	for(int i=0; i< nDataSize; i++) {
		pPList[i].setPoint(Pointlist[i].getX(),Pointlist[i].getY(), 0.0);
	}
	_polly->init(nDataSize, pPList);
	delete [] pPList;
}
void Pollygon::WriteData(Point _polly,int _parentID)
{
	if(_parentID == -1 )
		return  ;
	CString SQL ;
	SQL.Format(_T("INSERT INTO TB_DECK_POLLY ( _X, _Y, REGINID) VALUES (%f,%f,%d)"),_polly.getX(),_polly.getY(),_parentID) ;
	CADODatabase::ExecuteSQLStatement(SQL) ;
}

double Pollygon::getArea() const
{
	double area = 0.0;
	double ax, ay, bx, by;
	bx = points[0].getX() - points[numPoints - 1].getX();
	by = points[0].getY() - points[numPoints - 1].getY();
	for (short i = 0; i < numPoints - 2; i++)
	{
		ax = bx;
		ay = by;
		bx = points[i + 1].getX() - points[numPoints - 1].getX();
		by = points[i + 1].getY() - points[numPoints - 1].getY();
		area += (ax * by - ay * bx);    //twice area of triangle in cm^2
	}
	area *= 0.00005;    //divide by two, convert to m^2
	return area;
}

bool Pollygon::IsCountClock() const
{
	return getArea()>=0;
}