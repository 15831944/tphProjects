#pragma once
#include "../MFCExControl/CoolTree.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgLandsideVehicleTypeDefine dialog
class LandsideVehicleBaseNode;
class LandsideVehicleTypeNode;
class CDlgLandsideVehicleTypeDefine : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgLandsideVehicleTypeDefine)

public:
	CDlgLandsideVehicleTypeDefine(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLandsideVehicleTypeDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDEVEHICLETYPEDEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnCmdNew();
	afx_msg void OnCmdDelete();
	afx_msg void OnCmdEdit();
	afx_msg void OnBnClickedSave();
	afx_msg void OnTvnSelchangedTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void InitToolBar(void);
	void SetTreeContent(void);
	void UpdateToolBar(void);

	void InsertTreeItem(HTREEITEM hItem,LandsideVehicleTypeNode* pNode);
private:
	CToolBar        m_ToolBar;
	CCoolTree	    m_wndTreeCtrl;
	LandsideVehicleBaseNode* m_pNodeList;
};
