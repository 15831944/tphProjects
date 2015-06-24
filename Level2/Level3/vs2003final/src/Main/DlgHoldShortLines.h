#pragma once 
#include <InputAirside/HoldShortLines.h>
#include <MFCExControl/ListCtrlEx.h>

class IntersectionNode;

class IntersectionNodeDistCompareInTaxiway
{
public:
	IntersectionNodeDistCompareInTaxiway(int nTaxiID){ m_nTaxiId = nTaxiID; }
	bool operator()(IntersectionNode& Node1, IntersectionNode& Node2) const;
protected:
	int m_nTaxiId;
};

/////////////////////////////////////////////////////////
enum ARCUnit_Length;
class CTaxiInterruptLine;

class CDlgHoldShortLines : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgHoldShortLines)
public:
	CDlgHoldShortLines(int nProjID,CWnd * pParent = NULL);
	virtual ~CDlgHoldShortLines(void);

	enum { IDD = IDD_HOLDSHORTLINE };

protected:
	int m_nProjID;
	TaxiInterruptLineList m_holdShortLines;
	CTaxiInterruptLine * m_pSelHoldShortLineNode; 
	CListCtrlEx	m_wndListCtrl;
	CToolBar	m_wndToolBar;
	int m_nRow;
	int m_nColumn;
protected:
	void InitToolbar(void);
	void SetListCtrlContent(void);
	void InitListCtrl(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void);
	afx_msg void OnNewItem(void);
	afx_msg void OnDeleteItem(void);
	afx_msg void OnEditItem(void);
	virtual void OnOK(void);
	virtual void OnCancel(void);	
	afx_msg void OnSave(void);
	DECLARE_MESSAGE_MAP()
protected:
	ARCUnit_Length GetCurUnit();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMRclickListHoldShort(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEndEditLabelListHoldShort(NMHDR *pNMHDR, LRESULT *pResult);
	void OnProcpropPickfrommap();
	void ShowDialog(CWnd* parentWnd);
	void HideDialog(CWnd* parentWnd);
	LRESULT OnTempFallbackFinished(WPARAM wParam, LPARAM lParam);
};