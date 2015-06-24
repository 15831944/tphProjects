// TreeCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TreeCtrlEx.h"
#include "..\common\Probdistmanager.h"
#include "common\WinMsg.h"
#include "DlgProbDist.h"
#include "inputs\in_term.h"
#include "..\inputs\probab.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx

CTreeCtrlEx::CTreeCtrlEx( void )
{
	m_pInTerm = NULL;
}

CTreeCtrlEx::~CTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CARCTreeCtrlEx)
	//{{AFX_MSG_MAP(CTreeCtrlEx)
	//ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx message handlers


//CEdit* CTreeCtrlEx::SpinEditLabel(HTREEITEM hItem,CString sDisplayTitle)
//{
//	if (hItem == NULL) return NULL;
//	
//	m_hEditedItem = hItem;
//	
//	CString strTitle, strPercent;
//	if( sDisplayTitle == "" )
//		strTitle = GetItemText(hItem);
//	else
//		strTitle = sDisplayTitle;
//	int iPercent = 0;
//	int iH = 15;
//	if( m_iSpinEditType == 0 )
//	{
//		// Set title and percent
//		strTitle.Remove(')'); strTitle.Remove('%');
//		int n = strTitle.ReverseFind('(');
//		if (n >= 0)
//		{
//			strTitle.Remove('(');
//			while(strTitle.GetLength() > n)
//			{
//				strPercent += strTitle.GetAt(n);
//				strTitle.Delete(n);
//			}
//			
//			strTitle.TrimLeft(); strTitle.TrimRight();
//			strPercent.TrimLeft(); strPercent.TrimRight();
//		}
//		if (strPercent.GetLength() > 0)
//		{
//			iPercent = atoi(strPercent.GetBuffer(0));
//		}
//
//			
//
//	}
//	else if( m_iSpinEditType == 1 )
//	{
//		iPercent = m_iNum;
//		strPercent.Format( "( %d )",m_iNum );
//		strTitle = strTitle.Left( 20 );
//	}
//	else if( m_iSpinEditType == 2 )
//	{
//		iPercent = m_iNum;
//		strPercent.Format( "( %d )",m_iNum );
//		strTitle = strTitle.Left( 12 );
//	}
//	else if( m_iSpinEditType == 3 )
//	{
//		iPercent = m_iNum;
//		strPercent.Format( "( %d )",m_iNum );
//		strTitle = strTitle.Left( 15 );
//	}
//	else if( m_iSpinEditType == 4 )
//	{
////		strTitle = strTitle.Left( 10 );
////		sDisplayTitle
////		iPercent = m_iNum;
////		strPercent.Format( "( %d%% )",m_iNum );
//		//strTitle = strPercent;
//		strPercent = sDisplayTitle; 
//        iPercent = atoi(strPercent.GetBuffer(0));	
//	}
//	else if( m_iSpinEditType == 99 )
//	{	
//		iPercent = m_iNum;
//		strPercent.Format( "( %d )",m_iNum );
//	}
//
//	if( iPercent < 10 )
//		iH = 24;
//	else if( iPercent < 100 )
//		iH = 35;
//	else 
//		iH = 45;
//
//	if( sDisplayTitle != "" )
//	{
//		//iH = sDisplayTitle.GetLength() *5;
//		iH = sDisplayTitle.GetLength() *3 +15;
//	}
//	// Create or move it
//
//
//	CRect rcItem;
//	GetItemRect(hItem, rcItem, TRUE);
//	
//	
//	if (!m_spinEdit.GetSafeHwnd())
//	{
//		m_spinEdit.Create(IDD_COOLTREE_EDITSPIN,this);
//		m_spinEdit.SetParent(this);
//	}
//
//	m_spinEdit.SetWindowPos(NULL,rcItem.right,rcItem.top,0,0,SWP_NOSIZE);
//	CString strValue;
//	strValue.Format("%d",iPercent);
//	m_spinEdit.m_editValue.SetWindowText(strValue);
//	GetParent()->SendMessage(UM_CEW_EDITSPIN_BEGIN,(WPARAM)hItem,(LPARAM)(&m_spinEdit));
//	m_spinEdit.SetSpinRange(m_nSpinEditLower, m_nSpinEditUpper );
////	m_spinEdit.SetTitle(strTitle);	
////	m_spinEdit.SetPercent(iPercent);
//	
//	//m_spinEdit.m_editValue.SetFocus();
//	m_spinEdit.ShowWindow(SW_SHOW);
//	//m_spinEdit.BringWindowToTop();
//	
//	return NULL;  // USER CAN NOT CHARGE IT.
////	return CTreeCtrl::EditLabel(hItem);
//}
//
//

LRESULT CTreeCtrlEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{	
	case WM_INPLACE_COMBO:
		{
		if (m_hEditedItem == NULL)
			return FALSE;

		int nSelIdx = (int)lParam;
		if( nSelIdx == -1 )
			return FALSE;

		if( nSelIdx == 0 )
		{
			CDlgProbDist dlg( m_pInTerm->m_pAirportDB, true );
			if( dlg.DoModal() != IDOK)
				return FALSE;

			nSelIdx = dlg.GetSelectedPDIdx();
		}
		else
			nSelIdx--;
		GetParent()->SendMessage( message, wParam, (LPARAM)nSelIdx );

		return TRUE;
		}

	}
	return CARCTreeCtrlEx::DefWindowProc(message, wParam, lParam);
}


//void CTreeCtrlEx::EditTime( HTREEITEM _hItem, COleDateTime _oTime )
//{
//	if( _hItem == NULL ) 
//		return;
//	
//	m_hEditedItem = _hItem;
//	
//	CRect rcItem;
//	GetItemRect(_hItem, rcItem, TRUE);
//	rcItem.bottom = rcItem.top + 20;
//	
//	
//	if (!m_inPlaceDateTimeCtrl.GetSafeHwnd())
//		m_inPlaceDateTimeCtrl.Create( DTS_TIMEFORMAT, rcItem, this, IDC_DATETIMEPICKER_TIME );
//	else
//		m_inPlaceDateTimeCtrl.MoveWindow(&rcItem);
//
//	m_inPlaceDateTimeCtrl.SetTime( _oTime );
//	m_inPlaceDateTimeCtrl.ShowWindow(SW_SHOW);
//	m_inPlaceDateTimeCtrl.BringWindowToTop();
//	m_inPlaceDateTimeCtrl.SetFocus();
//}


void CTreeCtrlEx::EditProbabilityDistribution( HTREEITEM _hItem  ,bool bOverlapItem)
{
	if( _hItem == NULL ) 
		return;
	
	m_hEditedItem = _hItem;


	CRect rcItem, rcEdit;
	GetItemRect(_hItem, rcItem, TRUE);
	if(!bOverlapItem)
		rcItem.OffsetRect(rcItem.Width(),0);	
	
	int nHeight = rcItem.Height();
	rcItem.bottom += nHeight * 5;
	rcItem.right = rcItem.left + 250;	
	
	if (!m_inPlaceComboBox.GetSafeHwnd())
	{

		DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;
		
		m_inPlaceComboBox.Create( dwStyle, rcItem, this, IDC_COMBO_LEVEL );
		m_inPlaceComboBox.SetItemHeight( -1, nHeight );
		m_inPlaceComboBox.SetHorizontalExtent( 300 );
		CString s;
		s.LoadString( IDS_STRING_NEWDIST );
		m_inPlaceComboBox.AddString( s );
		int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( m );			
			m_inPlaceComboBox.AddString( pPBEntry->m_csName );
		}
	}
	else
	{
		// first remove all string in the combbox
		m_inPlaceComboBox.ResetContent();
		
		// then add new string to combox
		CString s;
		s.LoadString( IDS_STRING_NEWDIST );
		m_inPlaceComboBox.AddString( s );
		int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( m );			
			m_inPlaceComboBox.AddString( pPBEntry->m_csName );
		}
		
		m_inPlaceComboBox.SetCurSel( -1 );
		m_inPlaceComboBox.MoveWindow(&rcItem);		
	}
	
	m_inPlaceComboBox.ShowWindow(SW_SHOW);
	m_inPlaceComboBox.BringWindowToTop();
	m_inPlaceComboBox.SetFocus();
	
}

void CTreeCtrlEx::EditProbabilityDistribution( InputTerminal*_pInTerm ,HTREEITEM _hItem,bool bOverlapItem )
{
	setInputTerminal(_pInTerm);
	EditProbabilityDistribution(_hItem, bOverlapItem);
}


//void CTreeCtrlEx::EditLabel(HTREEITEM _hItem)
//{
//	if( _hItem == NULL ) 
//		return;
//	
//	m_hEditedItem = _hItem;
//	
//	CRect rcItem;
//	GetItemRect(_hItem, rcItem, TRUE);
//	rcItem.bottom = rcItem.top + 20;
//		
//	if (!m_inPlaceEdit.GetSafeHwnd())
//		m_inPlaceEdit.Create( WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rcItem, this, IDC_IPEDIT );
//	else
//		m_inPlaceEdit.MoveWindow(&rcItem);
//
//	m_inPlaceEdit.SetWindowText( GetItemText( _hItem ) );
//	m_inPlaceEdit.ShowWindow(SW_SHOW);
//	m_inPlaceEdit.BringWindowToTop();
//	m_inPlaceEdit.SetFocus();
//}
//
//
//
//CEdit* CTreeCtrlEx::StringEditLabel(HTREEITEM hItem, CString sDisplayTitle)
//{
//	if (hItem == NULL) return NULL;
//	
//	m_hEditedItem = hItem;
//	
//	CString strTitle, strPercent;
//	if( sDisplayTitle == "" )
//		strTitle = GetItemText(hItem);
//	else if ( sDisplayTitle == _T("NULL"))
//		strTitle = _T("");
//	else
//		strTitle = sDisplayTitle;
//
//	CRect rcItem;
//	GetItemRect(hItem, rcItem, TRUE);
//	
//	
//	if (!m_inPlaceEdit.GetSafeHwnd())
//	{
//		m_inPlaceEdit.Create( WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, CRect(10, 10, 100, 30), this, 1 );
//	}
//
//	m_inPlaceEdit.SetWindowPos(NULL,rcItem.right,rcItem.top,0,0,SWP_NOSIZE);
//	
//
//	m_inPlaceEdit.SetWindowText(strTitle);
//	m_inPlaceEdit.SetFocus();
//
//	m_inPlaceEdit.ShowWindow(SW_SHOW);
//
//	
//	return NULL;  // USER CAN NOT CHARGE IT.
//}
//CEdit* CTreeCtrlEx::SetComboString(HTREEITEM _hItem,const std::vector<CString>& strlist ){
//	if( _hItem == NULL ) 
//		return NULL;
//
//
//	
//
//	m_hEditedItem = _hItem;
//	CString itemstr = GetItemText(_hItem);
//
//	CRect rcItem, rcEdit;
//	GetItemRect(_hItem, rcItem, TRUE);
//
//	
//
//	int nHeight = rcItem.Height();
//	rcItem.left += itemstr.GetLength()*6;
//	rcItem.bottom += nHeight * 4;
//	rcItem.right = rcItem.left + m_nWidth;
//
//	if (!m_comboBox.GetSafeHwnd())
//	{
//
//		DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;
//
//		m_comboBox.Create( dwStyle, rcItem, this, IDC_COMBO_LEVEL );
//		CFont* font = GetParent()->GetFont();
//		m_comboBox.SetFont( font );
//		m_comboBox.SetItemHeight( -1, nHeight );
//		m_comboBox.SetHorizontalExtent( 300 );
//		m_comboBox.MoveWindow(&rcItem);	
//		
//		for( int m=0; m<int(strlist.size()); m++ ){					
//			m_comboBox.AddString( strlist[m] );
//		}
//	}
//	else
//	{
//		// first remove all string in the combbox
//		m_comboBox.ResetContent();
//
//		for( int m=0; m<int(strlist.size()); m++ ){					
//			m_comboBox.AddString( strlist[m] );
//		}
//
//		m_comboBox.SetCurSel( -1 );
//		m_comboBox.MoveWindow(&rcItem);
//	}
//
//	m_comboBox.ShowWindow(SW_SHOW);
//	m_comboBox.BringWindowToTop();
//	m_comboBox.SetFocus();
//	return NULL;
//}
//void CTreeCtrlEx::SetComboWidth(int nWidth)
//{
//	m_nWidth = nWidth;
//}
//
//void CTreeCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
//{
//	// TODO: Add your message handler code here and/or call default
//	if( GetKeyState(VK_CONTROL) & 0x8000 )
//	{
//		CTreeCtrl::OnLButtonDblClk(nFlags, point);
//		return;
//	}
//	UINT flags;
//	HTREEITEM hItem=HitTest(point, &flags);
//	m_hEditedItem=hItem;
//	CTreeCtrl::SelectItem(m_hEditedItem);
//	if( hItem &&(flags & TVHT_ONITEMLABEL))
//	{
//		GetParent()->SendMessage(MODIFYVALUE,(WPARAM)m_hEditedItem,0);
//		if (m_bEableEdit)
//		{
//			SpinEditLabel(this->GetSelectedItem());
//		}
//		return;
//	}
//	CTreeCtrl::OnLButtonDblClk(nFlags, point);	
//}