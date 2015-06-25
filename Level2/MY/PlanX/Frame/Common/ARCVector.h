#ifndef ARC_VECTOR_H
#define ARC_VECTOR_H

#include "ARCMathCommon.h"
#include <vector>
#include <math.h>
#include "VectorBase.h"

class ARCVector3;
class ARCMatrxi4x4;
class Point;
class CPoint2008;


//DO NOT SUBCLASS THIS CLASS (NO VIRTUAL DESTRUCTOR)
class ARCVector2  : public Vector2Base
{
	friend class ARCVector3;
public:	
	// Construction
	inline ARCVector2() {};
	inline ARCVector2(const double& _x, const double& _y)
		{ n[VX]=_x; n[VY]=_y; }
	inline explicit ARCVector2(const double& _d)
		{ n[VX]=_d; n[VY]=_d; }
	inline ARCVector2(const ARCVector2& _rhs)
		{ n[VX]=_rhs.n[VX]; n[VY]=_rhs.n[VY]; }

	inline ARCVector2(const ARCVector2& _from, const ARCVector2& _to)
		{ n[VX]=_to.n[VX]-_from.n[VX]; n[VY]=_to.n[VY]-_from.n[VY]; }
	explicit ARCVector2(const Point& _rhs);
	explicit ARCVector2(const CPoint2008& _rhs);

	explicit ARCVector2 (const ARCVector3& v3);

	//have all arguments like constructor
	void reset(double x,double y);
	void reset( const ARCVector2 & _from,const ARCVector2 & _to){ n[VX]=_to.n[VX]-_from.n[VX]; n[VY]=_to.n[VY]-_from.n[VY]; }
	void reset(Point _from ,Point _to);
	void reset(CPoint2008 _from ,CPoint2008 _to);
	// Destruction
	//~ARCVector2() {} //destructor not virtual (for efficiency) DO NOT SUBCLASS THIS CLASS
	double AngleFromCoorndinateX()const;
	// functions
	inline double Magnitude() const
		{ return sqrt(n[VX]*n[VX]+n[VY]*n[VY]); }
	inline double Magnitude2() const
		{ return n[VX]*n[VX]+n[VY]*n[VY]; }
	ARCVector2& Normalize();

	ARCVector2 GetNorm()const;
	
	// Operators
	ARCVector2& operator =	( const ARCVector2& _rhs )		// assignment operator
		{ n[VX]=_rhs.n[VX]; n[VY]=_rhs.n[VY]; return *this; }

	ARCVector2& operator =	( const ARCVector3& _rhs );		// assignment operator (inline at end of this file

	ARCVector2& operator +=	( const ARCVector2& _v )
		{ n[VX]+=_v.n[VX]; n[VY]+=_v.n[VY]; return *this; }
	ARCVector2& operator +=	( const double& _d )		// increases this vector's length by _d units
		{ SetLength(Length() + _d); return *this; }
	ARCVector2& operator -=	( const ARCVector2& _v )
		{ n[VX]-=_v.n[VX]; n[VY]-=_v.n[VY]; return *this; }
	ARCVector2& operator *=	( const double _d )
		{ n[VX]*=_d; n[VY]*=_d; return *this; }
	ARCVector2& operator /=	( const double _d )
		{ n[VX]/=_d; n[VY]/=_d; return *this; }

	double& operator [] ( int i )					//read-write indexing
		{ return n[i]; }
	const double& operator [] ( int i ) const		//read-only indexing
		{ return n[i]; }
	operator double*()							//pointer cast
		{ return n; }
	operator const double*() const				//const pointer cast
		{ return n; }

	int operator > (const ARCVector2& _rhs) const	//greater-than compare (x and y components must be greater)
		{ return n[VX]>_rhs.n[VX] && n[VY]>_rhs.n[VY]; }
    int operator < (const ARCVector2& _rhs) const	//lesser-than compare
		{ return n[VX]<_rhs.n[VX] && n[VY]<_rhs.n[VY]; }

	friend ARCVector2 operator - ( const ARCVector2& _v )								// -v
		{ return ARCVector2(-_v.n[VX],-_v.n[VY]); }
	friend ARCVector2 operator + ( const ARCVector2& _v, const ARCVector2& _u );		// v + u
	friend ARCVector2 operator - ( const ARCVector2& _v, const ARCVector2& _u );		// v - u
	friend ARCVector2 operator / ( const ARCVector2& _v, const double _d )		// v / scalar
		{ return ARCVector2(_v.n[VX]/_d,_v.n[VY]/_d); }
	friend ARCVector2 operator * ( const ARCVector2& _v, const double _d )		// v * scalar
		{ return ARCVector2(_v.n[VX]*_d,_v.n[VY]*_d); }
	friend ARCVector2 operator * ( const double _d, const ARCVector2& _v )		// scalar * v
		{ return ARCVector2(_v.n[VX]*_d,_v.n[VY]*_d); }
	//friend ARCVector2 operator * ( const ARCMatrix4x4& _m, const ARCVector2& _v );	// m . v
	friend double operator * ( const ARCVector2& _v, const ARCVector2& _u )		// v . u (dot product)
		{ return _v.n[VX]*_u.n[VX]+_v.n[VY]*_u.n[VY]; }
	friend ARCVector3 operator ^ ( const ARCVector2& _v, const ARCVector2& _u );		// v x u (cross product)
	friend int operator == ( const ARCVector2& _v, const ARCVector2& _u )				// v == u ?
		{ return _v.n[VX]==_u.n[VX] && _v.n[VY]==_u.n[VY]; }
	friend int operator != ( const ARCVector2& _v, const ARCVector2& _u )				// v != u ?
		{ return _v.n[VX]!=_u.n[VX] || _v.n[VY]!=_u.n[VY]; }
	//friend std::ostream& operator << ( std::ostream& _s, ARCVector2& _v );				// output to stream
	//friend std::istream& operator >> ( std::istream& _s, ARCVector2& _v );				// input from stream
	bool Compare(const ARCVector2& _rhs, const double _epsilon=ARCMath::EPSILON) const;						// this == v ? with epsilon

	double DistanceTo(const ARCVector2& _rhs) const;

    void Rotate(const double _degrees);						// rotate vector (2D)
	void Rotate(const double _degrees, const ARCVector2& _point);	// rotate vector around _point (2D)

    double GetHeading() const;				// returns heading, in degrees, 0 degrees + due north, inc clockwise

    void SetLength(const double _len);		// sets the length of the vector to len
    double Length() const						// returns distance from origin to receiver (length of vector)
		{ return Magnitude(); }

	double GetCosOfTwoVector( const ARCVector2& _rhs ) const;				// get cos angle of the two vectors.

	double GetAngleOfTwoVector(const ARCVector2 &_rhs)const;

    ARCVector2 GetRandPoint(const double _radius) const;
    ARCVector2 GetNearest(const std::vector<ARCVector2>& _vList) const;	// return the closest point
	ARCVector2 PerpendicularLCopy() const { return ARCVector2(-n[VY], n[VX]); }
	ARCVector2 PerpendicularRCopy() const { return ARCVector2(n[VY], -n[VX]); }

	void DoOffset(double _xOffset, double _yOffset );

	double dot(const ARCVector2& vdir)const{ return x*vdir.x  + y*vdir.y; }
	double cross(const ARCVector2& vdir)const{ return (n[VX]*vdir.n[VY]) - (n[VY]*vdir.n[VX]); }
};

typedef ARCVector2 ARCPoint2;

//DO NOT SUBCLASS THIS CLASS (NO VIRTUAL DESTRUCTOR)




class ARCVector3  : public Vector3Base
{
	friend class ARCVector2;
public:
	
public:	
	// Construction
	ARCVector3();
	ARCVector3(const double _x, const double _y, const double _z){ n[VX]=_x; n[VY]=_y; n[VZ]=_z; }
	explicit ARCVector3(const double _d){ n[VX]=n[VY]=n[VZ]=_d; }
	ARCVector3(const ARCVector3& _rhs){ n[VX]=_rhs.n[VX]; n[VY]=_rhs.n[VY]; n[VZ]=_rhs.n[VZ]; }
	ARCVector3(const ARCVector3& _from, const ARCVector3& _to){ n[VX]=_to.n[VX]-_from.n[VX]; n[VY]=_to.n[VY]-_from.n[VY]; n[VZ]=_to.n[VZ]-_from.n[VZ]; }
	ARCVector3(const Point & pfrom,const Point & pto);
	ARCVector3(const CPoint2008 & pfrom,const CPoint2008 & pto);
	ARCVector3(const Point&_pt);
	ARCVector3(const CPoint2008&_pt);

	inline explicit ARCVector3(const Vector3Base& v):Vector3Base(v){}
	ARCVector3(const ARCVector2& v2 , const double& _z);
	

	double Magnitude() const; //inlined in release version (see end of this file)
	ARCVector3& Normalize();
	
	ARCVector3& operator	+=	( const ARCVector3& _v )
		{ n[VX]+=_v.n[VX]; n[VY]+=_v.n[VY]; n[VZ]+=_v.n[VZ]; return *this; }
	ARCVector3& operator	-=	( const ARCVector3& _v )
		{ n[VX]-=_v.n[VX]; n[VY]-=_v.n[VY]; n[VZ]-=_v.n[VZ]; return *this; }
	ARCVector3& operator	*=	( const double _d )
		{ n[VX]*=_d; n[VY]*=_d; n[VZ]*=_d; return *this; }
	ARCVector3& operator	/=	( const double _d )
		{ n[VX]/=_d; n[VY]/=_d; n[VZ]/=_d; return *this; }

		ARCVector3& operator	*=	( const ARCVector3& _v )
		{ n[VX]*=_v[VX]; n[VY]*=_v[VY]; n[VZ]*=_v[VZ]; return *this; }
		ARCVector3& operator	/=	( const ARCVector3& _v )
		{ n[VX]/=_v[VX]; n[VY]/=_v[VX]; n[VZ]/=_v[VZ]; return *this; }

		ARCVector3 operator	*	( const ARCVector3& _v )const
		{ return ARCVector3(*this)*=_v; }
		ARCVector3 operator	/	( const ARCVector3& _v )const
		{ return ARCVector3(*this)/=_v; }
		double dot(const ARCVector3& _v)const{
			return n[VX]*_v[VX]+n[VY]*_v[VY]+n[VZ]*_v[VZ];
		}
		ARCVector3 cross(const ARCVector3& v)const;

	double& operator [] ( int i ) { return n[i]; }
	const double& operator [] ( int i ) const { return n[i]; }
	operator double*() { return n; }				//pointer cast
	operator const double*() const { return n; }	//const pointer cast

	friend ARCVector3 operator - ( const ARCVector3& _v )								// -v
		{ return ARCVector3(-_v.n[VX],-_v.n[VY],-_v.n[VZ]); }
	friend ARCVector3 operator + ( const ARCVector3& _v, const ARCVector3& _u )			// v + u
		{ return ARCVector3(_v.n[VX] + _u.n[VX], _v.n[VY] + _u.n[VY], _v.n[VZ] + _u.n[VZ]); }
	friend ARCVector3 operator - ( const ARCVector3& _v, const ARCVector3& _u )			// v - u
		{ return ARCVector3(_v.n[VX] - _u.n[VX], _v.n[VY] - _u.n[VY], _v.n[VZ] - _u.n[VZ]); }
	friend ARCVector3 operator / ( const ARCVector3& _v, const double _d )		// v / scalar
		{ return ARCVector3(_v.n[VX]/_d,_v.n[VY]/_d,_v.n[VZ]/_d); }
	friend ARCVector3 operator * ( const ARCVector3& _v, const double _d )		// v * scalar
		{ return ARCVector3(_v.n[VX]*_d,_v.n[VY]*_d,_v.n[VZ]*_d); }
	friend ARCVector3 operator * ( double _d, const ARCVector3& _v )				// scalar * v
		{ return ARCVector3(_v.n[VX]*_d,_v.n[VY]*_d,_v.n[VZ]*_d); }
	friend ARCVector3 operator ^ ( const ARCVector3& _v, const ARCVector3& _u );		// v x u (cross product)
	friend int operator == ( const ARCVector3& _v, const ARCVector3& _u )				// v == u ?
		{ return _v.n[VX]==_u.n[VX] && _v.n[VY]==_u.n[VY] && _v.n[VZ]==_u.n[VZ]; }
	friend int operator != ( const ARCVector3& _v, const ARCVector3& _u )				// v != u ?
		{ return _v.n[VX]!=_u.n[VX] || _v.n[VY]!=_u.n[VY] || _v.n[VZ]!=_u.n[VZ]; }
	int Compare(const ARCVector3& _v, const double _epsilon=ARCMath::EPSILON) const;						// this == v ? with epsilon

	double DistanceTo(const ARCVector3& _rhs) const;
	double Magnitude2() const
	{ return n[VX]*n[VX]+n[VY]*n[VY]+n[VZ]*n[VZ]; }

    ARCVector3& RotateXY(double _degrees);						// rotate vector (3D)
	ARCVector3& RotateXY(double _degrees, const ARCVector3& _point);	// rotate vector around _point (3D)
	ARCVector3 GetRotateXY(double _degree)const;
	ARCVector3 GetLengtDir(double dlen)const;

    ARCVector3& SetLength(const double _len);		// sets the length of the vector to len
    double Length() const{ return Magnitude(); }	// returns distance from origin to receiver (length of vector)	
	double GetCosOfTwoVector( const ARCVector3& _rhs ) const;				// get cos angle of the two vectors.

    ARCVector3 GetRandPoint(const double _radius) const;
    ARCVector3 GetNearest(const std::vector<ARCVector3>& _vList) const;	// return the closest point
	ARCVector3 PerpendicularLCopy() const { return ARCVector3(-n[VY], n[VX],n[VZ]); } //left perpendicular
	ARCVector3 PerpendicularRCopy() const { return ARCVector3(-n[VY], n[VX],n[VZ]); } //right perpendicular;


	CPoint2008 operator+(const CPoint2008& pt)const;
	ARCVector2 xy()const{ return ARCVector2(n[VX], n[VY]); }
	void DoOffset(double _xOffset, double _yOffset, double _zOffset );

	static ARCVector3 ZERO;
	static ARCVector3 UNIT_X;
	static ARCVector3 UNIT_Y;
	static ARCVector3 UNIT_Z;
};

typedef ARCVector3 ARCPoint3;

inline ARCVector3 cross(const ARCVector3& v1,const ARCVector3 & v2){
	return v1 ^ v2;
}

#ifndef _DEBUG
inline double ARCVector3::Magnitude() const
{
	return sqrt(n[VX]*n[VX]+n[VY]*n[VY]+n[VZ]*n[VZ]);
}
#endif
inline ARCVector2& ARCVector2::operator=( const ARCVector3& _rhs )
{ 
	n[VX]=_rhs.n[VX]; n[VY]=_rhs.n[VY];
	return *this;
}

#endif //ARC_VECTOR_H
