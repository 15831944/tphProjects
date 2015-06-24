#pragma once


// CDlgMobElementsDiagnosis dialog
#include "MFCExControl/ARCToolbar.h"

class CDlgMobElementsDiagnosis : public CDialog
{
	DECLARE_DYNAMIC(CDlgMobElementsDiagnosis)

public:
	CDlgMobElementsDiagnosis(CString strProjPath, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMobElementsDiagnosis();

	int			mDisableAll;
	CListBox	m_wndListBox;
	CEdit		m_wndEditBox;

	CImageList  m_ImageList;
	CARCToolbar m_wndARCToolbar;
	CString		m_strProjPath; 

	void LoadMobileElementsID();
private:
	void UpdateUIState();
	void EditItem( int _nIdx );

	std::vector< std::string > mMobElementIdList;
	int m_nSelIdx;
// Dialog Data
	enum { IDD = IDD_DIALOG_DIAGNOSIS_MOBELEMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnAddItem();
	virtual void OnEditItem();
	virtual void OnDeleteItem();

	virtual BOOL OnInitDialog();

	afx_msg void OnDblclkListBox();
	afx_msg LONG OnEndEdit( WPARAM p_wParam, LPARAM p_lParam );
	afx_msg void OnSelchangeListBox();
	afx_msg void OnEnKillfocusEditMobElementID();

	afx_msg void OnBnClickedOpen();

	virtual void OnOK();
};
