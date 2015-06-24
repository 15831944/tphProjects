#pragma once
#include "altobject3d.h"
#include "../InputAirside/StopSign.h"
#include "./ObjectControlPoint2008.h"
#include "3dmodel.h"

class CStopSign3D :
	public ALTObject3D
{
public:
	CStopSign3D(int nID);
	~CStopSign3D(void);

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	ARCPoint3 GetLocation(void) const;

	void RenderEditPoint( C3DView * pView );

	StopSign * GetStopSign();

	virtual void DoSync();

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void OnRotate(DistanceUnit dx);

	ALTObjectControlPoint2008List m_vControlPoints;
protected:
	C3dModel*	m_pModel;
	C3dModel*   m_pModel2D;
};
