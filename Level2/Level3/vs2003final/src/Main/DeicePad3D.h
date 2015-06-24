#pragma once
#include "altobject3d.h"
#include "./ObjectControlPoint2008.h"


class DeicePad;
class DeicePadDisplayProp;

class CDeicePad3D :
	public ALTObject3D
{
public:
	CDeicePad3D(int id);
	virtual ~CDeicePad3D(void);
	DeicePad * GetDeicePad();
	//DeicePadDisplayProp * GetDisplayProp();

	virtual void DrawOGL(C3DView * m_pView);

	virtual void DrawSelect(C3DView * pView);

	ARCPoint3 GetLocation(void) const;

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz= 0);

	virtual void DoSync();

	virtual void FlushChange();

	virtual void OnRotate(DistanceUnit dx);


	void RenderSurface(C3DView * pView);
	void RenderStand(C3DView * pView);
	void RenderInOutConstrain(C3DView * pView);
	void RenderTruck(C3DView * pView);
	void RenderEditPoint(C3DView * pView);
	//void UpdateIntersectNodes(const ALTObject3DList& otherObjList);

	void SnapTo(const ALTObject3DList& TaxiList);
	void SnapTo( const ALTObject3D* Taxi);

	ref_ptr<CALTObjectPointControlPoint2008> m_pServePoint;
	ALTObjectControlPoint2008List m_vInConstPoint;
	ALTObjectControlPoint2008List m_vOutConstPoint;

protected:
};
