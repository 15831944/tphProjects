#pragma once

#include "MySpin.h"
#include "InPlaceDateTimeCtrl.h"	// Added by ClassView
#include "InPlaceComboBox.h"	// Added by ClassView
#include "InPlaceEdit2.h"

#include "CoolTreeEditSpin.h"
#include "InplaceComboBox2.h"
// CARCTreeCtrlEx

#define MODIFYVALUE				WM_USER + 1000

class MFCEXCONTROL_API CARCTreeCtrlEx : public CTreeCtrl
{
	DECLARE_DYNAMIC(CARCTreeCtrlEx)

public:
	CARCTreeCtrlEx();
	virtual ~CARCTreeCtrlEx();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlEx)
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//}}AFX_VIRTUAL
public:
	void EditLabel( HTREEITEM _hItem );
	void EditTime( HTREEITEM _hItem, COleDateTime _oTime );

	CEdit* SpinEditLabel( HTREEITEM hItem ,CString sDisplayTitle="" );
	void SetDisplayType( int _type );;
	void SetDisplayNum ( int _num );;
	void SetSpinRange( int _nLower, int _nUpper );;

	//Combo box
	CEdit* SetComboString(HTREEITEM _hItem,const std::vector<CString>& strlist );

	void SetEditableComboString(HTREEITEM _hItem,const std::vector<CString>& strlist );

	void SetComboWidth(int nWidth);


public:
	CEdit* StringEditLabel(HTREEITEM hItem, CString sDisplayTitle="");
	CInPlaceEdit2 m_inPlaceEdit;
	CInPlaceComboBox m_inPlaceComboBox;
	CInPlaceDateTimeCtrl m_inPlaceDateTimeCtrl;
	CCoolTreeEditSpin m_spinEdit;
	HTREEITEM m_hEditedItem;
	int m_iSpinEditType;
	int m_iNum;
	int m_nSpinEditLower;
	int m_nSpinEditUpper;

	CInplaceComboBox2 m_comboBox;
	int m_nComboSelectItem;
	int m_nWidth;
	BOOL m_bEableEdit;
protected:

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};


