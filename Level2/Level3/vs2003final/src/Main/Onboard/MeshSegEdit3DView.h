#pragma once
#include <renderer\renderengine\3dbaseview.h>
#include <renderer\RenderEngine\MeshXZEditScene.h>
#include <Renderer\RenderEngine\CoordinateDisplayScene.h>
#include "MeshEdit3DView.h"
class CSimpleMeshEditScene;
class CComponentEditContext;

class CMeshSegEdit3DView : public CMeshEdit3DView
{
public:
	DECLARE_DYNCREATE(CMeshSegEdit3DView)

	CMeshSegEdit3DView(void);
	~CMeshSegEdit3DView(void);

	virtual void SetupViewport();	
	C3DSceneBase* GetScene( Ogre::SceneManager* pScene );
	//inherit from edit scene listener
protected:
	//on msg functions
	//CMeshXZEditScene mScene;
	CCoordinateDisplayScene m_coordDisplayScene;
	virtual bool MoveCamera(UINT nFlags, const CPoint& ptTo, int vpidx =0 );
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:

};
