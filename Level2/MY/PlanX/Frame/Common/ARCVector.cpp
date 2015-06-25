#include <cmath>
#include "commondll.h"
#include "replace.h"

#include "ARCVector.h"
#include "Point.h"
#include "Point2008.h"
#include "ARCMathCommon.h"

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
void ARCVector2 ::reset(double x,double y){
	n[VX] = x;
	n[VY] =y;
}

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
	double dMag = Magnitude();
	if(dMag) {
		n[VX]/=dMag;
		n[VY]/=dMag;
	}
	return *this;
}

bool ARCVector2::Compare(const ARCVector2& _rhs, const double _epsilon) const
{
	return (abs(n[VX]-_rhs.n[VX]) < ARCMath::EPSILON) && (abs(n[VY]-_rhs.n[VY]) < ARCMath::EPSILON);
}

double ARCVector2::DistanceTo(const ARCVector2& _rhs) const
{
	return (*this-_rhs).Magnitude();
}

void ARCVector2::Rotate(const double _degrees)
{
	if(abs(_degrees) < ARCMath::EPSILON)
        return;

    double oldX = n[VX];
    double oldY = n[VY];

    // calculate radians
    double radians = ARCMath::DegreesToRadians(-_degrees);
    double cosTheta (cos (radians));
    double sinTheta (sin (radians));

    n[VX] = (oldX * cosTheta) - (oldY * sinTheta);
    n[VY] = (oldX * sinTheta) + (oldY * cosTheta);
}

void ARCVector2::Rotate(const double _degrees, const ARCVector2& _point)
{
	if(abs(_degrees) < ARCMath::EPSILON)
        return;

    double oldX = n[VX];// - _point[VX];
    double oldY = n[VY];// - _point[VY];

	oldX -= _point[VX];
	oldY -= _point[VY];

    // calculate radians
    double radians = ARCMath::DegreesToRadians(-_degrees);
    double cosTheta (cos (radians));
    double sinTheta (sin (radians));

    n[VX] = (oldX * cosTheta) - (oldY * sinTheta);
    n[VY] = (oldX * sinTheta) + (oldY * cosTheta);

	n[VX] += _point[VX];
	n[VY] += _point[VY];
}
void ARCVector2::DoOffset(double _xOffset, double _yOffset)
{
	n[VX] += _xOffset;
	n[VY] += _yOffset;
}
double ARCVector2::GetHeading() const
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

void ARCVector2::SetLength(const double _len)
{
	double curLength = Magnitude();
    if(abs(curLength)<ARCMath::EPSILON)
        n[VX] = _len;
    else
        (*this) *= (_len / curLength);
}

double ARCVector2::GetCosOfTwoVector( const ARCVector2& _rhs ) const
{
	return (*this * _rhs) / ( Magnitude() * _rhs.Magnitude() );
}

ARCVector2 ARCVector2::GetRandPoint(const double _radius) const
{
	double minX = n[VX] - _radius;
    double minY = n[VY] - _radius;
    double interval = 2 * _radius;
    double xProb = static_cast<double>(rand()) / RAND_MAX;
    double yProb = static_cast<double>(rand()) / RAND_MAX;
    return ARCVector2(minX + (xProb * interval), minY + (yProb * interval));
}

ARCVector2 ARCVector2::GetNearest(const std::vector<ARCVector2>& _vList) const
{
	if(_vList.size() == 0)
		return ARCVector2(0.0,0.0);

	double d, closest = DistanceTo(*(_vList.begin()));
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

ARCVector3& ARCVector3::Normalize()
{
	double dMag = Magnitude();
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

int ARCVector3::Compare(const ARCVector3& _v, double _epsilon) const
{
	return 0;
}

#ifdef _DEBUG //non-debug is inline
double ARCVector3::Magnitude() const
{
	return sqrt(n[VX]*n[VX]+n[VY]*n[VY]+n[VZ]*n[VZ]);
}
#endif

double ARCVector3::DistanceTo(const ARCVector3& _rhs) const
{
	return (*this-_rhs).Magnitude();
}

double ARCVector3::GetCosOfTwoVector( const ARCVector3& _rhs ) const
{
	return ((*this).dot(_rhs) ) / (Magnitude() * _rhs.Magnitude());
}

ARCVector3 ARCVector3::GetRandPoint(const double _radius) const
{
	double minX = n[VX] - _radius;
    double minY = n[VY] - _radius;
	double minZ = n[VZ] - _radius;
    double interval = 2 * _radius;

    double xProb = static_cast<double>(rand()) / RAND_MAX;
    double yProb = static_cast<double>(rand()) / RAND_MAX;
	double zProb = static_cast<double>(rand()) / RAND_MAX;
	
    return ARCVector3(minX + (xProb * interval), minY + (yProb * interval), minZ + (zProb * interval) );
}

ARCVector3 ARCVector3::GetNearest(const std::vector<ARCVector3>& _vList) const
{
	if(_vList.size() == 0)
		return ARCVector3(0.0,0.0,0.0);

	double d, closest = DistanceTo(*(_vList.begin()));
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

ARCVector3::ARCVector3( const ARCVector2& v2 , const double& _z )
{
	n[VX] = v2[VX]; n[VY] = v2[VY]; n[VZ] = _z;
}

ARCVector3::ARCVector3()
{
x = y = z = (double)0;
}

void ARCVector3::DoOffset(double _xOffset, double _yOffset, double _zOffset )
{
	n[VX] += _xOffset;
	n[VY] += _yOffset;
	n[VZ] += _zOffset;
}

ARCVector3& ARCVector3::RotateXY( double _degrees )
{
	if(abs(_degrees) < ARCMath::EPSILON)
		return *this;

	double oldX = n[VX];
	double oldY = n[VY];

	// calculate radians
	double radians = ARCMath::DegreesToRadians(-_degrees);
	double cosTheta (cos (radians));
	double sinTheta (sin (radians));

	n[VX] = (oldX * cosTheta) - (oldY * sinTheta);
	n[VY] = (oldX * sinTheta) + (oldY * cosTheta);

	return *this;
}

ARCVector3& ARCVector3::RotateXY( double _degrees, const ARCPoint3& _point )
{
	if(abs(_degrees) < ARCMath::EPSILON)
		return *this;

	double oldX = n[VX];// - _point[VX];
	double oldY = n[VY];// - _point[VY];

	oldX -= _point[VX];
	oldY -= _point[VY];

	// calculate radians
	double radians = ARCMath::DegreesToRadians(-_degrees);
	double cosTheta (cos (radians));
	double sinTheta (sin (radians));

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
	const double* lv = n;
	const double* rv = v.n;
	double x = lv[1]*rv[2]-rv[1]*lv[2];
	double y = lv[2]*rv[0]-rv[2]*lv[0];
	double z = lv[0]*rv[1]-rv[0]*lv[1]; 
	return ARCVector3(x,y,z);
}

ARCVector3 ARCVector3::UNIT_Z(0,0,1);

ARCVector3 ARCVector3::UNIT_Y(0,1,0);

ARCVector3 ARCVector3::UNIT_X(1,0,0);

ARCVector3 ARCVector3::ZERO = ARCVector3(0,0,0);
