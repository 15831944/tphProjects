#pragma once
#include ".\ALTObject3D.h"
#include "./ObjectControlPoint2008.h"

class Surface;
class C3DView;
class CTexture;

class CSurface3D :
	public ALTObject3D
{
public:
	CSurface3D(int nObjID);
	virtual ~CSurface3D(void);

	Surface * GetSurface()const;

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	ARCPoint3 GetLocation(void) const;
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);

	virtual void DoSync();
	

	virtual void FlushChange();

	void RenderSurface(C3DView * pView);

protected:
	CTexture * pTexture;
	ALTObjectControlPoint2008List m_vControlPoints;

};
