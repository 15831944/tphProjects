#pragma once

#include "RenderScene3DNode.h"
class LandsideIntersectionNode;
#include "LandsideStretch3D.h"

//#include "CommonData/3DViewCommon.h"
//#include "SceneState.h"

using namespace Ogre;

class IntersectionLaneLinkage3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(IntersectionLaneLinkage3D, CRenderScene3DNode)
	PROTOTYPE_DECLARE(IntersectionLaneLinkage3D);
	//
	void Update3D(LandsideIntersectionNode* pNode, int iLinkIdx,const CPoint2008& ptC);

	LandsideIntersectionNode* GetNode() const;
	int GetIndex() const;
protected:

	void SetQueryData( LandsideIntersectionNode* pNode, int iLinkIdx );

};

//////////////////////////////////////////////////////////////////////////
class StretchPortList;
class LandIntersection3D :  public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandIntersection3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandIntersection3D)

	virtual void OnUpdate3D();
// 	void BuildPole(ManualObject* pObj,CPoint2008 stretchLeftPos,CPoint2008 stretchRightPos,CPoint2008 dir);
	virtual void OnUpdateEditMode(BOOL b);


	static void DrawLinkNodes(const StretchPortList& portlist, Ogre::ManualObject* pObj,bool bStreet, BOOL bInEdit );

	virtual bool IsDraggable()const{ return false; }
protected:
	LandsideIntersectionNode* getIntersection();
	void UpdateLinkages3D(const CPoint2008& ptC);
	IntersectionLaneLinkage3D GetLinkage3D(int idx);
	void UpdateTrafficLight3D(CPoint2008 ptC);
};

//////////////////////////////////////////////////////////////////////////
