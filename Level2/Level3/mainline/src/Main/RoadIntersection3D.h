#pragma once
#include "altobject3d.h"

class Intersections;

class CRoadIntersection3D :
	public ALTObject3D
{
public:
	CRoadIntersection3D(int nID);
	~CRoadIntersection3D(void);

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	void RenderSurface(C3DView * pView);

	Intersections* GetRoadIntersection(){ return (Intersections* ) GetObject(); }

	ARCPoint3 GetLocation(void) const;	
protected:
	ARCPoint3 m_vCenter;

};
