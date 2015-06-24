// DlgAircraftConfigurationSpecView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftConfigurationSpecView.h"
#include "../InputOnBoard/AircaftLayOut.h"
#include "../InputOnBoard/Deck.h"
#include "DlgAircraftDeckNameDefine.h"
// CDlgAircraftConfigurationSpecView dialog
#include "../Common/ACTypesManager.h"
#include "../Common/AirportDatabase.h"
#include "../Inputs/IN_TERM.H"
#include "LayoutView.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "LayOutFrame.h"
#include "LayoutOverUpToolBar.h"
#include "../InputOnBoard/AircraftLayoutEditContext.h"
#include "../InputOnBoard/CInputOnboard.h"
#include "OnBoard/OnboardViewMsg.h"
#include <InputAirside\ARCUnitManager.h>


IMPLEMENT_DYNAMIC(CDlgAircraftConfigurationSpecView, CXTResizeDialog)
CDlgAircraftConfigurationSpecView::CDlgAircraftConfigurationSpecView(CAircaftLayOut* _layout ,InputTerminal* _terminal ,CTermPlanDoc* doc,CWnd* pParent /*=NULL*/)
	:m_layout(_layout),m_Terminal(_terminal),m_Doc(doc), CXTResizeDialog(CDlgAircraftConfigurationSpecView::IDD, pParent)
{
}

CDlgAircraftConfigurationSpecView::~CDlgAircraftConfigurationSpecView()
{
	for (int i = 0 ; i < (int)m_treeNodeData.size() ;i++)
	{
		delete m_treeNodeData[i] ;
	}
	m_treeNodeData.clear() ;
}

void CDlgAircraftConfigurationSpecView::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_CONFIG,m_tree);
}


BEGIN_MESSAGE_MAP(CDlgAircraftConfigurationSpecView, CXTResizeDialog)
	ON_WM_SIZE()
	ON_COMMAND(ID_LAYOUT_ADDDECK,OnAddDeck)
	ON_COMMAND(ID_LAYOUT_COMMENTS,OnDeckComment)
	ON_COMMAND(ID_LAYOUT_HELP,OnDeckHelp)
    ON_COMMAND(ID_DECK_DEL,OnDelDeck)
	ON_COMMAND(ID_DECK_EDITD,OnEditDeck)
	ON_COMMAND(ID_BOTTOM_SAVE,OnSave)
	ON_COMMAND(ID_BUTTOM_SAVEAS,OnSaveAs)
	ON_COMMAND(IDC_BUTTON_CANCEL,OnCancel)
	ON_COMMAND(IDC_BUTTON_OK,OnOK)
		ON_WM_PAINT()
END_MESSAGE_MAP()
void CDlgAircraftConfigurationSpecView::DisplayAllBmpInCtrl()
{
	CString actype = m_layout->GetAcType() ;
	 CACTypesManager* m_pAcMan  = m_Terminal->m_pAirportDB->getAcMan() ;
	CACType* pACType =  m_pAcMan->getACTypeItem(actype) ;
	m_modelHeight = pACType->m_fHeight ;
	m_modelWidth = pACType->m_fLen ;
	//if(pACType == NULL || pACType->m_sFileName.IsEmpty())
	//{
	//	MessageBox("No 3D model has been assigned for this Actype!","Warning",MB_OK) ;
	//	return ;
	//}
	const HBITMAP* hBmp = pACType->GetBmpHandle();
	if (!pACType->m_bLoad)
	{				
		pACType->LoadAllBMPHandle(m_pAcMan->getDBPath());
	}
	DisplayBmpInCtrl(hBmp[2],IDC_STATIC_MODEX);
	DisplayBmpInCtrl(hBmp[1],IDC_STATIC_MODEY) ;
}
void CDlgAircraftConfigurationSpecView::DisplayBmpInCtrl(HBITMAP hBmp,UINT nID)
{
	if (hBmp != NULL)
	{
		CBitmap bmpObj;
		bmpObj.Attach(hBmp);
		CWnd *pWnd = GetDlgItem(nID);
		CDC *pDC = pWnd->GetDC();
     
		CDC DCCompatible;
		DCCompatible.CreateCompatibleDC(pDC);
		CWnd::DefWindowProc(WM_PAINT, (WPARAM)DCCompatible.m_hDC , 0);

		CRect rect;
		pWnd->GetClientRect(rect);
		CBitmap *pOldBmp = DCCompatible.SelectObject(&bmpObj);
		BITMAP bmpInfo;
		bmpObj.GetObject(sizeof(bmpInfo),&bmpInfo);

		pDC->SetStretchBltMode(STRETCH_HALFTONE);
		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &DCCompatible, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY );
		DCCompatible.SelectObject(pOldBmp);
		bmpObj.Detach();
		if(nID == IDC_STATIC_MODEX)
		{
			m_XmodelHeight = bmpInfo.bmHeight ;
			m_XmodelWidth = bmpInfo.bmWidth ;
		//	m_xPercent = bmpInfo.bmHeight / m_modelHeight ;
		}
		if(nID == IDC_STATIC_MODEY)
		{
			m_YmodelHeight = bmpInfo.bmHeight ;
			m_YmodeWidth = bmpInfo.bmWidth ;
		//	m_ypercent = bmpInfo.bmHeight / m_modelHeight ;
		}
		return;

	}
	// bkcolor
	CRect rcClient;
	CWnd *pWnd = GetDlgItem(nID);
	CDC *dc = pWnd->GetDC();
	pWnd->GetClientRect(&rcClient);

	CBrush pOrgBrush(RGB(255,255,255));
	dc->FillRect(rcClient,&pOrgBrush);
}
void CDlgAircraftConfigurationSpecView::OnPaint()
{	
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl))
			|| pWnd == GetDlgItem(IDC_STATIC_MODEX)
			|| pWnd == GetDlgItem(IDC_STATIC_MODEY))
		{
			if(!pWnd->IsWindowVisible())
			{
				hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
				continue;
			}
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);

	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);
	DisplayAllBmpInCtrl();
	/*CAircraftModel acModel ;
	acModel.SetType(m_layout->GetAcType());
	acModel.Load(m_Doc->GetInputOnboard()->GetAirportDB()->getFolder());
	m_layout->UpdateDecksOutline(acModel);
	for (int i = 0 ; i < m_layout->GetDeckManager()->getCount() ;i++)
	{
		AddLineToPic(&dc , m_layout->GetDeckManager()->getItem(i)) ;
	}*/
	CXTResizeDialog::OnPaint();
}
void CDlgAircraftConfigurationSpecView::OnSize(UINT nType, int cx, int cy)
{
	if(m_tree.GetSafeHwnd() == NULL)
		return ; 
	CRect newrect ;
	CXTResizeDialog::OnSize(nType,cx,cy) ;
	m_tree.GetWindowRect(&newrect) ;
	ScreenToClient(newrect) ;
	int offx = newrect.Width() - m_oldRect.Width() ;
	CRect x_view ;
	GetDlgItem(IDC_STATIC_MODEX)->GetWindowRect(&x_view) ;
	ScreenToClient(x_view) ;
	GetDlgItem(IDC_STATIC_MODEX)->SetWindowPos(NULL,x_view.left,x_view.top,x_view.Width() + offx,x_view.Height(),NULL) ;

	CRect y_view ;
	GetDlgItem(IDC_STATIC_MODEY)->GetWindowRect(&y_view) ;
	ScreenToClient(y_view) ;
	GetDlgItem(IDC_STATIC_MODEY)->SetWindowPos(NULL,y_view.left + offx ,y_view.top ,y_view.Width() - offx ,x_view.Height(),NULL) ;
	m_tree.GetWindowRect(&m_oldRect) ;
	ScreenToClient(m_oldRect) ;
}
BOOL CDlgAircraftConfigurationSpecView::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	CString actype = m_layout->GetAcType() ;
	CACTypesManager* m_pAcMan  = m_Terminal->m_pAirportDB->getAcMan() ;
	CACType* pACType =  m_pAcMan->getACTypeItem(actype) ;
	m_modelHeight = 50 ;
	InitDialogCaption() ;
	InitConfigTree() ;
	InitModelView() ;
	DisplayAllBmpInCtrl() ;
	SetResize(IDC_STATIC_CONFIG_TREE,SZ_TOP_LEFT,SZ_BOTTOM_CENTER) ;
	SetResize(IDC_STATIC_MODEX,SZ_TOP_CENTER,SZ_BOTTOM_CENTER) ;
	SetResize(IDC_STATIC_MODEY,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_TREE_CONFIG,SZ_TOP_LEFT,SZ_BOTTOM_CENTER) ;
	SetResize(ID_BOTTOM_SAVE,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	SetResize(ID_BUTTOM_SAVEAS,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	SetResize(IDC_BUTTON_CANCEL,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	SetResize(IDC_BUTTON_OK ,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	return TRUE ;
}
void CDlgAircraftConfigurationSpecView::InitDialogCaption()
{
   CString caption ;
   caption.Format(_T("Aircraft Configuration Specification View - %s"),m_layout->GetName()) ;
   SetWindowText(caption) ;
}
void CDlgAircraftConfigurationSpecView::InitConfigTree()
{
     COOLTREE_NODE_INFO info ;
	 m_tree.InitNodeInfo(this,info) ;
	 info.net = NET_NORMAL ;
	 info.nt = NT_NORMAL ;
	 info.bAutoSetItemText = FALSE ;
	HTREEITEM root =  m_tree.InsertItem("Aircraft Configuration",info,FALSE) ;

	//insert layout node 
	info.unMenuID = IDR_MENU_ONBOARD_LAYOUT ;
    CNodeData* nodedata = new CNodeData ;
	nodedata->m_Ty = TY_NODE_LAYOUT ;
	nodedata->m_DwData = m_layout ;
	m_treeNodeData.push_back(nodedata) ;
	m_LayoutItem = m_tree.InsertItem(m_layout->GetName(),info,FALSE,FALSE,root) ;
	m_tree.SetItemData(m_LayoutItem,(DWORD_PTR)nodedata) ;
	CDeckManager* deckManager = m_layout->GetDeckManager() ;
	for (int i = 0 ; i < deckManager->getCount() ;i++)
	{
		AddDeckTreeNode(m_LayoutItem,deckManager->getItem(i)) ;
		//AddLineToPic(deckManager->getItem(i)) ;
	}
	m_tree.Expand(m_LayoutItem,TVE_EXPAND) ;
	m_tree.Expand(root,TVE_EXPAND) ;
}
void CDlgAircraftConfigurationSpecView::AddDeckTreeNode(HTREEITEM _item , CDeck* _deck)
{
	if(_item == NULL || _deck == NULL)
		return ;
	COOLTREE_NODE_INFO info ;
	m_tree.InitNodeInfo(this,info) ;
	info.net = NET_NORMAL ;
	info.nt = NT_NORMAL ;
	info.unMenuID = IDR_MENU_DELETE_DECK ;
	info.bAutoSetItemText = FALSE ;
	HTREEITEM decknode =  m_tree.InsertItem(_deck->GetName(),info,FALSE,FALSE,_item) ;
	CNodeData* nodedata = new CNodeData ;
	nodedata->m_Ty = TY_NODE_DECK ;
	nodedata->m_DwData = _deck ;
	m_tree.SetItemData(decknode,(DWORD_PTR)_deck) ;
}
void CDlgAircraftConfigurationSpecView::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_layout->GetDeckManager()->WriteDeckToDataBase() ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}
void CDlgAircraftConfigurationSpecView::OnSaveAs()
{
	/*	CFileDialog filedlg( FALSE,"zip", NULL, OFN_CREATEPROMPT| OFN_SHAREAWARE| OFN_OVERWRITEPROMPT , \
			"ZIP File (*.zip)|*.zip;*.zip|All type (*.*)|*.*|", NULL );
		filedlg.m_ofn.lpstrTitle = "Export AirCraft Configuration" ;
		if(filedlg.DoModal()!=IDOK)
			return;
		CString csFileName=filedlg.GetPathName();
		CAircraftModelConfigLoadManager xmlGenerator(m_Terminal) ;
		if(!xmlGenerator.ExportAircraftModelConfiguration(m_layout,csFileName))
			MessageBox(_T("Export Configuration Error!"),"Error",MB_OK) ;
		else
			MessageBox(_T("Export Configuration Successfully !"),"Success",MB_OK) ;*/
}
void CDlgAircraftConfigurationSpecView::OnOK()
{
	OnSave() ;
	CXTResizeDialog::OnOK() ;
	if(m_Doc != NULL)
	   m_Doc->UpdateAllViews((CView*)this,VM_ONBOARD_DECK_CHANGED) ;
}
void CDlgAircraftConfigurationSpecView::OnCancel()
{
	m_layout->GetDeckManager()->ReloadData() ;
	CXTResizeDialog::OnCancel() ;
}
void  CDlgAircraftConfigurationSpecView::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
{

}
LRESULT CDlgAircraftConfigurationSpecView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_SHOWMENU_BEGIN:
	case UM_CEW_SHOWMENU_END:
		{			
			m_Rclick =( HTREEITEM)wParam;
		}
		break ;
	default:
		break ;
	}
	return  CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgAircraftConfigurationSpecView::OnAddDeck()
{
    CDeck* pdeck = new CDeck ;
	CDlgAircraftDeckNameDefine dlgDeckDefine(pdeck,this) ;
	if(dlgDeckDefine.DoModal() != IDOK)
	{
		delete pdeck ;
		return ;
	}


	m_layout->GetDeckManager()->addItem(pdeck) ;
	AddDeckTreeNode(m_LayoutItem,pdeck) ;
	//AddLineToPic(pdeck) ;
	/*CAircraftModel acModel ;
	acModel.SetType(m_layout->GetAcType());
	acModel.Load(m_Doc->GetInputOnboard()->GetAirportDB()->getFolder());
	m_layout->UpdateDecksOutline(acModel);*/
	OnPaint() ;
}
void CDlgAircraftConfigurationSpecView::OnDelDeck()
{
   CDeck* _peck = (CDeck*)m_tree.GetItemData(m_Rclick) ;
   m_layout->GetDeckManager()->RemoveDeck(_peck) ;
   m_tree.DeleteItem(m_Rclick) ;

}
void CDlgAircraftConfigurationSpecView::OnEditDeck()
{
   CDeck* _peck = (CDeck*)m_tree.GetItemData(m_Rclick) ;
   CDlgAircraftDeckNameEdit  editDlg(_peck,this) ;
   if(editDlg.DoModal() == IDOK)
        m_tree.SetItemText(m_Rclick,_peck->GetName()) ;
  /* CAircraftModel acModel ;
   acModel.SetType(m_layout->GetAcType());
   acModel.Load(m_Doc->GetInputOnboard()->GetAirportDB()->getFolder());
   m_layout->UpdateDecksOutline(acModel);*/
   OnPaint() ;
}
int CDlgAircraftConfigurationSpecView::GetPenWidth(CDeck* _deck)
{
	double thickness =CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_METER,_deck->GetThickness()) ;
	if(thickness < 0.5)
		return 1 ;
	return (int)(thickness/0.5) ;

}
void CDlgAircraftConfigurationSpecView::AddLineToPic(CPaintDC* pDC ,CDeck* _deck)
{
   if(_deck == NULL)
	   return ;
   CWnd *pWnd = GetDlgItem(IDC_STATIC_MODEX);
   CDC* pd = pWnd->GetDC() ;
   CPen pen(PS_USERSTYLE,GetPenWidth(_deck),RGB(255,125,125)) ;
   pd->SelectObject(pen) ;
   CPoint p1 ;
   CPoint p2 ;
   CString actype = m_layout->GetAcType() ;
   CACTypesManager* m_pAcMan  = m_Terminal->m_pAirportDB->getAcMan() ;
   CACType* pACType =  m_pAcMan->getACTypeItem(actype) ;
   _deck->GetXProjPts(p1,p2,pACType) ;
   CRect rect;
   pWnd->GetWindowRect(&rect) ;
   ScreenToClient(rect) ;

   double lenp1_y = (double)p1.y/(double)m_XmodelHeight ;
   double lenp1_x = (double)p1.x/m_XmodelWidth ;

   double lenp2_y = (double)p2.y/(double)m_XmodelHeight ;
   double lenp2_x = (double)p2.x/m_XmodelWidth ;

   pd->MoveTo((int)(lenp1_x * rect.Width()),(int)((lenp1_y)* rect.Height())) ;
   pd->LineTo((int)(lenp2_x * rect.Width()),(int)((lenp2_y)* rect.Height())) ;

  pWnd = GetDlgItem(IDC_STATIC_MODEY);
 _deck->GetYProjPts(p1,p2,pACType) ;
  pd = pWnd->GetDC() ;
  pd->SelectObject(pen) ;

   lenp1_y = (double)p1.y/(double)m_YmodelHeight ;
   lenp1_x = (double)p1.x/m_YmodeWidth ;

   lenp2_y = (double)p2.y/(double)m_YmodelHeight ;
   lenp2_x = (double)p2.x/m_YmodeWidth ;

   pd->MoveTo((int)(lenp1_x * rect.Width()),(int)((lenp1_y)* rect.Height())) ;
   pd->LineTo((int)(lenp2_x * rect.Width()),(int)((lenp2_y)* rect.Height())) ;
  //delete dc ;
}

void CDlgAircraftConfigurationSpecView::DelLineFromPic()
{
  
}
void CDlgAircraftConfigurationSpecView::InitModelView()
{
   DisplayAllBmpInCtrl() ;
}
void CDlgAircraftConfigurationSpecView::OnDeckComment()
{

}
void CDlgAircraftConfigurationSpecView::OnDeckHelp()
{

}
// #include "OnBoard/AircraftLayoutView.h"

// CDlgAircraftConfigurationSpecView message handlers
