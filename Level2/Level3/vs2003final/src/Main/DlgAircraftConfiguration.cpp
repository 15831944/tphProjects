// DlgAircraftConfiguration.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftConfiguration.h"
#include "../InputOnBoard/AircaftLayOut.h"
#include "../Inputs/IN_TERM.H"
#include "DlgAircraftConfigurationNameDefine.h"
#include "DlgAircraftConfigurationSpecView.h"
// CDlgAircraftConfiguration dialog
#include "../InputOnBoard/CInputOnboard.h"
#include "../InputOnBoard/AircraftLayoutEditContext.h"
#include "TermPlanDoc.h"
#include "Onboard/OnboardViewMsg.h"
#include "Onboard/AircraftLayOutFrame.h"
#include "Onboard/AircraftLayout3DView.h"
#include "MainFrm.h"
#include "LayoutView.h"
#include "onboard/AircraftLayoutEditor.h"

IMPLEMENT_DYNAMIC(CDlgAircraftConfiguration, CXTResizeDialog)
CDlgAircraftConfiguration::CDlgAircraftConfiguration(const CString& _actype,int Flightid ,InputTerminal* _input , CWnd* pParent /*=NULL*/)
	: m_ACType(_actype),m_terminal(_input),CXTResizeDialog(CDlgAircraftConfiguration::IDD, pParent)
{
	m_ColumName.push_back("Index") ;
	m_ColumName.push_back("Level1 Name") ;
	m_ColumName.push_back("Level2 Name") ;
	m_ColumName.push_back("Level3 Name") ;
//	m_ColumName.push_back("Level4 Name") ;
	m_ColumName.push_back("AcType") ;
	m_ColumName.push_back("Create Time") ;
	m_ColumName.push_back("Modify Time") ;
	m_ColumName.push_back("Creator") ;
	m_flightId = Flightid ;
	m_LayOutManager = CAircarftLayoutManager::GetInstance() ;
	  pActiveDoc= (CTermPlanDoc*)((CView*)pParent)->GetDocument();
}

CDlgAircraftConfiguration::~CDlgAircraftConfiguration()
{
}

void CDlgAircraftConfiguration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_CON,m_Static_ToolBar) ;
	DDX_Control(pDX,IDC_LIST_CONFIG,m_list) ;
}


BEGIN_MESSAGE_MAP(CDlgAircraftConfiguration, CXTResizeDialog)
	ON_WM_SIZE()
	ON_WM_CREATE() 
    ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_CONFIG,OnLvnItemchangedListRules)
	ON_COMMAND(ID_AIRCRAFT_CONFIG_NEW,OnNewAircraftConfig)
	ON_COMMAND(ID_AIRCRAFT_CONFIG_DELETE,OnDelAircraftConfig)
	ON_COMMAND(ID_AIRCRAFT_CONFIG_EDIT,OnEditAircraftConfig)
	ON_COMMAND(ID_SELECT,OnSelect)
END_MESSAGE_MAP()
void CDlgAircraftConfiguration::OnLvnItemchangedListRules(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION posi = m_list.GetFirstSelectedItemPosition() ;
	if(posi != NULL)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_CONFIG_DELETE,TRUE) ;
		m_toolBar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_CONFIG_EDIT,TRUE) ;
	}else
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_CONFIG_DELETE,FALSE) ;
		m_toolBar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_CONFIG_EDIT,FALSE) ;
	}

}
void CDlgAircraftConfiguration::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
	if(m_list.GetSafeHwnd())
		m_list.ResizeColumnWidth() ;
}
int CDlgAircraftConfiguration::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here

	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP) ||
		!m_toolBar.LoadToolBar(IDR_AIRCRAFT_CONFIGURATION))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}
BOOL CDlgAircraftConfiguration::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	InitToolBar() ;
	OnInitList() ;
    SetResize(IDC_STATIC_CONFIG,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_STATIC_CON,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(m_toolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_CONFIG,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(ID_SELECT,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
    return TRUE ;
}
void CDlgAircraftConfiguration::OnInitList()
{
	OnInitListView();
	OnInitListData() ;
}
void CDlgAircraftConfiguration::OnInitListView()
{
	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle( dwStyle );
	int nwidth[] = {45,95,95,95,150,90,90,50} ;
	for (int i = 0 ; i < (int)m_ColumName.size() ; i++)
	{
		m_list.InsertColumn(i,m_ColumName[i],LVCFMT_CENTER,nwidth[i]) ;
	}
	m_list.InitColumnWidthPercent() ;
}
void CDlgAircraftConfiguration::OnInitListData()
{
	CAircaftLayOut* _layout = NULL ;
	int index = 0 ;
    for (int i = 0 ; i < m_LayOutManager->getCount() ; i++)
    {
      _layout = m_LayOutManager->getItem(i) ;
	  if(strcmp(_layout->GetAcType(),m_ACType) == 0)
	  {
		  index++ ;
		  InsertListItem(_layout,index) ;
	  }
    }
}
void CDlgAircraftConfiguration::InsertListItem(CAircaftLayOut* _layout,int _index)
{
    if(_layout == NULL)
		return ;
	CString index ;
	index.Format("%d",_index) ;
	int col = m_list.InsertItem(_index,index) ;
	m_list.SetItemText(col,1,_layout->GetLevel1Name()) ;
	m_list.SetItemText(col,2,_layout->GetLevel2Name()) ;
	m_list.SetItemText(col,3,_layout->GetLevel3Name()) ;
//	m_list.SetItemText(col,4,_layout->GetLevel4Name()) ;
	m_list.SetItemText(col,4,_layout->GetAcType()) ;
	m_list.SetItemText(col,5,_layout->GetCreateTime()) ;
	m_list.SetItemText(col,6,_layout->GetModifyTime()) ;
	m_list.SetItemText(col,7,_layout->GetCreator()) ;
	m_list.SetItemData(col,(DWORD_PTR)_layout) ;
}
void CDlgAircraftConfiguration::EditListItem(CAircaftLayOut* _layout,int col)
{
	if(_layout == NULL)
		return ;
	m_list.SetItemText(col,1,_layout->GetLevel1Name()) ;
	m_list.SetItemText(col,2,_layout->GetLevel2Name()) ;
	m_list.SetItemText(col,3,_layout->GetLevel3Name()) ;
//	m_list.SetItemText(col,4,_layout->GetLevel4Name()) ;
	m_list.SetItemText(col,4,_layout->GetAcType()) ;
	m_list.SetItemText(col,5,_layout->GetCreateTime()) ;
	m_list.SetItemText(col,6,_layout->GetModifyTime()) ;
	m_list.SetItemText(col,7,_layout->GetCreator()) ;
	m_list.SetItemData(col,(DWORD_PTR)_layout) ;
}
void CDlgAircraftConfiguration::InitToolBar()
{
	CRect rect ;
	m_Static_ToolBar.GetWindowRect(&rect) ;
	ScreenToClient(rect) ;
	m_Static_ToolBar.ShowWindow(SW_HIDE) ;
	m_toolBar.MoveWindow(rect) ;
	m_toolBar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_CONFIG_NEW,TRUE) ;
	m_toolBar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_CONFIG_DELETE,FALSE);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_AIRCRAFT_CONFIG_EDIT,FALSE) ;
}
void CDlgAircraftConfiguration::OnNewAircraftConfig()
{
	CAircaftLayOut* _layout = new CAircaftLayOut;
	_layout->SetAcType(m_ACType);
   CDlgAircraftConfigurationNameDefine nameedit(_layout,m_terminal,this);
   if(nameedit.DoModal() == IDOK)
   {
	   m_LayOutManager->addItem(_layout) ;
	   //CDlgAircraftConfigurationSpecView dlgconfigView(_layout,m_terminal,pActiveDoc,this) ;
	   //if(dlgconfigView.DoModal() == IDOK)
	   //{
		   InsertListItem(_layout,m_list.GetItemCount()+1) ;
	 //  }
   }
}
void CDlgAircraftConfiguration::OnDelAircraftConfig()
{
	POSITION posi = m_list.GetFirstSelectedItemPosition() ;
	CAircaftLayOut* _layout = NULL ;
	if(posi!= NULL)
	{
		int ndx = m_list.GetNextSelectedItem(posi) ;
		_layout = (CAircaftLayOut*)m_list.GetItemData(ndx);
		if(pActiveDoc->GetAircraftLayoutEditor()->GetEditLayOut() == _layout )
		{
			MessageBox(_T("This AircraftLayout is being used,Please close the Layoutview first! "),"Warning",MB_OK) ;
			return ;
		}
		m_LayOutManager->RemoveLayOut(_layout) ;
		m_list.DeleteItem(ndx) ;
	}
}
void CDlgAircraftConfiguration::OnEditAircraftConfig()
{
	POSITION posi = m_list.GetFirstSelectedItemPosition() ;
	CAircaftLayOut* _layout = NULL ;
	if(posi!= NULL)
	{
		int ndx = m_list.GetNextSelectedItem(posi) ;
		_layout = (CAircaftLayOut*)m_list.GetItemData(ndx);

		CDlgAircraftConfigurationNameEdit nameedit(_layout,m_terminal,this);
		if(nameedit.DoModal() == IDOK)
		{
			//CDlgAircraftConfigurationSpecView dlgconfigView(_layout,m_terminal,pActiveDoc,this) ;
			//dlgconfigView.DoModal() ;
			EditListItem(_layout,ndx) ;
		}
	} 
}
void CDlgAircraftConfiguration::AfterSelectItem()
{
	POSITION posi = m_list.GetFirstSelectedItemPosition() ;
	CAircaftLayOut* _layout = NULL ;
	if(posi!= NULL)
	{
		int ndx = m_list.GetNextSelectedItem(posi) ;
		_layout = (CAircaftLayOut*)m_list.GetItemData(ndx);
		m_LayOutManager->LinkTheLayOutWithFlightID(_layout,m_flightId) ;

		if(pActiveDoc != NULL)
		{
			CWaitCursor wc;
			pActiveDoc->GetAircraftLayoutEditor()->SetEditLayOut(_layout) ;

			CAircraftLayOutFrame* layoutFrame = (CAircraftLayOutFrame*)pActiveDoc->GetMainFrame()->CreateOrActivateFrame(  theApp.m_pAircraftLayoutTermplate, RUNTIME_CLASS(CAircraftLayout3DView) );
		/*	CLayoutView* view = pActiveDoc->GetLayoutMSVView() ;;*/
			layoutFrame->OnUpdateFrameTitle(TRUE) ;
			layoutFrame->ShowWindow(SW_SHOWMAXIMIZED);
			//layoutFrame->CreateLayoutTool() ;
			//view->Invalidate(FALSE);
// 			pActiveDoc->UpdateAllViews((CView*)this, VM_ONBOARD_LAYOUT_CHANGE, (CObject*)_layout);
		}
	}
}
void CDlgAircraftConfiguration::OnOK()
{
// 	POSITION posi = m_list.GetFirstSelectedItemPosition() ;
// 	CAircaftLayOut* _layout = NULL ;
// 	if(posi== NULL)
// 	{
// 		MessageBox(_T("Please select one item!"));
// 		return;
// 	}
// 	int ndx = m_list.GetNextSelectedItem(posi) ;
// 	_layout = (CAircaftLayOut*)m_list.GetItemData(ndx);
// 	m_LayOutManager->LinkTheLayOutWithFlightID(_layout,m_flightId) ;

	try
	{
		CADODatabase::BeginTransaction() ;
		m_LayOutManager->SaveData() ;
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

   CXTResizeDialog::OnOK() ;
}
void CDlgAircraftConfiguration::OnSelect()
{
	AfterSelectItem() ;
	m_LayOutManager->SaveData() ;
	CXTResizeDialog::OnOK() ;
}
// CDlgAircraftConfiguration message handlers
