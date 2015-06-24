// DlgSectBlockLogTime.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSectBlockLogTime.h"
#include "../Inputs/IN_TERM.H"
#include "../InputOnBoard/AircaftLayOut.h"
#include "../InputOnBoard/AircraftPlacements.h"
#include "DlgBlockSelect.h"
#include "DlgNewPassengerType.h"
// CDlgSectBlockLogTime dialog
#include "DlgSeatSelection.h"
#include "../InputOnBoard/SeatBlockLagTime.h"
IMPLEMENT_DYNAMIC(CDlgSectBlockLogTime, CXTResizeDialog)
CDlgSectBlockLogTime::CDlgSectBlockLogTime( int nProjID,InputTerminal * pInterm,CAirportDatabase* pAirportDB,COnBoardAnalysisCandidates* _pOnBoardList,CWnd* pParent )
	:m_boardingCall(pAirportDB,pInterm->inStrDict)
	,m_input(pInterm)
	,m_projectid(nProjID)
	,m_onboardList(_pOnBoardList)
	,CXTResizeDialog(CDlgSectBlockLogTime::IDD, pParent)
{
	fligtimeData = new CFlightLagTimeData() ;
	m_boardingCall.ReadData(nProjID) ;
}

CDlgSectBlockLogTime::~CDlgSectBlockLogTime()
{
	if(fligtimeData != NULL)
        delete fligtimeData ;
}

void CDlgSectBlockLogTime::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_LAGTIME,m_LogTimeTree) ;
	DDX_Control(pDX,IDC_STATIC_TOOLBAR,m_ToolBarEdit) ;
}


BEGIN_MESSAGE_MAP(CDlgSectBlockLogTime, CXTResizeDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LAGTIME, OnSelchangedTree)
	ON_COMMAND(ID_LOGTIME_ADD,OnToolBarAdd)
	ON_COMMAND(ID_LOGTIME_DEL,OnToolBarDel)
END_MESSAGE_MAP()
LRESULT CDlgSectBlockLogTime::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CWnd* pWnd = m_LogTimeTree.GetEditWnd((HTREEITEM)wParam);
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;

			m_LogTimeTree.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			if (pCB->GetCount() != 0)
			{
				pCB->ResetContent();
			}
			if (pCB->GetCount() == 0)
			{
				InitComboString(pCB,hItem) ;
				pCB->SetCurSel(0);
			}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
			if(_item != NULL)
			{
			CLagTimeForPax* lagPax = (CLagTimeForPax*)m_LogTimeTree.GetItemData(_item) ;
			CPassengerType PaxTY(m_input->inStrDict);
			CDlgNewPassengerType DlgPaxType(&PaxTY,m_input,this) ;
			if(DlgPaxType.DoModal() == IDOK )
			{
				lagPax->GetPaxType()->FormatLevelPaxTypeFromString(PaxTY.GetStringForDatabase()) ;
				CString val ;
				val.Format(_T("Passenger Type:  %s"),lagPax->GetPaxType()->PrintStringForShow()) ;
				m_LogTimeTree.SetItemText(_item,val) ;
			}
			}
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
	case UM_CEW_COMBOBOX_END:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
			CWnd* pWnd=m_LogTimeTree.GetEditWnd(_item);
			CComboBox* pTCB=(CComboBox*)pWnd;
			HandleComboxChange(pTCB,_item) ;
		}
		break;
	case UM_CEW_LABLE_END:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			HandleEditLable(strValue,_item) ;
		}
		break;
	default:
		break;
	}
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}
void CDlgSectBlockLogTime::HandleEditLable(const CString& _val ,HTREEITEM item)
{
	int type = m_LogTimeTree.GetItemData(item) ;
	HTREEITEM ParItem = m_LogTimeTree.GetParentItem(item);
	CString val ;
	if(type == TY_NODE_FROM)
	{
        val = FormatFromSeatString(_val) ;
	}
	if(type == TY_NODE_TO )
		val = FormatToSeatString(_val) ;
	if(type == TY_NODE_BASETIME)
	{
		CLagTimeForPax* paxdata = (CLagTimeForPax*)m_LogTimeTree.GetItemData(ParItem) ;
		int Ival = atoi(_val) ;
		if(paxdata != NULL)
		  paxdata->SetBaseTime(Ival) ;
		val.Format(_T("%d"),Ival) ;
		val = FormatBaseTime(val) ;
	}
	if(type == TY_NODE_FACTOR)
	{
		COnBoardSeatBlockItem* blockItem = (COnBoardSeatBlockItem*)m_LogTimeTree.GetItemData(ParItem) ;
		CSeatBlockLagTime* lagtime = fligtimeData->GetLagTimeByBlockID(blockItem->getID()) ;
		if(lagtime == NULL)
			return ;
		double IVal = atof(_val) ;
		if(blockItem != NULL)
		{
		   lagtime->SetFactor(IVal) ;
		}
		val.Format(_T("%0.2f"),IVal) ;
		val = FormatLagFactor(val) ;
	}
	m_LogTimeTree.SetItemText(item,val) ;
}
void CDlgSectBlockLogTime::HandleComboxChange(CComboBox* _box ,HTREEITEM _item)
{
	if(_box == NULL || _item == NULL)
		return ;
	int nodety = m_LogTimeTree.GetItemData(_item) ;
	HTREEITEM ParItem = m_LogTimeTree.GetParentItem(_item);
  COnBoardSeatBlockItem* blockItem = (COnBoardSeatBlockItem*)m_LogTimeTree.GetItemData(ParItem) ;
  int index = _box->GetCurSel() ;
  CString str ;
  CSeat* pseat = NULL ;
  CSeatBlockLagTime* lagtime = fligtimeData->GetLagTimeByBlockID(blockItem->getID()) ;
  if(lagtime == NULL)
	  return ;
  if(nodety == TY_NODE_POSITION)
  {
	  _box->GetLBText(index,str) ;
	 lagtime->SetPosition(str) ;
	  m_LogTimeTree.SetItemText(_item,FormatPositionString(str)) ;
  }
  if(nodety == TY_NODE_FROM || nodety == TY_NODE_TO)
  {
	   _box->GetLBText(index,str) ;
	   if(nodety == TY_NODE_FROM)
	   {
          m_LogTimeTree.SetItemText(_item,FormatFromSeatString(str)) ;
		 pseat = (CSeat*)_box->GetItemData(index) ;
         lagtime->SetFromSeat(pseat->GetID()) ;
	   }
	   if(nodety == TY_NODE_TO)
	   {
		   m_LogTimeTree.SetItemText(_item,FormatToSeatString(str)) ;
		   pseat = (CSeat*)_box->GetItemData(index) ;
		   lagtime->SetToSeat(pseat->GetID()) ;
	   }
	  
  }
  
}
void CDlgSectBlockLogTime::InitComboString(CComboBox* _box,HTREEITEM Item)
{
		if(_box == NULL || Item == NULL)
			return ;
		int nodeTy = m_LogTimeTree.GetItemData(Item) ;
		if(nodeTy == TY_NODE_POSITION)
		{
			_box->AddString("inner") ;
			_box->AddString("aisle") ;
		}
		if(nodeTy == TY_NODE_FROM || nodeTy ==TY_NODE_TO)
		{
          HTREEITEM Paritem =    m_LogTimeTree.GetParentItem(Item) ;
          COnBoardSeatBlockItem* block = (COnBoardSeatBlockItem*)m_LogTimeTree.GetItemData(Paritem) ;
		  COnBoardingCallFlight* flight = (COnBoardingCallFlight*)m_LogTimeTree.GetItemData(m_LogTimeTree.GetParentItem(m_LogTimeTree.GetParentItem(Paritem))) ;
		  CAircaftLayOut* layout =   CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(flight->GetFlightAnalysis().getID()) ;
		  int ndx ;
		  CSeat* pSeat = NULL ;
		  for (int i = 0 ;i < block->getCount() ;i++)
		  {
			  if(nodeTy == TY_NODE_FROM)
				  pSeat =block->getItem(i)->GetStartSeat(layout) ;
			  if(nodeTy == TY_NODE_TO)
				  pSeat = block->getItem(i)->GetEndSeat(layout) ;
			  if(pSeat == NULL)
				  continue ;
               ndx = _box->AddString(pSeat->GetIDName().GetIDString()) ;
			  _box->SetItemData(ndx,(DWORD_PTR)pSeat) ;
		  }
		}
}
BOOL CDlgSectBlockLogTime::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	fligtimeData->ReadData(m_input->inStrDict) ;
	OnInitToolBar();
	OnInitTree() ;

	SetResize(m_ToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_STATIC_TREE,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_TREE_LAGTIME,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(ID_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetWindowText(_T("Lag Time For Deplanement")) ;
    return TRUE ;
}
void CDlgSectBlockLogTime::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy);
}
int CDlgSectBlockLogTime::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1 ; 
	if(!m_ToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP) ||
		!m_ToolBar.LoadToolBar(IDR_ADDDEL_LOGTIME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0 ;
}
void CDlgSectBlockLogTime::OnInitToolBar()
{
   CRect rect ;
   m_ToolBarEdit.GetWindowRect(&rect) ;
   ScreenToClient(&rect) ;
   m_ToolBar.MoveWindow(rect) ;
   m_ToolBarEdit.ShowWindow(FALSE) ;
   m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_ADD,TRUE) ;
   m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_DEL,FALSE) ;
   m_ToolBar.ShowWindow(TRUE) ;
}
void CDlgSectBlockLogTime::OnInitTree()
{
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	HTREEITEM item ;
	COnBoardSeatBlockItem* blockItem = NULL ;
	CFlightLagTimeData::TY_FLIGHT_LAG_CON_ITER  iter = fligtimeData->GetDataSet()->begin() ;
	for (  ; iter != fligtimeData->GetDataSet()->end();iter++)
	{
		for (int i = 0; i < m_boardingCall.getCount(); i++)
		{
			CString strValue ;
			COnBoardingCallStage* pStage = m_boardingCall.getItem(i);
			for (int j = 0; j < pStage->getCount(); j++)
			{
				COnBoardingCallFltType* pFltType = pStage->getItem(j);
				for (int n = 0; n < pFltType->getCount(); n++)
				{
				  COnBoardingCallFlight* pFlight = pFltType->getItem(n);
				  if(pFlight->GetFlightAnalysis().getID() == (*iter)->m_FlightID)
				  {
                     item =  AddFlightNode(pFlight,&((*iter)->m_LagTimes)) ;
				  }
				}
			}
		}
	}
  
}
HTREEITEM CDlgSectBlockLogTime::AddFlightNode(COnBoardingCallFlight* _type)
{
	if(_type == NULL)
		return NULL;
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.net = NET_NORMAL;
	info.bAutoSetItemText = FALSE ;
	CString val ;
	val = FormatFlightNodeString(_type) ;
	HTREEITEM SeatPositionItem = m_LogTimeTree.InsertItem(val,info,TRUE) ;
	m_LogTimeTree.SetItemData(SeatPositionItem,(DWORD_PTR)_type);
    HTREEITEM blockItems = m_LogTimeTree.InsertItem(_T("Seat Blocks"),info,TRUE,FALSE,SeatPositionItem) ;
	m_LogTimeTree.SetItemData(blockItems,(DWORD_PTR)TY_NODE_BLOCKS) ;
	return  blockItems ;
}
HTREEITEM CDlgSectBlockLogTime::AddFlightNode(COnBoardingCallFlight* _type,CMappingFlightID_LagTime::TY_LAGTIME_CONTAINER* _BlockData)
{
	
	HTREEITEM SeatPositionItem = AddFlightNode(_type) ;
	COnBoardSeatBlockItem* blockItem = NULL ;
	HTREEITEM subItem ;
	for (int m = 0 ; m < _type->getSeatBlockList().getCount() ; m++)
	{
		blockItem = _type->getSeatBlockList().getItem(m) ;
		CMappingFlightID_LagTime::TY_LAGTIME_CONTAINER_ITER iter = _BlockData->begin() ;
		for ( ; iter != _BlockData->end() ;iter++)
		{
			if((*iter)->GetSeatBlockID() == blockItem->getID())
			{
				subItem = AddBlockItem(blockItem,SeatPositionItem) ;
				m_LogTimeTree.SetItemData(subItem,(DWORD_PTR)blockItem) ;
				AddLagtimeItem((*iter),subItem,_type);
			}
		}
		
	}
	 m_LogTimeTree.Expand(SeatPositionItem,TVE_EXPAND) ;
	return SeatPositionItem ;
}
HTREEITEM CDlgSectBlockLogTime::AddBlockItem(COnBoardSeatBlockItem* _blcok ,HTREEITEM _item)
{
	if(_blcok == NULL || _item == NULL)
		return NULL;
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.net = NET_NORMAL;
	info.bAutoSetItemText = FALSE ;
  HTREEITEM subItem = m_LogTimeTree.InsertItem(_blcok->getBlockName(),info,TRUE,FALSE ,_item) ;
  m_LogTimeTree.SetItemData(subItem,(DWORD_PTR)_blcok) ;
  return subItem ;
}
CString CDlgSectBlockLogTime::FormatFlightNodeString(COnBoardingCallFlight* _type)
{
	CString strValue ;
	COnboardFlight& pFlightAnalysis = _type->GetFlightAnalysis();
	if (pFlightAnalysis.getFlightType() == ArrFlight)
	{
		strValue.Format(_T("Flight %s - ACTYPE %s Arr"),pFlightAnalysis.getFlightID(),pFlightAnalysis.getACType());
	}
	else
	{
		strValue.Format(_T("Flight %s - ACTYPE %s Dep"),pFlightAnalysis.getFlightID(),pFlightAnalysis.getACType());
	}
	return strValue ;
}
CString CDlgSectBlockLogTime::FormatPositionString(const CString& _val)
{
		CString val ;
        val.Format(_T("Seat Position :   %s"),_val) ;
		return val ;
}
CString CDlgSectBlockLogTime::FormatFromSeatString(const CString& _val)
{

	CString val ;
	val.Format(_T("FROM :   %s"),_val) ;
	return val ;
}
CString CDlgSectBlockLogTime::FormatToSeatString(const CString& _val)
{
	CString val ;
	val.Format(_T("TO :    %s"),_val) ;
	return val ;
}
CString CDlgSectBlockLogTime::FormatLagFactor(const CString& _val)
{
	CString val ;
	val.Format(_T("Lag Factor :    %s"),_val) ;
	return val ;
}
CString CDlgSectBlockLogTime::FormatBaseTime(const CString& _val)
{
	CString val ;
	val.Format(_T("Base Time :    %s"),_val) ;
	return val ;
}
void CDlgSectBlockLogTime::AddLagtimeItem(CSeatBlockLagTime* logTime , HTREEITEM item,COnBoardingCallFlight* _flightTy)
{
  if(_flightTy == NULL)
	  return ;
  	CAircaftLayOut* layout =   CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(_flightTy->GetFlightAnalysis().getID()) ;
	if(layout == NULL)
		return ;
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.net = NET_COMBOBOX;
	info.bAutoSetItemText = FALSE ;
	CString val ;
	val = FormatPositionString(logTime->GetPosition()) ;
	HTREEITEM SeatPositionItem = m_LogTimeTree.InsertItem(val,info,TRUE,FALSE ,item) ;
	m_LogTimeTree.SetItemData(SeatPositionItem,TY_NODE_POSITION);
    
	CSeat* pseat = layout->GetPlacements()->GetSeatData()->GetSeatByID(logTime->GetFromSeat()) ;
	val = FormatFromSeatString(pseat==NULL?"":pseat->GetIDName().GetIDString()) ;
   HTREEITEM subItem =  m_LogTimeTree.InsertItem(val,info,TRUE,FALSE ,item) ;
   m_LogTimeTree.SetItemData(subItem,TY_NODE_FROM) ;

   pseat = layout->GetPlacements()->GetSeatData()->GetSeatByID(logTime->GetToSeat()) ;
   val = FormatToSeatString(pseat==NULL?"":pseat->GetIDName().GetIDString()) ;
   subItem =  m_LogTimeTree.InsertItem(val,info,TRUE,FALSE ,item) ;
   m_LogTimeTree.SetItemData(subItem,TY_NODE_TO) ;
  
   val.Format(_T("%0.2f"),logTime->GetFactor()) ;
   val = FormatLagFactor(val) ;
   info.net = NET_LABLE ;
   subItem =  m_LogTimeTree.InsertItem(val,info,TRUE,FALSE ,item) ;
   m_LogTimeTree.SetItemData(subItem,TY_NODE_FACTOR) ;

   info.net = NET_NORMAL ;
   subItem =  m_LogTimeTree.InsertItem("Passenger Types",info,TRUE,FALSE ,item) ;
   m_LogTimeTree.SetItemData(subItem,TY_NODE_PAXTYS) ;
   CSeatBlockLagTime::TY_LOGTIME_PAX_ITER iter = logTime->GetPaxVector()->begin() ;
   for ( ; iter != logTime->GetPaxVector()->end() ;iter++)
   {
        AddPaxDataItem(*iter,subItem) ;
   }
   m_LogTimeTree.Expand(subItem,TVE_EXPAND) ;
}
void CDlgSectBlockLogTime::AddPaxDataItem(CLagTimeForPax* _paxData, HTREEITEM item)
{
   if(_paxData == NULL || item == NULL)
	   return ;
   COOLTREE_NODE_INFO info ;
   CCoolTree::InitNodeInfo(this,info);
   info.net = NET_SHOW_DIALOGBOX;
   CString val ;
   val.Format(_T("Passenger Type:  %s"),_paxData->GetPaxType()->PrintStringForShow()) ;
   HTREEITEM subItem =  m_LogTimeTree.InsertItem(val,info,TRUE,FALSE ,item) ;
   m_LogTimeTree.SetItemData(subItem,(DWORD_PTR)_paxData) ;

   info.net = NET_LABLE ;
   val.Format(_T("%d"),_paxData->GetBaseTime()) ;
   val = FormatBaseTime(val) ;
   HTREEITEM _item  =  m_LogTimeTree.InsertItem(val,info,TRUE,FALSE ,subItem) ;
   m_LogTimeTree.SetItemData(_item,TY_NODE_BASETIME) ;
}
void CDlgSectBlockLogTime::OnOK()
{
 OnSave();
 CXTResizeDialog::OnOK() ;
}
void CDlgSectBlockLogTime::OnCancel()
{

	CXTResizeDialog::OnCancel() ;
}
void CDlgSectBlockLogTime::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		fligtimeData->SaveData() ;
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}
void CDlgSectBlockLogTime::OnToolBarAdd()
{
      HTREEITEM hselItem = m_LogTimeTree.GetSelectedItem() ;
	  if(hselItem == NULL)
	  {
          OnAddFlight() ;
		  return ;
	  }
	  if(m_LogTimeTree.GetItemData(hselItem) == TY_NODE_PAXTYS)
	  {
		  OnAddLagTimePaxData() ;
		  return ;
	  }
	  if(m_LogTimeTree.GetItemData(hselItem) == TY_NODE_BLOCKS)
	  {
		  OnAddBlock() ;
		  return ;
	  }
	  if(m_LogTimeTree.GetParentItem(hselItem) == NULL)
	  {
		   OnAddFlight() ;
	  }
}
void CDlgSectBlockLogTime::OnToolBarDel()
{
	HTREEITEM hselItem = m_LogTimeTree.GetSelectedItem() ;
	if(hselItem == NULL)
		return ;
	if(m_LogTimeTree.GetParentItem(hselItem) == NULL)
	{
		OnDelFlight() ;
		return ;
	}
	if(m_LogTimeTree.GetItemData(m_LogTimeTree.GetParentItem(hselItem)) == TY_NODE_PAXTYS)
	{
		OnDelLagTimePaxData() ;
		return ;
	}
	 if(m_LogTimeTree.GetItemData(m_LogTimeTree.GetParentItem(hselItem)) == TY_NODE_BLOCKS)
		 OnDelBlock() ;
}
void CDlgSectBlockLogTime::OnAddFlight()
{
     CDlgFlightSelect dlg(m_projectid,m_input,&m_boardingCall,m_Airport,m_onboardList,this) ;
	 dlg.DoModal() ;
	 COnBoardingCallFlight* flight =  dlg.GetSelFlight() ;
	 if(flight != NULL)
	 {
		 AddFlightNode(flight) ;
	 }
}

void CDlgSectBlockLogTime::OnDelFlight()
{
	HTREEITEM selItem = m_LogTimeTree.GetSelectedItem() ;
	COnBoardingCallFlight* flight = (COnBoardingCallFlight*)m_LogTimeTree.GetItemData(selItem) ;
	if(selItem == NULL || flight == NULL)
		return ;
	fligtimeData->DelFlightID(flight->GetFlightAnalysis().getID()) ;
	m_LogTimeTree.DeleteItem(selItem) ;
}
BOOL CDlgSectBlockLogTime::CheckBlockIfHaveDefined(HTREEITEM _item,COnBoardSeatBlockItem* blockItem)
{
  HTREEITEM subItem = m_LogTimeTree.GetChildItem(_item) ;
  COnBoardSeatBlockItem* subblock = NULL ;
  while(subItem)
  {
      subblock = (COnBoardSeatBlockItem*)m_LogTimeTree.GetItemData(subItem) ;
	  if(subblock == blockItem || subblock->getID() == blockItem->getID())
		  return TRUE ;
	  subItem = m_LogTimeTree.GetNextSiblingItem(subItem) ;
  }
  return FALSE ;
}
void CDlgSectBlockLogTime::OnAddBlock()
{
	HTREEITEM SelItem = m_LogTimeTree.GetSelectedItem() ;
	HTREEITEM ParITem = m_LogTimeTree.GetParentItem(SelItem) ;
	COnBoardingCallFlight* PFLight = (COnBoardingCallFlight*)m_LogTimeTree.GetItemData(ParITem) ;
	CDlgBlockSelect dlg(PFLight,this) ;
	COnBoardSeatBlockItem* blockItem = NULL ;
	dlg.DoModal() ;
	blockItem  = dlg.GetSelBlock() ;
	HTREEITEM blockNode = NULL ;
	if(blockItem != NULL)
	{
         if(CheckBlockIfHaveDefined(SelItem,blockItem))
		 {
			 MessageBox(_T("This Block has Defined Lag Time"),_T("Warning"),MB_OK) ;
			 return ;
		 }
		 CSeatBlockLagTime* lagtime = new CSeatBlockLagTime ;
		 lagtime->SetFlightID(PFLight->GetFlightAnalysis().getID()) ;
		 lagtime->SetSeatBlockID(blockItem->getID()) ;
		 blockNode = AddBlockItem(blockItem,SelItem) ;
		 AddLagtimeItem(lagtime,blockNode,PFLight) ;
		 fligtimeData->AddLagTime(lagtime) ;
	}
}
void CDlgSectBlockLogTime::OnDelBlock()
{
   HTREEITEM SelItem = m_LogTimeTree.GetSelectedItem() ;
   COnBoardSeatBlockItem* block = (COnBoardSeatBlockItem*)m_LogTimeTree.GetItemData(SelItem) ;
   HTREEITEM parItem = m_LogTimeTree.GetParentItem(m_LogTimeTree.GetParentItem(SelItem)) ;
   COnBoardingCallFlight* flight = (COnBoardingCallFlight*)m_LogTimeTree.GetItemData(parItem) ;
   fligtimeData->DelLagTimeByID(flight->GetFlightAnalysis().getID() , block->getID()) ;
   m_LogTimeTree.DeleteItem(SelItem) ;
}
void CDlgSectBlockLogTime::OnAddLagTimePaxData()
{
    HTREEITEM selItem = m_LogTimeTree.GetSelectedItem() ;
	HTREEITEM ParItem = m_LogTimeTree.GetParentItem(selItem) ;
	COnBoardSeatBlockItem* blockItem = (COnBoardSeatBlockItem*)m_LogTimeTree.GetItemData(ParItem) ;
	if(blockItem == NULL)
		return ;
	CSeatBlockLagTime* lagtime = fligtimeData->GetLagTimeByBlockID(blockItem->getID()) ;
	if(lagtime == NULL)
		return ;
	CLagTimeForPax* paxData = new CLagTimeForPax(m_input->inStrDict) ;
	CDlgNewPassengerType  dlg(paxData->GetPaxType(),m_input,this) ;
	if(dlg.DoModal() == IDOK)
	{
		lagtime->AddPaxData(paxData) ;
		AddPaxDataItem(paxData,selItem) ;
	}else
	{
		delete paxData ;
	}
}
void CDlgSectBlockLogTime::OnDelLagTimePaxData()
{
     HTREEITEM selItem = m_LogTimeTree.GetSelectedItem() ;
     CLagTimeForPax* paxData = (CLagTimeForPax*)m_LogTimeTree.GetItemData(selItem) ;
     HTREEITEM ParItem = m_LogTimeTree.GetParentItem(selItem) ;
	 ParItem = m_LogTimeTree.GetParentItem(ParItem) ;
	 COnBoardSeatBlockItem* blockItem = (COnBoardSeatBlockItem*)m_LogTimeTree.GetItemData(ParItem) ;
	 if(blockItem == NULL)
		 return ;
	 CSeatBlockLagTime* lagtime = fligtimeData->GetLagTimeByBlockID(blockItem->getID()) ;
	 if(lagtime == NULL)
		 return ;
	lagtime->DelPaxData(paxData) ;
	m_LogTimeTree.DeleteItem(selItem) ;
}
// CDlgSectBlockLogTime message handlers
void CDlgSectBlockLogTime::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hSelItem = m_LogTimeTree.GetSelectedItem();
	if(hSelItem == NULL )
		return ;
	if(m_LogTimeTree.GetItemData(hSelItem) == TY_NODE_PAXTYS)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_ADD,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_DEL,FALSE) ;
		return ;
	}
	if(m_LogTimeTree.GetItemData(hSelItem) == TY_NODE_BLOCKS)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_ADD,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_DEL,FALSE) ;
		return ;
	}
	if(m_LogTimeTree.GetParentItem(hSelItem) == NULL)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_ADD,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_DEL,TRUE) ;
		return ;
	}else
	{
		HTREEITEM parItem = m_LogTimeTree.GetParentItem(hSelItem) ;
		if(m_LogTimeTree.GetItemData(parItem) == TY_NODE_PAXTYS)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_ADD,FALSE) ;
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_DEL,TRUE) ;
			return ;
		}
		if(m_LogTimeTree.GetItemData(parItem) == TY_NODE_BLOCKS)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_ADD,FALSE) ;
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_DEL,TRUE) ;
			return ;
		}else
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_ADD,FALSE) ;
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_LOGTIME_DEL,FALSE) ;
			return ;
		}
	}
	
}