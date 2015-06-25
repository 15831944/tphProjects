#pragma once

#include "ARCVector.h"
#include <algorithm>
#include <set>
class Path;
class ARCLine3;
class ARCRay2;

class ARCPath : public std::vector<ARCPoint2>
{
public:
	ARCPath() {}
	ARCPath(const ARCPath& _rhs) : std::vector<ARCPoint2>(_rhs) {}
	ARCPath(const std::vector<ARCPoint2>& _rhs) : std::vector<ARCPoint2>(_rhs) {}
	ARCPath(const Path* _pPath);

	
	ARCPath& operator=(const ARCPath& _rhs);
	ARCPath& operator=(const std::vector<ARCPoint2>& _rhs);

	//flips the order of the points in the list
	void InvertList()
		{ std::reverse(begin(),end()); }
	//rotates path about the origin by _degrees
	void Rotate(DistanceUnit _degrees);

    const ARCPoint2& GetClosestPoint(const ARCPoint2& _p) const;
    void Distribute(const std::vector<ARCPoint2>& _vList, DistanceUnit _clearance);

	//offset each point in list
	virtual void DoOffset(DistanceUnit _xOffset, DistanceUnit _yOffset);
	virtual void DoOffset(const ARCVector2& _offset);
	virtual ARCPoint2 GetRandPoint(DistanceUnit _area) const;

	ARCPoint2 Centroid() const ;
    // returns total area of the polygon in units^2 (cm^2)
	DistanceUnit CalculateArea() const;
	//
	ARCVector2 GetIndexDir()const;

	void TransfromToRayCoordX( const ARCRay2& ray );

};


#include "path.h"
#include "Path2008.h"
class ARCPath3 : public std::vector<ARCPoint3> 
{
public:
	//
	ARCPath3(){}
	ARCPath3(const std::vector<ARCVector3>& veclist ) : std::vector<ARCVector3>(veclist){}
	Path getPath()const;
	CPath2008 getPath2008()const;	
	
	//get the point distance from the begin point,the start point must in the Path
	ARCVector3 getDistancePoint(ARCVector3 startPt, double dist)const;

	//get the Point distant from the path's first point
	ARCVector3 getDistancePoint(double dist) const;

	//get relative point to length 
	ARCVector3 getRelativePoint(double relateDist)const;
	
	//get the distance from the path's first pt to the endpt;
	double getPointDistance(ARCVector3 endPt)const;

	//************************************
	//below is the new interface for the project
	//use them often
	//************************************
	// new interface
	ARCPath3(const ARCPath3& path);
	ARCPath3(const CPath2008& path);

	ARCPath3& DoOffset(const ARCVector3 & doffset);
	ARCPath3& RotateXY(double degree, const ARCVector3& center);
	ARCPath3& RotateXY(double degree);

	ARCPath3& Reverse(){ std::reverse(begin(),end()); return *this; }

	DistanceUnit GetLength()const;
	//get the relate distance from the real distance; 
	double GetDistIndex(double realDist)const;
	double GetIndexDist(double fIndex)const;
	ARCPoint3 GetIndexPos(double fIndex)const;
	ARCVector3 GetIndexDir(double fIndex)const;
	ARCPoint3 GetDistPos(double realDist)const{ return GetIndexPos(GetDistIndex(realDist)); }
	ARCVector3 GetDistDir(double realDist)const{ return GetIndexDir(GetDistIndex(realDist)); }
	ARCPath3 GetSubPathToEndIndex(double beginIndex)const;
	ARCPath3 GetSubPathToEndDist(double beginDist)const{ return GetSubPathToEndIndex(GetDistIndex(beginDist)); }
	ARCPath3 GetSubPathFromBeginIndex(double endIndex)const;
	ARCPath3 GetSubPathFromBeginDist(double endDist)const{ return GetSubPathFromBeginIndex(GetDistIndex(endDist)); }
	void Append(const ARCPath3& otherPath){ insert(end(),otherPath.begin(),otherPath.end()); }
	ARCPath3 GetSubPathIndex(double beginIndex, double endIndex)const;
	ARCPath3 GetSubPathDist(double beginDist, double endDist)const
	{
		return GetSubPathIndex( GetDistIndex(beginDist), GetDistIndex(endDist) );
	}



	//get point projection position in path
	float GetProjectPosition(const ARCPoint3& p)const;

	// return intersects positions (relate to path)
	bool GetIntersection(const ARCPath3& otherPath, std::set<float>& intersections)const;
	//
	bool GetIntersection(const ARCLine3& line, std::set<float>& intersections)const;

	int getClosestPointId(const ARCPoint3& pt )const;


	//
	//get round angle path
	ARCPath3 GetRoundCornerPath(double dCornerRad)const;	


	//
	

};

