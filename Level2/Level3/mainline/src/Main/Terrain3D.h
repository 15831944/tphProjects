#pragma once
#include "altobject3d.h"


//the contour tree
class CTerrain3D :
	public ALTObject3D
{
public:
	CTerrain3D(int nRootID);
	virtual ~CTerrain3D(void);
};
