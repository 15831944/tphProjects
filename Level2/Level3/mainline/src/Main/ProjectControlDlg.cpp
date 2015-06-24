// ProjectControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProjectControlDlg.h"

#include <vector>

#include "TermPlanDoc.h"
#include "TermPlan.h"
#include "../InputAirside/GlobalDatabase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectControlDlg dialog


CProjectControlDlg::CProjectControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProjectControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectControlDlg)
	DDX_Control(pDX, IDC_LIST_DELETED_PROJECT, m_cDeletedList);
	DDX_Control(pDX, IDC_LIST_PROJECT, m_cProjList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectControlDlg, CDialog)
	//{{AFX_MSG_MAP(CProjectControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_TO_DELETE, OnButtonToDelete)
	ON_BN_CLICKED(IDC_BUTTON_TO_RESTORE, OnButtonToRestore)
	ON_BN_CLICKED(IDC_BUTTON_PURGE, OnButtonPurge)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PROJECT, OnGetdispinfoListProject)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_DELETED_PROJECT, OnGetdispinfoListDeletedProject)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PROJECT, OnClickListProject)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DELETED_PROJECT, OnClickListDeletedProject)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PROJECT, OnColumnclickListProject)
	ON_NOTIFY(HDN_ITEMCLICK, IDC_LIST_PROJECT, OnItemclickListProject)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROJECT, OnItemchangedListProject)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DELETED_PROJECT, OnItemchangedListDeletedProject)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DELETED_PROJECT, OnColumnclickListDeletedProject)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectControlDlg message handlers

BOOL CProjectControlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	this->CenterWindow();

		CRect wndrect, listrect;
	GetClientRect(&wndrect);
	CString colname;
	int cx = wndrect.Width();
	int cy = wndrect.Height();

	GetDlgItem(IDCANCEL)->MoveWindow(cx-85, cy-36, 75, 26);
	GetDlgItem(IDOK)->MoveWindow(cx-170, cy-36, 75, 26);
	

	//m_cProjList.MoveWindow(0, 0, cx, cy-46);
	m_cProjList.GetClientRect(&listrect);
	colname.LoadString(IDS_PSD_PROJNAME);
	m_cProjList.InsertColumn(0, colname, LVCFMT_LEFT, listrect.Width()/4);
	colname.LoadString(IDS_PSD_USERNAME);
	m_cProjList.InsertColumn(1, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_MACHINENAME);
	m_cProjList.InsertColumn(2, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_DATECREATED);
	m_cProjList.InsertColumn(3, colname, LVCFMT_LEFT, listrect.Width()/4);
	colname.LoadString(IDS_PSD_DATEMODIFIED);
	m_cProjList.InsertColumn(4, colname, LVCFMT_LEFT, listrect.Width()/4);

	m_cProjList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	ListView_SetExtendedListViewStyle(m_cProjList.GetSafeHwnd(),LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);

	


	
//	m_cDeletedList.MoveWindow(0, 0, cx, cy-46);
	m_cDeletedList.GetClientRect(&listrect);
	colname.LoadString(IDS_PSD_PROJNAME);
	m_cDeletedList.InsertColumn(0, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_USERNAME);
	m_cDeletedList.InsertColumn(1, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_MACHINENAME);
	m_cDeletedList.InsertColumn(2, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_DATECREATED);
	m_cDeletedList.InsertColumn(3, colname, LVCFMT_LEFT, listrect.Width()/4);
	colname.LoadString(IDS_PSD_DATEMODIFIED);
	m_cDeletedList.InsertColumn(4, colname, LVCFMT_LEFT, listrect.Width()/4);

	m_cDeletedList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	ListView_SetExtendedListViewStyle(m_cDeletedList.GetSafeHwnd(),LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);


	//now populate the list
	std::vector<CString> vList;

	CProjectManager* pProjectManager=CProjectManager::GetInstance();
	pProjectManager->GetProjectList(vList);
	for(int i=0; i<static_cast<int>(vList.size()); i++) 
	{
		CString projName = vList[i];
		PROJECTINFO* pi = new PROJECTINFO();
		PROJMANAGER->GetProjectInfo(projName, pi);
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT |  LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.iImage = 0;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.lParam = (LPARAM) pi;
		if(pi->isDeleted==1)
			m_cDeletedList.InsertItem(&lvi);
		else
			m_cProjList.InsertItem(&lvi);
	}
	

	CWnd *pButton = GetDlgItem(IDC_BUTTON_PURGE);
	if(pButton)	
		pButton->SetWindowText(_T("Delete"));

	return TRUE;  
}

void CProjectControlDlg::OnButtonToDelete() 
{

	CProjectManager* pManger=CProjectManager::GetInstance();

	POSITION pos;

		int selectNum=m_cProjList.GetSelectedCount();

	   while (selectNum>0)
	   {
			
		  pos = m_cProjList.GetFirstSelectedItemPosition();
		  
		  if(pos!=NULL)
		  {
		  int nItem = m_cProjList.GetNextSelectedItem(pos);

		  
		  	PROJECTINFO* pi =(PROJECTINFO* ) m_cProjList.GetItemData(nItem);
		
			LV_ITEM lvi;
			lvi.mask = LVIF_TEXT |  LVIF_PARAM;
			lvi.iItem = 0;
			lvi.iSubItem = 0;
			lvi.iImage = 0;
			lvi.pszText = LPSTR_TEXTCALLBACK;
			lvi.lParam = (LPARAM) pi;

			m_cDeletedList.InsertItem(&lvi);
			m_cProjList.DeleteItem(nItem);

			pManger->SetProjectDeletedFlag(pi->name,true);

			CloseThisWin(pi);

			selectNum=m_cProjList.GetSelectedCount();
		  }
		  else

			  selectNum=0;	  
	   }

	   if(m_cProjList.GetItemCount()>0)
	   {
		GetDlgItem(IDC_BUTTON_TO_DELETE)->EnableWindow();
	   }
	   else
	   {
		GetDlgItem(IDC_BUTTON_TO_DELETE)->EnableWindow(false);
	   }
	
}

void CProjectControlDlg::OnButtonToRestore() 
{

		CProjectManager* pManger=CProjectManager::GetInstance();

		POSITION pos;

		int selectNum=m_cDeletedList.GetSelectedCount();

	   while (selectNum>0)
	   {
			
		  pos = m_cDeletedList.GetFirstSelectedItemPosition();
		  
		  if(pos!=NULL)
		  {
		  int nItem = m_cDeletedList.GetNextSelectedItem(pos);

		  
		  	PROJECTINFO* pi =(PROJECTINFO* ) m_cDeletedList.GetItemData(nItem);
		
			LV_ITEM lvi;
			lvi.mask = LVIF_TEXT |  LVIF_PARAM;
			lvi.iItem = 0;
			lvi.iSubItem = 0;
			lvi.iImage = 0;
			lvi.pszText = LPSTR_TEXTCALLBACK;
			lvi.lParam = (LPARAM) pi;

			m_cProjList.InsertItem(&lvi);
			m_cDeletedList.DeleteItem(nItem);

			pManger->SetProjectDeletedFlag(pi->name,false);
			selectNum=m_cDeletedList.GetSelectedCount();
		  }
		  else

			  selectNum=0;
		  
	   }
	

	   if(m_cDeletedList.GetItemCount()>0)
	   {
		   GetDlgItem(IDC_BUTTON_PURGE)->EnableWindow();
		   GetDlgItem(IDC_BUTTON_TO_RESTORE)->EnableWindow();
		   

	   }
	   else
	   {
		   GetDlgItem(IDC_BUTTON_PURGE)->EnableWindow(false);
		   GetDlgItem(IDC_BUTTON_TO_RESTORE)->EnableWindow(false);
	   }
		   
	
}




void CProjectControlDlg::OnButtonPurge() 
{
	if(AfxMessageBox("Are you sure?",MB_OKCANCEL)==IDCANCEL)
		return;

	CProjectManager* pProjectManager=CProjectManager::GetInstance();
	
	int iCount=m_cDeletedList.GetItemCount();
	for(int i=iCount-1;i>=0;--i)
	{
		BOOL bChecked = m_cDeletedList.GetCheck(i);
		if(bChecked)
		{
			PROJECTINFO* pi =(PROJECTINFO* ) m_cDeletedList.GetItemData(i);
			pProjectManager->DeleteProject(*pi);
			m_cDeletedList.DeleteItem(i);



            //Delete Project Database Name From Database
			try
			{
				CTermPlanApp *pTermApp = (CTermPlanApp *)AfxGetApp();
				if(pTermApp)
				{
					CMasterDatabase * pMasterDatabase = pTermApp->GetMasterDatabase();
					if(pMasterDatabase)
						pMasterDatabase->DeleteProjectDatabaseFromDB(pi->name);
				}
			}
			catch (_com_error &e)
			{
				CString strError = (char*)_bstr_t(e.Description());
			}
		}
	}
//SetCurrentDirectory("F:\\ARCTERM\\Project");
//RemoveDirectory("F:\\ARCTERM\\Project\\a");
	//m_cDeletedList.DeleteAllItems();

	
}




void CProjectControlDlg::FreeItemMemory()
{
	int nCount = m_cProjList.GetItemCount();
	if(nCount > 0)
		for(int i=0; i<nCount; i++)
			delete (PROJECTINFO*) m_cProjList.GetItemData(i);


	nCount = m_cDeletedList.GetItemCount();
	if(nCount > 0)
		for(int i=0; i<nCount; i++)
			delete (PROJECTINFO*) m_cDeletedList.GetItemData(i);


}

void CProjectControlDlg::OnDestroy() 
{
	FreeItemMemory();
	CDialog::OnDestroy();
	
	
	
}




void CProjectControlDlg::OnGetdispinfoListProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if(pDispInfo->item.mask & LVIF_TEXT) {
		PROJECTINFO* pInfo = (PROJECTINFO*) pDispInfo->item.lParam;
		switch(pDispInfo->item.iSubItem) {
		case 0: //proj name
			lstrcpy(pDispInfo->item.pszText, pInfo->name);
			break;
		case 1: //user
			lstrcpy(pDispInfo->item.pszText, pInfo->user);
			break;
		case 2: //machine
			lstrcpy(pDispInfo->item.pszText, pInfo->machine);
			break;
		case 3:
			lstrcpy(pDispInfo->item.pszText, pInfo->createtime.Format("%x %X"));
			break;
		case 4:
			lstrcpy(pDispInfo->item.pszText, pInfo->modifytime.Format("%x %X"));
			break;
		}
	}
	*pResult = 0;
	
	*pResult = 0;
}

void CProjectControlDlg::OnGetdispinfoListDeletedProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if(pDispInfo->item.mask & LVIF_TEXT) {
		PROJECTINFO* pInfo = (PROJECTINFO*) pDispInfo->item.lParam;
		switch(pDispInfo->item.iSubItem) {
		case 0: //proj name
			lstrcpy(pDispInfo->item.pszText, pInfo->name);
			break;
		case 1: //user
			lstrcpy(pDispInfo->item.pszText, pInfo->user);
			break;
		case 2: //machine
			lstrcpy(pDispInfo->item.pszText, pInfo->machine);
			break;
		case 3:
			lstrcpy(pDispInfo->item.pszText, pInfo->createtime.Format("%x %X"));
			break;
		case 4:
			lstrcpy(pDispInfo->item.pszText, pInfo->modifytime.Format("%x %X"));
			break;
		}
	}
	*pResult = 0;
	
	
}

void CProjectControlDlg::OnClickListProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	int selectedNum=m_cProjList.GetSelectedCount();
	if(selectedNum>0)
	{
		GetDlgItem(IDC_BUTTON_TO_DELETE)->EnableWindow();
			
	}		
	else
	{
		GetDlgItem(IDC_BUTTON_TO_DELETE)->EnableWindow(false);
			
	}



	for(int i=0;i<m_cProjList.GetItemCount();++i)
	{
		if(m_cProjList.GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED)
			m_cProjList.SetCheck(i);
		else
			m_cProjList.SetCheck(i,false);

	}

*pResult = 0;
		
	
}

void CProjectControlDlg::OnClickListDeletedProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int selectedNum=m_cDeletedList.GetSelectedCount();
	if(selectedNum>0)
	{
		GetDlgItem(IDC_BUTTON_TO_RESTORE)->EnableWindow();
		GetDlgItem(IDC_BUTTON_PURGE)->EnableWindow();
			
	}		
	else
	{
		GetDlgItem(IDC_BUTTON_TO_DELETE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_PURGE)->EnableWindow(false);
		
			
	}

		for(int i=0;i<m_cDeletedList.GetItemCount();++i)
	{
		if(m_cDeletedList.GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED)
			m_cDeletedList.SetCheck(i);
		else
			m_cDeletedList.SetCheck(i,false);

	}

		
	
	*pResult = 0;
}

void CProjectControlDlg::OnColumnclickListProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_cProjList.SortItems(CompareFunc, pNMListView->iSubItem);	
	*pResult = 0;
}

void CProjectControlDlg::OnItemclickListProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	AfxMessageBox("aaa");
	
	*pResult = 0;
}

int CALLBACK CProjectControlDlg::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//static int sortorder = 0;
	PROJECTINFO* pItem1 = (PROJECTINFO*) lParam1;
	PROJECTINFO* pItem2 = (PROJECTINFO*) lParam2;
	int nResult;
	switch(lParamSort) {
	case 0: // sort by proj name
		nResult = pItem1->name.CompareNoCase(pItem2->name);
		break;
	case 1: // sort by user
		nResult = pItem1->user.CompareNoCase(pItem2->user);
		break;
	case 2: // sort by machine
		nResult = pItem1->machine.CompareNoCase(pItem2->machine);
		break;
	case 3: // sort by create date
		nResult = (pItem1->createtime < pItem2->createtime) ? 1 : -1;
		if(pItem1->createtime == pItem2->createtime)
			nResult = 0;
		break;
	case 4: // sort by modify date
		nResult = (pItem1->modifytime < pItem2->modifytime) ? 1 : -1;
		if(pItem1->modifytime == pItem2->modifytime)
			nResult = 0;
		break;
	}
	//if(sortorder = (++sortorder % 2))
	//	nResult = -nResult;
	return nResult;
}



void CProjectControlDlg::OnItemchangedListProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	for(int i=0;i<m_cProjList.GetItemCount();++i)
	{
		if(ListView_GetCheckState(m_cProjList.GetSafeHwnd(),i))
		{
			m_cProjList.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
		}
		else
			m_cProjList.SetItemState(i,0,LVIS_SELECTED);
			
	}


	int selectedNum=m_cProjList.GetSelectedCount();
	if(selectedNum>0)
	{
		GetDlgItem(IDC_BUTTON_TO_DELETE)->EnableWindow();
			
	}		
	else
	{
		GetDlgItem(IDC_BUTTON_TO_DELETE)->EnableWindow(false);
			
	}
		
	
	*pResult = 0;
}

void CProjectControlDlg::OnItemchangedListDeletedProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	

	for(int i=0;i<m_cDeletedList.GetItemCount();++i)
	{
		if(ListView_GetCheckState(m_cDeletedList.GetSafeHwnd(),i))
		{
			m_cDeletedList.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
		}
		else
			m_cDeletedList.SetItemState(i,0,LVIS_SELECTED);
			
	}


	int selectedNum=m_cDeletedList.GetSelectedCount();
	if(selectedNum>0)
	{
		GetDlgItem(IDC_BUTTON_TO_RESTORE)->EnableWindow();
		GetDlgItem(IDC_BUTTON_PURGE)->EnableWindow();
			
	}		
	else
	{
		GetDlgItem(IDC_BUTTON_TO_RESTORE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_PURGE)->EnableWindow();
			
	}
	
	
	*pResult = 0;
}

void CProjectControlDlg::OnColumnclickListDeletedProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		m_cDeletedList.SortItems(CompareFunc, pNMListView->iSubItem);	
	
	*pResult = 0;
}

void CProjectControlDlg::CloseThisWin(PROJECTINFO* pi)
{

		std::vector<CString> vList;
		CProjectManager* pManger=CProjectManager::GetInstance();

		pManger->GetProjectList(vList);
		int iCount=vList.size();

		CMDIChildWnd* pMDIActive =NULL;
		CTermPlanDoc* pDoc =NULL;
		boolean bNotFind=true;
		for(int j=0;j<iCount&&bNotFind;++j)
		{
		
			pMDIActive=((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->MDIGetActive();
			
			if(pMDIActive!=NULL)
			{
				
				pDoc=(CTermPlanDoc*)pMDIActive->GetActiveDocument();
				if(pDoc->m_ProjInfo.name==pi->name)
				{
					pMDIActive->DestroyWindow();
					bNotFind=false;
				}
								
			}

			((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->MDINext();
			

		}

}



void CProjectControlDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	static const int nSmallSpace=5;
	static const int nLargeSpace=10;
	static const int nXLargeSpace=15;
	
	CRect rcBtn;
	CRect rcBtnOK;
	CRect rcTmp;
	CWnd* pWnd = GetDlgItem(IDC_BUTTON_PURGE);
	pWnd->GetWindowRect(&rcBtn);
	pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(rcBtnOK);

	int nFrameWidth = ((cx - rcBtn.Width())/2)-nSmallSpace-nLargeSpace;
	int nFrameHeight = cy-2*nLargeSpace-nSmallSpace-rcBtnOK.Height();

	pWnd = GetDlgItem(IDC_STATIC_AVAILABLEPROJECTS);
	pWnd->MoveWindow(nSmallSpace, nLargeSpace, nFrameWidth, nFrameHeight);
	pWnd->GetWindowRect(&rcTmp);
	ScreenToClient(&rcTmp);
	m_cProjList.MoveWindow(rcTmp.left+nSmallSpace, rcTmp.top+nXLargeSpace, nFrameWidth-2*nSmallSpace, nFrameHeight-nXLargeSpace-nSmallSpace);

	pWnd = GetDlgItem(IDC_STATIC_DELETEDPROJECTS);
	pWnd->MoveWindow(nFrameWidth+rcBtn.Width()+2*nLargeSpace+nSmallSpace, nLargeSpace, nFrameWidth, nFrameHeight);
	pWnd->GetWindowRect(&rcTmp);
	ScreenToClient(&rcTmp);
	m_cDeletedList.MoveWindow(rcTmp.left+nSmallSpace,rcTmp.top+nXLargeSpace, nFrameWidth-2*nSmallSpace, nFrameHeight-nXLargeSpace-2*nSmallSpace-rcBtn.Height());

	pWnd = GetDlgItem(IDC_BUTTON_PURGE);
	pWnd->MoveWindow(rcTmp.left+static_cast<int>(0.5*(nFrameWidth-rcBtn.Width())), rcTmp.bottom-rcBtn.Height()-nSmallSpace, rcBtn.Width(), rcBtn.Height());

	pWnd = GetDlgItem(IDC_BUTTON_TO_DELETE);
	pWnd->MoveWindow(static_cast<int>(0.5*(cx-rcBtn.Width())), static_cast<int>(0.2*cy), rcBtn.Width(), rcBtn.Height());

	pWnd = GetDlgItem(IDC_BUTTON_TO_RESTORE);
	pWnd->MoveWindow(static_cast<int>(0.5*(cx-rcBtn.Width())), static_cast<int>(0.65*cy), rcBtn.Width(), rcBtn.Height());

	pWnd = GetDlgItem(IDOK);
	pWnd->MoveWindow(cx-nSmallSpace-2*rcBtnOK.Width()-nLargeSpace, cy-nSmallSpace-rcBtnOK.Height(), rcBtnOK.Width(), rcBtnOK.Height());

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->MoveWindow(cx-nSmallSpace-rcBtnOK.Width(), cy-nSmallSpace-rcBtnOK.Height(), rcBtnOK.Width(), rcBtnOK.Height());
}
