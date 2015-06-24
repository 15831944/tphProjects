#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CInputValueDlg �Ի���

class CInputValueDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputValueDlg)

public:
	CInputValueDlg(CString strPrompt,long nValue,long nMin=1,long nMax=1000,BOOL bNotDragWnd=FALSE,CString strTitle=_T("Input"),CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputValueDlg();

// �Ի�������
	enum { IDD = IDD_INPUT_VALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strTitle;
	CString m_strPrompt;
	long m_nValue;
	long m_nMin;
	long m_nMax;
	virtual BOOL OnInitDialog();
	CEdit m_editValue;
	afx_msg void OnBnClickedOk();
	CSpinButtonCtrl m_spinValue;
	afx_msg UINT OnNcHitTest(CPoint point);
	BOOL m_bNotDragWnd;
};
