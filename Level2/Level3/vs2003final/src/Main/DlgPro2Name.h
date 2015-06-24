#pragma once


// CDlgPro2Name dialog
class CTermPlanDoc;
class CProcessor2;
class CDlgPro2Name : public CDialog
{
	DECLARE_DYNAMIC(CDlgPro2Name)

public:
	CDlgPro2Name(CProcessor2* pProc2,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPro2Name();

// Dialog Data
	enum { IDD = IDD_DLGPRO2NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CProcessor2* m_pProc2;

public:
	CComboBox m_wndComboxLevel1;
	CComboBox m_wndComboxLevel2;
	CComboBox m_wndComboxLevel3;
	CComboBox m_wndComboxLevel4;

	CString	m_csLevel1;
	CString	m_csLevel2;
	CString	m_csLevel3;
	CString	m_csLevel4;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnDropdownComboLevel2();
	afx_msg void OnDropdownComboLevel3();
	afx_msg void OnDropdownComboLevel4();
private:
	BOOL IsNameChanged(CString newName);
	void LoadCombo();
	CTermPlanDoc* GetTermPlanDoc();
};
