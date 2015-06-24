#pragma once
#include "altobject3d.h"
#include "../InputAirside/CircleStretch.h"
#include "./ObjectControlPoint2008.h"
#include "./AirsideThickness3D.h"
#include "../Common/line2008.h"

class CCircleStretch3D :
	public ALTObject3D
{
public:
	CCircleStretch3D(int nID);
	~CCircleStretch3D(void);

	void RenderBaseWithSideLine(C3DView * pView);
	void RenderBase(C3DView * pView);
	void RenderMarkings(C3DView * pView);
	void RenderEditPoint( C3DView * pView );
	CircleStretch * GetCircleStretch();	

	virtual void DoSync();
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void AddEditPoint(double x, double y, double z);
	virtual void FlushChange();
	virtual void DrawSelect(C3DView * pView);

	void UpdateSync(){SyncDateType::Update();};
	void RenderEditPointText();
	ARCPoint3 GetLocation( void ) const;

public:
	ALTObjectControlPoint2008List m_vControlPoints;
	CAirsideThickness3D airsideThick;
protected:
	CPath2008 m_path;
};
