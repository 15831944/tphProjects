#include "elaptime.h"
#include "point.h"

#include <stdlib.h>
#include <string.h>
#include "replace.h"
#include "ARCMathCommon.h"

long Point::peek (void) const
{
    double X, Y, Z;
    X = x;
    Y = y;
    Z = z;
    Z += X + Y;
    return (long)Z;
}

void Point::swap (Point& aPoint)
{
    Point temp (aPoint);
    aPoint = *this;
    *this = temp;
}

int Point::operator ! (void) const
{
    return (x == 0 && y == 0);
}

void Point::initVector (const Point& from, const Point& to)
{
    x = to.x-from.x;
    y = to.y-from.y;
    z = 0;
}
void Point::Vector1XVector2( const Point& _vector1, const Point& _vector2)
{
	x = 0;
	y = 0;
	z = _vector1.x * _vector2.y - _vector1.y * _vector2.x;
}
std::ostream& operator << (std::ostream& out, const Point& point)
{
    char str[128];
	point.printPoint (str);
	return out << str;
}

void Point::printPoint (char *p_str, int p_precision) const
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

Point Point::getRandPoint (double radius) const
{
    double minX = x - radius;
    double minY = y - radius;
    double interval = 2 * radius;

    double xProb = (float)rand()/ RAND_MAX;
    double yProb = (float)rand()/ RAND_MAX;
    return Point ((minX + (double) (xProb * interval)), (minY +
        (double) (yProb * interval)), (double) z);
}

Point Point::getNearest (const Point *pointList, short count) const
{
	double d, closest = distance(pointList[0]);
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

int Point::getNearest (const std::vector<Point>& vPoints) const
{
	double d, closest = distance(vPoints[0]);
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

Point* Point::makeArray (unsigned count) const
{
    Point *ps = (Point *) new char [sizeof (Point) * count];
    for (unsigned i = 0; i < count; i++)
		ps[i] = *this;
	return ps;

}

// calculate the expected point, given the direction, speed and time
Point Point::calcMidPoint (const Point& p, double speed,
    const ElapsedTime& remainingTravelTime) const
{
	Point vector (*this, p);

	if (!vector.length())
		return *this;
    double distance = remainingTravelTime.asSeconds() * speed;
	vector.length (distance);
	return (p - vector);
}

int Point::within (const Point& p1, const Point& p2, long p_buf) const
{
	if( x > MAX (p1.x, p2.x) + p_buf || x < MIN (p1.x, p2.x) - p_buf )
		return FALSE;

	if( y > MAX (p1.y, p2.y) + p_buf || y < MIN (p1.y, p2.y) - p_buf )
		return FALSE;

    long X = (long) (1000.0 * x);
    long Y = (long) (1000.0 * y);

    long minX = (long) (1000.0 * MIN (p1.x, p2.x));
    long maxX = (long) (1000.0 * MAX (p1.x, p2.x));

    long minY = (long) (1000.0 * MIN (p1.y, p2.y));
    long maxY = (long) (1000.0 * MAX (p1.y, p2.y));

    // add buffers if points are perfectly horizontal or vertical
    if (p1.x == p2.x)
    {
        minX -= (long)(1000.0 * p_buf);
        maxX += (long)(1000.0 * p_buf);
    }
    if (p1.y == p2.y)
    {
        minY -= (long)(1000.0 * p_buf);
        maxY += (long)(1000.0 * p_buf);
    }

    return (X >= minX && X <= maxX && Y >= minY && Y <= maxY);
}

double Point::getHeading (void) const
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

double Point::distance (const Point& p) const
{
    Point d (*this, p);
    return d.length();
}

double Point::length (void) const
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

void Point::length (double len)
{
    double curLength = length();
    if (curLength == 0)
        x = len;
    else
        (*this) *= (len / curLength);
}

Point Point::operator + (double distance) const
{
    Point result (*this);
    double curLength = length();
    result *= (distance + curLength) / curLength;
    return result;
}


// return dot of vector
double Point::operator * ( const Point& _pt ) const
{
	return x * _pt.x + y * _pt.y;
}


// get cos angle of the two vectors.
double Point::GetCosOfTwoVector( const Point& _pt ) const
{
	double dDotValue = *this * _pt;
	if (length() == 0 || _pt.length() == 0)
	{
		return 0.0;
	}
	return dDotValue / ( length() * _pt.length() );
}


// 2D rotation around origin
void Point::rotate (double degrees)
{
    if (degrees == 0.0)
        return;

    double oldX = x;
    double oldY = y;

    // calculate radians
    double radians (((-degrees) / 180.0) * M_PI);
    double cosTheta (cos (radians));
    double sinTheta (sin (radians));

    x = (double)((oldX * cosTheta) - (oldY * sinTheta));
    y = (double)((oldX * sinTheta) + (oldY * cosTheta));

//    x = (float)((oldX * cosTheta) + (oldY * (-sinTheta)));
//    y = (float)((oldX * sinTheta) + (oldY * cosTheta));
}

// 2D rotation around _point
void Point::rotate( double degrees, Point _point )
{
    if (degrees == 0.0)
        return;

    double oldX = x - _point.getX();
    double oldY = y - _point.getY();

	oldX -= _point.getX();
	oldY -= _point.getY();

    // calculate radians
    double radians (((-degrees) / 180.0) * M_PI);
    double cosTheta (cos (radians));
    double sinTheta (sin (radians));

    x = (double)((oldX * cosTheta) - (oldY * sinTheta));
    y = (double)((oldX * sinTheta) + (oldY * cosTheta));

	x += _point.getX();
	y += _point.getY();
//    x = (float)((oldX * cosTheta) + (oldY * (-sinTheta)));
//    y = (float)((oldX * sinTheta) + (oldY * cosTheta));
}


// 2D rotation around aroundPoint
// but the degree is described by rotateVector
// it means rotateVector.y/rotateVector.x = tan(degrees)
void Point::rotate( Point rotateVector, const Point aroundPoint )
{
	rotateVector.z = 0.0; // ignore z-axis
	rotateVector.length(1.0);  // make it a unit vector

	Point ptTmp = *this;
	x = ptTmp.getX()*rotateVector.getX() - ptTmp.getY()*rotateVector.getY() + aroundPoint.getX();
	y = ptTmp.getX()*rotateVector.getY() + ptTmp.getY()*rotateVector.getX() + aroundPoint.getY();
}

void Point::getPoint (float& X, float& Y, short& Z) const
{
    X = (float)x;
    Y = (float)y;
    Z = (short)z;
}
void Point::getPoint (float& X, float& Y, float& Z) const
{
	X = (float)x;
	Y = (float)y;
	Z = (float)z;
}
void Point::setPoint (float X, float Y, int Z)
{
    x = (double)X;
    y = (double)Y;
    z = Z;
}
void Point::setPoint (double X, double Y, double Z) 
{
	x = X;
    y = Y;
    z = Z;
}

void Point::DoOffset(double _xOffset, double _yOffset, double _zOffset )
{
	x += _xOffset;
	y += _yOffset;
	z += _zOffset;
}


// make the vector normalized.
void Point::Normalize()
{
	length( 1.0 );
}

void Point::SetPoint(const Point& _pt)
{
	x = _pt.getX();
	y = _pt.getY();
	z = _pt.getZ();
}


const Point& Point::GetPoint() const
{
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// add some function, in these function, handle z coordinate.
//////////////////////////////////////////////////////////////////////////
double Point::distance3D( const Point& _point ) const
{
	double x2 = ( _point.x - x ) * ( _point.x - x );
	double y2 = ( _point.y - y ) * ( _point.y - y );
	double z2 = ( _point.z - z ) * ( _point.z - z );

	long double sumOfSquares = (long double)( x2 + y2 + z2 );
	return sqrtl(sumOfSquares);
}

void Point::initVector3D (const Point& from, const Point& to)
{
	x = to.x-from.x;
    y = to.y-from.y;
    z = to.z-from.z;
}


double Point::length3D (void) const
{
	double x2 = x*x;
	double y2 = y*y;
	double z2 = z*z;

	long double sumOfSquares = (long double)( x2 + y2 + z2 );
	return sqrtl(sumOfSquares);
}

void Point::length3D (double len)
{
	double curLength = length3D();
    if (curLength == 0)
        x = len;
    else
        scale3D( len / curLength );
}

void Point::scale3D( double scale )
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void Point::plus( const Point& _point )
{
	x += _point.x;
	y += _point.y;
	z += _point.z;
}

// offset the point base on ptFrom, fDir, dDist
void Point::offsetCoords( Point ptFrom, double dDir, double dDist )
{
    Point vector = ptFrom - *this;
    vector.length ( (double)dDist );
    vector.rotate ( dDir );
    *this += vector;
}

Point Point::GetDistancePoint(const Point& ptTo, double dDist)
{
	Point ptNew = ptTo;	

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

Point Point::GetDistancePointUnlimited(const Point& ptTo, double dDist)
{
	Point ptNew = ptTo;	
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
double Point::getDistanceTo(const  Point  & pointTo){
	double res;
	res=(x-pointTo.getX())*(x-pointTo.getX())+(y-pointTo.getY())*(y-pointTo.getY())+(z-pointTo.getZ())*(z-pointTo.getZ());
	return sqrt(res);
}

