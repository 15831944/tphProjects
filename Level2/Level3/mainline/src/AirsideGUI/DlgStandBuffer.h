#pragma once
#include "NodeViewDbClickHandler.h"
#include "EditTreeCtrl.h"
#include "../InputAirside/CParkingStandBuffer.h"
#include "VehiclePoolsAndDeploymentCoolTree.h"
#include "../MFCExControl/XTResizeDialog.h"
class CDlgStandBuffer : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgStandBuffer)
public:
	CDlgStandBuffer(int nProjID, PSelectFlightType pSelectFlightType,CAirportDatabase *pAirportDatabase , CWnd* pParent = NULL);
	virtual ~CDlgStandBuffer();

protected:
	int					m_nProjID;
	CParkingStandBuffer m_psbStdBuffer;
	CToolBar m_wndToolBar;
	/*CEditTreeCtrl*/CVehiclePoolsAndDeploymentCoolTree m_wndTreeCtrl;
	PSelectFlightType	m_pSelectFlightType;
	CAirportDatabase *  m_pAirportDatabase;
	CStringArray m_ayDepFltType;
	CStringArray m_ayArrFltType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();	
    void InitToolbar(void);
	int GetTreeLevel(HTREEITEM hItem);
	void RefreshTreeCtrl(void);
	afx_msg void OnNewItem(void);
	afx_msg void OnDeleteItem(void);
	afx_msg void OnEditItem(void);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSave(void);
	afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult); 
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
};
