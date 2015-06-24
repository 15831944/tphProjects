#pragma once
#include "../Common/ARCPath.h"
#include "altobject3d.h"


class AirHold; 
class AirHoldDisplayProp;
class CAirside3D;

class CHold3D :
	public ALTObject3D
{
public:
	CHold3D(int id);
	virtual ~CHold3D(void);
	
	AirHold * GetHold()const;
	//AirHoldDisplayProp * GetDisplayProp();

	virtual ARCPoint3 GetLocation(void) const;

	virtual void DrawOGL(C3DView * m_pView);
	virtual void DrawSelect(C3DView * pView);

	virtual void RenderShape(C3DView  * pView);
	virtual void RenderShadow(C3DView * pView);
	virtual void RenderLines(C3DView * pView);

	virtual void DoSync();

	void Update(CAirside3D* pAirside);


	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */);
	virtual void OnRotate(DistanceUnit dx);

	virtual void FlushChange();


protected:

	CAirside3D * m_pAirside;

	Path m_RoundPath;
	ref_ptr<ALTObject3D> m_pWayPoint;
};
