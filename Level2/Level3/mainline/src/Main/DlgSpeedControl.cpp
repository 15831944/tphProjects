// DlgSpeedControl.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSpeedControl.h"
#include "Landside/InputLandside.h"
#include "Common/UnitPiece.h"
#include "landside/SpeedControlData.h"
#include "common/ProbabilityDistribution.h"
#include "..\common\probdistmanager.h"
#include "DlgProbDist.h"
#include ".\SelectALTObjectDialog.h"
#include "DlgCurbsideSelect.h"
#include "TermPlanDoc.h"
#include "../Common/WinMsg.h"
#include "DlgSelectLandsideObject.h"

// CDlgSpeedControl dialog


IMPLEMENT_DYNAMIC(CDlgSpeedControl, CXTResizeDialog)
CDlgSpeedControl::CDlgSpeedControl(InputLandside* pLandsideInput,/*int nProjectID,*/CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgSpeedControl::IDD, pParent)
{
	m_pSpeedCtrlData= new StretchSpeedControlDataList();
	m_pSpeedCtrlData->ReadData(-1);
	m_pLandsideInput= pLandsideInput;
}

BEGIN_MESSAGE_MAP(CDlgSpeedControl, CXTResizeDialog)
	ON_BN_CLICKED(IDC_OK,OnOk)
	ON_BN_CLICKED(IDC_CANCEL,OnCancel)
	ON_BN_CLICKED(IDC_SAVE,OnSave)
	ON_COMMAND(ID_PRODUCT_ADD,OnAddStretch)
	ON_COMMAND(ID_PRODUCT_DEL,OnDelStretch)
	ON_COMMAND(ID_PRODUCT_EDIT,OnEditStretch)
	ON_WM_CREATE()
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnDbClickListCtrl)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONSTRA, OnLvnItemchangedListSpeedControl)
 	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_CONSTRA, OnLvnItemEditdListOperateDistribution)
END_MESSAGE_MAP()

CDlgSpeedControl::~CDlgSpeedControl()
{
	delete m_pSpeedCtrlData;
}

void CDlgSpeedControl::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONSTRA, m_wndListCtrl);
}

// CDlgSpeedControl message handlers
BOOL CDlgSpeedControl::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	CUnitPiece::InitializeUnitPiece(m_nProjectID,UP_ID_1000,this);

	CRect rectItem;
	m_wndListCtrl.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.bottom+10;
	m_wndListCtrl.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Velocity_InUse);

	InitListCtrl();

	SetResize(IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_OK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_CANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_wndSpeedToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_LIST_CONSTRA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	UpdateToolBarState();
	return TRUE;
}

int CDlgSpeedControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(m_wndSpeedToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_wndSpeedToolBar.LoadToolBar(IDR_ADDDELEDIT_PRODUCT))
	{
		return -1;
	}

	m_wndSpeedToolBar.GetToolBarCtrl().SetCmdID(0, ID_PRODUCT_ADD);
	m_wndSpeedToolBar.GetToolBarCtrl().SetCmdID(1, ID_PRODUCT_DEL);
	m_wndSpeedToolBar.GetToolBarCtrl().SetCmdID(2, ID_PRODUCT_EDIT);

	return 0;
}

void CDlgSpeedControl::InitToolBar()
{
	CRect rc;
	m_wndListCtrl.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.left=rc.left+2;
	rc.top=rc.top-24;
	rc.bottom=rc.top+23;
	m_wndSpeedToolBar.MoveWindow(&rc);
	m_wndSpeedToolBar.ShowWindow(SW_SHOW);
	m_wndSpeedToolBar.BringWindowToTop();
	 
	m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD);
	m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL, FALSE);
	m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT, FALSE);
}

void CDlgSpeedControl::OnSave()
{
	m_pSpeedCtrlData->SaveData(-1);
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CDlgSpeedControl::OnOk()
{
	OnSave();
	CXTResizeDialog::OnOK();
}

void CDlgSpeedControl::OnCancel()
{
	CXTResizeDialog::OnCancel();
}

void CDlgSpeedControl::OnAddStretch()
{
	m_wndListCtrl.SetFocus();
	if (m_pLandsideInput == NULL)
		return;
	int nCount = m_wndListCtrl.GetItemCount();
	LandsideFacilityLayoutObjectList* pLandsideObjectList = &m_pLandsideInput->getObjectList();
	//CDlgLandsideObjectSelect dlg(pLandsideObjectList,ALT_LSTRETCH,m_pParentWnd);
	CDlgSelectLandsideObject dlg(pLandsideObjectList,this);
	for(int i = ALT_LTYPE_START; i < ALT_LTYPE_END;i++)
	{
		dlg.AddObjType((ALTOBJECT_TYPE)i);
	}
	if (dlg.DoModal() == IDOK)
	{

		ALTObjectID altObj = dlg.getSelectObject();//there will be a "All" item returen from dialog
		CString strSelectedObject = altObj.GetIDString();
		strSelectedObject.Trim();


		if(strSelectedObject.CompareNoCase(_T("ALL")) == 0)//default value
		{
			//pStretchSpeedControlData->SetStretchName(strSelectedObject);//the name is empty if default
			strSelectedObject = _T("");
		}
		ALTObjectID altTest;
		altTest.FromString(strSelectedObject);

		if(getData().FindStretch(altTest, CString()))
		{
			CString strMessage;
			strMessage.Format(_T("The selected object - %s - has been existing in the list."), dlg.getSelectObject().GetIDString());
			MessageBox(strMessage, _T("ARCPort Warning"), MB_OK);
			return ;
		}

		StretchSpeedControlData* pStretchSpeedControlData = new StretchSpeedControlData();
		pStretchSpeedControlData->SetStretchName(strSelectedObject);
		getData().AddNewItem(pStretchSpeedControlData);

		m_wndListCtrl.InsertItem(nCount,dlg.getSelectObject().GetIDString());

		CString strPostSpeed;
		double dSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),pStretchSpeedControlData->GetStretchSpeed());
		strPostSpeed.Format(_T("%.2f"),dSpeed);
		m_wndListCtrl.SetItemText(nCount,1,strPostSpeed);

		m_wndListCtrl.SetItemText(nCount,2,pStretchSpeedControlData->GetStretchProDistribution().getDistName());

		m_wndListCtrl.SetItemState(nCount, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_wndListCtrl.SetItemData(nCount,(DWORD_PTR)pStretchSpeedControlData);

		UpdateToolBarState();
	}
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgSpeedControl::OnDelStretch()
{
	int m_ncurSel = (int)m_wndListCtrl.GetCurSel();
	if ( m_ncurSel < 0)
		return;
	getData().DeleteItem(m_ncurSel);
	m_wndListCtrl.DeleteItem(m_ncurSel);
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgSpeedControl::OnEditStretch()
{
	int cursel =m_wndListCtrl.GetCurSel();
	if ( cursel < 0)
		return;
	StretchSpeedControlData* pStretchDistributionChangeData = (StretchSpeedControlData*)m_wndListCtrl.GetItemData(cursel);
	LandsideFacilityLayoutObjectList* pLandsideObjectList = &m_pLandsideInput->getObjectList();
	CDlgSelectLandsideObject dlg(pLandsideObjectList,this);
	for(int i = ALT_LTYPE_START; i < ALT_LTYPE_END;i++)
	{
		dlg.AddObjType((ALTOBJECT_TYPE)i);
	}
	if (dlg.DoModal() == IDOK)
	{
		ALTObjectID altObj = dlg.getSelectObject();//there will be a "All" item return from dialog
		CString strSelectedObject = altObj.GetIDString();
		strSelectedObject.Trim();

		if(strSelectedObject.CompareNoCase(_T("ALL")) == 0)//default value
		{
			//pStretchSpeedControlData->SetStretchName(strSelectedObject);//the name is empty if default
			strSelectedObject = _T("");
		}
		ALTObjectID altTest;
		altTest.FromString(strSelectedObject);

		if(getData().FindStretch(altTest, CString()))
		{
			CString strMessage;
			strMessage.Format(_T("The selected object - %s - has been existing in the list."), dlg.getSelectObject().GetIDString());
			MessageBox(strMessage, _T("ARCPort Warning"), MB_OK);
			return ;
		}

		pStretchDistributionChangeData->SetStretchName(strSelectedObject);
		m_wndListCtrl.SetItemText(cursel,0,dlg.getSelectObject().GetIDString());
	}
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgSpeedControl::OnLvnItemEditdListOperateDistribution(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	*pResult = 0;
	StretchSpeedControlData* pStretchDistributionChangeData = (StretchSpeedControlData*)m_wndListCtrl.GetItemData(dispinfo->item.iItem);
	if(!pStretchDistributionChangeData)
		return;

	CString strSel;
	strSel = dispinfo->item.pszText;
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pLandsideInput->GetAirPortDataBase()->getProbDistMan();
	if (dispinfo->item.iSubItem == 1)
	{
		CString strSpeed;
		double f = atof(strSel);
		strSpeed.Format("%.2f",f);
		double dMaxSpeed = atof(strSpeed);
		pStretchDistributionChangeData->SetStretchSpeed(dMaxSpeed);
		dMaxSpeed = CARCVelocityUnit::ConvertVelocity(CUnitPiece::GetCurSelVelocityUnit(),DEFAULT_DATABASE_VELOCITY_UNIT,pStretchDistributionChangeData->GetStretchSpeed());
		pStretchDistributionChangeData->SetStretchSpeed(dMaxSpeed);

		double dSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),pStretchDistributionChangeData->GetStretchSpeed());
		strSpeed.Format("%.2f",dSpeed);
		m_wndListCtrl.SetItemText(dispinfo->item.iItem,1,strSpeed);
	}

	if (dispinfo->item.iSubItem == 2)
	{
		if (strSel == _T("New Operating Distribution..."))
		{
			CProbDistEntry* pPDEntry = NULL;
			CDlgProbDist dlg(m_pLandsideInput->GetAirPortDataBase(), false);
			if(dlg.DoModal() == IDOK)
			{
				pPDEntry = dlg.GetSelectedPD();
				if(pPDEntry == NULL)
				{
					return;
				}
				pStretchDistributionChangeData->SetStretchProDistribution(pPDEntry);
				m_wndListCtrl.SetItemText(dispinfo->item.iItem,dispinfo->item.iSubItem,pPDEntry->m_csName);
			}
		}
		else
		{
			CProbDistEntry* pPDEntry = NULL;

			int nCount = pProbDistMan->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = pProbDistMan->getItem( i );
				if(pPDEntry->m_csName == strSel)
					break;
			}
			pStretchDistributionChangeData->SetStretchProDistribution(pPDEntry);
			m_wndListCtrl.SetItemText(dispinfo->item.iItem,dispinfo->item.iSubItem,pPDEntry->m_csName);
		}

	}	
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

bool CDlgSpeedControl::ConvertUnitFromDBtoGUI( void )
{
	return (false);
}

bool CDlgSpeedControl::RefreshGUI( void )
{
	InitListCtrl();
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	return (true);
}

bool CDlgSpeedControl::ConvertUnitFromGUItoDB( void )
{
	return (false);
}

LRESULT CDlgSpeedControl::OnDbClickListCtrl( WPARAM wParam, LPARAM lParam )
{
	int nCurSel = (int)wParam;
	if (nCurSel == LB_ERR)
		return FALSE;

	StretchSpeedControlData* pStretchDistributionChangeData = (StretchSpeedControlData*)m_wndListCtrl.GetItemData(nCurSel);
	if(!pStretchDistributionChangeData)
		return FALSE;

	int iSubItem = (int)lParam;
	if (iSubItem != 0)
		return FALSE;
	LandsideFacilityLayoutObjectList* pLandsideObjectList = &m_pLandsideInput->getObjectList();
	CDlgSelectLandsideObject dlg(pLandsideObjectList,this);
	for(int i = ALT_LTYPE_START; i < ALT_LTYPE_END;i++)
	{
		dlg.AddObjType((ALTOBJECT_TYPE)i);
	}
	if (dlg.DoModal() == IDOK)
	{
		pStretchDistributionChangeData->SetStretchName(dlg.getSelectObject().GetIDString());
		m_wndListCtrl.SetItemText(nCurSel,0,dlg.getSelectObject().GetIDString());
	}
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	return FALSE;
}

void CDlgSpeedControl::InitListCtrl()
{
	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);

	//Intersection name
	CStringList strList;
	CProbDistManager* pProbDistMan = 0;	
	CProbDistEntry* pPDEntry = 0;
	int nCount = -1;
	if(m_pLandsideInput)
		pProbDistMan = m_pLandsideInput->GetAirPortDataBase()->getProbDistMan();
	strList.AddTail(_T("New Operating Distribution..."));
	if(pProbDistMan)
		nCount = pProbDistMan->getCount();
	for( int i=0; i< nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if(pPDEntry)
			strList.AddTail(pPDEntry->m_csName);
	}

	//vehicle Type
	LV_COLUMNEX	lvc;
	lvc.csList = & strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Facility");
	lvc.cx = 160;
	m_wndListCtrl.InsertColumn(0,&lvc);

	//Posted Speed
	CString strTitle;
	strTitle.Format(_T("Posted Speed(%s)"),CARCVelocityUnit::GetVelocityUnitString(CUnitPiece::GetCurSelVelocityUnit()));
	lvc.csList = & strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strTitle.GetBuffer();
	strTitle.ReleaseBuffer();
	lvc.cx = 160;
	m_wndListCtrl.InsertColumn(1,&lvc);

	//Operating Distribution
	lvc.csList = & strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = _T("Operating Distribution");
	lvc.cx = 219;
	m_wndListCtrl.InsertColumn(2,&lvc);
	SetListContent();
}

void CDlgSpeedControl::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = getData().GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		StretchSpeedControlData* pStretchSpeedControlData = getData().GetItem(i);
		CString strStretchName = pStretchSpeedControlData->GetStretchName();
		strStretchName.Trim();
		if(strStretchName.IsEmpty())
			strStretchName = _T("All");
		m_wndListCtrl.InsertItem(i, strStretchName);
		CString strPostSpeed;
		double dSpeed = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),pStretchSpeedControlData->GetStretchSpeed());
		strPostSpeed.Format(_T("%.2f"),dSpeed);
		m_wndListCtrl.SetItemText(i,1,strPostSpeed);
		m_wndListCtrl.SetItemText(i,2,pStretchSpeedControlData->GetStretchProDistribution().getDistName());

		m_wndListCtrl.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pStretchSpeedControlData);
	}
	if(nCount > 0)
	{
		m_wndListCtrl.SetCurSel(0);
	}


}

void CDlgSpeedControl::OnLvnItemchangedListSpeedControl( NMHDR *pNMHDR, LRESULT *pResult )
{

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		//m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
		//m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,FALSE);
		//m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,FALSE);
		//return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			//m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
			//m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,FALSE);
			//m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,FALSE);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) 
	{
		//get selected item data
		m_wndListCtrl.SetCurSel(pnmv->iItem);

		//StretchSpeedControlData* pStretchDistributionChangeData = (StretchSpeedControlData*)m_wndListCtrl.GetItemData(pnmv->iItem);
		//if(pStretchDistributionChangeData)
		//{
		//	CString strStretchName = pStretchDistributionChangeData->GetStretchName();
		//	strStretchName.Trim();
		//	if(strStretchName.IsEmpty())//the default item can not be deleted, and edited facility name
		//	{
		//		m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
		//		m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,FALSE);
		//		m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,FALSE);
		//		return;
		//	}
		//}

		//m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
		//m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,TRUE);
		//m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,TRUE);
	}
	else
	{
		//ASSERT(0);
	}

	UpdateToolBarState();
}
void CDlgSpeedControl::UpdateToolBarState()
{
	int nCurSel = m_wndListCtrl.GetCurSel();
	if(nCurSel >= 0)
	{
		StretchSpeedControlData* pStretchDistributionChangeData = (StretchSpeedControlData*)m_wndListCtrl.GetItemData(nCurSel);
		if(pStretchDistributionChangeData)
		{
			CString strStretchName = pStretchDistributionChangeData->GetStretchName();
			strStretchName.Trim();
			if(!strStretchName.IsEmpty())//the default item can not be deleted, and edited facility name
			{
				m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
				m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,TRUE);
				m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,TRUE);
				return;
			}
		}
	}
	m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
	m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,FALSE);
	m_wndSpeedToolBar.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,FALSE);

}



