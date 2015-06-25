#pragma once
#include "path2008.h"
#include "point2008.h"
#include <vector>

class CPollygon2008 : public CPath2008
{
public:
	CPollygon2008(void);
	virtual ~CPollygon2008(void);

	virtual void init (int pointCount, const CPoint2008 *pointList);

	// use even_odd rule to determine if the Polygon contains c
	int contains (const CPoint2008& pnt) const;
	int containsExcludeEdge( const CPoint2008& _pt) const;

	bool contains2(const CPoint2008& pt)const;

	// test a point if is a edge point
	bool isEdgePoint( const CPoint2008& _pt, int& _iSeg ) const;

	bool contains ( const CPollygon2008& _another ) const;
	bool IsCountClock(){calculateArea();return m_bcountClockwise;}
	bool IsCountClock()const;
	// test Line (p1, p2) for intersection with any polygon segment
	int intersects (const CPoint2008& p1, const CPoint2008& p2) const;

	bool IsOverLap( const CPoint2008& p1, const CPoint2008& p2) const;

	bool IfOverlapWithOtherPolygon( const CPollygon2008& _otherPlygon )const;

	// returns total area of the polygon
	double calculateArea (void);
	double getArea()const;

	// test if this pollygon is a contex pollygon
	bool IsAConvexPollygon()const;

	bool IfLineInsidePollygon( const CPoint2008& p1, const CPoint2008& p2 )const;

	void DividAsConvexPollygons( std::vector<CPollygon2008>& _vResult )const;

	void SetBoundsPoint();

	// returns a random point within the polygon
	virtual CPoint2008 getRandPoint (DistanceUnit area = 0.0) const;

	// do offset to all the position related data.
	virtual void DoOffset( DistanceUnit _xOffset,  DistanceUnit _yOffset,  DistanceUnit _zOffset);
	virtual void Rotate (DistanceUnit degrees);

	// perform scale transformation on receiver
	void scale (double sx, double sy, double sz = 1.0);

	void Zoom(DistanceUnit _dX, DistanceUnit _dY, DistanceUnit _dZ = 1.0 );

	void GetBoundsPoint(CPoint2008& ptBottomLeft,CPoint2008& ptTopRight)const;

protected:
	// bounding box
	CPoint2008 bottomLeft, topRight;
	bool m_bcountClockwise;
};

typedef std::vector<CPollygon2008> POLLYGON2008VECTOR;
