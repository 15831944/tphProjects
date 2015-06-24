// DlgDeplanementSequenceSpec.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgDeplanementSequenceSpec.h"

#include "../InputOnBoard/OnBoardAnalysisCondidates.h"
#include "../InputOnBoard/DeplanementSequence.h"
#include "../InputOnBoard/Deck.h"
// CDlgDeplanementSequenceSpec dialog
#include "DlgDeplanementSequenceDefine.h"
#include "../InputOnBoard/AircaftLayOut.h"
#include "../InputOnBoard/OnBoardingCall.h"
#include "../Inputs/IN_TERM.H"


IMPLEMENT_DYNAMIC(CDlgDeplanementSequenceSpec, CXTResizeDialog)
CDlgDeplanementSequenceSpec::CDlgDeplanementSequenceSpec( COnBoardAnalysisCandidates* _pOnBoardList ,InputTerminal* _input ,CWnd* pParent /*=NULL*/)
	:m_pOnBoardList(_pOnBoardList), m_Input(_input),CXTResizeDialog(CDlgDeplanementSequenceSpec::IDD, pParent)
{
	m_Deplanement = new CDeplanementSequenceDataSet ;
}

CDlgDeplanementSequenceSpec::~CDlgDeplanementSequenceSpec()
{
	if(m_Deplanement != NULL)
		delete m_Deplanement ;
}

void CDlgDeplanementSequenceSpec::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEQUENCE, m_listCtrl);
	DDX_Control(pDX,IDC_STATIC_TOOL,m_ToolEdit) ;
}

void CDlgDeplanementSequenceSpec::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
	if(m_listCtrl.GetSafeHwnd())
		m_listCtrl.ResizeColumnWidth() ;
}
BEGIN_MESSAGE_MAP(CDlgDeplanementSequenceSpec, CDialog)
	ON_WM_SIZE()
    ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SEQUENCE, OnLvnItemchangedList)
	ON_COMMAND(ID_DEPLANEMENT_NEW,OnAddDeplanementSeq)
	ON_COMMAND(ID_DEPLANEMENT_DEL,OnDelDeplanementSeq)
	ON_COMMAND(ID_DEPLANEMENT_EDIT,OnEditDeplanementSeq)

	ON_COMMAND(ID_DEPLANEMENT_PRIO_ADD ,OnMoveListItemUp) 
	ON_COMMAND(ID_DEPLANEMENT_PRIO_DOWN,OnMoveListItemDown)
END_MESSAGE_MAP()
int CDlgDeplanementSequenceSpec::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1 ;
	if (!m_ToolBar.CreateEx(this,  TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP ) ||
		!m_ToolBar.LoadToolBar(IDR_ONBOARD_DEPLANCE_SEQ))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}
void CDlgDeplanementSequenceSpec::InitToolBar()
{
  CRect rect ;
  m_ToolEdit.GetWindowRect(&rect) ;
  ScreenToClient(&rect) ;
  m_ToolBar.MoveWindow(&rect) ;
  m_ToolEdit.ShowWindow(SW_HIDE) ;

  m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_NEW,TRUE);
  m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_DEL,FALSE) ;
  m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_EDIT,FALSE) ;
   m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_PRIO_ADD,FALSE) ;
    m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_PRIO_DOWN,FALSE) ;
}
BOOL CDlgDeplanementSequenceSpec::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	m_Deplanement->ReadData(m_Input->inStrDict) ;
	OnInitList() ;
	InitToolBar() ;
	SetResize(m_ToolBar.GetToolBarCtrl().GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_LIST_SEQUENCE,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK ,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	return TRUE ;
}
void CDlgDeplanementSequenceSpec::OnInitList()
{
	InitView() ;
	InitData() ;
}
void CDlgDeplanementSequenceSpec::InitView()
{
	DWORD dwStyle = m_listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrl.SetExtendedStyle( dwStyle );

	TCHAR* columnName[] = {"Seat Block","Deck","Passenger Type"} ;
	int columnWidth[] = {150,250,250} ;
	for (int i = 0 ; i < 4 ; i++)
	{
		m_listCtrl.InsertColumn(i,columnName[i],LVCFMT_CENTER,columnWidth[i]) ;
	}
	m_listCtrl.InitColumnWidthPercent() ;
}
void CDlgDeplanementSequenceSpec::InitData()
{
	m_listCtrl.DeleteAllItems() ;
	CDeplanementSequenceDataSet::TY_DATA* m_Data = m_Deplanement->GetDeplanmentSeqenceData() ;
	std::sort(m_Data->begin(),m_Data->end(),CSortFunction()) ;

	CDeplanementSequenceDataSet::TY_DATA_ITER iter = m_Data->begin() ;
	for ( ; iter != m_Data->end() ; iter++)
	{
		AddItemToList(*iter) ;
	}
}
void CDlgDeplanementSequenceSpec::AddItemToList( CDeplanementSequence* _deplane ,int ndx) 
{
	if(_deplane == NULL)
		return ;
     int index = -1 ;
	   COnBoardSeatBlockItem* blockitem = GetBlockByID(_deplane->GetCarbinClass()) ;
	 if(ndx == -1)
	    index = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(),blockitem == NULL ? "No Defined":blockitem->getBlockName()) ;
	 else
	 {
		 index = ndx ;
		 m_listCtrl.InsertItem(index,blockitem == NULL ? "No Defined":blockitem->getBlockName()) ;
	 }
	 CDeck* _deck = GetDeckByID(_deplane->GetDeckID()) ;
	 m_listCtrl.SetItemText(index,1,_deck == NULL ? "No Defined":_deck->GetName()) ;
	 m_listCtrl.SetItemText(index,2,_deplane->GetPaxType()->PrintStringForShow()) ;
	 m_listCtrl.SetItemData(index,(DWORD_PTR)_deplane) ;
}
void CDlgDeplanementSequenceSpec::EditItemList(CDeplanementSequence* _deplane ,int ndx)
{
	if(_deplane == NULL)
		return ;
   CDeck* _deck = GetDeckByID(_deplane->GetDeckID()) ;
   COnBoardSeatBlockItem* blockitem = GetBlockByID(_deplane->GetCarbinClass()) ;
   m_listCtrl.SetItemText(ndx,0,blockitem == NULL ? "No Defined":blockitem->getBlockName()) ;
   m_listCtrl.SetItemText(ndx,1,_deck == NULL ? "No Defined":_deck->GetName()) ;
   m_listCtrl.SetItemText(ndx,2,_deplane->GetPaxType()->PrintStringForShow()) ;
   m_listCtrl.SetItemData(ndx,(DWORD_PTR)_deplane) ;
}
void CDlgDeplanementSequenceSpec::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION position = m_listCtrl.GetFirstSelectedItemPosition() ;
	if(position != NULL)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_NEW,TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_DEL,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_EDIT,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_PRIO_ADD,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_PRIO_DOWN,TRUE) ;
	}else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_NEW,TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_DEL,FALSE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_EDIT,FALSE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_PRIO_ADD,FALSE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_DEPLANEMENT_PRIO_DOWN,FALSE) ;
	}

}
void CDlgDeplanementSequenceSpec::OnAddDeplanementSeq()
{
    CDeplanementSequence* Seq = new CDeplanementSequence(m_Input->inStrDict) ;
    CDlgDeplanementSequenceDefine dlgDefine(Seq,m_Input,m_pOnBoardList,this) ;
	if(dlgDefine.DoModal() == IDOK)
	{
		m_Deplanement->AddDeplanmentSeqence(Seq) ;
		AddItemToList(Seq) ;
	}else
		delete Seq ;
}
void CDlgDeplanementSequenceSpec::OnDelDeplanementSeq()
{
	POSITION position = m_listCtrl.GetFirstSelectedItemPosition() ;
	if(position == NULL)
	{
		MessageBox(_T("No items were selected!\n"),_T("Warning"),MB_OK) ;
		return ;
	}
	int index = m_listCtrl.GetNextSelectedItem(position) ;
    CDeplanementSequence*  _seq = (CDeplanementSequence*)m_listCtrl.GetItemData(index) ;
	m_Deplanement->DelDeplanmentSeqence(_seq) ;
	m_listCtrl.DeleteItem(index) ;
}
void CDlgDeplanementSequenceSpec::OnEditDeplanementSeq()
{
	POSITION position = m_listCtrl.GetFirstSelectedItemPosition() ;
	if(position == NULL)
	{
		MessageBox(_T("No items were selected!\n"),_T("Warning"),MB_OK) ;
		return ;
	}
	int index = m_listCtrl.GetNextSelectedItem(position) ;
	CDeplanementSequence*  _seq = (CDeplanementSequence*)m_listCtrl.GetItemData(index) ;
	CDlgDeplanementSequenceEdit editDlg(_seq,m_Input,m_pOnBoardList,this) ;
    editDlg.DoModal() ;
     EditItemList(_seq,index) ;
}
CDeck* CDlgDeplanementSequenceSpec::GetDeckByID(int _id)
{
	int onboardsize = m_pOnBoardList->getCount() ;
		CAircaftLayOut* _layout = NULL ;
		CDeckManager* _deckManger = NULL ;
		for (int i = 0 ; i < onboardsize ;i++)
		{
			for (int i = 0; i < m_pOnBoardList->getCount(); i++)
			{
				CFlightTypeCandidate* pData = m_pOnBoardList->getItem(i);
				for (int j = 0; j < pData->getCount(); j++)
				{

					COnboardFlight* pFlightItem = pData->getItem(j);
				   _layout   = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pFlightItem->getID());
				   if(_layout != NULL)
				   {
                       _deckManger = _layout->GetDeckManager() ;
					   for (int j = 0 ; i < _deckManger->getCount() ; j++)
					   {
						   if(_deckManger->getItem(j)->GetID() == _id)
							   return _deckManger->getItem(j) ;
					   }
				   }
				}
			}
		}
		return NULL ;
}
COnBoardSeatBlockItem* CDlgDeplanementSequenceSpec::GetBlockByID(int _id)
{
	COnBoardSeatBlockItem* blockItem =NULL ;
	COnBoardSeatBlockList OnboardSeatBlockList ;
	for (int i = 0; i < OnboardSeatBlockList.getCount(); i++)
	{
		CString strValue ;
		blockItem = OnboardSeatBlockList.getItem(i) ;
		if(blockItem->getID() == _id)
			return blockItem ;
	}
	return NULL ;
}
void CDlgDeplanementSequenceSpec::OnMoveListItemUp()
{
	POSITION position = m_listCtrl.GetFirstSelectedItemPosition() ;
	if(position == NULL)
	{
		MessageBox(_T("No items were selected!\n"),_T("Warning"),MB_OK) ;
		return ;
	}
   int index = m_listCtrl.GetNextSelectedItem(position) ; 
   CDeplanementSequence*  _seq = (CDeplanementSequence*)m_listCtrl.GetItemData(index) ;
   if(index == 0)
	   return ;
   m_listCtrl.DeleteItem(index) ;
   index-- ;
   AddItemToList(_seq,index) ;
   UINT flag = LVIS_SELECTED|LVIS_FOCUSED;
   m_listCtrl.SetItemState(index, flag, flag);
}
void CDlgDeplanementSequenceSpec::OnMoveListItemDown()
{
	POSITION position = m_listCtrl.GetFirstSelectedItemPosition() ;
	if(position == NULL)
	{
		MessageBox(_T("No items were selected!\n"),_T("Warning"),MB_OK) ;
		return ;
	}
	int index = m_listCtrl.GetNextSelectedItem(position) ; 
	 CDeplanementSequence*  _seq = (CDeplanementSequence*)m_listCtrl.GetItemData(index) ;
	if(index >= m_listCtrl.GetItemCount() - 1)
		return ;
	 m_listCtrl.DeleteItem(index) ;
	index++ ;
	AddItemToList(_seq,index) ;
	UINT flag = LVIS_SELECTED|LVIS_FOCUSED;
	m_listCtrl.SetItemState(index, flag, flag);
}
void CDlgDeplanementSequenceSpec::OnOK()
{
	CDeplanementSequence* _seq = NULL ;
	for (int i = 0 ; i < m_listCtrl.GetItemCount() ;i++)
	{
		_seq = (CDeplanementSequence*)m_listCtrl.GetItemData(i) ;
		if(_seq != NULL)
			_seq->SetPriority(i+1) ;
	}
	try
	{
		CADODatabase::BeginTransaction() ;
		m_Deplanement->SaveData() ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	CXTResizeDialog::OnOK() ;
}
void CDlgDeplanementSequenceSpec::OnCancel()
{
	//m_Deplanement->ReadData() ;
	CXTResizeDialog::OnCancel() ;
}
// CDlgDeplanementSequenceSpec message handlers
