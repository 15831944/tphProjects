#pragma once
#include "renderscene3dnode.h"
#include "Common/ARCPipe.h"
#include "SceneState.h"


class LandsideLinkStretchControlPoint3D;
class LandsideLinkStretchObject;
class LandsideLinkStretchControlPath3D;
class LandsideCurbSide;

//Pax Pick Area 3D
class LandsideLinkStretchPickArea3D : public CLayoutObject3DNode
{
	typedef	std::vector<LandsideLinkStretchControlPoint3D>				ControlPointList;
	typedef	std::vector<LandsideLinkStretchControlPoint3D>::iterator		ControlPoint_Iter;
public:
	SCENENODE_CONVERSION_DEF(LandsideLinkStretchPickArea3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideLinkStretchPickArea3D)

public:
	void OnUpdate3D(const CPoint2008& ptCenter);
	void SetQueryData(LandsideLinkStretchObject* pLinkStretchObject);
	LandsideLinkStretchObject* getLinkStretchObject();
	//double GetStretchHight();

	void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	void EndDrag();
	bool IsDraggable()const;

	//call when double clicked the area
	void OnUpdateEditMode(BOOL b);

	LandsideLinkStretchControlPath3D GetControlPath3D();
private:
	void UpdateControlPath3D(LandsideLinkStretchObject* pLinkStretchObject, const CPoint2008& center);
};
//Stretch Area in 3D
class LandsideLinkStretchArea3D :
	public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideLinkStretchArea3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideLinkStretchArea3D)
 
protected:

public:
	LandsideLinkStretchObject* getLinkStretchObject();
	void OnUpdate3D();

	//
	void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	void EndDrag();
	bool IsDraggable()const;

	//call when double clicked the area
	void OnUpdateEditMode(BOOL b);

	//get the index of point which was set byCubrsideControlPoint3D::SetQueryData;
	LandsideLinkStretchControlPoint3D GetControlPoint3D(int idx);

	//ARCVector3 getCenter();
	bool UpdateDraw(CPoint2008& outcenter);

	void SetQueryData(LandsideLinkStretchObject* pLinkStretchObject);

	static void GetPartStretch(LandsideLinkStretchObject* pBusStation, const CPoint2008& center, ARCPipe& out);
private:
	void BuildControlPoints(const ARCPipe& );
};

//DecisionLine 3D
class LandsideLinkStretchDecisionLine3D :
	public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideLinkStretchDecisionLine3D, CLayoutObject3DNode)
		PROTOTYPE_DECLARE(LandsideLinkStretchDecisionLine3D)

	LandsideCurbSide* getLandsideCurbSideObject();
	void SetQueryData(LandsideCurbSide* pLandsideCurbSideOject);
	bool OnUpdate3D(CPoint2008& ptCenter);
};


//con
class LandsideLinkStretchControlPath3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideLinkStretchControlPath3D, CRenderScene3DNode)
	PROTOTYPE_DECLARE(LandsideLinkStretchControlPath3D)

public:
	void Update3D(LandsideLinkStretchObject* pLinkStretch,const CPoint2008& center);

	LandsideLinkStretchObject* GetLandsideLinkStretchObject();
	void SetQueryData(LandsideLinkStretchObject*pLinkStretch);

	LandsideLinkStretchControlPoint3D GetControlPoint3D(int idx);

	// add a point to control point list, return its new index
	int AddControlPoint(Vector3 position);
	int RemoveControlPoint(Vector3 position);

	//dynamic
	virtual void OnRButtonDown( const MouseSceneInfo& mouseInfo );
};

class LandsideLinkStretchControlPoint3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideLinkStretchControlPoint3D, CRenderScene3DNode)
	PROTOTYPE_DECLARE(LandsideLinkStretchControlPoint3D)

	enum Type
	{
		CP_DEFAULT,
		CP_PICKAREA,
		CP_STRETCH,
	};

	void Update3D();
	//drag operations
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const{ return true; }

	//	
	void SetQueryData(LandsideLinkStretchObject* pLinkStretchObject, int idx, Type t = CP_DEFAULT);
	int GetControlPointIndex();
	Type GetControlPointType();
	LandsideLinkStretchObject* GetLandsideLinkStretchObject()const;

public:
	Ogre::Vector3  m_lastCrossPoint;

};