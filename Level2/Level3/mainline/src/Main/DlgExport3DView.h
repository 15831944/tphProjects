#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgExport3DView dialog

class CDlgExport3DView : public CDialog
{
	DECLARE_DYNAMIC(CDlgExport3DView)

public:
	CDlgExport3DView(const CSize& viewSize, const CSize& maxSize, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgExport3DView();

	CString GetFileName() const { return m_strFileName; }
	int GetWidth() const { return m_nWidth; }
	int GetHeight() const { return m_nHeight; }

// Dialog Data
	enum { IDD = IDD_DIALOG_EXPORT_3DVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnBnClickedCheckLockRatio();
	afx_msg void OnDeltaposSpinWidth(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditWidth();

	DECLARE_MESSAGE_MAP()

protected:
	void SetLockRatioChecked(bool bChecked);
	void UpdateHeightInput( bool bChecked );

	CSpinButtonCtrl m_spnWidth;
	CSpinButtonCtrl m_spnHeight;
	CButton m_btnLockRatio;

	int m_nViewWidth;
	int m_nViewHeight;

	int m_nMaxWidth;
	int m_nMaxHeight;

	int m_nWidth;
	int m_nHeight;

	CString m_strFileName;

};
