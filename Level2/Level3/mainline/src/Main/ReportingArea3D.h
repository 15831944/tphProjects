#pragma once
#include "altobject3d.h"
#include "../InputAirside/ReportingArea.h"
#include "./ObjectControlPoint2008.h"

class CReportingArea3D :
	public ALTObject3D
{
public:
	CReportingArea3D(int nID);
	~CReportingArea3D(void);

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	ARCPoint3 GetLocation(void) const;

	void RenderLine( C3DView * pView );
	void Render( C3DView * pView );

	CReportingArea * GetReportingArea();

	virtual void DoSync();
	virtual void OnRotate(DistanceUnit dx);

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);

	ALTObjectControlPoint2008List m_vControlPoints;
};
