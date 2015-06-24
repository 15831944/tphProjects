// EnvironmentMSView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "EnvironmentMSView.h"
#include "XPStyle/NewMenu.h"
#include "TreeItemLayoutObject.h"
#include ".\environmentmsview.h"
#include "DlgAirPollutantsList.h"
#include "DlgFuelsProperties.h"
#include "PollutantSourceEmissionSheet.h"



#define IDC_ENVIRONMENT_TREE 3
//Tree Ctrl Helper , support some functions for tree ctrl

using namespace MSV;

// CEnvironmentMSView

IMPLEMENT_DYNCREATE(CEnvironmentMSView, CView)
CEnvironmentMSView::CEnvironmentMSView()
{
	mpNodeDataAllocator= new CNodeDataAllocator();
}

CEnvironmentMSView::~CEnvironmentMSView()
{
	delete mpNodeDataAllocator;
}


BEGIN_MESSAGE_MAP(CEnvironmentMSView, CView)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()

END_MESSAGE_MAP()

// CCargoMSView drawing
static const LOGFONT lfTitle = 
{
	20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Arial"
};
void CEnvironmentMSView::OnDraw(CDC* pDC)
{
	
}

void CEnvironmentMSView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CEnviromentTreeCtrl treeCtrl(m_wndTreeCtrl,mpNodeDataAllocator);
	treeCtrl.LoadImage(m_imgList);
	treeCtrl.InitTree();

	SetScrollPos(SB_VERT,0);
}
BOOL CEnvironmentMSView::PreCreateWindow( CREATESTRUCT& cs )
{
	if(!CView::PreCreateWindow(cs))
		return FALSE;

	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | WS_CLIPCHILDREN;

	return TRUE;
}

int CEnvironmentMSView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS |TVS_EDITLABELS ;


	BOOL bResult = m_wndTreeCtrl.Create(dwStyle, CRect(0,0,0,0),
		this, IDC_ENVIRONMENT_TREE);

	return (bResult ? 0 : -1);
}
// CCargoMSView diagnostics

#ifdef _DEBUG
void CEnvironmentMSView::AssertValid() const
{
	CView::AssertValid();
}

void CEnvironmentMSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

// CEnvironmentMSView message handlers
void CEnvironmentMSView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{

	CView::OnUpdate(pSender, lHint, pHint);
	/////////////////////////////////////////////////
	//send message to tree ctrl to make h_scroolbar scroll to end_left 	
	GetTreeCtrl().SendMessage( WM_HSCROLL, MAKEWPARAM(SB_LEFT,0),(LPARAM)NULL );

}

void CEnvironmentMSView::OnSize( UINT nType, int cx, int cy )
{
	CView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndTreeCtrl.m_hWnd))
		m_wndTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);
}

LRESULT CEnvironmentMSView::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case WM_LBUTTONDBLCLK:
		OnLButtonDblClk(wParam,lParam);
		break;
	case ENDLABELEDIT:
		//OnEndLabelEdit(wParam,lParam);
		break;
	case BEGINLABELEDIT:
		//OnBeginLabelEdit(wParam,lParam);
		break;
	case SELCHANGED:
		//OnSelChanged(wParam,lParam);
		break;
	default:
		break;
	}
	return CView::WindowProc(message, wParam, lParam);	
}

void CEnvironmentMSView::OnRClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	// Send WM_CONTEXTMENU to self
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	// Mark message as handled and suppress default handling
	*pResult = 1;
}

void CEnvironmentMSView::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CPoint cliPt = point;
	ScreenToClient(&cliPt);
	CNewMenu menu, *pCtxMenu = NULL;
	UINT uFlags;
	m_hRightClkItem = GetTreeCtrl().HitTest(cliPt, &uFlags);
}

void CEnvironmentMSView::OnLButtonDblClk( WPARAM wParam, LPARAM lParam )
{
	UINT uFlags;
	CPoint point = *(CPoint*)lParam;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	CString strNodeName; 

	//////////////////////////////////////////////////////////////////////////
	if((hItem != NULL)&& (TVHT_ONITEM & uFlags))
	{
		strNodeName = GetTreeCtrl().GetItemText(hItem);

		CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hItem);
		ASSERT(pNodeData != NULL);

		if (pNodeData == NULL)
			return;
		if(pNodeData->nodeType == NodeType_Dlg)
		{
			ShowDialog(pNodeData->nSubType);
		}
		else if(pNodeData->nodeType == NodeType_Normal)
		{
#ifdef _DEBUG
			ASSERT(NULL);
#endif
		}
		else
		{
			ASSERT(0);
		}

	}
}

void MSV::CEnvironmentMSView::ShowDialog( int nEnum )
{	
	ASSERT(nEnum >= enumDialog_0 && nEnum < enumDialog_Max);
	enumDialog enumdlg	= enumDialog_0;
	if(nEnum >= enumDialog_0 && nEnum < enumDialog_Max)
		enumdlg = (enumDialog)nEnum;

	switch (enumdlg)
	{
	case enumDialog_0:
	case enumDialog_Max:
		{
#ifdef _DEBUG
			ASSERT(NULL);
#endif
		}
		break;

	case enumDialog_AirPollutants:
		{
			TRACE("\r\n Call Dialog Air Pollutants List \r\n ");
			//ASSERT(NULL);			
			
			CDlgAirPollutantsList dlg;
			dlg.DoModal();
			
		}
		break;
	case enumDialog_FuelsProperties:
		{
			TRACE("\r\n Call Dialog Fuel and Properties \r\n ");
			//ASSERT(NULL);
			CDlgFuelsProperties dlg;
			dlg.DoModal();
		}
		break;
	case enumDialog_SourcesEmissions:
		{
			TRACE("\r\n Call Dialog Sources and Emissions \r\n ");
			
			CPollutantSourceEmissionSheet sheet("Pollutant Source and Emission",this);
			sheet.DoModal();
		}
		break;

	default:
		{

		}

	}
}




//////////////////////////////////////////////////////////////////////////
//CEnvironmentMSView
CEnvironmentMSView::CEnviromentTreeCtrl::CEnviromentTreeCtrl( CARCBaseTree& tree,MSV::CNodeDataAllocator* pNodeDataAlloc ) 
:m_treeCtrl(tree)
{
	m_pNodeDataAlloc = pNodeDataAlloc;
}

void CEnvironmentMSView::CEnviromentTreeCtrl::LoadImage( CImageList& imglist )
{
	if (imglist.m_hImageList == NULL)
	{
		imglist.Create(16,16,ILC_COLOR8|ILC_MASK,0,1);
		CBitmap bmp;
		bmp.LoadBitmap(IDB_COOLTREE);
		imglist.Add(&bmp,RGB(255,0,255));
	}
	CBitmap bmL;

	bmL.LoadBitmap(IDB_SMALLNODEIMAGES);
	imglist.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_NODESMALLIMG);
	imglist.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_FLOORSMALLIMG);
	imglist.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_AREASSMALLIMG);
	imglist.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_PORTALSSMALLIMG);
	imglist.Add(&bmL, RGB(255,0,255));

	m_treeCtrl.SetImageList(&imglist,TVSIL_NORMAL);
	m_treeCtrl.SetImageList(&imglist,TVSIL_STATE);
}

void CEnvironmentMSView::CEnviromentTreeCtrl::InitTree()
{
	CTreeCtrlItem rootItem(&m_treeCtrl, TVI_ROOT);
	//environment
	CString s;
	s = "Environment";
	CTreeCtrlItem envItem = AddItem(s, rootItem);
	{
		//Air quality
		s = "Air Quality";
		CTreeCtrlItem airQItem = AddItem(s,envItem);
		{
			//Air Pollutant List
			s = "Air Pollutants List";
			CTreeCtrlItem airPollutItem = AddItem(s,airQItem,NodeType_Dlg,enumDialog_AirPollutants);
			//Fuels and Properties
			s = "Fuels and Properties";
			CTreeCtrlItem fulePropItem = AddItem(s,airQItem,NodeType_Dlg,enumDialog_FuelsProperties);
			//sources and emissions
			s = "Sources and Emissions";
			CTreeCtrlItem srcEmissItem = AddItem(s,airQItem,NodeType_Dlg,enumDialog_SourcesEmissions);
			airQItem.Expand();
		}
		//Energy Consumption
		s = "Energy Consumption";
		AddItem(s,envItem);
		//Solid Waste
		s = "Solid Waste";
		AddItem(s,envItem);
		//Biologically Sensitive Areas
		s = "Biologically Sensitive Areas";
		AddItem(s,envItem);
		//Water
		s =  "Water";
		AddItem(s,envItem);
		//Noise
		s = "Noise";
		AddItem(s,envItem);
		//Reflectivity
		s = "Reflectivity";
		AddItem(s,envItem);

		envItem.Expand();
	}
	//Settings
	s = "Settings";
	AddItem(s,rootItem);
	//Analysis Parameters
	s = "Analysis Parameters";
	AddItem(s,rootItem);

	rootItem.Expand();
}

CTreeCtrlItem CEnvironmentMSView::CEnviromentTreeCtrl::AddItem( const CString& strText, CTreeCtrlItem& hParent, MSV::enumNodeType enumType, enumDialog enumDlg /*= enumDialog_0*/ )
{
	//add item
	CTreeCtrlItem hChild = hParent.AddChild(strText);

	//node information
	MSV::CNodeData *pNodeData =  m_pNodeDataAlloc->allocateNodeData(enumType);
	pNodeData->nSubType = enumDlg;
	hChild.SetData((DWORD)pNodeData);

	return hChild;
}

CEnvironmentMSView::CEnviromentTreeCtrl::~CEnviromentTreeCtrl()
{
	std::vector<MSV::CNodeData *>::iterator iter =  m_vNodeData.begin();
	for (;iter != m_vNodeData.end(); ++ iter)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vNodeData.clear();



}


