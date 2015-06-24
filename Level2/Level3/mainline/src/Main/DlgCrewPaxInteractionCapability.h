#pragma once
#include "../MFCExControl/CoolTree.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgCrewPaxInteractionCapability dialog
class CabinCrewGeneration;
class CCrewPaxInteractionCapabilityList;
class InputTerminal;
class CDlgCrewPaxInteractionCapability : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgCrewPaxInteractionCapability)

public:
	CDlgCrewPaxInteractionCapability(InputTerminal* pTerminal, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCrewPaxInteractionCapability();

// Dialog Data
	enum { IDD = IDD_DLGCREWPAXINTERACTIONCAPABILITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void AddNewItem();
	afx_msg void RemoveItem();
	afx_msg void EditItem();

	afx_msg void OnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
protected:
	void AddRankItem();
	void RemoveRankItem();

	void AddCrewItem();
	void RemoveCrewItem();

	void AddPaxType();
	void RemovePaxType();

	void OnInitTreeCtrl();
private:
	InputTerminal* m_pInputTerminal;
	CToolBar m_wndToolbar;
	CCoolTree m_wndTreeCtrl;
	HTREEITEM m_hRclickItem;
	CCrewPaxInteractionCapabilityList* m_pCrewPaxInteractionCapabilitys;
	CabinCrewGeneration* m_pCabinCrewGeneration;

};
