// ConWithIDDBListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ConWithIDDBListCtrl.h"
#include "..\common\ProbabilityDistribution.h"
#include "..\inputs\probab.h"
#include "..\inputs\flt_db.h"
#include "..\inputs\pax_db.h"
#include "..\common\ProbDistManager.h"
#include "..\inputs\GageLagTimeData.h"
#include "..\inputs\MobileElemConstraintDatabase.h"
#include "..\inputs\MobileElemConstraint.h"
#include "..\inputs\flightconwithprociddatabase.h"
#include "DlgProbDist.h"
#include "ProcesserDialog.h"
#include "SelectALTObjectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConWithIDDBListCtrl

CConWithIDDBListCtrl::CConWithIDDBListCtrl()
{
	
}

CConWithIDDBListCtrl::~CConWithIDDBListCtrl()
{
}


BEGIN_MESSAGE_MAP(CConWithIDDBListCtrl, CConDBListCtrl)
	//{{AFX_MSG_MAP(CConWithIDDBListCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConWithIDDBListCtrl message handlers
//Reload data from the memory to the list control
void CConWithIDDBListCtrl::ReloadData( Constraint* _pSelCon )
{
	DeleteAllItems();
	if( m_pConDB == NULL )
		return;
	
	int nCount = m_pConDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		ConstraintWithProcIDEntry* pEntryEx = (ConstraintWithProcIDEntry*)(m_pConDB->getItem( i ));
		const Constraint* pCon = pEntryEx->getConstraint();
		const ProcessorID procID = pEntryEx->getProcID();

		//char szName[256];
		CString szName;
		pCon->screenPrint( szName, 0, 256 );
		const ProbabilityDistribution* pProbDist = pEntryEx->getValue();
		char szDist[1024];
		//CString szDist;
		pProbDist->screenPrint( szDist);
		// insert pax name
		int nIdx = InsertItem( i, szName.GetBuffer(0) );
		if( m_nForceItemData == -1 )
			SetItemData( nIdx, -1 );
		else
			SetItemData( nIdx, i );
		// insert processID
		if( !procID.isBlank() )
			SetItemText( nIdx, 1, procID.GetIDString() );
		// insert distribution
		SetItemText( nIdx, 2, szDist );
		
		if( pCon == _pSelCon )
			SetItemState( nIdx, LVIS_SELECTED, LVIS_SELECTED );
	}	
}


bool CConWithIDDBListCtrl::AddEntry(enum CON_TYPE _enumConType, Constraint* _pNewCon)
{
	if( m_pConDB == NULL )
		return false;

	assert( m_pInTerm );

	// add flight database
	if( _enumConType == FLIGHT_CON )
	{
		//need not to check if exist, because it can repeated
		FlightConstraint* pFltCon = new FlightConstraint;
		*pFltCon = *(FlightConstraint*)_pNewCon;
		ConstraintWithProcIDEntry* pNewEntry = new ConstraintWithProcIDEntry();
		ConstantDistribution* pDist = new ConstantDistribution( 0 );
		ProcessorID defaultid;
		defaultid.SetStrDict( m_pInTerm->inStrDict );

		pNewEntry->initialize( pFltCon, pDist, defaultid );
		m_pConDB->addEntry( pNewEntry );
		ReloadData( pFltCon );
	}
	else
	{
		//need not to check if exist, because it can repeated
		// add new pax constraint.
//		char buf[256]; matt
		char buf[2560];
		
		_pNewCon->WriteSyntaxStringWithVersion( buf );
		CMobileElemConstraint* pMobileCon	= new CMobileElemConstraint( m_pInTerm );
		pMobileCon->SetInputTerminal( m_pInTerm );
		pMobileCon->setConstraintWithVersion( buf);
		ConstraintWithProcIDEntry* pNewEntry = new ConstraintWithProcIDEntry();
		ConstantDistribution* pDist = new ConstantDistribution( 0 );
		ProcessorID defaultid;
		defaultid.SetStrDict( m_pInTerm->inStrDict );

		pNewEntry->initialize( pMobileCon, pDist,defaultid );
		m_pConDB->addEntry( pNewEntry );
		ReloadData( pMobileCon );	
	}	
	return true;	
}


BOOL CConWithIDDBListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	CListCtrlEx::OnEndlabeledit( pNMHDR, pResult );
	if(m_bIsDefault)
		if( !m_pConDB )
			return FALSE;

 	LV_ITEM* plvItem = &pDispInfo->item;
	if( plvItem->iSubItem == 2 )
	{
		// change data.
		ProbabilityDistribution* pProbDist = NULL;
		CString s;
		s.LoadString( IDS_STRING_NEWDIST );
		if( strcmp( plvItem->pszText, s ) == 0 )
		{
			CDlgProbDist dlg( m_pInTerm->m_pAirportDB, true );
			if(dlg.DoModal()==IDOK) {
				int idxPD = dlg.GetSelectedPDIdx();
				ASSERT(idxPD!=-1);
				CProbDistEntry* pde = _g_GetActiveProbMan( m_pInTerm )->getItem(idxPD);
				SetItemText( plvItem->iItem, plvItem->iSubItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
			else {
				//cancel
				if(m_bIsDefault)
				{
					const ProbabilityDistribution* pProbDist = m_pConDB->getItem( GetItemData( plvItem->iItem ) )->getValue();
					char szDist[1024];
					pProbDist->screenPrint( szDist);
					SetItemText(plvItem->iItem, plvItem->iSubItem, szDist);
				}
				else
				{
					CGageLagTimeData* pConEntry=(CGageLagTimeData*)m_pGLTConDB->getItem( GetItemData( plvItem->iItem ) );
					const ProbabilityDistribution* pProbDist=pConEntry->GetDistribution();
					char szDist[1024];
					pProbDist->screenPrint( szDist );
					SetItemText(plvItem->iItem, plvItem->iSubItem, szDist);
				}
				return FALSE;
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
			//assert( i < nCount );
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

void CConWithIDDBListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	CConDBListCtrl::OnLButtonDblClk(nFlags, point);
	//////////////////////////////////////////////////////////////////////////
	// do hit_test
	int index, colnum;
	if( (index = HitTestEx(point, &colnum)) != -1
		&& colnum == 1 )	// double click processor colnum
	{
		int iItemData = GetItemData( index ); 
		if( iItemData == -1 )
			return;
		ConstraintWithProcIDEntry* pEntryEx = (ConstraintWithProcIDEntry*)(m_pConDB->getItem( iItemData ));
		
		
		ProcessorID id;
		//ProcessorIDList idList;
		id.SetStrDict( m_pInTerm->inStrDict );

		CSelectALTObjectDialog objDlg(0,m_nPrjID);
		objDlg.SetType(ALT_STAND);
	//	if(objDlg.DoModal()!=IDOK) return ;
		

		if(objDlg.DoModal() == IDOK){

			CString objIDstr;
			if( !objDlg.GetObjectIDString(objIDstr) )return;
			std::vector<CString> strList;
			if( objDlg.GetObjectIDStringList(strList) ){
				int nIDCnt = (int) strList.size();
				for(int i = 0;i<nIDCnt;++i){
					id.setID(strList[i]);
					pEntryEx->setProcID( id );
					SetItemText(index,colnum,strList.at(i));
				}
			}
		}else
		{
			pEntryEx->setProcID( id );
			SetItemText(index,colnum,"");
		}
		
		// set as stand gate
		//CProcesserDialog dlg( m_pInTerm );
		//dlg.SetType( GateProc );
		//dlg.SetType2(StandProcessor);
		//dlg.SetOnlyShowACStandGate(true);
		//if( dlg.DoModal() == IDOK )
		//{
		//	if( dlg.GetProcessorIDList( idList ) )
		//	{
		//		int nIDCount = idList.getCount();
		//		for(int i = 0; i < nIDCount ;i++ )
		//		{
		//			id = *idList[i];
		//			pEntryEx->setProcID( id );
		//			// set item text
		//			SetItemText( index, colnum, id.GetIDString() );
		//		}
		//	}
		//}
		//else
		//{
		//	pEntryEx->setProcID( id );
		//	// set item text
		//	SetItemText( index, colnum, "" );
		//}
	
		GetParent()->SendMessage( UM_LIST_CONTENT_CHANGED );
	}
}

void CConWithIDDBListCtrl::SetProjectID( int nPrjID )
{
	m_nPrjID = nPrjID;
}