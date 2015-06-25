#include "StdAfx.h"
#include "point2008.h"
#include "stdafx.h"
#include "elaptime.h"

#include <stdlib.h>
#include <string.h>
#include "replace.h"
#include "ARCMathCommon.h"
#include <limits>
#include "ARCVector.h"

CPoint2008::CPoint2008(void)
{
	 x = y = z = (DistanceUnit)0;
}

/////////////////////////////////////////////////////////////////////////////////////////////


long CPoint2008::peek (void) const
{
	DistanceUnit X, Y, Z;
	X = x;
	Y = y;
	Z = z;
	Z += X + Y;
	return (long)Z;
}

void CPoint2008::swap (CPoint2008& aPoint)
{
	CPoint2008 temp (aPoint);
	aPoint = *this;
	*this = temp;
}

int CPoint2008::operator ! (void) const
{
	return (x == 0 && y == 0);
}

void CPoint2008::initVector (const CPoint2008& from, const CPoint2008& to)
{
	x = to.x-from.x;
	y = to.y-from.y;
	z = 0;
}
void CPoint2008::Vector1XVector2( const CPoint2008& _vector1, const CPoint2008& _vector2)
{
	x = 0;
	y = 0;
	z = _vector1.x * _vector2.y - _vector1.y * _vector2.x;
}
std::ostream& operator << (std::ostream& out, const CPoint2008& point)
{
	char str[128];
	point.printPoint (str);
	return out << str;
}

void CPoint2008::printPoint (char *p_str, int p_precision) const
{
	if (!p_precision)
	{
		_ltoa ((long)(x / 100.0), p_str, 10);
		strcat (p_str, ",");
		_ltoa ((long)(y / 100.0), p_str + strlen(p_str), 10);
		strcat (p_str, ",");
		_ltoa ((long)z, p_str + strlen(p_str), 10);
	}
	else
	{
		_ltoa ((long)x, p_str, 10);
		strcat (p_str, ",");
		_ltoa ((long)y, p_str + strlen(p_str), 10);
		strcat (p_str, ",");
		_ltoa ((long)z, p_str + strlen(p_str), 10);
	}
}

CPoint2008 CPoint2008::getRandPoint (DistanceUnit radius) const
{
	DistanceUnit minX = x - radius;
	DistanceUnit minY = y - radius;
	double interval = 2 * radius;

	double xProb = (float)rand()/ RAND_MAX;
	double yProb = (float)rand()/ RAND_MAX;
	return CPoint2008 ((minX + (DistanceUnit) (xProb * interval)), (minY +
		(DistanceUnit) (yProb * interval)), (DistanceUnit) z);
}

CPoint2008 CPoint2008::getNearest (const CPoint2008 *pointList, short count) const
{
	DistanceUnit d, closest = distance(pointList[0]);
	int closestIdx = 0;

	for (int i = 1; i < count; i++)
	{
		if ((d = distance (pointList[i])) < closest)
		{
			closestIdx = i;
			closest = d;
		}
	}
	return pointList[closestIdx];
}

int CPoint2008::getNearest (const std::vector<CPoint2008>& vPoints) const
{
	DistanceUnit d, closest = distance(vPoints[0]);
	int closestIdx = 0;

	for (unsigned i = 1; i < vPoints.size(); i++)
	{
		if ((d = distance (vPoints[i])) < closest)
		{
			closestIdx = i;
			closest = d;
		}
	}
	return closestIdx;
}

CPoint2008* CPoint2008::makeArray (unsigned count) const
{
	//CPoint2008 *ps = (CPoint2008 *) new char [sizeof (CPoint2008) * count];
	//for (unsigned i = 0; i < count; i++)
	//	ps[i] = *this;
	//return ps;

	CPoint2008 *pointList = new CPoint2008[count];
	return pointList;
}

// calculate the expected point, given the direction, speed and time
CPoint2008 CPoint2008::calcMidPoint (const CPoint2008& p, DistanceUnit speed,
						   const ElapsedTime& remainingTravelTime) const
{
	ARCVector3 vector (*this, p);

	if (!vector.Length())
		return *this;
	double distance = remainingTravelTime.asSeconds() * speed;
	vector.SetLength(distance);
	return (p - vector);
}

int CPoint2008::within (const CPoint2008& p1, const CPoint2008& p2, long p_buf) const
{
	if( x > MAX (p1.x, p2.x) + p_buf || x < MIN (p1.x, p2.x) - p_buf )
		return FALSE;

	if( y > MAX (p1.y, p2.y) + p_buf || y < MIN (p1.y, p2.y) - p_buf )
		return FALSE;

	long X = (long) (/*1000.0 **/ x);
	long Y = (long) (/*1000.0 **/ y);

	long minX = (long) (/*1000.0 * */MIN (p1.x, p2.x));
	long maxX = (long) (/*1000.0 * */MAX (p1.x, p2.x));

	long minY = (long) (/*1000.0 * */MIN (p1.y, p2.y));
	long maxY = (long) (/*1000.0 * */MAX (p1.y, p2.y));

	// add buffers if points are perfectly horizontal or vertical
	if (p1.x == p2.x)
	{
		minX -= (long)(/*1000.0 * */p_buf);
		maxX += (long)(/*1000.0 * */p_buf);
	}
	if (p1.y == p2.y)
	{
		minY -= (long)(/*1000.0 * */p_buf);
		maxY += (long)(/*1000.0 * */p_buf);
	}

	return (X >= minX && X <= maxX && Y >= minY && Y <= maxY);
}

double CPoint2008::getHeading (void) const
{
	if (x && y)
	{
		double beta = atan (y / x);
		double theta = ARCMath::RadiansToDegrees(beta);

		return (x > 0)? (90.0 - theta): (270.0 - theta);
	}
	else
		if (x)          // y == 0.0
			return (x > 0.0)? 90.0: 270.0;
		else if (y)     // x == 0.0
			return (y > 0.0)? 0.0: 180.0;
		else            // 0.0, 0.0 vector
			return -1.0;
}

double CPoint2008::distance (const CPoint2008& p) const
{
	CPoint2008 d (*this, p);
	return d.length();
}

double CPoint2008::length (void) const
{
	if (x && y)
	{
		double result;
		double xSquared = (x * x);
		double ySquared = (y * y);
		long double sumOfSquares = (long double)(xSquared + ySquared);
		result = sqrtl (sumOfSquares);
		return result;
	}
	else if (x)
		return ABS(x);
	else if (y)
		return ABS(y);
	else
		return 0.0;
}

void CPoint2008::length (double len)
{
	double curLength = length();
	if (curLength == 0)
		x = len;
	else
		(*this) *= (len / curLength);
}

CPoint2008 CPoint2008::operator + (double distance) const
{
	CPoint2008 result (*this);
	double curLength = length();
	result *= (distance + curLength) / curLength;
	return result;
}

CPoint2008 CPoint2008::operator+( const ARCVector3& v ) const
{
	return CPoint2008(x+v.x,y+v.y,z+v.z);
}


// return dot of vector
DistanceUnit CPoint2008::operator * ( const CPoint2008& _pt ) const
{
	return x * _pt.x + y * _pt.y;
}


// get cos angle of the two vectors.
double CPoint2008::GetCosOfTwoVector( const CPoint2008& _pt ) const
{
	DistanceUnit dDotValue = *this * _pt;
	if (length() == 0 || _pt.length() == 0)
	{
		return 0.0;
	}
	return dDotValue / ( length() * _pt.length() );
}


// 2D rotation around origin
CPoint2008& CPoint2008::rotate (double degrees)
{
	if (degrees == 0.0)
		return *this;

	DistanceUnit oldX = x;
	DistanceUnit oldY = y;

	// calculate radians
	double radians (((-degrees) / 180.0) * M_PI);
	double cosTheta (cos (radians));
	double sinTheta (sin (radians));

	x = (DistanceUnit)((oldX * cosTheta) - (oldY * sinTheta));
	y = (DistanceUnit)((oldX * sinTheta) + (oldY * cosTheta));

	return *this;

	//    x = (float)((oldX * cosTheta) + (oldY * (-sinTheta)));
	//    y = (float)((oldX * sinTheta) + (oldY * cosTheta));
}

// 2D rotation around _point
void CPoint2008::rotate( double degrees, CPoint2008 _point )
{
	if (degrees == 0.0)
		return;

	DistanceUnit oldX = x - _point.getX();
	DistanceUnit oldY = y - _point.getY();

	oldX -= _point.getX();
	oldY -= _point.getY();

	// calculate radians
	double radians (((-degrees) / 180.0) * M_PI);
	double cosTheta (cos (radians));
	double sinTheta (sin (radians));

	x = (DistanceUnit)((oldX * cosTheta) - (oldY * sinTheta));
	y = (DistanceUnit)((oldX * sinTheta) + (oldY * cosTheta));

	x += _point.getX();
	y += _point.getY();
	//    x = (float)((oldX * cosTheta) + (oldY * (-sinTheta)));
	//    y = (float)((oldX * sinTheta) + (oldY * cosTheta));
}

void CPoint2008::getPoint (float& X, float& Y, short& Z) const
{
	X = (float)x;
	Y = (float)y;
	Z = (short)z;
}

void CPoint2008::getPoint (float& X, float& Y, float& Z) const
{
	X = (float)x;
	Y = (float)y;
	Z = (float)z;
}

void CPoint2008::setPoint (float X, float Y, int Z)
{
	x = (DistanceUnit)X;
	y = (DistanceUnit)Y;
	z = Z;
}
void CPoint2008::setPoint (DistanceUnit X, DistanceUnit Y, DistanceUnit Z) 
{
	x = X;
	y = Y;
	z = Z;
}

void CPoint2008::DoOffset(DistanceUnit _xOffset, DistanceUnit _yOffset, DistanceUnit _zOffset )
{
	x += _xOffset;
	y += _yOffset;
	z += _zOffset;
}


// make the vector normalized.
CPoint2008& CPoint2008::Normalize()
{
	length( 1.0 );
	return *this;
}

void CPoint2008::SetPoint(const CPoint2008& _pt)
{
	x = _pt.getX();
	y = _pt.getY();
	z = _pt.getZ();
}


//const CPoint2008& CPoint2008::GetPoint() const
//{
//	return *this;
//}

//////////////////////////////////////////////////////////////////////////
// add some function, in these function, handle z coordinate.
//////////////////////////////////////////////////////////////////////////
double CPoint2008::distance3D( const CPoint2008& _point ) const
{
	double x2 = ( _point.x - x ) * ( _point.x - x );
	double y2 = ( _point.y - y ) * ( _point.y - y );
	double z2 = ( _point.z - z ) * ( _point.z - z );

	long double sumOfSquares = (long double)( x2 + y2 + z2 );
	return sqrtl(sumOfSquares);
}

void CPoint2008::initVector3D (const CPoint2008& from, const CPoint2008& to)
{
	x = to.x-from.x;
	y = to.y-from.y;
	z = to.z-from.z;
}


double CPoint2008::length3D (void) const
{
	double x2 = x*x;
	double y2 = y*y;
	double z2 = z*z;

	long double sumOfSquares = (long double)( x2 + y2 + z2 );
	return sqrtl(sumOfSquares);
}

void CPoint2008::length3D (double len)
{
	double curLength = length3D();
	if (curLength == 0)
		x = len;
	else
		scale3D( len / curLength );
}

void CPoint2008::scale3D( double scale )
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void CPoint2008::plus( const CPoint2008& _point )
{
	x += _point.x;
	y += _point.y;
	z += _point.z;
}

// offset the point base on ptFrom, fDir, dDist
void CPoint2008::offsetCoords( CPoint2008 ptFrom, double dDir, DistanceUnit dDist )
{
	ARCVector3 vector = ptFrom - *this;
	vector.SetLength( dDist );
	vector.RotateXY( dDir );
	*this += vector;
}

CPoint2008 CPoint2008::GetDistancePoint(const CPoint2008& ptTo, double dDist)
{
	CPoint2008 ptNew = ptTo;	

	double dist = distance(ptNew);
	if(dist <= dDist)
		return ptNew;

	double xDist = 0, yDist = 0;
	xDist = ptNew.x - x;
	yDist = ptNew.y - y;

	double dsin = xDist / dist;
	double dcos = yDist / dist;

	xDist = dDist * dsin;
	yDist = dDist * dcos;

	ptNew.x = x + xDist;
	ptNew.y = y + yDist;

	return ptNew;
}

CPoint2008 CPoint2008::GetDistancePointUnlimited(const CPoint2008& ptTo, double dDist)
{
	CPoint2008 ptNew = ptTo;	
	double dist = distance(ptNew);

	double xDist = 0, yDist = 0;
	xDist = ptNew.x - x;
	yDist = ptNew.y - y;

	double dsin = xDist / dist;
	double dcos = yDist / dist;

	xDist = dDist * dsin;
	yDist = dDist * dcos;

	ptNew.x = x + xDist;
	ptNew.y = y + yDist;

	return ptNew;
}
DistanceUnit CPoint2008::getDistanceTo(const  CPoint2008  & pointTo){
	DistanceUnit res;
	res=(x-pointTo.getX())*(x-pointTo.getX())+(y-pointTo.getY())*(y-pointTo.getY())+(z-pointTo.getZ())*(z-pointTo.getZ());
	return sqrt(res);
}

DistanceUnit CPoint2008::dotProduct( const CPoint2008& pt ) const
{
	return x*pt.x+y*pt.y+z*pt.z;
}

CPoint2008 CPoint2008::crossProduct( const CPoint2008& pt ) const
{
	CPoint2008 ret;
	const DistanceUnit* lv = pt.c_v();
	const DistanceUnit* rv = c_v();
	ret.x = lv[1]*rv[2]-rv[1]*lv[2];
	ret.y = lv[2]*rv[0]-rv[2]*lv[0];
	ret.z = lv[0]*rv[1]-rv[0]*lv[1]; 
	return ret;
}

CPoint2008 CPoint2008::getNorm() const
{
	CPoint2008 ret = *this;
	ret.Normalize();
	return ret;
}

CPoint2008 CPoint2008::getLengtDir( DistanceUnit dlen ) const
{
	CPoint2008 ret = *this;
	ret.length(dlen);
	return ret;
}

CPoint2008 CPoint2008::getRotate( double degrees ) const
{
	CPoint2008 ret = *this;
	return ret.rotate(degrees);
}

CPoint2008& CPoint2008::operator+=( const ARCVector3& v )
{
	x+=v.x; y+= v.y; z+= v.z; return *this;
}

CPoint2008& CPoint2008::operator-=( const ARCVector3& v )
{
	x-=v.x; y-= v.y; z-= v.z; return *this;
}

ARCVector3 CPoint2008::operator-( const CPoint2008& pt ) const
{
	return ARCVector3(x-pt.x, y-pt.y, z-pt.z);
}

CPoint2008 CPoint2008::operator-( const ARCVector3& v ) const
{
	return CPoint2008(x-v.x,y-v.y,z-v.z);
}

ARCVector3 CPoint2008::toVector() const
{
	return ARCVector3(x,y,z);
}
