#pragma once
#include<vector>
#include <common/ListenerCommon.h>
#include <Renderer/RenderEngine/3DScene.h>
#include <Renderer/RenderEngine/3DNodeObject.h>
#include <Renderer/RenderEngine/Grid3D.h>
#include <Renderer/RenderEngine/3DCADMap.h>
#include "SimpleMesh3D.h"
#include <common/ARCPath.h>
#include "lines3d.h"

class CComponentMeshModel;
class CComponentMeshSection;


class SimpleMeshEditorConfig;
class CGrid;


#define XZVisibleFlag 16
#define YZVisibleFlag 32
#define PERSP_VisibleFlag 64
#define MeshEditCenterLineName _T("CenterLine")

class  CComponentEditOperation;

class CComponentEditContext;
//////////////////////////////////////////////////////////////////////////
class RENDERENGINE_API CSimpleMeshEditScene :	public C3DSceneBase
{
public:
	//lister the scene's changes
	BEGINE_LISTENER_DECLARE()
		LISTEN_METHOD_VOID(void OnRBtnDownScene(const CPoint& pt, const ARCVector3& worldPos))
		LISTEN_METHOD_VOID(void OnRBtnDownSection(CComponentMeshSection* pSection,const CPoint& pt))
		LISTEN_METHOD_VOID(void OnDBClikcOnSectionPoint(CComponentMeshSection* pSection,int pIdx, const CPoint& pt))
		LISTEN_METHOD_VOID( void OnFinishDistanceMesure(double) )
	END_LISTENER_DECLARE()


	enum emMouseState
	{
		_default,
		_getonepoint,
		_gettwopoints, 
		_getmanypoints,
		_getonerect,	
		_distancemeasure,
		_moveElements,
		_rotateElements,
	};
	//	
	CSimpleMeshEditScene();
	void Setup(CComponentEditContext* pEdCtx); //setup scene camera
	
	void OnModelChange( CComponentMeshModel* pModel );
	
	//save model snap bmp and mesh
	void SaveModel();
	

	//set the section to be selected
	void ActiveSection(CComponentMeshSection* pSection);
	void SetSelectSegment(int idx);

	void UpdateMeshViewType();
	CString GetCurMeshMat()const; 

	ARCVector3 GetMousePosXY()const;
	ARCVector3 GetMousePosXZ()const;
	ARCVector3 GetMousePosYZ()const;

	ARCVector3 GetWorldIntersect(const Ogre::Vector3& vMin, const Ogre::Ray& ray)const;

public:
	//mouse functions on the scene, inherit from scene base
	virtual BOOL OnMouseMove(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnLButtonDown(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnLButtonUp(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnRButtonDown(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnLButtonDblClk(const MouseSceneInfo& mouseInfo);


	void OnUpdateOperation(CComponentEditOperation* pOP);
	void OnUpdateGrid(CComponentMeshModel* pMeshModel);
	
	void UpdateCADMap(CComponentMeshSection* pSection);

	void SetEditContext(CComponentEditContext* pEditContext){ mpEditContext = pEditContext; }
	CComponentEditContext* mpEditContext;


	ARCPath3 m_vMousePosList;
	emMouseState mMouseState;
	void ResetMouseState();	
	void StartDistanceMeasure();
	void EndDistanceMeasure();	
	CLines3D mPickLine;	

	BOOL OnRButtonUp( const MouseSceneInfo& mouseInfo );
protected://data members
	CGrid3D mGridXY;
	CGrid3D mGridYZ;
	CGrid3D mGridXZ;

	//edit mod
	CCenterLine3D mCenerLine;
	CSection3DControllerList mSection3DList;
	CSectionDetailController mSetionDetailCtrl;
	C3DCADMapNonSharp mLoadMap;
	CSimpleMesh3D mEditMesh;

	//display mode
	C3DNodeObject mExternalMesh;	


	//
	void UpdateGrid( CComponentMeshModel* pModel );
};


	