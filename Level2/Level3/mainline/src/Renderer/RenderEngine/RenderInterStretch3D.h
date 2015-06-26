#pragma once

#include "Terminal3DObject.h"

#include <InputAirside/IntersectedStretch.h>

namespace Ogre
{
	class SceneNode;
}

class CRenderInterStretch3DPoint : public C3DSceneNodeObject
{
public:
	typedef boost::shared_ptr<CRenderInterStretch3DPoint> SharedPtr;

	CRenderInterStretch3DPoint(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, IntersectedStretch& point, int mid);
	virtual EnumObjectType GetObjectType() const { return ObjectType_Airside;}

	void Update();
// 	void DrawOGL(C3DView* pView);
	ARCPoint3 GetLocation()const;
// 	virtual SelectType GetSelectType()const;
// 	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
// 	virtual void AfterMove();
	IntersectedStretchPoint& GetIntersectedStretchPt();
	IntersectionNode& GetIntersectionNode();
	DistanceUnit GetDistInObj() const;

protected:
	IntersectedStretch& m_stretch3D;
	int m_index;
};

/************************************************************************/
/*                  CIntersectedStretch3D                               */
/************************************************************************/
class CRenderInterStretch3D : public C3DSceneNodeObject
{

public:
	typedef boost::shared_ptr<CRenderInterStretch3D> SharedPtr;

	CRenderInterStretch3D(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, IntersectedStretch& interStretch);
	virtual EnumObjectType GetObjectType() const { return ObjectType_Airside;}

	void Update();
// 	void DrawSelect(C3DView* pView);
// 	void DrawOGL(C3DView* pView);
// 	void DrawEditPoint(C3DView* pView,bool bSelect = false);
// 	void DrawLine(C3DView* pView,bool bEdit = false);
// 	void RenderOneStencil();
// 	virtual SelectType GetSelectType()const;
	ARCPoint3 GetLocation() const;
	void GenSmoothStretchPath();

public:
	CRenderInterStretch3DPoint::SharedPtr m_fristPoint;
	CRenderInterStretch3DPoint::SharedPtr m_secondPoint;
	IntersectedStretch& m_interStretch;

protected:
	ARCPath3 m_vPath;
};


/************************************************************************/
/*                    CRenderInterStretch3DInAirport                    */
/************************************************************************/

class CRenderInterStretch3DInAirport
{
public:
// 	typedef std::vector<int> Int;
// 	typedef std::vector<IntersectedStretchPoint> PInterSt;

	CRenderInterStretch3DInAirport(CRender3DScene* p3DScene);
// 	void DrawOGL(C3DView* pView);
// 	void DrawSelect(C3DView* pView);
// 	void DrawLine(C3DView* pView);
// 	void DrawLane(C3DView* pView);
// 	void RenderStencil();

// 	bool IsInEdit()const{return m_bInEdit;}
// 	void SetEdit(bool b){ m_bInEdit = b; }
// 	void SetInEditFillet(int id){ m_iInEdit = id; }
// 	CIntersectedStretch3D* GetInEditFillet();

	void Update();

public:
	void Init(int nAirport,const IntersectionNodesInAirport& nodeList);
// 	void FetchPositionOfLane();
	int GetCount()const{ return (int)m_vIntersectedStretch3D.size(); }
	CRenderInterStretch3D* GetStretch3D(int idx)const{ return (CRenderInterStretch3D* )m_vIntersectedStretch3D.at(idx).get(); }
	IntersectedStretchInAirport GetIntersectedStretchInAirport()const { return m_intersectedStretchInAirport; }

	void UpdateIntersectedStretchInAirport(const IntersectionNodesInAirport& nodeList, const std::vector<int>& vChangeNodeIndexs);

protected:
	IntersectedStretchInAirport m_intersectedStretchInAirport;
	std::vector<CRenderInterStretch3D::SharedPtr> m_vIntersectedStretch3D;
// 	std::pair<Int, PInterSt> m_vLane;
	int m_nAirport;
// 	bool m_bInEdit;
// 	int m_iInEdit;

	CRender3DScene* m_p3DScene;
};

