#if !defined(AFX_VEHICLEDISTLISTCTRL_H__1991D569_B0BF_4C0E_A900_9FC0B0878FD6__INCLUDED_)
#define AFX_VEHICLEDISTLISTCTRL_H__1991D569_B0BF_4C0E_A900_9FC0B0878FD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxDistListCtrl.h : header file
//
#include "../InputAirside/VehicleDispPropItem.h"
#include "InPlaceComboBoxEx.h"
#include "BmpBtnDropList.h"
/////////////////////////////////////////////////////////////////////////////
// CPaxDispListCtrl window
#define IDC_BUTTON_DOWNLIST 0

#define VEHICLE_DISP_PROP_CHANGE1     (WM_USER + 12019)
#define VEHICLE_DISP_PROP_CHANGE2     (WM_USER + 12020)

class CVehicleDispPropListCtrl : public CListCtrl
{
	// Construction
public:
	CVehicleDispPropListCtrl();

	// Attributes
public:

protected:
	int m_nShapeSelItem;
	int m_nLineSelItem;
	int m_nVehicleItem;
	CImageList m_shapesImageList;
	CImageList m_linesImageList;
	CInPlaceComboBoxEx m_vehicleTypeInPlaceComboBox;
	CInPlaceComboBoxEx m_linesInPlaceComboBox;
	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;
	//	HBITMAP m_hBitmapShape;
	CBmpBtnDropList m_bbDropList;

	int m_nFlag_WM_INPLACE_COMBO;
	// Operations
public:

protected:
	void GetColorBoxRect(CRect& r);
	void GetVisibleRect(CRect& r);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVehicleDispPropListCtrl)
public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
public:
	void InsertPaxDispItem( int _nIdx, CVehicleDispPropItem* _pItem );
	virtual ~CVehicleDispPropListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVehicleDispPropListCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VEHICLEDISTLISTCTRL_H__1991D569_B0BF_4C0E_A900_9FC0B0878FD6__INCLUDED_)
