// 3DMath.h: interface for the C3DMath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DMATH_H__6CAF249D_7669_11D3_9156_0080C8F982B1__INCLUDED_)
#define AFX_3DMATH_H__6CAF249D_7669_11D3_9156_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#include <fstream>
#include <vector>

#define MAG2(v) sqrt((v[0] * v[0]) + (v[1] * v[1]))
#define MAG3(v)	sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]))
#define MAG4(v) sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]) + (v[3] * v[3]))

#define X2D		0
#define Y2D		1
#define X3D		0
#define Y3D		1
#define Z3D		2
#define W3D		3

#define Pi				3.14159265359
#define PiOver180		1.74532925199433E-002
#define PiUnder180		5.72957795130823E+001

#define SMALL_NUMBER	0.00001

#define UNIT_MATRIX		1
#define ZERO_MATRIX		0

class C3DMath  
{
public:
	class CMatrix4x4;
	class CVector2D;
	class CVector3D;
	class CVector4D;

	typedef std::vector<CVector2D> C2DVectorList;
	typedef std::vector<CVector3D> C3DVectorList;
	typedef std::vector<CVector4D> C4DVectorList;

	C3DMath();
	virtual ~C3DMath();

	static double DegreesToRadians(double dAngle);
	static double RadiansToDegrees(double dAngle);

	static void GetTranslate3D(double tx, double ty, double tz, CMatrix4x4* pM);
	static void GetTranslate3D(CVector3D& tv, CMatrix4x4* pM);
	static void GetRotate3D(short axis, double dAngle, CMatrix4x4* pM);
	static void GetRotate3D(CVector3D& axis, double dAngle, CMatrix4x4* pM);

	class CMatrix4x4
	{
	protected:
		double n[4][4];

	public:
		// Construction
		CMatrix4x4();
		CMatrix4x4(short type);
		CMatrix4x4(const CMatrix4x4& m);

		// Destruction
		virtual ~CMatrix4x4();

		// functions
		void Transpose();
		void Invert();
		void SetZero();
		void SetIdentity();

		// Operators
		CMatrix4x4& operator = ( const CMatrix4x4& m );
		CMatrix4x4& operator += ( const CMatrix4x4& m );
		CMatrix4x4& operator -= ( const CMatrix4x4& m );
		CMatrix4x4& operator *= ( const double d );
		CMatrix4x4& operator /= ( const double d );
		double* operator [] ( int i );

		friend CMatrix4x4 operator - ( const CMatrix4x4& m );
		friend CMatrix4x4 operator + ( const CMatrix4x4& a, const CMatrix4x4& b );
		friend CMatrix4x4 operator - ( const CMatrix4x4& a, const CMatrix4x4& b );
		friend CMatrix4x4 operator * ( const CMatrix4x4& a, const CMatrix4x4& b );
		friend CMatrix4x4 operator * ( const CMatrix4x4& m, const double d);
		friend CMatrix4x4 operator / ( const CMatrix4x4& m, const double d);
		friend CMatrix4x4 operator * ( const double d, const CMatrix4x4& m );
		friend BOOL operator == ( const CMatrix4x4& a, const CMatrix4x4& b );
		friend BOOL operator != ( const CMatrix4x4& a, const CMatrix4x4& b );
		friend std::ostream& operator << ( std::ostream& s, CMatrix4x4& m);
		friend std::istream& operator >> ( std::istream& s, CMatrix4x4& m);

		friend CVector3D operator * ( const CMatrix4x4& m, const CVector3D& v );
		friend CVector4D operator * ( const CMatrix4x4& m, const CVector4D& v );
	};

	class CVector2D
	{
	protected:
		double n[2];

	public:
		// construction
		CVector2D();
		CVector2D(const double x, const double y);
		CVector2D(const double d);
		CVector2D(const CVector2D& v);
		CVector2D(const CVector3D& v);
		
		// Destruction
		virtual ~CVector2D();

		// functions
		double X() const { return n[0]; }
		double Y() const { return n[1]; }
		double Magnitude();
		void Normalize();

		CVector2D& operator	=	( const CVector2D& v );
		CVector2D& operator	+=	( const CVector2D& v );
		CVector2D& operator	-=	( const CVector2D& v );
		CVector2D& operator	*=	( const double d );
		CVector2D& operator	/=	( const double d );
		double& operator [] ( int i );

		operator double*() { return n; }

		friend CVector2D operator - ( const CVector2D& v );							// -v
		friend CVector2D operator + ( const CVector2D& v, const CVector2D& u );		// v + u
		friend CVector2D operator - ( const CVector2D& v, const CVector2D& u );		// v - u
		friend CVector2D operator * ( const CVector2D& v, const double d );			// v * scalar
		friend CVector2D operator / ( const CVector2D& v, const double d );			// v / scalar
		friend CVector2D operator * ( const double d, const CVector2D& v );			// scalar * v
		friend double operator * ( const CVector2D& v, const CVector2D& u );		// v . u (dot product)
		friend BOOL operator == ( const CVector2D& v, const CVector2D& u );			// v == u ?
		friend BOOL operator != ( const CVector2D& v, const CVector2D& u );			// v != u ?
		friend std::ostream& operator << ( std::ostream& s, CVector2D& v );			// output to stream
		friend std::istream& operator >> ( std::istream& s, CVector2D& v );

		friend class CVector3D;

	};

	class CVector3D
	{
	protected:
		double n[3];
	
	public:
		
		// Construction
		CVector3D();
		CVector3D(const double x, const double y, const double z);
		CVector3D(const double d);
		CVector3D(const CVector3D& v);
		CVector3D(const CVector2D& v);
		
		// Destruction
		virtual ~CVector3D();

		// functions
		double Magnitude();
		void Normalize();
		
		// Operators
		CVector3D& operator	=	( const CVector3D& v );
		CVector3D& operator	+=	( const CVector3D& v );
		CVector3D& operator	-=	( const CVector3D& v );
		CVector3D& operator	*=	( const double d );
		CVector3D& operator	/=	( const double d );
		double& operator [] ( int i );
		double operator [] ( int i ) const;

		operator double*() { return n; }

		friend CVector3D operator - ( const CVector3D& v );							// -v
		friend CVector3D operator + ( const CVector3D& v, const CVector3D& u );		// v + u
		friend CVector3D operator - ( const CVector3D& v, const CVector3D& u );		// v - u
		friend CVector3D operator * ( const CVector3D& v, const double d );			// v * scalar
		friend CVector3D operator / ( const CVector3D& v, const double d );			// v / scalar
		friend CVector3D operator * ( const double d, const CVector3D& v );			// scalar * v
		friend CVector3D operator * ( const CMatrix4x4& m, const CVector3D& v );	// m . v
		friend double operator * ( const CVector3D& v, const CVector3D& u );		// v . u (dot product)
		friend CVector3D operator ^ ( const CVector3D& v, const CVector3D& u );		// v x u (cross product)
		friend BOOL operator == ( const CVector3D& v, const CVector3D& u );			// v == u ?
		friend BOOL operator != ( const CVector3D& v, const CVector3D& u );			// v != u ?
		friend std::ostream& operator << ( std::ostream& s, CVector3D& v );			// output to stream
		friend std::istream& operator >> ( std::istream& s, CVector3D& v );			// input from stream

		friend class CVector2D;
	};

	class CVector4D
	{
	protected:
		double n[4];
	
	public:
		
		// Construction
		CVector4D();
		CVector4D(const double x, const double y, const double z, const double w);
		CVector4D(const double d);
		CVector4D(const CVector4D& v);
		
		// Destruction
		virtual ~CVector4D();

		// functions
		double Magnitude();
		void Normalize();
		
		// Operators
		CVector4D& operator	=	( const CVector4D& v );
		CVector4D& operator	+=	( const CVector4D& v );
		CVector4D& operator	-=	( const CVector4D& v );
		CVector4D& operator	*=	( const double d );
		CVector4D& operator	/=	( const double d );
		double& operator [] ( int i );

		operator double*() { return n; }

		friend CVector4D operator - ( const CVector4D& v );							// -v
		friend CVector4D operator + ( const CVector4D& v, const CVector4D& u );		// v + u
		friend CVector4D operator - ( const CVector4D& v, const CVector4D& u );		// v - u
		friend CVector4D operator * ( const CVector4D& v, const double d );			// v * scalar
		friend CVector4D operator / ( const CVector4D& v, const double d );			// v / scalar
		friend CVector4D operator * ( const double d, const CVector4D& v );			// scalar * v
		friend CVector4D operator * ( const CMatrix4x4& m, const CVector4D& v );	// m . v
		friend double operator * ( const CVector4D& v, const CVector4D& u );		// v . u (dot product)
		// Cross Product of 4d vectors in cartesian coordinates (ie. divide x, y and z by w)
		friend CVector4D operator ^ ( const CVector4D& v, const CVector4D& u );		// v x u (cross product)
		friend BOOL operator == ( const CVector4D& v, const CVector4D& u );			// v == u ?
		friend BOOL operator != ( const CVector4D& v, const CVector4D& u );			// v != u ?
	};

};

#endif // !defined(AFX_3DMATH_H__6CAF249D_7669_11D3_9156_0080C8F982B1__INCLUDED_)