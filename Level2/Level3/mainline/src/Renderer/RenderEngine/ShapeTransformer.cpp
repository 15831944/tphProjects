#include "StdAfx.h"
#include ".\shapetransformer.h"

#include <Ogre/OgreMatrix4.h>

using namespace Ogre;
//
ShapeTransformer::ShapeTransformer()
{
	SetIdent();
}

void ShapeTransformer::Scale( const ARCVector3& scale )
{
	Matrix4 mat = Matrix4::IDENTITY;
	mat.setScale(OGREVECTOR(scale));
	mTranfMat =  mat * mTranfMat;
}

void ShapeTransformer::Rotate( const ARCVector3& axis, double dangle )
{
	Matrix3 mat;
	mat.FromAngleAxis(OGREVECTOR(axis),Degree(dangle) );	
	mTranfMat = Matrix4(mat) * mTranfMat;
}

void ShapeTransformer::Translate( const ARCVector3& offset )
{
	Matrix4 mat = Matrix4::IDENTITY;
	mat.setTrans(OGREVECTOR(offset));
	mTranfMat = mat*mTranfMat;
}

void ShapeTransformer::SetIdent()
{
	mTranfMat = Matrix4::IDENTITY;
}

