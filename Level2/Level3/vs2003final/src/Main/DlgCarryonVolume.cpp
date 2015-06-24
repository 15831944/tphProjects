// DlgCarryonVolume.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgCarryonVolume.h"
#include "..\InputOnboard\Carryon.h"

// CDlgCarryonVolume dialog

IMPLEMENT_DYNAMIC(CDlgCarryonVolume, CXTResizeDialog)
CDlgCarryonVolume::CDlgCarryonVolume(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgCarryonVolume::IDD, pParent)
{
	m_CarryonVolumeList.ReadData(-1);
	if (m_CarryonVolumeList.GetItemCount()==0)
	{
		m_CarryonVolumeList.addDefaultData(GetDefaultPDEntry());
	}
}

CDlgCarryonVolume::~CDlgCarryonVolume()
{
}


BEGIN_MESSAGE_MAP(CDlgCarryonVolume, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_ADD,OnAddCarryonVolume)
	ON_COMMAND(ID_TOOLBAR_DEL,OnDelCarryonVolume)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_MAINTENANCE_SAVE, OnBnClickedSave)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_MAINTENANCE,OnSelComboBox)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_MAINTENANCE,OnItemChangeList)
END_MESSAGE_MAP()

void CDlgCarryonVolume::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_MAINTENANCE,m_listCarryonVolume);
}

int CDlgCarryonVolume::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolbar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

BOOL CDlgCarryonVolume::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	InitList();
	SetListContent();

	SetResize(IDC_STATIC_BARFRAME, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_MAINTENANCE, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BTN_MAINTENANCE_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetWindowText("Carryon Volume Specification");

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return true;
}
void CDlgCarryonVolume::OnSelComboBox(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	*pResult = 0;
	if(!dispinfo)
		return;
	CString strSel;
	strSel = dispinfo->item.pszText;
	int nRow=dispinfo->item.iItem;
	int nColumn=dispinfo->item.iSubItem;
	int nCarryonType;


	CCarryonVolume *pCarryonVolume=(CCarryonVolume *)m_listCarryonVolume.GetItemData(nRow);
	if (nColumn==0)
	{
		for (nCarryonType=0;nCarryonType<CARRYON_NUM;nCarryonType++)
		{
			if (CarryonItemName[nCarryonType] == strSel)
			{
				break;
			}
		}
		if (nCarryonType==CARRYON_NUM)
		{
			return;
		}
		pCarryonVolume->SetCarryonType((EnumCarryonType)nCarryonType);
	}else if (nColumn==1)
	{
		char szBuffer[1024] = {0};
		ProbabilityDistribution* pProbDist = NULL;
		CProbDistManager* pProbDistMan = GetInputTerminal()->m_pAirportDB->getProbDistMan();
		if(strSel == "NEW PROBABILITY DISTRIBUTION" )
		{
			CDlgProbDist dlg(GetInputTerminal()->m_pAirportDB,true,this);
			if (dlg.DoModal() != IDOK)
			{    
				m_listCarryonVolume.SetItemText(nRow,1,pCarryonVolume->GetVolumeDist()->getPrintDist());
				return;
			}

			CProbDistEntry* pPDEntry = dlg.GetSelectedPD();

			if(pPDEntry == NULL)
				return;

			pProbDist = pPDEntry->m_pProbDist;

			pProbDist->printDistribution(szBuffer);

			pCarryonVolume->SetVolumeDist(pPDEntry);
			
			m_listCarryonVolume.SetItemText(nRow,1,pCarryonVolume->GetVolumeDist()->getPrintDist());

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

			pProbDist = pPDEntry->m_pProbDist;
			ASSERT( pProbDist );		//pPorbDist cannot be null

			pProbDist->printDistribution(szBuffer);

			pCarryonVolume->SetVolumeDist(pPDEntry);

			m_listCarryonVolume.SetItemText(nRow,1,pCarryonVolume->GetVolumeDist()->getPrintDist());

		} 
	}
}
void CDlgCarryonVolume::OnItemChangeList(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nCurSel=m_listCarryonVolume.GetCurSel();
// 	if (nCurSel<1 /*|| ( m_listCarryonVolume.GetItemState( nCurSel, LVIS_SELECTED ) != LVIS_SELECTED )*/)
// 	{
// 		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
// 		return;
// 	}else 
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
}
void CDlgCarryonVolume::OnBnClickedOk()
{
	OnBnClickedSave();
	CXTResizeDialog::OnOK();
}
void CDlgCarryonVolume::OnBnClickedSave()
{
	m_CarryonVolumeList.SaveData(-1);
}
void CDlgCarryonVolume::InitToolBar()
{
	CRect rectFltToolbar;
	GetDlgItem(IDC_STATIC_MAINTENANCETOOLBAR)->GetWindowRect(&rectFltToolbar);
	GetDlgItem(IDC_STATIC_MAINTENANCETOOLBAR)->ShowWindow(FALSE);
	ScreenToClient(&rectFltToolbar);
	m_toolbar.MoveWindow(&rectFltToolbar, true);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
}
void CDlgCarryonVolume::InitList()
{

	DWORD dwStyle = m_listCarryonVolume.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCarryonVolume.SetExtendedStyle(dwStyle);
	CStringList strlist;	
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;


	for (int i=DEFAULTCARRYON;i<CARRYON_NUM;i++)
	{
		
		strlist.InsertAfter(strlist.GetTailPosition(),CarryonItemName[i]);
	}

	//mobile element type
	lvc.pszText = "Carryon Type";
	lvc.cx = 200;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &strlist;
	m_listCarryonVolume.InsertColumn(0,&lvc);


	//distribution
	strlist.RemoveAll();
	CString s;
	s.LoadString( IDS_STRING_NEWDIST );
	strlist.InsertAfter( strlist.GetTailPosition(), s );
	int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
		strlist.InsertAfter( strlist.GetTailPosition(), pPBEntry->m_csName );
	}
	CString strTitle = "Volume(Liter)";
	lvc.pszText  = (LPSTR) ((LPCTSTR)strTitle);
	lvc.cx = 300;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &strlist;
	m_listCarryonVolume.InsertColumn(1, &lvc);
}
void CDlgCarryonVolume::SetListContent()
{
	m_listCarryonVolume.DeleteAllItems();
	CCarryonVolume *pCarryonVolume;
	for (int i=0;i<m_CarryonVolumeList.GetItemCount();i++)
	{
		pCarryonVolume=(CCarryonVolume *)m_CarryonVolumeList.GetItem(i);
		m_listCarryonVolume.InsertItem(i,_T(""));
// 		if (pCarryonVolume->GetCarryonType() == DEFAULTCARRYON)
// 		{
// 			m_listCarryonVolume.SetItemText(i,0,"DEFUALT");
// 		}else
		{
			m_listCarryonVolume.SetItemText(i,0,CarryonItemName[(int)pCarryonVolume->GetCarryonType()]);
		}
		
		m_listCarryonVolume.SetItemText(i,1,pCarryonVolume->GetVolumeDist()->getPrintDist());
		m_listCarryonVolume.SetItemData(i,(DWORD)pCarryonVolume);
	}
}
CProbDistEntry* CDlgCarryonVolume::GetDefaultPDEntry()
{
	CProbDistManager* pProbDistMan = GetInputTerminal()->m_pAirportDB->getProbDistMan();
	CProbDistEntry* pPDEntry = NULL;

	int nCount = pProbDistMan->getCount();
	for( int i=0; i<nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if(pPDEntry->m_csName == "100")
			break;
	}

	return pPDEntry;
}
void CDlgCarryonVolume::OnAddCarryonVolume()
{

	CCarryonVolume *pCarryonVolume=new CCarryonVolume();
	pCarryonVolume->SetVolumeDist(GetDefaultPDEntry());
	m_CarryonVolumeList.AddItem(pCarryonVolume);

	int nCount=m_listCarryonVolume.GetItemCount();
	m_listCarryonVolume.InsertItem(nCount,_T(""));
 	m_listCarryonVolume.SetItemText(nCount,0,CarryonItemName[(int)pCarryonVolume->GetCarryonType()]);
	m_listCarryonVolume.SetItemText(nCount,1,pCarryonVolume->GetVolumeDist()->getPrintDist());
	m_listCarryonVolume.SetItemData(nCount,(DWORD)pCarryonVolume);
	m_listCarryonVolume.SetCurSel(nCount);
	m_listCarryonVolume.DblClickItem(nCount,0);

}
void CDlgCarryonVolume::OnDelCarryonVolume()
{
	int nCurItem=m_listCarryonVolume.GetCurSel();
	if (nCurItem<0)
	{
		return;
	}
	CCarryonVolume *pCarryonVolume=(CCarryonVolume *)m_listCarryonVolume.GetItemData(nCurItem);
	if (pCarryonVolume==NULL)
	{
		return;
	}
    m_listCarryonVolume.DeleteItemEx(nCurItem);
	m_CarryonVolumeList.DeleteItem(pCarryonVolume);

// 	if (m_CarryonVolumeList.GetItemCount()==0)
// 	{
// 		m_CarryonVolumeList.addDefaultData(GetDefaultPDEntry());
// 	}
}
InputTerminal* CDlgCarryonVolume::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (InputTerminal*)&pDoc->GetTerminal();
}

// CDlgCarryonVolume message handlers
