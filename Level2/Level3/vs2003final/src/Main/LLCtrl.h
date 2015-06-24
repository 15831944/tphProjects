#pragma once
#include "afxwin.h"

#include "XPStyle/ComboBoxXP.h"


typedef struct tagIPA_ADDR
{
	unsigned char	nAddr1;	
	unsigned char	nAddr2;
	unsigned char	nAddr3;
	unsigned char	nAddr4;
	UINT	nInError;		
} IPA_ADDR;


#define	IPAS_NOVALIDATE	0x0001	

// Messages 
enum IPAM 
{
	IPAM_GETADDRESS = WM_USER,
	IPAM_SETADDRESS,
	IPAM_SETREADONLY
};

enum RID 
{
	IDC_ADDR1 = 1,
	IDC_ADDR2,
	IDC_ADDR3,
	IDC_ADDR4,
	IDC_ADDR5
};

class CLLCtrl;

class CLLEdit :public CEdit
{
	// Construction
public:
	CLLEdit();

	// Attributes
public:
	friend class CLLCtrl;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPAddrEdit)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CLLEdit();
	int m_nMax;

	// Generated message map functions
protected:
	CLLCtrl* m_pParent;
	void SetParent(CLLCtrl* pParent);
	void SetMax(int nMax);
	//{{AFX_MSG(CIPAddrEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChange();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


class CLLCtrl :public CWnd
{
public:
	CLLCtrl(void);
	~CLLCtrl(void);


	friend class CLLEdit;
	CLLEdit m_Addr[4];
	CComboBoxXP m_combo;

	CRect	m_rcAddr[4];
	CRect	m_rcDot[3];

	CRect m_rcEdit[4];
	int nFlag;
public :
	long GetCtrlValue();
	void SetComboBoxItem(CString strItem);
	
	//if (n1 ,n2 n3) 
	void SetItemRange(int n1,int n2=0 ,int n3=0);

	void SetCtrlText(CString strText);

	void GetCtrlText(CString &strText);

	int SplitString(LPCTSTR lpszString,int nStrLen,LPCTSTR lpszSep,CStringArray& strArray);

	int SplitString(LPCTSTR lpszString,int nStrLen,int chSep,CStringArray& strArray);

protected:
	void OnChildChar(UINT nChar, UINT nRepCnt, UINT nFlags, CLLEdit& child);


	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnEnSetfocus();
	afx_msg void OnPaint();
	afx_msg void OnKillFocusCombo();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocusEdit1();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
