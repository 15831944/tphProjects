#pragma once

#include <renderer\renderengine\3dbaseview.h>
#include <Renderer\RenderEngine\SectionEditScene.h>
#include "MeshEdit3DView.h"

class CComponentMeshModel;
class CComponentEditContext;
class CSimpleMeshEditScene;
/** 
* the view of the edit section scene, deal with user mouse actions

*/
class CMeshSectionEditView : public CMeshEdit3DView
{
	DECLARE_DYNCREATE(CMeshSectionEditView)
protected:
	CMeshSectionEditView(void);

public:
	~CMeshSectionEditView(void);
	virtual void SetupViewport();
	C3DSceneBase* GetScene( Ogre::SceneManager* pScene );
	void OnDBClikcOnSectionPoint(CComponentMeshSection* pSection,int pIdx, const CPoint& pt);
protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);


	afx_msg void OnAddStationPoint();
	afx_msg void OnDelStationPoint();
	afx_msg void OnModifyThickness();
	afx_msg void OnStationProp();

	//CSectionEditScene m_sectionScene;
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnLinetypeCubic();
	afx_msg void OnUpdateLinetypeCubic(CCmdUI *pCmdUI);
	afx_msg void OnLinetypeStraight();
	afx_msg void OnUpdateLinetypeStraight(CCmdUI *pCmdUI);
};
