// DlgBlockSelect.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgBlockSelect.h"
//#include "../Common/AirportDatabase.h"
//#include "../InputOnBoard/OnBoardAnalysisCondidates.h"
#include "../Inputs/IN_TERM.H"
// CDlgFlightSelect dialog
#include "../InputOnBoard/OnBoardingCall.h"
IMPLEMENT_DYNAMIC(CDlgFlightSelect, CXTResizeDialog)
CDlgFlightSelect::CDlgFlightSelect(int nProjID,InputTerminal * pInterm,COnBoardingCall* boardcall , CAirportDatabase* pAirportDB,COnBoardAnalysisCandidates* _pOnBoardList,CWnd* pParent )
	: CXTResizeDialog(CDlgFlightSelect::IDD, pParent)
	,m_pOnBoardList(_pOnBoardList)
	,m_boardingCall(boardcall)
	,m_nProjID(nProjID)
	,m_pInterm(pInterm)
	,m_selFlight(NULL)
{
	
}

CDlgFlightSelect::~CDlgFlightSelect()
{

}

void CDlgFlightSelect::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_BLOCK, m_wndTreeCtrl);
	DDX_Control(pDX, IDOK, m_ButtonSel);
}

BOOL CDlgFlightSelect::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	OnInitTreeCtrl();
	m_ButtonSel.EnableWindow(FALSE) ;
	SetResize(IDC_TREE_BLOCK,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	return TRUE ;
}
void CDlgFlightSelect::OnInitTreeCtrl()
{
	if(m_boardingCall == NULL)
		return ;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hItem = m_wndTreeCtrl.InsertItem(_T("Flight Select"),cni,FALSE);
	for (int i = 0; i < m_boardingCall->getCount(); i++)
	{
		CString strValue ;
		COnBoardingCallStage* pStage = m_boardingCall->getItem(i);
		for (int j = 0; j < pStage->getCount(); j++)
		{
			COnBoardingCallFltType* pFltType = pStage->getItem(j);
			FlightConstraint fltType = pFltType->GetFltType();
			char szBuffer[1024] = {0};
			fltType.screenPrint(szBuffer);
			strValue.Format(_T("Flight Type: %s"),szBuffer);
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			HTREEITEM hFltTypeItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE);
			m_wndTreeCtrl.SetItemData(hFltTypeItem,(DWORD_PTR)NULL);
            COnBoardSeatBlockList* blocklist = NULL ;
			for (int n = 0; n < pFltType->getCount(); n++)
			{
				cni.unMenuID = IDR_MENU_FLIGHT_TYPE;
				COnBoardingCallFlight* pFlight = pFltType->getItem(n);
				COnboardFlight& pFlightAnalysis = pFlight->GetFlightAnalysis();
				if (pFlightAnalysis.getFlightType() == ArrFlight)
				{
					strValue.Format(_T("Flight %s - ACTYPE %s Arr"),pFlightAnalysis.getFlightID(),pFlightAnalysis.getACType());
				}
				else
				{
					strValue.Format(_T("Flight %s - ACTYPE %s Dep"),pFlightAnalysis.getFlightID(),pFlightAnalysis.getACType());
				}
				HTREEITEM hFlightItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hFltTypeItem);
				m_wndTreeCtrl.SetItemData(hFlightItem,(DWORD_PTR)pFlight);
				m_wndTreeCtrl.Expand(hFlightItem,TVE_EXPAND);
			}
			m_wndTreeCtrl.Expand(hFltTypeItem,TVE_EXPAND);
		}
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}
BEGIN_MESSAGE_MAP(CDlgFlightSelect, CXTResizeDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BLOCK, OnTvnSelchangedTreeBlock)
END_MESSAGE_MAP()


// CDlgFlightSelect message handlers
void CDlgFlightSelect::OnTvnSelchangedTreeBlock(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM selitem = m_wndTreeCtrl.GetSelectedItem() ;
	if(selitem == NULL)
		return ;
	if(!m_wndTreeCtrl.ItemHasChildren(selitem) )
		m_ButtonSel.EnableWindow(TRUE) ;
	else
		m_ButtonSel.EnableWindow(FALSE) ;
	*pResult = 0;
}
void CDlgFlightSelect::OnOK()
{
	HTREEITEM selitem = m_wndTreeCtrl.GetSelectedItem() ;
	if(selitem == NULL)
		return ;
	m_selFlight = (COnBoardingCallFlight*)m_wndTreeCtrl.GetItemData(selitem) ;
	CDialog::OnOK() ;
}
void CDlgFlightSelect::OnCancel()
{
	m_selFlight = NULL ;
    CDialog::OnCancel() ;
}

CDlgBlockSelect::CDlgBlockSelect(COnBoardingCallFlight* _flight,CWnd* pParent )
:CDlgFlightSelect(pParent),m_selBlock(NULL),m_flight(_flight)
{
 m_selFlight = _flight ;
}
CDlgBlockSelect::~CDlgBlockSelect()
{

}
void CDlgBlockSelect::OnInitTreeCtrl()
{
	if(m_flight == NULL)
		return ;
	CString strValue ;
	strValue.Format(_T("Flight %d - ACTYPE %s Arr"),m_flight->GetID(),m_flight->GetFlightAnalysis().getACType());
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	HTREEITEM hItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE);
	m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)m_flight) ;
	for (int i = 0; i <  m_flight->getSeatBlockList().getCount(); i++)
	{
		AddBlockItem(m_flight->getSeatBlockList().getItem(i),hItem) ;

	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}
void CDlgBlockSelect::AddBlockItem(COnBoardSeatBlockItem* _block , HTREEITEM _item)
{
	if(_block == NULL || _item == NULL)
		return ;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hblockItem = m_wndTreeCtrl.InsertItem(_block->getBlockName(),cni,FALSE,FALSE,_item);
	m_wndTreeCtrl.SetItemData(hblockItem,(DWORD_PTR)_block) ;
}
void CDlgBlockSelect::OnOK()
{
	HTREEITEM selitem = m_wndTreeCtrl.GetSelectedItem() ;
	if(selitem == NULL)
		return ;
	m_selBlock =(COnBoardSeatBlockItem*) m_wndTreeCtrl.GetItemData(selitem) ;
	HTREEITEM parItem = m_wndTreeCtrl.GetParentItem(selitem) ;
	m_selFlight = (COnBoardingCallFlight*)m_wndTreeCtrl.GetItemData(parItem) ;
	CDialog::OnOK() ;
}
BOOL CDlgBlockSelect::OnInitDialog()
{
	CDlgFlightSelect::OnInitDialog() ;
	SetWindowText(_T("Block Select Dialog")) ;
	return TRUE ;
}