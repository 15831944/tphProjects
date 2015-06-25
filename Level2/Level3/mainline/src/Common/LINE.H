#ifndef LINE__H
#define LINE__H

#include "fsstream.h"
#include "point.h"
#include "util.h"
//#include "common\plane.h"
class ARCVector3;

class Line
{
protected:
    Point p1;
    Point p2;

public:

    Line () {};
    Line (const Point& P1, const Point& P2) { p1 = P1; p2 = P2; };
	Line( const ARCVector3& _p1, const ARCVector3& _p2);

	Point GetPoint1()const{ return p1; }
	Point GetPoint2()const{ return p2; }
	Point& GetPoint1(){ return p1; }
	Point& GetPoint2(){ return p2; }
	
    virtual ~Line () {};

    void init (const Point& P1, const Point& P2) { p1=P1; p2=P2; };

    Point intersection (const Line& line2) const;
	// version 2 of intersection
	bool intersection2 (const Line& line2, Point& intersectPoint) const;
    int intersects (const Line& line2) const;

    int intersects (const Point& p) const;
    int mayIntersect (const Line& line2) const;
    int contains (const Point& pnt) const;
    int isCornerPoint (const Point& pnt) const
        { return pnt.preciseCompare (p1) || pnt.preciseCompare (p2); }

    Point nearest (const Point& pnt) const;
    Point getMinPoint (void) const { return Point (MIN(p1.x,p2.x),MIN(p1.y,p2.y),1); };
    Point getMaxPoint (void) const { return Point (MAX(p1.x,p2.x),MAX(p1.y,p2.y),1); };
	double GetLineLength() const { return p1.distance( p2 ) ;};
	Point getInlinePoint( double relatePos){
		return p2 * relatePos +  p1*(1- relatePos );
	}

	// make a 90 perp vector from _point to this line
	// if the perp vector intersect with this line, return the intersection point
	// return TRUE for there is a result, FALSE for no result.
	bool GetIntersectPoint( const Point& _point, Point& _resPoint, DistanceUnit& _dLength ) const;

    friend std::ostream& operator << (std::ostream& s, const Line& l);

	int intersects (const Line& line2, Point& _resPt ) const;

	void GetCenterPoint( Point& _ptInterPoint )const;

	DistanceUnit GetZ() const{ return p1.getZ(); }
	
	bool containsex( const Point& p ) const;

	//Point intersects(const Plane& plane )const;

	// check a point if belong to the line
	bool comprisePt( const Point& _pt) const;
	Point getProjectPoint(const Point & _p)const;

	double PointDistance(const Point& pt)	const;

	Line& Offset(DistanceUnit dx, DistanceUnit dy, DistanceUnit dz);
	Line& Offset(const Point& off);

	// shift line in XY-plan, shift left when off>0.0, else right
	bool Shift(DistanceUnit off);
};


#endif
