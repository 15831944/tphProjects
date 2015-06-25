#pragma once
#include "afxcmn.h"
#include "../InputAirside/HoldShortLines.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgHoldShortLineSel dialog

class CDlgHoldShortLineSel : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgHoldShortLineSel)

public:
	CDlgHoldShortLineSel(TaxiInterruptLineList* _holdshortLines , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgHoldShortLineSel();

// Dialog Data
	enum { IDD = IDD_DIALOG_SHORTLINE_SEL };
protected:
	TaxiInterruptLineList* m_holdshortLines ; 
	CTaxiInterruptLine* m_SelShortLine ; 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CTreeCtrl m_treeCtrl;
	CButton m_button_Sel ;
public:
	afx_msg void OnTvnSelchangedTreeHoldShortline(NMHDR *pNMHDR, LRESULT *pResult);
    CTaxiInterruptLine* GetSelHoldShortLine() ;
protected:
	BOOL OnInitDialog() ;
	void OnSize(UINT nType, int cx, int cy) ;
	void InitTree() ;
	void AddItem(CTaxiInterruptLine* _lineNode ) ;
	void OnOK() ;
};
