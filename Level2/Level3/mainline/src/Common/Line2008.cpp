#include "StdAfx.h"
#include "line2008.h"
#include "stdafx.h"
#include "point2008.h"
#include "ARCVector.h"

CPoint2008 CLine2008::intersection (const CLine2008& line2) const
{
	//	ASSERT( p1.getZ() == line2.GetZ() );


	//line A;
	double a1 = p2.y - p1.y;
	double b1 = p1.x - p2.x;
	double c1 = p2.x * p1.y - p1.x * p2.y; //a1*x + b1*y + c1 = 0 is line 1

	//line B;
	double a2 = line2.p2.y - line2.p1.y;
	double b2 = line2.p1.x - line2.p2.x;
	double c2 = line2.p2.x * line2.p1.y - line2.p1.x * line2.p2.y; //a2*x + b2*y + c2 = 0 is line 2

	//just it;
	double denom = a1*b2 - a2*b1;
	if( 0 == denom )
	{		// no insection.		
		return CPoint2008 (1000000000l, 1000000000l, 0);
	}

	double xval = (b1*c2 - b2*c1)/denom;
	double yval = (a2*c1 - a1*c2)/denom;

	return CPoint2008 (xval, yval, GetZ() );

}
CPoint2008 CLine2008::getProjectPoint(const CPoint2008 & _p)const{
	if(comprisePt(_p))return _p;

	ARCVector3 vl=_p-p1;
	ARCVector3 vprj=p2-p1;

	DistanceUnit len=  vl.dot(vprj) / vprj.Length();
	vprj.SetLength(len);

	return p1+vprj;
}
int CLine2008::mayIntersect (const CLine2008& line2) const
{
	if (p1 == line2.p1 || p1 == line2.p2 || p2 == line2.p1 || p2 == line2.p2)
		return FALSE;
	return (getMaxPoint() > line2.getMinPoint() && getMinPoint() < line2.getMaxPoint());
}

int CLine2008::intersects (const CPoint2008& p) const
{
	return (p.within (p1, p2));
}

int CLine2008::intersects (const CLine2008& line2) const
{
	if (!mayIntersect (line2))
		return FALSE;

	CPoint2008 tempPnt (intersection (line2));
	return contains (tempPnt) && line2.contains (tempPnt);
}


int CLine2008::contains (const CPoint2008& pnt) const
{
	return pnt.within (p1, p2, 1l);
}

CPoint2008 CLine2008::nearest (const CPoint2008& pnt) const
{
	return ((p1.distance (pnt) < p2.distance (pnt))? p1: p2);
}

std::ostream& operator << (std::ostream& s, const CLine2008& l)
{
	return s << "From " << l.p1 << " to " << l.p2;
}

// make a 90 perp vector from _point to this line
// if the perp vector intersect with this line, return the intersection point
// return TRUE for there is a result, FALSE for no result.
bool CLine2008::GetIntersectPoint( const CPoint2008& _point, CPoint2008& _resPoint, DistanceUnit& _dLength ) const
{
	// check z
	ASSERT( p1.getZ() == _point.getZ() );

	// check if the point will have intersect perp line.
	CPoint2008 thisVector = CPoint2008( p1, p2 );

	CPoint2008 thatVector = CPoint2008( p1, _point );
	double dCosVal = thisVector.GetCosOfTwoVector( thatVector );
	if( dCosVal < 0 )
		return false;

	DistanceUnit dPtLenth = p1.distance( _point );
	_dLength = dPtLenth * dCosVal;
	if( _dLength > GetLineLength() )
		return false;

	thisVector.length( _dLength );
	_resPoint = p1 + thisVector;
	return true;
}


int CLine2008::intersects (const CLine2008& line2, CPoint2008& _resPt ) const
{
	if (!mayIntersect (line2))
		return FALSE;

	_resPt = intersection (line2);
	return contains( _resPt ) && line2.contains( _resPt );
}

void CLine2008::GetCenterPoint( CPoint2008& _ptInterPoint )const
{
	_ptInterPoint.x = p1.x + ( p2.x - p1.x ) / 2.0;
	_ptInterPoint.y = p1.y + ( p2.y - p1.y ) / 2.0;
	_ptInterPoint.z = p1.z + ( p2.z - p1.z ) / 2.0;
}

bool CLine2008::containsex( const CPoint2008& p ) const
{
	if( p.z != p1.z || p.z != p2.z )
		return false;

	if( p.preciseCompare( p1) || p.preciseCompare(p2) )
		return true;

	double d1,d2;
	if( (p.x-p1.x)==0.0 )
		d1 = 99999.0;
	else
		d1 = (p.y-p1.y)/(p.x-p1.x);

	if( (p.x-p2.x)==0.0 )
		d2 = 99999.0;
	else
		d2 = (p.y-p2.y)/(p.x-p2.x);

	long l1 = (long)(d1*1000);
	long l2 = (long)(d2*1000);
	if( l1 == l2 )
		return true;

	return false;
}

// check a point if belong to the line
bool CLine2008::comprisePt( const CPoint2008& _pt) const
{
	return containsex( _pt ) && contains( _pt );
}

//cal the distance to the point
double CLine2008::PointDistance( const CPoint2008& pt ) const
{
	CPoint2008 p = getProjectPoint(pt);
	if(contains(p)) return pt.distance(p);
	double len1 = pt.distance(p1);
	double len2 = pt.distance(p2);
	if( len2 > len1 ) return len1;else return len2;
}

CLine2008::CLine2008( const ARCPoint3& _p1, const ARCPoint3& _p2 )
{
	p1.setPoint(_p1[VX],_p1[VY],_p1[VZ]);
	p2.setPoint(_p2[VX],_p2[VY],_p2[VZ]);
}