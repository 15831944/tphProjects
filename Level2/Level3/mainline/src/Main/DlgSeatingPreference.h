#pragma once
#include "../MFCExControl/ListCtrlEx.h"
#include "../InputOnBoard/SeatingPreference.h"
#include "../MFCExControl/XTResizeDialog.h"
class InputTerminal;

// CDlgSeatingPreference dialog

class CDlgSeatingPreference : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSeatingPreference)

public:
	CDlgSeatingPreference(InputTerminal * pInterm,int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSeatingPreference();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEATINGPREFERENCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

protected:
	void OnInitToolbar();
	void OnInitListCtrl();
	void DisplayData();
	void OnUpdateToolbar();
	SeatType getSeatTypeFromString(CString szData);

	afx_msg void OnAddPaxType();
	afx_msg void OnRemovePaxType();
	afx_msg void OnEditPaxType();
	afx_msg void OnSave();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnDBClick(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelChangePassengerType(NMHDR *pNMHDR, LRESULT *pResult);

private:
	InputTerminal * m_pInterm;
	CSeatingPreferenceList m_seatingPreferenceList;
	CToolBar  m_wndToolbar;
	CListCtrlEx m_wndListCtrl;
	int m_nPorjID;
};
