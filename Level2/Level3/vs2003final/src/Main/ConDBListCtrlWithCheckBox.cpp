// ConDBListCtrlWithCheckBox.cpp: implementation of the CConDBListCtrlWithCheckBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "ConDBListCtrlWithCheckBox.h"
#include "..\common\ProbabilityDistribution.h"
#include "..\inputs\MobileElemConstraint.h"
#include "..\Inputs\probab.h"
#include "ProbDistDlg.h"
#include "DlgProbDIst.h"
#include "..\common\ProbDistEntry.h"
#include "..\common\probdistmanager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConDBListCtrlWithCheckBox::CConDBListCtrlWithCheckBox()
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
	
}

CConDBListCtrlWithCheckBox::~CConDBListCtrlWithCheckBox()
{

}

BEGIN_MESSAGE_MAP(CConDBListCtrlWithCheckBox, CConDBExListCtrl)
	//{{AFX_MSG_MAP(CConDBListCtrlWithCheckBox)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE( UM_ITEM_CHECK_STATUS_CHANGED, OnCheckStateChanged )
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
END_MESSAGE_MAP()

void CConDBListCtrlWithCheckBox::DrawItem( LPDRAWITEMSTRUCT lpdis)
{
	COLORREF clrTextSave, clrBkSave;
	CRect rcItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	int nItem = lpdis->itemID;
	LVITEM lvi; 
    lvi.mask = LVIF_PARAM | LVIF_STATE; 
    lvi.iItem = nItem;
    lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
    GetItem(&lvi); 
	
	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lvi.state & LVIS_SELECTED) || (lvi.state & LVIS_DROPHILITED);
	
	CRect rcAllLabels; 
    this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS); 



	rcAllLabels.left = 0;
    if(bSelected) { 
        clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
        clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
        pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT))); 
    } 
    else {
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
        pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_WINDOW)));
	}

	//draw Case name
    CRect rcPBCount; 
    this->GetItemRect(nItem, rcPBCount, LVIR_LABEL);
	rcPBCount.left+=2;
	CString strName=GetItemText(lvi.iItem,0);
	pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcPBCount);
	rcPBCount.left+=2;
	strName=GetItemText(lvi.iItem,1);
	pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	this->GetSubItemRect(nItem,3,LVIR_LABEL,rcPBCount);
	rcPBCount.left+=2;
	strName=GetItemText(lvi.iItem,3);
	pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	this->GetSubItemRect(nItem,4,LVIR_LABEL,rcPBCount);
	rcPBCount.left+=2;
	strName=GetItemText(lvi.iItem,4);
	pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	this->GetSubItemRect(nItem,5,LVIR_LABEL,rcPBCount);
	rcPBCount.left+=2;
	strName=GetItemText(lvi.iItem,5);
	pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	this->GetSubItemRect(nItem,6,LVIR_LABEL,rcPBCount);
	rcPBCount.left+=2;
	strName=GetItemText(lvi.iItem,6);
	pDC->DrawText(strName , rcPBCount, DT_LEFT | DT_VCENTER | DT_SINGLELINE );


	CRect rcCheckBox;
	this->GetSubItemRect( nItem,2,LVIR_LABEL,rcCheckBox );
	rcCheckBox.left+=(rcCheckBox.Width()-m_sizeCheck.cx)/2;
	rcCheckBox.top+=(rcCheckBox.Height()-m_sizeCheck.cy)/2;

		CDC bitmapDC;
		HBITMAP hOldBM;
		int nCheck = GetItemCheck(lvi.iItem);
		if(bitmapDC.CreateCompatibleDC(pDC)) {
			hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
			pDC->BitBlt(rcCheckBox.left, rcCheckBox.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
				m_sizeCheck.cx * nCheck, 0, SRCCOPY);
			::SelectObject(bitmapDC.m_hDC, hOldBM);
			bitmapDC.Detach();
		}
	//draw checkbox
	
	
	if (lvi.state & LVIS_FOCUSED) 
        pDC->DrawFocusRect(rcAllLabels); 
	
    if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}	

void CConDBListCtrlWithCheckBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nItem=HitTest(point);
	if(nItem != -1) 
	{
		CRect rcVisible;
		GetSubItemRect(nItem,2,LVIR_LABEL,rcVisible);
		if(rcVisible.PtInRect(point))
		{
			BOOL bCheck=GetItemCheck(nItem);
			if(bCheck)
				SetItemCheck( nItem,FALSE);
			else
				SetItemCheck( nItem,TRUE);
			SendMessage(UM_ITEM_CHECK_STATUS_CHANGED,(WPARAM)nItem,(LPARAM)!bCheck);
		}
		Invalidate();
	}
	CConDBListCtrl::OnLButtonDown(nFlags,point);
	
}


void CConDBListCtrlWithCheckBox::GetVisibleRect(CRect &r)
{
	r.left = (r.left+r.right-m_sizeCheck.cx)/2;
	r.top = (r.bottom+r.top-m_sizeCheck.cy)/2;
	r.bottom = r.top+m_sizeCheck.cy;
	r.right = r.left+m_sizeCheck.cx;
}

void CConDBListCtrlWithCheckBox::SetItemCheck(int nItem,BOOL bCheck)
{
	if(bCheck)
		SetItemText(nItem,2,"1");
	else
		SetItemText(nItem,2,"0");
}

BOOL CConDBListCtrlWithCheckBox::GetItemCheck(int nItem)
{
	CString strCheck=GetItemText(nItem,2);
	if(strCheck==CString("1"))
		return TRUE;
	return FALSE;
}


//Reload data from the memory to the list control
void CConDBListCtrlWithCheckBox::ReloadData( Constraint* _pSelCon )
{
	CConDBListCtrl::ReloadData( _pSelCon );
	if( m_pConDB == NULL )
		return;
	
	int nCount = GetItemCount();
	for( int ii=0; ii<nCount; ii++ )
	{
		int iItemData = GetItemData( ii );
		if( iItemData == -1 )	
		{
			SetItemCheck( ii, FALSE );
		}
		else
		{
			ConstraintEntryEx* pEntry = (ConstraintEntryEx*)(m_pConDB->getItem( iItemData ) );
			bool flag = pEntry->getFlag();
			SetItemCheck( ii, flag );
		}

		
	}

////////////////////////////////////////////////////////////////
//add by porter
	
	for ( int j = 0; j < nCount; j++ )
	{
		int iItemData = GetItemData( j );
		ConstraintEntryEx* pConEntry = (ConstraintEntryEx*)(m_pConDB->getItem( iItemData )) ;
	    const Constraint * pConstriant = pConEntry->getConstraint();
		
///////////////////////////insert ImpactSpeed
		bool bEnable = false;

		int nSpeed = m_pImpactSpeedDB->getCount();
		for (int k = 0; k < nSpeed; k++)
		{
			ConstraintEntry* pEntry = m_pImpactSpeedDB->getItem(k);
			const Constraint* pCon = pEntry->getConstraint();

			if (pConstriant->isEqual(pCon))
			{
				bEnable = true;

				const ProbabilityDistribution* pProbDis = pEntry->getValue();
				char szDist[1024];
				pProbDis->screenPrint(szDist);
				SetItemText(iItemData,3,szDist);
			}
		}


		   if (bEnable == false)
			{
				char buf[2560];
				pConstriant->WriteSyntaxStringWithVersion( buf );

				CMobileElemConstraint* pNewMobileCon = new CMobileElemConstraint( m_pInTerm );
				assert( m_pInTerm );
				//pNewMobileCon->SetInputTerminal( m_pInTerm );
				pNewMobileCon->setConstraintWithVersion( buf );

				ConstraintEntry* pNewEntry = new ConstraintEntry;
				ConstantDistribution* conDis = new ConstantDistribution(0);
				pNewEntry->initialize(pNewMobileCon,conDis);
				m_pImpactSpeedDB->addEntry(pNewEntry,true);

				char szDist[1024];
				conDis->screenPrint(szDist);
				BOOL b =SetItemText(iItemData,3,szDist);
            }
		

///////////////////////////////insert SideStep

		bEnable = false;

		int nSide = m_pSideStepDB->getCount();
		for (int k = 0; k < nSide; k++)
		{
			ConstraintEntry* pEntry = m_pSideStepDB->getItem(k);
			const Constraint* pCon = pEntry->getConstraint();

			if (pConstriant->isEqual(pCon))
			{
				bEnable = true;
				const ProbabilityDistribution* pProbDis = pEntry->getValue();
				char szDist[1024];
				pProbDis->screenPrint(szDist);
				SetItemText(j,5,szDist);
			}

		}

		if (bEnable == false)
		{
			char buf[2560];

			pConstriant->WriteSyntaxStringWithVersion( buf );
			CMobileElemConstraint* pNewMobileCon = new CMobileElemConstraint( m_pInTerm );
			assert( m_pInTerm );
			//pNewMobileCon->SetInputTerminal( m_pInTerm );
			pNewMobileCon->setConstraintWithVersion( buf );

			ConstraintEntry* pNewEntry = new ConstraintEntry;
			ConstantDistribution* conDis = new ConstantDistribution(0);
			pNewEntry->initialize(pNewMobileCon,conDis);
			m_pSideStepDB->addEntry(pNewEntry,true);

			char szDist[1024];
			conDis->screenPrint(szDist);
			SetItemText(j,5,szDist);

		}
///////////////////////////////insert ImpactInStep

		bEnable = false;

		int nInStep = m_pImpactInstepDB->getCount();
		for (int k = 0; k < nInStep; k++)
		{
			ConstraintEntry* pEntry = m_pImpactInstepDB->getItem(k);
			const Constraint* pCon = pEntry->getConstraint();

			if (pConstriant->isEqual(pCon))
			{
				bEnable = true;
				const ProbabilityDistribution* pProbDis = pEntry->getValue();
				char szDist[1024];
				pProbDis->screenPrint(szDist);
				SetItemText(j,4,szDist);
			}

		}

		if (bEnable == false)
		{
			char buf[2560];

			pConstriant->WriteSyntaxStringWithVersion( buf );
			CMobileElemConstraint* pNewMobileCon = new CMobileElemConstraint( m_pInTerm );
			assert( m_pInTerm );
			pNewMobileCon->SetInputTerminal( m_pInTerm );
			pNewMobileCon->setConstraintWithVersion( buf );

			ConstraintEntry* pNewEntry = new ConstraintEntry;
			ConstantDistribution* conDis = new ConstantDistribution(0);
			pNewEntry->initialize(pNewMobileCon,conDis);
			m_pImpactInstepDB->addEntry(pNewEntry,true);

			char szDist[1024];
			conDis->screenPrint(szDist);
			SetItemText(j,4,szDist);

		}
/////////////////////////////////insert EmerImpact

		bEnable = false;

		int nEmer = m_pEmerImpactDB->getCount();
		for (int k = 0; k < nEmer; k++)
		{
			ConstraintEntry* pEntry = m_pEmerImpactDB->getItem(k);
			const Constraint* pCon = pEntry->getConstraint();

			if (pConstriant->isEqual(pCon))
			{
				bEnable = true;
				const ProbabilityDistribution* pProbDis = pEntry->getValue();
				char szDist[1024];
				pProbDis->screenPrint(szDist);
				SetItemText(j,6,szDist);
			}

		}

		if (bEnable == false)
		{
			char buf[2560];

			pConstriant->WriteSyntaxStringWithVersion( buf );
			CMobileElemConstraint* pNewMobileCon = new CMobileElemConstraint(m_pInTerm);
			assert( m_pInTerm );
			pNewMobileCon->SetInputTerminal( m_pInTerm );
			pNewMobileCon->setConstraintWithVersion( buf);

			ConstraintEntry* pNewEntry = new ConstraintEntry;
			ConstantDistribution* conDis = new ConstantDistribution(0);
			pNewEntry->initialize(pNewMobileCon,conDis);
			m_pEmerImpactDB->addEntry(pNewEntry,true);

			char szDist[1024];
			conDis->screenPrint(szDist);
			SetItemText(j,6,szDist);

		}
////////////////////////////////////////////

	}

}



LRESULT CConDBListCtrlWithCheckBox::OnCheckStateChanged( WPARAM wParam, LPARAM lParam )
{
	int iItem = (int) wParam;
	bool flag = lParam ? true : false;
	int iItemData = GetItemData( iItem );

	if( iItemData != -1 )
	{
		ConstraintEntryEx* pEntry = (ConstraintEntryEx*)(m_pConDB->getItem( iItemData ));
		pEntry->setFlag( flag );
	}

	GetParent()->SendMessage(UM_ITEM_CHECK_STATUS_CHANGED,wParam,lParam);
	return TRUE;
}

void CConDBListCtrlWithCheckBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();
	CConDBListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CConDBListCtrlWithCheckBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();
	CConDBListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CConDBListCtrlWithCheckBox::SetImpactSpeedDB( ConstraintDatabase* _pConDB, int _nForceItemData/* =0  */)
{
	m_pImpactSpeedDB = _pConDB;
}

void CConDBListCtrlWithCheckBox::SetImpactInstepDB( ConstraintDatabase* _pConDB, int _nForceItemData/* =0  */)
{
	m_pImpactInstepDB =_pConDB;
}

void CConDBListCtrlWithCheckBox::SetSideStepDB( ConstraintDatabase* _pConDB, int _nForceItemData/* =0  */)
{
	m_pSideStepDB = _pConDB;
}
void CConDBListCtrlWithCheckBox::SetEmerImpactDB( ConstraintDatabase* _pConDB, int _nForceItemData/* =0  */)
{
	m_pEmerImpactDB = _pConDB;
}

void CConDBListCtrlWithCheckBox::OnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here

	CConDBListCtrl::OnEndlabeledit(pNMHDR,pResult);

	LV_ITEM* plvItem = &pDispInfo->item;

	int nCount = m_pConDB->getCount();

//////////////////////////////////////////////
	if( plvItem->iSubItem == 3 )//Impact Speed
	{
		// change data.
		ProbabilityDistribution* pProbDist = NULL;
		CString s;
		s.LoadString( IDS_STRING_NEWDIST );
		if( strcmp( plvItem->pszText, s ) == 0 )
		{
			CDlgProbDist dlg(m_pInTerm->m_pAirportDB,  true );
			if(dlg.DoModal()==IDOK)
			{
				int idxPD = dlg.GetSelectedPDIdx();
				ASSERT(idxPD!=-1);
				CProbDistEntry* pde = _g_GetActiveProbMan( m_pInTerm )->getItem(idxPD);
				SetItemText( plvItem->iItem, plvItem->iSubItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
			else
			{
				ConstraintEntry* pConEntryQuery = m_pConDB->getItem(GetItemData(plvItem->iItem ));
				const Constraint* pConsQuery = pConEntryQuery->getConstraint();
				ConstraintEntry* pEntryQuery = NULL;

				int nSpeedCount = m_pImpactSpeedDB->getCount();
				for (int m = 0; m < nSpeedCount; m++)
				{
					ConstraintEntry* pSpeedEntry = m_pImpactSpeedDB->getItem(m);
					const Constraint* pSpeed = pSpeedEntry->getConstraint();
					if (pConsQuery->isEqual(pSpeed))//find the same type
					{
						pEntryQuery = m_pImpactSpeedDB->getItem(m);
						break;
					}

				}

				assert(pEntryQuery);
                ProbabilityDistribution* pProb = pEntryQuery->getValue();
				char szDist[1024];
				pProb->screenPrint( szDist );
				SetItemText(plvItem->iItem, plvItem->iSubItem, szDist);
				pProbDist = ProbabilityDistribution::CopyProbDistribution(pProb);
			}
		}
		else
		{
			CProbDistEntry* pPDEntry = NULL;
			int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( i );
				if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
					break;
			}
			assert( i < nCount );
			pProbDist = pPDEntry->m_pProbDist;
		}
		assert( pProbDist );

		ConstraintEntry* pModConEntry = NULL;
		ConstraintEntry* pConEntry = m_pConDB->getItem(GetItemData(plvItem->iItem ));
		const Constraint* pCons = pConEntry->getConstraint();

		int nSpeedCount = m_pImpactSpeedDB->getCount();
		for (int m = 0; m < nSpeedCount; m++)
		{
			ConstraintEntry* pSpeedEntry = m_pImpactSpeedDB->getItem(m);
			const Constraint* pSpeed = pSpeedEntry->getConstraint();
			if (pCons->isEqual(pSpeed))
			{
				pModConEntry = m_pImpactSpeedDB->getItem(m);
				break;
			}

		}
		assert(pModConEntry);
		ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pProbDist);
		pModConEntry->setValue( pDist );
	}
//////////////////////////////////////////////////
	if( plvItem->iSubItem == 4 )//Impact In-Step
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
				SetItemText( plvItem->iItem, plvItem->iSubItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
			else
			{
				ConstraintEntry* pConEntryQuery = m_pConDB->getItem(GetItemData(plvItem->iItem ));
				const Constraint* pConsQuery = pConEntryQuery->getConstraint();
				ConstraintEntry* pEntryQuery = NULL;

				int nSpeedCount = m_pImpactInstepDB->getCount();
				for (int m = 0; m < nSpeedCount; m++)
				{
					ConstraintEntry* pInStepEntry = m_pImpactInstepDB->getItem(m);
					const Constraint* pInStep = pInStepEntry->getConstraint();
					if (pConsQuery->isEqual(pInStep))//find the same type
					{
						pEntryQuery = m_pImpactInstepDB->getItem(m);
						break;
					}

				}

				assert(pEntryQuery);
				ProbabilityDistribution* pProb = pEntryQuery->getValue();
				char szDist[1024];
				pProb->screenPrint( szDist );
				SetItemText(plvItem->iItem, plvItem->iSubItem, szDist);
			    pProbDist = ProbabilityDistribution::CopyProbDistribution(pProb);
			}
		}
		else
		{
			CProbDistEntry* pPDEntry = NULL;
			int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( i );
				if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
					break;
			}
			assert( i < nCount );
			pProbDist = pPDEntry->m_pProbDist;
		}
		assert( pProbDist );

		ConstraintEntry* pModConEntry = NULL;
		ConstraintEntry* pConEntry = m_pConDB->getItem(GetItemData(plvItem->iItem ));
		const Constraint* pCons = pConEntry->getConstraint();

		int nInStepCount = m_pImpactInstepDB->getCount();
		for (int m = 0; m < nInStepCount; m++)
		{
			ConstraintEntry* pSpeedEntry = m_pImpactInstepDB->getItem(m);
			const Constraint* pSpeed = pSpeedEntry->getConstraint();
			if (pCons->isEqual(pSpeed))
			{
				pModConEntry = m_pImpactInstepDB->getItem(m);
				break;
			}

		}
		
		assert(pModConEntry);
		ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pProbDist);
		pModConEntry->setValue( pDist );
	}
///////////////////////////////////////////////
	if( plvItem->iSubItem == 5 )//SideStep
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
				SetItemText( plvItem->iItem, plvItem->iSubItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
			else
			{
				ConstraintEntry* pConEntryQuery = m_pConDB->getItem(GetItemData(plvItem->iItem ));
				const Constraint* pConsQuery = pConEntryQuery->getConstraint();
				ConstraintEntry* pEntryQuery = NULL;

				int nSpeedCount = m_pSideStepDB->getCount();
				for (int m = 0; m < nSpeedCount; m++)
				{
					ConstraintEntry* pSideEntry = m_pSideStepDB->getItem(m);
					const Constraint* pSide = pSideEntry->getConstraint();
					if (pConsQuery->isEqual(pSide))//find the same type
					{
						pEntryQuery = m_pSideStepDB->getItem(m);
						break;
					}
				}

				assert(pEntryQuery);
				ProbabilityDistribution* pProb = pEntryQuery->getValue();
				char szDist[1024];
				pProb->screenPrint( szDist );
				SetItemText(plvItem->iItem, plvItem->iSubItem, szDist);
				pProbDist = ProbabilityDistribution::CopyProbDistribution(pProb);
			}
		}
		else
		{
			CProbDistEntry* pPDEntry = NULL;
			int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( i );
				if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
					break;
			}
			assert( i < nCount );
			pProbDist = pPDEntry->m_pProbDist;
		}
		assert( pProbDist );

		ConstraintEntry* pModConEntry = NULL;		
		ConstraintEntry* pConEntry = m_pConDB->getItem(GetItemData(plvItem->iItem ));
		const Constraint* pCons = pConEntry->getConstraint();

		int nSideCount = m_pSideStepDB->getCount();
		for (int m = 0; m < nSideCount; m++)
		{
			ConstraintEntry* pSpeedEntry = m_pSideStepDB->getItem(m);
			const Constraint* pSpeed = pSpeedEntry->getConstraint();
			if (pCons->isEqual(pSpeed))
			{
				pModConEntry = m_pSideStepDB->getItem(m);
				break;
			}

		}
	
		assert(pModConEntry);
		ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pProbDist);
		pModConEntry->setValue( pDist );
	}
///////////////////////////////////////////////
	if( plvItem->iSubItem == 6 )//Emer impact
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
				SetItemText( plvItem->iItem, plvItem->iSubItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
			else
			{
				ConstraintEntry* pConEntryQuery = m_pConDB->getItem(GetItemData(plvItem->iItem ));
				const Constraint* pConsQuery = pConEntryQuery->getConstraint();
				ConstraintEntry* pEntryQuery = NULL;

				int nSpeedCount = m_pEmerImpactDB->getCount();
				for (int m = 0; m < nSpeedCount; m++)
				{
					ConstraintEntry* pEmerEntry = m_pEmerImpactDB->getItem(m);
					const Constraint* pEmer = pEmerEntry->getConstraint();
					if (pConsQuery->isEqual(pEmer))//find the same type
					{
						pEntryQuery = m_pEmerImpactDB->getItem(m);
						break;
					}
				}

				assert(pEntryQuery);
				ProbabilityDistribution* pProb = pEntryQuery->getValue();
				char szDist[1024];
				pProb->screenPrint( szDist );
				SetItemText(plvItem->iItem, plvItem->iSubItem, szDist);
				pProbDist = ProbabilityDistribution::CopyProbDistribution(pProb);
			}
		}
		else
		{
			CProbDistEntry* pPDEntry = NULL;
			int nCount = _g_GetActiveProbMan( m_pInTerm )->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = _g_GetActiveProbMan( m_pInTerm )->getItem( i );
				if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
					break;
			}
			assert( i < nCount );
			pProbDist = pPDEntry->m_pProbDist;
		}
		assert( pProbDist );

		ConstraintEntry* pModConEntry = NULL;		
		ConstraintEntry* pConEntry = m_pConDB->getItem(GetItemData(plvItem->iItem ));
		const Constraint* pCons = pConEntry->getConstraint();

		int nEmerCount = m_pEmerImpactDB->getCount();
		for (int m = 0; m < nEmerCount; m++)
		{
			ConstraintEntry* pEmerEntry = m_pEmerImpactDB->getItem(m);
			const Constraint* pEmer = pEmerEntry->getConstraint();
			if (pCons->isEqual(pEmer))
			{
				pModConEntry = m_pEmerImpactDB->getItem(m);
				break;
			}

		}
		
		assert(pModConEntry);
		ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pProbDist);
		pModConEntry->setValue( pDist );
	}


	*pResult = 0;
}
