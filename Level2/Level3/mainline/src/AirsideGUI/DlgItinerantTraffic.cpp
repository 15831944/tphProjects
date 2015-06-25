#include "stdafx.h"
#include "DlgItinerantTraffic.h"
#include "DlgItinerantTrafficDefine.h"
#include ".\dlgitineranttraffic.h"
#include "..\InputAirside\ALTObjectGroup.h"
#include "..\InputAirside\ALTAirport.h"
#include "DlgItinerantFlightSchedule.h"
#include "..\InputAirside\ItinerantFlightSchedule.h"
#include "../Database/DBElementCollection_Impl.h"


static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;

IMPLEMENT_DYNAMIC(CDlgItinerantTraffic, CXTResizeDialog)
CDlgItinerantTraffic::CDlgItinerantTraffic(int nProjID, PSelectFlightType pSelectFlightType, InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgItinerantTraffic::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pSelectFlightType(pSelectFlightType)
	,m_pInputAirside(pInputAirside)
	,m_pSelectProbDistEntry(pSelectProbDistEntry)
{
	m_pItinerantFlightList = new ItinerantFlightList();
	m_pItinerantFlightSchedule = new ItinerantFlightSchedule();
}

CDlgItinerantTraffic::~CDlgItinerantTraffic()
{
	delete m_pItinerantFlightList;
	delete m_pItinerantFlightSchedule;
}

void CDlgItinerantTraffic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONTENTS, m_wndListCtrl);
}

BOOL CDlgItinerantTraffic::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	InitListCtrl();
	GetAllStand();
	m_pItinerantFlightList->SetAirportDB(m_pInputAirside->m_pAirportDB);
	m_pItinerantFlightSchedule->SetAirportDB(m_pInputAirside->m_pAirportDB);

	m_pItinerantFlightList->ReadData(m_nProjID);
	m_pItinerantFlightSchedule->ReadData(m_nProjID);

	SetListContent();
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	SetResize(IDC_STATIC_AREA, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_CONTENTS, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_VIEW_SCHEDULE, SZ_BOTTOM_LEFT , SZ_BOTTOM_LEFT);
	SetResize(IDC_UPDATE_SCHEDULE,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);

	SetResize(IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);	

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgItinerantTraffic::InitToolBar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->GetWindowRect(&rectToolBar);
	ScreenToClient(&rectToolBar);
	rectToolBar.left += 3;
	rectToolBar.top += 5;
	m_wndToolBar.MoveWindow(&rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->ShowWindow(SW_HIDE);

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_ITEM);
	toolbar.SetCmdID(1, ID_DEL_ITEM);
	toolbar.SetCmdID(2, ID_EDIT_ITEM);

	UpdateToolBar();
}

void CDlgItinerantTraffic::InitListCtrl()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.csList = &strList;

	lvc.pszText = _T("Name");
	lvc.cx = 65;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.pszText = _T("EnRoute");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.pszText = _T("Enter");
	lvc.cx = 80;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText = _T("Exit");
	lvc.cx = 80;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.pszText = _T("ArrTime Window");
	lvc.cx = 130;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.pszText = _T("DepTime Window");
	lvc.cx = 130;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(5,&lvc);

	lvc.pszText = _T("Number of flight");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(6, &lvc);


	lvc.pszText = _T("Inter-arrival Distribution(mins)");
	lvc.cx = 130;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(7, &lvc);

	lvc.pszText = _T("Inter-departure Distribution(mins)");
	lvc.cx = 130;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(8,&lvc);
}

void CDlgItinerantTraffic::UpdateToolBar()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	// if the item count > MAXITEMCOUNT, disable the new item button.
	BOOL bEnable = TRUE;
	int nItemCount = m_wndListCtrl.GetItemCount();
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM, bEnable);

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	bEnable = (pos == NULL ? FALSE : TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, bEnable);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, bEnable);
	
}

BEGIN_MESSAGE_MAP(CDlgItinerantTraffic, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DEL_ITEM, OnDeleteItem)
	ON_COMMAND(ID_EDIT_ITEM, OnEditItem)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_VIEW_SCHEDULE, OnViewSchedule)
	ON_BN_CLICKED(IDC_UPDATE_SCHEDULE,OnUpdateSchedule)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONTENTS, OnLvnItemchangedListContents)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CONTENTS, OnNMDblclkListContents)
END_MESSAGE_MAP()

// CDlgItinerantTraffic message handlers

int CDlgItinerantTraffic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndToolBar.CreateEx(this);
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT);

	return 0;
}

void CDlgItinerantTraffic::OnNewItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	ItinerantFlight* pItinerantFlight = new ItinerantFlight;
	CDlgItinerantTrafficDefine dlg(m_nProjID, m_pSelectFlightType, m_pInputAirside, m_pSelectProbDistEntry, pItinerantFlight, TRUE);
	
	if(IDOK == dlg.DoModal())
	{
		//		ItinerantFlight* pItinerantFlight = dlg.GetItinerantFlight();
		//		;
		int nIndex = m_wndListCtrl.GetItemCount();

		//
		CString strTempValue;

	//	m_wndListCtrl.InsertItinerantFlightItem(nIndex, pItinerantFlight);
		strTempValue.Format(_T("%d"),(int)(pItinerantFlight->GetEnRoute()));
		m_wndListCtrl.SetItemText(nIndex,1,strTempValue);

		int nEntryFlag = pItinerantFlight->GetEntryFlag();
		int nEntryID = pItinerantFlight->GetEntryID();
		if(nEntryFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
		{
			for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
			{
				if(m_vWaypoint[i].getID() == nEntryID)
				{
					CString strEntry = m_vWaypoint[i].GetObjNameString();
					strTempValue.Format("%s", strEntry);
					break;
				}
			}	
		}
		else
		{
			ALTObjectGroup altObjGroup;
			altObjGroup.ReadData(nEntryID);	
			strTempValue = altObjGroup.getName().GetIDString();
		}
		m_wndListCtrl.SetEntryValue(strTempValue);


		int nExitFlag = pItinerantFlight->GetExitFlag();
		int nExitID = pItinerantFlight->GetExitID();
		if(nExitFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
		{
			for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
			{
				if(m_vWaypoint[i].getID() == nExitID)
				{
					CString strExit = m_vWaypoint[i].GetObjNameString();
					strTempValue.Format("%s",strExit);
					break;
				}
			}
		}
		else
		{
			ALTObjectGroup altObjGroup;
			altObjGroup.ReadData(nExitID);	
			strTempValue = altObjGroup.getName().GetIDString();
		}
		m_wndListCtrl.SetExitValue(strTempValue);
		m_wndListCtrl.InsertItinerantFlightItem(nIndex,pItinerantFlight);

		//
// 		CString strTimeRange = _T(""),strDay = _T("");
// 
// 		ElapsedTime* etInsertStart = pItinerantFlight->GetArrStartTime();
// 		long lSecond = etInsertStart->asSeconds();
// 		strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
// 		strTimeRange = strDay + _T(" - ");
// 		ElapsedTime* etInsertEnd = pItinerantFlight->GetArrEndTime();
// 		if (*etInsertStart == 0L
// 			&& *etInsertEnd == 0L)
// 		{
// 			etInsertEnd->SetTime("23:59:59");
// 		}
// 		lSecond = etInsertEnd->asSeconds();
// 		strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
// 		strTimeRange += strDay;
// 		strTempValue = strTimeRange;
// 		//
// 		if (pItinerantFlight->GetEntryID() != -1)
// 		{
// 			m_wndListCtrl.SetItemText(nIndex, 4, strTempValue);
// 		}
// 
// 		etInsertStart = pItinerantFlight->GetDepStartTime();
// 		lSecond = etInsertStart->asSeconds();
// 		strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
// 		strTimeRange = strDay + _T(" - ");
// 		etInsertEnd = pItinerantFlight->GetDepEndTime();
// 		if (*etInsertStart == 0L
// 			&& *etInsertEnd == 0L)
// 		{
// 			etInsertEnd->SetTime("23:59:59");
// 		}
// 		lSecond = etInsertEnd->asSeconds();
// 		strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
// 		strTimeRange += strDay;
// 		strTempValue = strTimeRange;
// 
// 	
// 		if (pItinerantFlight->GetExitID() != -1 && !pItinerantFlight->GetEnRoute())
// 		{
// 			m_wndListCtrl.SetItemText(nIndex, 5, strTempValue);
// 		}
// 
// 
// 		strTempValue.Format("%d", pItinerantFlight->GetFltCount());
// 		m_wndListCtrl.SetItemText(nIndex, 6, strTempValue);
// 
// 		strTempValue = pItinerantFlight->GetArrDistScreenPrint();
// 		m_wndListCtrl.SetItemText(nIndex, 7, strTempValue);
// 
// 		strTempValue = pItinerantFlight->GetDepDistScreenPrint();
// 		m_wndListCtrl.SetItemText(nIndex, 8, strTempValue);

		m_pItinerantFlightList->AddNewItem(pItinerantFlight);
		
		GetDlgItem(IDC_SAVE)->EnableWindow();
		UpdateToolBar();
	}
}
void CDlgItinerantTraffic::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	size_t nIninerantCount = m_pItinerantFlightList->GetElementCount();

	CString strTempValue;
	for (size_t i = 0; i < nIninerantCount; i++)
	{
		ItinerantFlight* pItinerantFlight = m_pItinerantFlightList->GetItem(i);

		strTempValue.Format(_T("%d"),(int)(pItinerantFlight->GetEnRoute()));
		m_wndListCtrl.SetItemText((int)i,1,strTempValue);
		int nEntryFlag = pItinerantFlight->GetEntryFlag();
		int nEntryID = pItinerantFlight->GetEntryID();
		if(nEntryFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
		{
			for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
			{
				if(m_vWaypoint[i].getID() == nEntryID)
				{
					CString strEntry = m_vWaypoint[i].GetObjNameString();
					strTempValue.Format("%s",strEntry);
					break;
				}
			}
		}
		else
		{
			ALTObjectGroup altObjGroup;
			altObjGroup.ReadData(nEntryID);	
			strTempValue = altObjGroup.getName().GetIDString();
		}
		m_wndListCtrl.SetEntryValue(strTempValue);

		int nExitFlag = pItinerantFlight->GetExitFlag();
		int nExitID = pItinerantFlight->GetExitID();
		if(nExitFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
		{
			for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
			{
				if(m_vWaypoint[i].getID() == nExitID)
				{
					CString strExit = m_vWaypoint[i].GetObjNameString();
					strTempValue.Format("%s",strExit);
					break;
				}
			}
		}
		else
		{
			ALTObjectGroup altObjGroup;
			altObjGroup.ReadData(nExitID);	
			strTempValue = altObjGroup.getName().GetIDString();
		}

		m_wndListCtrl.SetExitValue(strTempValue);
		m_wndListCtrl.InsertItinerantFlightItem((int)i, pItinerantFlight);
	}
}
void CDlgItinerantTraffic::OnDeleteItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
//TODO
			m_wndListCtrl.DeleteItemEx(nItem);
			m_pItinerantFlightList->DeleteItem(nItem);
		}
	}

	GetDlgItem(IDC_SAVE)->EnableWindow();
	UpdateToolBar();
}

void CDlgItinerantTraffic::OnEditItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			//TODO
			ItinerantFlight* pItinerantFlight = m_pItinerantFlightList->GetItem(nItem);
			//-------------------
			CDlgItinerantTrafficDefine dlg(m_nProjID, m_pSelectFlightType, m_pInputAirside, m_pSelectProbDistEntry, pItinerantFlight);

			if(IDOK == dlg.DoModal())
			{
				SetListContent();

// 				m_wndListCtrl.SetItemText(nIndex, 0, pItinerantFlight->GetName());
// 
// 				strTempValue.Format(_T("%d"),(int)(pItinerantFlight->GetEnRoute()));
// 				m_wndListCtrl.SetItemText(nIndex,1,strTempValue);
// 
// 				int nEntryFlag = pItinerantFlight->GetEntryFlag();
// 				int nEntryID = pItinerantFlight->GetEntryID();
// 				if(nEntryFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
// 				{
// 					for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
// 					{
// 						if(m_vWaypoint[i].getID() == nEntryID)
// 						{
// 							CString strEntry = m_vWaypoint[i].GetObjNameString();
// 							strTempValue.Format("%s",strEntry);
// 							break;
// 						}
// 					}
// 				}
// 				else
// 				{
// 					ALTObjectGroup altObjGroup;
// 					altObjGroup.ReadData(nEntryID);	
// 					strTempValue = altObjGroup.getName().GetIDString();
// 				}
// 				m_wndListCtrl.SetItemText(nIndex, 2, strTempValue);
// 
// 				int nExitFlag = pItinerantFlight->GetExitFlag();
// 				int nExitID = pItinerantFlight->GetExitID();
// 				if(nExitFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
// 				{
// 					for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
// 					{
// 						if(m_vWaypoint[i].getID() == nExitID)
// 						{
// 							CString strExit = m_vWaypoint[i].GetObjNameString();
// 							strTempValue.Format("%s",strExit);
// 							break;
// 						}
// 					}
// 				}
// 				else
// 				{
// 					ALTObjectGroup altObjGroup;
// 					altObjGroup.ReadData(nExitID);	
// 					strTempValue = altObjGroup.getName().GetIDString();
// 				}
// 				m_wndListCtrl.SetItemText(nIndex, 3, strTempValue);
// 
// 				//
// 				CString strTimeRange = _T(""),strDay = _T("");
// 
// 				ElapsedTime* etInsertStart = pItinerantFlight->GetArrStartTime();
// 				long lSecond = etInsertStart->asSeconds();
// 				strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
// 				strTimeRange = strDay + _T(" - ");
// 				ElapsedTime* etInsertEnd = pItinerantFlight->GetArrEndTime();
// 				if (*etInsertStart == 0L
// 					&& *etInsertEnd == 0L)
// 				{
// 					etInsertEnd->SetTime("23:59:59");
// 				}
// 				lSecond = etInsertEnd->asSeconds();
// 				strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
// 				strTimeRange += strDay;
// 				strTempValue = strTimeRange;
// 				//
// 				if (pItinerantFlight->GetEntryID() != -1)
// 				{
// 					m_wndListCtrl.SetItemText(nIndex, 4, strTempValue);
// 				}
// 				else
// 				{
// 					m_wndListCtrl.SetItemText(nIndex,4,_T(""));
// 				}
// 				etInsertStart = pItinerantFlight->GetDepStartTime();
// 				lSecond = etInsertStart->asSeconds();
// 				strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
// 				strTimeRange = strDay + _T(" - ");
// 				etInsertEnd = pItinerantFlight->GetDepEndTime();
// 				if (*etInsertStart == 0L
// 					&& *etInsertEnd == 0L)
// 				{
// 					etInsertEnd->SetTime("23:59:59");
// 				}
// 				lSecond = etInsertEnd->asSeconds();
// 				strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
// 				strTimeRange += strDay;
// 				strTempValue = strTimeRange;
// 				if(pItinerantFlight->GetEnRoute())
// 				{
// 					m_wndListCtrl.SetItemText(nIndex,5,_T(""));
// 				}
// 				else
// 				{
// 					if (pItinerantFlight->GetExitID() != -1)
// 					{
// 						m_wndListCtrl.SetItemText(nIndex, 5, strTempValue);
// 					}
// 					else
// 					{
// 						m_wndListCtrl.SetItemText(nIndex,5,_T(""));
// 					}
// 				}
// 				
// 				strTempValue.Format("%d", pItinerantFlight->GetFltCount());
// 				m_wndListCtrl.SetItemText(nIndex, 6, strTempValue);
// 				
// 				if (pItinerantFlight->GetEntryID() != -1)
// 				{
// 					strTempValue = pItinerantFlight->GetArrDistScreenPrint();
// 					m_wndListCtrl.SetItemText(nIndex, 7, strTempValue);
// 				}
// 				else
// 				{
// 					m_wndListCtrl.SetItemText(nIndex,7,_T(""));
// 				}
// 
// 				if (pItinerantFlight->GetExitID() != -1)
// 				{
// 					strTempValue = pItinerantFlight->GetDepDistScreenPrint();
// 					m_wndListCtrl.SetItemText(nIndex,8,strTempValue);
// 				}
// 				else
// 				{
// 					m_wndListCtrl.SetItemText(nIndex,8,_T(""));
// 				}
 			}
 			//-------------------
 		}

	}

	GetDlgItem(IDC_SAVE)->EnableWindow();
	UpdateToolBar();
}

void CDlgItinerantTraffic::OnViewSchedule()
{
	m_pItinerantFlightSchedule->DeleteData();
	m_pItinerantFlightSchedule->ReadData(m_nProjID);
	CDlgItinerantFlightSchedule dlg(m_pItinerantFlightSchedule,m_vWaypoint);
	dlg.DoModal();
}

void CDlgItinerantTraffic::OnUpdateSchedule()
{
	m_pItinerantFlightSchedule->DeleteData();
	GenerateItinerantFlightSchedule();
	CDlgItinerantFlightSchedule dlg(/*m_pItinerantFlightList, */m_pItinerantFlightSchedule, m_vWaypoint);
	dlg.DoModal();
}
void CDlgItinerantTraffic::OnBnClickedOk()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pItinerantFlightSchedule->DeleteData();
		GenerateItinerantFlightSchedule();

		m_pItinerantFlightList->SaveData(m_nProjID);
		m_pItinerantFlightSchedule->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation();
	}

	
	OnOK();
}

void CDlgItinerantTraffic::OnBnClickedCancel()
{
	m_pItinerantFlightSchedule->DeleteData();
	GenerateItinerantFlightSchedule();

	m_pItinerantFlightList->SaveData(m_nProjID);
	OnCancel();
}

void CDlgItinerantTraffic::OnBnClickedSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pItinerantFlightList->SaveData(m_nProjID);
		m_pItinerantFlightSchedule->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation();
	}

	OnOK();
}

void CDlgItinerantTraffic::OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateToolBar();
	*pResult = 0;
}

void CDlgItinerantTraffic::OnNMDblclkListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	OnEditItem();
	*pResult = 0;
}
void CDlgItinerantTraffic::GetAllStand()
{
	ALTObject::GetObjectList(ALT_WAYPOINT,m_nProjID,m_vWaypoint);
}

void CDlgItinerantTraffic::GenerateItinerantFlightSchedule()
{
	int nItinerantFltID = 0;
	size_t nCount = m_pItinerantFlightList->GetElementCount();
	for (size_t i = 0; i < nCount; i++)
	{
		ItinerantFlight* pItem = m_pItinerantFlightList->GetItem( i );

		ProbabilityDistribution *pProbDist = pItem->GetArrProbDistribution();
		double dArrInteValue = pProbDist->getRandomValue() *60;

		pProbDist = pItem->GetDepProbDistribution();
		double dDepInteValue = pProbDist->getRandomValue() * 60;

		std::vector<ALTObject> vEntryObject;
		std::vector<ALTObject> vExitObject;

		ALTObjectGroup objectGroupEntry;
		objectGroupEntry.ReadData(pItem->GetEntryID());
		if(-1 != objectGroupEntry.getID() 
			&& -1 != objectGroupEntry.GetProjID() 
			&& objectGroupEntry.getType() != ALT_UNKNOWN)
			objectGroupEntry.GetObjects(vEntryObject);

		ALTObjectGroup objectGroupExit;
		objectGroupExit.ReadData(pItem->GetExitID());
		if(-1 != objectGroupExit.getID() 
			&& -1 != objectGroupExit.GetProjID() 
			&& objectGroupExit.getType() != ALT_UNKNOWN)
			objectGroupExit.GetObjects(vExitObject);
		
		int nFltCount = pItem->GetFltCount();
		for (int j = 0; j < nFltCount; j++)
		{
			nItinerantFltID++;
			ItinerantFlightScheduleItem* pNewItem = new ItinerantFlightScheduleItem;
		//	pNewItem->SetFltType( pItem->GetFltType() );
			pNewItem->setACType(pItem->GetACType());
			pNewItem->setAirline( pItem->GetName() );
			pNewItem->SetEnRoute(pItem->GetEnRoute());
			CString strTemp = _T("");
			strTemp.Format(_T("%d"),nItinerantFltID);
			if (pItem->GetEntryID() != -1)
			{
				pNewItem->setFlightID( strTemp ,'A');
			}
			if (pItem->GetExitID() != -1)
			{
				pNewItem->setFlightID(strTemp,'D');
			}
			size_t nEntrySize = vEntryObject.size();
			int nEntryID = nEntrySize > 0 ? vEntryObject[rand()%nEntrySize].getID() : pItem->GetEntryID();

			size_t nExitSize = vExitObject.size();
			int nExitID = nExitSize > 0 ? vExitObject[rand()%nExitSize].getID() : pItem->GetExitID();

			pNewItem->SetEntryID( nEntryID );
			pNewItem->SetEntryFlag( pItem->GetEntryFlag() );
			pNewItem->SetExitID( nExitID );
			pNewItem->SetExitFlag( pItem->GetExitFlag() );
			pNewItem->setOrigin(pItem->GetOrig());
			pNewItem->setDestination(pItem->GetDest());

			//Arrival time
			ElapsedTime arrTime = *pItem->GetArrStartTime() + ElapsedTime(dArrInteValue)*(long(j+1));
			if (arrTime < 0L)
				arrTime = *pItem->GetArrStartTime();

			pNewItem->setArrTime( arrTime);

			ElapsedTime depTime = *pItem->GetDepStartTime() + ElapsedTime(dDepInteValue)*(long(j+1));
			if (depTime < 0L)
			{
				depTime = *pItem->GetDepStartTime();
			}
			pNewItem->setDepTime(depTime);
			//make sure in time window
			if (pNewItem->getArrTime() > pNewItem->getDepTime() || pNewItem->getArrTime() == pNewItem->getDepTime())
			{
				pNewItem->setDepTime(pNewItem->getArrTime()+ElapsedTime(dArrInteValue)*(long(j+1)));
			}
			m_pItinerantFlightSchedule->AddNewItem( pNewItem );
		}
	}
}