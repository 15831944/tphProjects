#pragma once

#include "../MFCExControl/ListCtrlEx.h"

// CDlgTransitionTimeModifier dialog

class InputTerminal;
class CTransitionTimeModifiers;
class CTransitionTimeModifiersData;

class CDlgTransitionTimeModifier : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgTransitionTimeModifier)

public:
	CDlgTransitionTimeModifier(InputTerminal *pInputTerm,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTransitionTimeModifier();

// Dialog Data
	enum { IDD = IDD_DIALOG_TRANSITIONTIMEMODIFIER };

protected:
	CToolBar m_toolbar;
	CListCtrlEx m_lstData;
	CButton m_btnSave;
	InputTerminal *m_pInputTerminal;


	CTransitionTimeModifiers *m_pTransTimeModifier;
protected:
	void InitToolbar();
	void InitListCtrl();
	void LoadListData();
	void AddDataToList(CTransitionTimeModifiersData *pData);
	void UpdateToolbarState();
	void EnableSaveButton(BOOL bEnable);
	BOOL SaveData();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonDel();
	afx_msg void OnSave();
	afx_msg void OnSelChangePassengerType(NMHDR *pNMHDR, LRESULT *pResult);
};
