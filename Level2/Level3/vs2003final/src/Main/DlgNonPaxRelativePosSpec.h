#pragma once

#include ".\NonPaxRelativePosEditCtrl.h"
#include "afxwin.h"
#include "afxcmn.h"



// CDlgNonPaxRelativePosSpec dialog


class InputTerminal;
class CTermPlanDoc;
class CMobileElemTypeStrDB;
class CNonPaxRelativePosSpec;


class CNonPaxListCtrl: public CListCtrl
{
public:
	CNonPaxListCtrl();
	virtual ~CNonPaxListCtrl();
	
	void InsertItem(int _nIdx,int nTypeIndex);
	void SetItemCheck(int nItem,BOOL bCheck);
	void SetNonPaxPosSpec(CNonPaxRelativePosSpec* pPosSpec){m_pPosSpec = pPosSpec;}
	void SetMobileElemTypeStrDB(CMobileElemTypeStrDB* pMobElemTypeDB) {m_pMobElemTypeDB = pMobElemTypeDB;}
protected:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	BOOL GetItemCheck(int nItem);
protected:
	afx_msg LRESULT OnCheckStateChanged( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
protected:
	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;
	CNonPaxRelativePosSpec* m_pPosSpec;
	CMobileElemTypeStrDB* m_pMobElemTypeDB;
};

class CDlgNonPaxRelativePosSpec : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgNonPaxRelativePosSpec)

public:
	CDlgNonPaxRelativePosSpec(CTermPlanDoc* pDoc , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNonPaxRelativePosSpec();


	CMobileElemTypeStrDB* GetTypeDB()const;
	CNonPaxRelativePosSpec* GetPosSpec()const;
	void InitListCtrlHead();
	void SetListCtrlContent();
	void UpdateListCtrl();

	CNonPaxRelativePosEditCtrl m_editPosCtrl;

	CTermPlanDoc* mpDoc;
// Dialog Data
	enum { IDD = IDD_DIALOG_NONPAXPOS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnLvnBegindragListRelative(NMHDR *pNMHDR, LRESULT *pResult);


	CImageList* m_pDragImage;	//For creating and managing the drag-image
	BOOL		m_bDragging;	//T during a drag operation
	int m_nDragIndex;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CNonPaxListCtrl m_nonPaxListCtrl;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedRadioTop();
	afx_msg void OnBnClickedRadioLeft();
	afx_msg void OnBnClickedRadioFront();
	afx_msg void OnBnClickedRadioPers();
	afx_msg void OnBnClickedCancel();
	LRESULT OnDeletePosItem(WPARAM,LPARAM);
};
