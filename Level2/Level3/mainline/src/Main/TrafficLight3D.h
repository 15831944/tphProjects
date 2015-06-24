#pragma once
#include "altobject3d.h"
#include "../InputAirside/TrafficLight.h"
#include "./ObjectControlPoint2008.h"
#include "3dmodel.h"
#include "../InputAirside/IntersectItem.h"

class CTrafficLight3D :
	public ALTObject3D
{
public:
	CTrafficLight3D(int nID);
	~CTrafficLight3D(void);

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	ARCPoint3 GetLocation(void) const;
	ARCPoint3 GetSubLocation(int index) const;
	
	void RenderEditPoint( C3DView * pView );
	void CalculateRotation();

	TrafficLight * GetTrafficLight();

	virtual void DoSync();

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void OnRotate(DistanceUnit dx);

	ALTObjectControlPoint2008List m_vControlPoints;
protected:
	C3dModel*	m_pModel;
	C3dModel*   m_pModel2D;
	StretchIntersectItem  stretchItem;
	std::vector<double> m_vDegree;
};
