// DlgLandSideDistribution.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "PassengerTypeDialog.h"
#include "DlgLandSideDistribution.h"
#include "../Landside/VehicleDistribtionList.h"
#include "../Landside/VehicleDistribution.h"
#include "../landside/VehicleDistributionDetail.h"
#include "../Landside//LandSideDefine.h"
#include "../Common/WinMsg.h"
#include "TermPlanDoc.h"
// #include "FlightDialog.h"
#include "..\inputs\distlist.h"
#include "..\inputs\typelist.h"
#include "..\inputs\paxdist.h"
#include ".\dlglandsidedistribution.h"
#include "../Landside/InputLandside.h"
// DlgLandSideDistribution dialog

#pragma warning (disable:4244)
#define CONSTRAINT_LEN 128

IMPLEMENT_DYNAMIC(DlgLandSideDistribution, CXTResizeDialog)
DlgLandSideDistribution::DlgLandSideDistribution(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgLandSideDistribution::IDD, pParent)
{
}
DlgLandSideDistribution::DlgLandSideDistribution(int tmpProjID,CTermPlanDoc *tmpDoc,CWnd*pParent /*=NULL*/)
: CXTResizeDialog(DlgLandSideDistribution::IDD, pParent),m_pProjID(tmpProjID)
{
  m_treeDist.setInputTerminal(GetInputTerminal());
  m_bSized=false;
  pDoc=tmpDoc;

}
DlgLandSideDistribution::~DlgLandSideDistribution()
{
}
int DlgLandSideDistribution::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct)==-1)
		return -1;
	
	return 0;
}
BOOL DlgLandSideDistribution::OnInitDialog()
{

	CXTResizeDialog::OnInitDialog();
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP) ||
		!m_ToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
    
	if (!m_TreeToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP) ||
		!m_TreeToolBar.LoadToolBar(IDR_PAXDIST))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	/*m_stcFrame2.ShowWindow(SW_HIDE);
	m_stcFrame.ShowWindow(SW_HIDE);*/
	InitToolBar();
	ReloadPaxList();
	m_nCurPaxListSel=-1;
	m_bOnChangePax=false;
	//SetResize(IDC_STATIC_TOOLBARCONTENTER,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_STATIC_FRAME_DISTRIBTUION,SZ_TOP_LEFT,SZ_BOTTOM_LEFT);
	SetResize(m_ToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_PAX_DISTRIBUTION,SZ_TOP_LEFT,SZ_BOTTOM_LEFT);

	//SetResize(IDC_STATIC_TOOLBARCONTENTER2,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_STATIC_FRAME3_DISTRIBTUION,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_TreeToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DISTRIBUTION,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

 	GetDlgItem(IDC_STATIC_FRAME_DISTRIBTUION)->SetWindowText(_T("Passenger Type"));
	

	return true;
}
void DlgLandSideDistribution::ReloadPaxList()
{
	m_listboxPax.ResetContent();
	CVehicleDistribution *tmpPax;//--c	
	CString strPaxType;
	for (int i=0;i<pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItemCount();i++)
	{
		tmpPax=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(i);
		tmpPax->getCMobileElemConstraint().screenPrint(strPaxType.GetBuffer(1024));//--c
		m_listboxPax.AddString(strPaxType);
		m_listboxPax.SetItemData(i,i);
	}
	ReLoadTree();
}
void DlgLandSideDistribution::ReLoadTree()
{
	HTREEITEM hItem;
	HTREEITEM hVehileParentItem;
	CString str;
	CVehicleDistributionDetail *detail;
	int nCursel=m_listboxPax.GetCurSel();
	if (m_listboxPax.GetCount()<=0)
	{
		m_treeDist.DeleteAllItems();
		return;
	}
    if (nCursel>=0)
	{
		m_treeDist.DeleteAllItems();
		 CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(nCursel);
		 for(int i=0;i<distribution->getItemCount();i++)
		 {
			detail=distribution->getItem(i);
			str=getItemLabel(detail->getName(),detail->getPercent());
			if (detail->getLevel()==0 )
			{  
				if (detail->getVehicleType()==Distribution_Vehicle)
				{
					hVehileParentItem=m_treeDist.InsertItem(str,TVI_ROOT);
					m_treeDist.SetItemData(hVehileParentItem,i);
					m_treeDist.SetItemData(hVehileParentItem,i);
				}
				else 
				{
					hItem=m_treeDist.InsertItem(str,TVI_ROOT);
					m_treeDist.SetItemData(hItem,i);
				}
				
				m_treeDist.Expand( hItem, TVE_EXPAND );
			}
			
		
		 }
		 for (int j=0;j<distribution->getItemCount();j++)
		 {
			 detail=distribution->getItem(j);
			 str=getItemLabel(detail->getName(),detail->getPercent());
			 if (detail->getLevel()==1&& j==0)
			 {
				hItem=m_treeDist.InsertItem(str,hVehileParentItem);
				m_treeDist.SetItemData(hItem,j);
			 }
			 else if(detail->getLevel()==1)
			 {
				 hItem=m_treeDist.InsertItem(str,hVehileParentItem,hItem);
				 m_treeDist.SetItemData(hItem,j);
			 }
			 m_treeDist.Expand(hItem,TVE_EXPAND);

		 }
		 m_treeDist.Expand(hVehileParentItem,TVE_EXPAND);
	}
}
void DlgLandSideDistribution::OnSelChangeListPax()
{
	if (m_bOnChangePax)
	{
		m_listboxPax.SetCurSel(m_nCurPaxListSel);
		return;
	}
	int nFltIdx = m_listboxPax.GetCurSel();
	m_nCurPaxListSel = m_listboxPax.GetCurSel();
	if( nFltIdx == LB_ERR )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,FALSE );
		m_TreeToolBar.GetToolBarCtrl().EnableButton(ID_PAXDISTBUTTON, FALSE);
	}
	//	m_btnDel.EnableWindow( false );
	else if(0==nFltIdx)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,FALSE );
		m_TreeToolBar.GetToolBarCtrl().EnableButton(ID_PAXDISTBUTTON, TRUE);

	}else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE );
		m_TreeToolBar.GetToolBarCtrl().EnableButton(ID_PAXDISTBUTTON, TRUE);
	}
	
	ReLoadTree();
}
void DlgLandSideDistribution::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	m_hRClickItem=NULL;
	CRect rectTree;
	m_treeDist.GetWindowRect(rectTree);
	if(!rectTree.PtInRect(pos))
		return;
	m_treeDist.SetFocus();
	CPoint pt=pos;
	m_treeDist.ScreenToClient(&pt);
	UINT iRet;
	m_hRClickItem=m_treeDist.HitTest(pt,&iRet);
	if(iRet!=TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}
	CMenu memuPop;
	memuPop.LoadMenu(IDR_MENU_POPUP);
	CMenu* pMenu = memuPop.GetSubMenu( 1 );
	if( m_hRClickItem )
	{
		m_treeDist.SelectItem( m_hRClickItem );   // Select it
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);	
	}
	else
	{
		m_treeDist.SelectItem( NULL );   // Select it
	}	
}
void DlgLandSideDistribution::OnVehicleDistributionEditSpin()
{
	if(!m_hRClickItem)
		return;
	m_treeDist.SpinEditLabel(m_hRClickItem);
	m_btnSave.EnableWindow(TRUE);
}
CString  DlgLandSideDistribution::getItemLabel(const CString& _csName,int _nPercent)
{
	CString str;
	str.Format( "%s (%d%%)", _csName, _nPercent );
	return str;
}
void DlgLandSideDistribution::OnVehicledistEvenRemainPercent()
{
  if (!m_hRClickItem)
	  return ;
  int fValue=0;
  HTREEITEM hParentItem=m_treeDist.GetParentItem(m_hRClickItem);
  HTREEITEM hChildItem;
  int childCount=0;
  if (hParentItem==NULL)
      
  {
	 childCount=2;
  }
  else 
  {
	  childCount=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(m_listboxPax.GetCurSel())->getItemCount()-2;
  }
  int nCurSel=m_listboxPax.GetCurSel();
  CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(nCurSel);
  for (int j=0;j<childCount;j++)
  {
	  if( j == 0 )
		  hChildItem = m_treeDist.GetChildItem( hParentItem );
	  else
		  hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
	  if (hChildItem==m_hRClickItem)
	  {
		  fValue=distribution->getItem(m_treeDist.GetItemData(m_hRClickItem))->getPercent();
		  break;
	  }

  }
  float fRemain = 100.00 - fValue;
  float fPercent, fLack = 0;
  if((int)fRemain % (childCount - 1) == 0 )
	  fPercent = fRemain / (childCount - 1);
  else
  {
	  fPercent = static_cast<float>((fRemain / (childCount - 1)));
	  fLack = fRemain - fPercent * (childCount - 1);
  }


  for( int j=0; j<childCount; j++ )
  {
	  if( j == 0 )
		  hChildItem = m_treeDist.GetChildItem( hParentItem );
	  else
		  hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );

	  if(hChildItem == m_hRClickItem)			//not set percent for the current selected item
		  continue;

	  float fPercentUse = fPercent;
	  if(fLack != 0)
	  {
		  fLack--;
		  fPercentUse = fPercent + 1;

	  }

	  CString csLabel = getItemLabel(distribution->getItem(m_treeDist.GetItemData(hChildItem))->getName(),static_cast<int>(fPercentUse));
	  m_treeDist.SetItemText( hChildItem, csLabel );
	  distribution->getItem(m_treeDist.GetItemData(hChildItem))->setPercent(static_cast<int>(fPercentUse));
  }

  m_btnSave.EnableWindow();	
   
}
void DlgLandSideDistribution::OnVehicledistEvenpercent()
{
	if (!m_hRClickItem)
		return;
	int fValue=0;
	HTREEITEM hParentItem=m_treeDist.GetParentItem(m_hRClickItem);
	HTREEITEM hChildItem;
	int childCount=0;
	if (hParentItem==NULL)

	{
		childCount=2;
	}
	else 
	{
		childCount=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(m_listboxPax.GetCurSel())->getItemCount()-2;
	}
	int iPercent=0;

	int nCurSel=m_listboxPax.GetCurSel();
	CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(nCurSel);
	CVehicleDistributionDetail *detail;
	fValue=100/childCount;
	for (int i=0;i<childCount;i++)
	{
		if (i==0)
		{
		   hChildItem=m_treeDist.GetChildItem(hParentItem);
		}
		else 
			
		{
			hChildItem=m_treeDist.GetNextItem(hChildItem,TVGN_NEXT);
		}
		if (i==childCount-1)
		{
			detail=distribution->getItem(m_treeDist.GetItemData(hChildItem));
			iPercent=100-iPercent;
			m_treeDist.SetItemText(hChildItem,getItemLabel(detail->getName(),iPercent));
			detail->setPercent(iPercent);
			break;
		}
		detail=distribution->getItem(m_treeDist.GetItemData(hChildItem));
		/*if (hChildItem==m_hRClickItem)
			continue;*/
		detail->setPercent(fValue);
		m_treeDist.SetItemText(hChildItem,getItemLabel(detail->getName(),fValue));
		iPercent+=detail->getPercent();
		
			
	}
	m_btnSave.EnableWindow();	
	

}
void DlgLandSideDistribution::InitToolBar()
{
	CRect rect;
	m_listboxPax.GetWindowRect(&rect);
	ScreenToClient(rect);
	rect.top=rect.top-26;
	rect.left=rect.left+2;
	rect.bottom=rect.top+22;
	m_ToolBar.MoveWindow(rect);
	
	m_treeDist.GetWindowRect(&rect);
	ScreenToClient(rect);
	rect.top=rect.top-26;
	rect.left=rect.left+4;
	rect.bottom=rect.top+22;
	m_TreeToolBar.MoveWindow(rect);

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,FALSE );
	m_TreeToolBar.GetToolBarCtrl().EnableButton(ID_PAXDISTBUTTON, FALSE);
}
void DlgLandSideDistribution::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DISTRIBUTION, m_treeDist);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDOK, m_bntOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_LIST_PAX_DISTRIBUTION, m_listboxPax);
	/*DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER2_DISTRIBUTION, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER_DISTRIBUTION, m_toolbarcontenter);*/
	DDX_Control(pDX, IDC_STATIC_FRAME3_DISTRIBTUION, m_stcFrame2);
	DDX_Control(pDX, IDC_STATIC_FRAME_DISTRIBTUION, m_stcFrame);
}
void DlgLandSideDistribution::SetPercent(int _nPercent)
{
	if( !m_hRClickItem )
		return;
	int nCurSel=m_listboxPax.GetCurSel();
	CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(nCurSel);
	CVehicleDistributionDetail *detail=distribution->getItem(m_treeDist.GetItemData(m_hRClickItem));
	CheckPercentFull( m_treeDist.GetParentItem(m_hRClickItem) );
	detail->setPercent(_nPercent);
	m_treeDist.SetItemText(m_hRClickItem,getItemLabel(detail->getName(),_nPercent));
	//ReLoadTree();
}
BOOL DlgLandSideDistribution::CheckPercentFull( HTREEITEM _hItem )
{
	

	int nTotalPercent = 0;
	int nCurSel=m_listboxPax.GetCurSel();
	HTREEITEM hChildItem = m_treeDist.GetChildItem( _hItem ? _hItem : TVI_ROOT );
	CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(nCurSel);
	CVehicleDistributionDetail *detail;
	while( hChildItem )
	{
		detail=distribution->getItem(m_treeDist.GetItemData(hChildItem));
	
		nTotalPercent += detail->getPercent();
		hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
		
	}
	if (nTotalPercent>100)
		return false;
	return true;
	
}
LRESULT DlgLandSideDistribution::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INPLACE_SPIN)
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;

		SetPercent( pst->iPercent );
		return TRUE;
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
void DlgLandSideDistribution::OnButtonSave()
{
	//if(CheckPercentFull(NULL) && CheckPercentFull())
	int zeroPercent;
	int onePercent;
	CVehicleDistribution *distribution;
	CVehicleDistributionDetail *detail;
	for(int i=0;i<pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItemCount();i++)
	{
		zeroPercent=0;
		onePercent=0;
		distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(i);
		for (int j=0;j<distribution->getItemCount();j++)
		{
			detail=distribution->getItem(j);
			if(detail->getLevel()==0)
			{
				zeroPercent+=detail->getPercent();
			}
			else 
			{
				onePercent+=detail->getPercent();
			}
		}
		if (zeroPercent!=100 ||onePercent!=100)
		{
			MessageBox( "All branches must total 100%", "Error", MB_ICONWARNING );
			pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().readData(m_pProjID);
			return;
		}
		
	}
	
	pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().SaveData();
	if (pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItemCount()<=0)
	{
		pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().readData(m_pProjID);
	}

	
	
}
CString DlgLandSideDistribution::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

void DlgLandSideDistribution::EvenRemainPercentCurrentLevel()//ID=2 even remaing percent all branch
{
	if (!m_hRClickItem )
		return;

	// SET DATABASE
	
	HTREEITEM hParentItem=m_treeDist.GetParentItem(m_hRClickItem);
	HTREEITEM hChildItem;
	int childCount=0;
	int i =0;
	if (hParentItem==NULL)

	{
		childCount=2;
	}
	else 
	{
		childCount=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(m_listboxPax.GetCurSel())->getItemCount()-2;
	}
	
	int nCurSel=m_listboxPax.GetCurSel();
	CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(nCurSel);
	CVehicleDistributionDetail *detail=distribution->getItem(m_treeDist.GetItemData(m_hRClickItem));

	
	if( childCount==1)return;

	
	
	int nUsedPercent = detail->getPercent();
	int nRemainEvenPercent=(100 - nUsedPercent)/(childCount -1);

	//get the sequence of selected item.
	int nSelectedItem=0;
	hChildItem=m_treeDist.GetChildItem(hParentItem);
	while(hChildItem!=NULL)
	{
		if(hChildItem==m_hRClickItem)continue;
		hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
		
		nSelectedItem++;
	}
    int  averagePercent=nRemainEvenPercent/nSelectedItem;
	int lastPercent=nRemainEvenPercent-averagePercent*nSelectedItem;
	hChildItem=m_treeDist.GetChildItem(hParentItem);
	while(hChildItem!=NULL)
	{
		if(hChildItem==m_hRClickItem)continue;
		hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
        detail=distribution->getItem(m_treeDist.GetItemData(hChildItem));
		detail->setPercent(averagePercent);
		m_treeDist.SetItemText(hChildItem,getItemLabel(detail->getName(),detail->getPercent()));
		if(i==nSelectedItem-1)
			break;
	}
	hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
	detail=distribution->getItem(m_treeDist.GetItemData(hChildItem));
	detail->setPercent(lastPercent);
	m_treeDist.SetItemText(hChildItem,getItemLabel(detail->getName(),detail->getPercent()));
  

	//get all brothers item in the same level .
	
	m_btnSave.EnableWindow(TRUE);
}
void DlgLandSideDistribution::EvenPercentCurrentLevel()//ID=1; even remaining percent
{
	if (!m_hRClickItem )
		return;

	// SET DATABASE

	HTREEITEM hParentItem=m_treeDist.GetParentItem(m_hRClickItem);
	HTREEITEM hChildItem;
	int childCount=0;
	int i=0;
	if (hParentItem==NULL)

	{
		childCount=2;
	}
	else 
	{
		childCount=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(m_listboxPax.GetCurSel())->getItemCount()-2;
	}

	int nCurSel=m_listboxPax.GetCurSel();
	CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(nCurSel);
	CVehicleDistributionDetail *detail=distribution->getItem(m_treeDist.GetItemData(m_hRClickItem));


	if( childCount==1)return;
	float nUsedPercent = static_cast<float>(detail->getPercent());
	float nRemainEvenPercent=(100.00 - nUsedPercent)/(childCount -1);
    CString str;
	CString istr;


	//get the sequence of selected item.
	int nSelectedItem=0;
	hChildItem=m_treeDist.GetChildItem(hParentItem);
	while(hChildItem!=NULL)
	{
		if(hChildItem==m_hRClickItem)continue;
		hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);

		nSelectedItem++;
	}
	int   iavergePercent=(int)(nRemainEvenPercent/nSelectedItem);
	float  averagePercent=nRemainEvenPercent/nSelectedItem;
	str.Format("%.2f",averagePercent);
    istr.Format("%.2f",iavergePercent);
	if (istr.Compare(str)<0)
	{
		iavergePercent=iavergePercent+1;
	}
	//int lastPercent=nRemainEvenPercent-averagePercent*nSelectedItem;
	hChildItem=m_treeDist.GetChildItem(hParentItem);
	while(hChildItem!=NULL)
	{
		if(hChildItem==m_hRClickItem)continue;
		hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
		detail=distribution->getItem(m_treeDist.GetItemData(hChildItem));
		detail->setPercent(iavergePercent);
		m_treeDist.SetItemText(hChildItem,getItemLabel(detail->getName(),detail->getPercent()));
		
	}
	/*hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
	detail=distribution->getItem(m_treeDist.GetItemData(hChildItem));
	detail->setPercent(lastPercent);
	m_treeDist.SetItemText(hChildItem,getItemLabel(detail->getName(),detail->getPercent()));*/


	//get all brothers item in the same level .

	m_btnSave.EnableWindow(TRUE);
}

void DlgLandSideDistribution::ModifyPercentAllBranch(int _nNewPercent)
{
	
    CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(m_listboxPax.GetCurSel());
	HTREEITEM hParentItem=m_treeDist.GetParentItem(m_hRClickItem);
	HTREEITEM hChildItem;
	int childCount=0;
	int i=0;
	if (hParentItem==NULL)

	{
		childCount=2;
	}
	else 
	{
		childCount=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(m_listboxPax.GetCurSel())->getItemCount()-2;
	}
	CVehicleDistributionDetail *detail;
	CString str;
	detail=distribution->getItem(m_treeDist.GetItemData(m_hRClickItem));
	//int curpercent=detail->getPercent();
	for (i=0;i<childCount;i++)
	{
		if (i==0)
		{
			hChildItem=m_treeDist.GetChildItem(hParentItem);
		}
		else 
		{
			hChildItem=m_treeDist.GetNextItem(hChildItem,TVGN_NEXT);
		}
		detail=distribution->getItem(m_treeDist.GetItemData(hChildItem));
    
		str=getItemLabel(detail->getName(),_nNewPercent);
		m_treeDist.SetItemText(hChildItem,str);
		detail->setPercent(_nNewPercent);
	}
	m_btnSave.EnableWindow();	
}

LRESULT DlgLandSideDistribution::OnEndEditPercent( WPARAM p_wParam, LPARAM p_lParam )
{
	SPINTEXT* pStData=(SPINTEXT*)p_lParam;
	switch(m_nAllBranchesFlag)
	{
	case 0:
		ModifyPercentAllBranch(pStData->iPercent);
		break;
	default:
		{
		   CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(m_listboxPax.GetCurSel());
		   CVehicleDistributionDetail *detail=distribution->getItem(m_treeDist.GetItemData(m_hRClickItem));
			//get the sequence of selected item.
			int nSelectedItem=0;
			/*HTREEITEM hParentItem=m_treeDist.GetParentItem(m_hRClickItem);
			HTREEITEM hChildItem=m_treeDist.GetChildItem(hParentItem);*/
		/*	while(hChildItem!=NULL)
			{
				if(hChildItem==m_hRClickItem)break;
				hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
				nSelectedItem++;
			}*/
			detail->setPercent(static_cast<int>(pStData->iPercent));
			CString csLabel = getItemLabel(detail->getName(),detail->getPercent());
			m_treeDist.SetItemText( m_hRClickItem, csLabel );
		}
		break;
	}
	//reset m_nAllBranchesFlag .
	m_nAllBranchesFlag=-1;
	return 0L;
}
void DlgLandSideDistribution::OnAllBranchesPercentEdit(UINT nID)
{
	m_nAllBranchesFlag = nID-ID_PAXDIST_MODIFYPERCENT_ALLBRANCHES;
	switch(m_nAllBranchesFlag)
	{
	case 0:
		//automaticly update data by OnEndEditPercent(...) in this case.
		if (!m_hRClickItem )
			return;
		m_treeDist.SpinEditLabel(m_hRClickItem);
		m_btnSave.EnableWindow();	

		break;
	case 1:
		//even all branches' percent.
		//EvenPercentCurrentLevel();
		OnVehicledistEvenpercent();
		break;
	case 2:
		//even remain all branches' percent.
		//EvenRemainPercentCurrentLevel();
		OnVehicledistEvenRemainPercent();
		break;
	default:
		break;
	}
}

InputTerminal* DlgLandSideDistribution::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (InputTerminal*)&pDoc->GetTerminal();
}

void DlgLandSideDistribution::OnPeoplemoverNew()
{
	CPassengerTypeDialog dlg( m_pParentWnd ,FALSE, TRUE );
	
	if( dlg.DoModal() == IDOK )
	{
		CString str;
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();

		int nCount =  m_listboxPax.GetCount();
		char szBuffer[1024] = {0};
		mobileConstr.screenPrint(szBuffer);

		CString strBuffer=szBuffer;

		for( int i=0; i<nCount; i++ )
		{
			CString strText;
			m_listboxPax.GetText(i,strText);

			if(strText.Trim().Compare(strBuffer.Trim())==0)
				/*if( fltConstr.isEqual(pConst) )*/
			{
				// select the item
				m_listboxPax.SetCurSel( i );
				// reload tree
				OnSelChangeListPax();
				return;
			}
		}
		if( i == nCount )
		{
			// add into database.

			m_listboxPax.AddString(strBuffer);
			m_listboxPax.SetItemData(pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItemCount(),pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItemCount());
			
			CVehicleDistribution *distribution=new CVehicleDistribution();
			distribution->setInputTerminal(GetInputTerminal());
			distribution->readData(m_pProjID);
			pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().AddItem(distribution);
			distribution->setAirportDataBase(pDoc->GetTerminal().m_pAirportDB);
		    mobileConstr.WriteSyntaxStringWithVersion(szBuffer);
			distribution->setName(szBuffer);
			m_listboxPax.SetCurSel(m_listboxPax.GetCount()-1);
// 			distribution->SaveData();
			OnSelChangeListPax();
			m_btnSave.EnableWindow( TRUE );
		}

		
		

		
	}	
}

void DlgLandSideDistribution::OnPeoplemoverDelete()
{
	int nFltIdx = m_listboxPax.GetCurSel();
	if( nFltIdx == -1 )
		return;
    // GetInputTerminal()->paxDistList->deleteItem( nFltIdx );
	pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().deleteItem(nFltIdx);
   
	ReloadPaxList();
   
	m_btnSave.EnableWindow( TRUE );
}




void DlgLandSideDistribution::OnPeoplemoverChange()
{
	m_bOnChangePax=true;

	if (m_nCurPaxListSel==LB_ERR)
	{
		return;
	}
	CPassengerTypeDialog dlg( m_pParentWnd ,FALSE, TRUE);

	if( dlg.DoModal() == IDOK )
	{
		CString str;
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();

		int nCount =  m_listboxPax.GetCount();
		char szBuffer[1024] = {0};
		mobileConstr.screenPrint(szBuffer);

		CString strBuffer=szBuffer;
		m_listboxPax.DeleteString(m_nCurPaxListSel);
		m_listboxPax.InsertString(m_nCurPaxListSel,szBuffer);
 		CVehicleDistribution *distribution=pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().getItem(m_nCurPaxListSel);
		mobileConstr.WriteSyntaxStringWithVersion(szBuffer);
		distribution->setName(szBuffer);
		OnSelChangeListPax();
		m_btnSave.EnableWindow( TRUE );

	}
	m_bOnChangePax=false;
	
}



void DlgLandSideDistribution::OnPrintDist()
{
	m_treeDist.PrintTree("Tree header","Tree footer");
}
void DlgLandSideDistribution::OnVehicledistModifypercent()
{

	if (!m_hRClickItem )
		return;

	// Show edit box
	m_treeDist.SpinEditLabel(m_hRClickItem);
	m_btnSave.EnableWindow();	
}
void DlgLandSideDistribution::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType, cx, cy);

	/*if(!m_bSized)
	{
		m_bSized = true;
		return;
	}*/


	// TODO: Add your message handler code here
	/*CRect  newrc,rc;

	newrc.left = 233;
	newrc.top = 55;
	newrc.bottom = cy-40;
	newrc.right =  cx - 7;
	m_treeDist.MoveWindow( &newrc );

	m_treeDist.GetWindowRect(&newrc);
	ScreenToClient(&newrc);

	newrc.left += 100;
	newrc.top = newrc.bottom + 10;
	newrc.bottom = newrc.top + 25;
	newrc.right = newrc.left + 80;
	m_btnSave.MoveWindow(&newrc);

	newrc.left = newrc.right + 15;
	newrc.right = newrc.left + 80;
	m_bntOk.MoveWindow(&newrc);

	newrc.left = newrc.right + 15;
	newrc.right = newrc.left + 80;
	m_btnCancel.MoveWindow(&newrc);

	m_treeDist.GetWindowRect(&newrc);
	ScreenToClient(&newrc);
	m_stcFrame.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.bottom = newrc.bottom + 3;
	m_stcFrame.MoveWindow(&rc);

	m_stcFrame2.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.bottom = newrc.bottom + 3;
	rc.right = newrc.right + 1;
	rc.left = newrc.left - 1;
	m_stcFrame2.MoveWindow(&rc);

	m_listboxFlight.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.bottom = newrc.bottom;
	m_listboxFlight.MoveWindow(&rc);*/


}
void DlgLandSideDistribution::OnCancelMode() 
{
	CDialog::OnCancelMode();

	// TODO: Add your message handler code here
}
void DlgLandSideDistribution::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (m_bOnChangePax)
	{
		return;
	}
	pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().readData(m_pProjID);
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnCancel();
		return;
	}

	AfxGetApp()->BeginWaitCursor();
	try
	{
		//GetInputTerminal()->paxDistList->loadDataSet( GetProjPath() );	
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	AfxGetApp()->EndWaitCursor();
	CDialog::OnCancel();
}
void DlgLandSideDistribution::OnOK() 
{
	// TODO: Add extra validation here
	if (m_bOnChangePax)
	{
		return;
	}
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}

	//	PaxTypeDistribution* pPaxDist = GetPaxDist();
	//	if( !pPaxDist->checkTotals())
	//	{
	//		MessageBox( "All branches must total 100%", "Error", MB_ICONWARNING);
	//		return;
	//	}
	//	

	AfxGetApp()->BeginWaitCursor();
 	OnButtonSave();		
	//---------------©ии╬х╔---------------------
// 	pDoc->getARCport()->m_pInputLandside->getVehicleDistributionList().readData(m_pProjID);	
	//------------------------------------------
	AfxGetApp()->EndWaitCursor();

	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(DlgLandSideDistribution, CXTResizeDialog)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_PAX_DISTRIBUTION,OnSelChangeListPax)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_COMMAND_RANGE(ID_PAXDIST_MODIFYPERCENT_ALLBRANCHES,ID_PAXDIST_EVENPERCENT_REMAIN_ALLBRANCHES,OnAllBranchesPercentEdit)
	ON_MESSAGE(WM_INPLACE_SPIN,OnEndEditPercent)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
  	ON_COMMAND(ID_PEOPLEMOVER_CHANGE, OnPeoplemoverChange)
	ON_BN_CLICKED(ID_PAXDISTBUTTON, OnPrintDist)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PAXDIST_MODIFYPERCENT, OnVehicledistModifypercent)
	ON_COMMAND(ID_PAXDIST_EVENPERCENT, OnVehicledistEvenpercent)
	ON_COMMAND(ID_PAXDIST_EVENPERCENT_REMAIN, OnVehicledistEvenRemainPercent)
	ON_WM_CANCELMODE()
	ON_WM_SIZE()
	//ON_BN_CLICKED(IDSAVE,OnButtonSave)
	//ON_BN_CLICKED(IDCANCEL)
	//ON_BN_CLICKED(IDC_STATIC_FRAME_DISTRIBTUION, OnBnClickedStaticFrameDistribtuion)
END_MESSAGE_MAP()


// DlgLandSideDistribution message handlers

