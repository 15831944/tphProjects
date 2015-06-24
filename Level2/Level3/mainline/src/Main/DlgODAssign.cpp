// DlgODAssign.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgODAssign.h"
#include "../Landside/LandSideDefine.h"
#include "../Landside/ODDetail.h"
#include "../Landside/ODTimeRange.h"
#include "../Landside/ODFligthType.h"
#include <vector>
#include "DlgTimeRange.h"
#include "DlgCurbsideSelect.h"
#include "TermPlanDoc.h"
#include "Resource.h"
#include ".\dlgodassign.h"
#include "FlightDialog.h"

// CDlgODAssign dialog

IMPLEMENT_DYNAMIC(CDlgODAssign, CXTResizeDialog)
CDlgODAssign::CDlgODAssign(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgODAssign::IDD, pParent)
{
}
CDlgODAssign::CDlgODAssign(CTermPlanDoc *tmpDoc,CWnd*pParent/* =NULL */)
:CXTResizeDialog(CDlgODAssign::IDD,pParent)
{
  pDoc=tmpDoc;
}
CDlgODAssign::~CDlgODAssign()
{
}
BOOL CDlgODAssign::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	if(!m_treeToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)||
		!m_treeToolBar.LoadToolBar(IDR_FLIGHTMANAGER_TOOLBAR))
	{
		TRACE("Failed create toolbar\n");
	}
	if (!m_OriginToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)||
		!m_OriginToolBar.LoadToolBar(IDR_ACTYPE_ALIASNAME))
	{
		TRACE("Failed create toolbar\n");
	}
	if (!m_destToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)||!m_destToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))

	{
		TRACE("Failed create toolbar\n");
	}
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
	m_OriginToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE);
	m_OriginToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
	m_OriginToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);

	m_destToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_destToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,FALSE);
	m_destToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );

	InitToolBar();
	SetResize(IDC_STATIC_ODASSIGNMENT,SZ_TOP_LEFT,SZ_BOTTOM_LEFT);
	SetResize(m_treeToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_ODASSIGNMENT,SZ_TOP_LEFT,SZ_BOTTOM_LEFT);

	SetResize(IDC_STATIC_ODDETAIL,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);

	SetResize(m_OriginToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_ODORIGINASSIGN,SZ_TOP_LEFT,SZ_TOP_RIGHT);

	SetResize(m_destToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_ODDESTASSIGN,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);

	SetResize(IDODSAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	//m_flightTypeList.ReadData();
    m_pLandSide=pDoc->getARCport()->m_pInputLandside;
	DWORD dwStyle = m_originCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_originCtrl.SetExtendedStyle(dwStyle);

	// set the runway assign list header contents
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Origin");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_originCtrl.InsertColumn(0, &lvc);
	lvc.pszText = _T("Percent");
	lvc.fmt = LVCFMT_NUMBER;
	m_originCtrl.InsertColumn(1, &lvc);

	dwStyle=m_destCtrl.GetExtendedStyle();
	dwStyle|=LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_destCtrl.SetExtendedStyle(dwStyle);
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Dest");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_destCtrl.InsertColumn(0, &lvc);

	lvc.pszText = _T("Percent");
	lvc.fmt = LVCFMT_NUMBER;
	m_destCtrl.InsertColumn(1,&lvc);
	InitTree();
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;

}
void CDlgODAssign::InitTree()
{
   int i=0;
   CODFligthType *flightType;
   HTREEITEM hParentItem;
   CString strFilghtType;
   for (i=0;i<m_pLandSide->getODFlightTypeList().getItemCount();i++)
   {
	   flightType=m_pLandSide->getODFlightTypeList().getItem(i);
	   flightType->getFlightConstraint().screenPrint(strFilghtType.GetBuffer(1024));
       hParentItem=m_treeCtrl.InsertItem(strFilghtType,TVI_ROOT);
	   m_treeCtrl.SetItemData(hParentItem,(DWORD_PTR)flightType);
	  
	   m_treeCtrl.Expand(hParentItem,TVE_EXPAND);
   }
}

BOOL CDlgODAssign::getTreeItemByType(TreeNodeType nodeType,HTREEITEM &hItem )
{
	HTREEITEM selectItem=m_treeCtrl.GetSelectedItem();
	HTREEITEM hParentItem;
	if (selectItem==NULL)
		return false;
	switch(nodeType)
	{
	case TREENODE_FLTTYPE:
		hParentItem=m_treeCtrl.GetParentItem(selectItem);
		if (hParentItem!=NULL)
		{
			return FALSE;
		}
		hItem=selectItem;
		break;
	case TREENODE_TIMERANGE:
		hParentItem=m_treeCtrl.GetParentItem(selectItem);
		if(hParentItem==NULL)
		{
			return FALSE;
		}
		hItem=selectItem;

		break;
	default:
		return FALSE;


	}
	return TRUE;
}
void CDlgODAssign::InitToolBar()
{
	CRect recttree;
	CRect rectOrigin;
	CRect rectDest;
	m_treeCtrl.GetWindowRect(&recttree);
	ScreenToClient(&recttree);
	recttree.left=recttree.left+2;
	recttree.top=recttree.top-26;
	recttree.bottom=recttree.top+22;
	m_treeToolBar.MoveWindow(&recttree);

	m_originCtrl.GetWindowRect(&rectOrigin);
	ScreenToClient(&rectOrigin);
	rectOrigin.left=rectOrigin.left+4;
	rectOrigin.top=rectOrigin.top-26;
	rectOrigin.bottom=rectOrigin.top+22;
	m_OriginToolBar.MoveWindow(&rectOrigin);

	m_destCtrl.GetWindowRect(&rectDest);
	ScreenToClient(&rectDest);
	rectDest.left=rectDest.left+4;
	rectDest.top=rectDest.top-26;
	rectDest.bottom=rectDest.top+22;
	m_destToolBar.MoveWindow(&rectDest);
}
void CDlgODAssign::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	CRect rect;
	m_treeCtrl.GetWindowRect(&rect);
	if(!rect.PtInRect(pos))
	{
		return;
	}
	HTREEITEM hItem;
	CPoint pt=pos;
	m_treeCtrl.ScreenToClient(&pt);
	UINT iRet;
	hItem=m_treeCtrl.HitTest(pt,&iRet);
	if (iRet!=TVHT_ONITEMLABEL)
	{
		hItem=NULL;
	}
	if(hItem==NULL)
		return;
	//CMenu menu;
	//m_treeCtrl.SelectItem(hItem);
	//menu.LoadMenu(IDR_MENU27);
 //  CMenu *pMenu=menu.GetSubMenu(0);
 //  if (getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
 //  {
	//   pMenu->EnableMenuItem(ID_LANDSIDE_DELTIMERANGE,MF_GRAYED);
	//   pMenu->EnableMenuItem(ID_LANDSIDE_EDITTIMERAGE,MF_GRAYED);
 //  }
 //  else if(getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem))
 //  {
	//   pMenu->EnableMenuItem(ID_LANDSIDE_NEWTIMERANGE,MF_GRAYED);
 //  }
 //  if (pMenu)
	//   pMenu->TrackPopupMenu(TPM_LEFTALIGN,pos.x,pos.y,this);
   {
   }
}
void CDlgODAssign::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ODASSIGNMENT, m_treeCtrl);
	DDX_Control(pDX, IDC_LIST_ODORIGINASSIGN, m_originCtrl);
	DDX_Control(pDX, IDC_LIST_ODDESTASSIGN, m_destCtrl);
}


BEGIN_MESSAGE_MAP(CDlgODAssign, CXTResizeDialog)

	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ODASSIGNMENT, OnTvnSelchangedTreeOdassignment)
	ON_COMMAND(ID_TOOLBAR_ADD,OnNewFltType)
	ON_COMMAND(ID_TOOLBAR_DEL,OnDelFltType)
	ON_COMMAND(ID_TOOLBAR_EDIT,OnEditFltType)
	ON_COMMAND(IDPARKINGLOTASSIGNSAVE,OnBtnSave)
	ON_COMMAND(ID_LANDSIDE_EDITTIMERAGE,OnEditTimeRange)
	ON_COMMAND(ID_LANDSIDE_NEWTIMERANGE,OnNewTimeRange)
	ON_COMMAND(ID_LANDSIDE_DELTIMERANGE,OnDelTimeRange)
	ON_COMMAND(ID_ACTYPE_ALIAS_EDIT,OnEditOriginOD)
	ON_COMMAND(ID_ACTYPE_ALIAS_DEL,OnDelOriginOD)
	ON_COMMAND(ID_ACTYPE_ALIAS_ADD,OnNewOriginOD)
	ON_COMMAND(ID_PEOPLEMOVER_CHANGE,OnEditDestOD)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE,OnDelDestOD)
	ON_COMMAND(ID_PEOPLEMOVER_NEW,OnNewDestOD)
	//ON_COMMAND(ID_TOOLBAR_EDIT
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ODORIGINASSIGN, OnLvnItemchangedListOdoriginassign)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ODORIGINASSIGN, OnLvnEndlabeleditListOdoriginassign)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ODDESTASSIGN, OnLvnItemchangedListOddestassign)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ODDESTASSIGN, OnLvnEndlabeleditListOddestassign)
END_MESSAGE_MAP()
void CDlgODAssign::OnEditDestOD()
{
	int nCursel=m_destCtrl.GetCurSel();
	if (nCursel<0)
	{
		return;
	}
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
	{
		return;
	}
	CODDetail *detail=(CODDetail*)m_destCtrl.GetItemData(nCursel);
	LandsideLayoutObjectList *objectList=&(pDoc->getARCport()->m_pInputLandside->getObjectList());
	CDlgLandsideObjectSelect dlg(objectList,ALT_LEXT_NODE,m_pParentWnd);
	if (dlg.DoModal()==IDOK)
	{
		detail->setName(dlg.getCurbSideName());
		m_destCtrl.SetItemText(nCursel,0,dlg.getCurbSideName());
	}
}
void CDlgODAssign::OnNewDestOD()
{
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
	{
		return;
	}

	LandsideLayoutObjectList *objectList= &(m_pLandSide->getObjectList());
	CDlgLandsideObjectSelect dlg(objectList,ALT_LEXT_NODE,m_pParentWnd);
	int nCount = m_destCtrl.GetItemCount();	
	if (dlg.DoModal()==IDOK)
	{
		CODFligthType *flightType=(CODFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
		CODDetail *detail=new CODDetail();
		detail->setType(ODAssignment_Dest);
		flightType->AddItem(detail);
		m_destCtrl.InsertItem(m_destCtrl.GetItemCount(),"");
		m_destCtrl.SetItemData(m_destCtrl.GetItemCount()-1,(DWORD_PTR)detail);

		CString strPercent = _T("");
		int i = 0;
		strPercent.Format(_T("%d"),100/(nCount + 1));
		for (i = 0; i < nCount + 1; i++)
		{
			if(i == nCount)
				strPercent.Format(_T("%d"),100-nCount*(100/(nCount + 1)));
			m_destCtrl.SetItemText(i,1,strPercent);
		}
		detail->setName(dlg.getCurbSideName());
		m_destCtrl.SetItemText(m_destCtrl.GetItemCount()-1,0,dlg.getCurbSideName());
		i = 0;
		for(int j=0;j<m_destCtrl.GetItemCount();j++)
		{
			CODDetail *itrPercent=(CODDetail*)m_destCtrl.GetItemData(j);
			if(j== nCount)
				itrPercent->setPercent(100-nCount*(100/(nCount + 1)));
			else
				itrPercent->setPercent(100/(nCount + 1));
			itrPercent->setParentID(flightType->getID());

		}
	}
}
void CDlgODAssign::OnDelDestOD()
{
	int averPercent=0;
	int alTotal=0;
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
	{
		return;
	}
	int nCurSel=m_destCtrl.GetCurSel();
	if (nCurSel<0)
	{
		return;
	}
	CODDetail *detail=(CODDetail*)m_destCtrl.GetItemData(nCurSel);
	CODFligthType *flightType=(CODFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	for (int i=0;i<flightType->getItemCount();i++)
	{
		if (detail==flightType->getItem(i))
		{
			flightType->deleteItem(i);
			m_destCtrl.DeleteItem(nCurSel);
		}
	}

	
	alTotal=m_destCtrl.GetItemCount();
	if (alTotal<=0)
	{
		return ;
	}
	averPercent=100/alTotal;
	CString strPercent="";
	int i=0;
	for (; i<alTotal-1; i++)
	{

		detail=(CODDetail*)m_destCtrl.GetItemData(i);
		detail->setPercent(averPercent);
		strPercent.Format("%d",averPercent);
		m_destCtrl.SetItemText(i,1,strPercent);
	}
	detail=(CODDetail*)m_destCtrl.GetItemData(i);
	detail->setPercent(100-(i*averPercent));
	strPercent.Format("%d",detail->getPercent());
	m_destCtrl.SetItemText(i,1,strPercent);
}
void CDlgODAssign::OnEditOriginOD()
{
	int nCursel=m_originCtrl.GetCurSel();
	if (nCursel<0)
	{
		return;
	}
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
	{
		return;
	}
	CODDetail *detail=(CODDetail*)m_originCtrl.GetItemData(nCursel);
	LandsideLayoutObjectList *objectList=&(pDoc->getARCport()->m_pInputLandside->getObjectList());
	CDlgLandsideObjectSelect dlg(objectList,ALT_LEXT_NODE,m_pParentWnd);
	if (dlg.DoModal()==IDOK)
	{
		detail->setName(dlg.getCurbSideName());
		m_originCtrl.SetItemText(nCursel,0,dlg.getCurbSideName());
	}
}
void CDlgODAssign::OnNewOriginOD()
{
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
	{
		return;
	}
	
	LandsideLayoutObjectList *objectList= &(m_pLandSide->getObjectList());
	CDlgLandsideObjectSelect dlg(objectList,ALT_LEXT_NODE,m_pParentWnd);
	int nCount = m_originCtrl.GetItemCount();	
	if (dlg.DoModal()==IDOK)
	{
		CODFligthType *flightType=(CODFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
		CODDetail *detail=new CODDetail();
		detail->setType(ODAssignment_Origin);
		flightType->AddItem(detail);
		m_originCtrl.InsertItem(m_originCtrl.GetItemCount(),"");
		m_originCtrl.SetItemData(m_originCtrl.GetItemCount()-1,(DWORD_PTR)detail);
		
		CString strPercent = _T("");
		int i = 0;
		strPercent.Format(_T("%d"),100/(nCount + 1));
		for (i = 0; i < nCount + 1; i++)
		{
			if(i == nCount)
				strPercent.Format(_T("%d"),100-nCount*(100/(nCount + 1)));
			m_originCtrl.SetItemText(i,1,strPercent);
		}
		detail->setName(dlg.getCurbSideName());
		m_originCtrl.SetItemText(m_originCtrl.GetItemCount()-1,0,dlg.getCurbSideName());
		i = 0;
		for(int j=0;j<m_originCtrl.GetItemCount();j++)
		{
			CODDetail *itrPercent=(CODDetail*)m_originCtrl.GetItemData(j);
			if(j == nCount)
				itrPercent->setPercent(100-nCount*(100/(nCount + 1)));
			else
				itrPercent->setPercent(100/(nCount + 1));
			itrPercent->setParentID(flightType->getID());

		}
	}
}
void CDlgODAssign::OnDelOriginOD()
{
	int alTotal=0;
	int averPercent=0;
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
	{
		return;
	}
	int nCurSel=m_originCtrl.GetCurSel();
	if (nCurSel<0)
	{
		return;
	}
	CODDetail *detail=(CODDetail*)m_originCtrl.GetItemData(nCurSel);
	CODFligthType *flightType=(CODFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	for (int i=0;i<flightType->getItemCount();i++)
	{
		if (detail==flightType->getItem(i))
		{
			flightType->deleteItem(i);
			m_originCtrl.DeleteItem(nCurSel);
		}
	}
	

	alTotal=m_originCtrl.GetItemCount();
	if (alTotal<=0)
	{
		return;
	}
	averPercent=100/alTotal;
	CString strPercent="";
	int i=0;
	for (;i<alTotal-1;i++)
	{

		detail=(CODDetail*)m_originCtrl.GetItemData(i);
		detail->setPercent(averPercent);
		strPercent.Format("%d",averPercent);
		m_originCtrl.SetItemText(i,1,strPercent);
	}
	detail=(CODDetail*)m_originCtrl.GetItemData(i);
	detail->setPercent(100-(i*averPercent));
	strPercent.Format("%d",detail->getPercent());
	m_originCtrl.SetItemText(i,1,strPercent);
}
void CDlgODAssign::OnEditTimeRange()
{
	
}
void CDlgODAssign::OnNewTimeRange()
{
	
}
void CDlgODAssign::OnDelTimeRange()
{
	
}
void CDlgODAssign::OnBtnSave()
{
	int OriginAllPercent=0, DestAllPercent=0;
	BOOL bDest=FALSE,bOrigin=FALSE;
	for (int i=0;i<m_pLandSide->getODFlightTypeList().getItemCount();i++)
	{
		CODFligthType *flightype=m_pLandSide->getODFlightTypeList().getItem(i);
		DestAllPercent=0;
		OriginAllPercent=0;
		bDest=FALSE;
		bOrigin=FALSE;
		for (int j=0;j<flightype->getItemCount();j++)
		{
			
			CODDetail *detail=flightype->getItem(j);
			if (detail->getType()==ODAssignment_Origin)
			{
				OriginAllPercent+=detail->getPercent();
				bOrigin=TRUE;
			}
			else if (detail->getType()==ODAssignment_Dest)
			{
				DestAllPercent+=detail->getPercent();
				bDest=TRUE;
			}
			
		}
		if ((bOrigin &&OriginAllPercent!=100)||(bDest&&DestAllPercent!=100))
		{
			MessageBox(_T("The sum of operation percent should equal 100%"));
			return;
		}
	}
	m_pLandSide->getODFlightTypeList().SaveData();
	if (m_pLandSide->getODFlightTypeList().getItemCount()<=0)
	{
		m_pLandSide->getODFlightTypeList().ReadData();
	}
}
void CDlgODAssign::OnOK()
{
	OnBtnSave();
	CXTResizeDialog::OnOK();
}
void CDlgODAssign::OnCancel()
{
	m_pLandSide->getODFlightTypeList().ReadData();
	CXTResizeDialog::OnCancel();
}
void CDlgODAssign::OnEditFltType()
{
	if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
		return;
	if (m_hTimeRangeTreeItem==NULL)
		return;
	// HTREEITEM hItem;
	CODFligthType *tmpFlight=(CODFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	CFlightDialog dlg( m_pParentWnd );

	if( dlg.DoModal() == IDOK )
	{
		FlightConstraint fltConstr = dlg.GetFlightSelection();
		char szBuffer[1024] = {0};
		fltConstr.screenPrint(szBuffer);

		CString strBuffer = szBuffer;
		HTREEITEM hItem = m_treeCtrl.GetRootItem();
		while (hItem)
		{
			if (m_treeCtrl.GetItemText(hItem) == strBuffer)
			{
				MessageBox(_T("Flight Type: \" ") +strBuffer+ _T(" \" already exists."));
				return;
			}
			hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
		}
		fltConstr.WriteSyntaxStringWithVersion(szBuffer);
		tmpFlight->setName(szBuffer);
		m_treeCtrl.SetItemText(m_hTimeRangeTreeItem,strBuffer);
	}
}
void CDlgODAssign::OnDelFltType()
{
	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	//	 HTREEITEM hitem;
	CODFligthType *tmpFLight;
	tmpFLight=(CODFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	for(int i=0;i<m_pLandSide->getODFlightTypeList().getItemCount();i++)
	{
		if(tmpFLight==m_pLandSide->getODFlightTypeList().getItem(i))
		{
			m_pLandSide->getODFlightTypeList().deleteItem(i);
			m_treeCtrl.DeleteItem(m_hTimeRangeTreeItem);
			m_originCtrl.DeleteAllItems();
			m_destCtrl.DeleteAllItems();
			break;
		}
	}
	
}
void CDlgODAssign::OnNewFltType()

{
	CFlightDialog dlg( m_pParentWnd );

	if( dlg.DoModal() == IDOK )
	{
		FlightConstraint fltConstr = dlg.GetFlightSelection();
		char szBuffer[1024] = {0};
		fltConstr.screenPrint(szBuffer);

		CString strBuffer = szBuffer;
		HTREEITEM hItem = m_treeCtrl.GetRootItem();
		while (hItem)
		{
			if (m_treeCtrl.GetItemText(hItem) == strBuffer)
			{
				MessageBox(_T("Flight Type: \" ") +strBuffer+ _T(" \" already exists."));
				return;
			}
			hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
		}

		CODFligthType *tmpFlight=new CODFligthType();
		tmpFlight->setAirportDataBase(pDoc->GetTerminal().m_pAirportDB);
		fltConstr.WriteSyntaxStringWithVersion(szBuffer);
		tmpFlight->setName(szBuffer);
		// m_treeCtrl.SetRedraw(FALSE);
		hItem=m_treeCtrl.InsertItem(strBuffer,TVI_ROOT);
		m_treeCtrl.SetItemData(hItem,(DWORD_PTR)tmpFlight);
		m_treeCtrl.SelectItem(hItem);
		m_pLandSide->getODFlightTypeList().AddItem(tmpFlight);
		//m_treeCtrl.SetRedraw(true);
		m_treeCtrl.Expand(hItem,TVE_EXPAND);
		if (m_originCtrl.GetItemCount()>=0)
		{
			m_originCtrl.DeleteAllItems();
		}
		if (m_destCtrl.GetItemCount()>=0)
		{
			m_destCtrl.DeleteAllItems();
		}
	}	
}
// CDlgODAssign message handlers

void CDlgODAssign::OnTvnSelchangedTreeOdassignment(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	int iOrigin=0;
	int iDest=0;
	CString strPercent;
	m_originCtrl.DeleteAllItems();
	m_destCtrl.DeleteAllItems();
	if (getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
	{
		CODFligthType *flightType=(CODFligthType*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
		for (int i=0;i<flightType->getItemCount();i++)
		{
			CODDetail *detail=flightType->getItem(i);
			strPercent.Format("%d",detail->getPercent());
			if (detail->getType()==ODAssignment_Origin)
			{
				m_originCtrl.InsertItem(iOrigin,"");
				m_originCtrl.SetItemData(iOrigin,(DWORD_PTR)detail);
				m_originCtrl.SetItemText(iOrigin,0,detail->getName());
				m_originCtrl.SetItemText(iOrigin,1,strPercent);
				iOrigin++;

			} 
			else if(detail->getType()==ODAssignment_Dest)
			{
				m_destCtrl.InsertItem(iDest,"");
				m_destCtrl.SetItemData(iDest,(DWORD_PTR)detail);
				m_destCtrl.SetItemText(iDest,0,detail->getName());
				m_destCtrl.SetItemText(iDest,1,strPercent);
				iDest++;
			}

		}
	}
	UpdateTreeBar();
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
bool CDlgODAssign::IsvalidTimeRange(CString strTimeRange1,CString strTimeRange2)
{
	
		return (true);
}

void CDlgODAssign::UpdateTreeBar()
{
	BOOL bEnabled=getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,bEnabled);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,bEnabled);
}
void CDlgODAssign::UpdateOriginBar()
{
	POSITION pos=m_originCtrl.GetFirstSelectedItemPosition();
	BOOL bEnabled=TRUE;
	if (pos==NULL)
	{
		bEnabled=FALSE;
	}
	m_OriginToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,bEnabled);
	m_OriginToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,bEnabled);
}
void CDlgODAssign::UpdateDestBar()
{
	POSITION pos=m_destCtrl.GetFirstSelectedItemPosition();
	BOOL bEnabled=TRUE;
	if (pos==NULL)
	{
		bEnabled=FALSE;
	}
	m_destToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_CHANGE,bEnabled);
	m_destToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE,bEnabled);
}
void CDlgODAssign::OnLvnItemchangedListOdoriginassign(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	LV_DISPINFO *plvDispInfo=(LV_DISPINFO*)pNMHDR;
	LV_ITEM *plvItem=&plvDispInfo->item;
	int nItem=plvItem->iItem;
	int nSubItem=plvItem->iSubItem;
	if (nItem<0)
	{
		return;
	}
	UpdateOriginBar();
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgODAssign::OnLvnEndlabeleditListOdoriginassign(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iTotals=0,nCount=0;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LV_DISPINFO *plvDispInfo=(LV_DISPINFO*)pNMHDR;
	LV_ITEM *plvItem=&plvDispInfo->item;
	int nItem=plvItem->iItem;
	int nSubItem=plvItem->iSubItem;
	if (nItem<0)
	{
		return ;
	}
	if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	//CODTimeRange *timeRange=(CODTimeRange*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	for (int j=0;j<m_originCtrl.GetItemCount();j++)
	{
		CODDetail *detail=(CODDetail*)m_originCtrl.GetItemData(j);
		if(plvItem->iItem!=j)
			iTotals+=detail->getPercent();
	}
	if(iTotals+_ttoi(m_originCtrl.GetItemText(nItem,1))>100)
	{
		CString strReplace = _T("0");
		strReplace.Format(_T("%d"),100 - iTotals);
		m_originCtrl.SetItemText(nItem,1,strReplace);
		MessageBox(_T("The sum of operation percent can not exceed 100%."));
		return;
	}
	m_originCtrl.OnEndlabeledit(pNMHDR,pResult);
	OnListItemEndLabelEdit(nItem,nSubItem,ODAssignment_Origin);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgODAssign::OnLvnItemchangedListOddestassign(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	LV_DISPINFO *plvDispInfo=(LV_DISPINFO*)pNMHDR;
	LV_ITEM *plvItem=&plvDispInfo->item;
	int nItem=plvItem->iItem;
	int nSubItem=plvItem->iSubItem;
	if (nItem<0)
	{
		return;
	}
	UpdateDestBar();
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgODAssign::OnLvnEndlabeleditListOddestassign(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iTotals=0,nCount=0;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	LV_DISPINFO *plvDispInfo=(LV_DISPINFO*)pNMHDR;
	LV_ITEM *plvItem=&plvDispInfo->item;
	int nItem=plvItem->iItem;
	int nSubItem=plvItem->iSubItem;
	if (nItem<0)
	{
		return ;
	}
	if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	//CODTimeRange *timeRange=(CODTimeRange*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	for (int j=0;j<m_destCtrl.GetItemCount();j++)
	{
		CODDetail *detail=(CODDetail*)m_destCtrl.GetItemData(j);
		if(plvItem->iItem!=j)
			iTotals+=detail->getPercent();
	}
	if(iTotals+_ttoi(m_destCtrl.GetItemText(nItem,1))>100)
	{
		CString strReplace = _T("0");
		strReplace.Format(_T("%d"),100 - iTotals);
		m_destCtrl.SetItemText(nItem,1,strReplace);
		MessageBox(_T("The sum of operation percent can not exceed 100%."));
		return;
	}
	m_destCtrl.OnEndlabeledit(pNMHDR,pResult);
	OnListItemEndLabelEdit(nItem,nSubItem,ODAssignment_Dest);
	*pResult = 0;
}

void CDlgODAssign::OnListItemEndLabelEdit(int nItem, int nSubItem,enumODAssignmentType tmpType)
{
	CString strValue="";
	int nCurSel=0;
	if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	if (tmpType==ODAssignment_Origin)
	{
		 strValue = m_originCtrl.GetItemText(nItem, nSubItem);
		 nCurSel=m_originCtrl.GetCurSel();
		CODDetail *detail=(CODDetail*)m_originCtrl.GetItemData(nItem);
		detail->setPercent(atoi(strValue));
	}
	else if (tmpType==ODAssignment_Dest)
	{
		strValue = m_destCtrl.GetItemText(nItem, nSubItem);
		nCurSel=m_destCtrl.GetCurSel();
		CODDetail *detail=(CODDetail*)m_destCtrl.GetItemData(nItem);
		detail->setPercent(atoi(strValue));
	}
	
}