#pragma once
#include "3DNodeObject.h"


#include <Common/ARCPath.h>
#include <InputAirside/FiletTaxiway.h>

#include <boost/shared_ptr.hpp>

#include "Terminal3DObject.h"

namespace Ogre
{
	class SceneNode;
}

class CRenderFilletPoint3D : public C3DSceneNodeObject
{
public:
	typedef boost::shared_ptr<CRenderFilletPoint3D> SharedPtr;
	typedef std::vector<SharedPtr>                  List;
	typedef std::vector<CRenderFilletPoint3D*>      PtrList;

	CRenderFilletPoint3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, FilletTaxiway& filletTaxiway, int idx );

	virtual EnumObjectType GetObjectType() const { return ObjectType_Airside;}

	ARCPoint3 GetLocation() const;
// 	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
// 	virtual void AfterMove();

	void Update();

	FilletPoint& GetFilletPt();

	DistanceUnit GetDistInObj()const;
	IntersectionNode& GetIntersectionNode();

protected:
	FilletTaxiway& m_filletTx;
	int m_idx;

};


class CRenderTaxiwayFillet3D : public C3DSceneNodeObject
{
public:
	typedef boost::shared_ptr<CRenderTaxiwayFillet3D> SharedPtr;
	typedef std::vector<SharedPtr>                    List;
	typedef std::vector<CRenderTaxiwayFillet3D*>      PtrList;

	CRenderTaxiwayFillet3D(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, FilletTaxiway& filetTaxiway);

	virtual EnumObjectType GetObjectType() const { return ObjectType_Airside;}

public:

	ARCPoint3 GetLocation()const;

	void Update() ;

// 	void RenderLine(C3DView * pView,bool bEdit );
// 	void RenderSurface( C3DView * pView,bool bEdit);
// 	void RenderEditPoint(C3DView * pView, bool bSelect);

// 	void DrawSelect(C3DView *pView);

// 	bool IsInEdit()const{ return m_bInEditMode; }
// 	void SetEdit(bool b){ m_bInEditMode = b; }



public: 
	CRenderFilletPoint3D::SharedPtr m_filetPt1;
	CRenderFilletPoint3D::SharedPtr m_filetPt2;

	FilletTaxiway& m_filetTaxway;
// 	bool m_bInEditMode;

protected:
	void GenSmoothPath();
	ARCPath3 m_vPath;

};


class CRenderFilletTaxiway3DInAirport
{
public:
	CRenderFilletTaxiway3DInAirport(CRender3DScene* p3DScene)
		: m_p3DScene(p3DScene)
	{

	}

	void Init(int nARPID, const IntersectionNodesInAirport& nodeList);

	void UpdateFillets(const IntersectionNodesInAirport& nodelist,const std::vector<int>& vChangeNodeIndexs );

// 	void RenderSurface(C3DView * pView);
// 	void RenderLine(C3DView * pView, bool bSelect);

	void Update();

	CRenderTaxiwayFillet3D* FindFillet3D(const FilletTaxiway& filet);

// 	bool IsInEdit()const{return m_bInEdit;}
// 	void SetEdit(bool b){ m_bInEdit = b; }
// 	void SetInEditFillet(int id){ m_iInEditFillet = id; }
// 	CFilletTaxiway3D * GetInEditFillet();

	int GetCount()const{ return (int)m_vFillTaxiway3D.size(); }
	CRenderTaxiwayFillet3D* GetFillet3D(int idx)const{ return (CRenderTaxiwayFillet3D* )m_vFillTaxiway3D.at(idx).get(); }

	FilletTaxiwaysInAirport m_vFilletTaxiways;

protected:
	std::vector<CRenderTaxiwayFillet3D::SharedPtr> m_vFillTaxiway3D;
	int m_nARPID;

// 	int m_iInEditFillet;
// 	bool m_bInEdit;

	CRender3DScene* m_p3DScene;
};
