// DlgInTrailSeperationEx.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "..\common\ProbDistManager.h"
#include "..\common\AirportDatabase.h"
#include "../InputAirside/InputAirside.h"
#include "..\InputAirside\ALTObject.h"
#include "DlgInTrailSeperationEx.h"
#include "DlgSelectSector.h"
#include ".\dlgintrailseperationex.h"
#include "../Database//DBElementCollection_Impl.h"
// CDlgInTrailSeperationEx dialog

IMPLEMENT_DYNAMIC(CDlgInTrailSeperationEx, CXTResizeDialog)
CDlgInTrailSeperationEx::CDlgInTrailSeperationEx(int nProjID, Terminal* pTerminal,CAirportDatabase* pAirportDB,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgInTrailSeperationEx::IDD, pParent)
	,m_pAirportDB(pAirportDB)
	,m_pInputAirside(pInputAirside)
	,m_pSelectProbDistEntry(pSelectProbDistEntry)
	, m_nCurDivRadius(0)
{
	m_nProjID = nProjID;
	m_pTerminal = pTerminal;
	m_pCurInTrailSepData = NULL;
	m_pCurInTrailSep = NULL;
	m_pClsNone = new AircraftClassifications(m_nProjID, NoneBased);
	m_pClsNone->ReadData();

	m_pClsWakeVortexWightBased = new AircraftClassifications(m_nProjID, WakeVortexWightBased);
	m_pClsWakeVortexWightBased->ReadData();

	m_pClsWingspanBased = new AircraftClassifications(m_nProjID, WingspanBased);
	m_pClsWingspanBased->ReadData();

	m_pClsSurfaceBearingWeightBased = new AircraftClassifications(m_nProjID, SurfaceBearingWeightBased);
	m_pClsSurfaceBearingWeightBased->ReadData();

	m_pClsApproachSpeedBased = new AircraftClassifications(m_nProjID, ApproachSpeedBased);
	m_pClsApproachSpeedBased->ReadData();
}

CDlgInTrailSeperationEx::~CDlgInTrailSeperationEx()
{
	if ( m_pClsNone )
	{
		delete m_pClsNone;
		m_pClsNone = NULL;
	}
	if ( m_pClsWakeVortexWightBased )
	{
		delete m_pClsWakeVortexWightBased;
		m_pClsWakeVortexWightBased = NULL;
	}
	if ( m_pClsWingspanBased )
	{
		delete m_pClsWingspanBased;
		m_pClsWingspanBased = NULL;
	}
	if ( m_pClsSurfaceBearingWeightBased )
	{
		delete m_pClsSurfaceBearingWeightBased;
		m_pClsSurfaceBearingWeightBased = NULL;
	}
	if ( m_pClsApproachSpeedBased )
	{
		delete m_pClsApproachSpeedBased;
		m_pClsApproachSpeedBased = NULL;
	}
}

void CDlgInTrailSeperationEx::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBOX_DATA,m_cbCategoryType);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
	DDX_Control(pDX,IDC_TOOLBAR_CONTROL,m_toolBarRect);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_nCurDivRadius);
}


BEGIN_MESSAGE_MAP(CDlgInTrailSeperationEx, CXTResizeDialog)
	ON_WM_CREATE()
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipText ) 
	ON_COMMAND(ID_TOOLBAR_INTRAIL_ADD,OnBtnToolAdd)
	ON_COMMAND(ID_TOOLBAR_INTRAIL_EDIT,OnBtnToolEdit)
	ON_COMMAND(ID_TOOLBAR_INTRAIL_DEL,OnBtnToolDel)
	ON_COMMAND(ID_Terminal,OnAddTerminalPhase)
	ON_COMMAND(ID_Approach,OnAddApproachPhase)
	ON_COMMAND(ID_CRUISE,OnAddCruisePhase)
	ON_COMMAND(ID_ALL,OnAddAllPhase)
	ON_NOTIFY(NM_RCLICK,IDC_TREE_DATA, OnRClick)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnSelchangedTree)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_DATA,OnSelListCtrlComboBox)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SAVE_DATA,SaveData)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBOX_DATA, OnCbnSelchangeComboData)
	ON_EN_KILLFOCUS(IDC_EDIT_RADIUS, OnEnKillfocusEditRadius)
END_MESSAGE_MAP()


// CDlgInTrailSeperationEx message handlers

BOOL CDlgInTrailSeperationEx::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	InitCategoryType();
	InitListCtrl();
	InitTreeCtrl();
	InitToolBar();
	m_pCurInTrailSep = NULL;
	
	m_nCurDivRadius = m_inTrailSepList.getRadiusofConcer();
	UpdateData(FALSE);
	SetResize(IDC_SAVE_DATA, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

 	SetResize(IDC_STATIC_SECTOR, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_CRITERA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	
	SetResize(IDC_TREE_DATA, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_LIST_DATA, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_TOOLBAR_CONTROL,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_STATIC_CLASS, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_RADIUS,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_NM,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_COMBOX_DATA, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_EDIT_RADIUS,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDC_SPIN_RADIUS,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);

	m_wndTreeCtrl.Select(m_wndTreeCtrl.GetChildItem(m_wndTreeCtrl.GetRootItem()),TVGN_CARET|TVIS_SELECTED);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	GetDlgItem(IDC_SAVE_DATA)->EnableWindow(FALSE);
	return TRUE;
}

void CDlgInTrailSeperationEx::InitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Trail Type", "Lead Type", "Min Distance(nm)", "Min Time" ,"Spatial Converging(nm)","Spatial diverging(nm)","Trail distribution","Time distribution","Spatial distribution"};
	int DefaultColumnWidth[] = { 85, 85, 100, 80, 80,80,100,100,100};

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT, 
			LVCFMT_NOEDIT,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_NUMBER,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN
	};

	
	CStringList strList;
	CProbDistManager* pProbDistMan = 0;	
	CProbDistEntry* pPDEntry = 0;
	int nCount = -1;
	if(m_pAirportDB)
		pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	strList.AddTail(_T("New Probability Distribution..."));
	if(pProbDistMan)
		nCount = pProbDistMan->getCount();
	for( int i=0; i< nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if(pPDEntry)
			strList.AddTail(pPDEntry->m_csName);
	}
	LV_COLUMNEX lvc;
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	for (int i = 0; i < 9; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

void CDlgInTrailSeperationEx::InitCategoryType()
{
	m_cbCategoryType.AddString( "None" );
	m_cbCategoryType.AddString( "Wake Vortex" );
	m_cbCategoryType.AddString( "Wingspans" );
	m_cbCategoryType.AddString( "Surface bearing weight" );
	m_cbCategoryType.AddString( "Approach speed" );

	m_cbCategoryType.SetCurSel( 0 );
	m_emCurCategoryType = NoneBased;
}

void CDlgInTrailSeperationEx::InitToolBar()
{
	CRect rc;
	m_toolBarRect.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_wndToolBar.MoveWindow(&rc,true);
	m_wndToolBar.ShowWindow( SW_SHOW );

	std::vector<ALTObject> vObject;
	ALTObject::GetObjectList(ALT_SECTOR,m_nProjID,vObject);
	int nObjectCount = (int)vObject.size();
	if (nObjectCount == 0)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_ADD,false);
	}
	else
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_ADD,true);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_DEL,false);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_EDIT,false);
}

void CDlgInTrailSeperationEx::InitTreeCtrl()
{
	CString strSectorName;
 //	HTREEITEM RootItem = m_wndTreeCtrl.InsertItem("Sector",TVI_ROOT);
	LoadData();
	ALTObject altObject;
	std::vector<ALTObject> vObject;
	ALTObject::GetObjectList(ALT_SECTOR,m_nProjID,vObject);
	int nIndex = 0;
	HTREEITEM curItem = NULL;
	HTREEITEM childItem = NULL;
	for(;nIndex != m_inTrailSepList.GetItemCount();++nIndex)
	{
		m_pCurInTrailSep = m_inTrailSepList.GetItem(nIndex);
		int nObjectCount = (int)vObject.size();
		if (m_pCurInTrailSep->getSectorID() == -1)
		{
			if (m_wndTreeCtrl.GetRootItem() == NULL)
			{
				curItem = m_wndTreeCtrl.InsertItem(_T("All Sector"),TVI_ROOT);
				m_wndTreeCtrl.SetItemData(curItem,(DWORD)m_pCurInTrailSep->getSectorID());
			}
			childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(m_pCurInTrailSep->getPhaseType()),m_wndTreeCtrl.GetRootItem(),TVI_LAST);
			m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);

			if (m_pCurInTrailSep->getPhaseType() == AllPhase)
			{
				DisplayCurData(m_pCurInTrailSep->getSectorID(),AllPhase);
			}
		}
		else
		{
			for (int i = 0;i < nObjectCount;i++)
			{
				int k = 0;
				CString strSectorName;
				ALTObjectID objName;
				altObject = vObject.at(i);
				altObject.getObjName(objName);
				if(m_pCurInTrailSep->getSectorID() == altObject.getID())
				{
					strSectorName = objName.GetIDString();
					if (m_wndTreeCtrl.GetRootItem() == NULL)
					{				
						childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(m_pCurInTrailSep->getPhaseType()),curItem,TVI_LAST);
						m_wndTreeCtrl.SetItemData(curItem,(DWORD)m_pCurInTrailSep->getSectorID());
						m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);
					}
					else
					{
						curItem = FindItem(m_wndTreeCtrl.GetRootItem(),m_pCurInTrailSep->getSectorID());
						if (curItem == NULL)
						{
							curItem = m_wndTreeCtrl.InsertItem(strSectorName,TVI_ROOT);
							m_wndTreeCtrl.SetItemData(curItem,(DWORD)m_pCurInTrailSep->getSectorID());
							childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(m_pCurInTrailSep->getPhaseType()),curItem,TVI_LAST);
							m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);
						}
						else
						{
							childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(m_pCurInTrailSep->getPhaseType()),curItem,TVI_LAST);
							m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);
						}
					}
					m_wndTreeCtrl.Expand(curItem,TVE_EXPAND);
				}
			}
		}
	}
}

int CDlgInTrailSeperationEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_INTRAIL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgInTrailSeperationEx::OnBtnToolAdd()
{
	CString strSector;
	BOOL bFind = FALSE;
	CDlgSelectSector dlg(m_nProjID);
	if(IDOK !=dlg.DoModal())
	{
		return;
	}
	strSector = dlg.GetListSector();
	m_nSector = dlg.GetSectorID();
	if(m_nSector<=0)
		 return;
	if(!strSector.IsEmpty())
	{	
		HTREEITEM curItem = NULL;
		HTREEITEM childItem = NULL;
		if (FindItem(m_wndTreeCtrl.GetRootItem(),m_nSector) == NULL)
		{
			AddAllNewData(m_nSector,AllPhase);
			curItem = m_wndTreeCtrl.InsertItem(strSector,TVI_ROOT);
			m_wndTreeCtrl.SetItemData(curItem,(int)m_nSector);
			childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(AllPhase),curItem,TVI_LAST);
			m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);
// 			AddAllNewData(m_nSector,ApproachPhase);
// 			childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(ApproachPhase),curItem,TVI_LAST);
// 			m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);
// 			AddAllNewData(m_nSector,TerminalPhase);
// 			childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(TerminalPhase),curItem,TVI_LAST);
// 			m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);	
		}

		m_wndTreeCtrl.Expand(curItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.SetRedraw(TRUE);
	GetDlgItem(IDC_SAVE_DATA)->EnableWindow(TRUE);
}

void CDlgInTrailSeperationEx::OnBtnToolEdit()
{
	CString strSector = _T("");
	CString strProperty = _T("");
	int nSectorID = 0;
	BOOL bSameSector = FALSE;
	CDlgSelectSector dlg(m_nProjID);
	if(IDOK !=dlg.DoModal())
	{
		return;
	}
	strSector = dlg.GetListSector();
	nSectorID = dlg.GetSectorID();
	HTREEITEM hCurItem = NULL;
	HTREEITEM htreeItem = m_wndTreeCtrl.GetSelectedItem();
	if (FindItem(m_wndTreeCtrl.GetRootItem(),nSectorID) == NULL)
	{
		m_inTrailSepList.MoidfyBySector(m_nSector,nSectorID);
		m_wndTreeCtrl.SetItemText(htreeItem,strSector);
		m_wndTreeCtrl.SetItemData(htreeItem,(DWORD)nSectorID);
	}
	
	m_wndTreeCtrl.Expand(htreeItem,TVE_EXPAND);
	m_wndTreeCtrl.SetRedraw(TRUE);
	GetDlgItem(IDC_SAVE_DATA)->EnableWindow(TRUE);
}
void CDlgInTrailSeperationEx::OnBtnToolDel()
{
	HTREEITEM curItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hChileItem = m_wndTreeCtrl.GetNextItem(curItem,TVGN_CHILD);

	if (hChileItem == NULL)
	{
		m_pCurInTrailSep = (CInTrailSeparationEx*)m_wndTreeCtrl.GetItemData(curItem);
		int nIndex = 0;
		int nDataIndex = 0;
		for(;nIndex != m_inTrailSepList.GetItemCount();++nIndex)
		{
			if (m_pCurInTrailSep == m_inTrailSepList.GetItem(nIndex))
			{
				m_pCurInTrailSep->DelDataByClass();
				m_inTrailSepList.Deltem(nIndex);
				break;
			}
		}
		m_wndTreeCtrl.DeleteItem(curItem);
	}
	else
	{
		m_nSector = (int)m_wndTreeCtrl.GetItemData(curItem);
		m_inTrailSepList.DeltemBySector(m_nSector);
		while (m_wndTreeCtrl.GetNextItem(curItem,TVGN_CHILD) != NULL)
		{
			m_wndTreeCtrl.DeleteItem(m_wndTreeCtrl.GetNextItem(curItem,TVGN_CHILD));
		}
		m_wndTreeCtrl.DeleteItem(curItem);
	}

	DisplayCurData(m_nSector,m_emCurPhaseType);
	GetDlgItem(IDC_SAVE_DATA)->EnableWindow(TRUE);
}

void CDlgInTrailSeperationEx::OnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM Item = m_wndTreeCtrl.GetSelectedItem();
	if (Item == m_wndTreeCtrl.GetRootItem())
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_EDIT,FALSE);
	}
	else
	{
		if (m_wndTreeCtrl.GetChildItem(Item) == NULL)
		{
			if (m_wndTreeCtrl.GetChildItem(m_wndTreeCtrl.GetRootItem()) == Item)
			{
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_DEL,FALSE);
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_EDIT,FALSE);
			}
			else
			{
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_EDIT,FALSE);
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_DEL,TRUE);
			}
		}
		else
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_DEL,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_INTRAIL_EDIT,TRUE);
		}
	}
	
	if (m_wndTreeCtrl.GetChildItem(Item) == NULL&& m_wndTreeCtrl.GetParentItem(Item) != NULL )
	{
		m_pCurInTrailSep = (CInTrailSeparationEx*)m_wndTreeCtrl.GetItemData(Item);
	}
	else
	{
		m_pCurInTrailSep = NULL;
		m_nSector = (int)m_wndTreeCtrl.GetItemData(Item);
	}
	if(Item == m_wndTreeCtrl.GetRootItem())
	{
		m_pCurInTrailSep = m_inTrailSepList.GetItemByType((int)m_wndTreeCtrl.GetItemData(Item));
	}
	if (NULL != m_pCurInTrailSep)
	{
		m_nSector = m_pCurInTrailSep->getSectorID();
		DisplayCurData(m_nSector,m_emCurPhaseType);
	}
	else
	{
	//	GetDlgItem(IDC_EDIT_RADIUS)->SetWindowText("0");
		m_cbCategoryType.SetCurSel(0);
		m_wndListCtrl.DeleteAllItems();
	}
	if (m_wndTreeCtrl.GetChildItem(Item) == NULL && m_wndTreeCtrl.GetParentItem(Item) == NULL&&Item !=m_wndTreeCtrl.GetRootItem())
	{
		m_wndTreeCtrl.DeleteItem(Item);
	}
	*pResult = 0;
}

void CDlgInTrailSeperationEx::LoadData()
{
	m_inTrailSepList.ReadData(m_nProjID);
}

void CDlgInTrailSeperationEx::SaveData()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_inTrailSepList.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	GetDlgItem(IDC_SAVE_DATA)->EnableWindow(FALSE);
}

void CDlgInTrailSeperationEx::AddAllNewData(int nSectorID,PhaseType emPaseType)
{
	switch(emPaseType)
	{
	case CruisePhase:
		{
			AddDataByType(CruisePhase, NoneBased,nSectorID);
			AddDataByType(CruisePhase, WingspanBased,nSectorID);
			AddDataByType(CruisePhase, SurfaceBearingWeightBased,nSectorID);
			AddDataByType(CruisePhase, WakeVortexWightBased,nSectorID);
			AddDataByType(CruisePhase, ApproachSpeedBased,nSectorID);

			m_pCurInTrailSep->setCategoryType(WakeVortexWightBased);
			m_pCurInTrailSep->setConvergentUnder(12);
			m_pCurInTrailSep->setDivergentOver(12);
			m_pCurInTrailSep->setRadiusofConcer(10);
		}
		break;
	case TerminalPhase:
		{
			AddDataByType(TerminalPhase, NoneBased,nSectorID);
			AddDataByType(TerminalPhase, WingspanBased,nSectorID);
			AddDataByType(TerminalPhase, SurfaceBearingWeightBased,nSectorID);
			AddDataByType(TerminalPhase, WakeVortexWightBased,nSectorID);
			AddDataByType(TerminalPhase, ApproachSpeedBased,nSectorID);
		
			m_pCurInTrailSep->setCategoryType(WakeVortexWightBased);
			m_pCurInTrailSep->setConvergentUnder(12);
			m_pCurInTrailSep->setDivergentOver(12);
			m_pCurInTrailSep->setRadiusofConcer(10);
		}
		break;
	case ApproachPhase:
		{
			AddDataByType(ApproachPhase, NoneBased,nSectorID);
			AddDataByType(ApproachPhase, WingspanBased,nSectorID);
			AddDataByType(ApproachPhase, SurfaceBearingWeightBased,nSectorID);
			AddDataByType(ApproachPhase, WakeVortexWightBased,nSectorID);
			AddDataByType(ApproachPhase, ApproachSpeedBased,nSectorID);

			m_pCurInTrailSep->setCategoryType(WakeVortexWightBased);
			m_pCurInTrailSep->setConvergentUnder(12);
			m_pCurInTrailSep->setDivergentOver(12);
			m_pCurInTrailSep->setRadiusofConcer(10);
		}
		break;
	case AllPhase:
		{
			AddDataByType(AllPhase, NoneBased,nSectorID);
			AddDataByType(AllPhase, WingspanBased,nSectorID);
			AddDataByType(AllPhase, SurfaceBearingWeightBased,nSectorID);
			AddDataByType(AllPhase, WakeVortexWightBased,nSectorID);
			AddDataByType(AllPhase, ApproachSpeedBased,nSectorID);

			m_pCurInTrailSep->setCategoryType(WakeVortexWightBased);
			m_pCurInTrailSep->setConvergentUnder(12);
			m_pCurInTrailSep->setDivergentOver(12);
			m_pCurInTrailSep->setRadiusofConcer(10);
		}
	default:
		break;
	}
}

void CDlgInTrailSeperationEx::AddDataByType(PhaseType emPhaseType, FlightClassificationBasisType emCategoryType,int nSectorID)
{
	BOOL IsExistType = TRUE;
	m_pCurInTrailSep = m_inTrailSepList.GetItemByType(nSectorID,emPhaseType);
	if (m_pCurInTrailSep == NULL)
	{
		IsExistType = FALSE;
		m_pCurInTrailSep = new CInTrailSeparationEx(emPhaseType,nSectorID);
		m_pCurInTrailSep->setProjID( m_nProjID );
	}

	switch ( emCategoryType )
	{
	case NoneBased:
		{
			if (m_pCurInTrailSep->GetItemCount() <= 0)
			{
				AircraftClassificationItem classificationItem(emCategoryType);
				classificationItem.setName( "Default" );
				m_pCurInTrailSepData = new CInTrailSeparationDataEx();
				m_pCurInTrailSepData->setInTrailSepID( m_pCurInTrailSep->getID() );
				m_pCurInTrailSepData->setClsLeadItem( classificationItem );
				m_pCurInTrailSepData->setClsTrailItem( classificationItem );
				m_pCurInTrailSepData->setMinDistance(5);
				m_pCurInTrailSepData->setMinTime(2);
				m_pCurInTrailSepData->setCategoryType(NoneBased);
				m_pCurInTrailSepData->setTrailDistName("U[10~20]");
				m_pCurInTrailSepData->setTrailPrintDist("Uniform:10;20");
				m_pCurInTrailSepData->setProbTrailType(UNIFORM);
				m_pCurInTrailSepData->setTimeDistName("U[3~5]");
				m_pCurInTrailSepData->setTimePrintDist("Uniform:3;5");
				m_pCurInTrailSepData->SetProbTimeType(UNIFORM);
				m_pCurInTrailSepData->setSpatialDistName("U[6~9]");
				m_pCurInTrailSepData->setSpatialPrintDist("Uniform:6;9");
				m_pCurInTrailSepData->setProbSpatialType(UNIFORM);
				m_pCurInTrailSepData->setSpatialConverging(1);
				m_pCurInTrailSepData->setSpatialDiverging(1);
				m_pCurInTrailSep->AddItem( m_pCurInTrailSepData );
				m_inTrailSepList.AddItem( m_pCurInTrailSep );
			}
		}
		return;
	case WakeVortexWightBased:
		m_pCurClassifications = m_pClsWakeVortexWightBased;
		break;
	case WingspanBased:
		m_pCurClassifications = m_pClsWingspanBased;
		break;
	case SurfaceBearingWeightBased:
		m_pCurClassifications = m_pClsSurfaceBearingWeightBased;
		break;
	case ApproachSpeedBased:
		m_pCurClassifications = m_pClsApproachSpeedBased;
		break;
	default:
		break;

	}

	int nCount = m_pCurClassifications->GetCount();

	for (int i = 0; i < nCount; i++)
	{
		for (int j = 0; j < nCount; j++)
		{
			BOOL IsItemExist = false;
			if (m_pCurInTrailSep != NULL)
			{
				for (int k = 0; k < m_pCurInTrailSep->GetItemCount(); k++)
				{
					m_pCurInTrailSepData = m_pCurInTrailSep->GetItem(k);
					int nClsTrailID = m_pCurInTrailSepData->getClsTrailItem()->m_nID;
					int nClsLeadID = m_pCurInTrailSepData->getClsLeadItem()->m_nID;

					if ( nClsTrailID == m_pCurClassifications->GetItem(i)->m_nID &&
						nClsLeadID == m_pCurClassifications->GetItem(j)->m_nID )
					{
						IsItemExist = TRUE;
						break;
					}
				}
			}
			if ( !IsItemExist )
			{
				AircraftClassificationItem *pClsTrailItem = m_pCurClassifications->GetItem(i);
				AircraftClassificationItem *pClsLeadItem = m_pCurClassifications->GetItem(j);
				m_pCurInTrailSepData = new CInTrailSeparationDataEx();
				m_pCurInTrailSepData->setInTrailSepID( m_pCurInTrailSep->getID() );
				m_pCurInTrailSepData->setClsTrailItem( *pClsTrailItem );
				m_pCurInTrailSepData->setClsLeadItem( *pClsLeadItem );
				m_pCurInTrailSepData->setCategoryType(m_pCurClassifications->GetBasisType());
				m_pCurInTrailSepData->setTrailDistName("U[10~20]");
				m_pCurInTrailSepData->setTrailPrintDist("Uniform:10;20");
				m_pCurInTrailSepData->setProbTrailType(UNIFORM);
				m_pCurInTrailSepData->setTimeDistName("U[3~5]");
				m_pCurInTrailSepData->setTimePrintDist("Uniform:3;5");
				m_pCurInTrailSepData->SetProbTimeType(UNIFORM);
				m_pCurInTrailSepData->setSpatialDistName("U[6~9]");
				m_pCurInTrailSepData->setSpatialPrintDist("Uniform:6;9");
				m_pCurInTrailSepData->setProbSpatialType(UNIFORM);
				m_pCurInTrailSepData->setSpatialConverging(1);
				m_pCurInTrailSepData->setSpatialDiverging(1);

				CString strName;
				strName.Format("%s", pClsLeadItem->m_strName);
				strName.MakeLower();
				if (!strName.CompareNoCase("light"))
				{
					m_pCurInTrailSepData->setMinDistance(3);
				}
				else if (!strName.CompareNoCase("medium"))
				{
					m_pCurInTrailSepData->setMinDistance(4);
				}
				else if (!strName.CompareNoCase("heavy"))
				{
					m_pCurInTrailSepData->setMinDistance(5);
				}
				else
				{
					m_pCurInTrailSepData->setMinDistance(5);
				}

				m_pCurInTrailSepData->setMinTime(2);
				m_pCurInTrailSep->AddItem( m_pCurInTrailSepData );
			}
		}
	}
	if ( !IsExistType )
	{
		m_inTrailSepList.AddItem( m_pCurInTrailSep );
	}
}

void CDlgInTrailSeperationEx::DisplayCurData(int nSectorID,PhaseType emPhaseType)
{
	m_wndListCtrl.DeleteAllItems();
	if (NULL == m_pCurInTrailSep)
	{
		return;
	}
	FlightClassificationBasisType emCurCategoryType = m_pCurInTrailSep->getCategoryType();

	int nSelComBox = GetComboxIndexByCategoryType(emCurCategoryType);
	m_cbCategoryType.SetCurSel(nSelComBox);
	
	m_nCurDivRadius = m_inTrailSepList.getRadiusofConcer();
	for (int i = 0; i < m_pCurInTrailSep->GetItemCount(); i++)
	{
		m_pCurInTrailSepData = m_pCurInTrailSep->GetItem(i);
		if(emCurCategoryType == m_pCurInTrailSep->GetItem(i)->getCategoryType())
		{
			CString strTmp;
			int nClsItem = 0;
			int nIndex = 0;
			m_pCurInTrailSepData = m_pCurInTrailSep->GetItem(i);

			nClsItem = m_pCurInTrailSepData->getClsTrailItem()->getID();
			strTmp = GetClassificationsName(nClsItem, emCurCategoryType);
			m_wndListCtrl.InsertItem(nIndex, strTmp);

			nClsItem = m_pCurInTrailSepData->getClsLeadItem()->getID();
			strTmp = GetClassificationsName(nClsItem, emCurCategoryType);
			m_wndListCtrl.SetItemText(nIndex, 1, strTmp);

			strTmp.Format("%ld", m_pCurInTrailSepData->getMinDistance());
			m_wndListCtrl.SetItemText(nIndex, 2, strTmp);

			strTmp.Format("%ld", m_pCurInTrailSepData->getMinTime());
			m_wndListCtrl.SetItemText(nIndex, 3, strTmp);

			strTmp.Format("%ld",m_pCurInTrailSepData->getSpatialConverging());
			m_wndListCtrl.SetItemText(nIndex,4,strTmp);

			strTmp.Format("%ld",m_pCurInTrailSepData->getSpatialDiverging());
			m_wndListCtrl.SetItemText(nIndex,5,strTmp);

			m_wndListCtrl.SetItemText(nIndex,6,m_pCurInTrailSepData->getTrailDistName());
			m_wndListCtrl.SetItemText(nIndex,7,m_pCurInTrailSepData->getTimeDistName());
			m_wndListCtrl.SetItemText(nIndex,8,m_pCurInTrailSepData->getSpatialDistName());

			m_wndListCtrl.SetItemData(nIndex, (DWORD_PTR)m_pCurInTrailSepData);
		}
	}
	UpdateData(FALSE);
}
CString CDlgInTrailSeperationEx::GetClassificationsName(int nClsItemID, FlightClassificationBasisType emCategoryType)
{
	CString strClsName;
	switch ( emCategoryType )
	{
	case NoneBased:
		strClsName = "Default";
		return strClsName;
	case WakeVortexWightBased:
		m_pCurClassifications = m_pClsWakeVortexWightBased;
		break;
	case WingspanBased:
		m_pCurClassifications = m_pClsWingspanBased;
		break;
	case SurfaceBearingWeightBased:
		m_pCurClassifications = m_pClsSurfaceBearingWeightBased;
		break;
	case ApproachSpeedBased:
		m_pCurClassifications = m_pClsApproachSpeedBased;
		break;
	default:
		break;

	}

	for (int i = 0; i < m_pCurClassifications->GetCount(); i++)
	{
		AircraftClassificationItem* m_pCurClsItem;
		m_pCurClsItem = m_pCurClassifications->GetItem(i);
		if (m_pCurClsItem->getID() == nClsItemID)
		{
			strClsName = m_pCurClsItem->m_strName;
		}
	}

	return strClsName;
}

int CDlgInTrailSeperationEx::GetComboxIndexByCategoryType(FlightClassificationBasisType emCategoryType)
{
	int nSelComBox = 0;
	switch ( emCategoryType )
	{
	case NoneBased:
		nSelComBox = 0;
		break;
	case WakeVortexWightBased:
		nSelComBox = 1;
		break;
	case WingspanBased:
		nSelComBox = 2;
		break;
	case SurfaceBearingWeightBased:
		nSelComBox = 3;
		break;
	case ApproachSpeedBased:
		nSelComBox = 4;
		break;
	default:
		break;
	}
	return nSelComBox;
}

void CDlgInTrailSeperationEx::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveData();
	OnOK();
}

void CDlgInTrailSeperationEx::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgInTrailSeperationEx::OnCbnSelchangeComboData()
{
	// TODO: Add your control notification handler code here
	int nSel = m_cbCategoryType.GetCurSel();

	switch ( nSel )
	{
	case 0:
		m_emCurCategoryType = NoneBased;
		break;
	case 1:
		m_emCurCategoryType = WakeVortexWightBased;
		break;
	case 2:
		m_emCurCategoryType = WingspanBased;
		break;
	case 3:
		m_emCurCategoryType = SurfaceBearingWeightBased;
		break;
	case 4:
		m_emCurCategoryType = ApproachSpeedBased;
		break;
	default:
		break;
	}
	
	if (NULL == m_pCurInTrailSep)
	{
		return;
	}
	m_pCurInTrailSep->setCategoryType(m_emCurCategoryType);
	DisplayCurData(m_nSector,m_emCurPhaseType);
	GetDlgItem(IDC_SAVE_DATA)->EnableWindow(TRUE);

}

CString CDlgInTrailSeperationEx::getCStringByPaseType(PhaseType emPaseType)
{
	CString strPaseType;
	switch(emPaseType)
	{
	case CruisePhase:
		strPaseType = "Cruise Phase";
		break;
	case TerminalPhase:
		strPaseType = "Terminal Phase";
		break;
	case ApproachPhase:
		strPaseType = "Approach Phase";
		break;
	case AllPhase:
		strPaseType = "All Phase";
		break;
	default:
		break;
	}
	return strPaseType;
}

void CDlgInTrailSeperationEx::OnSelListCtrlComboBox(NMHDR *pNMHDR, LRESULT *pResult)
{

	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if(!pDispInfo)
		return;	 
	CString strSel;
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);

	int nItem = pDispInfo->item.iItem;
	m_pCurInTrailSepData = (CInTrailSeparationDataEx*)m_wndListCtrl.GetItemData(nItem);

	int nMinDistance = atoi(m_wndListCtrl.GetItemText(nItem, 2));
	int nMinTime = atoi(m_wndListCtrl.GetItemText(nItem, 3));


	m_pCurInTrailSepData->setMinDistance(nMinDistance);
	m_pCurInTrailSepData->setMinTime(nMinTime);
	strSel = pDispInfo->item.pszText;
	CString strTmp = _T("");
	char szBuffer[1024] = {0};
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	if(strSel == _T("New Probability Distribution..." ) )
	{
		CProbDistEntry* pPDEntry = NULL;
		pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
		switch(pDispInfo->item.iSubItem)
		{
		case 6:
			strTmp = m_pCurInTrailSepData->getTrailDistName();
			break;
		case 7:
			strTmp = m_pCurInTrailSepData->getTimeDistName();
			break;
		case 8:
			strTmp = m_pCurInTrailSepData->getSpatialDistName();
			break;
		default:
			break;
		}
		if(pPDEntry == NULL)
		{
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem,strTmp);
			return;
		}
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		CString strDistName = pPDEntry->m_csName;

		pProbDist->printDistribution(szBuffer);
		switch(pDispInfo->item.iSubItem)
		{
		case 6:
			{
				m_pCurInTrailSepData->setTrailDistName(strDistName);
				m_pCurInTrailSepData->setProbTrailType((ProbTypes)pProbDist->getProbabilityType());
				m_pCurInTrailSepData->setTrailPrintDist(szBuffer);
			}
			break;
		case 7:
			{
				m_pCurInTrailSepData->setTimeDistName(strDistName); 
				m_pCurInTrailSepData->SetProbTimeType((ProbTypes)pProbDist->getProbabilityType());
				m_pCurInTrailSepData->setTimePrintDist(szBuffer);
			}
			break;
		case 8:
			{
				m_pCurInTrailSepData->setSpatialDistName(strDistName);
				m_pCurInTrailSepData->setProbSpatialType((ProbTypes)pProbDist->getProbabilityType());
				m_pCurInTrailSepData->setSpatialPrintDist(szBuffer);
			}
			break;
		default:
			break;
			
		}
		m_wndListCtrl.SetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem,strDistName);
	}
	else if(pDispInfo->item.iSubItem > 5)
	{
		CProbDistEntry* pPDEntry = NULL;

		int nCount = pProbDistMan->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = pProbDistMan->getItem( i );
			if(pPDEntry->m_csName == strSel)
				break;
		}
		//assert( i < nCount );
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );

		CString strDistName = pPDEntry->m_csName;	
		pProbDist->printDistribution(szBuffer);
		switch(pDispInfo->item.iSubItem)
		{
		case 6:
			{
				m_pCurInTrailSepData->setTrailDistName(strDistName);
				m_pCurInTrailSepData->setProbTrailType((ProbTypes)pProbDist->getProbabilityType());
				m_pCurInTrailSepData->setTrailPrintDist(szBuffer);
			}
			break;
		case 7:
			{
				m_pCurInTrailSepData->setTimeDistName(strDistName); 
				m_pCurInTrailSepData->SetProbTimeType((ProbTypes)pProbDist->getProbabilityType());
				m_pCurInTrailSepData->setTimePrintDist(szBuffer);
			}
			break;
		case 8:
			{
				m_pCurInTrailSepData->setSpatialDistName(strDistName);
				m_pCurInTrailSepData->setProbSpatialType((ProbTypes)pProbDist->getProbabilityType());
				m_pCurInTrailSepData->setSpatialPrintDist(szBuffer);
			}
			break;
		default:
			break;

		}
	}
	*pResult = 0;
	GetDlgItem(IDC_SAVE_DATA)->EnableWindow(TRUE);
}

void CDlgInTrailSeperationEx::OnEnKillfocusEditRadius()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_inTrailSepList.setRadiusofConcer(m_nCurDivRadius);
	m_inTrailSepList.ModifyRadiusofConcer(m_inTrailSepList.getRadiusofConcer());
	GetDlgItem(IDC_SAVE_DATA)->EnableWindow(TRUE);

}


HTREEITEM CDlgInTrailSeperationEx::FindItem(HTREEITEM item,int nSectorID)
{
	HTREEITEM hFind; 
	if (item == NULL)
	{
		return NULL;
	}
	while(item != NULL)
	{
		if((int)m_wndTreeCtrl.GetItemData(item) == nSectorID)
		{
			hFind = item ;
			return hFind;
		}
		item = m_wndTreeCtrl.GetNextSiblingItem(item);
	}
	return NULL;
}

void CDlgInTrailSeperationEx::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint cliPt;
	GetCursorPos(&cliPt);
	CPoint point = cliPt;
	m_wndTreeCtrl.ScreenToClient(&cliPt);
	CMenu menu;
	UINT uFlags;
	HTREEITEM hItem = m_wndTreeCtrl.HitTest(cliPt, &uFlags);
	m_hRightClick = hItem;
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags) && m_wndTreeCtrl.GetParentItem(hItem) == NULL)
	{
		menu.LoadMenu(IDR_MENU_PHASE);
		HTREEITEM curItem = m_wndTreeCtrl.GetChildItem(m_hRightClick);
		while (curItem)
		{
			CInTrailSeparationEx* pInTrail = (CInTrailSeparationEx*)m_wndTreeCtrl.GetItemData(curItem);
			UINT nID = 0;
			switch(pInTrail->getPhaseType())
			{
			case AllPhase:
				nID = ID_ALL;
				break;
			case ApproachPhase:
				nID = ID_Approach;
				break;
			case CruisePhase:
				nID = ID_CRUISE;
				break;
			case TerminalPhase:
				nID = ID_Terminal;
				break;
			default:
				break;
			}
			menu.GetSubMenu(0)->EnableMenuItem(nID,MF_BYCOMMAND|MF_GRAYED);
			curItem = m_wndTreeCtrl.GetNextSiblingItem(curItem);
		}
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,point.x,point.y,this); 
	}  
}

void CDlgInTrailSeperationEx::OnAddTerminalPhase()
{
	HTREEITEM childItem;
	m_nSector = (int)m_wndTreeCtrl.GetItemData(m_hRightClick);
	if (!FindItemPhase(m_wndTreeCtrl.GetChildItem(m_hRightClick),TerminalPhase))
	{
		AddAllNewData(m_nSector,TerminalPhase);
		childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(TerminalPhase),m_hRightClick,TVI_LAST);
		m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);
	}
}

void CDlgInTrailSeperationEx::OnAddApproachPhase()
{
	HTREEITEM childItem;
	m_nSector = (int)m_wndTreeCtrl.GetItemData(m_hRightClick);
	if (!FindItemPhase(m_wndTreeCtrl.GetChildItem(m_hRightClick),ApproachPhase))
	{
		AddAllNewData(m_nSector,ApproachPhase);
		childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(ApproachPhase),m_hRightClick,TVI_LAST);
		m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);
	}
}

void CDlgInTrailSeperationEx::OnAddCruisePhase()
{
	HTREEITEM childItem;
	m_nSector = (int)m_wndTreeCtrl.GetItemData(m_hRightClick);
	if (!FindItemPhase(m_wndTreeCtrl.GetChildItem(m_hRightClick),CruisePhase))
	{
		AddAllNewData(m_nSector,CruisePhase);
		childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(CruisePhase),m_hRightClick,TVI_LAST);
		m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);
	}
}

void CDlgInTrailSeperationEx::OnAddAllPhase()
{
	HTREEITEM childItem;
	m_nSector = (int)m_wndTreeCtrl.GetItemData(m_hRightClick);
	if (!FindItemPhase(m_wndTreeCtrl.GetChildItem(m_hRightClick),AllPhase))
	{
		AddAllNewData(m_nSector,AllPhase);
		childItem = m_wndTreeCtrl.InsertItem(getCStringByPaseType(AllPhase),m_hRightClick,TVI_LAST);
		m_wndTreeCtrl.SetItemData(childItem,(DWORD_PTR)m_pCurInTrailSep);
	}
}

BOOL CDlgInTrailSeperationEx::FindItemPhase(HTREEITEM item,PhaseType emPhaseType)
{
	if (item == NULL)
	{
		return NULL;
	}
	while(item != NULL)
	{
		CInTrailSeparationEx* pInTrail =  (CInTrailSeparationEx*)m_wndTreeCtrl.GetItemData(item);
		if(pInTrail->getPhaseType() == emPhaseType)
		{
			return TRUE;
		}
		item = m_wndTreeCtrl.GetNextSiblingItem(item);
	}
	return FALSE;
}

BOOL CDlgInTrailSeperationEx::OnToolTipText(UINT,NMHDR* pNMHDR,LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// if there is a top level routing frame then let it handle the message
	if (GetRoutingFrame() != NULL) return FALSE;

	// to be thorough we will need to handle UNICODE versions of the message also !!
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[512];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool 
		nID = ::GetDlgCtrlID((HWND)nID);
	}

 	if (nID!= 0) // will be zero on a separator
 	{

		AfxLoadString(nID, szFullText);
 		strTipText=szFullText;

#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			_wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
		}
		else
		{
			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#endif

		*pResult = 0;

		// bring the tooltip window above other popup windows
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

		return TRUE;
	}

	return FALSE;
}
