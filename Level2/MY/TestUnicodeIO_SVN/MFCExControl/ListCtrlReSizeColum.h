#pragma once
#include "XListCtrl.h"
//add by cj
class MFCEXCONTROL_API CReSizeColumnListCtrl : public CListCtrl
{
public:
	CReSizeColumnListCtrl() ;
	virtual ~CReSizeColumnListCtrl() ;
	void ResizeColumnWidth() ;
	void InitColumnWidthPercent() ;
	int GetColumnCount();
public: 
	float m_percent[1024] ;
};
class MFCEXCONTROL_API CSortListCtrlEx : public CReSizeColumnListCtrl
{
public:
	CSortListCtrlEx();
	BOOL m_fAsc;//
	int m_nSortedCol;//
public:
	virtual ~CSortListCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSortListCtrl)
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void PrintHead(CDC* pDC,int nPageWidth ,int nPageHMargin,int y) ;
public:
	void PrintList( CString _sHeader, CString _sFoot ,CString _title);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintableListCtrl)
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
private:
	CString m_sHeader;
	CString m_sFoot;
    void CutStringForPrint(int _x,int _nextX ,TCHAR* _val,int _charwidth,int& cutlen) ;
	void PrintMutiLine(CDC* pDC,int _x,int _nextX ,int _y ,TCHAR* _val,int _charwidth,int _charHight,int& lines);
  // void PrintTop(CDC* pDC,TEXTMETRIC metrics,CFont FooterFont,CFont HeadingFont,CFont TitleFont,int& y);
};