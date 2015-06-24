#if !defined(AFX_PROCESSDEFINEDLG_H__B65D855E_7DDD_4919_B1AB_14E76112C745__INCLUDED_)
#define AFX_PROCESSDEFINEDLG_H__B65D855E_7DDD_4919_B1AB_14E76112C745__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessDefineDlg.h : header file
//
#include "inputs\IN_TERM.h"
#include "MFCExControl\SmartEdit.h"
#include <vector>
#include "paxFlowdlg.h"
#include "PrintableTreeCtrl.h"
#include <MFCExControl/SplitterControl.h>
#include "resource.h"


class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// CProcessDefineDlg dialog

class CProcessDefineDlg : public CDialog
{
	DECLARE_DYNCREATE(CProcessDefineDlg)
	// Construction	
	enum CopyProcessorOperation
	{
		Add_CopyProcessor_Operation,
		Insert_CopyProcessor_Operation
	};
public:
	CProcessDefineDlg(CTermPlanDoc* pDoc, CWnd* pParent = NULL);   // standard constructor
public:		
	~CProcessDefineDlg();
// Dialog Data
	//{{AFX_DATA(CProcessDefineDlg)
	enum { IDD = IDD_DIALOG_PROCESS };
	CStatic	m_static2;
	CStatic	m_static1;
	CStatic	m_splitter;
	CButton	m_ok;
	CButton	m_cancel;
	CStatic	m_staticProcSeq;
	CStatic	m_straticProcList;
	CStatic	m_staticTreeFrame;
	CStatic	m_staticTreeToolBar;
	CStatic	m_staticFrame;
	CColorTreeCtrl	m_treeProcess;
	CTreeCtrl	m_treeAllProcessor;
	CListBox	m_listProcess;
	CButton	m_butSave;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessDefineDlg)
	public:
//	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

private:
	CString GetArcInfoDescription( CFlowDestination* _pArcInfo , bool _bNeedDetail = false,bool* _pbHasComplementData =NULL);
	CString GetPipeString( CFlowDestination* _pArcInfo );
	void LoadSubTree(HTREEITEM hStartRoot,CProcessorDestinationList* startPair);
	void SetPercent(int _nPercent);
	CString GetOtherInfo( CFlowDestination* _pArcInfo );
	void SetToolBarRemoveBut();
	void SetToolbarAddBut();
	void SetProcessTreeToolBar();
	void DoResize( int delta );
	void InitSpiltter();
	void PopProcessDefineMenu( CPoint point );
	bool IfHaveCircle( HTREEITEM _testItem,const ProcessorID _procID ) const;
	void LoadSubTree( HTREEITEM _hItem );
	void ReloadProcessFlow(  );
	CString GetProjectPath();
	bool IfExistInProcessor(CString _strName );
	void EditItem( int _idx );
	void LoadProcessData();
	InputTerminal* GetInputTerminal();
	void LoadAllProcessor();
	void InitToolBar();
	bool CopyProcessorOperationVaild(CopyProcessorOperation emtype,CString& strError);
	CToolBar  m_toolBar;
	CToolBar  m_toolBarProcessTree;
	std::vector<ProcessorID> m_vectProcessorID;
	CSmartEdit* pEdit; 
	ITEMDATAVECTOR m_vTreeItemData;
	CSinglePaxTypeFlow* m_pCurFlow;
	HTREEITEM m_hRClickItem;
	HTREEITEM m_hModifiedItem;
	// for drag and drop
	BOOL          m_bDragging;       
	CImageList*   m_pDragImage;      
	HTREEITEM     m_hItemDragSrc;   
	HTREEITEM	  m_hItemDragDes;
	CImageList	  m_image;
	HTREEITEM	  m_hAllProcessorSelItem;
	HTREEITEM	  m_hPocessSelItem;
	CSize m_sizeLastWnd;

	CSplitterControl m_wndSplitter;
	CSize m_sizeFormView;

	CTermPlanDoc* m_pDoc;

	void OnProcessRouteconditionStraightline();

	//the processors copy from one own flow to another flow
	std::deque<TreeNodeInfor> m_vCopyItem;
	FLOWDESTVECT m_vCopyProcessors;

	std::stack<int> m_stack1x1; // store {1x1:start} ,in order to confirm {1x1:start} and {1x1:end} is pair
	bool m_b1x1SeqError ; // true if 1x1 Sequence error ( miss {1x1:start} before {1x1:end})

	static std::vector<ProcessorID>m_vFocusInPath;
	HTREEITEM m_hInterestItem;
	void BuildInterestInPath( HTREEITEM _hCurrentItem  ,std::vector<ProcessorID>& _vInterestInPath = m_vFocusInPath );
	bool IsInterestNode( HTREEITEM _hCurrentItem );

	std::vector<CString>m_vCollapsedNode;
	CString BuildPathKeyString( HTREEITEM _hCurrentItem );
	void BuildCollapsedNodeVector();
	void BuildCollapsedNodeVectorSubRoute( HTREEITEM _hRoot );
	bool IfThisItemShouldBeCollapsed(  HTREEITEM _hCurrentItem );
	void CollapsedTree();
	void CollapsedSpecificNode( HTREEITEM _hCurrentItem  );

public:
	//************************************************************************
	//**Multi Operation
	void SubFlowEditPipeAuto(HTREEITEM hItem);
	void SubFlowAddDestNode(HTREEITEM hItem);
	void SubFlowInsertBeforeNode(HTREEITEM hItem);
	void SubFlowInsertAfterNode(HTREEITEM hItem);
	void SubFlowCurSourceLink(HTREEITEM hItem);
	void SubFlowDeleteNodeOnly(HTREEITEM hItem);
	void SubFlowEditPipManual(HTREEITEM hItem,CPaxFlowSelectPipes* pSelectPipeDlg);
	void SubFlowSetOneToOne(HTREEITEM hItem);
	void SubFlowChannelSetOneXOneStart(HTREEITEM hItem);
	void SubFlowchannelSetOneXOneEnd(HTREEITEM hItem);
	void SubFlowSetOneXOneStart(HTREEITEM hItem);
	void SubFlowSetOneXOneEnd(HTREEITEM hItem);
	void SubFlowChannelSetOneXOneClear(HTREEITEM hItem);
	void SubFlowEventRemaining(HTREEITEM hItem);

private:
	void UpdateModifyPercentState();
	void UpdateEvenPercentState();
	void UpdateEidtPipeState();
	void UpdateAutoPipeState();
	void UpdateToolBarState(int nToolBarID);
// Implementation
protected:

	
	// Generated message map functions
	//{{AFX_MSG(CProcessDefineDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnSelchangeListProcess();
	afx_msg LONG OnEndEdit( WPARAM p_wParam, LPARAM p_lParam );
	afx_msg void OnButSave();
	afx_msg void OnBegindragTreeAllprocessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnProcessAddIsolateNode();
	afx_msg void OnProcessAddDestNode();
	afx_msg void OnProcessAddBefore();
	afx_msg void OnProessAddAfter();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProcessCutSourceLink();
	afx_msg void OnProcessTakeover();
	afx_msg void OnSelchangedTreeProcess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeAllprocessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPdAddDest();
	afx_msg void OnPdAddNew();
	afx_msg void OnPdInsertAfter();
	afx_msg void OnPdInsertBefore();
	afx_msg void OnPdPrune();
	afx_msg void OnPdRemoveProcess();
	afx_msg void OnProcessOnetoone();
	afx_msg void OnPdOnetoone();
	afx_msg void OnPd1x1start();
	afx_msg void OnPd1x1end();
	afx_msg void OnPdChannelcondition1x1End();
	afx_msg void OnPdChannelcondition1x1Start();
	afx_msg void OnPdChannelcondition1x1Clear();
	afx_msg void OnMyok();
	afx_msg void OnMycancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnUpdateBtnDel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnAddNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnAddDest(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnInsertBefore(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnInsertAfter(CCmdUI* pCmdUI);

	afx_msg void OnUpdateBtnPrune(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnRemoveProcess(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnOntToOne(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtn1X1Start(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtn1X1End(CCmdUI* pCmdUI);
	afx_msg void OnProcessModifypercent();
	afx_msg void OnProcessEditPipeAuto();
	afx_msg void OnProcessEditPipe();
	afx_msg void OnUpdateBtnPipe(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateBtnPipeAuto(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnModifyPercent(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateBtnEvenPercent(CCmdUI* pCmdUI); 
	afx_msg void OnClickTreeProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCopyProcessors();
	afx_msg void OnAddCopyProcessors();
	afx_msg void OnInsertCopyProcessors();

	DECLARE_MESSAGE_MAP()

public:
	void SetActiveDoc(CDocument* pDoc);
	void OnClickDropdown();
	
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif
	
	afx_msg void OnProcessEvenpercent();

	afx_msg void OnPipesUserselect();
	afx_msg void OnPipesEnableautomatic();
	afx_msg void OnChannelOff();
	afx_msg void OnChannelOn();
	afx_msg void OnPercentPercentsplit();
	afx_msg void OnPercentEvenremaining();

	void InitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSDEFINEDLG_H__B65D855E_7DDD_4919_B1AB_14E76112C745__INCLUDED_)
