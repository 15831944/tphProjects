#pragma once

#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "Common\Point2008.h"
class LandsideStretch;
class LaneNode;
class StretchSideInfo;
//////////////////////////////////////////////////////////////////////////
class LandsideControlPoint3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideControlPoint3D, CRenderScene3DNode)
	PROTOTYPE_DECLARE(LandsideControlPoint3D)

	virtual	void Update3D();
	//mouse move
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const;
	//
	virtual void OnLButtonDblClk( const MouseSceneInfo& mouseinfo );
	bool IsStraightPoint()const;


	LandsideFacilityLayoutObject* getLayoutObject()const;
	int getPointIndex()const;

	void SetQueryData(LandsideFacilityLayoutObject* pobj, int idx);
protected:

};
//////////////////////////////////////////////////////////////////////////
class ILandsideEditContext;
class CPath2008;
class CPoint2008;
class ARCPipe;
class ARCPath3;

class LandsideObjectControlPath3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideObjectControlPath3D, CRenderScene3DNode)
	PROTOTYPE_DECLARE(LandsideObjectControlPath3D)

	void Init(LandsideFacilityLayoutObject* pStretch);
	
	void Update3D(LandsideFacilityLayoutObject* pStreth,ILandsideEditContext* pCtx,const CPoint2008& ptCenter, const CPath2008& path);

	LandsideControlPoint3D GetControlPoint3D(int idx);

	virtual void OnRButtonDown( const MouseSceneInfo& mouseInfo );

	LandsideFacilityLayoutObject* getLayoutObject();

protected:
	void SetQueryData(LandsideFacilityLayoutObject* pStreth);

};

//utile functions to update stretches
class LandsideStretch3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideStretch3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideStretch3D)

	//edit mode 
	virtual	void OnUpdate3D();
	virtual void OnUpdateEditMode(BOOL b);
	virtual void OnUpdateBePickMode(IRenderSceneMouseState* pState);

	LandsideObjectControlPath3D GetControlPath3D();

//help function
	double GetMousePosInStretch(const MouseSceneInfo& mouseInfo);
	double GetMousePosIndexInStretchControlPath(const MouseSceneInfo& mouseInfo);
	LandsideStretch* getStretch();

	static void GetPipeMidPath(const ARCPipe& pipe, ARCPath3& path, double dDist );

	static CString sMarkLaneMat[];

protected:
	void SetQueryData(LandsideStretch* pStretch);

	void ShowLaneNode3D();
	void ShowStretchSideNode3D();
public://util functions
	//static bool GetLaneEnd(const LaneNode& lanenode, StretchSideInfo& end);

	static double ls_dDefaultHighwayBaseThick;
	static double ls_dDefaultStreetBaseThick;
	static double ls_dDefaultHighwaySideWidth;
	static double ls_dDefaultHighwaySideHeight;
	static double ls_dDefaultStreetSideHeight;

	static Ogre::String& GetSideMat(BOOL bHightWay, BOOL bEdit);

	static Ogre::String mat_highwayside;
	static Ogre::String mat_streetwalkside;

	static Ogre::String mat_highwayside_blend;
	static Ogre::String mat_hightwayFoot;
	static Ogre::String mat_hightwayFoot_blend ;
	static Ogre::String mat_streetwalkside_blend ;

};

//////////////////////////////////////////////////////////////////////////
class LaneLinkageNode3D : public CRenderScene3DNode
{
public: 
	SCENENODE_CONVERSION_DEF(LaneLinkageNode3D, CRenderScene3DNode)	
	PROTOTYPE_DECLARE(LaneLinkageNode3D)

	bool getNodeData(LaneNode& node);
	void setNodeData(LaneNode& node);	
	void Update3D(LaneNode& node);
};


//////////////////////////////////////////////////////////////////////////
//draw  object to help select stretch side
class StretchSideNode3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(StretchSideNode3D, CRenderScene3DNode)

	bool getNodeData(StretchSideInfo& side);
	void setNodeData(const StretchSideInfo& side);
	
	void Update3D(StretchSideInfo& side);

	
};

