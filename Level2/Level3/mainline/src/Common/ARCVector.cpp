// ARCVector.cpp: implementation of the ARCVector2 & ARCVector3 classes.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <cmath>
#include "commondll.h"
#include "replace.h"

#include "ARCVector.h"
#include "Point.h"
#include "Point2008.h"
#include "ARCMathCommon.h"


//////////////////////////////////////////////////////////////////////
// ARCVector2
//////////////////////////////////////////////////////////////////////
ARCVector2::ARCVector2(const Point& _rhs)
{
	n[VX] = _rhs.getX();
	n[VY] = _rhs.getY();
}

ARCVector2::ARCVector2(const CPoint2008& _rhs)
{
	n[VX] = _rhs.getX();
	n[VY] = _rhs.getY();
}

ARCVector2::ARCVector2( const ARCVector3& v3 )
{
	n[VX] = v3[VX];
	n[VY] = v3[VY];
}
void ARCVector2 ::reset(DistanceUnit x,DistanceUnit y){
	n[VX] = x;
	n[VY] =y;
}
/*
ARCVector2::ARCVector2(const Point& _rhs)
{
	n[VX]=static_cast<DistanceUnit>(_rhs.x);
	n[VY]=static_cast<DistanceUnit>(_rhs.y);
}
*/
//return angle of two vector  form this to _rhs in anti clock;

double ARCVector2::GetAngleOfTwoVector(const ARCVector2 &_rhs)const
{

	double sig=(this->n[VX]*_rhs.n[VY]) - (this->n[VY] * _rhs.n[VX]);
	
	if(sig>0)sig=1.0;else sig=-1.0;
	return ARCMath::RadiansToDegrees(sig*acos(GetCosOfTwoVector(_rhs)));
	

}

void ARCVector2::reset(Point _from ,Point _to){
	n[VX]=_to.getX()-_from.getX();
	n[VY]=_to.getY()-_from.getY();
}

void ARCVector2::reset(CPoint2008 _from ,CPoint2008 _to)
{
	n[VX]=_to.getX()-_from.getX();
	n[VY]=_to.getY()-_from.getY();
}

double ARCVector2::AngleFromCoorndinateX()const
{
	double res = 0.0;
	double dMag = Magnitude();
	if (abs(dMag)>=ARCMath::EPSILON)
 	{
		res=ARCMath::RadiansToDegrees(acos( n[VX]/dMag));
		if(n[VY]<0.0)res=360-res;
 	}
	return res;

}
ARCVector2& ARCVector2::Normalize()
{
	DistanceUnit dMag = Magnitude();
	if(dMag) {
		n[VX]/=dMag;
		n[VY]/=dMag;
	}
	return *this;
}

bool ARCVector2::Compare(const ARCVector2& _rhs, const DistanceUnit _epsilon) const
{
	return (abs(n[VX]-_rhs.n[VX]) < ARCMath::EPSILON) && (abs(n[VY]-_rhs.n[VY]) < ARCMath::EPSILON);
}

DistanceUnit ARCVector2::DistanceTo(const ARCVector2& _rhs) const
{
	return (*this-_rhs).Magnitude();
}

void ARCVector2::Rotate(const DistanceUnit _degrees)
{
	if(abs(_degrees) < ARCMath::EPSILON)
        return;

    DistanceUnit oldX = n[VX];
    DistanceUnit oldY = n[VY];

    // calculate radians
    DistanceUnit radians = ARCMath::DegreesToRadians(-_degrees);
    DistanceUnit cosTheta (cos (radians));
    DistanceUnit sinTheta (sin (radians));

    n[VX] = (oldX * cosTheta) - (oldY * sinTheta);
    n[VY] = (oldX * sinTheta) + (oldY * cosTheta);
}

void ARCVector2::Rotate(const DistanceUnit _degrees, const ARCVector2& _point)
{
	if(abs(_degrees) < ARCMath::EPSILON)
        return;

    DistanceUnit oldX = n[VX];// - _point[VX];
    DistanceUnit oldY = n[VY];// - _point[VY];

	oldX -= _point[VX];
	oldY -= _point[VY];

    // calculate radians
    DistanceUnit radians = ARCMath::DegreesToRadians(-_degrees);
    DistanceUnit cosTheta (cos (radians));
    DistanceUnit sinTheta (sin (radians));

    n[VX] = (oldX * cosTheta) - (oldY * sinTheta);
    n[VY] = (oldX * sinTheta) + (oldY * cosTheta);

	n[VX] += _point[VX];
	n[VY] += _point[VY];
}
void ARCVector2::DoOffset(DistanceUnit _xOffset, DistanceUnit _yOffset)
{
	n[VX] += _xOffset;
	n[VY] += _yOffset;
}
DistanceUnit ARCVector2::GetHeading() const
{
	if (n[VX] && n[VY])
    {
        double beta = atan (n[VY] / n[VX]);
        double theta = ARCMath::RadiansToDegrees(beta);

        return (n[VX] > 0)? (90.0 - theta): (270.0 - theta);
    }
    else
        if (n[VX])          // y == 0.0
            return (n[VX] > 0.0)? 90.0: 270.0;
        else if (n[VY])     // x == 0.0
            return (n[VY] > 0.0)? 0.0: 180.0;
        else            // 0.0, 0.0 vector
            return -1.0;

}

void ARCVector2::SetLength(const DistanceUnit _len)
{
	DistanceUnit curLength = Magnitude();
    if(abs(curLength)<ARCMath::EPSILON)
        n[VX] = _len;
    else
        (*this) *= (_len / curLength);
}

DistanceUnit ARCVector2::GetCosOfTwoVector( const ARCVector2& _rhs ) const
{
	return (*this * _rhs) / ( Magnitude() * _rhs.Magnitude() );
}

ARCVector2 ARCVector2::GetRandPoint(const DistanceUnit _radius) const
{
	DistanceUnit minX = n[VX] - _radius;
    DistanceUnit minY = n[VY] - _radius;
    DistanceUnit interval = 2 * _radius;
    DistanceUnit xProb = static_cast<DistanceUnit>(rand()) / RAND_MAX;
    DistanceUnit yProb = static_cast<DistanceUnit>(rand()) / RAND_MAX;
    return ARCVector2(minX + (xProb * interval), minY + (yProb * interval));
}

ARCVector2 ARCVector2::GetNearest(const std::vector<ARCVector2>& _vList) const
{
	if(_vList.size() == 0)
		return ARCVector2(0.0,0.0);

	DistanceUnit d, closest = DistanceTo(*(_vList.begin()));
	std::vector<ARCVector2>::const_iterator itClosest = _vList.begin();
	for(std::vector<ARCVector2>::const_iterator it=_vList.begin()+1; it!=_vList.end(); it++) {
		if( (d = DistanceTo(*it)) < closest ) {
			closest = d;
			itClosest = it;
		}
	}
	return ARCVector2(*itClosest);
}

ARCVector2 ARCVector2::GetNorm() const
{
	ARCVector2 vec = *this;
	vec.Normalize();
	return vec;
}
/*
ARCVector2* makeArray(unsigned _count) const;
*/
/*
ARCVector2 ARCVector2::CalcMidPoint(const ARCVector2& _dest, const DistanceUnit _speed, const ElapsedTime& _travelTime) const
{
	ARCVector2 v(_dest-*this);

	if(abs(v.Magnitude())<ARCMath::EPSILON)
		return *this;
    double distance = _travelTime.asSeconds() * _speed;
	v.SetLength(distance);
	return (_dest - v);
}
*/
/*
bool ARCVector2::Within(const ARCVector2& _v1, const ARCVector2& _v2, long p_buf = 0l) const
{
	if( n[VX] > MAX (p1.x, p2.x) + p_buf || n[VY] < MIN (p1.x, p2.x) - p_buf )
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
*/
/*
void ARCVector2::PrintPoint(char *p_str, int p_precision) const
{
	if (!p_precision)
    {
        ltoa ((long)(n[VX] / 100.0), p_str, 10);
        strcat (p_str, ",");
        ltoa ((long)(n[VY] / 100.0), p_str + strlen(p_str), 10);
        strcat (p_str, ",");
        ltoa (0, p_str + strlen(p_str), 10);
    }
    else
    {
        ltoa (n[VX], p_str, 10);
        strcat (p_str, ",");
        ltoa (n[VY], p_str + strlen(p_str), 10);
        strcat (p_str, ",");
        ltoa (0, p_str + strlen(p_str), 10);
    }
}
*/
//friends of ARCVector2
ARCVector2 operator + ( const ARCVector2& _v, const ARCVector2& _u )		// v + u
{
	return ARCVector2(_v[VX]+_u[VX],_v[VY]+_u[VY]);
}

ARCVector2 operator - ( const ARCVector2& _v, const ARCVector2& _u )		// v - u
{
	return ARCVector2(_v[VX]-_u[VX],_v[VY]-_u[VY]);
}

ARCVector3 operator ^ ( const ARCVector2& _v, const ARCVector2& _u )		// v x u (cross product)
{
	return ARCVector3(0.0, 0.0, (_v.n[VX]*_u.n[VY]) - (_v.n[VY]*_u.n[VX]));
}



//////////////////////////////////////////////////////////////////////
// ARCVector3
//////////////////////////////////////////////////////////////////////

ARCVector3& ARCVector3::Normalize()
{
	DistanceUnit dMag = Magnitude();
	if(dMag) {
		n[VX] /= dMag;
		n[VY] /= dMag;
		n[VZ] /= dMag;
	}
	return *this;
}

ARCVector3 operator ^ (const ARCVector3& _v, const ARCVector3& _u)
{
	return ARCVector3((_v.n[VY] * _u.n[VZ]) - (_v.n[VZ] * _u.n[VY]),
							  (_v.n[VZ] * _u.n[VX]) - (_v.n[VX] * _u.n[VZ]),
							  (_v.n[VX] * _u.n[VY]) - (_v.n[VY] * _u.n[VX]));
}

int ARCVector3::Compare(const ARCVector3& _v, DistanceUnit _epsilon) const
{
	return 0;
}

#ifdef _DEBUG //non-debug is inline
DistanceUnit ARCVector3::Magnitude() const
{
	return sqrt(n[VX]*n[VX]+n[VY]*n[VY]+n[VZ]*n[VZ]);
}
#endif

DistanceUnit ARCVector3::DistanceTo(const ARCVector3& _rhs) const
{
	return (*this-_rhs).Magnitude();
}

DistanceUnit ARCVector3::GetCosOfTwoVector( const ARCVector3& _rhs ) const
{
	return ((*this).dot(_rhs) ) / (Magnitude() * _rhs.Magnitude());
}

ARCVector3 ARCVector3::GetRandPoint(const DistanceUnit _radius) const
{
	DistanceUnit minX = n[VX] - _radius;
    DistanceUnit minY = n[VY] - _radius;
	DistanceUnit minZ = n[VZ] - _radius;
    DistanceUnit interval = 2 * _radius;

    DistanceUnit xProb = static_cast<DistanceUnit>(rand()) / RAND_MAX;
    DistanceUnit yProb = static_cast<DistanceUnit>(rand()) / RAND_MAX;
	DistanceUnit zProb = static_cast<DistanceUnit>(rand()) / RAND_MAX;
	
    return ARCVector3(minX + (xProb * interval), minY + (yProb * interval), minZ + (zProb * interval) );
}

ARCVector3 ARCVector3::GetNearest(const std::vector<ARCVector3>& _vList) const
{
	if(_vList.size() == 0)
		return ARCVector3(0.0,0.0,0.0);

	DistanceUnit d, closest = DistanceTo(*(_vList.begin()));
	std::vector<ARCVector3>::const_iterator itClosest = _vList.begin();
	for(std::vector<ARCVector3>::const_iterator it=_vList.begin()+1; it!=_vList.end(); it++) {
		if( (d = DistanceTo(*it)) < closest ) {
			closest = d;
			itClosest = it;
		}
	}
	return ARCVector3(*itClosest);
}
ARCVector3& ARCVector3::SetLength(double _len)				// sets the length of the vector to len
{
	if(double dLen = Length() > ARCMath::EPSILON)
	{
		double scale = _len / Length();
		(*this)*=scale;
	}
	return *this;
}


ARCVector3::ARCVector3(const Point & pfrom,const Point & pto)
{
	n[VX]=pto.getX()-pfrom.getX();n[VY]=pto.getY()-pfrom.getY();n[VZ]=pto.getZ()-pfrom.getZ();
}

ARCVector3::ARCVector3(const CPoint2008 & pfrom,const CPoint2008 & pto)
{
	n[VX]=pto.getX()-pfrom.getX();n[VY]=pto.getY()-pfrom.getY();n[VZ]=pto.getZ()-pfrom.getZ();
}

ARCVector3::ARCVector3(const Point&_pt)
{
	n[VX]=_pt.getX();n[VY]=_pt.getY();n[VZ]=_pt.getZ();
}

ARCVector3::ARCVector3(const CPoint2008&_pt)
{
	n[VX]=_pt.getX();n[VY]=_pt.getY();n[VZ]=_pt.getZ();
}

ARCVector3::ARCVector3( const ARCVector2& v2 , const DistanceUnit& _z )
{
	n[VX] = v2[VX]; n[VY] = v2[VY]; n[VZ] = _z;
}

ARCVector3::ARCVector3()
{
x = y = z = (DistanceUnit)0;
}

void ARCVector3::DoOffset(DistanceUnit _xOffset, DistanceUnit _yOffset, DistanceUnit _zOffset )
{
	n[VX] += _xOffset;
	n[VY] += _yOffset;
	n[VZ] += _zOffset;
}

ARCVector3& ARCVector3::RotateXY( double _degrees )
{
	if(abs(_degrees) < ARCMath::EPSILON)
		return *this;

	DistanceUnit oldX = n[VX];
	DistanceUnit oldY = n[VY];

	// calculate radians
	DistanceUnit radians = ARCMath::DegreesToRadians(-_degrees);
	DistanceUnit cosTheta (cos (radians));
	DistanceUnit sinTheta (sin (radians));

	n[VX] = (oldX * cosTheta) - (oldY * sinTheta);
	n[VY] = (oldX * sinTheta) + (oldY * cosTheta);

	return *this;
}

ARCVector3& ARCVector3::RotateXY( double _degrees, const ARCPoint3& _point )
{
	if(abs(_degrees) < ARCMath::EPSILON)
		return *this;

	DistanceUnit oldX = n[VX];// - _point[VX];
	DistanceUnit oldY = n[VY];// - _point[VY];

	oldX -= _point[VX];
	oldY -= _point[VY];

	// calculate radians
	DistanceUnit radians = ARCMath::DegreesToRadians(-_degrees);
	DistanceUnit cosTheta (cos (radians));
	DistanceUnit sinTheta (sin (radians));

	n[VX] = (oldX * cosTheta) - (oldY * sinTheta);
	n[VY] = (oldX * sinTheta) + (oldY * cosTheta);

	n[VX] += _point[VX];
	n[VY] += _point[VY];
	return *this;
}

ARCVector3 ARCVector3::GetRotateXY(double _degree) const
{
	ARCVector3 ret = *this;
	return ret.RotateXY(_degree);
}

ARCVector3 ARCVector3::GetLengtDir( double dlen ) const
{
	ARCVector3 ret = *this;
	return ret.SetLength(dlen);
}

CPoint2008 ARCVector3::operator+( const CPoint2008& pt ) const
{
	return pt+*this;
}

ARCVector3 ARCVector3::cross( const ARCVector3& v ) const
{
	const DistanceUnit* lv = n;
	const DistanceUnit* rv = v.n;
	double x = lv[1]*rv[2]-rv[1]*lv[2];
	double y = lv[2]*rv[0]-rv[2]*lv[0];
	double z = lv[0]*rv[1]-rv[0]*lv[1]; 
	return ARCVector3(x,y,z);
}

ARCVector3 ARCVector3::UNIT_Z(0,0,1);

ARCVector3 ARCVector3::UNIT_Y(0,1,0);

ARCVector3 ARCVector3::UNIT_X(1,0,0);

ARCVector3 ARCVector3::ZERO = ARCVector3(0,0,0);
