#include "StdAfx.h"
#include ".\curbsidefacilitydatapage.h"
#include "Landside\CurbsideBehavior.h"
#include "Common\ProbDistEntry.h"
#include "Landside\InputLandside.h"
#include "Common\ProbDistManager.h"
#include "Inputs\Probab.h"
#include "Inputs\In_term.h"
#include "DlgProbDist.h"


CCurbsideFacilityDataPage::CCurbsideFacilityDataPage(InputLandside *pInputLandside,
													 InputTerminal* _pInTerm,
													 CFacilityBehaviorList *pBehaviorDB,
													 enum ALTOBJECT_TYPE enumFacType,
													 UINT nIDCaption,
													 int nProjectID)
													:CFacilityDataPage(pInputLandside, _pInTerm, pBehaviorDB, enumFacType, nIDCaption, nProjectID)
													,m_hStoppingTime(NULL)
{

}

CCurbsideFacilityDataPage::~CCurbsideFacilityDataPage(void)
{
}

void CCurbsideFacilityDataPage::LoadTreeProperties()
{
	CFacilityDataPage::LoadTreeProperties();


	CFacilityBehavior *pBehavior = GetCurrentBehavior();
	if(pBehavior == NULL)
	{
		return;
	}

	CCurbsideBehavior *pCurbsideBehavior = (CCurbsideBehavior *)pBehavior;

	if(pCurbsideBehavior == NULL)
		return;


	CString strPorbDist = pCurbsideBehavior->GetProDist().getPrintDist();

	CString strIntemText;
	strIntemText.Format(_T("Max stopping time(seconds): %s"), strPorbDist);
	//Max stopping time

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net = NET_COMBOBOX;
	m_hStoppingTime = m_TreeData.InsertItem(strIntemText, cni, FALSE, FALSE, TVI_ROOT);

}

LRESULT CCurbsideFacilityDataPage::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	CString s("NEW PROBABILITY DISTRIBUTION");

	if (message == 	UM_CEW_COMBOBOX_END)
	{

	}
	else if(message == UM_CEW_COMBOBOX_BEGIN)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		if(hItem == m_hStoppingTime)
		{
			CWnd* pWnd=m_TreeData.GetEditWnd((HTREEITEM)wParam);
			CComboBox* pCB=(CComboBox*)pWnd;
			if(pCB)
			{	
				CRect rectItem;
				m_TreeData.GetItemRect((HTREEITEM)wParam,rectItem,TRUE);
				pWnd->SetWindowPos(NULL,rectItem.right,rectItem.top,0,0,SWP_NOSIZE);
				pCB->SetDroppedWidth(160);
				pCB->ResetContent();
				//---------------------------------------------------------------------------------------
				CStringList strList;

				pCB->AddString(s);
				int nCount = _g_GetActiveProbMan((InputTerminal *)(m_pInputLandside->getTerminal()))->getCount();
				for( int m=0; m<nCount; m++ )
				{
					CProbDistEntry* pPBEntry = _g_GetActiveProbMan((InputTerminal *)(m_pInputLandside->getTerminal()))->getItem( m );			
					pCB->AddString(pPBEntry->m_csName);
				}
				//---------------------------------------------------------------------------------------
			}
		}
	}
	else if(message == UM_CEW_COMBOBOX_SELCHANGE)
	{
		CFacilityBehavior *pBehavior = GetCurrentBehavior();
		CCurbsideBehavior *pCurbsideBehavior = (CCurbsideBehavior *)pBehavior;
		InputTerminal *pInputTerm = (InputTerminal *)m_pInputLandside->getTerminal();

		if(pCurbsideBehavior != NULL && pInputTerm != NULL)
		{

			HTREEITEM hItemSeled=(HTREEITEM)wParam;
			int nIndexSeled=m_TreeData.GetCmbBoxCurSel(hItemSeled);

			ProbabilityDistribution* pProbDist = NULL;

			if( nIndexSeled == 0 ) 
			{
				CDlgProbDist dlg(pInputTerm->m_pAirportDB, true );
				if(dlg.DoModal()==IDOK)
				{
					int idxPD = dlg.GetSelectedPDIdx();
					ASSERT(idxPD!=-1);
					CProbDistEntry* pde = _g_GetActiveProbMan( ((InputTerminal *)(m_pInputLandside->getTerminal())) )->getItem(idxPD);
					m_TreeData.SetItemText( hItemSeled, pde->m_csName );
					pCurbsideBehavior->SetProDist(pde);
					UpdateStoppingTimeItem();
					SetModified();
				}
				else
				{
					
				}
			}
			else
			{
				CProbDistEntry* pPDEntry = NULL;
				int nCount = _g_GetActiveProbMan( pInputTerm)->getCount();

				if(nIndexSeled > 0 && nIndexSeled <= nCount)
				{
					pPDEntry = _g_GetActiveProbMan( pInputTerm )->getItem( nIndexSeled - 1 );
					pCurbsideBehavior->SetProDist(pPDEntry);
					UpdateStoppingTimeItem();
					SetModified();
				}
			}
		}
	}
	return CFacilityDataPage::DefWindowProc(message, wParam, lParam);
}

void CCurbsideFacilityDataPage::UpdateStoppingTimeItem()
{
	if(m_hStoppingTime)
	{
		CCurbsideBehavior *pCurbsideBehavior = (CCurbsideBehavior *)GetCurrentBehavior();
		if(pCurbsideBehavior)
		{
			CString strPorbDist = pCurbsideBehavior->GetProDist().getPrintDist();

			CString strIntemText;
			strIntemText.Format(_T("Max stopping time(seconds): %s"), strPorbDist);

			m_TreeData.SetItemText(m_hStoppingTime, strIntemText);
		}
	}
}














