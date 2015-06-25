#ifndef POINT_H
#define POINT_H

#include <iostream>
#include <vector>
#include <math.h>
#include "template.h"
#include "util.h"

//#define DEGREES_TO_RADIANS  0.01745329251994
//#define RADIANS_TO_DEGREES  57.29577951308
#define _NM_TO_DIST_UNITS    185300l
#define DIMENSIONS          2
#define COLUMNS             3

class ElapsedTime;
class COMMON_TRANSFER Point
{
protected:

    friend Line;
	union{
		struct{
			double x, y, z;	// z is the floor. 000 for 0, 100 for 1, 200 for 2
		};
		double val[3];
	};

public:
	void SetPoint( const Point& _pt );
	void DoOffset( double _xOffset, double _yOffset, double _zOffset );
	const Point& GetPoint() const;

    Point () { x = y = z = (double)0; }
    Point (double newX, double newY, double newZ)
        { init (newX, newY, newZ); }
    void init (double newX, double newY, double newZ ){ x = newX; y = newY; z = newZ; }

    Point (const Point& p) { x = p.x; y = p.y; z = p.z; }
    long peek (void) const;

    void getPoint (float& X, float& Y, short& Z) const;
	void getPoint (float& X, float& Y, float& Z) const;
    void getPoint (double& X, double& Y, double& Z) const
        { X = x; Y = y; Z = z; }

    void setPoint (float X, float Y, int Z);
	void setPoint (double X, double Y, double Z) ;
	double getDistanceTo(const  Point  & pointTo);
    double getX (void) const { return x; }
    double getY (void) const { return y; }
    double getZ (void) const { return z; }

	void Translated(double dx,double dy,double dz){
		x+=dx;
		y+=dy;
		z+=dz;
	}

    double getNMX (void) const { return x / (double)_NM_TO_DIST_UNITS; }
    double getNMY (void) const { return y / (double)_NM_TO_DIST_UNITS; }

    void setX (double newX) { x = newX; }
    void setY (double newY) { y = newY; }
    void setZ (double newZ) { z = newZ; }

    void swap (Point& aPoint);

    Point getMaxPoint (const Point& c) const
        { return Point(MAX(x,c.x),MAX(y,c.y),MAX(z,c.z)); }
	Point getMinPoint (const Point& c) const
        { return Point(MIN(x,c.x),MIN(y,c.y),MIN(z,c.z)); }

    Point operator * (double scale) const
        { return Point (x * scale, y * scale, z); }
    Point operator + (const Point &p) const { return Point (x+p.x,y+p.y,z); }
    Point operator - (const Point &p) const { return Point (x-p.x,y-p.y,z); }
    int operator > (const Point &p) const { return ((x > p.x) && (y > p.y)); }
    bool operator < (const Point &p) const { return ((x < p.x) && (y < p.y)); }
    int operator == (const Point &p) const  { return (x == p.x) && (y == p.y); }
    int operator != (const Point &p) const { return ((x!=p.x) || (y!=p.y)); }
    void operator *= (double scale) { x *= scale; y *= scale;z*=scale ;}
    int operator ! (void) const;
    bool preciseCompare (const Point &p) const
    { 
		return ((long)(1000.0 * x) == (long)(1000.0 * p.x) &&
                (long)(1000.0 * y) == (long)(1000.0 * p.y));
	}

    Point (const Point& from, const Point& to) { initVector (from, to); }

    void initVector (const Point& from, const Point& to);

	// _vector1 x _vector2
	void Vector1XVector2( const Point& _vector1, const Point& _vector2);

    // returns distance between p and receiver
    double distance (const Point& p) const;

    // returns distance from origin to receiver (length of vector)
    double length (void) const;

    // returns vector with opposite heading
    Point inverse (void) const { return Point (-x, -y, z); }

    // returns perpendicular vector over X axis
    Point perpendicular (void) const { return Point (-y, x, z); }
	Point perpendicularY(void) const { return Point (y, -x, z); }

    // perform scale transformation on receiver
    void scale (double sx, double sy, double sz = 1.0)
        { x *= sx; y *= sy; z *= sz; }

    // perform rotation transformation on receiver
    void rotate (double degrees);

	// 2D rotation around _point
	void rotate( double degrees, Point _point );

	// 2D rotation around aroundPoint
	// but the degree is described by rotateVector
	// it means rotateVector.y/rotateVector.x = tan(degrees)
	void rotate( Point rotateVector, const Point aroundPoint );

    // returns heading, in degrees, 0 degrees + due north, inc clockwise
    double getHeading (void) const;

    // sets the length of the vector to len
    void length (double len);

    // adds distance to receiving vector
    void operator += (double distance) { length (this->length() + distance); };

    // adds point to receiving vector
    void operator += (Point pnt) { x += pnt.x; y += pnt.y; }
    void operator -= (Point pnt) { x -= pnt.x; y -= pnt.y; }

    // returns vector, equal to receiver plus distance
    Point operator + (double distance) const;

	// return dot of vector
	double operator * ( const Point& _pt ) const;

	// get cos angle of the two vectors.
	double GetCosOfTwoVector( const Point& _pt ) const;

    Point getRandPoint (double radius) const;

    // return the closest point
    Point getNearest (const Point *pointList, short count) const;
    int getNearest (const std::vector<Point>& vPoints) const;

    Point* makeArray (unsigned count) const;

    Point calcMidPoint (const Point& dest, double speed, const ElapsedTime& travelTime) const;

    int within (const Point& p1, const Point& p2, long p_buf = 0l) const;

    void printPoint (char *p_str, int p_precision = 0) const;

	// make the vector normalized.
	void Normalize();

    friend std::ostream& operator << (std::ostream& out, const Point& point);
	
	double* c_v(){ return val; }  
	/**********************************************************
	 *add some function, in these function, handle z coordinate.	
	 **********************************************************/
	double distance3D( const Point& _point ) const;
	
	void initVector3D (const Point& from, const Point& to);

	double length3D (void) const;

	void length3D (double len);
	
	void scale3D( double scale );

	void plus( const Point& _point );


	// offset the point base on ptFrom, fDir, dDist
	void offsetCoords( Point ptFrom, double dDir, double dDist );

	Point GetDistancePoint(const Point& ptTo, double dDist);
	Point GetDistancePointUnlimited(const Point& ptTo, double dDist);
};

#endif


