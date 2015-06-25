#pragma once
#include <stdio.h>
#include <vector>
#include "point2008.h"
#include "path.h"

typedef std::pair< CPoint2008, int >  Point2008IdxPair;
enum Side
{
	_Left,
	_Right,
	_Inside,
};

class CPath2008
{
public:	
	CPath2008(){}
	CPath2008(const Path &path);

	inline void Resize(int ncout){ points.resize(ncout); }
	
	CPoint2008& operator[] ( int _idx );
	const CPoint2008& operator[] ( int _idx )const;
	
	void init (const CPath2008& pointList);
	void init (int pointCount);
	virtual void init (int pointCount, const CPoint2008 *pointList);
	void clear (void);

	int getCount (void) const { return (int)points.size(); };
	const CPoint2008 *getPointList (void) const;
	CPoint2008* getPointList();

	CPoint2008 getPoint (int index) const;

	void insertPointAfterAt(const CPoint2008 & _p,int _iIdx);
	void insertPointBeforeAt(const CPoint2008 & _p,int _iIdx);
	void ConnectPath(const CPath2008& path);
	void push_back(const CPoint2008& p);
	void split(DistanceUnit ds, CPath2008& p1,CPath2008&p2);

	void RemovePointAt( int _iIdx );
	// flips the order of the points in the list
	void invertList (void);
	virtual void Rotate (DistanceUnit degrees);

	CPoint2008 getClosestPoint (const CPoint2008& pnt) const;
	virtual CPoint2008 getRandPoint (DistanceUnit area) const;
	void distribute (int pointCount, const CPoint2008 *pointList,
		DistanceUnit clearance);

	// do offset to all the position related data.
	virtual void DoOffset( DistanceUnit _xOffset,  DistanceUnit _yOffset,  DistanceUnit _zOffset);

	//do offset 
	virtual void DoOffset(const ARCVector3& pos);

	virtual void Mirror( const CPath2008* _p );

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
	bool within(CPoint2008 pt);
	// identify is path vertex
	bool IsVertex(CPoint2008 pt);
	// identify if intersect with dest path
	bool intersects(const CPath2008* pDest)const;

	//int GetAllocSize()const{ return numPoints *sizeof(CPoint2008);}

	// get two points fact distance
	DistanceUnit getFactLength( const CPoint2008& _pt1, const CPoint2008& _pt2 ,const std::vector<double>& vRealAltitude) const;

	//
	bool GetSnapPoint(double width, const CPoint2008& pt, CPoint2008& reslt)const;
	bool GetSnapPoint(double width, const CPoint2008& pt, CPoint2008& reslt, int& nSegIndex)const; // will also return the segment index, if failed, -1


	//all functions below are based on the first point in path
	// get the point distance from the first point: input: (0,length)
	bool GetDistPoint(double dist, CPoint2008& pt)const;   

	// get the point relate distance from the first point : input ( 0, point count-1);
	bool GetIndexPoint(double index, CPoint2008& pt)const;

	CPoint2008 GetIndexPoint(double dist)const;

	//get the direction at relate pos dist
	ARCVector3 GetIndexDir(double index)const;
	//get
	CPoint2008 GetDistPoint(double dist)const;
	//-----------------------------------------------------------------------------------------------------------
	//Summary:
	//		Get point in and out the path
	//-----------------------------------------------------------------------------------------------------------
	CPoint2008 GetDistPointEx(double dist)const;
	ARCVector3 GetDistDir(double dist)const;

	//get the relate distance from the real distance; 
	double GetDistIndex(double realDist)const;

	//get the real distance from the relate distance
	double GetIndexDist(double relateDist)const;

	//get sub points (not include dfrom ,dto)
	int GetInPathPoints(double dFrom, double dTo, std::vector<CPoint2008>& vPts)const;

	//identify if two path are same
	bool IsSameAs(const CPath2008& otherPath);

	//supply path to circle
	void PathSupplement(CPath2008& otherPath);

	//not used this use GetPointDist Instead
	//double GetPointDistInPath(const CPoint2008& pt)const;

	//test is the Point in path
	bool bPointInPath(const CPoint2008& pt, double dBuffer = 0.0001 )const;
	//get the distance in path
	double GetPointDist(const CPoint2008& pt)const;
	//get the pt index in path
	float GetPointIndex(const CPoint2008&pt,CPoint2008& projPt)const;

	//
	double GetPointIndex(const CPoint2008&pt)const;
	
	//get sub path
	CPath2008 GetSubPath(DistanceUnit from ,DistanceUnit to )const;
	CPath2008 GetSubPathIndex(double IndexFrom, double IndexTo)const;

	CPath2008 operator+(const CPath2008& path)const;
	CPath2008& operator+=(const CPath2008& path);
	//get left path from dist
	CPath2008 GetLeftPath(DistanceUnit from)const;

	//get closet pt [distF ,end]
	DistanceUnit GetClosestPtDist(const CPoint2008& pos, DistanceUnit distF = 0)const;

	//get segment idx close pos
	int GetSegmentNearPos(CPoint2008& pos);


	CPath2008 GetSubPathToEndIndex(double beginIndex)const;
	CPath2008 GetSubPathToEndDist(double beginDist)const{ return GetSubPathToEndIndex(GetDistIndex(beginDist)); }
	CPath2008 GetSubPathFromBeginIndex(double endIndex)const;
	CPath2008 GetSubPathFromBeginDist(double endDist)const{ return GetSubPathFromBeginIndex(GetDistIndex(endDist)); }


	//b
	bool GetBeginPoint(CPoint2008& pos)const;
	bool GetEndPoint(CPoint2008& pos)const;

	//erase behind pos left the distance 
	void Shrink(DistanceUnit leftDist);
	CPath2008& Append(const CPath2008& other);

	Side getPointSide(const CPoint2008& pt)const;

	bool getPolyCenter(CPoint2008&pt)const;
protected:
	virtual void Initialization(){}

public:
	//int numPoints;
	//CPoint2008* points;
	std::vector<CPoint2008> points;
};
