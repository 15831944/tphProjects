// ConDBExListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ConDBExListCtrl.h"
#include "..\common\winmsg.h"
#include "..\common\ProbabilityDistribution.h"
#include "..\inputs\flt_db.h"
#include "..\inputs\pax_db.h"
#include "..\inputs\MobileElemConstraintDatabase.h"
#include "..\inputs\MobileElemConstraint.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConDBExListCtrl

CConDBExListCtrl::CConDBExListCtrl()
{
}

CConDBExListCtrl::~CConDBExListCtrl()
{
}


BEGIN_MESSAGE_MAP(CConDBExListCtrl, CConDBListCtrl)
	//{{AFX_MSG_MAP(CConDBExListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_MESSAGE( WM_INPLACE_DATETIME, OnInplaceDateTime )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConDBExListCtrl message handlers
//Reload data from the memory to the list control
void CConDBExListCtrl::ReloadData( Constraint* _pSelCon )
{
	DeleteAllItems();
	if( m_pConDB == NULL )
		return;
	
	int nCount = m_pConDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		ConstraintEntryEx* pEntryEx = (ConstraintEntryEx*)(m_pConDB->getItem( i ));
		const Constraint* pCon = pEntryEx->getConstraint();
		CString szName;
		pCon->screenPrint( szName, 0, 255 );
		const ProbabilityDistribution* pProbDist = pEntryEx->getValue();
		char szDist[1024];
		pProbDist->screenPrint( szDist );
		int nIdx = InsertItem( i, szName.GetBuffer(0) );
		if( m_nForceItemData == -1 )
			SetItemData( nIdx, -1 );
		else
			SetItemData( nIdx, i );
		SetItemText( nIdx, 1, szDist );
		//////////////////////////////////////////////////////////////////////////
		// display degin_time, end_time
		char szBuf[64];
		pEntryEx->getBeginTime().printTime( szBuf );
		SetItemText( nIdx, 2, szBuf );
		pEntryEx->getEndTime().printTime( szBuf );
		SetItemText( nIdx, 3, szBuf );
		//////////////////////////////////////////////////////////////////////////
		
		if( pCon == _pSelCon )
			SetItemState( nIdx, LVIS_SELECTED, LVIS_SELECTED );
	}	
}


bool CConDBExListCtrl::AddEntry(enum CON_TYPE _enumConType, Constraint* _pNewCon)
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
			ConstraintEntryEx* pNewEntry = new ConstraintEntryEx();
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
			ConstraintEntryEx* pNewEntry = new ConstraintEntryEx();
			ConstantDistribution* pDist = new ConstantDistribution( 0 );
			pNewEntry->initialize( pMobileCon, pDist );
			m_pConDB->addEntry( pNewEntry );
			ReloadData( pMobileCon );	
		}
	}	
	return true;
}


void CConDBExListCtrl::InsertOtherCol(int bBeginIndex)
{
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT |LVCF_FMT;
	
	CStringList listStr;
	lvc.csList=&listStr;
	lvc.cx=100;
	
	lvc.fmt=LVCFMT_DATETIME;
	lvc.pszText="Begin Time";
	InsertColumn(bBeginIndex,&lvc);	
	
	lvc.cx=80;
	lvc.pszText="End Time";
	InsertColumn(bBeginIndex+1,&lvc);
}



LRESULT CConDBExListCtrl::OnInplaceDateTime( WPARAM wParam, LPARAM lParam )
{
	if( m_nTimeSelItem < 0 || m_nTimeSelCol<0 )
		return TRUE;
	
	int iItemData = GetItemData( m_nTimeSelItem );
	if( iItemData == -1 )	
		return TRUE;
	
	ElapsedTime	time;
	COleDateTime ole_time = *((COleDateTime* ) lParam);
	time.set( ole_time.GetHour(), ole_time.GetMinute(), ole_time.GetSecond() );
	ConstraintEntryEx* pEntryEx = (ConstraintEntryEx*)(m_pConDB->getItem(iItemData ));
	
	if( m_nTimeSelCol == 2)		
	{
		ElapsedTime endTime = pEntryEx->getEndTime();
		if( time>= endTime )
		{
			MessageBox("The begin time can not more than end time",NULL,MB_OK|MB_ICONINFORMATION );
			return TRUE;
		}
		pEntryEx->setBeginTime( time );
	}
	else if( m_nTimeSelCol == 3 )
	{
		ElapsedTime beginTim = pEntryEx->getBeginTime();
		if( time <= beginTim )
		{
			MessageBox(" The end time can not less than begin time",NULL,MB_OK|MB_ICONINFORMATION );
			return TRUE;
		}
		pEntryEx->setEndTime( time );
	}
	
	else
		return TRUE;
	
	// set item text
	char szBuf[64];
	time.printTime(szBuf);
	SetItemText( m_nTimeSelItem, m_nTimeSelCol,szBuf );
	
	// send message to parent
	GetParent()->SendMessage( UM_LIST_CONTENT_CHANGED, 0l,0l );
	return TRUE;
}
