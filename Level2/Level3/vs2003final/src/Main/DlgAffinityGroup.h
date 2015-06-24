#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "TreeCtrlEx.h"

class CAffinityGroup;
class CAffinity_FlightTypeData;
class CAffinity_GroupTypeData;
class CAffinity_PassengerTypeData;
class InputTerminal;

class CDlgAffinityGroup: public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAffinityGroup)
public:
	CDlgAffinityGroup(InputTerminal* _terminal, CWnd* pParent = NULL);
	~CDlgAffinityGroup(void);

	enum { IDD = IDD_DIALOG_AFFINITY_GROUPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog() ;
	BOOL OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	DECLARE_MESSAGE_MAP()

public:
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;

	afx_msg void OnNewToolBar() ;
	afx_msg void OnDelToolBar() ;

	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnSave();
	afx_msg void OnOK() ;
	afx_msg void OnCancel() ;

protected:
	void OnInitTree() ;
	void OnInitToolBar() ;

	HTREEITEM AddFltDataToTree(CAffinity_FlightTypeData* pFltData);
	HTREEITEM AddGroupDataToTree(CAffinity_GroupTypeData* pGroupData, HTREEITEM hRoot);
	HTREEITEM AddPaxDataToTree(CAffinity_PassengerTypeData* pPaxData, HTREEITEM hGroup);

	void OnNewAffinityFltType();
	void OnDelAffinityFltType();
	void OnEditAffinityFltType();

	void OnNewAffinityGroup();
	void OnDelAffinityGroup();

	void OnNewAffinityPaxType();
	void OnDelAffinityPaxType();
	void OnEditAffinityPaxType();

private:
	CCoolTree m_wndTreeCtrl;
	CToolBar m_wndToolbar;
	InputTerminal* m_pTerminal;
	CAffinityGroup* m_pAffinityGroup;


};
