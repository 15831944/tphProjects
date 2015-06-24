#pragma once
#include "altobject3d.h"

class AirSector;

class CAirSector3D :
	public ALTObject3D
{
public:
	CAirSector3D(int id);
	virtual ~CAirSector3D(void);

	virtual ARCPoint3 GetLocation(void) const;

	AirSector * GetSector()const;

	virtual void DrawOGL(C3DView * m_pView);
	virtual void DrawSelect(C3DView * pView);

	virtual void Render(C3DView * pView);
	virtual void RenderShadow(C3DView * pView);

	void FlushChange();

	virtual void Update();

	void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */);
};
