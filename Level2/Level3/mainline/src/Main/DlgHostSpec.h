#pragma once
#include "MFCExControl/CoolTree.h"
#include "inputs\AllPaxTypeFlow.h"
#include "inputs\PaxFlowConvertor.h"
#include "inputs\SinglePaxTypeFlow.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgHostSpec dialog
class TrayHostList;
class TrayHost;
class InputTerminal;
class CDlgHostSpec : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgHostSpec)

	enum TreeNodeType
	{
		HOST_NON = 0,
		HOST_NAME,
		HOST_MOB,
		HOST_CON,
		HOST_RADIUS,
		HOST_ITEM
	};

	class TowNodeData
	{
	public:
		TowNodeData()
			:dwData(NULL)
			,emNodeType(HOST_NON)
			,nNonPaxIndex(-1)
		{

		}
		~TowNodeData()
		{

		}
	public:
		DWORD dwData;
		TreeNodeType emNodeType;
		int nNonPaxIndex;
	};

public:
	CDlgHostSpec(int nProjID,InputTerminal* pInterm,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgHostSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_HOSTSPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCmdNew();
	afx_msg void OnCmdDelete();
	afx_msg void OnCmdEdit();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();
	afx_msg void OnCancel();
	afx_msg void OnTvnSelchangedTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void InitToolBar(void);
	void SetTreeContent(void);
	void UpdateToolBar(void);
	InputTerminal* GetInputTerminal();
	void AddHostNode();
	void AddNonPaxNode(HTREEITEM hItem);
	bool CheckDeleteOperationValid(TrayHost* pHost);
private:
	CToolBar        m_ToolBar;
	CCoolTree	    m_wndTreeCtrl;
	TrayHostList*	m_pTrayHostList;
	int             m_nProjID;
	InputTerminal*	m_pInterm;
};
