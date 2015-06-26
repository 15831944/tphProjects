#pragma once
#include "3DNodeObject.h"

class IRenderSceneMouseState;
class SceneNodeQueryData;
////////////////////////////root node//////////////////////////////////////////////
class CRender3DScene;
class CRenderSceneRootNode : public C3DNodeObject
{
public:
	SCENENODE_CONVERSION_DEF(CRenderSceneRootNode,C3DNodeObject)
	CRender3DScene* getRenderScene();
	void setRenderScene(CRender3DScene* pQuerier);		
};


//////////////////////////////////////////////////////////////////////////
#define PROTOTYPE_DECLARE(TCLASS)  static TCLASS ProType;\
								   virtual CRenderScene3DNode* getProType()const{ return &ProType; }
#define PROTYPE_INSTANCE(TCLASS) TCLASS TCLASS::ProType;


class MouseSceneInfo;
class IEnvModeEditContext;
class ILandsideEditContext;
class IAirsideEditContext;
class ITerminalEditContext;
enum EnvironmentMode;

class RENDERENGINE_API CRenderScene3DNode : public C3DNodeObject
{
public:
	SCENENODE_CONVERSION_DEF(CRenderScene3DNode,C3DNodeObject)
	PROTOTYPE_DECLARE(CRenderScene3DNode);


	//get render scene
	CRender3DScene* getRenderScene()const;

	//query data 
	SceneNodeQueryData* getQueryData()const;

	//drag operations
	BOOL IsInDragging()const;
	void SetInDragging(BOOL b);
	//dynamic
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const;
	//
public://mouse operations
	virtual void OnRButtonDown( const MouseSceneInfo& mouseInfo );
	virtual void OnLButtonDblClk( const MouseSceneInfo& mouseinfo );
	virtual void OnMouseMove(const MouseSceneInfo& mouseinfo);

	//edit mode
	IEnvModeEditContext* GetEditContext(EnvironmentMode mode)const;

	ILandsideEditContext* GetLandsideEditContext()const;
	ITerminalEditContext* GetTerminalEditContext()const;
	IAirsideEditContext* GetAirsideEditContext()const;
public:
	void FlipEditMode();
	BOOL IsInEditMode()const;
	void SetEditMode(BOOL b);
public:
	virtual void OnUpdateEditMode(BOOL b);	
	virtual void OnUpdate3D();
	//selected state
public:
	BOOL IsSelected()const;
	void Select(BOOL b);
protected:
	virtual void OnUpdateSelect(BOOL b);
	void SetSelected(BOOL b);

	//be pick state
public:
	IRenderSceneMouseState* GetBePickMode()const;
	void ChangeToPicMode(IRenderSceneMouseState* pState);
protected:
	virtual void OnUpdateBePickMode(IRenderSceneMouseState* pState);
	void SetBePickMode(IRenderSceneMouseState* pState);

	//create or get the node with the name in the scene
	CRenderScene3DNode CreateGetNameNode(CString sName)const;
	//dynamic call function
public:
	CRenderScene3DNode* DynamicCast()const;	
	bool IsRealType()const;
	CRenderScene3DNode* getDynamicProtype()const;
protected:
	//init node data once, force will all
	SceneNodeQueryData* InitQueryData(bool bForceInit = false);
	virtual bool IsValid()const;
};

//////////////////////////////////////////////////////////////////////////
class LandsideFacilityLayoutObject;
class ILandsideEditContext;

class  CLayoutObject3DNode : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(CLayoutObject3DNode, CRenderScene3DNode)
	PROTOTYPE_DECLARE(CLayoutObject3DNode);

	virtual void Setup3D(LandsideFacilityLayoutObject *pObj);

	LandsideFacilityLayoutObject* getLayOutObject()const;
	virtual void OnLButtonDblClk( const MouseSceneInfo& mouseinfo );


	virtual bool IsLocked()const;
	virtual bool IsDraggable()const;
protected:
	virtual void SetQueryData(LandsideFacilityLayoutObject* pObj);

	void OnUpdateEditMode(BOOL b);

	//dynamic drag
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();

};


