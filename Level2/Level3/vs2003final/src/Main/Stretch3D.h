#pragma once
#include "altobject3d.h"
#include "../InputAirside/Stretch.h"
#include "./ObjectControlPoint2008.h"
#include "IntersectedStretch3D.h"
#include "./AirsideThickness3D.h"
#include "../Common/line2008.h"


//class CIntersectedStretch3DInAirport;
class CIntersectedStretch3DPoint;
class ARCPipe;

class CStretch3D :
	public ALTObject3D
{
public:
	CStretch3D(int nID);
	~CStretch3D(void);

	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);

	ARCPoint3 GetLocation(void) const;

	void RenderBaseWithSideLine(C3DView * pView);
	void RenderBase(C3DView * pView);
	void RenderMarkings(C3DView * pView);
	void RenderEditPoint( C3DView * pView );

    void SnapTo(const ALTObject3DList& RwOrTaxiList);
	void GetCullPath(CPath2008& path, Stretch* pStretch);
	Stretch * GetStretch();	
	virtual void DoSync();

    void SetRelatFillets( const CIntersectedStretch3DInAirport& vStretchs );
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void AddEditPoint(double x, double y, double z);
	virtual void FlushChange();
	void UpdateSync(){SyncDateType::Update();};

	void RenderOneStencil();
	void RenderEditPointText();
	ALTObjectControlPoint2008List m_vControlPoints;
	CAirsideThickness3D airsideThick;
protected:
	std::vector<CIntersectedStretch3DPoint*> m_vStretchPoints;
	CIntersectedStretch3DInAirport stetchInAirport;

	CPath2008 m_path;
	//C3DView * m_pView;
	//int m_iScale;
};
