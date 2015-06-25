#pragma once


// CEditTreeCtrl

class AIRSIDEGUI_API CEditTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CEditTreeCtrl)

public:
	CEditTreeCtrl();
	virtual ~CEditTreeCtrl();
	long GetNumEditValue(void);
	void SetNumEditValue(long lValue);
	void ShowEdit(BOOL bShow = TRUE);
	
	static UINT WM_PRESHOWEDIT;
	static UINT WM_ENDSHOWEDIT;
protected:
	CEdit m_wndEdit;
	BOOL m_bShowEdit;
	BOOL m_bSendMsg;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
};
