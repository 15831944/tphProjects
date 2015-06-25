#pragma once
#include "../MFCExControl/CoolTree.h"

// DlgACTypeStandConstraint dialog

class ACTypeStandConstraintList;
class InputTerminal;
class DlgACTypeStandConstraint : public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgACTypeStandConstraint)

public:
	DlgACTypeStandConstraint(InputTerminal* pTerminal,int nAirportID,int nProjID,ACTypeStandConstraintList* pConstraints,CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgACTypeStandConstraint();

// Dialog Data
	enum { IDD = IDD_DLGACTYPESTANDCONSTRAINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void AddNewItem();
	afx_msg void RemoveItem();
	afx_msg void OnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedImportFromFile();
protected:
	void OnInitTreeCtrl();

	void UpdateToolbarStatus();
	void AddACTypes();
	void DelACType();
private:
	InputTerminal* m_pInputTerminal;
	CToolBar m_wndToolbar;
	CCoolTree m_wndTreeCtrl;
	HTREEITEM m_hRclickItem;
	ACTypeStandConstraintList* m_pACTypeStandConstraints;
	int m_nAirportID;
	int m_nProjID;

};
