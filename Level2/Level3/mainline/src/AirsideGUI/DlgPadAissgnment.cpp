// DlgPadAissgnment.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "InputAirside/WeatherImpact.h"
#include "InputAirside/PadAssignment.h"
#include "DlgPadAissgnment.h"
#include ".\dlgpadaissgnment.h"
#include "..\InputAirside\ALTAirport.h"
#include "..\InputAirside\ALTObject.h"
#include "../Database/DBElementCollection_Impl.h"
#include "../Common/FLT_CNST.H"
// CDlgPadAissgnment dialog

IMPLEMENT_DYNAMIC(CDlgPadAissgnment, CXTResizeDialog)
CDlgPadAissgnment::CDlgPadAissgnment(int nProjID,PSelectFlightType pSelectFlightType,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgPadAissgnment::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pSelectFlightType(pSelectFlightType)
	,m_bEnable(TRUE)
{
	m_pWeatherList = new WeatherListInfo();
	m_pWeatherList->ReadData(nProjID);

	m_pCurWeather = new WeatherInfo();
	m_pCurFlightType = new FlightTypeInfo();
	m_pPadInfo  = NULL;

	m_vPadList.clear();
	ALTAirport::GetDeicePadList(nProjID,m_vPadList);
}

CDlgPadAissgnment::~CDlgPadAissgnment()
{
	if (m_pWeatherList)
	{
		delete m_pWeatherList;
		m_pWeatherList = NULL;
	}
}

void CDlgPadAissgnment::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_WEATHER,m_wndTreeCtrl);
	DDX_Control(pDX,IDC_LIST_PAD,m_wndListCtrl);
	DDX_Control(pDX,IDC_LIST_FLIGHTTYPE,m_wndListBox);
}


BEGIN_MESSAGE_MAP(CDlgPadAissgnment, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_PAD_ADD,OnPadItemAdd)
	ON_COMMAND(ID_TOOLBAR_PAD_DEL,OnPadItemDel)
	ON_COMMAND(ID_TOOLBAR_FLIGHT_TYPE_ADD,OnFlightTypeAdd)
	ON_COMMAND(ID_TOOLBAR_FLIGHT_TYPE_DEL,OnFlightTypeDel)
	ON_COMMAND(ID_TOOLBAR_FLIGHT_TYPE_EDIT,OnFlightTypeEdit)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_WEATHER, OnTvnSelchangedTreeWeather)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHTTYPE, OnLbnSelchangeListFlighttype)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PAD, OnLvnItemchangedListPad)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_PAD, OnEndlabeledit)
	ON_BN_CLICKED(IDC_RADIO_GATE, OnBnClickedRadioGate)
	ON_BN_CLICKED(IDC_RADIO_DEICE, OnBnClickedRadioDeice)
END_MESSAGE_MAP()


// CDlgPadAissgnment message handlers

BOOL CDlgPadAissgnment::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitWeatherConditionTree();
	OnInitListCtrl();
	OnInitToolbar();
	OnInitFlightTypeToolbarStatus();
	OnInitPadToolbarStatus();
	CheckRadioButton(IDC_RADIO_GATE, IDC_RADIO_DEICE, IDC_RADIO_GATE );
	m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,FALSE);
	m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);

	SetResize(IDC_STATIC_WEATHER,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_TREE_WEATHER,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_FLIGHT,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(m_wndFlighTypeToolbar.GetDlgCtrlID(),SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_PAD,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_FLIGHTTYPE,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_DEICE,SZ_BOTTOM_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_RADIO_GATE,SZ_BOTTOM_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_RADIO_DEICE,SZ_BOTTOM_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_TYPEFRAME,SZ_BOTTOM_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(m_wndPadToolbar.GetDlgCtrlID(),SZ_BOTTOM_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_PAD,SZ_BOTTOM_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgPadAissgnment::InitWeatherConditionTree()
{
	HTREEITEM hTreeItem       = NULL;
	HTREEITEM hTreeItemChild  = NULL;

	CString strItemCaption;

	//VFR
	strItemCaption.LoadString(IDS_VFR);
	hTreeItem = m_wndTreeCtrl.InsertItem(strItemCaption);
	strItemCaption.LoadString(IDS_NOFREEZINGPRECITITATION);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(VFR_NONFREEZINGPRECP));
	strItemCaption.LoadString(IDS_FREEZINGDRIZZLE);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(VFR_FREEZINGDRIZZLE));
	m_wndTreeCtrl.Expand(hTreeItem, TVE_EXPAND);

	//IFR
	strItemCaption.LoadString(IDS_IFR);
	hTreeItem = m_wndTreeCtrl.InsertItem(strItemCaption);
	strItemCaption.LoadString(IDS_NOFREEZINGPRECITITATION);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_NONFREEZINGPRECP));
	strItemCaption.LoadString(IDS_FREEZINGDRIZZLE);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_FREEZINGDRIZZLE));
	strItemCaption.LoadString(IDS_LIGHTSNOW);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_LIGHTSONW));
	strItemCaption.LoadString(IDS_MEDIUMSNOW);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_MEDIUMSNOW));
	strItemCaption.LoadString(IDS_HEAVYSNOW);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(IFR_HEAVYSONW));
	m_wndTreeCtrl.Expand(hTreeItem, TVE_EXPAND);

	//CAT II/III
	strItemCaption.LoadString(IDS_CAT);
	hTreeItem = m_wndTreeCtrl.InsertItem(strItemCaption);
	strItemCaption.LoadString(IDS_NOFREEZINGPRECITITATION);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_NONFREEZINGPRECP));
	strItemCaption.LoadString(IDS_FREEZINGDRIZZLE);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_FREEZINGDRIZZLE));
	strItemCaption.LoadString(IDS_LIGHTSNOW);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_LIGHTSONW));
	strItemCaption.LoadString(IDS_MEDIUMSNOW);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_MEDIUMSNOW));
	strItemCaption.LoadString(IDS_HEAVYSNOW);
	hTreeItemChild = m_wndTreeCtrl.InsertItem(strItemCaption, hTreeItem, TVI_LAST);
	m_wndTreeCtrl.SetItemData(hTreeItemChild, (DWORD_PTR)(CAT_HEAVYSONW));
	m_wndTreeCtrl.Expand(hTreeItem, TVE_EXPAND);

	m_wndTreeCtrl.SelectItem(0);
}

void CDlgPadAissgnment::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	CStringList strList;
	strList.RemoveAll();
	ALTObject* pAltObject = NULL;
	//std::vector<ALTObject> vObject;
	//ALTObject::GetObjectList(ALT_DEICEBAY,m_nProjID,vObject);
	ALTObjectID objName;
	int nCount = (int)m_vPadList.size();
	for (int i = 0; i< nCount; i++)
	{
		pAltObject = m_vPadList.at(i).get();
		pAltObject->getObjName(objName);
		strList.AddTail(objName.GetIDString());
	}
	
	CString strCaption;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	strCaption = "Deice Pad Name";
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 150;
	lvc.csList = &strList;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(0, &lvc);

	strCaption  = "Percent";
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(1, &lvc);
}

int CDlgPadAissgnment::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_wndFlighTypeToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_TOOLBAR_FLIGHT_TYPE)||
		!m_wndFlighTypeToolbar.LoadToolBar(IDR_TOOLBAR_FLIGHT_TYPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndPadToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_TOOLBAR_PAD_ADD_DEL)||
		!m_wndPadToolbar.LoadToolBar(IDR_TOOLBAR_PAD_ADD_DEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CDlgPadAissgnment::OnInitToolbar()
{
	CRect rc;
	GetDlgItem(IDC_STATIC_FLIGHT_TYPE)->GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_wndFlighTypeToolbar.MoveWindow(&rc,true);
	m_wndFlighTypeToolbar.ShowWindow( SW_SHOW );
 	m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
 	m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,TRUE);
 	m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,TRUE);

	GetDlgItem(IDC_STATIC_PAD)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_wndPadToolbar.MoveWindow(&rc,true);
	m_wndPadToolbar.ShowWindow(SW_SHOW);
 	m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,TRUE);
 	m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,TRUE);
}

void CDlgPadAissgnment::OnPadItemAdd()
{
	//ALTObject altObject;
	//std::vector<ALTObject> vObject;
	//ALTObject::GetObjectList(ALT_DEICEBAY,m_nProjID,vObject);
	int nCount = (int)m_vPadList.size();
	if (nCount == 0)
	{
		MessageBox(_T("plz define a Pad"),"Warning",MB_OK|MB_ICONQUESTION);
	}
	else
	{
		m_wndListCtrl.EditNew();
	}
}

void CDlgPadAissgnment::OnPadItemDel()
{
	int nCurItem = m_wndListCtrl.GetCurSel();
	PadInfo* pPadInfo = NULL; 
	if( nCurItem != -1)
	{
		pPadInfo = (PadInfo*)m_wndListCtrl.GetItemData(nCurItem);
		if (pPadInfo)
		{
			m_pCurFlightType->DelItem(pPadInfo);
		}
		m_wndListCtrl.DeleteItemEx( nCurItem );
	}
}

void CDlgPadAissgnment::OnFlightTypeAdd()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);
	int nCount = m_wndListBox.GetCount();
	for (int i=0; i<nCount; i++)
	{
		CString strFlightType = _T("");
		m_wndListBox.GetText(i,strFlightType);
		if (!strFlightType.Compare(szBuffer))
		{
			MessageBox(_T("The Flight Type already exists!"));

			return;
		}
	}
	if (m_pCurWeather == NULL)
	{
		m_pCurWeather = new WeatherInfo();
		m_pCurWeather->setWeatherCondition(m_emWeather);
		m_pWeatherList->AddNewItem(m_pCurWeather);
	}

	m_pCurFlightType = m_pCurWeather->getFlightTypeByCString(szBuffer);
	if (m_pCurFlightType == NULL)
	{
		m_pCurFlightType = new FlightTypeInfo();
	}
	m_pCurFlightType->setFlightType(szBuffer);
	m_pCurWeather->AddNewItem(m_pCurFlightType);
	int nIndex = m_wndListBox.InsertString(m_wndListBox.GetCount(),szBuffer);
	m_wndListBox.SetItemData(nIndex,(DWORD_PTR)m_pCurFlightType);
}

void CDlgPadAissgnment::OnFlightTypeDel()
{
	int nItem = m_wndListBox.GetCurSel();
	m_pCurFlightType->DelAllItem();
	m_pCurWeather->DelItem(m_pCurFlightType);
	m_wndListBox.DeleteString(nItem);
	m_wndListCtrl.DeleteAllItems();
}

void CDlgPadAissgnment::OnFlightTypeEdit()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);
	int nCount = m_wndListBox.GetCount();
	for (int i=0; i<nCount; i++)
	{
		CString strFlightType = _T("");
		m_wndListBox.GetText(i,strFlightType);
		if (!strFlightType.Compare(szBuffer))
		{
			MessageBox(_T("The Flight Type already exists!"));

			return;
		}
	}
	m_pCurFlightType->setFlightType(szBuffer);
	DisplayFlightType(m_emWeather);
}
void CDlgPadAissgnment::OnTvnSelchangedTreeWeather(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	m_wndListBox.ResetContent();
	m_wndListCtrl.DeleteAllItems();
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	WEATHERCONDITION weatherCondtion = (WEATHERCONDITION)m_wndTreeCtrl.GetItemData(hItem);

	switch(weatherCondtion) 
	{
	case VFR_NONFREEZINGPRECP:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(VFR_NONFREEZINGPRECP);
			m_emWeather = VFR_NONFREEZINGPRECP;
		}
		break;

	case VFR_FREEZINGDRIZZLE:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(VFR_FREEZINGDRIZZLE);
			m_emWeather = VFR_FREEZINGDRIZZLE;
		}
		break;

	case IFR_NONFREEZINGPRECP:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(IFR_NONFREEZINGPRECP);
			m_emWeather = IFR_NONFREEZINGPRECP;
		}
		break;

	case IFR_FREEZINGDRIZZLE:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(IFR_FREEZINGDRIZZLE);
			m_emWeather = IFR_FREEZINGDRIZZLE;
		}
		break;

	case IFR_LIGHTSONW:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(IFR_LIGHTSONW);
			m_emWeather = IFR_LIGHTSONW;
		}
		break;

	case IFR_MEDIUMSNOW:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(IFR_MEDIUMSNOW);
			m_emWeather = IFR_MEDIUMSNOW;
		}
		break;

	case IFR_HEAVYSONW:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(IFR_HEAVYSONW);
			m_emWeather = IFR_HEAVYSONW;
		}
		break;

	case CAT_NONFREEZINGPRECP:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(CAT_NONFREEZINGPRECP);
			m_emWeather = CAT_NONFREEZINGPRECP;
		}
		break;

	case CAT_FREEZINGDRIZZLE:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(CAT_FREEZINGDRIZZLE);
			m_emWeather = CAT_FREEZINGDRIZZLE;
		}
		break;

	case CAT_LIGHTSONW:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(CAT_LIGHTSONW);
			m_emWeather = CAT_LIGHTSONW;
		}
		break;

	case CAT_MEDIUMSNOW:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(CAT_MEDIUMSNOW);
			m_emWeather = CAT_MEDIUMSNOW;
		}
		break;

	case CAT_HEAVYSONW:
		{
			m_pCurWeather = m_pWeatherList->getItemByCondition(CAT_HEAVYSONW);
			m_emWeather = CAT_HEAVYSONW;
		}
		break;

	default:
		{
			m_pCurWeather = NULL;
			m_emWeather = DEFAULT_WEATHERCONDITION;
		}
	}
	
	if (m_pCurWeather)
	{
		DisplayFlightType(m_emWeather);
	}
	m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
	m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,FALSE);
	m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,FALSE);
	m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,FALSE);
	m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);
	m_wndListCtrl.EnableWindow(FALSE);
	CheckRadioButton(IDC_RADIO_GATE, IDC_RADIO_DEICE, IDC_RADIO_GATE );
}

void CDlgPadAissgnment::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	OnInitPadValue();
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pWeatherList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgPadAissgnment::OnLbnSelchangeListFlighttype()
{
	// TODO: Add your control notification handler code here
	int nCurItem = m_wndListBox.GetCurSel();
	CString strFlightType = _T("");
	m_wndListBox.GetText(nCurItem,strFlightType);
	m_pCurFlightType = m_pCurWeather->getFlightTypeByCString(strFlightType);
	if (m_bEnable)
	{
		DisplayPadInfo(strFlightType);
		OnInitFlightTypeToolbarStatus();
	}
	else
	{
		m_pCurFlightType->DelAllItem();	
	}

	if (nCurItem == 0)
	{
		m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,FALSE);
		m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,FALSE);
		if (!m_bEnable)
		{
			m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,TRUE);
		}
		if (m_pCurFlightType->getCount() == 0)
		{
			CheckRadioButton(IDC_RADIO_GATE,IDC_RADIO_DEICE,IDC_RADIO_GATE);
			m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,FALSE);
			m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);
			m_wndListCtrl.EnableWindow(FALSE);
		}
	}
	else
	{
		if (nCurItem == LB_ERR)
		{
			m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,FALSE);
			m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);
		}
		else
		{
			if (!m_bEnable)
			{
				m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,TRUE);
				if (m_wndListCtrl.GetCurSel() == LB_ERR)
				{
					m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);
				}
				else
				{
					m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,TRUE);
				}
			}
			else
			{
				if (m_pCurFlightType->getCount() == 0)
				{
					CheckRadioButton(IDC_RADIO_GATE,IDC_RADIO_DEICE,IDC_RADIO_GATE);
					m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,FALSE);
					m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);
					m_wndListCtrl.EnableWindow(FALSE);
				}
				else
				{
					m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,TRUE);
					m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);
				}
			}
		}
	}
}

void CDlgPadAissgnment::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	try
	{
		CADODatabase::BeginTransaction() ;
		m_pWeatherList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	OnOK();
}

void CDlgPadAissgnment::OnInitPadValue()
{
	CString strPadName = _T("");
	CString strPadPercent = _T("");
	int nObjectID;
	int nCurItem = m_wndListCtrl.GetItemCount();
	ALTObject* pAltObject = NULL;
	//std::vector<ALTObject> vObject;
	//ALTObject::GetObjectList(ALT_DEICEBAY,m_nProjID,vObject);
	ALTObjectID objName;
	int nCount = (int)m_vPadList.size();
	for (int k = 0;k < nCurItem; k++)
	{
		strPadName = m_wndListCtrl.GetItemText(k,0);
		strPadPercent = m_wndListCtrl.GetItemText(k,1);
		for (int i = 0; i< nCount; i++)
		{
			pAltObject = m_vPadList.at(i).get();
			pAltObject->getObjName(objName);
			if (strPadName.CompareNoCase(objName.GetIDString()) == 0)
			{
				nObjectID = pAltObject->getID();
				break;
			}
		}
		m_pPadInfo = (PadInfo*)m_wndListCtrl.GetItemData(k);
		if (m_pPadInfo == NULL)
		{
			m_pPadInfo = new PadInfo();
			m_pPadInfo->setObjectID(nObjectID);
			m_pPadInfo->setAissgnmentPercent(atof(strPadPercent));
			m_pCurFlightType->AddNewItem(m_pPadInfo);
		}
		else
		{
			m_pPadInfo->setObjectID(nObjectID);
			m_pPadInfo->setAissgnmentPercent(atof(strPadPercent));
		}
	}
	int n = m_pCurFlightType->getCount();
}

void CDlgPadAissgnment::DisplayPadInfo(CString& strFlightType)
{
	FlightTypeInfo* pFlightType = NULL;
	pFlightType = m_pCurWeather->getFlightTypeByCString(strFlightType);

	m_wndListCtrl.DeleteAllItems();
	CString strPercent = _T("");
	ALTObject* pAltObject = NULL;
	//std::vector<ALTObject> vObject;
	//ALTObject::GetObjectList(ALT_DEICEBAY,m_nProjID,vObject);
	ALTObjectID objName;
	if (pFlightType)
	{
		for ( int i = 0; i < pFlightType->getCount(); i++)
		{
			for (int j = 0; j < (int)m_vPadList.size(); j++)
			{
				if ((m_vPadList.at(j).get())->getID() == pFlightType->getItem(i)->getObjectID())
				{
					pAltObject = m_vPadList.at(j).get();
					break;
				}
			}
			pAltObject->getObjName(objName);
			m_wndListCtrl.InsertItem(i,objName.GetIDString());
			strPercent.Format("%.2f",pFlightType->getItem(i)->getAissgnmentPercent());
			m_wndListCtrl.SetItemText(i,1,strPercent);
			m_wndListCtrl.SetItemData(i,(DWORD_PTR)pFlightType->getItem(i));
		}
	}
}

void CDlgPadAissgnment::DisplayFlightType(WEATHERCONDITION emWeather)
{
	WeatherInfo* pWeather = NULL;
	pWeather = m_pWeatherList->getItemByCondition(emWeather);

	m_wndListBox.ResetContent();
	if (pWeather)
	{
		for (int i = 0 ; i < pWeather->getCount(); i++)
		{
			m_wndListBox.InsertString(i,pWeather->getItem(i)->getFlightType());
		}
	}
}

void CDlgPadAissgnment::OnInitFlightTypeToolbarStatus()
{
	if ( m_wndTreeCtrl.GetSelectedItem())
	{
		m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
		if (m_wndListBox.GetCurSel() == LB_ERR)
		{
			m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,FALSE);
		}
		else
		{
			m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,TRUE);
			m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,TRUE);
		}
	}
	else
	{
		m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,FALSE);
		m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,FALSE);
		m_wndFlighTypeToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,FALSE);
	}
}

void CDlgPadAissgnment::OnInitPadToolbarStatus()
{
	if (m_wndListBox.GetCurSel() == LB_ERR)
	{
		CheckRadioButton(IDC_RADIO_GATE, IDC_RADIO_DEICE, IDC_RADIO_GATE );
		m_wndListCtrl.EnableWindow(FALSE);
	}
	else
	{
		m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,TRUE);
		if (m_pCurFlightType->getCount() != 0)
		{
			CheckRadioButton(IDC_RADIO_GATE, IDC_RADIO_DEICE, IDC_RADIO_DEICE );
			m_wndListCtrl.EnableWindow(TRUE);
		}
		else
		{
			if (!m_bEnable)
			{
				CheckRadioButton(IDC_RADIO_GATE, IDC_RADIO_DEICE, IDC_RADIO_DEICE );
				m_wndListCtrl.EnableWindow(TRUE);
				if (m_wndListCtrl.GetCurSel() == LB_ERR)
				{
					m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);
				}
				else
				{
					m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,TRUE);
				}
			}
		}
	}
}
void CDlgPadAissgnment::OnLvnItemchangedListPad(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	OnInitPadToolbarStatus();
	if (m_wndListCtrl.GetCurSel() == LB_ERR)
	{
		m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);
	}
	else
	{
		m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,TRUE);
	}
}

void CDlgPadAissgnment::OnBnClickedRadioGate()
{
	// TODO: Add your control notification handler code here
	m_bEnable = FALSE;
	m_pCurFlightType->DelAllItem();
	m_wndListCtrl.DeleteAllItems();
	m_wndListCtrl.EnableWindow(m_bEnable);
	m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,FALSE);
	m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_DEL,FALSE);
}

void CDlgPadAissgnment::OnBnClickedRadioDeice()
{
	// TODO: Add your control notification handler code here
	m_bEnable = TRUE;
	m_wndPadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_PAD_ADD,TRUE);
	m_wndListCtrl.EnableWindow(m_bEnable);
}

void CDlgPadAissgnment::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	int nCount = m_wndListCtrl.GetItemCount();
	int nSelectItem = m_wndListCtrl.GetCurSel();
	for (int i = 0; i < nCount; i++)
	{
		if (i!= nSelectItem)
		{
			if (m_wndListCtrl.GetItemText(i,0).CompareNoCase(m_wndListCtrl.GetItemText(nSelectItem,0)) == 0)
			{
				::AfxMessageBox("Plz select another Deicing Pad name");
				return;
			}
		}
	}
}