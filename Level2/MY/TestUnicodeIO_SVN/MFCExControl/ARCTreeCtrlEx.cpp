// ARCTreeCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ARCTreeCtrlEx.h"
#include "WinMsg.h"
#include "Resource.h"
// CARCTreeCtrlEx
#define IDC_COMBO_LEVEL                 1162
#define IDC_DATETIMEPICKER_TIME         1292
#define IDC_IPEDIT                      1225

IMPLEMENT_DYNAMIC(CARCTreeCtrlEx, CTreeCtrl)
CARCTreeCtrlEx::CARCTreeCtrlEx()
: m_iSpinEditType(0),
m_iNum(1),
m_nSpinEditLower(0),
m_nSpinEditUpper(100)
{
	m_nWidth = 100;	
	m_bEableEdit = FALSE;
}

CARCTreeCtrlEx::~CARCTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CARCTreeCtrlEx, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlEx)
	ON_WM_LBUTTONDBLCLK()

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// CARCTreeCtrlEx message handlers

CEdit* CARCTreeCtrlEx::SpinEditLabel(HTREEITEM hItem,CString sDisplayTitle)
{
	if (hItem == NULL) return NULL;

	m_hEditedItem = hItem;

	CString strTitle, strPercent;
	if( sDisplayTitle == "" )
		strTitle = GetItemText(hItem);
	else
		strTitle = sDisplayTitle;
	int iPercent = 0;
	int iH = 15;
	if( m_iSpinEditType == 0 )
	{
		// Set title and percent
		strTitle.Remove(')'); strTitle.Remove('%');
		int n = strTitle.ReverseFind('(');
		if (n >= 0)
		{
			strTitle.Remove('(');
			while(strTitle.GetLength() > n)
			{
				strPercent += strTitle.GetAt(n);
				strTitle.Delete(n);
			}

			strTitle.TrimLeft(); strTitle.TrimRight();
			strPercent.TrimLeft(); strPercent.TrimRight();
		}
		if (strPercent.GetLength() > 0)
		{
			iPercent = atoi(strPercent.GetBuffer(0));
		}



	}
	else if( m_iSpinEditType == 1 )
	{
		iPercent = m_iNum;
		strPercent.Format( "( %d )",m_iNum );
		strTitle = strTitle.Left( 20 );
	}
	else if( m_iSpinEditType == 2 )
	{
		iPercent = m_iNum;
		strPercent.Format( "( %d )",m_iNum );
		strTitle = strTitle.Left( 12 );
	}
	else if( m_iSpinEditType == 3 )
	{
		iPercent = m_iNum;
		strPercent.Format( "( %d )",m_iNum );
		strTitle = strTitle.Left( 15 );
	}
	else if( m_iSpinEditType == 4 )
	{
		//		strTitle = strTitle.Left( 10 );
		//		sDisplayTitle
		//		iPercent = m_iNum;
		//		strPercent.Format( "( %d%% )",m_iNum );
		//strTitle = strPercent;
		strPercent = sDisplayTitle; 
		iPercent = atoi(strPercent.GetBuffer(0));	
	}
	else if( m_iSpinEditType == 99 )
	{	
		iPercent = m_iNum;
		strPercent.Format( "( %d )",m_iNum );
	}

	if( iPercent < 10 )
		iH = 24;
	else if( iPercent < 100 )
		iH = 35;
	else 
		iH = 45;

	if( sDisplayTitle != "" )
	{
		//iH = sDisplayTitle.GetLength() *5;
		iH = sDisplayTitle.GetLength() *3 +15;
	}
	// Create or move it


	CRect rcItem;
	GetItemRect(hItem, rcItem, TRUE);


	if (!m_spinEdit.GetSafeHwnd())
	{
		m_spinEdit.Create(IDD_COOLTREE_EDITSPIN,this);
		m_spinEdit.SetParent(this);
	}

	m_spinEdit.SetWindowPos(NULL,rcItem.right,rcItem.top,0,0,SWP_NOSIZE);
	CString strValue;
	strValue.Format("%d",iPercent);
	m_spinEdit.m_editValue.SetWindowText(strValue);
	GetParent()->SendMessage(UM_CEW_EDITSPIN_BEGIN,(WPARAM)hItem,(LPARAM)(&m_spinEdit));
	m_spinEdit.SetSpinRange(m_nSpinEditLower, m_nSpinEditUpper );
	//	m_spinEdit.SetTitle(strTitle);	
	//	m_spinEdit.SetPercent(iPercent);

	//m_spinEdit.m_editValue.SetFocus();
	m_spinEdit.ShowWindow(SW_SHOW);
	//m_spinEdit.BringWindowToTop();

	return NULL;  // USER CAN NOT CHARGE IT.
	//	return CTreeCtrl::EditLabel(hItem);
}
LRESULT CARCTreeCtrlEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case UM_CEW_EDITSPIN_END:
		{
			SPINTEXT st;
			_tcscpy(st.szTitle, "");
			CString strValue=*((CString*)lParam);
			st.iPercent =atoi(strValue);

			GetParent()->SendMessage(WM_INPLACE_SPIN,0,(LPARAM)&st);
		}
		break;
	case WM_INPLACE_SPIN:
		{
			if (m_hEditedItem == NULL)
				return FALSE;

			LPSPINTEXT pst = (LPSPINTEXT) lParam;

			CString strItem, strPercent;
			strItem = pst->szTitle;
			strPercent.Format(" (%d", pst->iPercent);
			m_iNum = pst->iPercent;
			if( m_iSpinEditType == 0 )
			{	
				strItem += strPercent + "%)";
			}
			else 
			{
				strItem += strPercent +")";
			}


			SetItemText(m_hEditedItem, strItem);
			GetParent()->SendMessage( message, wParam, lParam );
			return TRUE;
		}
	case WM_INPLACE_DATETIME:
	case WM_INPLACE_EDIT:
		GetParent()->SendMessage( message, wParam, lParam );
		return TRUE;

	case WM_MOUSEMOVE:
		GetParent()->SendMessage( message, wParam, lParam );
		return TRUE;
	case WM_INPLACE_EDITSPIN:
		{
			GetParent()->SendMessage( message, wParam, lParam );
			return TRUE;
		}
	case WM_INPLACE_COMBO2:
		GetParent()->SendMessage(message,wParam,lParam);
	}
	return CTreeCtrl::DefWindowProc(message, wParam, lParam);
}


void CARCTreeCtrlEx::EditTime( HTREEITEM _hItem, COleDateTime _oTime )
{
	if( _hItem == NULL ) 
		return;

	m_hEditedItem = _hItem;

	CRect rcItem;
	GetItemRect(_hItem, rcItem, TRUE);
	rcItem.bottom = rcItem.top + 20;


	if (!m_inPlaceDateTimeCtrl.GetSafeHwnd())
		m_inPlaceDateTimeCtrl.Create( DTS_TIMEFORMAT, rcItem, this, IDC_DATETIMEPICKER_TIME );
	else
		m_inPlaceDateTimeCtrl.MoveWindow(&rcItem);

	m_inPlaceDateTimeCtrl.SetTime( _oTime );
	m_inPlaceDateTimeCtrl.ShowWindow(SW_SHOW);
	m_inPlaceDateTimeCtrl.BringWindowToTop();
	m_inPlaceDateTimeCtrl.SetFocus();
}
void CARCTreeCtrlEx::EditLabel(HTREEITEM _hItem)
{
	if( _hItem == NULL ) 
		return;

	m_hEditedItem = _hItem;

	CRect rcItem;
	GetItemRect(_hItem, rcItem, TRUE);
	rcItem.bottom = rcItem.top + 20;

	if (!m_inPlaceEdit.GetSafeHwnd())
		m_inPlaceEdit.Create( WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rcItem, this, IDC_IPEDIT );
	else
		m_inPlaceEdit.MoveWindow(&rcItem);

	m_inPlaceEdit.SetWindowText( GetItemText( _hItem ) );
	m_inPlaceEdit.ShowWindow(SW_SHOW);
	m_inPlaceEdit.BringWindowToTop();
	m_inPlaceEdit.SetFocus();
}



CEdit* CARCTreeCtrlEx::StringEditLabel(HTREEITEM hItem, CString sDisplayTitle)
{
	if (hItem == NULL) return NULL;

	m_hEditedItem = hItem;

	CString strTitle, strPercent;
	if( sDisplayTitle == "" )
		strTitle = GetItemText(hItem);
	else if ( sDisplayTitle == _T("NULL"))
		strTitle = _T("");
	else
		strTitle = sDisplayTitle;

	CRect rcItem;
	GetItemRect(hItem, rcItem, TRUE);


	if (!m_inPlaceEdit.GetSafeHwnd())
	{
		m_inPlaceEdit.Create( WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, CRect(10, 10, 100, 30), this, 1 );
	}

	m_inPlaceEdit.SetWindowPos(NULL,rcItem.right,rcItem.top,0,0,SWP_NOSIZE);


	m_inPlaceEdit.SetWindowText(strTitle);
	m_inPlaceEdit.SetFocus();

	m_inPlaceEdit.ShowWindow(SW_SHOW);


	return NULL;  // USER CAN NOT CHARGE IT.
}

void CARCTreeCtrlEx::SetEditableComboString( HTREEITEM _hItem,const std::vector<CString>& strlist )
{
	if( _hItem == NULL ) 
		return;

	m_hEditedItem = _hItem;
	CString itemstr = GetItemText(_hItem);

	CRect rcItem, rcEdit;
	GetItemRect(_hItem, rcItem, TRUE);



	int nHeight = rcItem.Height();
	rcItem.left += itemstr.GetLength()*6;
	rcItem.bottom += nHeight * 4;
	rcItem.right = rcItem.left + m_nWidth;

	if (m_comboBox.GetSafeHwnd())
	{
		DWORD dwStyle = m_comboBox.GetStyle();
		if ((dwStyle & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
			m_comboBox.DestroyWindow();
	}
	
	if (!m_comboBox.GetSafeHwnd())
	{

		DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWN|CBS_DISABLENOSCROLL;

		m_comboBox.Create( dwStyle, rcItem, this, IDC_COMBO_LEVEL );
		CFont* font = GetParent()->GetFont();
		m_comboBox.SetFont( font );
		m_comboBox.SetItemHeight( -1, nHeight );
		m_comboBox.SetHorizontalExtent( 300 );
		m_comboBox.MoveWindow(&rcItem);	

 		for( int m=0; m<int(strlist.size()); m++ ){					
 			m_comboBox.AddString( strlist[m] );
 		}
	}
	else
	{
		// first remove all string in the combobox
		m_comboBox.ResetContent();

		int nWidth = rcItem.right - rcItem.left;
 		for( int m=0; m<int(strlist.size()); m++ ){					
 			m_comboBox.AddString( strlist[m] );
 			int nWidth2 = m_comboBox.GetDC()->GetTextExtent(strlist[m]).cx;
 			if (nWidth2>nWidth)
 				nWidth = nWidth2;
 		}
		int nDropDownWidth = m_comboBox.GetDroppedWidth();
		m_comboBox.SetDroppedWidth(nWidth);

		m_comboBox.SetCurSel( -1 );
		m_comboBox.MoveWindow(&rcItem,TRUE);
	}

	m_comboBox.ShowWindow(SW_SHOW);
	m_comboBox.BringWindowToTop();
	m_comboBox.ShowDropDown();
	m_comboBox.SetFocus();
}

CEdit* CARCTreeCtrlEx::SetComboString(HTREEITEM _hItem,const std::vector<CString>& strlist ){
	if( _hItem == NULL ) 
		return NULL;




	m_hEditedItem = _hItem;
	CString itemstr = GetItemText(_hItem);

	CRect rcItem, rcEdit;
	GetItemRect(_hItem, rcItem, TRUE);



	int nHeight = rcItem.Height();
	rcItem.left += itemstr.GetLength()*6;
	rcItem.bottom += nHeight * 4;
	rcItem.right = rcItem.left + m_nWidth;

	if (m_comboBox.GetSafeHwnd())
	{
		DWORD dwStyle = m_comboBox.GetStyle();
		if ((dwStyle & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST)
			m_comboBox.DestroyWindow();
	}
	
		

	if (!m_comboBox.GetSafeHwnd())
	{

		DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;

		m_comboBox.Create( dwStyle, rcItem, this, IDC_COMBO_LEVEL );
		CFont* font = GetParent()->GetFont();
		m_comboBox.SetFont( font );
		m_comboBox.SetItemHeight( -1, nHeight );
		m_comboBox.SetHorizontalExtent( 300 );
		m_comboBox.MoveWindow(&rcItem);	

		for( int m=0; m<int(strlist.size()); m++ ){					
			m_comboBox.AddString( strlist[m] );
		}
	}
	else
	{
		// first remove all string in the combobox
		m_comboBox.ResetContent();

		int nWidth = rcItem.right - rcItem.left;
		for( int m=0; m<int(strlist.size()); m++ ){					
			m_comboBox.AddString( strlist[m] );
			int nWidth2 = m_comboBox.GetDC()->GetTextExtent(strlist[m]).cx;
			if (nWidth2>nWidth)
				nWidth = nWidth2;
		}
		m_comboBox.SetDroppedWidth(nWidth);

		m_comboBox.SetCurSel( -1 );
		m_comboBox.MoveWindow(&rcItem);
	}

	m_comboBox.ShowWindow(SW_SHOW);
	m_comboBox.BringWindowToTop();
	m_comboBox.ShowDropDown();
	m_comboBox.SetFocus();
	return NULL;
}
void CARCTreeCtrlEx::SetComboWidth(int nWidth)
{
	m_nWidth = nWidth;
}

void CARCTreeCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetKeyState(VK_CONTROL) & 0x8000 )
	{
		CTreeCtrl::OnLButtonDblClk(nFlags, point);
		return;
	}
	UINT flags;
	HTREEITEM hItem=HitTest(point, &flags);
	m_hEditedItem=hItem;
	CTreeCtrl::SelectItem(m_hEditedItem);
	if( hItem &&(flags & TVHT_ONITEMLABEL))
	{
		GetParent()->SendMessage(MODIFYVALUE,(WPARAM)m_hEditedItem,0);
		if (m_bEableEdit)
		{
			SpinEditLabel(this->GetSelectedItem());
		}
		return;
	}
	CTreeCtrl::OnLButtonDblClk(nFlags, point);	
}

void CARCTreeCtrlEx::SetSpinRange( int _nLower, int _nUpper )
{
	m_nSpinEditLower = _nLower; m_nSpinEditUpper = _nUpper;
}

void CARCTreeCtrlEx::SetDisplayNum( int _num )
{
	m_iNum = _num; /*m_spinEdit.SetPercent( _num );*/
}

void CARCTreeCtrlEx::SetDisplayType( int _type )
{
	m_iSpinEditType = _type ;/* m_spinEdit.SetDisplayType( _type );*/
}

