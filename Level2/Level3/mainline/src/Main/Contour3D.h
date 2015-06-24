#pragma once
#include "altobject3d.h"
#include "../InputAirside/contour.h"
//#include "./ObjectControlPoint.h"
#include "./ObjectControlPoint2008.h"

class CContour3D :
	public ALTObject3D
{
public:
	CContour3D(int nID);	
	~CContour3D(void);

	Contour * GetContour()const;

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	ARCPoint3 GetLocation(void) const;
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);

	virtual void DoSync();


	virtual void FlushChange();

	void GenAllTriBlock(Contour* m_pFather,const DistanceUnit& dAptAlt);

	void RenderSurface(C3DView * pView);
	void RenderOutLine(C3DView * pView);
	void RenderEditPoint( C3DView * pView );
	void RenderContourOutLine(Contour* pContour);
protected:
	ALTObjectControlPoint2008List m_vControlPoints;
	double min, max;
};


class CTerrian3D
{
public:
	

};