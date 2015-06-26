#include "StdAfx.h"
//#include "termplan.h"
#include "InPlaceComboBox2.h"
#include "WinMsg.h"
#include ".\inplacecombobox2.h"


CInplaceComboBox2::CInplaceComboBox2(void)
{
}

CInplaceComboBox2::~CInplaceComboBox2(void)
{
}

BEGIN_MESSAGE_MAP(CInplaceComboBox2, CARCTipComboBox)
	//ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillComboFocus)
END_MESSAGE_MAP()

void CInplaceComboBox2::OnKillFocus(CWnd* pNewWnd)
{
	CARCTipComboBox::OnKillFocus(pNewWnd);

	
	int nSelIdx = GetCurSel();

	ShowWindow(SW_HIDE);
	GetParent()->SendMessage(WM_INPLACE_COMBO2, -1, (LPARAM)nSelIdx);

	// TODO: Add your message handler code here
}

void CInplaceComboBox2::OnKillComboFocus()
{
	int nSelIdx = GetCurSel();

	ShowWindow(SW_HIDE);
	GetParent()->SendMessage(WM_INPLACE_COMBO2, -1, (LPARAM)nSelIdx);
}
