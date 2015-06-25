#pragma once
#include "ARCVector.h"


class Transform2D
{
public:
	double dScale;
	double dRotation; //angle
	ARCVector2 vOffset; //x,y 


public:
	void GetTransform( const ARCVector2& v1, const ARCVector2& v2, const ARCVector2& v1To, const ARCVector2& v2To);
};
