#ifndef PATH_H
#define PATH_H

#include <stdio.h>
#include <vector>
#include "point.h"

class CPath2008;

typedef std::pair< Point, int >  PointIdxPair;

class Path
{
protected:

    int numPoints;
	Point* points;

	virtual void Initialization(){}
	
public:

    Path() { points = NULL; numPoints = 0; };
    Path(const Path &path);
	Path(const CPath2008 &path);
    virtual ~Path() { clear(); };
	Path& operator = ( const Path& _anotherPath);
	Point& operator[] ( int _idx );
	const Point& operator[](int _idx)const;
    void init (const Path& pointList);
    void init (int pointCount);
    virtual void init (int pointCount, const Point *pointList);
    void clear (void) { if (numPoints) delete [] points; points = NULL;
        numPoints = 0; };

    int getCount (void) const { return numPoints; };
    Point *getPointList (void) const { return points; };
	Point getPoint (int index) const;
	void setPoint (const Point& pt, int index) const;

	void insertPointAfterAt(const Point & _p,int _iIdx);

	void RemovePointAt( int _iIdx );
    // flips the order of the points in the list
    void invertList (void);
	virtual void Rotate (DistanceUnit degrees);

    Point getClosestPoint (const Point& pnt) const;
    virtual Point getRandPoint (DistanceUnit area) const;
    void distribute (int pointCount, const Point *pointList,
        DistanceUnit clearance);

	// do offset to all the position related data.
	virtual void DoOffset( DistanceUnit _xOffset,  DistanceUnit _yOffset,  DistanceUnit _zOffset);

	virtual void Mirror( const Path* _p );
	
	DistanceUnit GetTotalLength()const;
	
	// get the path's fact length
	DistanceUnit getFactLength( const std::vector<double>& vRealAltitude  ) const;

	// get max angle
	double getMaxAngle( const std::vector<double>& vRealAltitude  ) const;

	// convert path, make its length to _newLength.
	// ( the z value of each point can not change )
	bool zoomPathByLength( DistanceUnit _newLength, CString& _strFailedReason,const std::vector<double>& vRealAltitude  );

	// convert path, make all angle to _newAngle
	// ( the length value can not change )
	//void convertPathByAngle( double _newAngle );

	// identify if point is in path 
	bool within(Point pt);
	// identify is path vertex
	bool IsVertex(Point pt);
	// identify if intersect with dest path
	bool intersects(const Path* pDest)const;

	int GetAllocSize()const{ return numPoints *sizeof(Point);}

	// get two points fact distance
	DistanceUnit getFactLength( const Point& _pt1, const Point& _pt2 ,const std::vector<double>& vRealAltitude) const;

	//
	bool GetSnapPoint(double width, const Point& pt, Point& reslt)const;

	
	//all functions below are based on the first point in path
	// get the point distance from the first point: input: (0,length)
	bool GetDistPoint(double dist, Point& pt)const;   

	// get the point relate distance from the first point : input ( 0, point count-1);
	bool GetRelateDistPoint(float dist, Point& pt)const;

	Point GetRelateDistPoint(float dist)const;

	//get the direction at relate pos dist
	Point GetDirection(float dist)const;
	//get
	Point GetDistPoint(double dist)const;


	//get the relate distance from the real distance; 
	float GetRelateDist(double realDist)const;

	//get the real distance from the relate distance
	double GetRealDist(float relateDist)const;

	//get sub points (not include dfrom ,dto)
	int GetInPathPoints(double dFrom, double dTo, std::vector<Point>& vPts)const;

	int getClosestPointIndex(const Point& point);

};

class CSegmentPath : public Path
{
public: 
	CSegmentPath();
	~CSegmentPath();

private:
	double	m_dPathLength;
	double	m_dSegmentLen;
	int		m_iSegmentCount;
	std::vector< PointIdxPair > m_segmentPath;
	std::vector< Point > m_slotsPoint;
	std::vector<int> m_vSegmentPathIndex;//every point in path include how many segment
public:
	void init( const Path& path, int _segCount );
	std::vector<Point> getNoFlagPointList( int _fromSeg, int _toSeg  ) const;
	std::vector< PointIdxPair > getFlagPointList( int _fromSeg, int _toSeg  ) const;
	Point getSlotPoint( int _idx ) const;

	bool getSegmentDrection(int nIdex,int& nFromFloor,int& nToFloor);
		
	double getPathLen( void ) const
	{
		return m_dPathLength;
	}
	
	double getSegLen( void ) const
	{
		return m_dSegmentLen;
	}
private:
	void splitPath( void );
	double getPathLength( void );
	void getSlotPoint( void );

};
#endif
