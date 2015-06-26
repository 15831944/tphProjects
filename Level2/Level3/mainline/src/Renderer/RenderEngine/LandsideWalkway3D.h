#ifndef __LandsideWalkway3D_H__
#define __LandsideWalkway3D_H__

#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "SceneState.h"
#include "../../Landside/LandsideWalkway.h"


//////////////////////////////////////////////////////////////////////////
class WalkwayRibPath3D;
class WalkwayControlPath3D;
class LandsideWalkway3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideWalkway3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideWalkway3D)

	virtual void			OnUpdate3D();

	//dynamic drag
	virtual void			StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void			OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void			EndDrag();
	LandsideWalkway*		getWalkway();

	WalkwayRibPath3D		GetRibPath3D(int idx);
	void					UpdateRibPath3D(LandsideWalkway* pWalkway);

	WalkwayControlPath3D	GetControlPath3D();
	void					UpdateControlPath3D(LandsideWalkway* pWalkway,  CPoint2008& center);
};
//////////////////////////////////////////////////////////////////////////
enum emLandsideItemType;
class Walkway3DItem : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(Walkway3DItem, CRenderScene3DNode)
	LandsideWalkway*			getWalkway();
	virtual emLandsideItemType	getType() = 0;
	void						OnUpdateEditMode(BOOL b);
	//void						OnRButtonDown(const MouseSceneInfo& mouseInfo);
	virtual void				SetQueryData(LandsideWalkway* pWalkway, int ipath, int ipoint);
	//dynamic drag
	void 						StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	void 						EndDrag();
	bool 						IsDraggable()const;
};
//////////////////////////////////////////////////////////////////////////
class WalkwayRibPoint3D;
class WalkwayRibPath3D : public Walkway3DItem
{
public:
	SCENENODE_CONVERSION_DEF(WalkwayRibPath3D, Walkway3DItem)
	PROTOTYPE_DECLARE(WalkwayRibPath3D)

	emLandsideItemType	getType();
	WalkwayRibPoint3D   GetRibPoint3D(int ipath, int ipoint);
	int					GetRibPathIndex();

	void				Update3D(LandsideWalkway* pWalkway, int idx, const CPoint2008& leftPt,const CPoint2008& rightPt);
	void				OnRButtonDown(const MouseSceneInfo& mouseInfo);

	//dynamic drag
	void 				OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
};
//////////////////////////////////////////////////////////////////////////
class WalkwayRibPoint3D : public Walkway3DItem
{
public:
	SCENENODE_CONVERSION_DEF(WalkwayRibPoint3D, Walkway3DItem)
	PROTOTYPE_DECLARE(WalkwayRibPoint3D)

	emLandsideItemType	getType();
	int				GetPathAndPointIndex(int& ipath, int&ipoint);
	void				Update3D(LandsideWalkway* pWalkway, int ipath, int ipoint);
	//dynamic drag
	void 				OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
};
//////////////////////////////////////////////////////////////////////////
class WalkwayControlPoint3D;
class WalkwayControlPath3D : public Walkway3DItem
{
public:
	SCENENODE_CONVERSION_DEF(WalkwayControlPath3D, Walkway3DItem)
	PROTOTYPE_DECLARE(WalkwayControlPath3D)

	emLandsideItemType		getType();
	WalkwayControlPoint3D	GetControlPoint(int ipoint);

	void					Update3D(LandsideWalkway* pWalkway, const CPath2008& path);
	void					OnRButtonDown(const MouseSceneInfo& mouseInfo);
};
//////////////////////////////////////////////////////////////////////////
class WalkwayControlPoint3D : public Walkway3DItem
{
public:
	SCENENODE_CONVERSION_DEF(WalkwayControlPoint3D, Walkway3DItem)
	PROTOTYPE_DECLARE(WalkwayControlPoint3D)

	emLandsideItemType	getType();
	void				SetQueryData(LandsideWalkway* pWalkway, int ipoint);
	int					GetPointIndex();

	void				Update3D(LandsideWalkway* pWalkway,int ipoint);
	//dynamic drag
	void 				OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
};
//////////////////////////////////////////////////////////////////////////

#endif