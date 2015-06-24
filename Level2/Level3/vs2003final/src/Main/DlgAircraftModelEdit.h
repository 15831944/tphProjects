#pragma once
#include "afxcmn.h"

// CDlgAircraftModelEdit dialog
class InputOnboard;
class CDlgAircraftModelEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgAircraftModelEdit)

public:
	CDlgAircraftModelEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftModelEdit();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRCRAFTMODELEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual	BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnSLBSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTree(NMHDR* pNMHDR, LRESULT* pResult);

public:
	void LoadAircraftData();
	void SetInputOnboard(InputOnboard* pInputOnboard);
protected:
	InputOnboard * m_pInputOnboard;
	CImageList m_image;

	HTREEITEM m_hItemDragSrc;
	CImageList*   m_pDragImage;
	BOOL m_bDragging;
	CPoint		m_ptLastMouse;
	CWnd*		m_pParent;
	CTreeCtrl m_wndTreeCtrl;
};
