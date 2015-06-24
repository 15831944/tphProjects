#if !defined(AFX_PAXFLOWDLG_H__BD3D7231_88A9_4CB6_858F_37A06B4BB5E3__INCLUDED_)
#define AFX_PAXFLOWDLG_H__BD3D7231_88A9_4CB6_858F_37A06B4BB5E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxFlowDlg.h : header file
//
//#include "PrintableTreeCtrl.h"
//#include "TreeCtrlEx.h"
#include "PaxFlowTreeCtrl.h"
#include "inputs\paxflow.h"
#include "inputs\AllPaxTypeFlow.h"
#include "inputs\PaxFlowConvertor.h"
#include "inputs\SinglePaxTypeFlow.h"
#include <afxtempl.h>
#include <MFCExControl/SplitterControl.h>
#include <afxpriv.h>
#include "SortListCtrl.h"
#include <set>
#include <stack>
#include "resource.h"

//----------------------------------------------------
// Menu define
//----------------------------------------------------
#define IDM_PF_ADDPROC                 101
#define IDM_PF_ADDCHILDPROC            102
#define IDM_PF_ADDSIBLINGPROC          103
#define IDM_PF_DELPROC                 104
#define IDM_PF_PERCENT                 105

//----------------------------------------------------
// struct define
//----------------------------------------------------

typedef struct tagPAXTYPE
{
	TCHAR szPaxType[256];
	TCHAR szFlight[256];
	TCHAR szSector[256];
	TCHAR szCategory[256];
} PAXTYPE, *LPPAXTYPE;

/*
class FlowTreeItemData
{
public:
	ProcessorID m_procId;
	int m_nPercent;
	bool m_bStationDep;
};
*/
struct TreeNodeInfor
{
	HTREEITEM m_hTreeNode;
	bool m_bBold;
	COLORREF m_color;

	bool operator == (const TreeNodeInfor& treeNode)const
	{
		if (treeNode.m_hTreeNode == m_hTreeNode)
		{
			return true;
		}
		return false;
	}
};

typedef struct
{
	ProcessorID m_procId;
	int m_iSpecialHandleType ;
	CFlowDestination* m_arcInfo;
}TREEITEMDATA;
typedef std::vector<TREEITEMDATA*>ITEMDATAVECTOR;
/////////////////////////////////////////////////////////////////////////////
// CPaxFlowDlg dialog

class C3DView;
class CPaxFlowSelectPipes;
class CPaxFlowDlg : public CDialog
{
private:
	void InitColorTree();
	CPaxFlowConvertor m_paxFlowConvertor;

	CAllPaxTypeFlow m_allPaxFlow;

	//use to show on the tree
	CSinglePaxTypeFlow m_OperatedPaxFlow;

	CSinglePaxTypeFlow m_copyflow;

	CAllPaxTypeFlow m_allFlightFlow;
	CSinglePaxTypeFlow m_GateFlow;
	BOOL m_bExpand;
	CRect m_rectHide;
	BOOL m_bUpTree;
	BOOL m_bHaveToGate;
	BOOL m_bHaveFromGate;
	bool m_bGateChanged;
	int  m_iGateChangedIdx;
	int m_nProcType;
	int m_nSel;   //the index of the selected item
	int m_nSelChange;  //select changed
	std::vector<ProcessorID> m_vectID;
	std::vector<CString > m_vStrBuff;  // find string buffer
//	std::multiset
	std::vector<HTREEITEM> m_vFoundItem; //use for search item in flowtree, m_multisethItem store Item that had found in flow tree.
	CString m_strFind;      //record the find word  last time;
	bool m_bMatchWholeWord; //record the station last time;
	std::vector<HTREEITEM>::const_iterator m_iterator; //record index of m_vFoundItem
	
	int m_iPreSortByColumnIdx;	

	std::stack<int> m_stack1x1; // store {1x1:start} ,in order to confirm {1x1:start} and {1x1:end} is pair
	bool m_b1x1SeqError ; // true if 1x1 Sequence error ( miss {1x1:start} before {1x1:end})
	
	/************************************/
	//make the pre-operation area still being selected when rebuild the tree
	static std::vector<ProcessorID>m_vInterestInPath;
	HTREEITEM m_hInterestItem;
	void BuildInterestInPath( HTREEITEM _hCurrentItem  ,std::vector<ProcessorID>& _vInterestInPath = m_vInterestInPath );
	bool IsInterestNode( HTREEITEM _hCurrentItem );
	/************************************/


	/************************************/
	//make the pre-operation tree being similar with the new tree .
	std::vector<CString>m_vCollapsedNode;
	CString BuildPathKeyString( HTREEITEM _hCurrentItem );
	void BuildCollapsedNodeVector();
	void BuildCollapsedNodeVectorSubRoute( HTREEITEM _hRoot );
	bool IfThisItemShouldBeCollapsed(  HTREEITEM _hCurrentItem );
	void CollapsedTree();
	void CollapsedSpecificNode( HTREEITEM _hCurrentItem  );
	/************************************/


	bool IsProcess( const ProcessorID& _testID )const;
	InputTerminal* m_pInTerm;
	bool m_bChanged;
	int m_iChangedIdx ;
	int m_iListSelect;
	ITEMDATAVECTOR m_vItemData;
	ITEMDATAVECTOR m_vGateItemData;
	bool IfHaveCircle( HTREEITEM _testItem ,const ProcessorID _procID ) const;
	CString GetPipeString( CFlowDestination* _pArcInfo );
	CString GetOtherInfo(  CFlowDestination* _pArcInfo );
	void Update3DViewData();
	void Update3DViewData(const CSinglePaxTypeFlow* _pSPTF, const CSinglePaxTypeFlow* _pSPTFForGateAssaign);
	C3DView* Get3DView();
	void ClearUselessTree();

	void SaveSyncPointData();
	//CSplitterControl m_wndSplitter1, m_wndSplitter2,m_wndSplitter3;

	//the processors copy from one own flow to another flow
	std::deque<TreeNodeInfor> m_vCopyItem;
	FLOWDESTVECT m_vCopyProcessors;

	void SetCopyFlowDestion(const ProcessorID& procID,const CFlowDestination& flowDest);

	bool m_bAreadyCopy;
	int m_nLevelLen;

	CSinglePaxTypeFlow* m_pNonPaxFlow;
	// the flow Attached NonPaxFlow
	CSinglePaxTypeFlow* m_pPaxFlowAttachedToNonPaxFlow;
	//Confirm input OneXOne's data Available , return True if Available
//	bool ConfirmOneXOneAvailable();
	// Search all 1x1start, 1x1end in flow
	// confirm input data is correct. if data is not available, get the wrong item and select it
//	void SearchOneXOne( HTREEITEM _hItem );

// Construction
public:
	//************************************************************************
	//**Multi Operation
	void PaxFlowEditPipeAuto(HTREEITEM hItem);
	void PaxFlowAddDestNode(HTREEITEM hItem);
	void PaxFlowInsertBeforeNode(HTREEITEM hItem);
	void PaxFlowInsertAfterNode(HTREEITEM hItem);
	void PaxFlowCurSourceLink(HTREEITEM hItem);
	void PaxFlowDeleteNodeOnly(HTREEITEM hItem);
	void PaxFlowDeleteNodeAll(HTREEITEM hItem);
	void PaxFlowEditPipManual(HTREEITEM hItem,CPaxFlowSelectPipes* pSelectPipeDlg);
	void PaxFlowSetOneToOne(HTREEITEM hItem);
	void PaxFlowChannelSetOneXOneStart(HTREEITEM hItem);
	void PaxFlowchannelSetOneXOneEnd(HTREEITEM hItem);
	void PaxFlowSetOneXOneStart(HTREEITEM hItem);
	void PaxFlowSetOneXOneEnd(HTREEITEM hItem);
	void PaxFlowChannelSetOneXOneClear(HTREEITEM hItem);
	void PaxFlowEventRemaining(HTREEITEM hItem);
	void PaxFlowAddEndProcessor(HTREEITEM hItem);

public:
	CString GetArcInfoDescription( CFlowDestination* _pArcInfo , bool _bNeedDetail = false ,bool* _pbHasComplementData =NULL);
	
	afx_msg void OnPaxflowEditElevator();
	void SetViewPickConveyor(BOOL bActive3DView=TRUE);
	BOOL m_bCheckConveyorBtn;
	void OnSizeEx(int cx,int cy);
	void DoResize(int delta,UINT nIDSplitter);
	CSplitterControl m_wndSplitter1;
	CSplitterControl m_wndSplitter2;
	CSplitterControl m_wndSplitter3;
	CSplitterControl m_wndSplitter4;
	CSplitterControl m_wndSplitter5;
	void ResizeBottomWnd(CSize size,CSize sizeLast);
	CSize m_sizeLastWnd;
	void ResizeRightWnd(CSize size,CSize sizeLast);
	void ResizeCenterWnd(CSize size,CSize sizeLast);
	void ResizeLeftWnd(CSize size,CSize sizeLast);
	void ShowRightWnd(BOOL bShow);
	void ResizeToHideRightWnd();
	void AdjustDragedProcess();
	int m_nDragedProcess;
	BOOL IsEnableAddItem();
	void DisableFlowTreeToolBarBtn();
	void UpdateFlowTreeToolBarBtn(UINT iRet,HTREEITEM hItem);
	void PopProcessDefineMenu(CPoint point);
	CImageList m_image;
	BOOL m_bDragging;
	void EnableBtnFromTBar(CToolBar& pTB,UINT ID,BOOL bEnable);
	BOOL GetProcessorID(ProcessorID& _ID);
	int m_nCurSel;
	BOOL m_bDisplayProcess;
	BOOL m_bDisplayBaggageDeviceSign;
	CString m_sSelectedProcess;
	HTREEITEM     m_hItemDragSrc;   
	HTREEITEM	  m_hItemDragDes;
	CImageList*   m_pDragImage;      
	void SetType(int _nProcType);
	BOOL InitProcessCtrls();

	void SetPaxTypeLevelLen(int _LevelLen){ m_nLevelLen = _LevelLen;}

	
	CSize m_sizeWnd;

	BOOL InitFlow();
	CPaxFlowDlg(InputTerminal* _pTerm,CWnd* pParent = NULL);   // standard constructor
	void SetInputTerm( InputTerminal* _pTerm ) { m_pInTerm = _pTerm;	};
	virtual ~CPaxFlowDlg();

// Dialog Data
	//{{AFX_DATA(CPaxFlowDlg)
	enum { IDD = IDD_DIALOG_PAXFLOW };
	CButton	m_checkDisplayFlow;
	CStatic	m_staticProcessor;
	CStatic	m_staticProcess;
	CStatic	m_staticFlowTree;
	CTreeCtrl	m_ProcessTree;
	CTreeCtrl	m_ProcTree;
	CStatic	m_staticMobileTag;
	CStatic	m_staticFlightTitle;
	CStatic	m_staticGateTitle;
	CButton	m_butRestore;
	CPrintableTreeCtrl	m_treeGate;
	CListBox	m_listFlight;
	CButton	m_butAdvance;
	CButton	m_btnCancel;
	CButton	m_btnOk;
	CButton	m_btnSave;
	CSortListCtrl	m_listCtrlPaxType;
	CPaxFlowTreeCtrl	m_FlowTree;
	COLORREF m_treeTextDefColor;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxFlowDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


// Implementation
protected:	
	void SearchItemInTree(const HTREEITEM _hItem ,const CString _strSearch,BOOL _bMatchAll=FALSE);
	void ClearGateUselessTree();

	int GetStationType(int nLevel);
	////////////////////////////////
	//gate flow
	void GateflowAddchildprocessor();
	void GateflowInsertBettween();
	void GateflowInsertAfter();
	void GateflowDeleteprocessors();
	void GateflowCutSourceLink();
	void GateflowTakeover();
	void GateflowTakeOverByall();
	void GateflowModifypercent();
	void SetGatePercent( int _nPercent );
	////////////////////////////////////////////////
	void GateFlowPopMenu(CWnd* pWnd, CPoint point);
	void ExpandWindow(BOOL _bExpand = FALSE );
	void LoadGateTree();
	void FilterFlight();
	
	BOOL InitConvetor();
	bool IsStation( const ProcessorID& _procID );

	BOOL CheckIfHaveCircle(CSinglePaxTypeFlow* _pFlow);

	CToolBar m_ToolBarFlowTree;
	CToolBar m_ToolBarMEType;
	//std::vector<FlowTreeItemData> m_vectFlowTreeItemData; // used for store tree item data.
	std::vector<CString> m_csRealPaxString;	// used for created for constraint
	HTREEITEM m_hRClickItem;
	HTREEITEM m_hModifiedItem;					//The tree item used for saving percent info.

	HTREEITEM m_hGateRClickItem;
	CArray <CWnd*,CWnd*> m_listRightWnd;
	CArray	<CWnd*,CWnd*> m_listLeftBtn;
	void ReloadPaxList();
	void ReloadTree();
	void LoadSubTree( HTREEITEM _hItem);
	void LoadGateSubTree( HTREEITEM _hItem);
	InputTerminal* GetInputTerminal();

	void SetPercent(int _nPercent);
	
	CString GetProjPath();

	void CheckToolBarEnable();

	bool IfStartProcIsOwned();
	int GetSyncPointIndex(const ProcessorID& procID);

	//----------------------------------------------------------------------------
	//Summary:
	//		When passenger flow changes, the non passenger flow that hierarchy from will need change
	//----------------------------------------------------------------------------
	void BuildHierarchyNonPaxFlow(CSinglePaxTypeFlow* pParentFlow);

	//----------------------------------------------------------------------------
	//Summary:
	//		Retrial relative non passenger flow
	//----------------------------------------------------------------------------
	CSinglePaxTypeFlow* GetNonPaxFlow(CSinglePaxTypeFlow* pParentFlow);
	// Generated message map functions
	//{{AFX_MSG(CPaxFlowDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnDelete();
	afx_msg void OnBtnNew();
	afx_msg void OnBtnSave();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelchangeListPaxtype();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaxflowAddchildprocessor();
	afx_msg void OnPaxflowDeleteprocessors();
	afx_msg void OnPaxflowModifypercent();
	afx_msg void OnPaxflowDelpercent();
	afx_msg void OnSelchangedTreeFlow(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonPrint();
	afx_msg void OnPaxflowCutSourceLink();
	afx_msg void OnPaxflowTakeover();
	afx_msg void OnPaxflowTakeoverByall();
	afx_msg void OnPaxflowInsertBettween();
	afx_msg void OnPaxflowEditPipe();
	afx_msg void OnPaxflowEditConveyor();
	afx_msg void OnPaxflowInsertAfter();
	afx_msg void OnPaxflowAddIsolateNode();
	afx_msg void OnPaxflowOnetoone();
	afx_msg void OnPaxflowInsertFromgate();
	afx_msg void OnPaxflowInsertTogate();
	afx_msg void OnButAdvance();
	afx_msg void OnSelchangeListFlight();
	afx_msg void OnKillfocusTreeGate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButRestore();
	afx_msg void OnPaxflowEvenPercent();
	afx_msg void OnPaxflowInsertEnd();
	afx_msg void OnPaxflowInsertStart();
	afx_msg void OnPaint();
	afx_msg void OnBtnCopyFlow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSelchangedProcessTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusProcessTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragProcessTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaxflowEditPipeAuto();
	afx_msg void OnClose();
	afx_msg void OnPaxflowFlowconditionPercentsplit();
	afx_msg void OnPaxflowFlowconditionEvenremaining();
	afx_msg void OnPaxflowFlowconditionQuejulengthxx();
	afx_msg void OnPaxflowFlowconditionWaittimexxmins();
	afx_msg void OnPaxflowFlowconditonSkipWhenTimeLessThan();
	afx_msg void OnPaxflowRouteconditionPipesEnableautomatic();
	afx_msg void OnPaxflowRouteconditionPipesUserselect();
	afx_msg void OnPaxflowRouteconditionStraightline();
	afx_msg void OnPaxflowRouteconditionAvoidfixedqueues();
	afx_msg void OnPaxflowRouteconditionAvoidoverflowqueues();
	afx_msg void OnPaxflowRouteconditionAvoiddensity();
	afx_msg void OnClickListPaxtype(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListPaxtype(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaxflowFind();
	afx_msg void OnPaxflow1x1start();
	afx_msg void OnPaxflow1x1end();
	afx_msg void OnPaxflowFollowState();
	afx_msg void OnPaxflowChannelcondition11On();
	afx_msg void OnPaxflowChannelcondition11Off();
	afx_msg void OnPaxflowChannelcondition1x1End();
	afx_msg void OnPaxflowChannelcondition1x1Start();
	afx_msg void OnPaxflowChannelcondition1x1Clear();
	afx_msg void OnPaxflowConditionSyncPoint();
	afx_msg void OnClickTreeProcess(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnCopyProcessors();
	afx_msg void OnAddCopyProcessors();
	afx_msg void OnInsertCopyProcessors();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnPCTreeSelectProc(WPARAM wParam,LPARAM lParam);	
	afx_msg void OnToolbarDropDown(NMHDR* pNMHDR, LRESULT* plRes);
	afx_msg void OnSelectSyncPoint(UINT nID);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXFLOWDLG_H__BD3D7231_88A9_4CB6_858F_37A06B4BB5E3__INCLUDED_)
