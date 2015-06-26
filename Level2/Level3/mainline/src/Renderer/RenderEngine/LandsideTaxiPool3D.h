#pragma once

#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "SceneState.h"
#include "../../Landside/LandsideTaxiPool.h"
#include "3DNodeObject.h"

class LandsideObjectControlPath3D;
class LandsideTaxiPool3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideTaxiPool3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideTaxiPool3D)

	virtual void	OnUpdate3D();
	LandsideTaxiPool*		getTaxiPool();
	void SetQueryData(LandsideTaxiPool* pTaxiPool);
	LandsideObjectControlPath3D GetControlPath3D();
	bool _GetEntrySideList(StretchPortList& spanMap);
	bool _GetExitSideList(StretchPortList& spanMap);

};

enum emLandsideItemType;
class LandsideTaxiPool3DItem : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideTaxiPool3DItem, CRenderScene3DNode)
	LandsideTaxiPool* getPool()const;
	virtual emLandsideItemType getType()const=0;
};

class TaxiPoolArea3DNode:public LandsideTaxiPool3DItem
{
public:
	SCENENODE_CONVERSION_DEF(TaxiPoolArea3DNode, LandsideTaxiPool3DItem)
	PROTOTYPE_DECLARE(TaxiPoolArea3DNode)
	emLandsideItemType getType()const;
	virtual void OnUpdate3D();
	virtual void OnUpdateEditMode(BOOL b);
	void OnLButtonDblClk( const MouseSceneInfo& mouseinfo );

	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const;

	void Build3D(LandsideTaxiPool* pTaxiPool,CPoint2008& ptCenter);
	void SetQueryData(LandsideTaxiPool* pTaxiPool);
};

class TaxiPoolControlPath3D:public LandsideTaxiPool3DItem
{
public:
	SCENENODE_CONVERSION_DEF(TaxiPoolControlPath3D, LandsideTaxiPool3DItem)
	//PROTOTYPE_DECLARE(TaxiPoolControlPath3D)
	virtual CRenderScene3DNode GetControlPoint3D(int idx);
	virtual CPath2008& getPath(LandsideTaxiPool* pTaxiPool);
	virtual void Update3D(LandsideTaxiPool* pTaxiPool,CPath2008& path, CPoint2008& ptCenter);
	virtual void UpdatePoints(LandsideTaxiPool* pTaxiPool,CPath2008& path)=0;
};

class TaxiPoolAreaControlPath3D:public TaxiPoolControlPath3D
{
public:
	SCENENODE_CONVERSION_DEF(TaxiPoolAreaControlPath3D, TaxiPoolControlPath3D)
	PROTOTYPE_DECLARE(TaxiPoolAreaControlPath3D)	
	emLandsideItemType getType()const;	
	virtual void UpdatePoints(LandsideTaxiPool* pTaxiPool, CPath2008& path);
	void OnRButtonDown( const MouseSceneInfo& mouseInfo );
	void Update3D(LandsideTaxiPool* pTaxiPool, CPath2008& path, CPoint2008& ptCenter);
};

class TaxiPoolControlPoint3D:public LandsideTaxiPool3DItem
{
public:
	SCENENODE_CONVERSION_DEF(TaxiPoolControlPoint3D, LandsideTaxiPool3DItem)
	//PROTOTYPE_DECLARE(TaxiPoolControlPoint3D)
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void EndDrag();
	virtual void OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual bool IsDraggable()const;

	int getPointIndex()const;
	virtual CPath2008& getPath(LandsideTaxiPool* pTaxiPool);	
	void Update3D(LandsideTaxiPool* pTaxiPool,int iPtIdx,CPath2008& path);
};

class TaxiPoolAreaControlPoint3D:public TaxiPoolControlPoint3D
{
public:
	SCENENODE_CONVERSION_DEF(TaxiPoolAreaControlPoint3D, TaxiPoolControlPoint3D)
	PROTOTYPE_DECLARE(TaxiPoolAreaControlPoint3D)
	emLandsideItemType getType()const;
};