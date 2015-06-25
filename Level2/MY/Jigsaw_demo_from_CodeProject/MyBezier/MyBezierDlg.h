
// MyBezierDlg.h : header file
//

#pragma once


// CMyBezierDlg dialog
class CMyBezierDlg : public CDialogEx
{
// Construction
public:
	CMyBezierDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MYBEZIER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDrawBezier();
private:
    POINT GetBezierPoint(POINT* oriPoints, int param2, int param3);
    int Factorial(int n);
    double GetBezierCoefficient(int nTotal, int nCur, int nPercent);
public:
    afx_msg void OnBnClickedButton1();
};
