#pragma once
#include "afxwin.h"


// CDlgIdeticalNameSelect dialog


//------------------------------------------------------------
// This dialog is for identical name selection
// You passing a IdenticalNameList and the dialog title
// and I will give you the string and the index of it that selected
// 
// 
// 
//
//
//------------------------------------------------------------



class CDlgIdeticalNameSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgIdeticalNameSelect)

public:
	typedef std::vector<CString> IdenticalNameList;

	CDlgIdeticalNameSelect(const IdenticalNameList& nameList, CString strDialogTitle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgIdeticalNameSelect();

	CString GetSelName() const { return m_strSelName; }
	int SetSelIndex() const { return m_nSelIndex; }

// Dialog Data
	enum { IDD = IDD_DIALOG_IDENTICAL_NAME_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnLbnDblclkListName();

	DECLARE_MESSAGE_MAP()

private:
	const IdenticalNameList& m_nameList;
	CString m_strTitle;

	CListBox m_listNameList;

	CString m_strSelName;
	int     m_nSelIndex;
};
