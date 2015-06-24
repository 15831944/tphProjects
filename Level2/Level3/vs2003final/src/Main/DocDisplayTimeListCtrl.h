#pragma once
class InputTerminal;
#include "../MFCExControl/ListCtrlEx.h"
class CDocDisplayTimeListCtrl :
	public CListCtrlEx
{
public:
	CDocDisplayTimeListCtrl(void);
	virtual ~CDocDisplayTimeListCtrl(void);

protected:
	int m_nForceItemData;
	InputTerminal* m_pInTerm;
	float m_percent[1024] ;
public:
	//{{AFX_MSG(CConDBListCtrl)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
void ResizeColumnWidth();
void InitColumnWidthPercent() ;
//	CPtrArray ddStyleList;
	//}}AFX_MSG
public :
	void SetInputTerminal(InputTerminal* _terminal) { m_pInTerm = _terminal ;} ;
protected:
DECLARE_MESSAGE_MAP()
};
