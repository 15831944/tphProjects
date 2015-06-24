#pragma once
#include "altobject3d.h"
#include "../InputAirside/Heliport.h"
#include "..\Common\ARCVector.h"
#include "..\Common\ARCPath.h"
#include "./ObjectControlPoint2008.h"


class CHeliport3D :
	public ALTObject3D
{
public:
	CHeliport3D(int id);
	virtual ~CHeliport3D(void);

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	Heliport* GetHeliport()const;

	virtual void FlushChange();

	void RenderBase(C3DView * pView);

	void RotateVector(float angle, float x, float y, float z, ARCVector3& m_vView);

	ARCPoint3 GetLocation( void ) const;

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */);

	virtual void OnRotate(DistanceUnit dx);

	virtual void DoSync();

	void RenderEditPoint(C3DView* pView, bool bSelectMode = false);

protected:
	ALTObjectControlPoint2008List m_vControlPoints;

private:
	ARCVector3 m_vPosition;


};
