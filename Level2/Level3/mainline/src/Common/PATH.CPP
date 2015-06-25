#include "stdafx.h"
#include "path.h"
#include "point.h"
#include "exeption.h"
//#include "Floor2.h"
//#include "main\resource.h"
//#include "main\termplandoc.h"
//#include "inputs\flight.h"
#include "line.h"
#include <assert.h>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include "Path2008.h"
#include "Point2008.h"
#include <common/ARCMathCommon.h>

static const double pi = 3.1415926535;


void Path::init (int pointCount)
{
    if (numPoints != pointCount)
    {
        clear();
        numPoints = pointCount; 
        //Point aPoint;
        points = new Point[numPoints]; //aPoint.makeArray (numPoints); 
    }
	Initialization();
}

void Path::init (const Path& pointList)
{
    init (pointList.numPoints);
    if (!numPoints)
		return;
    memcpy (points, pointList.points, sizeof (Point) * numPoints);
	Initialization();
}

void Path::init (int pointCount, const Point *pointList)
{
    init (pointCount);
    if (!numPoints)
		return;
    memcpy (points, pointList, sizeof (Point) * numPoints);
	Initialization();
}

Point Path::getPoint (int index) const
{
	ASSERT(index<numPoints);   
	if (index >= numPoints)
        throw new OutOfRangeError ("Path::getPoint");

    return points[index];
}


void Path::setPoint( const Point& pt, int index ) const
{
	ASSERT(index<numPoints);   
	if (index >= numPoints)
		throw new OutOfRangeError ("Path::getPoint");

	points[index] = pt;
}

Point Path::getClosestPoint (const Point& pnt) const
{
    double closest = pnt.distance (points[0]);
    short ndx = 0;

    for (short i = 1; i < numPoints; i++)
        if (closest > pnt.distance (points[i]))
        {
            closest = pnt.distance (points[i]);
            ndx = i;
        }

    return points[ndx];
}

// flips the order of the points in the list
void Path::invertList (void)
{
    for (int i = 0, j = numPoints-1; i < j; i++, j--)
        points[i].swap (points[j]);
}

Point Path::getRandPoint (DistanceUnit area) const
{
    Point aPnt = points[random (numPoints)];
    return aPnt.getRandPoint (area);
}

void Path::distribute (int pointCount, const Point *pointList,
    DistanceUnit clearance)
{
    clear();
    if (!pointCount)
        return;

    //if (pointCount == 1 || pointList[0].distance (pointList[1]) < clearance)
	if (pointCount == 1 )
    {
        init (pointCount, pointList);
        return;
    }

    if (pointCount >= 1023)
        throw new StringError ("Max point count exceeded");

	std::vector<Point>vResutPoint;
	Point ptvector;
	//numPoints = 0;
	for (short i = 0; i < pointCount-1; i++)
    {
		vResutPoint.push_back( pointList[i] );
        //points[numPoints] = pointList[i];
        ptvector.initVector (pointList[i], pointList[i+1]);
        ptvector.length ((double)clearance);
        while (vResutPoint[ vResutPoint.size() -1 ].distance (pointList[i+1]) > clearance)
        {
			vResutPoint.push_back(  vResutPoint[ vResutPoint.size() -1 ] + ptvector );
        }
    }

	int iPointCount = vResutPoint.size() ;
	init( vResutPoint.size() );
	for( int j=0; j<iPointCount; ++j )
	{
		points[ j ] = vResutPoint[ j ];
	}

	/*
    init (1024);
    numPoints = 0;
    Point vector;

    for (short i = 0; i < pointCount-1; i++)
    {
        points[numPoints] = pointList[i];
        vector.initVector (pointList[i], pointList[i+1]);
        vector.length ((double)clearance);

        while (points[numPoints].distance (pointList[i+1]) > clearance)
        {
            numPoints++;
            points[numPoints] = points[numPoints-1] + vector;
        }
        numPoints++;
    }
	*/
}

//mirror the points of this path relative to _p (1st 2 pts)
void Path::Mirror( const Path* _p )
{
	ASSERT(_p->getCount()>1);
	Point ab(_p->points[0], _p->points[1]);
	for( int i=0; i<numPoints; i++ ) {
		Point ap(_p->points[0], points[i]);
		double magSquared = ab.getX()*ab.getX()+ab.getY()*ab.getY();
		Point an( ab * ((ap*ab)/magSquared) );
		Point pn( (an+_p->points[0])-points[i] );
		points[i] += pn*2;
	}
}


void Path::DoOffset(DistanceUnit _xOffset, DistanceUnit _yOffset, DistanceUnit _zOffset)
{
	for( int i=0; i<numPoints; i++ )
		points[i].DoOffset( _xOffset, _yOffset, _zOffset );
}

Path& Path::operator = ( const Path& _anotherPath)
{
	if( this != &_anotherPath )
	{
		//clear();
		init( _anotherPath );

	}
	return *this;
}

Point& Path::operator[] ( int _idx )
{
	assert( _idx >=0 && _idx < numPoints );
	return points[ _idx ];
}

const Point& Path::operator[]( int _idx ) const
{
	assert( _idx >=0 && _idx < numPoints );
	return points[ _idx ];
}

void Path::Rotate (DistanceUnit degrees)
{
	for(int i=0; i<numPoints; ++i )
	{
		points[i].rotate( degrees );
	}
}

void Path::RemovePointAt( int _iIdx )
{
	ASSERT( _iIdx>=0 && _iIdx< numPoints );
	int iTotalCount = numPoints-1;
	int i=0;
	Point* pNewPointList = new Point[ iTotalCount ];
	for( i=0; i< _iIdx ; ++i )
	{
		Point tempPoint = points[i];
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}

	for( i= _iIdx; i< iTotalCount ; ++i )
	{
		Point tempPoint = points[i+1];
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}
	init( iTotalCount , pNewPointList );
	delete [] pNewPointList;
}

void Path ::insertPointAfterAt(const Point & _p,int _iIdx){
	ASSERT( _iIdx>=0 && _iIdx< numPoints );
	int iTotalCount = numPoints+1;
	int i=1;
	Point* pNewPointList = new Point[ iTotalCount ];
	for( i=0; i< _iIdx+1 ; ++i )
	{
		Point& tempPoint = points[i];
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}
	pNewPointList[_iIdx+1].setPoint(_p.getX(),_p.getY(),_p.getZ());
	for( i= _iIdx+2; i< iTotalCount ; ++i )
	{
		Point& tempPoint = points[i-1];
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}
	init( iTotalCount , pNewPointList );
	delete [] pNewPointList;
}

DistanceUnit Path::GetTotalLength()const
{
	if( numPoints <= 1 )
	{
		return 0.0;
	}

	DistanceUnit dLength =0.0;
	for(int i=0,j=1; j<numPoints; ++i,++j)
	{
		dLength += points[i].distance( points[j] );
	}

	return dLength;
}

// get the path's fact length
DistanceUnit Path::getFactLength( const std::vector<double>& vRealAltitude ) const
{
	if( numPoints <=1 )
		return 0.0;

	DistanceUnit _dLength = 0.0;
	for( int i=1; i< numPoints; i++ )
	{
		_dLength += getFactLength( points[i-1], points[i],vRealAltitude );
	}

	return _dLength;
}

// get two points fact distance
DistanceUnit Path::getFactLength( const Point& _pt1, const Point& _pt2 ,const std::vector<double>& vRealAltitude) const
{
	//CTermPlanDoc* _pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	//assert( _pDoc );

	double _dPt1Floor	= _pt1.getZ() / SCALE_FACTOR;
	double _dPt2Floor	= _pt2.getZ() / SCALE_FACTOR;
	
	double _dFloorFrom	= min( _dPt1Floor, _dPt2Floor );
	double _dFloorTo	= max( _dPt1Floor, _dPt2Floor );

	int _iFloorFrom = ( int) _dFloorFrom;
	int _iFloorTo	= ( int) _dFloorTo;

	int _iFloorSize = vRealAltitude.size()-1;

	DistanceUnit _dVerticalDistance = vRealAltitude[ _iFloorTo ] - vRealAltitude[ _iFloorFrom];
	DistanceUnit _dV1 = ( _dFloorFrom - _iFloorFrom ) *
						( vRealAltitude[ min( _iFloorFrom +1, _iFloorSize) ] - vRealAltitude[ _iFloorFrom ]);
	DistanceUnit _dV2 = ( _dFloorTo - _iFloorTo ) *
						( vRealAltitude[ min(_iFloorTo +1, _iFloorSize) ] - vRealAltitude[ _iFloorTo ] );
	
	_dVerticalDistance = _dVerticalDistance - _dV1 + _dV2;

	DistanceUnit _dHorizonDistance = _pt1.distance( _pt2 );


	Point _tmpPt( _dHorizonDistance, _dVerticalDistance, 0.0 );
	
	return _tmpPt.length();
}

// get max angle
double Path::getMaxAngle( const std::vector<double>& vRealAltitude  ) const
{
	double _dMaxAngle = 0.0;
	
	for( int i=1; i< numPoints; i++ )
	{
		// get distance and horizon distance
		DistanceUnit _distance			= getFactLength( points[i-1], points[i],vRealAltitude);
		DistanceUnit _dVerticalDistance = getFactLength( Point( 0.0, 0.0, points[i-1].getZ()), Point( 0.0, 0.0,points[i].getZ() ),vRealAltitude );
				
		if( _distance == 0.0 )
			continue;

		if( points[i-1].getZ() > points[i].getZ() )
			_dVerticalDistance *= -1.0;
		// get angle by asin
		long double _sin_radians = asin( _dVerticalDistance / _distance );
		_sin_radians = _sin_radians * 180.0 / pi;
		
		if( abs( _sin_radians) > abs( _dMaxAngle ) )
			_dMaxAngle = _sin_radians;
	}

	return _dMaxAngle;
}

// convert path, make its length to _newLength.
// ( the z value of each point can not change )
bool Path::zoomPathByLength( DistanceUnit _newLength, CString& _strFailedReason,const std::vector<double>& vRealAltitude  )
{
	if( numPoints<=1 )
	{
		_strFailedReason = "the number of point less than 1";
		return false;
	}

	//temp path, to save new points
	Path _newPath;
	_newPath.init( numPoints );
	
	// old fact length
	DistanceUnit _oldLength = getFactLength(vRealAltitude );		
	if( _oldLength == 0.0 )
	{
		DistanceUnit _dSegLength = _newLength/numPoints;
		for( int i=0; i< numPoints; i++ )
			_newPath[i] = Point( points[i].getX() + _dSegLength, points[i].getY(), points[i].getZ() );

		_strFailedReason = "zoom succeed!";
		return true;
	}

	// get zoom percent
	double _dZoomPercent = _newLength / _oldLength;
	
	// build new path
	DistanceUnit _distance = 0.0;
	DistanceUnit _dVerticalDistance = 0.0;
	DistanceUnit _dHorizonDistance = 0.0;
	_newPath[0] = points[0];
	for( int i=1; i< numPoints; i++ )
	{
		// get new distance of the two point
		_distance = getFactLength( points[i-1], points[i], vRealAltitude);
		_distance *= _dZoomPercent;
		
		// get their vertical high
		_dVerticalDistance = getFactLength( Point(0.0, 0.0, points[i-1].getZ()), Point( 0.0, 0.0, points[i].getZ() ) , vRealAltitude);
		
		if( _distance < _dVerticalDistance )
		{
			_strFailedReason.Format("the vertical distance of point %d and point %d is too long, can not zoom the path", i-1, i );
			return false;
		}
		
		// get horizon distance
		long double _sqrt = _distance * _distance -_dVerticalDistance * _dVerticalDistance;
		_dHorizonDistance = sqrtl( _sqrt );
		
		// get the new point
		Point _tmpVector( Point( points[i-1].getX(), points[i-1].getY(), 0.0 ),
			              Point( points[i].getX(), points[i].getY(), 0.0 ) );
		_tmpVector.length( _dHorizonDistance );
		Point _newPoint = Point( _newPath[i-1].getX(), _newPath[i-1].getY(), 0.0 ) + _tmpVector;
		_newPoint.setZ( points[i].getZ() );

		// add the new point to new path
		_newPath[i] = _newPoint;
	}
	
	// change the old path
	init( _newPath );
	_strFailedReason = " zoom succeed!";
	return true;	
}


// convert path, make all angle to _newAngle
// ( the length value can not change )
//the function has been move to class CProcPropDlg
//void Path::convertPathByAngle( double _newAngle )
//{
//	ASSERT(0);
	//CTermPlanDoc* _pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	//assert( _pDoc );
	//
	//if( numPoints <=1 )
	//	return ;
	//
	//long double _radians = _newAngle * pi / 180.0;
	////temp path, to save new points
	//Path _newPath;
	//_newPath.init( numPoints );

	//// build new path
	//DistanceUnit _distance = 0.0;
	//DistanceUnit _dVerticalDistance = 0.0;
	//DistanceUnit _dHorizonDistance = 0.0;
	//_newPath[0] = points[0];
	//for( int i=1; i < numPoints; i++ )
	//{
	//	// get distance of two point
	//	_distance = getFactLength( points[i-1], points[i] );
	//	
	//	// get ver & hor length
	//	_dVerticalDistance = sin( _radians ) * _distance;
	//	_dHorizonDistance  = cos( _radians ) * _distance;
	//	
	//	// adjust z value
	//	double _dRealHight = _pDoc->GetCurModeFloor().getRealAltitude( _newPath[i-1].getZ() );
	//	_dRealHight += _dVerticalDistance;
	//	double _dLogicHight = _pDoc->GetCurModeFloor().getLogicAltitude( _dRealHight );
	//	
	//	// get the new point
	//	_dVerticalDistance = getFactLength( Point( 0.0, 0.0, _newPath[i-1].getZ()), 
	//										Point( 0.0, 0.0, _dLogicHight ) );

	//	if( _dVerticalDistance != 0.0 && _radians != 0.0 )
	//	{
	//		_dHorizonDistance = _dVerticalDistance / tan( _radians );
	//	}

	//	Point _tmpVector( Point( points[i-1].getX(), points[i-1].getY(), 0.0 ),
	//					  Point( points[i].getX(), points[i].getY(), 0.0 ) );
	//	_tmpVector.length( _dHorizonDistance );
	//	Point _newPoint = Point( _newPath[i-1].getX(), _newPath[i-1].getY(), 0.0 ) + _tmpVector;
	//	_newPoint.setZ( _dLogicHight );
	//	
	//	// add the new point to new path
	//	_newPath[i] = _newPoint;
	//}

	//// change the old path
	//init( _newPath );
//	return ;	
//}


//2d
bool Path::within(Point pt)
{
	if (numPoints == 1)	return false;

	for(int i=1; i<numPoints; i++)
	{
		Line ln(points[i-1],points[i]);
		if (true == ln.containsex(pt))
			return true;
	}
	return false;

}

bool Path::IsVertex(Point pt)
{
	for(int i=0; i<numPoints; i++)
	{
		if (pt.distance(points[i]) < 1.0)//!!!
			return true;
	}
	return false;
}
//2d
bool Path::intersects(const Path* pDest)const
{
	for (int i=1; i<numPoints; i++) 
	{
		Line ln1(points[i-1],points[i]);
		for (int j=1; j<pDest->getCount(); j++) 
		{
			Line ln2(pDest->getPoint(j-1), pDest->getPoint(j));
			Point retPt;
			if (ln1.intersects(ln2, retPt))
				return true;
		}
	}
	return false;
}

Path::Path( const Path &path )
{
	numPoints = 0;
	points = NULL;
	init(path.getCount(),path.points);
}

Path::Path(const CPath2008 &path)
{
	numPoints = 0;
	points = NULL;

	if (numPoints != path.getCount())
	{
		clear();
		numPoints = path.getCount(); 
		Point aPoint;
		points = aPoint.makeArray (numPoints); 

		for (int i=0; i<numPoints; i++)
		{
			points[i].setPoint(path.getPoint(i).getX(), path.getPoint(i).getY(), path.getPoint(i).getZ());
		}
	}
}

 bool Path::GetSnapPoint( double width, const Point& pt, Point& reslt )const
{
	double mindist  = (std::numeric_limits<double>::max)();
	

	for(int i =0 ;i < getCount()-1; i++){
		Line line( getPoint(i), getPoint(i+1) );
		double dist;
		if( (dist = line.PointDistance(pt)) < mindist ){
			mindist = dist; 
			reslt = line.getProjectPoint(pt);
			if(!line.contains(reslt)) reslt = line.nearest(reslt);
		}
	}
	if(mindist <width*0.5 )return true;
	return false;
}


bool Path::GetDistPoint( double dist, Point& pt ) const
{
	//if(getCount()<1) return false;
	if(getCount()<2) return false;
	
	double accdist = 0;
	double thisdist = 0;
	int i=0;
	for( i=0 ;i< getCount()-1;i++){
		if(accdist > dist ){
			break;
		}		
		thisdist  = getPoint(i).distance3D(getPoint(i+1));
		accdist += thisdist;
	}
	if(i<1){
		return false;
	}
	double exceed = (accdist - dist) / thisdist;
	pt = getPoint(i-1)* exceed + getPoint(i)*(1-exceed);
	return true;
	
}

Point Path::GetDistPoint( double dist ) const
{
	ASSERT(getCount()>1);
	if(getCount() < 1)
		return Point(0,0,0);

	double accdist = 0;
	double thisdist = 0;
	Point orignPt = getPoint(0) ;
	Point vDir = Point(0,0,0);
	
	int i;
	for(i=0 ;i< getCount()-1;i++){
		if(accdist > dist ){			
			break;
		}		
		thisdist  = getPoint(i).distance3D(getPoint(i+1));
		accdist += thisdist;
		orignPt = getPoint(i+1);
	}
	
	if(i<1){
		//vDir = getPoint(i) - getPoint(i+1);
	}
	else if(i>getCount()-1)
	{
		//vDir = getPoint(i-1) - getPoint(i-2);
	}
	else
	{
		vDir = getPoint(i-1) - getPoint(i);
	}

	vDir.Normalize();
	

	double exceed = accdist - dist;
	
	
	return orignPt + vDir * exceed;

}

bool Path::GetRelateDistPoint( float dist, Point& pt ) const
{
	int npt = (int)dist;
	float offset = dist - npt;

	if(npt < 0) return false;
	
	if( npt > getCount()-1 ) return false;
	if(npt == getCount()-1){
		pt= getPoint(npt);
		return true;
	}
	pt = getPoint(npt) * (1-offset) + getPoint(npt+1)*offset;
	return true;
}

Point Path::GetRelateDistPoint( float dist ) const
{
	
	ASSERT(dist >=0 );
	ASSERT(dist < getCount() );

	int npt = (int)dist;
	float offset = dist - npt;

	if(npt < 0){
		npt = 0;
		offset = 0;
	}
	if(npt >= getCount() - 1 ){
		npt = getCount() - 2;
		offset = 1;
	}
	
	return getPoint(npt) * (1-offset) + getPoint(npt+1)*offset;	
}

float Path::GetRelateDist( double realDist ) const
{
	if(getCount()<2) return 0;

	double accdist = 0;
	double thisdist = 0;
	int i=0 ;
	for(i=0 ;i< getCount()-1;i++){
		if(accdist >= realDist ){
			break;
		}		
		thisdist  = getPoint(i).distance3D(getPoint(i+1));
		accdist += thisdist;
	}

	double exceed =0;
	if(thisdist == 0)
	{
		exceed = 0;
	}
	else
	{
		exceed = (accdist - realDist) / thisdist;
	}
	if(exceed > 1)
	{
		//ASSERT(false);
		exceed = 1;
	}
	if(exceed <0)
	{
		//ASSERT(false);
		exceed = 0;
	}

	return i - (float)exceed;
}

double Path::GetRealDist( float relateDist ) const
{
	if(relateDist < 0) return -1;
	if(getCount() < 2) return 0;

	if(relateDist >= getCount() -1 ) return GetTotalLength();

	int npt = (int)relateDist;
	float offset = relateDist - npt;
	
	double len = 0;
	for(int i=0;i< npt;i++){
		len += getPoint(i).distance3D( getPoint(i+1) );
	}	
	len += getPoint(npt).distance3D(getPoint(npt+1)) * offset;
	return len;

}

Point Path::GetDirection( float dist ) const
{
	

	ASSERT(getCount()>1);
	int idist =(int)dist;
	

	if(idist < 0)
	{
		return getPoint(0) - getPoint(1);
	}
	
	if(idist > getCount() -2)
	{
		return getPoint(getCount() -1) - getPoint( getCount() -2 );
	}

	return getPoint(idist+1) - getPoint(idist);

}

int Path::GetInPathPoints( double dFrom, double dTo, std::vector<Point>& vPts ) const
{
	
	int nAdded = 0;
	double dAccLen = 0;
	bool bReveres = (dFrom > dTo);
	
	if(bReveres)
		std::swap(dFrom,dTo);

	std::vector<Point> vInsertPts;

	for(int i =0;i< getCount()-1;i++)
	{
		DistanceUnit theDist =  points[i].distance3D(points[i+1]);
		dAccLen += theDist;
		
		if( dFrom < dAccLen &&  dAccLen< dTo )
		{
			vInsertPts.push_back( points[i+1] );			
		}
		
		if( dTo <= dAccLen )
		{
			break;
		}
	}
	
	if(bReveres)
		std::reverse(vInsertPts.begin(),vInsertPts.end());

	vPts.insert(vPts.end(),vInsertPts.begin(),vInsertPts.end() );
	return vInsertPts.size();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CSegmentPath::CSegmentPath() : m_dPathLength(0.0), m_dSegmentLen(0.0),
							   m_iSegmentCount(0)
{
	
}


CSegmentPath::~CSegmentPath()
{
	m_segmentPath.clear();
}


void CSegmentPath::init( const Path& path, int _segCount )
{	
	assert( path.getCount() >1 );
	assert( _segCount >0 );

	Path::init( path );
	m_segmentPath.clear();
	m_iSegmentCount = _segCount;
	m_dPathLength = getPathLength();
	m_dSegmentLen = m_dPathLength / m_iSegmentCount;
	
	splitPath();
	getSlotPoint();
}


double CSegmentPath::getPathLength( void )
{
	double dPathLength = 0.0;

	for( int i=0; i< numPoints-1; i++ )
	{
		dPathLength += points[i].distance3D( points[ i+1] );
	}

	return dPathLength;
}

void CSegmentPath::splitPath( void )
{
	int segIndex=0;
	Point pVector;
	double vector_length = m_dSegmentLen/2.0;
	double segmentLen = m_dSegmentLen/2.0;
	double total_distance = 0.0;
	
	m_segmentPath.push_back( std::make_pair( points[0], -1) );

	for( int i=1; i < numPoints; i++ )
	{
		double dp2p = points[i].distance3D( points[i-1] );
		total_distance += dp2p;
		pVector.initVector3D( points[i-1],points[i] );
		Point segPoint = points[i-1];

		while( (long)(10000*total_distance) >= (long)(segmentLen*10000) )
		{
			pVector.length3D( vector_length );
			segPoint.plus( pVector );
			m_segmentPath.push_back( std::make_pair( segPoint,segIndex++ ) );

			total_distance -= segmentLen;
			//segmentLen = (segIndex == m_iSegmentCount) ? m_dSegmentLen/2.0 : m_dSegmentLen; // the last point
			segmentLen = m_dSegmentLen;
			vector_length = segmentLen;
		}
		
		if( (long)(10000*total_distance) > 0 )
		{
			vector_length = segmentLen- total_distance;
			m_segmentPath.push_back( std::make_pair( points[i] ,-1 ) );
		}
		m_vSegmentPathIndex.push_back(segIndex);
	}
}

bool CSegmentPath::getSegmentDrection(int nIdex,int& nFromFloor,int& nToFloor)
{
	for (int i = 0; i < (int)m_vSegmentPathIndex.size(); i++)
	{
		if (nIdex < m_vSegmentPathIndex[i])
		{
			int iFromFloor = (int)(points[i].getZ() / SCALE_FACTOR);
			double diff = (points[i].getZ() / SCALE_FACTOR) - iFromFloor;
			int iToFloor = iFromFloor + ((diff>ARCMath::EPSILON)?1:0);
			
			if(i+1 < getCount())
			{				
				int i1FromFloor = (int)(points[i+1].getZ() / SCALE_FACTOR);
				diff = (points[i+1].getZ() / SCALE_FACTOR) - i1FromFloor;
				int i1ToFloor = i1FromFloor + ((diff>ARCMath::EPSILON)?1:0);
				nFromFloor = min(i1FromFloor,iFromFloor);
				nToFloor = max(iToFloor, i1ToFloor);

				if(points[i+1].getZ()<points[i].getZ())
				{
					std::swap(nFromFloor,nToFloor);
				}				
				return true;
			}

			/*nFromFloor = iFromFloor;
			nToFloor = iToFloor;
			return true;*/
		}
	}
	return false;
}

void CSegmentPath::getSlotPoint( void )
{
	m_slotsPoint.clear();

	int _iSegIdx = 0;
	for( unsigned i=0; i<m_segmentPath.size(); i++ )
	{
//		// TRACE("%d: %d\r\n",i, m_segmentPath[i].second);
		if( m_segmentPath[i].second == _iSegIdx )
		{
			m_slotsPoint.push_back( m_segmentPath[i].first );
			_iSegIdx++;
		}
	}

	assert( m_slotsPoint.size() == m_iSegmentCount );
}


std::vector<Point> CSegmentPath::getNoFlagPointList( int _fromSeg, int _toSeg ) const
{
	std::vector< Point > ptList;
	bool bStart = false;

	for( unsigned i=0; i< m_segmentPath.size(); i++ )
	{
		if( !bStart &&
			m_segmentPath[i].second == _fromSeg  )
			bStart = true;

		if( bStart )
			ptList.push_back( m_segmentPath[i].first );

		if( bStart &&  m_segmentPath[i].second!= -1 &&
			m_segmentPath[i].second == _toSeg )
			break;
	}

	return ptList;
}


std::vector< PointIdxPair > CSegmentPath::getFlagPointList( int _fromSeg, int _toSeg  ) const
{
	std::vector< PointIdxPair > ptList;
	bool bStart = false;
	
	for( unsigned i=0; i< m_segmentPath.size(); i++ )
	{
		if( !bStart &&
			m_segmentPath[i].second == _fromSeg  )
			bStart = true;
		
		if( bStart )
			ptList.push_back( m_segmentPath[i] );
		
		if( bStart &&  m_segmentPath[i].second!= -1 &&
			m_segmentPath[i].second == _toSeg )
			break;
	}
	
	return ptList;
}


Point CSegmentPath::getSlotPoint( int _idx ) const
{
	assert( _idx>=0 && _idx< m_iSegmentCount );
	return m_slotsPoint.at( _idx );
}
int Path::getClosestPointIndex(const Point& point)
{

	double closest = point.distance (points[0]);
	short ndx = 0;

	for (short i = 1; i < numPoints; i++)
	{
		if (closest > point.distance (points[i]))
		{
			closest = point.distance (points[i]);
			ndx = i;
		}
	}
		return ndx;
}
