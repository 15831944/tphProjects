#ifndef POLYGON_H
#define POLYGON_H

#include "common\point.h"
#include "common\path.h"
#include <vector>
#include "../Database/ADODatabase.h"
class Pollygon : public Path
{
protected:
    // bounding box
    Point bottomLeft, topRight;
	bool m_bcountClockwise;
    int m_ID ;
public:
    virtual ~Pollygon () {};

    virtual void init (int pointCount, const Point *pointList);

    // use even_odd rule to determine if the Polygon contains c
    int contains (const Point& pnt) const;
	int containsExcludeEdge( const Point& _pt) const;
	
	// test a point if is a edge point
	bool isEdgePoint( const Point& _pt, int& _iSeg ) const;

	bool contains ( const Pollygon& _another ) const;
	bool IsCountClock(){calculateArea();return m_bcountClockwise;}
	bool IsCountClock()const;
	// test Line (p1, p2) for intersection with any polygon segment
    int intersects (const Point& p1, const Point& p2) const;

	bool IsOverLap( const Point& p1, const Point& p2) const;

	bool IfOverlapWithOtherPolygon( const Pollygon& _otherPlygon )const;

    // returns total area of the polygon
	double calculateArea (void);
	double getArea()const; //if countclockwise <0 

	// test if this pollygon is a contex pollygon
	bool IsAConvexPollygon()const;

	bool IfLineInsidePollygon( const Point& p1, const Point& p2 )const;

	void DividAsConvexPollygons( std::vector<Pollygon>& _vResult )const;

	void SetBoundsPoint();
	void GetBoundsPoint(Point& ptBottomLeft,Point& ptTopRight)const;

    // returns a random point within the polygon
    virtual Point getRandPoint (DistanceUnit area = 0.0) const;

	// do offset to all the position related data.
	virtual void DoOffset( DistanceUnit _xOffset,  DistanceUnit _yOffset,  DistanceUnit _zOffset);
	virtual void Rotate (DistanceUnit degrees);

	    // perform scale transformation on receiver
    void scale (double sx, double sy, double sz = 1.0);

	void Zoom(DistanceUnit _dX, DistanceUnit _dY, DistanceUnit _dZ = 1.0 );

    static void ReadDataFromDB(Pollygon* _polly,int _parentID) ;
	static void WriteDataToDB(Pollygon* _polly,int _parentID) ;
protected:
	static void GetDataSet(CADORecordset& _set,Pollygon* _polly) ;
	static void WriteData(Point _polly,int _parentID) ;
};

typedef std::vector<Pollygon> POLLYGONVECTOR;

#endif
