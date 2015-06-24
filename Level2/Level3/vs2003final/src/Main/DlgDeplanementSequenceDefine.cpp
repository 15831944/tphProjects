// DlgDeplanementSequenceDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgDeplanementSequenceDefine.h"
#include ".\dlgdeplanementsequencedefine.h"
#include "../Inputs/IN_TERM.H"
#include "../InputOnBoard/OnBoardAnalysisCondidates.h"
#include "../InputOnBoard/DeplanementSequence.h"
#include "../InputOnBoard/Deck.h"
// CDlgDeplanementSequenceDefine dialog
#include "../InputOnBoard/AircaftLayOut.h"
#include "DlgNewPassengerType.h"
#include "../InputOnBoard/OnBoardingCall.h"
IMPLEMENT_DYNAMIC(CDlgDeplanementSequenceDefine, CXTResizeDialog)
CDlgDeplanementSequenceDefine::CDlgDeplanementSequenceDefine(CDeplanementSequence* _seq ,InputTerminal* _input ,COnBoardAnalysisCandidates* _POnblardList ,CWnd* pParent /*=NULL*/)
	:CXTResizeDialog(CDlgDeplanementSequenceDefine::IDD, pParent)
	,m_Deplanement(_seq) 
	,m_input(_input)
	,m_POnboardList(_POnblardList)
	,paxty(_input->inStrDict)
{
}

CDlgDeplanementSequenceDefine::~CDlgDeplanementSequenceDefine()
{
}

void CDlgDeplanementSequenceDefine::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FLIGHT, m_treeFlight);
	
	DDX_Control(pDX,IDC_COMBO_CABIN,m_ComboCabin) ;
	DDX_Control(pDX,IDC_EDIT_PAX,m_PaxTyEdit) ;
	DDX_Control(pDX,IDOK,m_Select) ;

}


BEGIN_MESSAGE_MAP(CDlgDeplanementSequenceDefine, CXTResizeDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLIGHT, OnTvnSelchangedTreeFlight)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_PAX,OnNewPassenger)
END_MESSAGE_MAP()

void CDlgDeplanementSequenceDefine::OnNewPassenger()
{

   CDlgNewPassengerType dlg(&paxty,m_input,this) ;
   dlg.DoModal()  ;
   m_PaxTyEdit.SetWindowText(paxty.PrintStringForShow()) ;
}
BOOL CDlgDeplanementSequenceDefine::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	m_PaxTyEdit.SetWindowText(m_Deplanement->GetPaxType()->PrintStringForShow()) ;
	m_ComboCabin.EnableWindow(FALSE) ;
    m_Select.EnableWindow(FALSE) ;
	InitTree() ;
	SetResize(IDC_STATIC_FLIGHT,SZ_TOP_LEFT,SZ_MIDDLE_RIGHT) ;
	SetResize(IDC_TREE_FLIGHT,SZ_TOP_LEFT,SZ_MIDDLE_RIGHT) ;
	SetResize(IDC_STATIC_CARBIN,SZ_MIDDLE_LEFT,SZ_MIDDLE_RIGHT) ;
	SetResize(IDC_STATIC_CARBIN_CLASS,SZ_MIDDLE_LEFT,SZ_MIDDLE_LEFT) ;
	SetResize(IDC_COMBO_CABIN,SZ_MIDDLE_LEFT,SZ_MIDDLE_RIGHT) ;
	SetResize(IDC_STATIC_PAX,SZ_MIDDLE_LEFT,SZ_MIDDLE_RIGHT) ;
	SetResize(IDC_STATIC_PAX_TYPE,SZ_MIDDLE_LEFT,SZ_MIDDLE_LEFT) ;
	SetResize(IDC_EDIT_PAX,SZ_MIDDLE_LEFT,SZ_MIDDLE_RIGHT) ;
    SetResize(IDC_BUTTON_PAX,SZ_MIDDLE_RIGHT,SZ_MIDDLE_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	return TRUE ;
}
// CDlgDeplanementSequenceDefine message handlers
void CDlgDeplanementSequenceDefine::InitTree()
{
	int onboardsize = m_POnboardList->getCount() ;
	
	CDeckManager* _deckManger = NULL ;
	HTREEITEM FlightITem = NULL ;
	for (int i = 0 ; i < onboardsize ;i++)
	{
		for (int i = 0; i < m_POnboardList->getCount(); i++)
		{
			CFlightTypeCandidate* pData = m_POnboardList->getItem(i);
			char szBuff[1024] = {0};
			pData->GetFltType().screenPrint(szBuff);
			FlightITem = m_treeFlight.InsertItem(_T("Flight Type ")+CString(_T(szBuff))) ;
			for (int j = 0; j < pData->getCount(); j++)
			{

				COnboardFlight* pFlightItem = pData->getItem(j);
				AddItemFlightItem(pFlightItem,FlightITem) ;
			
			}
			m_treeFlight.Expand(FlightITem,TVE_EXPAND) ;
		}
	}
}
CString CDlgDeplanementSequenceDefine::FormatFlightIdNodeForCandidate(const CString& _configName,int _nodeType,const CString& _Actype,CString FlightID)
{
	CString strFlight ;
	if (_nodeType == ArrFlight)
	{
		strFlight.Format(_T("Flight %s - ACTYPE %s Arr"),FlightID,_Actype);
	}
	else
	{
		strFlight.Format(_T("Flight %s - ACTYPE %s Dep"),FlightID,_Actype);
	}    
	if(!_configName.IsEmpty())
	{
		CString config;
		config.Format("(%s)",_configName) ;
		strFlight.Append(config) ;
	}
	return strFlight ;
}

void CDlgDeplanementSequenceDefine::AddItemFlightItem(COnboardFlight* _Analysis ,HTREEITEM _item)
{
	if(_Analysis == NULL )
		return ;
	CAircaftLayOut* _layout   = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(_Analysis->getID());
	if(_layout == NULL)
		return ;
	
	CString str = FormatFlightIdNodeForCandidate(_layout->GetName(),_Analysis->getFlightType(),_Analysis->getACType(),_Analysis->getFlightID()) ;
	HTREEITEM DeckItem = m_treeFlight.InsertItem(str,_item) ;
	m_treeFlight.SetItemData(DeckItem,_Analysis->getID()) ;
	CDeckManager* _deckManger = NULL ;
	if(_layout != NULL)
	{
		_deckManger = _layout->GetDeckManager() ;
		for (int j = 0 ; j < _deckManger->getCount() ; j++)
		{
            AddDeckItem(_deckManger->getItem(j),DeckItem) ;
		}
		m_treeFlight.Expand(DeckItem,TVE_EXPAND) ;
	}
}
void CDlgDeplanementSequenceDefine::AddDeckItem(CDeck* _deck , HTREEITEM _item)
{
	if(_deck == NULL || _item == NULL)
		return ;
	HTREEITEM Item = m_treeFlight.InsertItem(_deck->GetName(),_item) ;
	m_treeFlight.SetItemData(Item,(DWORD_PTR)_deck) ;
}
void CDlgDeplanementSequenceDefine::InitComboBox()
{
	  m_ComboCabin.ResetContent();
      HTREEITEM item = m_treeFlight.GetSelectedItem() ;
	  int filghtid = m_treeFlight.GetItemData(m_treeFlight.GetParentItem(item)) ;
	  COnBoardSeatBlockItem* blockItem = NULL  ;
	  COnBoardSeatBlockList  PonboardSeatBlockList ;
	  PonboardSeatBlockList.ReadData(filghtid) ;
	  int ndx = 0 ;
	  for (int i = 0; i < PonboardSeatBlockList.getCount(); i++)
	  {
		  CString strValue ;
		   blockItem = PonboardSeatBlockList.getItem(i) ;
		   ndx = m_ComboCabin.AddString(blockItem->getBlockName()) ;
		   m_ComboCabin.SetItemData(ndx,blockItem->getID()) ;
	  }
	  m_ComboCabin.SetCurSel(0) ;
}
void CDlgDeplanementSequenceDefine::OnTvnSelchangedTreeFlight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM item = m_treeFlight.GetSelectedItem() ;
	if(!m_treeFlight.ItemHasChildren(item))
	{
		m_Select.EnableWindow(TRUE) ;
		m_ComboCabin.EnableWindow(TRUE) ;
		InitComboBox() ;
	}
	else
	{
		m_Select.EnableWindow(FALSE) ;
		m_ComboCabin.EnableWindow(FALSE) ;
	}
	*pResult = 0;
}
void CDlgDeplanementSequenceDefine::OnOK()
{
	HTREEITEM item = m_treeFlight.GetSelectedItem() ;
	CDeck* _deck = (CDeck*)m_treeFlight.GetItemData(item) ;
	if(_deck != NULL )
	{
		CString val ;
        m_PaxTyEdit.GetWindowText(val) ;
		m_Deplanement->SetDeckID(_deck->GetID()) ;
		m_ComboCabin.GetWindowText(val) ;
		int ndx = m_ComboCabin.GetCurSel() ;
		m_Deplanement->SetCarbinClass(m_ComboCabin.GetItemData(ndx)) ;
		m_Deplanement->GetPaxType()->FormatLevelPaxTypeFromString(paxty.GetStringForDatabase()) ;
	}
	CXTResizeDialog::OnOK() ;
}
void CDlgDeplanementSequenceDefine::OnCancel()
{
	CXTResizeDialog::OnCancel() ; 
}

CDlgDeplanementSequenceEdit::CDlgDeplanementSequenceEdit(CDeplanementSequence* _seq ,InputTerminal* _input ,COnBoardAnalysisCandidates* _POnblardList ,CWnd* pParent):CDlgDeplanementSequenceDefine(_seq,_input,_POnblardList,pParent)
{

}
CDlgDeplanementSequenceEdit::~CDlgDeplanementSequenceEdit()
{

}
BOOL CDlgDeplanementSequenceEdit::OnInitDialog()
{
	CDlgDeplanementSequenceDefine::OnInitDialog() ;
	SetWindowText(_T("Deplanement Sequence Edit")) ;
	return TRUE ;
}