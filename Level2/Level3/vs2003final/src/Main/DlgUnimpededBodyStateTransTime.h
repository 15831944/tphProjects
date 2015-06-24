#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgUnimpededBodyStateTransTime dialog

class InputTerminal;
class UnimpededStateTransTime;
class UnimpededStateTransTimePaxType;
class UnimpededStateTransTimePaxTypeData;
class CDlgUnimpededBodyStateTransTime : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgUnimpededBodyStateTransTime)

public:
	CDlgUnimpededBodyStateTransTime(InputTerminal *pInputTerminal, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUnimpededBodyStateTransTime();

// Dialog Data
	enum { IDD = IDD_DIALOG_UNIMPEDEDSTATETRANSTIME };
protected:
	InputTerminal *m_pInputTerminal;
	UnimpededStateTransTime* m_pUnimpededData;
protected:
	CToolBar m_toolBar;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void InitToolbar();
	void InitStateList();
	void LoadPaxTypeList();
	void LoadStateList();
	void SetStateValue(UnimpededStateTransTimePaxTypeData *pData);
	void AddPaxType(UnimpededStateTransTimePaxType *pPaxType);


	void UpdateToolbarState();
	bool SaveData();

	void EnableSaveButton(BOOL bShow);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAddPaxType();
	afx_msg void OnEditPaxType();
	afx_msg void OnDelePaxType();
	CListBox m_lstBoxPaxType;
	CListCtrl m_lstData;
	CButton m_btnSave;
	afx_msg void OnLbnSelchangeListPaxtype();
	afx_msg void OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSave();
};
