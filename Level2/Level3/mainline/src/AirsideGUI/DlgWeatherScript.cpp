// DlgWeatherScript.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgWeatherScript.h"
#include "DlgTimeRange.h"
#include "../InputAirside/WeatherScripts.h"
// CDlgWeatherScript dialog
#include "../InputAirside/WeatherSciptXMLGenerator.h"
IMPLEMENT_DYNAMIC(CDlgWeatherScript, CXTResizeDialog)
CDlgWeatherScript::CDlgWeatherScript(CWnd* pParent /*=NULL*/)
	: m_WeatherScripts(NULL),CXTResizeDialog(CDlgWeatherScript::IDD, pParent)
{
	m_WeatherScripts = new CWeatherScripts ;
}

CDlgWeatherScript::~CDlgWeatherScript()
{
	delete m_WeatherScripts ;
}

void CDlgWeatherScript::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_VIEW, m_TreeView);
	DDX_Control(pDX, IDC_STATIC_TOOL, m_StaticTool);
	DDX_Control(pDX, IDC_BUTTON_LOAD, m_ButtonLoad);
	DDX_Control(pDX, IDC_BUTTON_SAVEAS, m_ButtonSaveAs);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_ButtonSave);
}


BEGIN_MESSAGE_MAP(CDlgWeatherScript, CXTResizeDialog)
	ON_WM_SIZE()
	ON_WM_CREATE() 
	ON_COMMAND(ID_BUTTON_NEW_EVENT,OnNewButton) 
	ON_COMMAND(ID_BUTTON_DEL,OnDelButton)
	ON_COMMAND(IDC_BUTTON_LOAD,OnLoad)
	ON_COMMAND(IDC_BUTTON_SAVEAS,OnSaveAs)
	ON_COMMAND(IDC_BUTTON_SAVE,OnSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_VIEW,OnTvnSelchangedTree)
END_MESSAGE_MAP()

void CDlgWeatherScript::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    HTREEITEM item = m_TreeView.GetSelectedItem() ;
	if(item == NULL || m_TreeView.GetParentItem(item) == NULL)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_NEW_EVENT,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,TRUE) ;
	}else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_NEW_EVENT,FALSE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,FALSE) ;
	}
}
void CDlgWeatherScript::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
int CDlgWeatherScript::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1 ;
	if (!m_ToolBar.CreateEx(this,  TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_WEATHERSCRIPT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}
void CDlgWeatherScript::InitToolBar()
{
	CRect rect ;
    m_StaticTool.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	m_ToolBar.MoveWindow(&rect) ;
    m_StaticTool.ShowWindow(SW_HIDE) ;
   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_NEW_EVENT,TRUE) ;
   m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,FALSE) ;

}
BOOL CDlgWeatherScript::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	m_WeatherScripts->ReadData() ;
	InitTreeView() ;
	InitToolBar() ;

	SetResize(m_StaticTool.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(m_ToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(m_TreeView.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_BUTTON_LOAD,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	SetResize(IDC_BUTTON_SAVEAS,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	return TRUE ;
}
void CDlgWeatherScript::InitTreeView()
{
	m_TreeView.DeleteAllItems() ;
	CWeatherScripts::TY_DATA_ITER iter = m_WeatherScripts->GetData()->begin() ;
    for ( ; iter != m_WeatherScripts->GetData()->end() ;iter++)
    {
		AddPrecipitationItem(*iter) ;
    }
}
CString CDlgWeatherScript::FormatPrecipitationItemName(CString& _name)
{
	return _T("Precipitation Type : ") + _name ;
}
CString CDlgWeatherScript::FormatStartTime(CString& _time)
{
	return _T("Start date and time : ") + _time ;
}
CString CDlgWeatherScript::FormatEndTime(CString& _time)
{
	return _T("End date and time : ") + _time ;
}
void CDlgWeatherScript::AddPrecipitationItem(CPrecipitationTypeEventItem* _data)
{
	if(_data == NULL)
		return ;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	HTREEITEM item = m_TreeView.InsertItem(_data->GetName(),cni,FALSE,FALSE) ;
    m_TreeView.SetItemData(item,(DWORD_PTR)_data) ;

	cni.net = NET_COMBOBOX ;
	HTREEITEM subItem = m_TreeView.InsertItem(FormatPrecipitationItemName(_data->GetType()),cni,FALSE,FALSE,item) ;
	

   cni.net = NET_SHOW_DIALOGBOX ;
   CString starttime ;
   CString endtime ;
   starttime.Format(_T("Day%d %d:%d:%d"),_data->GetStartTime().GetDay(),_data->GetStartTime().GetHour(),_data->GetStartTime().GetMinute(),_data->GetStartTime().GetSecond()) ;
   endtime.Format(_T("Day%d %d:%d:%d"),_data->GetEndTime().GetDay(),_data->GetEndTime().GetHour(),_data->GetEndTime().GetMinute(),_data->GetEndTime().GetSecond()) ;
   m_TreeView.InsertItem(FormatStartTime(starttime),cni,FALSE,FALSE,subItem) ;

   m_TreeView.InsertItem(FormatEndTime(endtime),cni,FALSE,FALSE,subItem) ;

}
void CDlgWeatherScript::OnNewButton()
{

   CString val ;
   val.Format(_T("Event%d"),m_WeatherScripts->GetData()->size() + 1) ;
   CPrecipitationTypeEventItem* PrecipitationTypeEvent = m_WeatherScripts->AddPrecipitationTypeEventItem(val) ;
   AddPrecipitationItem(PrecipitationTypeEvent) ;
}
void CDlgWeatherScript::OnDelButton()
{
   HTREEITEM item = m_TreeView.GetSelectedItem() ;
   if(item == NULL)
	   return ;
    CPrecipitationTypeEventItem* PrecipitationTypeEvent = (CPrecipitationTypeEventItem*)m_TreeView.GetItemData(item) ;
	if(PrecipitationTypeEvent == NULL)
		return ;
	m_WeatherScripts->DelPrecipitationTypeEventItem(PrecipitationTypeEvent) ;
	m_TreeView.DeleteItem(item) ;
}
void CDlgWeatherScript::InitComboString(CComboBox* pCB)
{
   if(pCB == NULL)
	   return ;
   pCB->AddString(_T("Non-freezing")) ;
   pCB->AddString(_T("Freezing drizzle")) ;
   pCB->AddString(_T("Light Snow")) ;
   pCB->AddString(_T("Medium Snow")) ;
   pCB->AddString(_T("Heavy snow")) ;
}
void CDlgWeatherScript::HandleComboxChange(CComboBox* pCB,HTREEITEM _item)
{
   if(pCB == NULL || _item == NULL)
	   return ;
   int ndx = pCB->GetCurSel() ;
   CString str ;
   pCB->GetLBText(ndx,str) ;
   HTREEITEM par = m_TreeView.GetParentItem(_item) ;
   CPrecipitationTypeEventItem* PrecipitationTypeEvent = (CPrecipitationTypeEventItem*)m_TreeView.GetItemData(par) ;
   if(PrecipitationTypeEvent == NULL)
	   return ;
   PrecipitationTypeEvent->SetType(str) ;
   m_TreeView.SetItemText(_item,FormatPrecipitationItemName(str)) ;
}
LRESULT CDlgWeatherScript::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CWnd* pWnd = m_TreeView.GetEditWnd((HTREEITEM)wParam);
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;

			m_TreeView.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
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
			CWnd* pWnd=m_TreeView.GetEditWnd(_item);
			CComboBox* pTCB=(CComboBox*)pWnd;
			HandleComboxChange(pTCB,_item) ;
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
			HTREEITEM par = m_TreeView.GetParentItem(_item) ;
			HTREEITEM root = m_TreeView.GetParentItem(par) ;
            CPrecipitationTypeEventItem* PrecipitationTypeEvent = (CPrecipitationTypeEventItem*)m_TreeView.GetItemData(root) ;
			if(PrecipitationTypeEvent == NULL)
				break ;
            CDlgTimeRange timeranger(PrecipitationTypeEvent->GetStartTime(),PrecipitationTypeEvent->GetEndTime());
			if(timeranger.DoModal() == IDOK)
			{
				PrecipitationTypeEvent->SetStartTime(timeranger.GetStartTime()) ;
				PrecipitationTypeEvent->SetEndTime(timeranger.GetEndTime()) ;

               HTREEITEM child = m_TreeView.GetChildItem(par) ;

			   CString starttime ;
			   CString endtime ;
			   starttime.Format(_T("Day%d %d:%d:%d"),PrecipitationTypeEvent->GetStartTime().GetDay(),PrecipitationTypeEvent->GetStartTime().GetHour(),PrecipitationTypeEvent->GetStartTime().GetMinute(),PrecipitationTypeEvent->GetStartTime().GetSecond()) ;
			   endtime.Format(_T("Day%d %d:%d:%d"),PrecipitationTypeEvent->GetEndTime().GetDay(),PrecipitationTypeEvent->GetEndTime().GetHour(),PrecipitationTypeEvent->GetEndTime().GetMinute(),PrecipitationTypeEvent->GetEndTime().GetSecond()) ;

			   m_TreeView.SetItemText(child,FormatStartTime(starttime)) ;
			   child = m_TreeView.GetNextSiblingItem(child) ;
			   m_TreeView.SetItemText(child,FormatEndTime(endtime)) ;
			}
		}
		break;
	case UM_CEW_LABLE_END:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			 CPrecipitationTypeEventItem* PrecipitationTypeEvent = (CPrecipitationTypeEventItem*)m_TreeView.GetItemData(_item) ;
			 PrecipitationTypeEvent->SetName(strValue) ;
			 m_TreeView.SetItemText(_item,strValue) ;
		}
		break;
	default:
	    break;
	}
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}
void CDlgWeatherScript::OnOK()
{
	OnSave() ;
	CXTResizeDialog::OnOK() ;
}
void CDlgWeatherScript::OnCancel()
{
	CXTResizeDialog::OnCancel() ;
}
void CDlgWeatherScript::OnSave()
{
	m_WeatherScripts->SaveData() ;
}
void CDlgWeatherScript::OnLoad()
{
	CFileDialog filedlg( TRUE,"XML", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"XML File (*.XML)|*.XML;*.XML|All type (*.*)|*.*|", NULL, 0, FALSE  );
	filedlg.m_ofn.lpstrTitle = "Load XML File"  ;
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();
	CWeatherSciptXMLGenerator XMLGenerator ;
	if(XMLGenerator.LoadFile(csFileName,m_WeatherScripts))
	{
		InitTreeView() ;
	}
	else
		MessageBox(_T("Load Data Error")) ;
}
void CDlgWeatherScript::OnSaveAs()
{
	CFileDialog filedlg( FALSE,"XML", NULL, OFN_CREATEPROMPT| OFN_SHAREAWARE| OFN_OVERWRITEPROMPT , \
		"XML File (*.XML)|*.XML;*.XML|All type (*.*)|*.*|", NULL, 0, FALSE  );
	filedlg.m_ofn.lpstrTitle = "Save As"  ;

	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

	CWeatherSciptXMLGenerator XMLGenerator;
	if(XMLGenerator.SaveFile(csFileName,m_WeatherScripts))
	{
		MessageBox(_T("Export Data Successfully")) ;
	}else
	{
		MessageBox(_T("Export Data Error")) ;
	}
}
// CDlgWeatherScript message handlers
