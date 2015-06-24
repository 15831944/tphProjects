#pragma once
#include "TreeCtrlEx.h"
#include <CommonData/Fallback.h>
#include "afxwin.h"
#include <common/ALTObjectID.h>
#include "AirsideObjectTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"
class CAircraftElement;
class CAircraftPlacements;

class CTermPlanDoc;
class CDlgOnboardElementProp: public CXTResizeDialog
{

	DECLARE_DYNAMIC(CDlgOnboardElementProp)
public:
	enum { IDD = IDD_DIALOG_OBJECTAIRSIDE };

	CDlgOnboardElementProp(CAircraftElement* pElement, CWnd* pParent = NULL);
	virtual ~CDlgOnboardElementProp(void);

	ALTObjectIDList m_lstExistObjectName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);
	virtual FallbackReason GetFallBackReason() = 0;
	virtual bool UpdateOtherData() = 0;
	virtual bool DoTempFallBackFinished(WPARAM wParam, LPARAM lParam);
	HTREEITEM m_hRClickItem;
	void AdjustCombox(int nWidth);
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	void InitComboBox();
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_cmbName1;
	CComboBox m_cmbName2;
	CComboBox m_cmbName3;
	CComboBox m_cmbName4;
	ALTObjectID m_objName;
	bool m_bNameModified;
	CAirsideObjectTreeCtrl m_treeProp;
	//except In constraint, out constraint, service location
	bool m_bPropModified;
	bool m_bPathModified;
	bool m_bTypeModify;
	afx_msg void OnCbnKillfocusComboNameLevel1();
	afx_msg void OnCbnDropdownComboNameLevel1();
	afx_msg void OnCbnDropdownComboNameLevel2();
	afx_msg void OnCbnDropdownComboNameLevel3();
	afx_msg void OnCbnDropdownComboNameLevel4();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnProcpropPickfrommap();
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	afx_msg LRESULT OnPropTreeDoubleClick(WPARAM, LPARAM);
	afx_msg void OnProcpropDelete();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

	CTermPlanDoc * GetDocument();
	virtual void UpdateTempObjectInfo();
	virtual void DoneEditTempObject();
public:
	CStatic m_ImageStatic;

};
