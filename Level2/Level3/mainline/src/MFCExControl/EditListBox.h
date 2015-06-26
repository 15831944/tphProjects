#pragma once
#include "EditHelper.h"

// CEditListBox

static const int WM_LBITEMCHANGED = WM_USER + 174;

class MFCEXCONTROL_API CEditListBox : public CListBox
{
	DECLARE_DYNAMIC(CEditListBox)

public:
	CEditListBox();
	virtual ~CEditListBox();

	void EditItem(int nIndex);

	int DeleteString( UINT nIndex );


protected:
	DECLARE_MESSAGE_MAP()

	CEditHelper m_wndInplaceEdit;
	int m_iEditingItem;

	afx_msg void OnLbnDblclk();
	afx_msg LRESULT OnEnKillfocus(WPARAM wParam, LPARAM lParam);
};


