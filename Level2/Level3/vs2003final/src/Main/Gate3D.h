#pragma once
#include "altobject3d.h"

#include "./ObjectControlPoint2008.h"

class Stand;
class StandDisplayProp;
class IntersectionNodesInAirport;

class CStand3D :
	public ALTObject3D
{
public:
	CStand3D(int id);
	virtual ~CStand3D(void);

	ARCPoint3 GetLocation(void) const;
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);

	Stand * GetStand();
	//StandDisplayProp * GetDisplayProp();


	virtual void DrawOGL(C3DView * m_pView);
	virtual void DrawSelect(C3DView * pView);

	virtual void DoSync();

	virtual void OnRotate(DistanceUnit dx);

	virtual void FlushChange();

	void SnapTo(const ALTObject3DList& TaxiList);
	void SnapTo( const ALTObject3D* Taxi);

	void RenderEditPoint(C3DView * pView,bool bSel = false);
	void UpdateIntersectionNodes(const IntersectionNodesInAirport& nodelist);
	void RenderLeadInOutLines(C3DView * pView);

	ref_ptr<CALTObjectPointControlPoint2008> m_pServePoint;
	ALTObjectControlPoint2008List m_vInConstPoint;
	ALTObjectControlPoint2008List m_vOutConstPoint;

	std::vector<ALTObjectControlPoint2008List> m_vLeadInLinesPoints;
	std::vector<ALTObjectControlPoint2008List> m_vLeadOutLinesPoints;

protected:
	

};
