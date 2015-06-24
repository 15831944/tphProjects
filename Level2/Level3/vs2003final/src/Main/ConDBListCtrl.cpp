// ConDBListCtrl.cpp : implementation file
//

#include "stdafx.h"
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
#include ".\condblistctrl.h"
#include "../Common/ProbDistEntry.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConDBListCtrl

CConDBListCtrl::CConDBListCtrl()
{
	m_bIsDefault=TRUE;
}

CConDBListCtrl::~CConDBListCtrl()
{
}


BEGIN_MESSAGE_MAP(CConDBListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CConDBListCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
//	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConDBListCtrl message handlers

void CConDBListCtrl::ReloadData( Constraint* _pSelCon )
{

	DeleteAllItems();
	if(!m_bIsDefault)
	{
		if( m_pGLTConDB == NULL )
			return;
		
		int nCount = m_pGLTConDB->getCount();
		for( int i=0; i<nCount; i++ )
		{
			CGageLagTimeData* pEntry = (CGageLagTimeData*)m_pGLTConDB->getItem( i );
			int nIdx = InsertItem( i, pEntry->getID()->GetIDString());
			if( m_nForceItemData == -1 )
				SetItemData( nIdx, -1 );
			else
				SetItemData( nIdx, i );
			const ProbabilityDistribution* pProbDist =pEntry->GetDistribution();
			char szDist[1024];
			pProbDist->screenPrint( szDist );
			SetItemText( nIdx, 1,szDist);
		}	
		return;
	}
	if( m_pConDB == NULL )
		return;

	int nCount = m_pConDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		ConstraintEntry* pEntry = m_pConDB->getItem( i );
		const Constraint* pCon = pEntry->getConstraint();
		CString szName;
		pCon->screenPrint( szName, 0, 256 );
		const ProbabilityDistribution* pProbDist = pEntry->getValue();
		char szDist[1024];
		pProbDist->screenPrint( szDist);
		int nIdx = InsertItem( i, szName.GetBuffer(0) );
		if( m_nForceItemData == -1 )
			SetItemData( nIdx, -1 );
		else
			SetItemData( nIdx, i );
		SetItemText( nIdx, 1, szDist);

		if( pCon == _pSelCon )
			SetItemState( nIdx, LVIS_SELECTED, LVIS_SELECTED );
	}	
}

// return true = dirty
bool CConDBListCtrl::AddEntry(enum CON_TYPE _enumConType, Constraint* _pNewCon )
{
	if( m_pConDB == NULL )
		return false;

	if( _enumConType == FLIGHT_CON )
	{
		// check if exist 			
		int nCount = GetItemCount();
		for( int i=0; i<nCount; i++ )
		{
			int nIdx = GetItemData( i );
			const FlightConstraint* pFltCon = ((FlightConstraintDatabase*)m_pConDB)->getConstraint( nIdx );

			if( _pNewCon->isEqual(pFltCon) )
			{
				// select the item
				SelectDataList( i );
				return false;
			}
		}
		if( i == nCount )
		{
			FlightConstraint* pFltCon = new FlightConstraint;
			*pFltCon = *(FlightConstraint*)_pNewCon;
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			ConstantDistribution* pDist = new ConstantDistribution( 0 );
			pNewEntry->initialize( pFltCon, pDist );
			m_pConDB->addEntry( pNewEntry );
			ReloadData( pFltCon );
		}
	}
	else
	{
		int nCount = GetItemCount();
		for( int i=0; i<nCount; i++ )
		{
			int nIdx = GetItemData( i );
			const CMobileElemConstraint * pPaxCon = ((CMobileElemConstraintDatabase*)m_pConDB)->GetConstraint( nIdx );
			if( _pNewCon->isEqual(pPaxCon) )
			{
				// select the item
				SelectDataList( i );
				return false;
			}
		}
		if( i == nCount )
		{
			// add new pax constraint.
			//char buf[256]; matt
			char buf[2560];
			
			_pNewCon->WriteSyntaxStringWithVersion( buf );
			//PassengerConstraint* pPaxCon = new PassengerConstraint;
			CMobileElemConstraint* pMobileCon	= new CMobileElemConstraint(m_pInTerm);
			//assert( m_pInTerm );
			//pMobileCon->SetInputTerminal( m_pInTerm );
			pMobileCon->setConstraintWithVersion( buf );
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			ConstantDistribution* pDist = new ConstantDistribution( 0 );
			pNewEntry->initialize( pMobileCon, pDist );
			m_pConDB->addEntry( pNewEntry );
			ReloadData( pMobileCon );	
		}
	}	
	return true;
}

void CConDBListCtrl::SetCurConDB( ConstraintDatabase *_pConDB, int _nForceItemData /*=0*/ )
{
	m_nForceItemData = _nForceItemData;
	m_pConDB = _pConDB;
}


void CConDBListCtrl::SelectDataList(int _nIdx)
{
	int nCount = GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( i == _nIdx )
			SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
		else
			SetItemState( i, 0, LVIS_SELECTED ); 
	}
}

bool CConDBListCtrl::DeleteEntry()
{
	int nIdx = GetSelectedItem();
	if( nIdx == -1 )
		return false;

	if( GetItemData(nIdx) == -1 )
		return false;
	if(m_bIsDefault)
		if( !m_pConDB )
			return false;

	int nDBIdx = GetItemData( nIdx );
	if(m_bIsDefault)
		m_pConDB->removeItem( nDBIdx );
	else
		m_pGLTConDB->removeItem(nDBIdx);
	ReloadData( NULL );	
	return true;
}

// Enhance method RemoveEntry, able to delete multi-selection.

bool CConDBListCtrl::DeleteMultiEntry(void)
{
	if(m_bIsDefault && !m_pConDB)
	{
		return false;
	}

	// deletion code
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		return false;
	}
	std::vector<int> vecDBIndex;
	while (pos)
	{
		int nIdx = GetNextSelectedItem(pos);
		// you could do your own processing on nItem here
		// just record the DB index
		int nDBIdx = GetItemData( nIdx );
		ASSERT(-1 != nDBIdx);
		vecDBIndex.push_back(nDBIdx);
	}

	// must sort in descend order before removing data
	std::sort(vecDBIndex.begin(), vecDBIndex.end(), std::greater<int>());
	for (size_t st = 0;st<vecDBIndex.size();st++)
	{
		int nDBIdx = vecDBIndex[st];
		if(m_bIsDefault)
			m_pConDB->removeItem( nDBIdx );
		else
			m_pGLTConDB->removeItem(nDBIdx);
	}
 
	ReloadData( NULL );
	return true;
}


int CConDBListCtrl::GetSelectedItem()
{
	int nCount = GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}
CProbDistEntry* CConDBListCtrl::getSelectProbDist()
{
	return m_Selectpde;
}
BOOL CConDBListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	CListCtrlEx::OnEndlabeledit( pNMHDR, pResult );
	if(m_bIsDefault)
		if( !m_pConDB )
			return FALSE;

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
				m_Selectpde=pde;
				SetItemText( plvItem->iItem, plvItem->iSubItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
			else {
				//cancel
				if(m_bIsDefault)
				{
					const ProbabilityDistribution* pProbDist = m_pConDB->getItem( GetItemData( plvItem->iItem ) )->getValue();
					char szDist[1024];
					pProbDist->screenPrint( szDist );
					SetItemText(plvItem->iItem, plvItem->iSubItem, szDist);
				}
				else
				{
					CGageLagTimeData* pConEntry=(CGageLagTimeData*)m_pGLTConDB->getItem( GetItemData( plvItem->iItem ) );
					const ProbabilityDistribution* pProbDist=pConEntry->GetDistribution();
					char szDist[1024];
					pProbDist->screenPrint( szDist);
					SetItemText(plvItem->iItem, plvItem->iSubItem, szDist);
				}
				return FALSE;
			}

			/*
			CProbDistDlg dlg(true);
			dlg.DoModal();
			int nSelIdx = dlg.GetLastSelection();
			CProbDistEntry* pPDEntry = PROBDISTMANAGER->getItem( nSelIdx );
			SetItemText( plvItem->iItem, plvItem->iSubItem, pPDEntry->m_csName );
			pProbDist = pPDEntry->m_pProbDist;
			*/
		}
		else
		{
			CProbDistEntry* pPDEntry = NULL;
			int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( i );
				m_Selectpde=pPDEntry;
				if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
					break;
			}
			assert( i < nCount );
			pProbDist = pPDEntry->m_pProbDist;
		}
		assert( pProbDist );
		if(m_bIsDefault)
		{
			ConstraintEntry* pConEntry = m_pConDB->getItem( GetItemData( plvItem->iItem ) );
			ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pProbDist);
			pConEntry->setValue( pDist );
		}
		else
		{
			CGageLagTimeData* pConEntry=(CGageLagTimeData*)m_pGLTConDB->getItem( GetItemData( plvItem->iItem ) );
			ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pProbDist);
			pConEntry->SetDistribution( pDist );
		}
	}
	*pResult = 0;
	return FALSE;
}


LRESULT CConDBListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_MOUSEMOVE )
	{
		GetParent()->SendMessage( message, wParam, lParam );
		return true;
	}
	return CListCtrlEx::DefWindowProc(message, wParam, lParam);
}



void CConDBListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
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



void CConDBListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	int index;
	if((index = HitTestEx(point, NULL)) != -1)
	{
		LastTimeSelection = CurrentSelection;
		CurrentSelection = index;
	}

	CListCtrl::OnLButtonDown(nFlags, point);
	//CListCtrlEx::OnLButtonDown(nFlags, point);
}
