#pragma once
#include "altobject3d.h"

#include "./ObjectControlPoint2008.h"

#include "../Common/line2008.h"
#include "../InputAirside/Taxiway.h"
#include "HoldPosition3D.h"


class TaxiwayDisplayProp;

class TaxiwayHoldPosition3D : public HoldPosition3D
{
public:	
	TaxiwayHoldPosition3D(Taxiway* pTaxiway, int id) : HoldPosition3D(pTaxiway,id){
		
	}

	Taxiway * GetTaxiway(){ 
		return (Taxiway*)m_pObject.get();
	}
	const Taxiway* GetTaxiway()const{ return (Taxiway*)m_pObject.get(); }
	
	virtual const CPath2008&GetPath()const{ return GetTaxiway()->GetPath(); }

	virtual void UpdateLine();
	virtual HoldPosition & GetHoldPosition();

protected:
	
};


class TaxiwayMarking3D : public Selectable
{
public:
	typedef ref_ptr<TaxiwayMarking3D> RefPtr;

	TaxiwayMarking3D (Taxiway* pTaxiway):m_pTaxiway(pTaxiway){}

	virtual SelectType GetSelectType()const { return ALT_OBJECT_MARK; }

	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0) ;

	virtual ARCPoint3 GetLocation()const;

	virtual void DrawSelect(C3DView * pView);

	void Draw(C3DView* pView);

	virtual void AfterMove();

protected:
	Taxiway * m_pTaxiway;
	ARCPoint3 m_pPos;


};

class CFilletTaxiway3DInAirport;
class CFilletPoint3D;


class CTaxiInterruptLine3D
{
public:
	CTaxiInterruptLine3D(int nFrontInterNodeId,double distance,int nBackInterNodeId, int item, CString strName ,int TaxiwayID, CPoint2008 position);
	void Draw(C3DView* pView);
	void DrawSelect(C3DView* pView);
	void MoveLocation(DistanceUnit dx, DistanceUnit dy, DistanceUnit dz=0);
	void FlushChangeOf();
protected:
	IntersectionNode m_FrontIntersectionNode;
	int m_nFrontIntersectionID;
	IntersectionNode m_BackIntersectionNode;
	int m_nBackIntersectionID;
	CString m_strName;
	int m_nItem;
	double m_dDistanceToInterNode;
	int m_TaxiwayID;
	CPoint2008 m_Pt;
};


class CTaxiway3D :
	public ALTObject3D
{
public:
	typedef ref_ptr<CTaxiway3D> RefPtr;
	CTaxiway3D(int id);
	//CTaxiway3D(Taxiway* pObj);
	virtual ~CTaxiway3D(void);

	Taxiway * GetTaxiway()const;	
	//TaxiwayDisplayProp * GetDisplayProp();


	virtual void DrawOGL(C3DView * pView);
	virtual void DrawSelect(C3DView * pView);
	virtual void AddEditPoint(double x, double y, double z);

	ARCPoint3 GetLocation(void) const;
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void OnRotate(DistanceUnit dx);

	void RenderMarkLine(C3DView  *pView);
	void RenderSurface(C3DView  *pView);
	void RenderMarkText(C3DView  *pView);
	void RenderEditPoint(C3DView * pView);
	void RenderHoldPosition(C3DView *pView);
	void RenderHoldName(C3DView* pView);
	void RenderMarkings(C3DView * pView);
	void RenderNodeNames(C3DView  *pView);
	

	//void RenderIntersectNodes(C3DView * pView);

	virtual void DoSync();

	void UpdateSync(){SyncDateType::Update();};

	virtual void FlushChange();

	void SnapTo(const ALTObject3DList& RwOrTaxiList);
	//void SnapTo( const ALTObject3D* RwOrTaxi);

	void UpdateIntersectionNodes(const IntersectionNodesInAirport& nodelist);
	void UpdateHoldPositions(const ALTAirport& airport, const ALTObject3DList&vRunwayList, const FilletTaxiwaysInAirport& vFillets, const IntersectionNodesInAirport& nodelist );
	void SetRelatFillets(const CFilletTaxiway3DInAirport& vFillets);
	void RefreshHoldPosition3D();

protected:

	std::vector<IntersectionNode> m_vIntersectionNodes;

	ALTObjectControlPoint2008List m_vControlPoints;
	
	HoldPosition3DList m_vAllHoldPositions3D;

	TaxiwayMarking3D::RefPtr m_pTaxiwayMark;
	
	std::vector<CFilletPoint3D*> m_vFilletPoints;

	CPath2008 m_renderPath;
protected:
	bool m_bautoSnap;
	int m_TaxiwayID;
	std::vector<CTaxiInterruptLine3D* > CHoldShortLine3DList;
};


