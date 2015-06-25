// ARCMatrix4x4.cpp: implementation of the ARCMatrix4x4 class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ARCMatrix4x4.h"

static const DistanceUnit IDENTITY[] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
};

ARCMatrix4x4 ARCMatrix4x4::GetTranslate3D(DistanceUnit _tx, DistanceUnit _ty, DistanceUnit _tz)
{
	ARCMatrix4x4 ret;
	GetTranslate3D(_tx, _ty, _tz, ret);
	return ret;
}
void ARCMatrix4x4::GetTranslate3D(DistanceUnit _tx, DistanceUnit _ty, DistanceUnit _tz, ARCMatrix4x4& _m)
{
	GetTranslate3D(ARCVector3(_tx,_ty,_tz), _m);
}

ARCMatrix4x4 ARCMatrix4x4::GetTranslate3D(const ARCVector3 _tv)
{
	ARCMatrix4x4 ret;
	GetTranslate3D(_tv, ret);
	return ret;
}

void ARCMatrix4x4::GetTranslate3D(const ARCVector3 _tv, ARCMatrix4x4& _m)
{
	//	Translation matrix identified as:
	//		 ----------------
	//		| 1   0   0   Tx |
	//		| 0   1   0   Ty |
	//		| 0   0   1   Tz |
	//		| 0   0   0   1  |
	//		 ----------------
	_m.SetIdentity();
	_m.n[0][3] = _tv[VX]; _m.n[1][3] = _tv[VY]; _m.n[2][3] = _tv[VZ];
}

ARCMatrix4x4 ARCMatrix4x4::GetRotate3D(short _axis, DistanceUnit _angle)
{
	ARCMatrix4x4 ret;
	GetRotate3D(_axis, _angle, ret);
	return ret;
}

void ARCMatrix4x4::GetRotate3D(short _axis, DistanceUnit _angle, ARCMatrix4x4& _m)
{
	double dCos, dSin;
	_m.SetZero();
	dCos = cos(ARCMath::DegreesToRadians(_angle));
	dSin = sin(ARCMath::DegreesToRadians(_angle));
	// compensate for rounding errors
	if(fabs(dCos) < ARCMath::EPSILON)
		dCos = 0.0;
	if(fabs(dSin) < ARCMath::EPSILON)
		dSin = 0.0;
	
	switch(_axis)
	{
		case VX:
			//	Rotation about the X-Axis:
			//		 -----------------------
			//		| 1     0      0      0 |
			//		| 0     cosX   -sinX  0 |
			//		| 0     sinX   cosX   0 |
			//		| 0     0      0      1 |
			//		 -----------------------

			_m.n[0][0] = 1.0;
			_m.n[1][1] = dCos;
			_m.n[1][2] = -dSin;
			_m.n[2][1] = dSin;
			_m.n[2][2] = dCos;
			_m.n[3][3] = 1.0;
			break;

		case VY:
			//	Rotation about the Y-Axis:
			//		 -----------------------
			//		| cosY  0      sinY   0 |
			//		| 0     1      0      0 |
			//		| -sinY 0      cosY   0 |
			//		| 0     0      0      1 |
			//		 -----------------------
			
			_m.n[0][0] = dCos;
			_m.n[0][2] = dSin;
			_m.n[1][1] = 1.0;
			_m.n[2][0] = -dSin;
			_m.n[2][2] = dCos;
			_m.n[3][3] = 1.0;
			break;

		case VZ:
			//	Rotation about the Z-Axis:
			//		 -----------------------
			//		| cosZ  -sinZ  0      0 |
			//		| sinZ  cosZ   0      0 |
			//		| 0     0      1      0 |
			//		| 0     0      0      1 |
			//		 -----------------------

			_m.n[0][0] = dCos;
			_m.n[0][1] = -dSin;
			_m.n[1][0] = dSin;
			_m.n[1][1] = dCos;
			_m.n[2][2] = 1.0;
			_m.n[3][3] = 1.0;
			break;
	}

}

ARCMatrix4x4 ARCMatrix4x4::GetRotate3D(const ARCVector3& _axis, DistanceUnit _angle)
{
	ARCMatrix4x4 ret;
	GetRotate3D(_axis, _angle, ret);
	return ret;
}

void ARCMatrix4x4::GetRotate3D(const ARCVector3& _axis, DistanceUnit _angle, ARCMatrix4x4& _m)
{
	double dCos, dSin;
	int i=0;
	_m.SetZero();
	dCos = cos(ARCMath::DegreesToRadians(_angle));
	dSin = sin(ARCMath::DegreesToRadians(_angle));
	// compensate for rounding errors
	if(fabs(dCos) < ARCMath::EPSILON)
		dCos = 0.0;
	if(fabs(dSin) < ARCMath::EPSILON)
		dSin = 0.0;

	ARCVector3 axis(_axis);
	axis.Normalize();

	double S[3][3];
	S[0][0] = 0.0; S[0][1] = -axis[VZ]; S[0][2] = axis[VY];
	S[1][0] = axis[VZ]; S[1][1] = 0.0; S[1][2] = -axis[VX];
	S[2][0] = -axis[VY]; S[2][1] = axis[VX]; S[2][2] = 0.0;
	for(i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			S[i][j] = S[i][j] * dSin;
		}
	}
	double UUT[3][3];
	UUT[0][0] = axis[VX] * axis[VX]; UUT[0][1] = axis[VX] * axis[VY]; UUT[0][2] = axis[VX] * axis[VZ];
	UUT[1][0] = axis[VY] * axis[VX]; UUT[1][1] = axis[VY] * axis[VY]; UUT[1][2] = axis[VY] * axis[VZ];
	UUT[2][0] = axis[VZ] * axis[VX]; UUT[2][1] = axis[VZ] * axis[VY]; UUT[2][2] = axis[VZ] * axis[VZ];
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
			_m.n[i][j] = (UUT[i][j]) + ((I[i][j] - UUT[i][j]) * dCos) + (S[i][j]);
		}
	}
	_m.n[3][3] = 1.0;

}

ARCMatrix4x4 ARCMatrix4x4::Identity()
{
	return ARCMatrix4x4(IDENTITY);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ARCMatrix4x4::ARCMatrix4x4(const ARCMatrix4x4& _rhs)
{
	memcpy(n, _rhs.n, 16*sizeof(DistanceUnit));
}

ARCMatrix4x4::ARCMatrix4x4(const DistanceUnit* _data)
{
	memcpy(n, _data, 16*sizeof(DistanceUnit));
}

/*
void Transpose();
void Invert();
*/

/*
ARCMatrix4x4& operator=(const ARCMatrix4x4& _rhs);
ARCMatrix4x4& operator+=(const ARCMatrix4x4& _rhs);
ARCMatrix4x4& operator-=(const ARCMatrix4x4& _rhs);
ARCMatrix4x4& operator*=(const ARCMatrix4x4& _rhs);
ARCMatrix4x4& operator/=(const ARCMatrix4x4& _rhs);

DistanceUnit& At(int _r, int _c);
const DistanceUnit& At(int _r, int _c) const;
*/





//friends
ARCVector3 operator*( const ARCMatrix4x4& _lhs, const ARCVector3& _rhs)
{
	// Note that this only works if the last row in _lhs is 0 0 0 1 and we assume _rhs[VW] = 1
	return ARCVector3((_lhs.n[0][0] * _rhs[0]) + (_lhs.n[0][1] * _rhs[1]) + (_lhs.n[0][2] * _rhs[2]) + _lhs.n[0][3],
					  (_lhs.n[1][0] * _rhs[0]) + (_lhs.n[1][1] * _rhs[1]) + (_lhs.n[1][2] * _rhs[2]) + _lhs.n[1][3],
					  (_lhs.n[2][0] * _rhs[0]) + (_lhs.n[2][1] * _rhs[1]) + (_lhs.n[2][2] * _rhs[2]) + _lhs.n[2][3]);
}