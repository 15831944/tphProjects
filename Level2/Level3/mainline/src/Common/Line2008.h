#pragma once
#include "fsstream.h"
#include "point2008.h"
#include "util.h"

class ARCVector3;

class CLine2008
{
protected:
	CPoint2008 p1;
	CPoint2008 p2;

public:

	CLine2008 () {};
	CLine2008 (const CPoint2008& P1, const CPoint2008& P2) { p1 = P1; p2 = P2; };
	CLine2008( const ARCVector3& _p1, const ARCVector3& _p2);

	CPoint2008 GetPoint1()const{ return p1; }
	CPoint2008 GetPoint2()const{ return p2; }
	CPoint2008& GetPoint1(){ return p1; }
	CPoint2008& GetPoint2(){ return p2; }

	virtual ~CLine2008 () {};

	void init (const CPoint2008& P1, const CPoint2008& P2) { p1=P1; p2=P2; };

	CPoint2008 intersection (const CLine2008& line2) const;
	int intersects (const CLine2008& line2) const;

	int intersects (const CPoint2008& p) const;
	int mayIntersect (const CLine2008& line2) const;
	int contains (const CPoint2008& pnt) const;
	int isCornerPoint (const CPoint2008& pnt) const
	{ return pnt.preciseCompare (p1) || pnt.preciseCompare (p2); }

	CPoint2008 nearest (const CPoint2008& pnt) const;
	CPoint2008 getMinPoint (void) const { return CPoint2008 (MIN(p1.x,p2.x),MIN(p1.y,p2.y),1); };
	CPoint2008 getMaxPoint (void) const { return CPoint2008 (MAX(p1.x,p2.x),MAX(p1.y,p2.y),1); };
	double GetLineLength() const { return p1.distance( p2 ) ;};
	CPoint2008 getInlinePoint( double relatePos){
		return p2 * relatePos +  p1*(1- relatePos );
	}

	// make a 90 perp vector from _point to this line
	// if the perp vector intersect with this line, return the intersection point
	// return TRUE for there is a result, FALSE for no result.
	bool GetIntersectPoint( const CPoint2008& _point, CPoint2008& _resPoint, DistanceUnit& _dLength ) const;

	friend std::ostream& operator << (std::ostream& s, const CLine2008& l);

	int intersects (const CLine2008& line2, CPoint2008& _resPt ) const;

	void GetCenterPoint( CPoint2008& _ptInterPoint )const;

	DistanceUnit GetZ() const{ return p1.getZ(); }

	bool containsex( const CPoint2008& p ) const;

	//CPoint2008 intersects(const Plane& plane )const;

	// check a point if belong to the line
	bool comprisePt( const CPoint2008& _pt) const;
	CPoint2008 getProjectPoint(const CPoint2008 & _p)const;

	double PointDistance(const CPoint2008& pt)	const;
};