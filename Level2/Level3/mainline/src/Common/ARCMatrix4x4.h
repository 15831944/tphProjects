// ARCMatrix4x4.h: interface for the ARCMatrix4x4 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ARCMATRIX4X4__H
#define ARCMATRIX4X4__H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ARCMathCommon.h"
#include "ARCVector.h"

class ARCMatrix4x4  
{
//statics
public:
	static ARCMatrix4x4 GetTranslate3D(DistanceUnit _tx, DistanceUnit _ty, DistanceUnit _tz);
	static void GetTranslate3D(DistanceUnit _tx, DistanceUnit _ty, DistanceUnit _tz, ARCMatrix4x4& _m);
	static ARCMatrix4x4 GetTranslate3D(const ARCVector3 _tv);
	static void GetTranslate3D(const ARCVector3 _tv, ARCMatrix4x4& _m);

	static ARCMatrix4x4 GetRotate3D(short _axis, DistanceUnit _angle);
	static void GetRotate3D(short _axis, DistanceUnit _angle, ARCMatrix4x4& _m);
	static ARCMatrix4x4 GetRotate3D(const ARCVector3& _axis, DistanceUnit _angle);
	static void GetRotate3D(const ARCVector3& _axis, DistanceUnit _angle, ARCMatrix4x4& _m);

	static ARCMatrix4x4 Identity();

protected:
	DistanceUnit n[4][4];
	ARCMatrix4x4(const DistanceUnit* _data);
	
public:
	ARCMatrix4x4() {}
	ARCMatrix4x4(const ARCMatrix4x4& _rhs);

	~ARCMatrix4x4() {}

	//void Transpose();
	//void Invert();
	void SetZero();
	void SetIdentity();

	//ARCMatrix4x4& operator=(const ARCMatrix4x4& _rhs);
	//ARCMatrix4x4& operator+=(const ARCMatrix4x4& _rhs);
	//ARCMatrix4x4& operator-=(const ARCMatrix4x4& _rhs);
	//ARCMatrix4x4& operator*=(const ARCMatrix4x4& _rhs);
	//ARCMatrix4x4& operator/=(const ARCMatrix4x4& _rhs);

	//DistanceUnit& At(int _r, int _c);
	//const DistanceUnit& At(int _r, int _c) const;
	
	friend ARCVector3 operator*( const ARCMatrix4x4& _lhs, const ARCVector3& _rhs);
	

};

inline
void ARCMatrix4x4::SetZero()
{
	memset(n, 0, 16*sizeof(DistanceUnit));
}

inline
void ARCMatrix4x4::SetIdentity()
{
	SetZero();
	for(int i=0; i<4; i++) 
		n[i][i] = 1.0;
}

#endif // !defined(ARCMATRIX4X4__H)
