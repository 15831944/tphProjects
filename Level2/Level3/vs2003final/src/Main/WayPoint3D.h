#pragma once
#include "altobject3d.h"


class AirWayPoint;
class AirWayPointDisplayProp;
class CAirside3D;


class CWayPoint3D :
	public ALTObject3D
{
public:
	CWayPoint3D(int id);
	virtual ~CWayPoint3D(void);

	AirWayPoint * GetAirWayPoint()const;
	//AirWayPointDisplayProp * GetDisplayProp();

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz =0);

	virtual void DrawOGL(C3DView * m_pView);
	virtual void DrawSelect(C3DView * pView);

	void RenderShape(C3DView * pView);

	ARCPoint3 GetLocation(void) const;

	void Update(CAirside3D* pAirside);
public:
	
	CAirside3D * m_pAirside;

};
