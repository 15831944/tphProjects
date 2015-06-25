#pragma once
#include <iostream>
#include <vector>
#include <math.h>
#include "template.h"
#include "util.h"
#include "VectorBase.h"

//#define _NM_TO_DIST_UNITS    185300l
//#define DIMENSIONS          2
#define COLUMNS             3

class ElapsedTime;
class CLine2008;
class ARCVector3;

class COMMON_TRANSFER CPoint2008 : public Vector3Base
{
public:
	friend CLine2008;
	// default constructor
	CPoint2008 ();
	CPoint2008 (DistanceUnit newX, DistanceUnit newY, DistanceUnit newZ)
	{ init (newX, newY, newZ); }
	
	explicit CPoint2008(const Vector3Base& v):Vector3Base(v){}

	ARCVector3 toVector()const;
public:
	void SetPoint( const CPoint2008& _pt );
	void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset, DistanceUnit _zOffset );
	//const CPoint2008& GetPoint() const;
	/*****
	*
	*   Constructors, initializing methods
	*
	*****/

	// copy constructor
	void init (DistanceUnit newX, DistanceUnit newY, DistanceUnit newZ )
	{ x = newX; y = newY; z = newZ; }	
	long peek (void) const;

	void getPoint (float& X, float& Y, short& Z) const;
	void getPoint (float& X, float& Y, float& Z) const;
	void getPoint (DistanceUnit& X, DistanceUnit& Y, DistanceUnit& Z) const
	{ X = x; Y = y; Z = z; }

	void setPoint (float X, float Y, int Z);
	void setPoint (DistanceUnit X, DistanceUnit Y, DistanceUnit Z) ;
	DistanceUnit getDistanceTo(const  CPoint2008  & pointTo);
	DistanceUnit getX (void) const { return x; }
	DistanceUnit getY (void) const { return y; }
	DistanceUnit getZ (void) const { return z; }

	//void Translated(DistanceUnit dx,DistanceUnit dy,DistanceUnit dz){
	//	x+=dx;
	//	y+=dy;
	//	z+=dz;
	//}


	CPoint2008& setX (DistanceUnit newX) { x = newX; return *this; }
	CPoint2008& setY (DistanceUnit newY) { y = newY; return *this; }
	CPoint2008& setZ (DistanceUnit newZ) { z = newZ; return *this; }

	void swap (CPoint2008& aPoint);

	CPoint2008 getMaxPoint (const CPoint2008& c) const
	{ return CPoint2008(MAX(x,c.x),MAX(y,c.y),MAX(z,c.z)); }
	CPoint2008 getMinPoint (const CPoint2008& c) const
	{ return CPoint2008(MIN(x,c.x),MIN(y,c.y),MIN(z,c.z)); }

	CPoint2008 operator * (double scale) const
	{ return CPoint2008 (x * scale, y * scale, z * scale); }
    
	CPoint2008& operator = (const CPoint2008 &p)
	{
		x=p.x;
		y=p.y;
		z=p.z;
		return *this;
	}

	CPoint2008 OffsetAtSameLevelAdd(const CPoint2008& p) const {return CPoint2008(x+p.x,y+p.y,z);} 
	CPoint2008 OffsetAtSameLevelMinus(const CPoint2008& p)const {return CPoint2008 (x-p.x,y-p.y,z); }

	CPoint2008 operator + (const CPoint2008 &p) const { return CPoint2008 (x+p.x,y+p.y,z+p.z); }
	//CPoint2008 operator - (const CPoint2008 &p) const { return CPoint2008 (x-p.x,y-p.y,z-p.z); }
	CPoint2008 operator -()const{ return CPoint2008(-x,-y,-z); } 
	int operator > (const CPoint2008 &p) const { return ((x > p.x) && (y > p.y) ); }
	bool operator < (const CPoint2008 &p) const { return ((x < p.x) && (y < p.y) ); }
	int operator == (const CPoint2008 &p) const  { return (x == p.x) && (y == p.y) ; }
	int operator != (const CPoint2008 &p) const { return ((x!=p.x) || (y!=p.y)); }
	void operator *= (double scale) { x *= scale; y *= scale;z*=scale ;}
	int operator ! (void) const;

	bool preciseCompare (const CPoint2008 &p) const
	{ return ((long)(1000.0 * x) == (long)(1000.0 * p.x) &&
	(long)(1000.0 * y) == (long)(1000.0 * p.y)); }

	/*****
	*
	*   Vector calculations
	*
	*****/

	// constructor to make a vector from p1 to p2
	CPoint2008 (const CPoint2008& from, const CPoint2008& to) { initVector (from, to); }

	void initVector (const CPoint2008& from, const CPoint2008& to);

	// _vector1 x _vector2
	void Vector1XVector2( const CPoint2008& _vector1, const CPoint2008& _vector2);

	// returns distance between p and receiver
	double distance (const CPoint2008& p) const;

	// returns distance from origin to receiver (length of vector)
	double length (void) const;

	// returns vector with opposite heading
	CPoint2008 inverse (void) const { return CPoint2008 (-x, -y, z); }

	// returns perpendicular vector over X axis
	CPoint2008 perpendicular (void) const { return CPoint2008 (-y, x, z); }
	CPoint2008 perpendicularY(void) const { return CPoint2008 (y, -x, z); }

	// perform scale transformation on receiver
	void scale (double sx, double sy, double sz = 1.0)
	{ x *= sx; y *= sy; z *= sz; }

	// perform rotation transformation on receiver
	CPoint2008& rotate (double degrees);

	CPoint2008 getRotate(double degrees)const;

	// 2D rotation around _point
	void rotate( double degrees, CPoint2008 _point );

	// returns heading, in degrees, 0 degrees + due north, inc clockwise
	double getHeading (void) const;

	// sets the length of the vector to len
	void length (double len);

	// adds distance to receiving vector
	void operator += (double distance) { length (this->length() + distance); };

	// adds point to receiving vector
	void operator += (CPoint2008 pnt) { x += pnt.x; y += pnt.y; }
	void operator -= (CPoint2008 pnt) { x -= pnt.x; y -= pnt.y; }

	//
	ARCVector3 operator - (const CPoint2008& pt)const;
	CPoint2008 operator - (const ARCVector3& v)const;

	CPoint2008 operator + (const ARCVector3& v)const;

	CPoint2008& operator += (const ARCVector3& v);
	CPoint2008& operator -= (const ARCVector3& v);

	// returns vector, equal to receiver plus distance
	CPoint2008 operator + (double distance) const;

	CPoint2008& operator=(const Vector3Base& b){  x = b.x; y=b.y; z=b.z; return *this;}
	// return dot of vector
	DistanceUnit operator * ( const CPoint2008& _pt ) const;

	// get cos angle of the two vectors.
	double GetCosOfTwoVector( const CPoint2008& _pt ) const;

	//dot product
	DistanceUnit dotProduct(const CPoint2008& pt)const;

	//cross product
	CPoint2008 crossProduct(const CPoint2008& pt)const;

	CPoint2008 getRandPoint (DistanceUnit radius) const;

	// return the closest point
	CPoint2008 getNearest (const CPoint2008 *pointList, short count) const;
	int getNearest (const std::vector<CPoint2008>& vPoints) const;

	CPoint2008* makeArray (unsigned count) const;

	CPoint2008 calcMidPoint (const CPoint2008& dest, DistanceUnit speed, const ElapsedTime& travelTime) const;

	int within (const CPoint2008& p1, const CPoint2008& p2, long p_buf = 0l) const;

	void printPoint (char *p_str, int p_precision = 0) const;

	// make the vector normalized.
	CPoint2008& Normalize();

	CPoint2008 getNorm()const;
	CPoint2008 getLengtDir(DistanceUnit dlen)const;

	friend std::ostream& operator << (std::ostream& out, const CPoint2008& point);

	DistanceUnit* c_v(){ return val; }  
	const DistanceUnit* c_v()const{ return val; }
	/**********************************************************
	*add some function, in these function, handle z coordinate.	
	**********************************************************/
	double distance3D( const CPoint2008& _point ) const;

	void initVector3D (const CPoint2008& from, const CPoint2008& to);

	double length3D (void) const;

	void length3D (double len);

	void scale3D( double scale );

	void plus( const CPoint2008& _point );


	// offset the point base on ptFrom, fDir, dDist
	void offsetCoords( CPoint2008 ptFrom, double dDir, DistanceUnit dDist );

	CPoint2008 GetDistancePoint(const CPoint2008& ptTo, double dDist);
	CPoint2008 GetDistancePointUnlimited(const CPoint2008& ptTo, double dDist);
};


