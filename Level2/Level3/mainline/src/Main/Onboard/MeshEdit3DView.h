#pragma once
#include <Renderer\RenderEngine\CoordinateDisplayScene.h>
#include "MeshEditFrame.h"
#include <Renderer\RenderEngine\SimpleMeshEditScene.h>
#include <renderer\renderengine\3dbaseview.h>

class CComponentMeshModel;
class CComponentEditContext;

class CMeshEdit3DView : public C3DBaseView , public CSimpleMeshEditScene::Listener
{
	DECLARE_DYNCREATE(CMeshEdit3DView)
public:
	CSimpleMeshEditScene* GetMeshEditScene();
	CComponentEditContext* GetEditContext();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	virtual void OnRBtnDownSection(CComponentMeshSection* pSection,const CPoint& pt );
	virtual void OnRBtnDownScene(const CPoint& pt, const ARCVector3& worldPos);
	//messg
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnOnboardcomponenteditConfiguregrid();
	afx_msg void OnOnboardcomponenteditAddstation();

	afx_msg void OnLayoutLock();
	afx_msg void OnDistanceMeasure();
	afx_msg void OnUpdateUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRedo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLayoutLock(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDistanceMeasure(CCmdUI *pCmdUI);
	ARCVector3 mLastMousePosOnScene;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEditsectionDelete();
	afx_msg void OnEditsectionCopy();
	afx_msg void OnEditsectionProp();
	afx_msg void OnViewSolid();
	afx_msg void OnUpdateViewSolid(CCmdUI *pCmdUI);
	afx_msg void OnViewWireframe();
	afx_msg void OnUpdateViewWireframe(CCmdUI *pCmdUI);
	afx_msg void OnViewSolidwireframe();
	afx_msg void OnUpdateViewSolidwireframe(CCmdUI *pCmdUI);
	afx_msg void OnViewTransfarent();
	afx_msg void OnUpdateViewTransfarent(CCmdUI *pCmdUI);
	afx_msg void OnViewTransparentwireframe();
	afx_msg void OnUpdateViewTransparentwireframe(CCmdUI *pCmdUI);
	afx_msg void OnViewHiddenwireframe();
	afx_msg void OnUpdateViewHiddenwireframe(CCmdUI *pCmdUI);
};






class CSimpleMeshEditView :
	public CMeshEdit3DView
{
public:
	DECLARE_DYNCREATE(CSimpleMeshEditView)

	CSimpleMeshEditView(void);
	~CSimpleMeshEditView(void);

	C3DSceneBase* GetScene( Ogre::SceneManager* pScene );
	virtual void SetupViewport();	
	//inherit from edit scene listener
	virtual void OnActiveView();

protected:
	virtual bool MoveCamera(UINT nFlags, const CPoint& ptTo, int vpidx =0 );
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	//on msg functions
protected:
	DECLARE_MESSAGE_MAP()
protected:
	CCoordinateDisplayScene m_coordDisplayScene;
	void OnFinishDistanceMesure(double);
};
