#pragma once

#include "DlgGatArp.h"

class CTermPlanDoc;
class CTVNode;
class CChildFrameSplit;
class InputTerminal;
class CPlacement;
class CReportParameter;
class CPaxFlowDlg;
class CProcessDefineDlg;
class CCoolTreeEx;
class CARCBaseTree;
class CDlgScheduleAndRostContent;
namespace airside_engine
{
	class Simulation;
	class FlightList;
	class FlightEvent;
}

namespace ns_AirsideInput
{
	class CFlightPlans;
}
/////////////////////////////////////////////////////////////////////////////
// CNodeView view

class CNodeView : public CView
{
protected:
	CNodeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNodeView)
	

	CImageList				m_imgList;;
	CTVNode					*m_pDragNode;
	CImageList				m_ilNodes;
//	CCoolTreeEx				*m_pTree;
	CARCBaseTree			m_wndTreeCtrl;
	CReportParameter		*m_pCopyReportPara;
	CDlgScheduleAndRostContent* m_pDlgScheduleRosterContent;

	CBitmapButton m_btnMoveFloorUp;
	CBitmapButton m_btnMoveFloorDown;
public:
	CPaxFlowDlg				*m_pDlgFlow;
	CProcessDefineDlg		*m_pDlgProcessDefine;
	CDlgGatArp				dlgGetArp;

// Attributes
public:
	CTermPlanDoc* GetDocument();
	InputTerminal* GetInputTerminal() const;
	CPlacement& GetPlaceMent() const;
	CString GetProjectPath() const;
	CChildFrameSplit* GetMDIChildOwner() { return (CChildFrameSplit*)(GetOwner()->GetOwner()); };
	void setCopyReortPara( CReportParameter* _pPara );
	CReportParameter* getCopyReportPara( void ) const;

	void UpdateSurfaceNode();
	void UpdateWallShapeNode();

private:
	bool IfLinkageOrGateAssignUsingThisProc( CString _strProcName )const;
	bool IfProcTypeCanBeChanged( CString _strProcName )const;
	void ChangeProcType( CString _strProcName, int _iChangeToType );
	CARCBaseTree& GetTreeCtrl(){ return m_wndTreeCtrl;}

public:
	HTREEITEM AddItem(CTVNode* pNode, CTVNode* pAfterNode = NULL);
	void AddSortItem(CTVNode* pNode,HTREEITEM hInsertAfter);
	void RefreshProcessorItem(CTVNode* pNode,CTVNode* pRootNode,const ProcessorID& procID);//in_parameter prootnode is route from pnode that can not modify
	void RefreshItem(CTVNode* pNode);
	static void UpdatePopMenu(CCmdTarget* pThis, CMenu* pPopMenu);

	virtual void OnInitialUpdate();
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	void OnNewProc(ProcessorClassList procType);
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnSize(UINT nType, int cx, int cy);
protected:
	virtual ~CNodeView();

	void PumpMessages();

protected:
	bool VersionCheck();
	bool UnZipLayoutFile( const CString& _strZip, const CString& _strPath );
	void DeleteAllChildren(HTREEITEM hItem);
	void DeleteFirstChild(HTREEITEM hItem);
	CTVNode* FindNode(HTREEITEM hItem);
	void RenameSelected();
	void SwapTwoFloor(int flrIndex1, int flrIndex2);
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnBeginLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCtxRename();
	afx_msg LRESULT OnSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemExpanding(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateFloorVisible(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGridVisible(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFloorMonochrome(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowHideMap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOpaqueFloor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEnableCoutourEdit(CCmdUI* pCmdUI);
	afx_msg void OnNewProcessor();
	afx_msg void OnCtxProcproperties();
	afx_msg void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCtxDeleteProc();
	afx_msg void OnCtxDeleteFloor();
	afx_msg void OnRenameProc2();
	afx_msg void OnCtxComments();
	afx_msg void OnRailwaylayoutDefine();
	afx_msg void OnPipeDefine();
	afx_msg void OnPaint();
	afx_msg void OnCtxChangeToPoint();
	afx_msg void OnCtxChangeToFloorchange();
	afx_msg void OnCtxChangeToGate();
	afx_msg void OnCtxChangeToSink();
	afx_msg void OnCtxChangeToSource();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLayoutexport();
	afx_msg void OnLayoutimportAppend();
	afx_msg void OnLayoutimportReplace();
	afx_msg void OnUpdateFloorsDelete(CCmdUI* pCmdUI);
	afx_msg void OnMenuNewProc(UINT nID);
	afx_msg void OnCtxArp();
	afx_msg void OnCtxPickXYFromMap();
	afx_msg void OnContourAddNextContour();
	afx_msg void OnCtxProcdispproperties();
	afx_msg void OnAddStructure();
	afx_msg void OnAddSurfaceArea();
	afx_msg void OnEnableCoutourEdit();
	afx_msg void OnSortFloor();


	LRESULT OnTempFallbackFinished(WPARAM, LPARAM);

public:
	afx_msg void OnCtxAddGate();
	afx_msg void OnGateProcdispproperties();
	afx_msg void OnWallshapeAddwallshape();
	afx_msg void OnBtnMoveUpFloor();
	afx_msg void OnBtnMoveDownFloor();

	void ReloadFloorsItem(HTREEITEM hParentItem, CString strSelFlrName=_T(""));

	void ResetFloorIndexToAll();


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
		afx_msg void OnPlaceMarkerLine() ;
		LRESULT SetActiveFloorMarkerLine(WPARAM wParam, LPARAM lParam) ;
private:
#define  CALLBACK_ALIGNLINE  0
#define CALLBACK_PICKXYFROMMAP 1 
        int callback_ver ;
};

#ifndef _DEBUG // debug version in NodeView.cpp
inline CTermPlanDoc* CNodeView::GetDocument()
{ return (CTermPlanDoc*) m_pDocument; }
#endif
