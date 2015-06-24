#if !defined(AFX_VEHICLETAGLISTCTRL_H__1991D569_B0BF_4C0E_A900_9FC0B0878FD6__INCLUDED_)
#define AFX_VEHICLETAGLISTCTRL_H__1991D569_B0BF_4C0E_A900_9FC0B0878FD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxDistListCtrl.h : header file
//
#include "../InputAirside/VehicleTagItem.h"
#include "InPlaceComboBoxEx.h" 
class CLandsideVehicleTypes;
/////////////////////////////////////////////////////////////////////////////
// CVehicleTagListCtrl window 
class CVehicleTagListCtrl : public CListBox
{
	// Construction
public:
	CVehicleTagListCtrl(void);

	// Attributes
public:
	void SetProjID(int nProjID);
protected: 
	int m_nVehicleItem;
	int m_nProjID;
	CInPlaceComboBoxEx m_vehicleTypeInPlaceComboBox;
	int m_nFlag_WM_INPLACE_COMBO;
	// Operations
public:
 
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVehicleTagListCtrl)
public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
public:
	void InsertVehicleTagItem( int _nIdx, CVehicleTagItem* _pItem );
	virtual ~CVehicleTagListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVehicleTagListCtrl) 
	//afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult); 
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class CLandsideVehicleTypeListCtrl : public CListBox
{
	// Construction
public:
	CLandsideVehicleTypeListCtrl(void);

	// Attributes
public:
	void SetProjID(int nProjID);
protected: 
	int m_nVehicleItem;
	int m_nProjID;
	CInPlaceComboBoxEx m_vehicleTypeInPlaceComboBox;
	int m_nFlag_WM_INPLACE_COMBO;
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVehicleTagListCtrl)
public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
public:
	void InsertVehicleTagItem( int _nIdx, CLandsideVehicleTypes* _pItem );
	virtual ~CLandsideVehicleTypeListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVehicleTagListCtrl) 
	//afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult); 
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VEHICLETAGLISTCTRL_H__1991D569_B0BF_4C0E_A900_9FC0B0878FD6__INCLUDED_)
