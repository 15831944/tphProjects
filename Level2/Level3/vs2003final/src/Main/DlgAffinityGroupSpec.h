#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "TreeCtrlEx.h"

// CDlgAffinityGroupSpec dialog
class CAffinityGroupManager ;
class CAffinityGroup;
class CAffinityPassengerTy ;
class CAffinityData ;
class NodeData
{
public:
	int nodeTy ;
	void* pData ;
	NodeData():nodeTy(0),pData(NULL){} ;
	~NodeData() {} ;
};
#define  TY_GROUP_NODE  0x0001
#define  TY_PAXTY_NODE  0x0002 
#define  TY_OTHER_NODE  0x0003
#define  TY_COHESION_NODE 0x0004 
#define  TY_PAXTY_ROOT_NODE 0x0005
#define  TY_PAXTY_NUMBER_NODE 0x0006
#define  TY_LEADER_NODE 0x0007
#define  TY_PACER_NODE 0x0008
#define  TY_FLIGHTTYPE_NODE 0x0009
class CDlgAffinityGroupSpec : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAffinityGroupSpec)

public:
	CDlgAffinityGroupSpec(InputTerminal* _terminal ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAffinityGroupSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_AFFINITY_GROUPS };
public:
	virtual BOOL OnInitDialog() ;
	BOOL OnCreate(LPCREATESTRUCT lpCreateStruct) ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CEdit m_StaToolBar;
	CCoolTree m_treeGroup;
public:
	afx_msg void OnBnClickedButtonSave();
private:
	CAffinityGroupManager* m_GroupManager ; 
	CToolBar m_wndToolbar;
	std::vector<NodeData*> m_TreeNodeData ;
	InputTerminal* m_terminal ;
	HTREEITEM m_hRClickItem ;
protected:
	void OnInitTree() ;
	void OnInitToolBar() ;


	CString FormatGroupTreeNodeName(CString _name) ;
	CString FormatPaxTYNodeName(CString _name);
	CString FormatGroupNumOnAircraftNodeName(int _num) ;
	CString FormatGroupCohensionNodeName(BOOL _res) ;
	CString FormatNumberInPaxTyNodeName(int _num) ;
	CString FormatPassengerTyLeaderNodeName(BOOL _res) ;
	CString FormatPassengerTyPacerNodeName(BOOL _res) ;

	void OnSize(UINT nType, int cx, int cy) ;
protected:
	void AddGroupTreeNode(CAffinityGroup* _group,HTREEITEM _FlightNode) ;
	void AddGroupPaxTyTreeNode(CAffinityPassengerTy* _paxty,HTREEITEM item) ;
    void AddFlightTyNode(CAffinityData* _data) ;
	void InsertLeaderItem(HTREEITEM item ,CAffinityPassengerTy* _paxty) ;
	void InserPacerItem(HTREEITEM item ,CAffinityPassengerTy* _paxty);
	 void DelchildItemByType(HTREEITEM item ,int TY) ;
protected:
	void InitComboString(CComboBox* _box,HTREEITEM Item) ;
	void SetGroupCohesion(CString str ,CAffinityGroup* _group) ;
	void SetPassengerTyLeader(CString str , CAffinityPassengerTy* _paxty) ;
	void SetPassengerTyPacer(CString str , CAffinityPassengerTy* _paxty);
	void HandleComboxChange(CComboBox* _box ,HTREEITEM _item) ;
	void HandleEditLable(CString _val ,int NodeTy,HTREEITEM _item) ;
protected:
	void OnNewAffinityFlightTY() ;
	void OnDelAffinityFlightTY() ;

	 void OnNewAffinityGroup() ;
	 void OnDelAffinityGroup() ;
//	 void OnEditAffinityGroup() ;
     
	 void OnNewAffinityPassengerTy();
	 void OnDelAffinityPassengerTy() ;
	// void OnEditAffinityPassengerTy() ;
public:
	afx_msg void OnNewToolBar() ;
	afx_msg void OnDelToolBar() ;
	afx_msg void OnEditToolBar() ;
	afx_msg void HandleLeader() ;
	afx_msg void HandlePacer() ;
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
    void OnContextMenu(CWnd* /*pWnd*/, CPoint point) ;
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	 void OnOK() ;
	  void OnCancel() ;

	   LRESULT OnEndLabelEdit(WPARAM wParam, LPARAM lParam) ;
	    LRESULT OnBeginLabelEdit(WPARAM wParam, LPARAM lParam) ;
public:
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
protected:
// 	CEdit m_Caption;
// 	CStatic m_StaticCap;
	CStatic m_sta_tree;
	NodeData* m_priNodedata ;
};
