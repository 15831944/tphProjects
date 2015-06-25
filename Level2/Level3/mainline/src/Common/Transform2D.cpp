#include "StdAfx.h"
#include ".\transform2d.h"


void Transform2D::GetTransform( const ARCVector2& v1, const ARCVector2& v2, const ARCVector2& v1To, const ARCVector2& v2To )
{
	ARCVector2 vDirOrig = v2 - v1;
	ARCVector2 vDirDest = v2To - v1To;

	dScale = vDirDest.Magnitude() / vDirOrig.Magnitude();
	dRotation = vDirOrig.GetAngleOfTwoVector(vDirDest);

	ARCVector2 v1cpy = v1;
	v1cpy.Rotate(-dRotation);
	vOffset = v1To - v1cpy * dScale;
}