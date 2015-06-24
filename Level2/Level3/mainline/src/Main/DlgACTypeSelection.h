#pragma once
#include "../Common/ACTypesManager.h"
#include "../MFCExControl/SimpleToolTipListBox.h"


// DlgACTypeSelection dialog

class DlgACTypeSelections : public CDialog
{
	DECLARE_DYNAMIC(DlgACTypeSelections)

public:
	DlgACTypeSelections(InputTerminal *pTerm, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgACTypeSelections();

// Dialog Data
	enum { IDD = IDD_DLGACTYPESELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
		void InitListBox();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	const CStringList& getSelACTyeps();
private:
	CSimpleToolTipListBox m_wndListBox;
	CStringList m_strSelACTypes;
	CACTypesManager *m_pACManager;
};
