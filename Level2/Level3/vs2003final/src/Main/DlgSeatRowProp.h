#pragma once
#include "afxwin.h"
#include <common/ARCVector.h>
#include <common/ARCPath.h>

// CDlgSeatRowProp dialog

class CDlgSeatRowProp : public CDialog
{
	DECLARE_DYNAMIC(CDlgSeatRowProp)

public:
	CDlgSeatRowProp(const CString& rowName, const ARCPath3& rowPath , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSeatRowProp();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEATROWDEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();	
	virtual BOOL OnInitDialog();
	int m_nRowCount;
	CString m_sRowName;		

	ARCPath3 mDestPath;
	afx_msg void OnBnClickedButtonPickline();

	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	LRESULT OnTempFallbackFinished(WPARAM wParam, LPARAM lParam);

};
