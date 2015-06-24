#include "StdAfx.h"
#include ".\docdisplaytimelistctrl.h"
#include "termplan.h"
#include "ConDBListCtrl.h"
#include "..\common\ProbabilityDistribution.h"
#include "..\inputs\flt_db.h"
#include "..\inputs\pax_db.h"
#include "..\Inputs\probab.h"
#include "..\inputs\MobileElemConstraint.h"
#include "ProbDistDlg.h"
#include "DlgProbDIst.h"
#include "..\common\probdistmanager.h"
#include "..\inputs\MobileElemConstraintDatabase.h"
#include "..\inputs\GageLagTimeData.h"
#include "ProbDistDlg.h"
#include "../InputOnBoard/DocDisplayTimeDB.h"
CDocDisplayTimeListCtrl::CDocDisplayTimeListCtrl(void):CListCtrlEx()
{
	memset( m_percent,0,sizeof(float) * 1024) ;
}

CDocDisplayTimeListCtrl::~CDocDisplayTimeListCtrl(void)
{
}
BEGIN_MESSAGE_MAP(CDocDisplayTimeListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CConDBListCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	//	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
END_MESSAGE_MAP()

void CDocDisplayTimeListCtrl::InitColumnWidthPercent()
{
	CRect rc ;
	GetWindowRect(&rc) ;
	ScreenToClient(&rc) ;
	float total =(float)(rc.right -rc.left ) ;
	for (int i = 0; i < 2&&i<1024 ; i++)
	{
		m_percent[i] = GetColumnWidth(i) /total ;
	}
}
BOOL CDocDisplayTimeListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	// TODO: Add your control notification handler code here
	LV_ITEM* plvItem = &pDispInfo->item;

	if( plvItem->iSubItem == 1 )
	{

		// change data.
		ProbabilityDistribution* pProbDist = NULL;
		CString s;
		s.LoadString( IDS_STRING_NEWDIST );
		if( strcmp( plvItem->pszText, s ) == 0 )
		{
			CDlgProbDist dlg(m_pInTerm->m_pAirportDB,  true );
			if(dlg.DoModal()==IDOK) {
				int idxPD = dlg.GetSelectedPDIdx();
				ASSERT(idxPD!=-1);
				CProbDistEntry* pde = _g_GetActiveProbMan( m_pInTerm )->getItem(idxPD);
				CDocDisplayTimeEntry* timeEntry = (CDocDisplayTimeEntry*)GetItemData(plvItem->iItem) ;
				if(timeEntry != NULL)
					timeEntry->GetProDistrubution()->SetProDistrubution(pde) ;
				SetItemText( plvItem->iItem, plvItem->iSubItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
		}else
		{
			CProbDistEntry* pPDEntry = NULL;
			int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( i );
				if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
					break;
			}

			CDocDisplayTimeEntry* timeEntry = (CDocDisplayTimeEntry*)GetItemData(plvItem->iItem) ;
			if(timeEntry != NULL && pPDEntry != NULL)
				timeEntry->GetProDistrubution()->SetProDistrubution(pPDEntry) ;
			SetItemText( plvItem->iItem, plvItem->iSubItem, plvItem->pszText );
		}
	}
	*pResult = 0;
	return FALSE;
}
void CDocDisplayTimeListCtrl::ResizeColumnWidth()
{
	CRect rc ;
	GetWindowRect(&rc) ;
	ScreenToClient(&rc) ;
	float total =(float)(rc.right -rc.left ) ;
	for (int i =0 ; i < 2&&i<1024 ; i++)
	{
		SetColumnWidth(i,(int)(m_percent[i]*total)) ;
	}
}
void CDocDisplayTimeListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//////////////////////////////////////////////////////////////////////////
	// get all pro name
	CStringList strList;
	CString s;
	s.LoadString( IDS_STRING_NEWDIST );
	strList.InsertAfter( strList.GetTailPosition(), s );
	int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( m );			
		strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
	}

	// get the click column
	int index, colnum;
	if((index = HitTestEx(point, &colnum)) != -1)
	{

		::SendMessage( this->GetParent()->GetSafeHwnd(), WM_COLLUM_INDEX,WPARAM ( index ),0);	
		if( GetItemData( index ) == -1 )
			return;

		UINT flag = LVIS_FOCUSED;
		switch(((LVCOLDROPLIST*)ddStyleList[colnum])->Style)
		{
		case DROP_DOWN:
			{
				// reomeve all old string, then add new string
				CStringList& strNameList = ((LVCOLDROPLIST*)ddStyleList[colnum])->List;
				strNameList.RemoveAll();

				strNameList.AddTail( &strList );
				break;
			}

		default:
			break;
		}
	}

	// call base class function
	CListCtrlEx::OnLButtonDblClk(nFlags, point);
	return;
}