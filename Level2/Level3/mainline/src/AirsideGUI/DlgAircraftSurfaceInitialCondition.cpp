// DlgAircraftSurfaceInitialCondition.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgAircraftSurfaceInitialCondition.h"
#include "DlgTimeRange.h"
#include "../InputAirside/SurfacesInitalEvent.h"
// CDlgAircraftSurfaceInitialCondition dialog
#include "../InputAirside/CSurfaceInitialXMLGenerator.h"
IMPLEMENT_DYNAMIC(CDlgAircraftSurfaceInitialCondition, CXTResizeDialog)
CDlgAircraftSurfaceInitialCondition::CDlgAircraftSurfaceInitialCondition(CAirportDatabase* _database ,PSelectFlightType	_pSelectFlightType, CWnd* pParent /*=NULL*/)
	:m_Setting(NULL),m_pSelectFlightType(_pSelectFlightType),m_AirportDataBase(_database), CXTResizeDialog(CDlgAircraftSurfaceInitialCondition::IDD, pParent)
{
	m_Setting = new CAircraftSurfaceSetting(_database) ;
}

CDlgAircraftSurfaceInitialCondition::~CDlgAircraftSurfaceInitialCondition()
{
	delete m_Setting ;
}

void CDlgAircraftSurfaceInitialCondition::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_TOOL,m_staticTool) ;
	DDX_Control(pDX,IDC_TREE_VIEW,m_Tree) ;

}


BEGIN_MESSAGE_MAP(CDlgAircraftSurfaceInitialCondition, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_SIZE() 
    ON_COMMAND(ID_BUTTON_NEWCONDITION,OnNewButton)
	ON_COMMAND(ID_BUTTON_DELSURFACE ,OnDelButton)
	ON_COMMAND(IDC_BUTTON_LOAD,OnLoad)
	ON_COMMAND(IDC_BUTTON_SAVE,OnSave)
	ON_COMMAND(IDC_BUTTON_SAVEAS,OnSaveAs)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_VIEW ,OnTvnSelchangedTree)
END_MESSAGE_MAP()

void CDlgAircraftSurfaceInitialCondition::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
   HTREEITEM item = m_Tree.GetSelectedItem() ;
   if(item == NULL || m_Tree.GetParentItem(item) == NULL)
   {
	   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_NEWCONDITION,TRUE) ;
	   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELSURFACE,TRUE) ;
	   return ;
   }
   if(m_Tree.GetItemData(item) == TY_FLIGHT_TY)
   {
	   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_NEWCONDITION,TRUE) ;
	   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELSURFACE,FALSE) ;
	   return ;
   }
   HTREEITEM par = m_Tree.GetParentItem(item) ;
   if(m_Tree.GetItemData(par) == TY_FLIGHT_TY)
   {
	   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_NEWCONDITION,FALSE) ;
	   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELSURFACE,TRUE) ;
	   return ;
   }
   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_NEWCONDITION,FALSE) ;
   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELSURFACE,FALSE) ;
}
void CDlgAircraftSurfaceInitialCondition::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
int CDlgAircraftSurfaceInitialCondition::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1 ;
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_AIRSURFACE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}
void CDlgAircraftSurfaceInitialCondition::InitToolBar()
{
	CRect rect ;
	m_staticTool.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	m_staticTool.ShowWindow(SW_HIDE) ;

	m_ToolBar.MoveWindow(&rect) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_NEWCONDITION,TRUE) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELSURFACE,FALSE) ;
	m_ToolBar.ShowWindow(SW_SHOW) ;
}
BOOL CDlgAircraftSurfaceInitialCondition::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	m_Setting->ReadData() ;
	InitTreeView() ;
	InitToolBar() ;
	SetResize(m_staticTool.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(m_ToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;

	SetResize(IDC_TREE_VIEW,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK , SZ_BOTTOM_RIGHT ,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL , SZ_BOTTOM_RIGHT ,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT ,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_BUTTON_LOAD,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;

	SetResize(IDC_BUTTON_SAVEAS,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	return TRUE ;
}
void CDlgAircraftSurfaceInitialCondition::InitTreeView()
{
	m_Tree.DeleteAllItems() ;
	CAircraftSurfaceSetting::TY_DATA_ITER iter = m_Setting->GetData()->begin() ;
	for ( ; iter != m_Setting->GetData()->end() ; iter++)
	{
		AddEventItemData(*iter) ;
	}
}
CString CDlgAircraftSurfaceInitialCondition::FormatEventItemName(CString& _name)
{
	return _name ;
}
CString CDlgAircraftSurfaceInitialCondition::FormatTimeItemName(CString& _start ,CString& _end)
{
	CString str ;
	str.Format(_T("Time Range : %s - %s"),_start,_end) ;
	return str ;
}
CString CDlgAircraftSurfaceInitialCondition::FormatFlightTypeName(CString& str)
{
	CString _name ;
	_name.Format(_T("Flight Type : %s"),str) ;
	return _name ;
}
CString CDlgAircraftSurfaceInitialCondition::FormatConditionItemName(CString& str)
{
	return _T("Condition : ") + str ;
}
void CDlgAircraftSurfaceInitialCondition::AddEventItemData(CSurfacesInitalEvent* _event)
{
	if(_event == NULL)
		return ;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	HTREEITEM _item = m_Tree.InsertItem(FormatEventItemName(_event->GetName()),cni,FALSE,FALSE) ;
	m_Tree.SetItemData(_item,(DWORD_PTR)_event) ;

	cni.net = NET_SHOW_DIALOGBOX ;
	CString starttime ;
	CString endtime ;
	starttime = _event->GetStartTime().PrintDateTime() ;
	endtime = _event->GetEndTime().PrintDateTime() ;
    HTREEITEM subItem = m_Tree.InsertItem(FormatTimeItemName(starttime,endtime),cni,FALSE,FALSE,_item) ;
    m_Tree.SetItemData(subItem,TY_TIME) ;

	cni.net = NET_NORMAL ;
	HTREEITEM flt_item = m_Tree.InsertItem(_T("Flight Types"),cni,FALSE ,FALSE,_item) ;
    m_Tree.SetItemData(flt_item,TY_FLIGHT_TY) ;

	CSurfacesInitalEvent::TY_DATA_ITER iter = _event->GetConditions()->begin() ;
	for ( ; iter != _event->GetConditions()->end() ; iter++)
	{
		AddFlightType(flt_item,*iter) ;
	}
	 m_Tree.Expand(flt_item,TVE_EXPAND) ;
}
void CDlgAircraftSurfaceInitialCondition::AddFlightType(HTREEITEM _item , CConditionItem* _condition)
{
	if(_item == NULL || _condition == NULL)
		return ;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;

	CString flightType ;
	_condition->GetFlightType()->screenPrint(flightType) ;
	HTREEITEM item = m_Tree.InsertItem(FormatFlightTypeName(flightType),cni,FALSE,FALSE,_item) ;
    m_Tree.SetItemData(item,(DWORD_PTR)_condition) ;

	cni.net = NET_COMBOBOX ;
	m_Tree.InsertItem(FormatConditionItemName(_condition->GetCondition()),cni,FALSE,FALSE,item) ;
}
// CDlgAircraftSurfaceInitialCondition message handlers
void CDlgAircraftSurfaceInitialCondition::OnNewButton()
{
	HTREEITEM item = m_Tree.GetSelectedItem() ;
	if(item == NULL  || m_Tree.GetParentItem(item) == NULL)
		NewEvent() ;
	else
	    NewFlightType() ;
}
void CDlgAircraftSurfaceInitialCondition::OnDelButton()
{
	HTREEITEM item = m_Tree.GetSelectedItem() ;
	if(item == NULL  || m_Tree.GetParentItem(item) == NULL)
		DelEvent() ;
	else
		DelFlightType() ;
}
void CDlgAircraftSurfaceInitialCondition::NewEvent()
{
	CString name ;
	name.Format(_T("Event%d"),m_Setting->GetData()->size() + 1) ;

	CSurfacesInitalEvent* _event =  m_Setting->AddSurfacesInitalEvent(name) ;

	AddEventItemData(_event) ;
}
void CDlgAircraftSurfaceInitialCondition::DelEvent()
{
   HTREEITEM item = m_Tree.GetSelectedItem() ;
   if(item == NULL)
	   return ;
   CSurfacesInitalEvent* _event = (CSurfacesInitalEvent*)m_Tree.GetItemData(item) ;
   if(_event == NULL)
	   return ;
   m_Setting->DelSurfacesInitalEvent(_event) ;

   m_Tree.DeleteItem(item) ;
}
BOOL CDlgAircraftSurfaceInitialCondition::CheckFlightType(CSurfacesInitalEvent* _event,FlightConstraint& _constraint)
{
	if(_event == NULL)
		return FALSE;
	std::vector<CConditionItem*>::iterator iter = _event->GetConditions()->begin() ;
	for ( ; iter != _event->GetConditions()->end() ; iter++)
	{
		if( (*iter)->GetFlightType()->isEqual(&_constraint))
			return TRUE ;
	}
    return FALSE ;
}
void CDlgAircraftSurfaceInitialCondition::NewFlightType()
{
	HTREEITEM item = m_Tree.GetSelectedItem() ;
    FlightConstraint fltcons = (*m_pSelectFlightType)(NULL);

	CSurfacesInitalEvent* _event = (CSurfacesInitalEvent*)m_Tree.GetItemData(m_Tree.GetParentItem(item)) ;
	if(!CheckFlightType(_event,fltcons))
	{
		 CConditionItem* condition = _event->AddConditionItem(fltcons) ;
		 AddFlightType(item,condition) ;
	}
}
void CDlgAircraftSurfaceInitialCondition::DelFlightType()
{
   HTREEITEM item = m_Tree.GetSelectedItem() ;
   CConditionItem* conditionItem = (CConditionItem*)m_Tree.GetItemData(item) ;
   HTREEITEM paresentItem = m_Tree.GetParentItem(item) ;
   paresentItem  = m_Tree.GetParentItem(paresentItem) ;
   CSurfacesInitalEvent* _event = (CSurfacesInitalEvent*)m_Tree.GetItemData(paresentItem) ;
   _event->DelConditionItem(conditionItem) ;
   m_Tree.DeleteItem(item) ;
}
void CDlgAircraftSurfaceInitialCondition::OnOK()
{
	OnSave() ;
	CXTResizeDialog::OnOK() ;
}
void CDlgAircraftSurfaceInitialCondition::OnCancel()
{
	CXTResizeDialog::OnCancel() ;
}
void CDlgAircraftSurfaceInitialCondition::OnSave()
{
	m_Setting->SaveData() ;
}
void CDlgAircraftSurfaceInitialCondition::OnLoad()
{
	CFileDialog filedlg( TRUE,"XML", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"XML File (*.XML)|*.XML;*.XML|All type (*.*)|*.*|", NULL, 0, FALSE );
	filedlg.m_ofn.lpstrTitle = "Load XML File"  ;
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();
	CSurfaceInitialXMLGenerator XMLGenerator(m_AirportDataBase) ;
	if(XMLGenerator.LoadFile(csFileName,m_Setting))
	{
		InitTreeView() ;
	}
	else
		MessageBox(_T("Load Data Error")) ;
}
void CDlgAircraftSurfaceInitialCondition::OnSaveAs()
{
	CFileDialog filedlg( FALSE,"XML", NULL, OFN_CREATEPROMPT| OFN_SHAREAWARE| OFN_OVERWRITEPROMPT , \
		"XML File (*.XML)|*.XML;*.XML|All type (*.*)|*.*|", NULL, 0, FALSE  );
	filedlg.m_ofn.lpstrTitle = "Save As"  ;

	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

	CSurfaceInitialXMLGenerator XMLGenerator(m_AirportDataBase) ;
	if(XMLGenerator.SaveFile(csFileName,m_Setting))
	{
		MessageBox(_T("Export Data Successfully")) ;
	}else
	{
		MessageBox(_T("Export Data Error")) ;
	}
}
void CDlgAircraftSurfaceInitialCondition::InitComboString(CComboBox* pCB)
{
	if(pCB == NULL)
		return ;
	pCB->AddString(_T("Clear")) ;
	pCB->AddString(_T("Light Frost")) ;
	pCB->AddString(_T("Ice layer")) ;
	pCB->AddString(_T("Dry/powder snow")) ;
	pCB->AddString(_T("Wet Snow")) ;
}
void CDlgAircraftSurfaceInitialCondition::HandleComboxChange(CComboBox* pCB,HTREEITEM _item)
{
	if(pCB == NULL || _item == NULL)
		return ;
	int ndx = pCB->GetCurSel() ;
	CString str ;
	pCB->GetLBText(ndx,str) ;
	HTREEITEM ParItem = m_Tree.GetParentItem(_item) ;
	if(ParItem == NULL)
		return ;
	CConditionItem* conditionItem = (CConditionItem*)m_Tree.GetItemData(ParItem) ;
	if(conditionItem == NULL)
		return ;
	conditionItem->SetCondition(str) ;
	m_Tree.SetItemText(_item,FormatConditionItemName(str)) ;
}
void CDlgAircraftSurfaceInitialCondition::ShowDialogHandle(HTREEITEM _item)
{
   if(_item == NULL)
	   return ;
   int itemdata = m_Tree.GetItemData(_item) ;
   HTREEITEM ParItem = NULL ;
   if(itemdata == TY_TIME)
   {
      ParItem = m_Tree.GetParentItem(_item) ;
       CSurfacesInitalEvent* _event = (CSurfacesInitalEvent*)m_Tree.GetItemData(ParItem) ;
	   if(_event == NULL)
		   return ;
	   CDlgTimeRange dlg(_event->GetStartTime(),_event->GetEndTime()) ;
	   if(dlg.DoModal() == IDOK)
	   {
		   _event->SetStartTime(dlg.GetStartTime()) ;
		   _event->SetEndTime(dlg.GetEndTime()) ;
		   CString starttime ;
		   CString endtime ;
		   starttime.Format(_T("Day%d %d:%d:%d"),_event->GetStartTime().GetDay(),_event->GetStartTime().GetHour(),_event->GetStartTime().GetMinute(),_event->GetStartTime().GetSecond()) ;
		   endtime.Format(_T("Day%d %d:%d:%d"),_event->GetEndTime().GetDay(),_event->GetEndTime().GetHour(),_event->GetEndTime().GetMinute(),_event->GetEndTime().GetSecond()) ;
		   m_Tree.SetItemText(_item,FormatTimeItemName(starttime,endtime)) ;
	   }
   }else
   {
	   ParItem = m_Tree.GetParentItem(_item) ;
	   ParItem = m_Tree.GetParentItem(ParItem) ;
           FlightConstraint fltcons = (*m_pSelectFlightType)(NULL);
		     CSurfacesInitalEvent* _event = (CSurfacesInitalEvent*)m_Tree.GetItemData(ParItem) ;
		   if(!CheckFlightType(_event,fltcons))
		   {
             CConditionItem* condition = (CConditionItem*)m_Tree.GetItemData(_item) ;
			 condition->SetFlightTy(fltcons) ;
			 CString str ;
			 fltcons.screenPrint(str) ;
			 m_Tree.SetItemText(_item,FormatFlightTypeName(str)) ;
		   }
   }
}
LRESULT CDlgAircraftSurfaceInitialCondition::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CWnd* pWnd = m_Tree.GetEditWnd((HTREEITEM)wParam);
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;

			m_Tree.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			if (pCB->GetCount() != 0)
			{
				pCB->ResetContent();
			}
			if (pCB->GetCount() == 0)
			{
				InitComboString(pCB) ;
				pCB->SetCurSel(0);
			}
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
	case UM_CEW_COMBOBOX_END:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
			CWnd* pWnd=m_Tree.GetEditWnd(_item);
			CComboBox* pTCB=(CComboBox*)pWnd;
			HandleComboxChange(pTCB,_item) ;
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
		    ShowDialogHandle(_item) ;
		}
		break;
	case UM_CEW_LABLE_END:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			CSurfacesInitalEvent* PrecipitationTypeEvent = (CSurfacesInitalEvent*)m_Tree.GetItemData(_item) ;
			PrecipitationTypeEvent->SetName(strValue) ;
			m_Tree.SetItemText(_item,strValue) ;
		}
		break;
	default:
	    break;
	}
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}