#pragma once
#include "altobject3d.h"
#include "../InputAirside/VehiclePoolParking.h"
#include "./ObjectControlPoint2008.h"

class CVehiclePoolParking3D :
	public ALTObject3D
{
public:
	CVehiclePoolParking3D(int nID);
	~CVehiclePoolParking3D(void);

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	ARCPoint3 GetLocation(void) const;
	
	void RenderEditPoint( C3DView * pView );

	VehiclePoolParking * GetVehiclePoolParking();

	virtual void DoSync();
	virtual void OnRotate(DistanceUnit dx);

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);

	ALTObjectControlPoint2008List m_vControlPoints;
};
