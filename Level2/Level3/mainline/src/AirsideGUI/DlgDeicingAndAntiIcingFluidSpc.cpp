// DlgDeicingAndAntiIcingFluidSpc.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgDeicingAndAntiIcingFluidSpc.h"
#include "../inputairside/AircraftDeicingAndAnti_icingsSettings.h"
#include "../Common/ProDistrubutionData.h"
// CDlgDeicingAndAntiIcingFluidSpc dialog
#include "../Inputs/IN_TERM.H"
#include "DeicingAndAnticingsXMLGenerator.h"
IMPLEMENT_DYNAMIC(CDlgDeicingAndAntiIcingFluidSpc, CXTResizeDialog)
CDlgDeicingAndAntiIcingFluidSpc::CDlgDeicingAndAntiIcingFluidSpc(InputTerminal* inputterminal ,InputAirside* _pInputAirside , PSelectProbDistEntry pSelectProbDistEntry,PSelectFlightType	_pSelectFlightType, CWnd* pParent /*=NULL*/)
	: m_Terminal(inputterminal) ,m_InputAirside(_pInputAirside) ,m_pSelectProbDistEntry(pSelectProbDistEntry),m_pSelectFlightType(_pSelectFlightType),CXTResizeDialog(CDlgDeicingAndAntiIcingFluidSpc::IDD, pParent)
{
	m_Setting = new CAircraftDeicingAndAnti_icingsSettings(m_InputAirside->m_pAirportDB) ;
}

CDlgDeicingAndAntiIcingFluidSpc::~CDlgDeicingAndAntiIcingFluidSpc()
{
	delete m_Setting ;
}

void CDlgDeicingAndAntiIcingFluidSpc::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_treeCtrl) ;
}


BEGIN_MESSAGE_MAP(CDlgDeicingAndAntiIcingFluidSpc, CXTResizeDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_BUTTON_ADD,OnNewButton)
	ON_COMMAND(ID_BUTTON_DEL,OnDelButton)
	ON_COMMAND(ID_LOAD,OnLoad) 
	ON_COMMAND(ID_EXPORT,OnSaveAs)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnTvnSelchangedTreeHoldShortline)
END_MESSAGE_MAP()
void CDlgDeicingAndAntiIcingFluidSpc::OnTvnSelchangedTreeHoldShortline(NMHDR *pNMHDR, LRESULT *pResult)
{
   HTREEITEM _item = m_treeCtrl.GetSelectedItem() ;
   if(m_treeCtrl.GetParentItem(_item) == NULL)
   {
	   m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,TRUE) ;
	   m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,TRUE) ;
   }else
   {
	   m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,FALSE) ;
	   m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,FALSE) ;
   }
}
BOOL CDlgDeicingAndAntiIcingFluidSpc::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	m_Setting->ReadData() ;
	OnInitToolBar() ;
	OnInitTree() ;
	SetResize(m_toolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_STATIC_TOOL,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(ID_LOAD,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	SetResize(ID_EXPORT,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	SetResize(ID_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	return TRUE ;
}
void CDlgDeicingAndAntiIcingFluidSpc::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ; 

}
int CDlgDeicingAndAntiIcingFluidSpc::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1 ;

	if (!m_toolBar.CreateEx(this,  TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP ) ||
		!m_toolBar.LoadToolBar(IDR_TOOLBAR_CONDITION))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}
void  CDlgDeicingAndAntiIcingFluidSpc::OnInitToolBar()
{
	CRect rect ; 
  GetDlgItem(IDC_STATIC_TOOL)->GetWindowRect(&rect) ;
  ScreenToClient(&rect) ;
  GetDlgItem(IDC_STATIC_TOOL)->ShowWindow(SW_HIDE) ;

  m_toolBar.MoveWindow(rect) ;
  m_toolBar.ShowWindow(SW_SHOW) ;

  m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,TRUE) ;
  m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,FALSE) ;

}
void CDlgDeicingAndAntiIcingFluidSpc::OnInitTree()
{
  m_treeCtrl.DeleteAllItems() ;
  CAircraftDeicingAndAnti_icingsSettings::TY_SETTING_ITER iter = m_Setting->GetDataSetting()->begin() ;
  for ( ; iter != m_Setting->GetDataSetting()->end() ;iter++)
  {
	  AddFlightTyTreeNode(*iter) ;
  }
}
CString CDlgDeicingAndAntiIcingFluidSpc::FormatFlightTyNodeName(CString& str)
{
	return _T("Flight Type :  ") + str ;
}
CString CDlgDeicingAndAntiIcingFluidSpc::FormatConditionNodeName(CString& str)
{
	return str ;
}
CString CDlgDeicingAndAntiIcingFluidSpc::FormatPrecipitationtypeNodeName(CString& str)
{
	return str ;
}
CString CDlgDeicingAndAntiIcingFluidSpc::FormatHoldoverTimeNodeName(float time)
{
	CString str ;
	str.Format(_T("Holdover Time(mins) : %0.2f") , time) ;
	return str ;
}
CString CDlgDeicingAndAntiIcingFluidSpc::FormatInspectionNodeName(const CString& time)
{
	CString str ;
	str.Format(_T("Inspection Time(mins) : %s") , time) ;
	return str ;
}
CString CDlgDeicingAndAntiIcingFluidSpc::FormatDe_ICENodeName(float time)
{
	CString str ;
	str.Format(_T("De-Ice Fluid(litres/USG) : %0.2f") , time) ;
	return str ;
}
CString CDlgDeicingAndAntiIcingFluidSpc::FormatAnti_ICENodeName(float time)
{
	CString str ;
	str.Format(_T("Anti-Ice Fluid(litres/USG) : %0.2f") , time) ;
	return str ;
}
void CDlgDeicingAndAntiIcingFluidSpc::AddFlightTyTreeNode(CAircraftDeicingAndAnti_icingsSetting* _constraint)
{
  COOLTREE_NODE_INFO  info  ;
  CCoolTree::InitNodeInfo(this,info) ;
  info.net = NET_NORMAL ;
  info.nt = NT_NORMAL ;
  info.bAutoSetItemText = FALSE ;
  CString Flighttype ;
  _constraint->GetFlightTy()->screenPrint(Flighttype) ;
  HTREEITEM flightNode =  m_treeCtrl.InsertItem(FormatFlightTyNodeName(Flighttype),info,FALSE,FALSE) ;
  m_treeCtrl.SetItemData(flightNode,(DWORD_PTR)_constraint) ;

  //add clear condition 
  AddConditionTreeNode(_constraint->GetClearCondition(),flightNode) ;
  //add Light Frost 
  AddConditionTreeNode(_constraint->GetLightFrostCondition(),flightNode) ;
  //add Ice layer
  AddConditionTreeNode(_constraint->GetIceLayer(),flightNode) ;
  //add Dry/powder snow
  AddConditionTreeNode(_constraint->GetDry_powdersnowCondition(),flightNode) ;
  //add Wet Snow 
  AddConditionTreeNode(_constraint->GetWetSnow(),flightNode) ;
}
void CDlgDeicingAndAntiIcingFluidSpc::AddConditionTreeNode(CAircraftSurfacecondition* condition,HTREEITEM _item)
{
	if(condition == NULL || _item == NULL)
		return ;
	COOLTREE_NODE_INFO  info  ;
	CCoolTree::InitNodeInfo(this,info) ;
	info.net = NET_NORMAL ;
	info.nt = NT_NORMAL ;
	info.bAutoSetItemText = FALSE ;
	HTREEITEM sub_item = m_treeCtrl.InsertItem(FormatConditionNodeName(condition->GetName()) ,info ,FALSE ,FALSE ,_item) ;
	m_treeCtrl.SetItemData(sub_item,(DWORD_PTR)condition) ;
	//add Non-freezing 
	AddPrecipitationTypeNode(condition->GetNonFreezing() , sub_item) ;
	//add Freezing drizzle
	AddPrecipitationTypeNode(condition->GetFreezingDrizzle() ,sub_item) ;
	//add Light Snow 
	AddPrecipitationTypeNode(condition->GetLightSnow() , sub_item) ;
	//add Medium Snow
	AddPrecipitationTypeNode(condition->GetMediumSnow() , sub_item) ;
	//add Heavy snow
	AddPrecipitationTypeNode(condition->GetHeavySnow() ,sub_item) ;
}
void CDlgDeicingAndAntiIcingFluidSpc::AddPrecipitationTypeNode(CPrecipitationtype* type,HTREEITEM _item)
{
	if(type == NULL || _item == NULL)
		return ;
	COOLTREE_NODE_INFO  info  ;
	CCoolTree::InitNodeInfo(this,info) ;
	info.net = NET_NORMAL ;
	info.nt = NT_NORMAL ;
	info.bAutoSetItemText = FALSE ;
	HTREEITEM sub_item = m_treeCtrl.InsertItem(FormatConditionNodeName(type->GetName()) ,info ,FALSE ,FALSE ,_item) ;
	m_treeCtrl.SetItemData(sub_item,(DWORD_PTR)type) ;

	//add hold over 
	info.net = NET_LABLE ;
	HTREEITEM item =  m_treeCtrl.InsertItem(FormatHoldoverTimeNodeName(type->GetHoldOverTime()),info,FALSE,FALSE,sub_item) ;
	m_treeCtrl.SetItemData(item,TY_HOLDOVER_TIME) ;

	//add inspection time 
	info.net  = NET_SHOW_DIALOGBOX ;
	item =  m_treeCtrl.InsertItem(FormatInspectionNodeName(type->GetInspection()->getDistName()),info,FALSE,FALSE,sub_item) ;
	m_treeCtrl.SetItemData(item,TY_Inspection_time) ;

	//add de-ice fluid 
	info.net = NET_LABLE ;
	 item =  m_treeCtrl.InsertItem(FormatDe_ICENodeName(type->GetDeIceTime()),info,FALSE,FALSE,sub_item) ;
	m_treeCtrl.SetItemData(item,TY_DE_ICE) ;

	info.net = NET_LABLE ;
	item =  m_treeCtrl.InsertItem(FormatAnti_ICENodeName(type->GetAntiIceTime()),info,FALSE,FALSE,sub_item) ;
	m_treeCtrl.SetItemData(item,TY_Anti_ICE) ;
}
LRESULT CDlgDeicingAndAntiIcingFluidSpc::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
   switch(message)
   {
   case UM_CEW_SHOW_DIALOGBOX_BEGIN:
	   {
		   HTREEITEM _item = (HTREEITEM)wParam;
		   HTREEITEM par = m_treeCtrl.GetParentItem(_item) ;
           CPrecipitationtype* _type = (CPrecipitationtype*)m_treeCtrl.GetItemData(par) ;

		   CProbDistEntry* pPDEntry = NULL;
		   pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_InputAirside);
		   if(pPDEntry == NULL)
		   {
			   break;
		   }
		   _type->GetInspection()->SetProDistrubution(pPDEntry) ;
		    m_treeCtrl.SetItemText(_item,FormatInspectionNodeName(_type->GetInspection()->getDistName())) ;
	   }
	   break;
   case UM_CEW_LABLE_END:
	   {
		   HTREEITEM _item = (HTREEITEM)wParam;
		   int  nodeTy = m_treeCtrl.GetItemData(_item) ;
		   CString strValue = *((CString*)lParam);
		   HandleEditLable(strValue,nodeTy ,_item) ;
	   }
	   break;
   }
return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}
void CDlgDeicingAndAntiIcingFluidSpc::HandleEditLable(CString str , int _Type ,HTREEITEM _item)
{
	HTREEITEM par = m_treeCtrl.GetParentItem(_item) ;
	CPrecipitationtype* p_type = (CPrecipitationtype*)m_treeCtrl.GetItemData(par) ;
	if(p_type == NULL)
		return ;
	float  val = 0 ;
	val = (float)atof(str) ;
	CString _ItemText ;
   switch(_Type)
   {
   case TY_HOLDOVER_TIME:
	   {
          p_type->SetHoldOverTime(val) ;
		 _ItemText = FormatHoldoverTimeNodeName(val) ;
	   }
   	break;
   case TY_DE_ICE:
	   {
         p_type->SetDeIceTime(val) ;
		 _ItemText = FormatDe_ICENodeName(val) ;
	   }
   	break;
   case TY_Anti_ICE:
	   {
        p_type->SetAntiIceTime(val) ;
		_ItemText = FormatAnti_ICENodeName(val) ;
	   }
       break;
   default:
       break;
   }
   m_treeCtrl.SetItemText(_item,_ItemText) ;
}
BOOL CDlgDeicingAndAntiIcingFluidSpc::CheckFlightTyIsDefined(FlightConstraint& _constraint)
{
   HTREEITEM root = m_treeCtrl.GetRootItem() ;
   if(root == NULL)
	   return FALSE ;
   CAircraftDeicingAndAnti_icingsSetting* setting =  (CAircraftDeicingAndAnti_icingsSetting*)m_treeCtrl.GetItemData(root) ;
   while(root)
   {
     if(setting != NULL)
	 {
		 if(setting->GetFlightTy()->isEqual(&_constraint))
			 return TRUE ;
	 }
	 root = m_treeCtrl.GetNextSiblingItem(root) ;
   }
   return FALSE ;
}
void CDlgDeicingAndAntiIcingFluidSpc::OnNewButton()
{
   FlightConstraint fltcons = (*m_pSelectFlightType)(NULL);
   if(!CheckFlightTyIsDefined(fltcons))
   {
      CAircraftDeicingAndAnti_icingsSetting* setting =  m_Setting->AddFlightTy(fltcons) ;
	   AddFlightTyTreeNode(setting) ;
   }
}
void CDlgDeicingAndAntiIcingFluidSpc::OnDelButton()
{
   HTREEITEM SelItem = m_treeCtrl.GetSelectedItem() ;
   if( m_treeCtrl.GetParentItem(SelItem) == NULL)
   {
      CAircraftDeicingAndAnti_icingsSetting* setting = (CAircraftDeicingAndAnti_icingsSetting*)m_treeCtrl.GetItemData(SelItem) ;
	  m_Setting->DelFlightTy(setting) ;
	  m_treeCtrl.DeleteItem(SelItem) ;
   }
}
void CDlgDeicingAndAntiIcingFluidSpc::OnOK()
{
   OnSave() ;
   CXTResizeDialog::OnOK() ;
}
void CDlgDeicingAndAntiIcingFluidSpc::OnCancel()
{
  CXTResizeDialog::OnCancel() ;
}
void CDlgDeicingAndAntiIcingFluidSpc::OnSave()
{

	m_Setting->WriteData() ;
}
void CDlgDeicingAndAntiIcingFluidSpc::OnSaveAs()
{
	CFileDialog filedlg( FALSE,"XML", NULL, OFN_CREATEPROMPT| OFN_SHAREAWARE| OFN_OVERWRITEPROMPT , \
		"XML File (*.XML)|*.XML;*.XML|All type (*.*)|*.*|", NULL, 0, FALSE  );
	filedlg.m_ofn.lpstrTitle = "Save As"  ;

	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

  CDeicingAndAnticingsXMLGenerator XMLGenerator(m_InputAirside->m_pAirportDB) ;
  if(XMLGenerator.SaveFile(csFileName,m_Setting))
  {
	  MessageBox(_T("Export Data Successfully")) ;
  }else
  {
     MessageBox(_T("Export Data Error")) ;
  }
}
void CDlgDeicingAndAntiIcingFluidSpc::OnLoad()
{
	CFileDialog filedlg( TRUE,"XML", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"XML File (*.XML)|*.XML;*.XML|All type (*.*)|*.*|", NULL, 0, FALSE  );
	filedlg.m_ofn.lpstrTitle = "Load XML File"  ;
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();
	CDeicingAndAnticingsXMLGenerator XMLGenerator(m_InputAirside->m_pAirportDB) ;
	if(XMLGenerator.LoadFile(csFileName,*m_Setting))
	{
	   OnInitTree() ;
	}
	else
		MessageBox(_T("Load Data Error")) ;
}
// CDlgDeicingAndAntiIcingFluidSpc message handlers
