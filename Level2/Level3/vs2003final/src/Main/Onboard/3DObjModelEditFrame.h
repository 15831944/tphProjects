#pragma once

#include "../XPStyle/StatusBarXP.h"
#include <Renderer/RenderEngine/3DNodeObject.h>

#include "../Dlg3DObjectProperty.h"

class CDlg3DObjectProperty;
class C3DObjModelEditTreeView;
class C3DObjModelEdit3DView;
class C3DObjModelEditContext;
class C3DObjModelEditScene;
class CModel;

class C3DObjModelEditFrame : public CMDIChildWnd, public CDlg3DObjectProperty::UpdateListener
{
	DECLARE_DYNCREATE(C3DObjModelEditFrame)
protected:
	C3DObjModelEditFrame();           // protected constructor used by dynamic creation
	virtual ~C3DObjModelEditFrame();

	virtual void On3DObjectPropertyUpdate();

	//update view in this frame
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual CRuntimeClass* Get3DViewClass() const;
	virtual C3DObjModelEditContext* CreateEditContext() const;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnUpdate3dMousePos(CCmdUI *pCmdUI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:

	C3DNodeObject GetEditModel() const;

	afx_msg void On3DNodeObjectProperty();

	enum UpdateViewType
	{
		Type_ReloadData = 0,
		Type_AddNewItem,
		Type_DeleteItem,
		Type_SelectItem,
	};
	void UpdateViews(CView* pSrcView = NULL, UpdateViewType updateType = Type_ReloadData, DWORD dwData = 0);

	C3DObjModelEditTreeView* GetTreeView() const { return m_pTreeView; }
	C3DObjModelEdit3DView* Get3DView() const { return m_p3DView; }

	C3DObjModelEditContext* GetModelEditContext() { return m_pModelEditContext; }
	const C3DObjModelEditContext* GetModelEditContext() const { return m_pModelEditContext; }
	C3DObjModelEditScene* GetModelEditScene();
	CStatusBarXP& GetStatusBar() { return m_wndStatusBar; }

	bool IsModelExternal() const;

private:
	C3DObjModelEditContext* m_pModelEditContext;

	CSplitterWnd m_wndSplitterLR;
	C3DObjModelEditTreeView* m_pTreeView;
	C3DObjModelEdit3DView*    m_p3DView;
	CStatusBarXP  m_wndStatusBar;

	CDlg3DObjectProperty* m_dlg3DObjProperty;

};


