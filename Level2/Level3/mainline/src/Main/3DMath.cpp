// 3DMath.cpp: implementation of the C3DMath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DMath.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

//////////////////////////////////////////////////////////////////////
// C3DMath
//////////////////////////////////////////////////////////////////////

C3DMath::C3DMath()
{

}

C3DMath::~C3DMath()
{

}

double C3DMath::DegreesToRadians(double dAngle)
{
	double r = dAngle * PiOver180; 
	return r;
}

double C3DMath::RadiansToDegrees(double dAngle)
{
	double d = dAngle * PiUnder180;
	return d;
}

void C3DMath::GetTranslate3D(double tx, double ty, double tz, CMatrix4x4* pM)
{
	//	Translation matrix identified as:
	//		 ----------------
	//		| 1   0   0   Tx |
	//		| 0   1   0   Ty |
	//		| 0   0   1   Tz |
	//		| 0   0   0   1  |
	//		 ----------------
	pM->SetIdentity();
	(*pM)[0][3] = tx; (*pM)[1][3] = ty; (*pM)[2][3] = tz;
}

void C3DMath::GetTranslate3D(C3DMath::CVector3D& tv, CMatrix4x4* pM)
{
	GetTranslate3D(tv[X3D], tv[Y3D], tv[Z3D], pM);
}

void C3DMath::GetRotate3D(C3DMath::CVector3D& axis, double dAngle, CMatrix4x4* pM)
{
	double dCos, dSin;
	pM->SetZero();
	dCos = cos(C3DMath::DegreesToRadians(dAngle));
	dSin = sin(C3DMath::DegreesToRadians(dAngle));
	// compensate for rounding errors
	if(fabs(dCos) < SMALL_NUMBER)
		dCos = 0.0;
	if(fabs(dSin) < SMALL_NUMBER)
		dSin = 0.0;

	axis.Normalize();

	double S[3][3];
	S[0][0] = 0.0; S[0][1] = -axis[Z3D]; S[0][2] = axis[Y3D];
	S[1][0] = axis[Z3D]; S[1][1] = 0.0; S[1][2] = -axis[X3D];
	S[2][0] = -axis[Y3D]; S[2][1] = axis[X3D]; S[2][2] = 0.0;
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			S[i][j] = S[i][j] * dSin;
		}
	}
	double UUT[3][3];
	UUT[0][0] = axis[X3D] * axis[X3D]; UUT[0][1] = axis[X3D] * axis[Y3D]; UUT[0][2] = axis[X3D] * axis[Z3D];
	UUT[1][0] = axis[Y3D] * axis[X3D]; UUT[1][1] = axis[Y3D] * axis[Y3D]; UUT[1][2] = axis[Y3D] * axis[Z3D];
	UUT[2][0] = axis[Z3D] * axis[X3D]; UUT[2][1] = axis[Z3D] * axis[Y3D]; UUT[2][2] = axis[Z3D] * axis[Z3D];
	double I[3][3];
	for( i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			if(i == j)
				I[i][j] = 1.0;
			else
				I[i][j] = 0.0;
		}
	}
	for(i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			(*pM)[i][j] = (UUT[i][j]) + ((I[i][j] - UUT[i][j]) * dCos) + (S[i][j]);
		}
	}
	(*pM)[3][3] = 1.0;
}

void C3DMath::GetRotate3D(short axis, double dAngle, CMatrix4x4* pM)
{
	double dCos, dSin;
	pM->SetZero();
	dCos = cos(C3DMath::DegreesToRadians(dAngle));
	dSin = sin(C3DMath::DegreesToRadians(dAngle));
	// compensate for rounding errors
	if(fabs(dCos) < SMALL_NUMBER)
		dCos = 0.0;
	if(fabs(dSin) < SMALL_NUMBER)
		dSin = 0.0;
	
	switch(axis)
	{
		case X3D:
			//	Rotation about the X-Axis:
			//		 -----------------------
			//		| 1     0      0      0 |
			//		| 0     cosX   -sinX  0 |
			//		| 0     sinX   cosX   0 |
			//		| 0     0      0      1 |
			//		 -----------------------

			*pM[0][0] = 1.0;
			*pM[1][1] = dCos;
			*pM[1][2] = -dSin;
			*pM[2][1] = dSin;
			*pM[2][2] = dCos;
			*pM[3][3] = 1.0;
			break;

		case Y3D:
			//	Rotation about the Y-Axis:
			//		 -----------------------
			//		| cosY  0      sinY   0 |
			//		| 0     1      0      0 |
			//		| -sinY 0      cosY   0 |
			//		| 0     0      0      1 |
			//		 -----------------------
			
			*pM[0][0] = dCos;
			*pM[0][2] = dSin;
			*pM[1][1] = 1.0;
			*pM[2][0] = -dSin;
			*pM[2][2] = dCos;
			*pM[3][3] = 1.0;
			break;

		case Z3D:
			//	Rotation about the Z-Axis:
			//		 -----------------------
			//		| cosZ  -sinZ  0      0 |
			//		| sinZ  cosZ   0      0 |
			//		| 0     0      1      0 |
			//		| 0     0      0      1 |
			//		 -----------------------

			*pM[0][0] = dCos;
			*pM[0][1] = -dSin;
			*pM[1][0] = dSin;
			*pM[1][1] = dCos;
			*pM[2][2] = 1.0;
			*pM[3][3] = 1.0;
			break;
	}
}


///////////////////////////////////////////////////////////
//	C3DMath::CMatrix4x4
///////////////////////////////////////////////////////////
C3DMath::CMatrix4x4::CMatrix4x4()
{
}

C3DMath::CMatrix4x4::CMatrix4x4(short type)
{
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			if((type == UNIT_MATRIX) && (i == j))
				n[i][i] = 1.0;
			else
				n[i][j] = 0.0;
		}
	}
}

C3DMath::CMatrix4x4::CMatrix4x4(const C3DMath::CMatrix4x4& m)
{
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
				n[i][j] = m.n[i][j];
		}
	}
}



C3DMath::CMatrix4x4::~CMatrix4x4()
{
}

void C3DMath::CMatrix4x4::SetZero()
{
	for(int i=0; i<4; i++) 
		for(int j=0; j<4; j++)
			n[j][i] = 0.0;
}

void C3DMath::CMatrix4x4::SetIdentity()
{
	SetZero();
	for(int i=0; i<4; i++) 
		n[i][i] = 1.0;
}

void C3DMath::CMatrix4x4::Invert()
{
	//TODO: Implement Invert!
	ASSERT(0);
}

void C3DMath::CMatrix4x4::Transpose()
{
	CMatrix4x4 m(*this);
	for(int i=0; i<4; i++) 
		for(int j=0; j<4; j++)
			n[j][i] = m.n[i][j];
}

C3DMath::CMatrix4x4& C3DMath::CMatrix4x4::operator = (const C3DMath::CMatrix4x4& m)
{
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			n[i][j] = m.n[i][j];
		}
	}
	return *this;
}

C3DMath::CMatrix4x4& C3DMath::CMatrix4x4::operator += (const C3DMath::CMatrix4x4& m)
{
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			n[i][j] += m.n[i][j];
		}
	}
	return *this;
}

C3DMath::CMatrix4x4& C3DMath::CMatrix4x4::operator -= (const C3DMath::CMatrix4x4& m)
{
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			n[i][j] -= m.n[i][j];
		}
	}
	return *this;
}

C3DMath::CMatrix4x4& C3DMath::CMatrix4x4::operator *= (const double d)
{
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			n[i][j] *= d;
		}
	}
	return *this;
}

C3DMath::CMatrix4x4& C3DMath::CMatrix4x4::operator /= (const double d)
{
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			n[i][j] /= d;
		}
	}
	return *this;
}

double* C3DMath::CMatrix4x4::operator [] (int i)
{
	ASSERT((i>=0) && (i<=3));
	return n[i];
}

C3DMath::CMatrix4x4 operator - ( const C3DMath::CMatrix4x4& m )
{
	C3DMath::CMatrix4x4* pMTemp = new C3DMath::CMatrix4x4(m);
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
				(*pMTemp)[i][j] = -(*pMTemp)[i][j];
		}
	}
	return *pMTemp;
}

C3DMath::CMatrix4x4 operator + ( const C3DMath::CMatrix4x4& a, const C3DMath::CMatrix4x4& b )
{
	C3DMath::CMatrix4x4* pMTemp = new C3DMath::CMatrix4x4();
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
				(*pMTemp)[i][j] = a.n[i][j] + b.n[i][j];
		}
	}
	return *pMTemp;
}

C3DMath::CMatrix4x4 operator - ( const C3DMath::CMatrix4x4& a, const C3DMath::CMatrix4x4& b )
{
	C3DMath::CMatrix4x4* pMTemp = new C3DMath::CMatrix4x4();
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
				(*pMTemp)[i][j] = a.n[i][j] - b.n[i][j];
		}
	}
	return *pMTemp;
}

C3DMath::CMatrix4x4 operator * ( const C3DMath::CMatrix4x4& a, const C3DMath::CMatrix4x4& b )
{
	double dSum;
	C3DMath::CMatrix4x4* pMTemp = new C3DMath::CMatrix4x4();
	for(int ia=0; ia<4; ia++) {			// for each row in a
		for(int jb=0; jb<4; jb++) {		// for each column in b
			dSum = 0;
			for(int k=0; k<4; k++) {
				dSum += (a.n[ia][k]  * b.n[k][jb]);
			}
			(*pMTemp)[ia][jb] = dSum;
		}
	}
	return *pMTemp;
}

C3DMath::CMatrix4x4 operator * ( const C3DMath::CMatrix4x4& m, const double d)
{
	C3DMath::CMatrix4x4* pMTemp = new C3DMath::CMatrix4x4();
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
				(*pMTemp)[i][j] = m.n[i][j] * d;
		}
	}
	return *pMTemp;
}

C3DMath::CMatrix4x4 operator / ( const C3DMath::CMatrix4x4& m, const double d)
{
	C3DMath::CMatrix4x4* pMTemp = new C3DMath::CMatrix4x4();
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
				(*pMTemp)[i][j] = m.n[i][j] / d;
		}
	}
	return *pMTemp;
}

C3DMath::CMatrix4x4 operator * ( const double d, const C3DMath::CMatrix4x4& m )
{
	return m*d;
}

BOOL operator == ( const C3DMath::CMatrix4x4& a, const C3DMath::CMatrix4x4& b )
{
	return ((a.n[0][0] == b.n[0][0]) && (a.n[0][1] == b.n[0][1]) && (a.n[0][2] == b.n[0][2]) && (a.n[0][3] == b.n[0][3]) &&
			(a.n[1][0] == b.n[1][0]) && (a.n[1][1] == b.n[1][1]) && (a.n[1][2] == b.n[1][2]) && (a.n[1][3] == b.n[1][3]) && 
			(a.n[2][0] == b.n[2][0]) && (a.n[2][1] == b.n[2][1]) && (a.n[2][2] == b.n[2][2]) && (a.n[2][3] == b.n[2][3]) && 
			(a.n[3][0] == b.n[3][0]) && (a.n[3][1] == b.n[3][1]) && (a.n[3][2] == b.n[3][2]) && (a.n[3][3] == b.n[3][3]));
}

BOOL operator != ( const C3DMath::CMatrix4x4& a, const C3DMath::CMatrix4x4& b )
{
	return !(a == b);
}


ostream& operator << ( ostream& s, C3DMath::CMatrix4x4& m)
{
	return s << "|" << m.n[0][0] << " " << m.n[0][1] << " " << m.n[0][2] << " " << m.n[0][3] << "|" << "\n"
			 << "|" << m.n[1][0] << " " << m.n[1][1] << " " << m.n[1][2] << " " << m.n[1][3] << "|" << "\n"
			 << "|" << m.n[2][0] << " " << m.n[2][1] << " " << m.n[2][2] << " " << m.n[2][3] << "|" << "\n"
			 << "|" << m.n[3][0] << " " << m.n[3][1] << " " << m.n[3][2] << " " << m.n[3][3] << "|";
}

istream& operator >> ( istream& s, C3DMath::CMatrix4x4& m)
{
	C3DMath::CMatrix4x4 mTemp;
	char c = ' ';
	while(isspace(c)) {
		s >> c;
	}
	if(c == '|') {
		s >> mTemp.n[0][0] >> mTemp.n[0][1] >> mTemp.n[0][2] >> mTemp.n[0][3];
		s >> c;
		if(c != '|')
			s.bad();
		s >> c;
		if(c != '\n')
			s.bad();
		s >> c;
		if(c != '|')
			s.bad();
		s >> mTemp.n[1][0] >> mTemp.n[1][1] >> mTemp.n[1][2] >> mTemp.n[1][3];
		s >> c;
		if(c != '|')
			s.bad();
		s >> c;
		if(c != '\n')
			s.bad();
		s >> c;
		if(c != '|')
			s.bad();
		s >> mTemp.n[2][0] >> mTemp.n[2][1] >> mTemp.n[2][2] >> mTemp.n[3][3];
		s >> c;
		if(c != '|')
			s.bad();
		s >> c;
		if(c != '\n')
			s.bad();
		s >> c;
		if(c != '|')
			s.bad();
		s >> mTemp.n[3][0] >> mTemp.n[3][1] >> mTemp.n[3][2] >> mTemp.n[3][3];
		s >> c;
		if(c != '|')
			s.bad();
	}
	else {
		s.bad();
	}
	if(s)
		m = mTemp;
	return s;
}

///////////////////////////////////////////////////////////////
//	CVector2D
///////////////////////////////////////////////////////////////

C3DMath::CVector2D::CVector2D()
{
}

C3DMath::CVector2D::CVector2D(const double d)
{
	n[X3D] = n[Y3D] = d;
}

C3DMath::CVector2D::CVector2D(const double x, const double y)
{
	n[X3D] = x;
	n[Y3D] = y;
}

C3DMath::CVector2D::CVector2D(const CVector2D& v)
{
	n[X3D] = v.n[X3D];
	n[Y3D] = v.n[Y3D];
}

C3DMath::CVector2D::CVector2D(const CVector3D& v)
{
	n[X3D] = v.n[X3D];
	n[Y3D] = v.n[Y3D];
}

C3DMath::CVector2D::~CVector2D()
{
}

void C3DMath::CVector2D::Normalize()
{
	double dMag = MAG2(n);
	if(dMag) {
		n[X3D] /= dMag;
		n[Y3D] /= dMag;
	}
}

double C3DMath::CVector2D::Magnitude()
{
	return MAG2(n);
}

C3DMath::CVector2D& C3DMath::CVector2D::operator = (const CVector2D& v)
{
	n[X3D] = v.n[X3D];
	n[Y3D] = v.n[Y3D];
	return *this;
}

C3DMath::CVector2D& C3DMath::CVector2D::operator += (const CVector2D& v)
{
	n[X3D] += v.n[X3D];
	n[Y3D] += v.n[Y3D];
	return *this;
}

C3DMath::CVector2D& C3DMath::CVector2D::operator -= (const CVector2D& v)
{
	n[X3D] -= v.n[X3D];
	n[Y3D] -= v.n[Y3D];
	return *this;
}

C3DMath::CVector2D& C3DMath::CVector2D::operator *= (const double d)
{
	n[X3D] *= d;
	n[Y3D] *= d;
	return *this;
}

C3DMath::CVector2D& C3DMath::CVector2D::operator /= (const double d)
{
	n[X3D] /= d;
	n[Y3D] /= d;
	return *this;
}

double& C3DMath::CVector2D::operator [] (int i)
{
	ASSERT((i>=X3D) && (i<=Y3D));
	return n[i];
}

C3DMath::CVector2D operator - (const C3DMath::CVector2D& v)
{
	return C3DMath::CVector2D(-v.n[X3D], -v.n[Y3D]);
}

C3DMath::CVector2D operator + (const C3DMath::CVector2D& v, const C3DMath::CVector2D& u)
{
	return C3DMath::CVector2D(v.n[X3D] + u.n[X3D], v.n[Y3D] + u.n[Y3D]);
}

C3DMath::CVector2D operator - (const C3DMath::CVector2D& v, const C3DMath::CVector2D& u)
{
	return C3DMath::CVector2D(v.n[X3D] - u.n[X3D], v.n[Y3D] - u.n[Y3D]);
}

C3DMath::CVector2D operator * (const C3DMath::CVector2D& v, const double d)
{
	return C3DMath::CVector2D(v.n[X3D] * d, v.n[Y3D] * d);
}

C3DMath::CVector2D operator * (const double d, const C3DMath::CVector2D& v)
{
	return v*d;
}

double operator * (const C3DMath::CVector2D& v, const C3DMath::CVector2D& u)
{
	return (v.n[X3D] * u.n[X3D]) + (v.n[Y3D] * u.n[Y3D]);
}

BOOL operator == (const C3DMath::CVector2D& v, const C3DMath::CVector2D& u)
{
	return (v.n[X3D] == u.n[X3D]) && (v.n[Y3D] == u.n[Y3D]);
}

BOOL operator != (const C3DMath::CVector2D& v, const C3DMath::CVector2D& u)
{
	return !(v == u);
}

ostream& operator << (ostream& s, C3DMath::CVector2D& v)
{
	return s << "|" << v.n[X3D] << ", " << v.n[Y3D] << "|";
}

istream& operator >> (istream& s, C3DMath::CVector2D& v)
{
	C3DMath::CVector2D vTemp;
	char c = ' ';
	while(isspace(c)) {
		s >> c;
	}
	if(c == '|') {
		s >> vTemp[X3D] >> vTemp[Y3D];
		s >> c;
		if(c != '|')
			s.bad();
	}
	else {
		s.bad();
	}
	if(s)
		v = vTemp;
	return s;
}

///////////////////////////////////////////////////////////////
//	CVector3D
///////////////////////////////////////////////////////////////

C3DMath::CVector3D::CVector3D()
{
}

C3DMath::CVector3D::CVector3D(const double d)
{
	n[X3D] = n[Y3D] = n[Z3D] = d;
}

C3DMath::CVector3D::CVector3D(const double x, const double y, const double z)
{
	n[X3D] = x;
	n[Y3D] = y;
	n[Z3D] = z;
}

C3DMath::CVector3D::CVector3D(const CVector3D& v)
{
	n[X3D] = v.n[X3D];
	n[Y3D] = v.n[Y3D];
	n[Z3D] = v.n[Z3D];
}

C3DMath::CVector3D::CVector3D(const CVector2D& v)
{
	n[X3D] = v.n[X3D];
	n[Y3D] = v.n[Y3D];
	n[Z3D] = 0.0;
}

C3DMath::CVector3D::~CVector3D()
{
}

void C3DMath::CVector3D::Normalize()
{
	double dMag = MAG3(n);
	if(dMag) {
		n[X3D] /= dMag;
		n[Y3D] /= dMag;
		n[Z3D] /= dMag;
	}
}

double C3DMath::CVector3D::Magnitude()
{
	return MAG3(n);
}

C3DMath::CVector3D& C3DMath::CVector3D::operator = (const CVector3D& v)
{
	n[X3D] = v.n[X3D];
	n[Y3D] = v.n[Y3D];
	n[Z3D] = v.n[Z3D];
	return *this;
}

C3DMath::CVector3D& C3DMath::CVector3D::operator += (const CVector3D& v)
{
	n[X3D] += v.n[X3D];
	n[Y3D] += v.n[Y3D];
	n[Z3D] += v.n[Z3D];
	return *this;
}

C3DMath::CVector3D& C3DMath::CVector3D::operator -= (const CVector3D& v)
{
	n[X3D] -= v.n[X3D];
	n[Y3D] -= v.n[Y3D];
	n[Z3D] -= v.n[Z3D];
	return *this;
}

C3DMath::CVector3D& C3DMath::CVector3D::operator *= (const double d)
{
	n[X3D] *= d;
	n[Y3D] *= d;
	n[Z3D] *= d;
	return *this;
}

C3DMath::CVector3D& C3DMath::CVector3D::operator /= (const double d)
{
	n[X3D] /= d;
	n[Y3D] /= d;
	n[Z3D] /= d;
	return *this;
}

double& C3DMath::CVector3D::operator [] (int i)
{
	ASSERT((i>=X3D) && (i<=Z3D));
	return n[i];
}

double C3DMath::CVector3D::operator [] (int i) const
{
	ASSERT((i>=X3D) && (i<=Z3D));
	return n[i];
}

C3DMath::CVector3D operator - (const C3DMath::CVector3D& v)
{
	return C3DMath::CVector3D(-v.n[X3D], -v.n[Y3D], -v.n[Z3D]);
}

C3DMath::CVector3D operator + (const C3DMath::CVector3D& v, const C3DMath::CVector3D& u)
{
	return C3DMath::CVector3D(v.n[X3D] + u.n[X3D], v.n[Y3D] + u.n[Y3D], v.n[Z3D] + u.n[Z3D]);
}

C3DMath::CVector3D operator - (const C3DMath::CVector3D& v, const C3DMath::CVector3D& u)
{
	return C3DMath::CVector3D(v.n[X3D] - u.n[X3D], v.n[Y3D] - u.n[Y3D], v.n[Z3D] - u.n[Z3D]);
}

C3DMath::CVector3D operator * (const C3DMath::CVector3D& v, const double d)
{
	return C3DMath::CVector3D(v.n[X3D] * d, v.n[Y3D] * d, v.n[Z3D] * d);
}

C3DMath::CVector3D operator * (const double d, const C3DMath::CVector3D& v)
{
	return v*d;
}

C3DMath::CVector3D operator / ( const C3DMath::CVector3D& v, const double d)
{
	return C3DMath::CVector3D(v.n[X3D] / d, v.n[Y3D] / d, v.n[Z3D] / d);
}

C3DMath::CVector3D operator * (const C3DMath::CMatrix4x4& m, const C3DMath::CVector3D& v)
{
	// Note that this only works if the last row in m is 0 0 0 1 and we assume v[W] = 1
	return C3DMath::CVector3D((m.n[0][0] * v.n[0]) + (m.n[0][1] * v.n[1]) + (m.n[0][2] * v.n[2]) + m.n[0][3],
							  (m.n[1][0] * v.n[0]) + (m.n[1][1] * v.n[1]) + (m.n[1][2] * v.n[2]) + m.n[1][3],
							  (m.n[2][0] * v.n[0]) + (m.n[2][1] * v.n[1]) + (m.n[2][2] * v.n[2]) + m.n[2][3]);
}

double operator * (const C3DMath::CVector3D& v, const C3DMath::CVector3D& u)
{
	return (v.n[X3D] * u.n[X3D]) + (v.n[Y3D] * u.n[Y3D]) + (v.n[Z3D] * u.n[Z3D]);
}

C3DMath::CVector3D operator ^ (const C3DMath::CVector3D& v, const C3DMath::CVector3D& u)
{
	return C3DMath::CVector3D((v.n[Y3D] * u.n[Z3D]) - (v.n[Z3D] * u.n[Y3D]),
							  (v.n[Z3D] * u.n[X3D]) - (v.n[X3D] * u.n[Z3D]),
							  (v.n[X3D] * u.n[Y3D]) - (v.n[Y3D] * u.n[X3D]));
}

BOOL operator == (const C3DMath::CVector3D& v, const C3DMath::CVector3D& u)
{
	return (v.n[X3D] == u.n[X3D]) && (v.n[Y3D] == u.n[Y3D]) && (v.n[Z3D] == u.n[Z3D]);
}

BOOL operator != (const C3DMath::CVector3D& v, const C3DMath::CVector3D& u)
{
	return !(v == u);
}


ostream& operator << (ostream& s, C3DMath::CVector3D& v)
{
	return s << "|" << v.n[X3D] << ", " << v.n[Y3D] << ", " << v.n[Z3D] << "|";
}

istream& operator >> (istream& s, C3DMath::CVector3D& v)
{
	C3DMath::CVector3D vTemp;
	char c = ' ';
	while(isspace(c)) {
		s >> c;
	}
	if(c == '|') {
		s >> vTemp[X3D] >> vTemp[Y3D] >> vTemp[Z3D];
		s >> c;
		if(c != '|')
			s.bad();
	}
	else {
		s.bad();
	}
	if(s)
		v = vTemp;
	return s;
}


///////////////////////////////////////////////////////////////
//	CVector4D
///////////////////////////////////////////////////////////////

C3DMath::CVector4D::CVector4D()
{
}

C3DMath::CVector4D::CVector4D(const double d)
{
	n[X3D] = n[Y3D] = n[Z3D] = n[W3D] = d;
}

C3DMath::CVector4D::CVector4D(const double x, const double y, const double z, const double w)
{
	n[X3D] = x;
	n[Y3D] = y;
	n[Z3D] = z;
	n[W3D] = w;
}

C3DMath::CVector4D::CVector4D(const CVector4D& v)
{
	n[X3D] = v.n[X3D];
	n[Y3D] = v.n[Y3D];
	n[Z3D] = v.n[Z3D];
	n[W3D] = v.n[W3D];
}

C3DMath::CVector4D::~CVector4D()
{
}

void C3DMath::CVector4D::Normalize()
{
	double dMag = MAG4(n);
	if(dMag) {
		n[X3D] /= dMag;
		n[Y3D] /= dMag;
		n[Z3D] /= dMag;
		n[W3D] /= dMag;
	}
}

double C3DMath::CVector4D::Magnitude()
{
	return MAG4(n);
}

C3DMath::CVector4D& C3DMath::CVector4D::operator = (const CVector4D& v)
{
	n[X3D] = v.n[X3D];
	n[Y3D] = v.n[Y3D];
	n[Z3D] = v.n[Z3D];
	n[W3D] = v.n[W3D];
	return *this;
}

C3DMath::CVector4D& C3DMath::CVector4D::operator += (const CVector4D& v)
{
	n[X3D] += v.n[X3D];
	n[Y3D] += v.n[Y3D];
	n[Z3D] += v.n[Z3D];
	n[W3D] += v.n[W3D];
	return *this;
}

C3DMath::CVector4D& C3DMath::CVector4D::operator -= (const CVector4D& v)
{
	n[X3D] -= v.n[X3D];
	n[Y3D] -= v.n[Y3D];
	n[Z3D] -= v.n[Z3D];
	n[W3D] -= v.n[W3D];
	return *this;
}

C3DMath::CVector4D& C3DMath::CVector4D::operator *= (const double d)
{
	n[X3D] *= d;
	n[Y3D] *= d;
	n[Z3D] *= d;
	n[W3D] *= d;
	return *this;
}

C3DMath::CVector4D& C3DMath::CVector4D::operator /= (const double d)
{
	n[X3D] /= d;
	n[Y3D] /= d;
	n[Z3D] /= d;
	n[W3D] /= d;
	return *this;
}

double& C3DMath::CVector4D::operator [] (int i)
{
	ASSERT((i>=X3D) && (i<=W3D));
	return n[i];
}

C3DMath::CVector4D operator - (const C3DMath::CVector4D& v)
{
	return C3DMath::CVector4D(-v.n[X3D], -v.n[Y3D], -v.n[Z3D], -v.n[W3D]);
}

C3DMath::CVector4D operator + (const C3DMath::CVector4D& v, const C3DMath::CVector4D& u)
{
	return C3DMath::CVector4D(v.n[X3D] + u.n[X3D], v.n[Y3D] + u.n[Y3D], v.n[Z3D] + u.n[Z3D], v.n[W3D] + u.n[W3D]);
}

C3DMath::CVector4D operator - (const C3DMath::CVector4D& v, const C3DMath::CVector4D& u)
{
	return C3DMath::CVector4D(v.n[X3D] - u.n[X3D], v.n[Y3D] - u.n[Y3D], v.n[Z3D] - u.n[Z3D], v.n[W3D] - u.n[W3D]);
}

C3DMath::CVector4D operator * (const C3DMath::CVector4D& v, const double d)
{
	return C3DMath::CVector4D(v.n[X3D] * d, v.n[Y3D] * d, v.n[Z3D] * d, v.n[W3D] * d);
}

C3DMath::CVector4D operator * (const double d, const C3DMath::CVector4D& v)
{
	return v*d;
}

C3DMath::CVector4D operator * (const C3DMath::CMatrix4x4& m, const C3DMath::CVector4D& v)
{
	return C3DMath::CVector4D((m.n[0][0] * v.n[0]) + (m.n[0][1] * v.n[1]) + (m.n[0][2] * v.n[2]) + (m.n[0][3] * v.n[3]),
							  (m.n[1][0] * v.n[0]) + (m.n[1][1] * v.n[1]) + (m.n[1][2] * v.n[2]) + (m.n[1][3] * v.n[3]),
							  (m.n[2][0] * v.n[0]) + (m.n[2][1] * v.n[1]) + (m.n[2][2] * v.n[2]) + (m.n[2][3] * v.n[3]),
							  (m.n[3][0] * v.n[0]) + (m.n[3][1] * v.n[1]) + (m.n[3][2] * v.n[2]) + (m.n[3][3] * v.n[3]));
}

double operator * (const C3DMath::CVector4D& v, const C3DMath::CVector4D& u)
{
	return (v.n[X3D] * u.n[X3D]) + (v.n[Y3D] * u.n[Y3D]) + (v.n[Z3D] * u.n[Z3D]);
}

C3DMath::CVector4D operator ^ (const C3DMath::CVector4D& v, const C3DMath::CVector4D& u)
{
	C3DMath::CVector4D tmpV(v);
	C3DMath::CVector4D tmpU(u);
	tmpV /= tmpV[W3D];
	tmpU /= tmpU[W3D];
	return C3DMath::CVector4D((tmpV.n[Y3D] * tmpU.n[Z3D]) - (tmpV.n[Z3D] * tmpU.n[Y3D]),
							  (tmpV.n[Z3D] * tmpU.n[X3D]) - (tmpV.n[X3D] * tmpU.n[Z3D]),
							  (tmpV.n[X3D] * tmpU.n[Y3D]) - (tmpV.n[Y3D] * tmpU.n[X3D]),
							  1.0);
}

BOOL operator == (const C3DMath::CVector4D& v, const C3DMath::CVector4D& u)
{
	return (v.n[X3D] == u.n[X3D]) && (v.n[Y3D] == u.n[Y3D]) && (v.n[Z3D] == u.n[Z3D]) && (v.n[W3D] == u.n[W3D]);
}

BOOL operator != (const C3DMath::CVector4D& v, const C3DMath::CVector4D& u)
{
	return !(v == u);
}