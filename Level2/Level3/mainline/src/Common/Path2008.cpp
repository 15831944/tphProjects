#include "StdAfx.h"
#include ".\path2008.h"
#include "stdafx.h"
#include "exeption.h"
#include "line.h"
#include <assert.h>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include "Line2008.h"
#include "./DistanceLineLine.h"
#include "ARCMathCommon.h"
#include "ARCVector.h"

static const double pi = 3.1415926535;



void CPath2008::clear (void) 
{ 
	points.clear();
}

void CPath2008::init (int pointCount)
{
	points.resize(pointCount);		
	Initialization();
}

void CPath2008::init (const CPath2008& pointList)
{
	points = pointList.points;
	Initialization();
}

void CPath2008::init (int pointCount, const CPoint2008 *pointList)
{
	points = std::vector<CPoint2008>(pointList,pointList+pointCount);
	Initialization();
}

CPoint2008 CPath2008::getPoint (int index) const
{
	ASSERT(index< getCount() );   
	if (index >=  getCount() )
		throw new OutOfRangeError ("CPath2008::getPoint");

	return points[index];
}

CPoint2008 CPath2008::getClosestPoint (const CPoint2008& pnt) const
{
	double closest = pnt.distance (points[0]);
	int ndx = 0;

	for (int i = 1; i < getCount(); i++)
		if (closest > pnt.distance (points[i]))
		{
			closest = pnt.distance (points[i]);
			ndx = i;
		}

		return points[ndx];
}

// flips the order of the points in the list
void CPath2008::invertList (void)
{
	for (int i = 0, j = getCount()-1; i < j; i++, j--)
		points[i].swap (points[j]);
}

CPoint2008 CPath2008::getRandPoint (DistanceUnit area) const
{
	CPoint2008 aPnt = points[random (getCount())];
	return aPnt.getRandPoint (area);
}

void CPath2008::distribute (int pointCount, const CPoint2008 *pointList,
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

	std::vector<CPoint2008>vResutPoint;
	CPoint2008 ptvector;
	//getCount() = 0;
	for (short i = 0; i < pointCount-1; i++)
	{
		vResutPoint.push_back( pointList[i] );
		//points[getCount()] = pointList[i];
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
}

//mirror the points of this path relative to _p (1st 2 pts)
void CPath2008::Mirror( const CPath2008* _p )
{
	ASSERT(_p->getCount()>1);
	ARCVector3 ab(_p->points[0], _p->points[1]);
	for( int i=0; i<getCount(); i++ ) {
		ARCVector3 ap(_p->points[0], points[i]);
		double magSquared = ab.x*ab.x+ab.y*ab.y;
		CPoint2008 an( ab * (( ap.dot(ab) )/magSquared) );
		CPoint2008 pn( (an+_p->points[0])-points[i] );
		points[i] += pn*2;
	}
}


void CPath2008::DoOffset(DistanceUnit _xOffset, DistanceUnit _yOffset, DistanceUnit _zOffset)
{
	for( int i=0; i<getCount(); i++ )
		points[i].DoOffset( _xOffset, _yOffset, _zOffset );
}

void CPath2008::DoOffset( const ARCVector3& pos )
{
	for( int i=0; i<getCount(); i++ )
		points[i] = points[i] + pos;
}



CPoint2008& CPath2008::operator[] ( int _idx )
{
	assert( _idx >=0 && _idx < getCount() );
	return points[ _idx ];
}

void CPath2008::Rotate (DistanceUnit degrees)
{
	for(int i=0; i<getCount(); ++i )
	{
		points[i].rotate( degrees );
	}
}

void CPath2008::RemovePointAt( int _iIdx )
{
	ASSERT( _iIdx>=0 && _iIdx< getCount() );
	int iTotalCount = getCount()-1;
	int i=0;
	CPoint2008* pNewPointList = new CPoint2008[ iTotalCount ];
	for( i=0; i< _iIdx ; ++i )
	{
		CPoint2008 tempPoint = points[i];
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}

	for( i= _iIdx; i< iTotalCount ; ++i )
	{
		CPoint2008 tempPoint = points[i+1];
		pNewPointList[i].setPoint( tempPoint.getX(), tempPoint.getY(), tempPoint.getZ() );
	}
	init( iTotalCount , pNewPointList );
	delete [] pNewPointList;
}

void CPath2008::ConnectPath(const CPath2008& path)
{
	points.insert(points.end(),path.points.begin(),path.points.end());

	//int iTotalCount = getCount()+path.getCount();
	//CPoint2008* pNewPointList = new CPoint2008[ iTotalCount ];
	//std::copy(points, points + getCount(), pNewPointList);
	//int iPoints = getCount();
	//for (int i = 0; i < path.getCount(); i++)
	//{
	//	pNewPointList[iPoints++] = path.getPoint(i);
	//}

	//init( iTotalCount , pNewPointList );
	//delete [] pNewPointList;
}

void CPath2008::insertPointAfterAt(const CPoint2008 & _p,int _iIdx){

	ASSERT( _iIdx>=0 && _iIdx< getCount() );
	points.insert(points.begin()+_iIdx+1,_p);

	/*int iTotalCount = getCount()+1;
	CPoint2008* pNewPointList = new CPoint2008[ iTotalCount ];
	std::copy(points, points + _iIdx + 1, pNewPointList);
	pNewPointList[_iIdx + 1] = _p;
	std::copy(points + _iIdx + 1, points + getCount(), pNewPointList + _iIdx + 2);

	init( iTotalCount , pNewPointList );
	delete [] pNewPointList;*/
}

void CPath2008 ::insertPointBeforeAt(const CPoint2008 & _p,int _iIdx)
{
	ASSERT( _iIdx>=0 && _iIdx<= getCount() );
	points.insert(points.begin()+_iIdx,_p);
	/*int nTotalCount = getCount() + 1;
	CPoint2008* pNewPointList = new CPoint2008[nTotalCount];
	std::copy(points, points + _iIdx, pNewPointList);
	pNewPointList[_iIdx] = _p;
	std::copy(points + _iIdx, points + getCount(), pNewPointList + _iIdx + 1);

	init( nTotalCount , pNewPointList );
	delete [] pNewPointList;*/
}

DistanceUnit CPath2008::GetTotalLength()const
{
	if( getCount() <= 1 )
	{
		return 0.0;
	}

	DistanceUnit dLength =0.0;
	for(int i=0,j=1; j<getCount(); ++i,++j)
	{
		dLength += points[i].distance3D( points[j] );
	}

	return dLength;
}

// get the path's fact length
DistanceUnit CPath2008::getFactLength( const std::vector<double>& vRealAltitude ) const
{
	if( getCount() <=1 )
		return 0.0;

	DistanceUnit _dLength = 0.0;
	for( int i=1; i< getCount(); i++ )
	{
		_dLength += getFactLength( points[i-1], points[i],vRealAltitude );
	}

	return _dLength;
}

// get two points fact distance
DistanceUnit CPath2008::getFactLength( const CPoint2008& _pt1, const CPoint2008& _pt2 ,const std::vector<double>& vRealAltitude) const
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


	CPoint2008 _tmpPt( _dHorizonDistance, _dVerticalDistance, 0.0 );

	return _tmpPt.length();
}

// get max angle
double CPath2008::getMaxAngle( const std::vector<double>& vRealAltitude  ) const
{
	double _dMaxAngle = 0.0;

	for( int i=1; i< getCount(); i++ )
	{
		// get distance and horizon distance
		DistanceUnit _distance			= getFactLength( points[i-1], points[i],vRealAltitude);
		DistanceUnit _dVerticalDistance = getFactLength( CPoint2008( 0.0, 0.0, points[i-1].getZ()), CPoint2008( 0.0, 0.0,points[i].getZ() ),vRealAltitude );

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
bool CPath2008::zoomPathByLength( DistanceUnit _newLength, CString& _strFailedReason,const std::vector<double>& vRealAltitude  )
{
	if( getCount()<=1 )
	{
		_strFailedReason = "the number of point less than 1";
		return false;
	}

	//temp path, to save new points
	CPath2008 _newPath;
	_newPath.init( getCount() );

	// old fact length
	DistanceUnit _oldLength = getFactLength(vRealAltitude );		
	if( _oldLength == 0.0 )
	{
		DistanceUnit _dSegLength = _newLength/getCount();
		for( int i=0; i< getCount(); i++ )
			_newPath[i] = CPoint2008( points[i].getX() + _dSegLength, points[i].getY(), points[i].getZ() );

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
	for( int i=1; i< getCount(); i++ )
	{
		// get new distance of the two point
		_distance = getFactLength( points[i-1], points[i], vRealAltitude);
		_distance *= _dZoomPercent;

		// get their vertical high
		_dVerticalDistance = getFactLength( CPoint2008(0.0, 0.0, points[i-1].getZ()), CPoint2008( 0.0, 0.0, points[i].getZ() ) , vRealAltitude);

		if( _distance < _dVerticalDistance )
		{
			_strFailedReason.Format("the vertical distance of point %d and point %d is too long, can not zoom the path", i-1, i );
			return false;
		}

		// get horizon distance
		long double _sqrt = _distance * _distance -_dVerticalDistance * _dVerticalDistance;
		_dHorizonDistance = sqrtl( _sqrt );

		// get the new point
		CPoint2008 _tmpVector( CPoint2008( points[i-1].getX(), points[i-1].getY(), 0.0 ),
			CPoint2008( points[i].getX(), points[i].getY(), 0.0 ) );
		_tmpVector.length( _dHorizonDistance );
		CPoint2008 _newPoint = CPoint2008( _newPath[i-1].getX(), _newPath[i-1].getY(), 0.0 ) + _tmpVector;
		_newPoint.setZ( points[i].getZ() );

		// add the new point to new path
		_newPath[i] = _newPoint;
	}

	// change the old path
	init( _newPath );
	_strFailedReason = " zoom succeed!";
	return true;	
}

//2d
bool CPath2008::within(CPoint2008 pt)
{
	if (getCount() == 1)	return false;

	for(int i=1; i<getCount(); i++)
	{
		CLine2008 ln(points[i-1],points[i]);
		if (true == ln.containsex(pt))
			return true;
	}
	return false;

}

bool CPath2008::IsVertex(CPoint2008 pt)
{
	for(int i=0; i<getCount(); i++)
	{
		if (pt.distance(points[i]) < 1.0)//!!!
			return true;
	}
	return false;
}
//2d
bool CPath2008::intersects(const CPath2008* pDest)const
{
	for (int i=1; i<getCount(); i++) 
	{
		CLine2008 ln1(points[i-1],points[i]);
		for (int j=1; j<pDest->getCount(); j++) 
		{
			CLine2008 ln2(pDest->getPoint(j-1), pDest->getPoint(j));
			CPoint2008 retPt;
			if (ln1.intersects(ln2, retPt))
				return true;
		}
	}
	return false;
}

//CPath2008::CPath2008( const CPath2008 &path )
//{
//	getCount() = 0;
//	points = NULL;
//	init(path.getCount(),path.points);
//}

CPath2008::CPath2008(const Path &path)
{
	/*getCount() = 0;
	points = NULL;

	if (getCount() != path.getCount())
	{
		clear();
		getCount() = path.getCount(); 
		CPoint2008 aPoint;
		points = aPoint.makeArray (getCount()); 

		for (int i=0; i<getCount(); i++)
		{
			points[i].setPoint();
		}
	}*/
	points.reserve(path.getCount());
	for(int i=0;i<path.getCount();i++){
		CPoint2008 ap(path.getPoint(i).getX(), path.getPoint(i).getY(), path.getPoint(i).getZ());
		points.push_back(ap);
	}
}

bool CPath2008::GetSnapPoint( double width, const CPoint2008& pt, CPoint2008& reslt )const
{
	int nSegIndex = -1;
	return GetSnapPoint(width, pt, reslt, nSegIndex);
}


bool CPath2008::GetSnapPoint( double width, const CPoint2008& pt, CPoint2008& reslt, int& nSegIndex )const
{
	double mindist  = (std::numeric_limits<double>::max)();
	nSegIndex = -1;

	for(int i = 0;i < getCount() - 1; i++)
	{
		CLine2008 line( getPoint(i), getPoint(i + 1) );
		double dist = line.PointDistance(pt);
		if (dist < mindist)
		{
			mindist = dist; 
			reslt = line.getProjectPoint(pt);
			if(!line.contains(reslt))
				reslt = line.nearest(reslt);
			nSegIndex = i;
		}
	}
	return mindist < width*0.5;
}


bool CPath2008::GetDistPoint( double dist, CPoint2008& pt ) const
{
	//if(getCount()<1) return false;
	if(getCount()<2) return false;

	double accdist = 0;
	double thisdist = 0;
	int i=0 ;
	for(i=0 ;i< getCount()-1;i++){
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

CPoint2008 CPath2008::GetDistPoint( double dist ) const
{
	//ASSERT(getCount()>1);
	double fIndex = GetDistIndex(dist);
	return GetIndexPoint(fIndex);
}

bool CPath2008::GetIndexPoint( double dist, CPoint2008& pt ) const
{
	int npt = (int)dist;
	double offset = dist - npt;

	if(npt < 0) return false;

	if( npt > getCount()-1 ) return false;
	if(npt == getCount()-1){
		pt= getPoint(npt);
		return true;
	}
	pt = getPoint(npt) * (1-offset) + getPoint(npt+1)*offset;
	return true;
}

CPoint2008 CPath2008::GetIndexPoint( double dist ) const
{

	ASSERT(dist >=0 );
	ASSERT(dist < getCount() );

	int npt = (int)dist;
	double offset = dist - npt;

	if(npt < 0){
		npt = 0;
		offset = 0;
	}
	if(npt >= getCount() - 1 ){
		npt = getCount() - 2;
		offset = 1;
	}
	if(npt>=0 && npt<getCount()-1)
	{
		return getPoint(npt) * (1-offset) + getPoint(npt+1)*offset;	
	}
	return CPoint2008(0,0,0);
}

double CPath2008::GetDistIndex( double realDist ) const
{
	if(getCount()<2) return 0;

	double accdist = 0;
	double thisdist = 0;
	int i=0;
	for( i=0 ;i< getCount()-1;i++){
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

	return i - (double)exceed;
}

double CPath2008::GetIndexDist( double relateDist ) const
{
	if(relateDist < 0) return -1;
	if(getCount() < 2) return 0;

	if(relateDist >= getCount() -1 ) return GetTotalLength();

	int npt = (int)relateDist;
	double offset = relateDist - npt;

	double len = 0;
	for(int i=0;i< npt;i++){
		len += getPoint(i).distance3D( getPoint(i+1) );
	}	
	len += getPoint(npt).distance3D(getPoint(npt+1)) * offset;
	return len;

}

ARCVector3 CPath2008::GetIndexDir( double dist ) const
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

int CPath2008::GetInPathPoints( double dFrom, double dTo, std::vector<CPoint2008>& vPts ) const
{

	int nAdded = 0;
	double dAccLen = 0;
	bool bReveres = (dFrom > dTo);

	if(bReveres)
		std::swap(dFrom,dTo);

	std::vector<CPoint2008> vInsertPts;

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

bool CPath2008::IsSameAs(const CPath2008& otherPath)
{
	if(getCount() != otherPath.getCount())
		return false;
	for (int i=0; i<getCount(); i++)
	{
		if ( points[i].getX()!=otherPath.getPoint(i).getX()
			|| points[i].getY()!=otherPath.getPoint(i).getY()
			|| points[i].getZ()!=otherPath.getPoint(i).getZ() )
		{
			return false;
		}
	}
	return true;
}

void CPath2008::PathSupplement(CPath2008& otherPath)
{
	if (getCount()<3)
	{
		return;
	}
	int elementCount = getCount()+2;
	CPoint2008 * m_vPoints = new CPoint2008[elementCount];
	for (int i=0; i<getCount(); i++)
	{
		m_vPoints[i].setPoint(points[i].getX(),points[i].getY(),points[i].getZ());
	}
	m_vPoints[getCount()].setPoint(points[0].getX(),points[0].getY(),points[0].getZ());
	m_vPoints[getCount()+1].setPoint(points[1].getX(),points[1].getY(),points[1].getZ());
	otherPath.init(elementCount,m_vPoints);
	delete [] m_vPoints;
}

//double CPath2008::GetPointDistInPath(const CPoint2008& pt )const
//{
//	double accDist =0;
//	int i=0;
//	for(i=0;i<getCount()-1;i++)
//	{
//		ARCVector3 v1 = pt - getPoint(i);
//		ARCVector3 v2 = pt - getPoint(i+1);
//		if( v1.GetCosOfTwoVector(v2) <-0.999 )
//		{
//			return accDist + v1.Length();
//		}
//		else
//		{
//			accDist += (getPoint(i+1)-getPoint(i)).setle();
//		}
//	}
//	return accDist;
//}
 

CPath2008 CPath2008::GetSubPath( DistanceUnit dfrom ,DistanceUnit dto )const
{
	bool bswaped = false;
	if(dfrom > dto){
		std::swap(dfrom, dto);
		bswaped = true;
	}

	std::vector<CPoint2008> vSubPath;
	double relatPos1 = GetDistIndex(dfrom);
	double relatPos2 = GetDistIndex(dto);
	//if(relatPos1<0) relatPos1 = 0;
	//if(relatPos2>pathsrc.getCount()-1) relatPos2 =(float) pathsrc.getCount()-1;
	ASSERT(relatPos1>-1 && relatPos1 <= getCount());
	ASSERT(relatPos2>-1 && relatPos2 <= getCount());

	int piFrom =(int) relatPos1; int piTo =(int)relatPos2;

	ASSERT(piFrom <= piTo);

	vSubPath.reserve(piTo -piFrom + 2);
	CPoint2008 DistPt;
	DistPt = GetDistPoint(dfrom);
	vSubPath.push_back(DistPt);
	for(int j = (int)relatPos1+1; j<relatPos2; j++)
	{
		vSubPath.push_back( getPoint(j) );			
	}
	DistPt = GetDistPoint(dto);
	vSubPath.push_back(DistPt);

	if(bswaped) std::reverse(vSubPath.begin(),vSubPath.end());

	CPath2008 restl;
	restl.init(vSubPath.size(), &vSubPath[0]);
	return restl;
}

CPath2008 CPath2008::GetLeftPath( DistanceUnit dfrom ) const
{
	std::vector<CPoint2008> vSubPath;
	double relatPos1 = GetDistIndex(dfrom);
	ASSERT(relatPos1>-1 && relatPos1 <= getCount());

	CPoint2008 DistPt = GetDistPoint(dfrom);
	vSubPath.push_back(DistPt);
	for(int j = (int)relatPos1+1; j<getCount(); j++)
	{
		vSubPath.push_back( getPoint(j) );			
	}
	CPath2008 result;
	result.init(vSubPath.size(),&vSubPath[0]);
	return result;
}

bool CPath2008::bPointInPath( const CPoint2008& pt, double dBuffer /*= ARCMath::EPSILON */ ) const
{
	DistancePointPath3D distPtPath;
	return distPtPath.GetSquared(pt,*this) < dBuffer*dBuffer;
	
}

double CPath2008::GetPointDist( const CPoint2008& pt ) const
{
	DistancePointPath3D distPtPath;
	distPtPath.GetSquared(pt,*this);
	return  GetIndexDist(distPtPath.m_fPathParameter);
}

const CPoint2008& CPath2008::operator[]( int _idx )const
{
	assert( _idx >=0 && _idx < getCount() );
	return points[ _idx ];

}

ARCVector3 CPath2008::GetDistDir( double dist ) const
{
	return GetIndexDir(GetDistIndex(dist));	
}

DistanceUnit GetProjectDist(const CPoint2008& pos1, const CPoint2008& pos2, const CPoint2008& posProj)
{
	ARCVector3 dirLine = pos2-pos1;
	ARCVector3 dirProj = posProj - pos1;
	return dirLine.dot(dirProj)/dirLine.Length();
}

DistanceUnit CPath2008::GetClosestPtDist( const CPoint2008& pos, DistanceUnit distF /*= 0*/ ) const
{
	if(getCount())
	{
		CPoint2008 closestPt = GetDistPoint(distF);
		DistanceUnit minDist= pos.distance(closestPt);
		DistanceUnit closestDist = distF;

		DistanceUnit accDist =0;
		for(int i=1;i<(int)getCount();i++)
		{
			const CPoint2008& p1 = points[i-1];
			const CPoint2008& p2 = points[i];
			DistanceUnit accDistPre = accDist;
			accDist+=p1.distance3D(p2);
			
			if(accDist<distF)
				continue;

			
			DistanceUnit dist = GetProjectDist(p1,p2,pos) + accDistPre;
			dist = MIN(dist,accDist);
			dist = MAX(dist,accDistPre);

			if(dist<distF)
				continue;

			if(accDist>accDistPre)
			{
				double drat  = (dist - accDistPre)/(accDist - accDistPre);
				CPoint2008 prjPt = p1*(1.0f-drat) + p2*drat;
				DistanceUnit ptDist = prjPt.distance(pos);
				if(ptDist<=minDist)
				{
					closestPt = prjPt;
					minDist = ptDist;
					closestDist = dist;
				}
			}
		}
		return closestDist;
	}
	return distF;
}

float CPath2008::GetPointIndex( const CPoint2008&pt,CPoint2008& projPt ) const
{
	DistancePointPath3D distPtPath;
	distPtPath.GetSquared(pt,*this);
	projPt = distPtPath.m_clostPoint;
	return  (float)distPtPath.m_fPathParameter;

}

double CPath2008::GetPointIndex( const CPoint2008&pt ) const
{
	DistancePointPath3D distPtPath;
	distPtPath.GetSquared(pt,*this);	
	return distPtPath.m_fPathParameter;
}

CPath2008 CPath2008::GetSubPathIndex( double relatPos1, double relatPos2 ) const 
{
	bool bswaped = false;
	if(relatPos1 > relatPos2){
		std::swap(relatPos1, relatPos2);
		bswaped = true;
	}
	std::vector<CPoint2008> vSubPath;
	//if(relatPos1<0) relatPos1 = 0;
	//if(relatPos2>pathsrc.getCount()-1) relatPos2 =(float) pathsrc.getCount()-1;
	ASSERT(relatPos1>-1 && relatPos1 <= getCount());
	ASSERT(relatPos2>-1 && relatPos2 <= getCount());

	int piFrom =(int) relatPos1; int piTo =(int)relatPos2;

	ASSERT(piFrom <= piTo);

	vSubPath.reserve(piTo -piFrom + 2);
	CPoint2008 DistPt;
	DistPt = GetIndexPoint(relatPos1);
	vSubPath.push_back(DistPt);
	for(int j = (int)relatPos1+1; j<relatPos2; j++)
	{
		vSubPath.push_back( getPoint(j) );			
	}
	DistPt = GetIndexPoint(relatPos2);
	vSubPath.push_back(DistPt);

	if(bswaped) std::reverse(vSubPath.begin(),vSubPath.end());

	CPath2008 restl;
	restl.init(vSubPath.size(), &vSubPath[0]);
	return restl;
}

int CPath2008::GetSegmentNearPos(CPoint2008& pos)
{
	int i = 0, index = -1;
	double minValue = ARCMath::DISTANCE_INFINITE, upValue = 0.0;
	for (; i < getCount() -1; i++)
	{
		if (((points[i].x < points[i+1].x) ? (pos.x > points[i].x ? ( pos.x < points[i+1].x ) : 0) : (pos.x < points[i].x ? ( pos.x > points[i+1].x ) : 0))
		   ||((points[i].y < points[i+1].y) ? (pos.y > points[i].y ? ( pos.y < points[i+1].y ) : 0) : (pos.y < points[i].y ? ( pos.y > points[i+1].y ) : 0)))
		{
			upValue =  std::abs( (points[i] - pos).dot((points[i]-points[i+1]).PerpendicularLCopy().Normalize()) );
			if ( upValue < minValue ) {
				minValue = upValue;
				index = i;
			}
		}
	}

	if (((points[getCount()-1].x < points[0].x) ? (pos.x > points[getCount()-1].x ? ( pos.x < points[0].x ) : 0) : (pos.x < points[getCount()-1].x ? ( pos.x > points[0].x ) : 0))
	   ||((points[getCount()-1].y < points[0].y) ? (pos.y > points[getCount()-1].y ? ( pos.y < points[0].y ) : 0) : (pos.y < points[getCount()-1].y ? ( pos.y > points[0].y ) : 0)))
	{
		//upValue =  std::abs( (points[getCount()-1] - pos).crossProduct(pos - points[0]).z );
		ARCVector3 n( points[getCount()-1].y-points[0].y, -points[getCount()-1].x + points[0].x, points[0].z);
		n.Normalize();
		upValue = (pos - points[0]).dot(n);
		upValue = std::abs(upValue);
		if ( upValue < minValue ) {
			minValue = upValue;
			index = getCount() - 1;
		}
	}

	return index;
}

CPoint2008 CPath2008::GetDistPointEx( double dist ) const
{
	if(getCount() < 1)
		return CPoint2008(0,0,0);
	if(getCount() ==1)
		return getPoint(0);

	double accdist = 0;
	double thisdist = 0;
	CPoint2008 orignPt = getPoint(0) ;
	ARCVector3 vDir = ARCVector3::ZERO;

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
		vDir = getPoint(i) - getPoint(i+1);
	}
	else if(i>getCount()-1)
	{
		vDir = getPoint(i-1) - getPoint(i-2);
	}
	else
	{
		vDir = getPoint(i-1) - getPoint(i);
	}

	vDir.Normalize();


	double exceed = accdist - dist;


	return orignPt + vDir * exceed;
}

bool CPath2008::GetEndPoint( CPoint2008& pos ) const
{
	if(!points.empty()){
		pos = *points.rbegin();
		return true;
	}
	return false;
}

const CPoint2008 * CPath2008::getPointList( void ) const
{
	if(points.empty())
		return NULL;
	return &points[0];
}

CPoint2008* CPath2008::getPointList()
{
	if(points.empty())
		return NULL;
	return &points[0];
}

void CPath2008::push_back( const CPoint2008& p )
{
	points.push_back(p);
}

void CPath2008::split( DistanceUnit ds, CPath2008& p1,CPath2008&p2 )
{

}

CPath2008 CPath2008::GetSubPathToEndIndex( double beginIndex ) const
{
	CPath2008 ret;
	int iIndex = (int)beginIndex;
	if(iIndex >=0 && iIndex < getCount()-1 )
	{
		const CPoint2008& p1 = points.at(iIndex);
		const CPoint2008& p2 = points.at(iIndex+1);
		double frat =  beginIndex - iIndex;
		ret.push_back( p1*(1-frat)+p2*frat );
	}

	for(int i = iIndex+1;i<getCount();i++)
	{
		ret.push_back(points.at(i));
	}

	return ret;	
}

CPath2008 CPath2008::GetSubPathFromBeginIndex( double endIndex ) const
{
	CPath2008 ret;
	int iIndex = (int)endIndex;
	for(int i = 0;i<=iIndex;i++)
	{
		ret.push_back(points.at(i));
	}
	if(endIndex > iIndex && iIndex < getCount()-1 && iIndex>=0)
	{
		const CPoint2008& p1 = points.at(iIndex);
		const CPoint2008& p2 = points.at(iIndex+1);
		double frat =  endIndex - iIndex;
		ret.push_back( p1*(1-frat)+p2*frat );
	}

	return ret;	
}

CPath2008 CPath2008::operator+( const CPath2008& path ) const
{
	CPath2008 ret;
	ret = *this;
	ret.points.insert(ret.points.end(),path.points.begin(),path.points.end());
	return ret;
}

CPath2008& CPath2008::operator+=( const CPath2008& path )
{
	points.insert(points.end(),path.points.begin(),path.points.end());
	return *this;
}

bool CPath2008::GetBeginPoint( CPoint2008& pos ) const
{
	if(!points.empty())
	{
		pos = *points.begin();
		return true;
	}
	return false;
}

void CPath2008::Shrink( DistanceUnit leftDist )
{
	if(getCount()<2)
		return;

	DistanceUnit dist = 0;
	int i = 0;
	for( i=1;i<getCount();i++)
	{
		if(dist>leftDist)
		{
			break;
		}
		DistanceUnit _dist = getPoint(i-1).distance3D(getPoint(i));
		dist+= _dist;		
	}
	points.erase(points.begin()+i,points.end());
	return;
	
}

CPath2008& CPath2008::Append( const CPath2008& other )
{
	points.insert(points.end(), other.points.begin(), other.points.end());
	return *this;
}

Side CPath2008::getPointSide( const CPoint2008& pt ) const
{
	double dIndex  = GetPointIndex(pt);
	ARCVector3 dir = GetIndexDir(dIndex);
	CPoint2008 ptInPath = GetIndexPoint(dIndex);

	ARCVector3 ptDir  = pt - ptInPath;

	double dx = dir.cross(ptDir).z;
	if(dx>0)
		return _Left;

	if(dx<0)
		return _Right;

	return _Inside;
}


bool CPath2008::getPolyCenter( CPoint2008&pt ) const
{
	if(getCount()<=0)
		return false;

	CPoint2008 ptAll;
	for(int i=0;i<getCount();++i)
	{
		ptAll += getPoint(i);
	}
	ptAll *= (1.0 / getCount());
	pt= ptAll;
	return true;
}
