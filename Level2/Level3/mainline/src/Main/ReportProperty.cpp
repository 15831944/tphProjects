// ReportProperty.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ReportProperty.h"
#include "ProcesserDialog.h"
#include "PassengerTypeDialog.h"
#include "DlgSelectArea.h"
#include "../common/elaptime.h"
#include "../compare/ReportDef.h"
#include "../common/ProjectManager.h"
#include "../common/SimAndReportManager.h"
#include "../inputs/AreasPortals.h"
#include "../engine/terminal.h"
#include "../compare/ModelToCompare.h"
#include <io.h>
#include <Inputs/PROCIDList.h>

#define TIMETOHOURDIV	360000
#define TIMETOMINDIV		6000
#define TIMETOSECDIV		100


/////////////////////////////////////////////////////////////////////////////
// CReportProperty dialog

CRect CReportProperty::m_rcWindow[];

CReportProperty::CReportProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CReportProperty::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReportProperty)
	m_strName = _T("");
	m_tEnd = COleDateTime::GetCurrentTime();
	m_tStart = COleDateTime::GetCurrentTime();
	m_dStart = COleDateTime::GetCurrentTime();
	m_dEnd = COleDateTime::GetCurrentTime();
	m_strArea = _T("");
	//}}AFX_DATA_INIT
	long nTime = m_tStart.GetHour()*TIMETOHOURDIV+m_tStart.GetMinute()*TIMETOMINDIV+m_tStart.GetSecond()*TIMETOSECDIV;
	m_tStart = GetOleDateTime(nTime);
	m_tEnd=GetOleDateTime(nTime);

	m_pModelsManager = NULL;
	m_reportsManager = NULL;
	m_strProjName = "";

//	m_pTerminal = NULL;

	m_strOldName = "";

	m_enumOldType = ENUM_PAXCOUNT_REP;

	m_bShowProcessor = TRUE;
}
//CReportProperty::~CReportProperty()
//{
//
//}
void CReportProperty::DeleteTreeData(CTreeCtrl& tree)
{
	HTREEITEM hRoot = tree.GetRootItem();
	while (hRoot)
	{
		ItemData *pItemData = (ItemData *)tree.GetItemData(hRoot);
		delete pItemData;

		DeleteTreeSubItemData(tree,hRoot);

		hRoot = tree.GetNextSiblingItem(hRoot);
	}

}
void CReportProperty::DeleteTreeSubItemData(CTreeCtrl& tree,HTREEITEM hItem)
{
	HTREEITEM hChildItem = tree.GetChildItem(hItem);
	while(hChildItem)
	{
		ItemData *pItemData = (ItemData *)tree.GetItemData(hChildItem);
		delete pItemData;
		DeleteTreeSubItemData(tree,hChildItem);

		hChildItem = tree.GetNextSiblingItem(hChildItem);	
	}

}
void CReportProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportProperty)
	//DDX_Control(pDX, IDC_COMBO_REPORT_AREA, m_cmbArea);
	DDX_Control(pDX, IDC_COMBO_ANIMDATEENDINDEX, m_cmbEndDateIndex);
	DDX_Control(pDX, IDC_COMBO_ANIMDATESTARTINDEX, m_cmbStartDateIndex);
	DDX_Control(pDX, IDC_DTANIMDATEEND, m_dtCtrlEndDate);
	DDX_Control(pDX, IDC_DTANIMDATESTART, m_dtCtrlStartDate);
	DDX_Control(pDX, IDC_COMBO_REPORTTYPE, m_nReportType);
	DDX_Control(pDX, IDC_DTANIMTIMEINTERVAL, m_tInterval);
	DDX_Control(pDX, IDC_STATICPROCESSORTYPE, m_toolbarProcessorType);
	DDX_Control(pDX, IDC_STATICPESSENGERTYPE, m_toolbarPessengerType);
	//DDX_Control(pDX, IDC_LIST1, m_listPessengerType);
	//DDX_Control(pDX, IDC_LIST2, m_listProcessorType);
	DDX_Control(pDX, IDC_TREE_PROCFROMTO, m_treeProcFromTo);
	DDX_Control(pDX, IDC_TREE_PAXTYPE, m_treePaxType);
	DDX_Control(pDX, IDC_TREE_PROCS, m_treeProcType);
	DDX_Control(pDX, IDC_TREE_AREA, m_treeArea);

	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_DateTimeCtrl(pDX, IDC_DTANIMTIMEEND, m_tEnd);
	DDX_DateTimeCtrl(pDX, IDC_DTANIMTIMESTART, m_tStart);
	DDX_DateTimeCtrl(pDX, IDC_DTANIMDATESTART, m_dStart);
	DDX_DateTimeCtrl(pDX, IDC_DTANIMDATEEND, m_dEnd);
	//DDX_CBString(pDX, IDC_COMBO_REPORT_AREA, m_strArea);
	DDX_Control(pDX, IDC_EDIT_INTERVAL, m_editInterval);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReportProperty, CDialog)
	//{{AFX_MSG_MAP(CReportProperty)
	ON_WM_CREATE()
	ON_COMMAND(ID_BUTTONADDFIR,OnPessengerTypeAdd)
	ON_COMMAND(ID_BUTTONADDSEC,OnProcessorTypeAdd)
	ON_COMMAND(ID_BUTTONADDFROMTO,OnProcessorFromToAdd)
	ON_COMMAND(ID_TOOLBAR_NEWAREA,OnAddArea)
	ON_COMMAND(ID_TOOLBAR_DELAREA,OnDelArea)
	ON_COMMAND(ID_BUTTONDELETEFIR,OnPessengerTypeDelete)
	ON_COMMAND(ID_BUTTONDELETESEC,OnProcessorTypeDelete)
	ON_COMMAND(ID_BUTTONDELETEFROMTO,OnProcessorFromToDelete)
	//ON_UPDATE_COMMAND_UI(ID_BUTTONADDFIR,OnUpdateUIPaxTypeAdd)
	//ON_UPDATE_COMMAND_UI(ID_BUTTONADDSEC,OnUpdateUIProcTypeAdd)
	//ON_UPDATE_COMMAND_UI(ID_BUTTONADDFROMTO,OnUpdateUIFromToAdd)
	//ON_UPDATE_COMMAND_UI(ID_TOOLBAR_NEWAREA,OnUpdateUIAreaAdd)
	//ON_UPDATE_COMMAND_UI(ID_TOOLBAR_DELAREA,OnUpdateUIAreaDel)
	//ON_UPDATE_COMMAND_UI(ID_BUTTONDELETEFIR,OnUpdateUIPaxTypeDel)
	//ON_UPDATE_COMMAND_UI(ID_BUTTONDELETESEC,OnUpdateUIProcTypeDel)
	//ON_UPDATE_COMMAND_UI(ID_BUTTONDELETEFROMTO,OnUpdateUIFromToDel)



	ON_CBN_SELCHANGE(IDC_COMBO_REPORTTYPE, OnSelchangeComboReporttype)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PAXTYPE, OnTvnSelchangedTreePaxtype)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCS, OnTvnSelchangedTreeProcs)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCFROMTO, OnTvnSelchangedTreeProcfromto)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_AREA, OnTvnSelchangedTreeArea)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportProperty message handlers
void CReportProperty::SetManager(CModelsManager* pModelManager,CReportsManager* pReportManager)
{
	m_pModelsManager = pModelManager;
	m_reportsManager = pReportManager;
}

BOOL CReportProperty::CheckParameter()
{
	std::vector<CModelToCompare *> vModels = m_pModelsManager->GetModelsList();
	CReportParamToCompare pReportParam = m_reportToCompare.GetParameter();
	std::vector<CModelParameter> vModelParam;
	pReportParam.GetModelParameter(vModelParam);
	std::vector<CModelToCompare *>::size_type i = 0;
	for (;i<vModels.size(); ++ i)
	{
		CString strModelName = vModels.at(i)->GetUniqueName();
		std::vector<CModelParameter>::size_type j = 0;
		
		bool bFind = false;
		for (; j< vModelParam.size(); ++ j)
		{
			if(strModelName.CompareNoCase(vModelParam.at(j).GetModelUniqueName()) == 0)
			{
				m_vModelParam.push_back(vModelParam.at(j));
				bFind = true;
				break;
			}
		}
		if (bFind == false)
		{
			CModelParameter modelParam;
			modelParam.SetModelUniqueName(strModelName);
			modelParam.InitDefaultParameter(m_strProjName,vModels.at(i));
			m_vModelParam.push_back(modelParam);
		}
	}
	return TRUE;
}
BOOL CReportProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//check the report parameter and models
	CheckParameter();

	InitToolBar();

	COleDateTime oleInterval;
	m_tInterval.SetFormat("HH:mm"); 
	oleInterval.SetTime(1, 0, 0);
	m_tInterval.SetTime(oleInterval); 

	m_editInterval.SetWindowText(_T("1"));
	for (int i = 0; i < sizeof(s_szReportCategoryName) / sizeof(s_szReportCategoryName[0]); i++)
	{
		m_nReportType.AddString(s_szReportCategoryName[i]);
	}


	ElapsedTime time;
	bool		bAbsDate;
	int			nDayIndex;
	COleDateTime	dtStart, dtEnd, dtTime;
	ASSERT((int)m_pModelsManager->GetModelsList().size() > 0);
	Terminal *pTerminal = m_pModelsManager->GetModelsList().at(0)->GetTerminal();
	CSimAndReportManager* pSim =pTerminal ->GetSimReportManager();
	m_sdStart = pSim->GetStartDate();
	time = pSim->GetUserEndTime();
	m_sdStart.GetDateTime(time, bAbsDate, dtEnd, nDayIndex, dtTime);
	for (int i = 0; i <= nDayIndex; i++)
	{
		char strDay[64];
		sprintf(strDay, "Day %d", i + 1);
		m_cmbStartDateIndex.InsertString(i, strDay);
		m_cmbEndDateIndex.InsertString(i, strDay);
	}
	if (bAbsDate)
		m_dEnd = dtEnd;
	else
		m_cmbEndDateIndex.SetCurSel(nDayIndex);
	m_tEnd = dtTime;

	time = pSim->GetUserStartTime();
	m_sdStart.GetDateTime(time, bAbsDate, dtStart, nDayIndex, dtTime);
	if (bAbsDate)
		m_dStart = dtStart;
	else
		m_cmbStartDateIndex.SetCurSel(nDayIndex);
	m_tStart = dtTime;
	UpdateData(FALSE);
	

	//CAreas* pAreas = m_pTerminal->m_pAreas;
	//int nCount = pAreas->m_vAreas.size();
	//m_cmbArea.ResetContent();
	//for(i=0; i<nCount; i++ )
	//{		
	//	CString csStr = pAreas->m_vAreas[i]->name;
	//	int nIdx = m_cmbArea.AddString( csStr );
	//	m_cmbArea.SetItemData( nIdx, i );
	//}

	/*
	m_tStart.SetTime(0, 0, 0);
	m_tEnd.SetTime(23, 59, 59);*/


	Init();

	GetDlgItem(IDC_PROC_TYPE)->GetWindowRect(m_rcWindow[0]);
	m_ToolBar2.GetWindowRect(m_rcWindow[1]);
	m_treeProcType.GetWindowRect(m_rcWindow[2]);
	m_treePaxType.GetWindowRect(m_rcWindow[3]);
	for (int i = 0; i < 4; i++)
		ScreenToClient(m_rcWindow[i]);

	ArrangeControls();

	m_strOldName = m_strName;

	UpdateData(FALSE);

	return TRUE;
}

void CReportProperty::InitToolBar()
{
	m_ToolBar.GetToolBarCtrl().SetCmdID(0, ID_BUTTONADDFIR);
	m_ToolBar.GetToolBarCtrl().SetCmdID(1, ID_BUTTONDELETEFIR);

	m_ToolBar2.GetToolBarCtrl().SetCmdID(0, ID_BUTTONADDSEC);
	m_ToolBar2.GetToolBarCtrl().SetCmdID(1, ID_BUTTONDELETESEC);

	m_ToolBar3.GetToolBarCtrl().SetCmdID(0, ID_BUTTONADDFROMTO);
	m_ToolBar3.GetToolBarCtrl().SetCmdID(1, ID_BUTTONDELETEFROMTO);

    CRect rc;
	m_toolbarPessengerType.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarPessengerType.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTONADDFIR );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTONDELETEFIR );

	m_toolbarProcessorType.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar2.MoveWindow(&rc);
	m_ToolBar2.ShowWindow(SW_SHOW);
	m_toolbarProcessorType.ShowWindow(SW_HIDE);
	m_ToolBar2.GetToolBarCtrl().EnableButton( ID_BUTTONADDSEC );
	m_ToolBar2.GetToolBarCtrl().EnableButton( ID_BUTTONDELETESEC );

	m_toolbarProcessorType.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar3.MoveWindow(&rc);
	m_ToolBar3.ShowWindow(SW_SHOW);
	m_toolbarProcessorType.ShowWindow(SW_HIDE);
	m_ToolBar3.GetToolBarCtrl().EnableButton( ID_BUTTONADDFROMTO );
	m_ToolBar3.GetToolBarCtrl().EnableButton( ID_BUTTONDELETEFROMTO );

	m_toolbarProcessorType.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar4.MoveWindow(&rc);
	m_ToolBar4.ShowWindow(SW_SHOW);
	m_toolbarProcessorType.ShowWindow(SW_HIDE);
	m_ToolBar4.GetToolBarCtrl().EnableButton( ID_TOOLBAR_NEWAREA );
	m_ToolBar4.GetToolBarCtrl().EnableButton( ID_TOOLBAR_DELAREA );

}

int CReportProperty::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_PIPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	if (!m_ToolBar2.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_TOP
		|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC)||
		!m_ToolBar2.LoadToolBar(IDR_TOOLBAR_PIPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_ToolBar3.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_TOP
		|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC)||
		!m_ToolBar3.LoadToolBar(IDR_TOOLBAR_PIPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_ToolBar4.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_TOP
		|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC)||
		!m_ToolBar4.LoadToolBar(IDR_TOOLBAR_CP_AREA))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CReportProperty::OnOK() 
{
	// TODO: Add extra validation here
//*frank test
/*	CReportToCompare report;
	CReportParamToCompare param;
	ElapsedTime tStart;
	tStart.set(0, 0, 0);
	ElapsedTime tEnd;
	tEnd.set(23, 59, 59);

	for (int i = 0; i < 1; i++)
	{
		report.SetCategory(i);
		param.SetReportParamter(tStart, tEnd, i, i, i, i, "ReportName");
		report.SetParameter(param);
		m_reportsManager.AddReport(report);
	}
//------------------------
*/
	UpdateData();

	if(m_strName.IsEmpty())
	{
		AfxMessageBox("Please enter an report name!", MB_ICONEXCLAMATION|MB_OK);
		return;
	}


	if(m_nReportType.GetCurSel() == CB_ERR)
	{
		AfxMessageBox("Please select an report type!", MB_ICONEXCLAMATION|MB_OK);
		return;
	}

	if (m_nReportType.GetCurSel() == 2 || m_nReportType.GetCurSel() == 0 )
	{
		//if (m_listProcessorType.GetCount() > 1)
		//{
		//	AfxMessageBox(_T("Only one processor allowed."));
		//	return;
		//}

		//CString strText;
		//m_listProcessorType.GetText(0, strText);
		//if (strText == "All Processors")
		//{
		//	AfxMessageBox(_T("All Processor is not allowed here"));
		//	return;
		//}
	}


	/*
	if(m_tStart >= m_tEnd)
	{
		AfxMessageBox("Start time must less than end time!", MB_ICONEXCLAMATION|MB_OK);
		return;
	}*/
	
	//it is modifying if the new name and the old name is the same, otherwise is adding
	if(m_strName != m_strOldName)
	{
		//if report already exists, warning and return
		std::vector<CReportToCompare>& report = m_reportsManager->GetReportsList();
		std::vector<CReportToCompare>::size_type size;
		for(size = 0; size < report.size(); size++)
		{
			if(m_strName.CompareNoCase(report[size].GetName()) == 0)
			{
				MessageBox("A report with the specified name already exists!", "Warning");
				return;
			}
		}
		
	}


	m_reportToCompare.SetName(m_strName);

	ENUM_REPORT_TYPE nReportType;
	switch (m_nReportType.GetCurSel())
	{
	case 0:
		nReportType = ENUM_QUEUELENGTH_REP;
		break;
	case 1:
		nReportType = ENUM_QUEUETIME_REP;
		break;
	case 2:
		nReportType = ENUM_THROUGHPUT_REP;
		break;
	case 3:
		nReportType = ENUM_PAXDENS_REP;
		break;
	case 4:
		nReportType = ENUM_PAXCOUNT_REP;
		break;
	case 5:
		nReportType = ENUM_ACOPERATION_REP;
		break;
	case 6:
		nReportType = ENUM_DURATION_REP;
		break;
	case 7:
		nReportType = ENUM_DISTANCE_REP;
		break;
	}
	if(m_enumOldType != nReportType)
	{
		for(int i = 0; i < (int)m_reportsManager->GetReportsList().size(); i++)
		{
			if(m_reportsManager->GetReport(i).GetCategory() == nReportType)
			{
				MessageBox("This category report already exists, please add other category report!");
				return;
			}
		}
	}

	m_reportToCompare.SetCategory(nReportType);
	if ((nReportType == 3 &&m_nReportType.GetCurSel() != 6) && m_strArea.IsEmpty())
	{
		AfxMessageBox(_T("Please Select An Area."));
		return;
	}
	if(nReportType == ENUM_PAXDENS_REP && !CheckUserHasAssignedArea())
	{
		if(AfxMessageBox("No Area Is Selected, Continue?", MB_YESNO|MB_ICONSTOP) == IDNO)
		{
			return;
		}
	}
	CReportParamToCompare param;
	ElapsedTime et;
	//et.set(m_tStart.GetHour(), m_tStart.GetMinute(), m_tStart.GetSecond());
	//param.SetStartTime(et);

	//et.set(m_tEnd.GetHour(), m_tEnd.GetMinute(), m_tEnd.GetSecond());
	//param.SetEndTime(et);

	int nDayIndex = -1;
	if (m_sdStart.IsAbsoluteDate())
	{
		m_sdStart.GetElapsedTime(true, m_dStart, nDayIndex, m_tStart, et);	
		param.SetStartTime(et);
		m_sdStart.GetElapsedTime(true, m_dEnd, nDayIndex, m_tEnd, et);	
		param.SetEndTime( et );
	}
	else
	{
		nDayIndex = m_cmbStartDateIndex.GetCurSel();
		m_sdStart.GetElapsedTime(false,m_dStart,nDayIndex,m_tStart,et);	
		param.SetStartTime( et );
		
		nDayIndex = m_cmbEndDateIndex.GetCurSel();
		m_sdStart.GetElapsedTime(false,m_dEnd,nDayIndex,m_tEnd,et);	
		param.SetEndTime( et );		
	}

	COleDateTime dt;
	m_tInterval.GetTime(dt);
	et.set(dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	param.SetInterval(et);
	
	CString strIntervalText = _T("");
	m_editInterval.GetWindowText(strIntervalText);
	param.SetInterval(::atol(strIntervalText));

	param.SetModelParameter(m_vModelParam);

	//if (!m_strArea.IsEmpty())
	//	param.SetArea(m_strArea);

	//std::vector<CMobileElemConstraint> mobList;
	//for( int i=0; i<m_paxConst.getCount(); i++)
	//{
	//	mobList.push_back( *(m_paxConst.getConstraint(i)));
	//}
	//param.SetPaxType(mobList);

	//if(nReportType == ENUM_QUEUETIME_REP && m_vProcList.size() <= 0)
	//{
	//	ProcessorID id;
	//	id.SetStrDict(m_pTerminal->inStrDict);
	//	id.setID("All Processors");
	//	m_vProcList.push_back(id);
	//}

	//param.SetProcessorID(m_vProcList);
	//param.SetFromToProcs(m_fromToProcs);

	m_reportToCompare.SetParameter(param);


//clear the data
	DeleteTreeData(m_treeProcType);
	DeleteTreeData(m_treeProcFromTo);
	DeleteTreeData(m_treeArea);
	DeleteTreeData(m_treePaxType);

	CDialog::OnOK();
}


COleDateTime CReportProperty::GetOleDateTime(long _nTime)
{
    COleDateTime ret = COleDateTime::GetCurrentTime();
	int nHour = _nTime/TIMETOHOURDIV;
	int nMin = (_nTime-nHour*TIMETOHOURDIV)/TIMETOMINDIV;
	int nSec = (_nTime-nHour*TIMETOHOURDIV-nMin*TIMETOMINDIV)/TIMETOSECDIV;
	ret.SetTime(nHour,nMin,nSec);
	return ret;
}

void CReportProperty::OnPessengerTypeAdd()
{
	/*CPassengerTypeDialog dlg( this );
	if( dlg.DoModal() == IDOK )
	{
		//PassengerConstraint paxSelection;
		//paxSelection = dlg.GetPaxSelection();
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
		int nCount = m_paxConst.getCount();
		for( int i=0; i<nCount; i++ )
		{
			const CMobileElemConstraint * pPaxConstr = m_paxConst.getConstraint( i );
			if( mobileConstr.isEqual(pPaxConstr) )
			{
				//char szPaxType[128];
				CString szPaxType;
				mobileConstr.screenPrint( szPaxType );
				CString csMsg;
				csMsg.Format( "Passenger Type : %s exists in the list", szPaxType.GetBuffer(0) );
				MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
				return;
			}
		}
		
		m_paxConst.addConstraint( mobileConstr );
		//LoadPaxList());
	}
*/

	//added by frank 0519
	//if no model in location, copy remote to location
	/*if(HasModelInLocation() == MT_NOLOCATION)
	{
		CWaitCursor wait;
		if(!CopyModelToLocal())
		{
			MessageBox("Copy model fails!", "Error");
			wait.Restore();
//			return ;
		}
		wait.Restore();         //Restore the Wait cursor.
	}*/

	HTREEITEM hItem = m_treePaxType.GetSelectedItem();
	if (hItem == NULL)
		return;

	ItemData *pItemData =  (ItemData *)m_treePaxType.GetItemData(hItem);
	if (pItemData == NULL || pItemData->m_itemType != IT_MODEL)
		return;

	int nModelIndex = pItemData->m_nIndex;

	CPassengerTypeDialog dlg(NULL);//
	CModelToCompare *pModel  = m_pModelsManager->GetModelsList().at(nModelIndex);

	dlg.SetTerminal(pModel->GetTerminal());

	dlg.m_bNewVersion = TRUE;
	if(dlg.DoModal() == IDOK)
	{
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
	 	CModelParameter& modelParam = m_vModelParam[nModelIndex];
		std::vector<CMobileElemConstraint> vPaxType;
		modelParam.GetPaxType(vPaxType);
		
		int nCount = static_cast<int>(vPaxType.size());
		for( int i=0; i<nCount; i++ )
		{
			const CMobileElemConstraint& pPaxConstr = vPaxType.at( i );
			if( mobileConstr.isEqual(&pPaxConstr) )
			{
				//char szPaxType[128];
				CString szPaxType;
				mobileConstr.screenPrint( szPaxType );
				CString csMsg;
				csMsg.Format( "Passenger Type : %s exists in the list", szPaxType.GetBuffer(0) );
				MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
				return;
			}
		}
		vPaxType.push_back(mobileConstr);
		modelParam.SetPaxType(vPaxType);
		CString strItemText;
		mobileConstr.screenPrint(strItemText);

		HTREEITEM hPaxTypeItem = m_treePaxType.InsertItem(strItemText,hItem);

		
		m_treePaxType.SetItemData(hPaxTypeItem,(DWORD_PTR)new ItemData(IT_PAXTYPE,nCount));
	
		m_treePaxType.Expand(hItem,TVE_EXPAND);

		
		//m_paxConst.addConstraint( mobileConstr );
		//LoadPaxList();
	}
}

CReportProperty::MODELTYPE CReportProperty::HasModelInLocation()
{
	std::vector<CModelToCompare *>& vModels = m_pModelsManager->GetModelsList();
	if(vModels.size() <= 0)
		return MT_NOMODEL;

	for(int i = 0; i < static_cast<int>(vModels.size()); i++)
	{
		if(vModels[i]->GetModelLocation().GetAt(0) != '\\')
			return MT_HASLOCATION;
	}

	return MT_NOLOCATION;
}

void CReportProperty::OnProcessorTypeAdd()
{

	HTREEITEM hItem = m_treeProcType.GetSelectedItem();
	if (hItem == NULL)
		return;

	ItemData *pItemData =  (ItemData *)m_treeProcType.GetItemData(hItem);
	if (pItemData == NULL || pItemData->m_itemType != IT_MODEL)
		return;

	int nModelIndex = pItemData->m_nIndex;
	CModelToCompare *pModel  = m_pModelsManager->GetModelsList().at(nModelIndex);

	CProcesserDialog dlg(pModel->GetTerminal());
	if(dlg.DoModal() == IDOK)
	{
		ProcessorID id;
		ProcessorIDList idList;
		if( dlg.GetProcessorIDList(idList) )
		{
			int nIDcount = idList.getCount();
			for(int i = 0; i < nIDcount; i++)
			{
				id = *idList[i];
				
				// check if exist in the current list.
	 			CModelParameter& modelParam = m_vModelParam[nModelIndex];
				
				std::vector<ProcessorID> vProcGroups;
				modelParam.GetProcessorID(vProcGroups);

				int nProcCount = vProcGroups.size();
				bool bFind = false;
				for( int j=0; j<nProcCount; j++ )
				{
					if( vProcGroups[j] == id )
					{
						bFind = true;
						break;
					}
				}
				if( bFind )
					continue;
				
				
				//if( nProcCount == 0 )
				//	m_listProcessorType.ResetContent();
				
				// add into the data list
				vProcGroups.clear();
				vProcGroups.push_back( id );
				modelParam.SetProcessorID(vProcGroups);
				
				// add into gui list
				char szBuf[256];
				id.printID(szBuf);
				HTREEITEM hTempItem = NULL;
				if (hTempItem =m_treeProcType.GetChildItem(hItem))
				{
					m_treeProcType.SetItemText(hTempItem,szBuf);
				}
				else
				{
					hTempItem= m_treeProcType.InsertItem(szBuf, hItem);
					m_treeProcType.SetItemData(hTempItem,(DWORD_PTR)new ItemData(IT_PROCTYPE,0));
				}



	
				m_treeProcType.Expand(hItem,TVE_EXPAND);
				//int nIdx = m_listProcessorType.AddString( szBuf );
				//m_listProcessorType.SetItemData( nIdx, m_vProcList.size()-1 );
				//m_listProcessorType.SetCurSel( nIdx );
			}
		}
		
	}
}

void CReportProperty::OnPessengerTypeDelete()
{

	HTREEITEM hItem = m_treePaxType.GetSelectedItem();
	if (hItem == NULL)
		return;

	ItemData *pItemData =  (ItemData *)m_treePaxType.GetItemData(hItem);
	if (pItemData == NULL || pItemData->m_itemType != IT_PAXTYPE)
		return;
	int nPaxTypeIndex = pItemData->m_nIndex;

	HTREEITEM hParentItem = m_treePaxType.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;
	ItemData *pParentData = (ItemData *)m_treePaxType.GetItemData(hParentItem);
	if (pParentData == NULL)
		return;
	


	int nModelIndex = pParentData->m_nIndex;
	
	CModelParameter& modelParam = m_vModelParam[nModelIndex];
	std::vector<CMobileElemConstraint> vPaxType;
	modelParam.GetPaxType(vPaxType);
	vPaxType.erase(vPaxType.begin() + nPaxTypeIndex);
	
	modelParam.SetPaxType(vPaxType);

//	int iSel = m_listPessengerType.GetCurSel();
//	m_paxConst.deleteConst(iSel);
	LoadPaxListByModel(nModelIndex,hParentItem);
}

void CReportProperty::OnProcessorTypeDelete()
{

	HTREEITEM hItem = m_treeProcType.GetSelectedItem();
	if (hItem == NULL)
		return;

	ItemData *pItemData =  (ItemData *)m_treeProcType.GetItemData(hItem);
	if (pItemData == NULL || pItemData->m_itemType != IT_PROCTYPE)
		return;
	int nProcTypeIndex = pItemData->m_nIndex;

	HTREEITEM hParentItem = m_treeProcType.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;
	ItemData *pParentData = (ItemData *)m_treeProcType.GetItemData(hParentItem);
	if (pParentData == NULL)
		return;

	int nModelIndex = pParentData->m_nIndex;

	CModelParameter& modelParam = m_vModelParam[nModelIndex];

	std::vector<ProcessorID> vProcGroups;
	modelParam.GetProcessorID(vProcGroups);
	vProcGroups.erase(vProcGroups.begin() + nProcTypeIndex);
	modelParam.SetProcessorID(vProcGroups);
	LoadProcListByModel(nModelIndex,hParentItem);

	//int nCurSel = m_listProcessorType.GetCurSel();
	//if( nCurSel == LB_ERR )
	//{
	//	return;
	//}

	//// Handle deleting all processors
	//int nIdx = m_listProcessorType.GetItemData( nCurSel );
	//if( nIdx == -1 )
	//{
	//	return;
	//}
	//
	//// Delete from data
	//m_vProcList.erase( m_vProcList.begin() + nIdx );
	//
	//
	//// delete from gui
	//
	//
	//m_listProcessorType.DeleteString( nCurSel );
	//m_listProcessorType.SetCurSel( -1 );
	//
	//if( m_vProcList.size() == 0 )
	//{
	//	int nIdx = m_listProcessorType.AddString( "All Processors" );
	//	m_listProcessorType.SetItemData( nIdx, -1 );
	//}

	//LoadProcList();
}

void CReportProperty::Init()
{
	if (!m_reportToCompare.GetName().IsEmpty())
	{
		m_strName = m_reportToCompare.GetName();
		
		m_enumReportType = (ENUM_REPORT_TYPE)m_reportToCompare.GetCategory();
		m_enumOldType = m_enumReportType;
		int iCurSel = -1;
		switch (m_enumReportType)
		{
		case ENUM_QUEUELENGTH_REP:
			iCurSel = 0;
			break;
		case ENUM_QUEUETIME_REP:
			iCurSel = 1;
			break;
		case ENUM_THROUGHPUT_REP:
			iCurSel = 2;
			break;
		case ENUM_PAXDENS_REP:
			iCurSel = 3;
			break;
		case ENUM_PAXCOUNT_REP:
			iCurSel = 4;
			break;			
		case ENUM_ACOPERATION_REP:
			iCurSel = 5;
			break;
		case ENUM_DURATION_REP:
			iCurSel = 6;
			break;
		case ENUM_DISTANCE_REP:
			iCurSel = 7;
			break;
		}
		m_nReportType.SetCurSel(iCurSel);


		bool bAbsDate;
		int nDayIndex = -1;
		COleDateTime dtStart, dtEnd, dtTime;
		CReportParamToCompare param = m_reportToCompare.GetParameter();
		ElapsedTime et = param.GetStartTime();
		m_sdStart.GetDateTime(et, bAbsDate, dtStart, nDayIndex, dtTime);
		if (bAbsDate)
			m_dStart = dtStart;
		else
			m_cmbStartDateIndex.SetCurSel(nDayIndex);
		m_tStart = dtTime;
		//m_tStart.SetTime(et.asHours(), et.asMinutes() % 60, et.asSeconds() % 60);
		et = param.GetEndTime();
		m_sdStart.GetDateTime(et, bAbsDate, dtEnd, nDayIndex, dtTime);
		if (bAbsDate)
			m_dEnd = dtEnd;
		else
			m_cmbEndDateIndex.SetCurSel(nDayIndex);
		m_tEnd = dtTime;
		//m_tEnd.SetTime(et.asHours(), et.asMinutes() % 60, et.asSeconds() % 60);

		et = param.GetInterval();
		COleDateTime dt;
		dt.SetTime(et.asHours(), et.asMinutes() % 60, et.asSeconds() % 60);
		//m_tInterval.SetFormat( "HH:mm" );
		m_tInterval.SetTime(dt);

		long lInterval =0;
		param.GetInterval(lInterval);
		CString strIntervalText = _T("");
		strIntervalText.Format(_T("%d"),lInterval);
		m_editInterval.SetWindowText(strIntervalText);

		// area
		//if (iCurSel == 3)
		//{
		//	ASSERT(0);
	/*		CString strArea = param.GetArea();
			if( !strArea.IsEmpty() )
			{
				for( int i=0; i<m_cmbArea.GetCount(); i++ )
				{
					CString str;
					m_cmbArea.GetLBText(i,str );
					if( strArea ==  str)
					{
						m_cmbArea.SetCurSel( i );
						break;
					}
				}
			}*/
	//	}
		
		//std::vector<CMobileElemConstraint> vPaxConstr;
		//if (param.GetPaxType(vPaxConstr))
		//{
		//	for( int i=0; i< static_cast<int>(vPaxConstr.size()); i++ )
		//	{
		//		//CString str;
		//		//vPaxConstr[i].screenPrint( str, 0, 128 );
		//		//m_listPessengerType.AddString( str.GetBuffer(0) );

		//		m_paxConst.addConstraint( vPaxConstr[i] );
		//	}
			LoadPaxList();
		//}

		//param.GetProcessorID(m_vProcList);
		LoadProcList();

		//param.GetFromToProcs(m_fromToProcs);
		LoadProcFromToList();

		LoadArea();
	}
	else
	{
		//CMobileElemConstraint mob;
		//mob.SetInputTerminal(m_pTerminal);
		//mob.SetTypeIndex(0);
		//m_paxConst.addConstraint( mob );
		LoadPaxList();
		LoadProcList();

		//ProcessorID startID,endID;
		//startID.SetStrDict(m_pTerminal->inStrDict);
		//startID.setID("START");
		//endID.SetStrDict(m_pTerminal->inStrDict);	
		//endID.setID("END");
//		m_fromToProcs.m_vFromProcs.push_back(startID);
//		m_fromToProcs.m_vToProcs.push_back(endID);

		LoadProcFromToList();

		LoadArea();

		m_nReportType.SetCurSel(0);
//		m_cmbArea.SetCurSel(-1);
	}
}

BOOL CReportProperty::CopyModelToLocal()
{
	CString strPath = PROJMANAGER->GetAppPath();
	ASSERT(strPath != "" && m_strProjName != "");
	strPath += "\\" + m_strProjName;
	if(_access(strPath, 0) == -1)//dir not exist
		CreateDirectory(strPath, NULL);
	strPath += "\\Comparative Report";
	if(_access(strPath, 0) == -1)//dir not exist
		CreateDirectory(strPath, NULL);
	CString strRemoteModel = (*(m_pModelsManager->GetModelsList().begin()))->GetModelLocation();
	if(strRemoteModel == "")
		return FALSE;
	strPath += "\\" + strRemoteModel;
	if(_access(strPath, 0) == -1)//dir not exist
		CreateDirectory(strPath, NULL);
	CString strRModelPath = (*(m_pModelsManager->GetModelsList().begin()))->GetModelLocation();
	if(_access(strRModelPath, 0) == -1)//dir not exist
		return FALSE;
	
	//copy files from strRModelPath to strPath
	if(!CopyDirectory(strRModelPath, strPath))
		return FALSE;

	

	return TRUE;
}

BOOL CReportProperty::CopyDirectory(const CString& strSrcDir, const CString& strDistDir, BOOL bDelSrc)
{
    CFileFind tempFind;
    CString tempFileFind = strSrcDir + "\\*.*";
    BOOL IsFinded = tempFind.FindFile(tempFileFind);
    while(IsFinded)
	{
		IsFinded = tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			CString tempSrc, tempDist;
			tempSrc = strSrcDir + "\\" + tempFind.GetFileName();
			tempDist = strDistDir + "\\" + tempFind.GetFileName();
			if(tempFind.IsDirectory())
			{
				CreateDirectory(tempDist, NULL);
				if(!CopyDirectory(tempSrc, tempDist, bDelSrc))
					return FALSE;
			}
			else
			{
				if(!::CopyFile(tempSrc, tempDist, FALSE))
					return FALSE;
				if(bDelSrc)
					if(!::DeleteFile(tempSrc))
						return FALSE;
			}
		}
	}
	tempFind.Close();

	if(bDelSrc)
	{
		if(!RemoveDirectory(strSrcDir))
		{
			return FALSE;
		}
	}
	return TRUE;
}

void CReportProperty::LoadPaxList()
{
	switch( m_enumReportType )
	{
	
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:/*
		{
			m_treePaxType.DeleteAllItems();
			m_hItemPaxType=m_treePaxType.InsertItem("Pax Type");
			int count=m_paxConst.getCount();
			if(count==0)
			{
				CMobileElemConstraint def;
				m_paxConst.addConstraint(def);
			}
			
			for(int i=0;i<count;i++)
			{
				const CMobileElemConstraint* paxConstr = m_paxConst.getConstraint( i );
				//char str[128];
				CString str;
				//paxConstr->screenPrint( str, 128 );
				paxConstr->screenPrint( str, 0, 128 );
				HTREEITEM hItem=m_treePaxType.InsertItem(str.GetBuffer(0),m_hItemPaxType);
				m_treePaxType.SetItemData(hItem,i);
			}
			
			HTREEITEM hItemFromTo=m_treePaxType.InsertItem("Processor To Processor");
			
			CReportParameter::FROM_TO_PROCS _fromToProcs;
			GetDocument()->GetReportPara()->GetFromToProcs(_fromToProcs);
			
			Terminal& tmnl=GetDocument()->GetTerminal();
			Terminal* pTmnl=&tmnl;
			InputTerminal* pInTmnl=(InputTerminal*)pTmnl;
			
			if(_fromToProcs.m_vFromProcs.size()==0)
			{
				const ProcessorID* pID=pInTmnl->procList->getProcessor(START_PROCESSOR_INDEX)->getID();
				_fromToProcs.m_vFromProcs.push_back(*pID);
				GetDocument()->GetReportPara()->SetFromToProcs(_fromToProcs);
			}
			
			if(_fromToProcs.m_vToProcs.size()==0)
			{
				const ProcessorID* pID=pInTmnl->procList->getProcessor(END_PROCESSOR_INDEX)->getID();
				_fromToProcs.m_vToProcs.push_back(*pID);
				GetDocument()->GetReportPara()->SetFromToProcs(_fromToProcs);
				
			}
			
			
			
			m_hItemFrom=m_treePaxType.InsertItem("From",hItemFromTo);
			for(int k=0;k<_fromToProcs.m_vFromProcs.size();k++)
			{
				HTREEITEM hItem=m_treePaxType.InsertItem(_fromToProcs.m_vFromProcs[k].GetIDString(),m_hItemFrom);
				m_treePaxType.SetItemData(hItem,k);
			}
			
			m_hItemTo=m_treePaxType.InsertItem("To",hItemFromTo);
			for(int j=0;j<_fromToProcs.m_vToProcs.size();j++)
			{
				HTREEITEM hItem=m_treePaxType.InsertItem(_fromToProcs.m_vToProcs[j].GetIDString(),m_hItemTo);
				m_treePaxType.SetItemData(hItem,j);
			}
		}
		break;*/
	default:
		{
			m_treePaxType.DeleteAllItems();
			m_treePaxType.SetRedraw();

			int nModelCount = static_cast<int>(m_vModelParam.size());
			for (int nModel = 0; nModel < nModelCount; ++nModel)
			{
				LoadPaxListByModel(nModel,NULL);
			}

			//m_listPessengerType.ResetContent();
			//int count = m_paxConst.getCount();
			//for( int i=0; i<count; i++ )
			//{
			//	const CMobileElemConstraint* paxConstr = m_paxConst.getConstraint( i );
			//	//char str[128];
			//	CString str;
			//	//paxConstr->screenPrint( str, 128 );
			//	paxConstr->screenPrint( str, 0, 128 );
			//	m_listPessengerType.AddString( str.GetBuffer(0) );
			//}
			//m_listPessengerType.SetCurSel( -1 );
			
		}
		break;
	}

}
void CReportProperty::LoadPaxListByModel(int nModelIndex,HTREEITEM hItemModel)
{
	CModelParameter modelParam = m_vModelParam[nModelIndex];

	if (hItemModel == NULL)
	{
		hItemModel = m_treePaxType.InsertItem(modelParam.GetModelUniqueName(),TVI_ROOT);
		m_treePaxType.SetItemState( hItemModel,TVIS_BOLD, TVIS_BOLD );
	}
	else
	{

		HTREEITEM hChildItem = NULL;
		while(hChildItem = m_treePaxType.GetChildItem(hItemModel))
		{
			ItemData *pChildItemData  = (ItemData *)m_treePaxType.GetItemData(hChildItem);
			delete pChildItemData;
			m_treePaxType.DeleteItem(hChildItem);

		}
	}

	

	ItemData *pItemData = new ItemData(IT_MODEL,nModelIndex);
	m_treePaxType.SetItemData(hItemModel,(DWORD_PTR)pItemData);

	std::vector<CMobileElemConstraint> vPaxType;

	modelParam.GetPaxType(vPaxType);

	int nCount = static_cast<int>(vPaxType.size());
	for( int i=0; i<nCount; i++ )
	{
		CMobileElemConstraint paxConstr = vPaxType[i];
		CString strItemText;
		paxConstr.screenPrint( strItemText, 0, 128 );

		HTREEITEM hItem = m_treePaxType.InsertItem(strItemText,hItemModel);

		m_treePaxType.SetItemData(hItem,(DWORD_PTR)new ItemData(IT_PAXTYPE,i));
	}
	
	m_treePaxType.Expand(hItemModel,TVE_EXPAND);


}

// load processor into the list base on the m_vProcList;
void CReportProperty::LoadProcList()
{

	m_treeProcType.DeleteAllItems();
	m_treeProcType.SetImageList(m_treeProcType.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	int nModelCount = static_cast<int>(m_vModelParam.size());
	for (int nModel = 0; nModel < nModelCount; ++nModel)
	{
		LoadProcListByModel(nModel,NULL);
	}

	//m_listProcessorType.ResetContent();
	//
	//int nProcTypeCount = m_vProcList.size();
	//
	//if( nProcTypeCount == 0 )
	//{
	//	int nIdx = m_listProcessorType.AddString( "All Processors" );
	//	m_listProcessorType.SetItemData( nIdx, -1 );
	//	return;
	//}
	//
	//for( int i=0; i<nProcTypeCount; i++ )
	//{
	//	ProcessorID id = m_vProcList[i];
	//	char szBuf[256];
	//	id.printID(szBuf);
	//	int nIdx = m_listProcessorType.AddString( szBuf );
	//	m_listProcessorType.SetItemData( nIdx, i );
	//}
}

void CReportProperty::LoadProcListByModel(int nModelIndex,HTREEITEM hItemModel)
{
	CModelParameter modelParam = m_vModelParam[nModelIndex];
	if (hItemModel == NULL)
	{
		hItemModel = m_treeProcType.InsertItem(modelParam.GetModelUniqueName(),TVI_ROOT);
		m_treeProcType.SetItemState( hItemModel,TVIS_BOLD, TVIS_BOLD );
	}
	else
	{

		HTREEITEM hChildItem = NULL;
		while(hChildItem = m_treeProcType.GetChildItem(hItemModel))
		{
			ItemData *pChildItemData  = (ItemData *)m_treeProcType.GetItemData(hChildItem);
			delete pChildItemData;
			m_treeProcType.DeleteItem(hChildItem);
		}
	}

	ItemData *pItemData = new ItemData(IT_MODEL,nModelIndex);
	m_treeProcType.SetItemData(hItemModel,(DWORD_PTR)pItemData);

	std::vector<ProcessorID> vProcGroups;
	int nProcTypeCount = modelParam.GetProcessorID(vProcGroups);

	for( int i=0; i<nProcTypeCount; i++ )
	{
		ProcessorID id = vProcGroups[i];
		char szBuf[256];
		id.printID(szBuf);

		HTREEITEM hProcItem = m_treeProcType.InsertItem(szBuf,hItemModel);
		m_treeProcType.SetItemData(hProcItem,(DWORD_PTR)new ItemData(IT_PROCTYPE,i));
	}
	m_treeProcType.Expand(hItemModel,TVE_EXPAND);

}
void CReportProperty::LoadProcFromToList()
{
	m_treeProcFromTo.DeleteAllItems();
	m_treeProcFromTo.SetRedraw();

	int nModelCount = static_cast<int>(m_vModelParam.size());
	for (int nModel = 0; nModel < nModelCount; ++nModel)
	{
		LoadProcFromToListByModel(nModel,NULL);
	}

	//from
	//m_hItemFrom = m_treeProcFromTo.InsertItem(_T("From"));
	//int nProcTypeCount = m_fromToProcs.m_vFromProcs.size();
	//for( int i=0; i<nProcTypeCount; i++ )
	//{
	//	ProcessorID id = m_fromToProcs.m_vFromProcs[i];
	//	char szBuf[256];
	//	id.printID(szBuf);
	//	HTREEITEM hItem = m_treeProcFromTo.InsertItem(szBuf,m_hItemFrom);
	//	m_treeProcFromTo.SetItemData(hItem,i);
	//}
	//m_treeProcFromTo.Expand(m_hItemFrom,TVE_EXPAND);
	//
	////to
	//m_hItemTo=m_treeProcFromTo.InsertItem("To");
	//for(int j=0;j<static_cast<int>(m_fromToProcs.m_vToProcs.size());j++)
	//{
	//	HTREEITEM hItem=m_treeProcFromTo.InsertItem(m_fromToProcs.m_vToProcs[j].GetIDString(),m_hItemTo);
	//	m_treeProcFromTo.SetItemData(hItem,j);
	//}
	//m_treeProcFromTo.Expand(m_hItemTo,TVE_EXPAND);
}

void CReportProperty::LoadProcFromToListByModel(int nModelIndex,HTREEITEM hItemModel)
{
		CModelParameter modelParam = m_vModelParam[nModelIndex];
		if (hItemModel == NULL)
		{
			hItemModel = m_treeProcFromTo.InsertItem(modelParam.GetModelUniqueName(),TVI_ROOT);
			m_treeProcFromTo.SetItemState( hItemModel,TVIS_BOLD, TVIS_BOLD );
		}
		else
		{
			
			HTREEITEM hChildItem = NULL;
			while(hChildItem = m_treeProcFromTo.GetChildItem(hItemModel))
			{
				HTREEITEM hChildChildItem = NULL;
				while (hChildChildItem = m_treeProcFromTo.GetChildItem(hChildItem))
				{
					ItemData *pChildChildItemData  = (ItemData *)m_treeProcFromTo.GetItemData(hChildChildItem);
					delete pChildChildItemData;
					m_treeProcFromTo.DeleteItem(hChildChildItem);
				}

				ItemData *pChildItemData  = (ItemData *)m_treeProcFromTo.GetItemData(hChildItem);
				delete pChildItemData;	
				m_treeProcFromTo.DeleteItem(hChildItem);
			}
		}

		ItemData *pItemData = new ItemData(IT_MODEL,nModelIndex);
		m_treeProcFromTo.SetItemData(hItemModel,(DWORD_PTR)pItemData);
		
		CReportParameter::FROM_TO_PROCS _fromToProcs;
		modelParam.GetFromToProcs(_fromToProcs);

		// from proc	
		HTREEITEM hItemFrom = m_treeProcFromTo.InsertItem(_T("From"),hItemModel);
		m_treeProcFromTo.SetItemData(hItemFrom,(DWORD_PTR)new ItemData(IT_FROM,0));

		int nProcTypeCount = _fromToProcs.m_vFromProcs.size();
		for( int i=0; i<nProcTypeCount; i++ )
		{
			ProcessorID id = _fromToProcs.m_vFromProcs[i];
			char szBuf[256];
			id.printID(szBuf);
			HTREEITEM hItem = m_treeProcFromTo.InsertItem(szBuf,hItemFrom);
			m_treeProcFromTo.SetItemData(hItem,(DWORD_PTR)new ItemData(IT_FROMPROC,i));
		}
		//to proc
		HTREEITEM hItemTo=m_treeProcFromTo.InsertItem("To",hItemModel);
		m_treeProcFromTo.SetItemData(hItemTo,(DWORD_PTR)new ItemData(IT_TO,0));
		for(int j=0;j<static_cast<int>(_fromToProcs.m_vToProcs.size());j++)
		{
			HTREEITEM hItem=m_treeProcFromTo.InsertItem(_fromToProcs.m_vToProcs[j].GetIDString(),hItemTo);
			m_treeProcFromTo.SetItemData(hItem,(DWORD_PTR)new ItemData(IT_TOPROC,j));
		}

		m_treeProcFromTo.Expand(hItemModel,TVE_EXPAND);
		m_treeProcFromTo.Expand(hItemFrom,TVE_EXPAND);
		m_treeProcFromTo.Expand(hItemTo,TVE_EXPAND);

}




void CReportProperty::LoadArea()
{
	m_treeArea.DeleteAllItems();
	m_treeArea.SetImageList(m_treeArea.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	int nModelCount = static_cast<int>(m_vModelParam.size());
	for (int nModel = 0; nModel < nModelCount; ++nModel)
	{
		LoadAreaByModel(nModel,NULL);
	}
}
void CReportProperty::LoadAreaByModel(int nModelIndex,HTREEITEM hItemModel)
{
	CModelParameter modelParam = m_vModelParam[nModelIndex];
	if (hItemModel == NULL)
	{
		hItemModel = m_treeArea.InsertItem(modelParam.GetModelUniqueName(),TVI_ROOT);
		m_treeArea.SetItemState( hItemModel,TVIS_BOLD, TVIS_BOLD );
	}
	else
	{

		HTREEITEM hChildItem = NULL;
		while(hChildItem = m_treeArea.GetChildItem(hItemModel))
		{
			ItemData *pChildItemData  = (ItemData *)m_treeArea.GetItemData(hChildItem);
			delete pChildItemData;
			m_treeArea.DeleteItem(hChildItem);

		}
	}
	ItemData *pItemData = new ItemData(IT_MODEL,nModelIndex);
	m_treeArea.SetItemData(hItemModel,(DWORD_PTR)pItemData);

	CString strArea = modelParam.GetArea();
	strArea.Trim();
	if (!strArea.IsEmpty())
	{
		HTREEITEM hItem = m_treeArea.InsertItem(strArea,hItemModel);

		m_treeArea.SetItemData(hItem,(DWORD_PTR)new ItemData(IT_AREA,nModelIndex));
	}
m_treeArea.Expand(hItemModel,TVE_EXPAND);

}
void CReportProperty::OnSelchangeComboReporttype() 
{
	ArrangeControls();
}

void CReportProperty::ArrangeControls()
{
	int nSel = m_nReportType.GetCurSel();
	CRect rcClient;
	m_treeProcFromTo.ShowWindow(SW_HIDE);
	m_ToolBar3.ShowWindow(SW_HIDE);

	GetDlgItem(IDC_DTANIMTIMEINTERVAL)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TIMEINTERVAL)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_INTERVAL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_AREATEXT)->ShowWindow(SW_HIDE);
	m_editInterval.ShowWindow(SW_HIDE);
	m_ToolBar4.ShowWindow(SW_HIDE);
	m_treeArea.ShowWindow(SW_HIDE);


	switch (nSel)
	{
	case 5:
	case 4:
	case 3:
		{		//	SpaceDensity
			m_treePaxType.MoveWindow(m_rcWindow[3]);
			GetDlgItem(IDC_PAX_TYPE)->ShowWindow(SW_SHOW);
			m_ToolBar.ShowWindow(SW_SHOW);
			m_treePaxType.ShowWindow(SW_SHOW);

			GetDlgItem(IDC_PROC_TYPE)->ShowWindow(SW_HIDE);
			m_ToolBar2.ShowWindow(SW_HIDE);
			m_treeProcType.ShowWindow(SW_HIDE);
			
			if (nSel == 3)
			{		
				m_treePaxType.GetWindowRect(rcClient);
				m_treePaxType.SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOMOVE);
					GetDlgItem(IDC_STATIC_AREATEXT)->ShowWindow(SW_SHOW);
				m_ToolBar4.ShowWindow(SW_SHOW);
				m_treeArea.ShowWindow(SW_SHOW);
				m_treeArea.MoveWindow(m_rcWindow[2]);
			}
			else
			{
				m_ToolBar.GetWindowRect(rcClient);
				//rcClient = m_rcWindow[1];
				ScreenToClient(rcClient);
				//rcClient.top -= 130;
				//rcClient.bottom -= 130;
//				rcClient.right += 210;
				m_ToolBar.MoveWindow(rcClient);


				m_treePaxType.GetWindowRect(rcClient);
				m_treePaxType.SetWindowPos(NULL, 0, 0, rcClient.Width() + 210, rcClient.Height(), SWP_NOMOVE);
			}
			m_bShowProcessor = FALSE;
		}
		break;

	case 1:		//	QTime	
		{
			m_treePaxType.MoveWindow(m_rcWindow[3]);
			GetDlgItem(IDC_PROC_TYPE)->MoveWindow(m_rcWindow[0]);
			m_ToolBar2.MoveWindow(m_rcWindow[1]);
			m_treeProcType.MoveWindow(m_rcWindow[2]);
			if (!m_bShowProcessor)
			{
				//m_listPessengerType.GetWindowRect(rcClient);
				//m_listPessengerType.SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height() - 130, SWP_NOMOVE);
			}
			GetDlgItem(IDC_PROC_TYPE)->ShowWindow(SW_SHOW);
			m_ToolBar2.ShowWindow(SW_SHOW);
			m_treeProcType.ShowWindow(SW_SHOW);

			GetDlgItem(IDC_PAX_TYPE)->ShowWindow(SW_SHOW);
			m_ToolBar.ShowWindow(SW_SHOW);
			m_treePaxType.ShowWindow(SW_SHOW);	

			m_bShowProcessor = TRUE;
		}
		break;
	case 6: //time in terminal
		{
			m_treePaxType.MoveWindow(m_rcWindow[3]);
			GetDlgItem(IDC_PROC_TYPE)->MoveWindow(m_rcWindow[0]);

			m_ToolBar2.ShowWindow(SW_HIDE);
			m_ToolBar3.MoveWindow(m_rcWindow[1]);
			m_treeProcFromTo.MoveWindow(m_rcWindow[2]);
			if (!m_bShowProcessor)
			{
				//m_listPessengerType.GetWindowRect(rcClient);
				//m_listPessengerType.SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height() - 130, SWP_NOMOVE);
			}
			GetDlgItem(IDC_PROC_TYPE)->ShowWindow(SW_SHOW);
			m_ToolBar3.ShowWindow(SW_SHOW);
			m_treeProcType.ShowWindow(SW_HIDE);
			m_treeProcFromTo.ShowWindow(SW_SHOW);

			GetDlgItem(IDC_PAX_TYPE)->ShowWindow(SW_SHOW);
			m_ToolBar.ShowWindow(SW_SHOW);
			m_treePaxType.ShowWindow(SW_SHOW);

			//GetDlgItem(IDC_DTANIMTIMEINTERVAL)->ShowWindow(SW_HIDE);
			//GetDlgItem(IDC_STATIC_TIMEINTERVAL)->ShowWindow(SW_HIDE);

			//m_editInterval.ShowWindow(SW_SHOW);
			//GetDlgItem(IDC_STATIC_INTERVAL)->ShowWindow(SW_SHOW);

			m_bShowProcessor = TRUE;		
		}
		break;
	case 7: //distance travel
		{
			m_treePaxType.MoveWindow(m_rcWindow[3]);
			GetDlgItem(IDC_PROC_TYPE)->MoveWindow(m_rcWindow[0]);

			m_ToolBar2.ShowWindow(SW_HIDE);
			m_ToolBar3.MoveWindow(m_rcWindow[1]);
			m_treeProcFromTo.MoveWindow(m_rcWindow[2]);
			if (!m_bShowProcessor)
			{
				//m_listPessengerType.GetWindowRect(rcClient);
				//m_listPessengerType.SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height() - 130, SWP_NOMOVE);
			}
			GetDlgItem(IDC_PROC_TYPE)->ShowWindow(SW_SHOW);
			m_ToolBar3.ShowWindow(SW_SHOW);
			m_treeProcType.ShowWindow(SW_HIDE);
			m_treeProcFromTo.ShowWindow(SW_SHOW);

			GetDlgItem(IDC_PAX_TYPE)->ShowWindow(SW_SHOW);
			m_ToolBar.ShowWindow(SW_SHOW);
			m_treePaxType.ShowWindow(SW_SHOW);

	//		GetDlgItem(IDC_STATIC_REPORT_AREA)->ShowWindow(SW_HIDE);
	//		GetDlgItem(IDC_COMBO_REPORT_AREA)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_DTANIMTIMEINTERVAL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_TIMEINTERVAL)->ShowWindow(SW_HIDE);

			m_editInterval.ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_INTERVAL)->ShowWindow(SW_SHOW);

			m_bShowProcessor = TRUE;
		}
		break;
	case 2://Throughput Report
	case 0:			//	QLength
		{
			//m_listPessengerType.MoveWindow(m_rcWindow[3]);
			GetDlgItem(IDC_PROC_TYPE)->MoveWindow(m_rcWindow[0]);
			m_ToolBar2.MoveWindow(m_rcWindow[1]);
			m_treeProcType.MoveWindow(m_rcWindow[2]);
			m_treePaxType.MoveWindow(m_rcWindow[3]);

			GetDlgItem(IDC_PAX_TYPE)->ShowWindow(SW_HIDE);
			m_ToolBar.ShowWindow(SW_HIDE);
			m_treePaxType.ShowWindow(SW_HIDE);

			GetDlgItem(IDC_PROC_TYPE)->GetWindowRect(rcClient);
			//rcClient = m_rcWindow[0];
			ScreenToClient(rcClient);
			//rcClient.top -= 130;
			//rcClient.bottom -= 130;
			rcClient.left -= 210;
			GetDlgItem(IDC_PROC_TYPE)->MoveWindow(rcClient);
			GetDlgItem(IDC_PROC_TYPE)->ShowWindow(SW_SHOW);

			m_ToolBar2.GetWindowRect(rcClient);
			//rcClient = m_rcWindow[1];
			ScreenToClient(rcClient);
			//rcClient.top -= 130;
			//rcClient.bottom -= 130;
			rcClient.left -= 210;
			m_ToolBar2.MoveWindow(rcClient);
			m_ToolBar2.ShowWindow(SW_SHOW);

			m_treeProcType.GetWindowRect(rcClient);
			//rcClient = m_rcWindow[2];
			ScreenToClient(rcClient);
			//rcClient.top -= 130;
			rcClient.left -= 210;
			m_treeProcType.MoveWindow(rcClient);
			m_treeProcType.ShowWindow(SW_SHOW);
		}
		break;
	default:
		break;
	}

	if (m_sdStart.IsAbsoluteDate())
	{
		CRect rc;
		m_cmbStartDateIndex.GetWindowRect(rc);
		ScreenToClient(rc);
		m_dtCtrlStartDate.MoveWindow(rc);

		m_cmbEndDateIndex.GetWindowRect(rc);
		ScreenToClient(rc);
		m_dtCtrlEndDate.MoveWindow(rc);

		m_cmbStartDateIndex.ShowWindow(SW_HIDE);
		m_cmbEndDateIndex.ShowWindow(SW_HIDE);
		m_dtCtrlStartDate.ShowWindow(SW_SHOW);
		m_dtCtrlEndDate.ShowWindow(SW_SHOW);
	}
	else
	{

		m_cmbStartDateIndex.ShowWindow(SW_SHOW);
		m_cmbEndDateIndex.ShowWindow(SW_SHOW);

		m_dtCtrlStartDate.ShowWindow(SW_HIDE);
		m_dtCtrlEndDate.ShowWindow(SW_HIDE);
	}

	OnUpdateToolBarUI();
}

void CReportProperty::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
void CReportProperty::OnProcessorFromToAdd()
{

	HTREEITEM hItem = m_treeProcFromTo.GetSelectedItem();
	if (hItem == NULL)
		return;

	CString strTemp = m_treeProcFromTo.GetItemText(hItem);
	ItemData *pItemData =  (ItemData *)m_treeProcFromTo.GetItemData(hItem);
	if (pItemData == NULL)
		return;
	if (pItemData->m_itemType != IT_FROM && pItemData->m_itemType != IT_TO)
		return;
	
	HTREEITEM hParentItem = m_treeProcFromTo.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;
	ItemData *pParentData = (ItemData *)m_treeProcFromTo.GetItemData(hParentItem);
	if (pParentData == NULL)
		return;

	int nModelIndex = pParentData->m_nIndex;


	if(pItemData->m_itemType == IT_FROM  )
	{
		CProcesserDialog dlg(m_pModelsManager->GetModelsList().at(nModelIndex)->GetTerminal());
		if(dlg.DoModal() == IDOK)
		{
			ProcessorID id;
			ProcessorIDList idList;
			if( dlg.GetProcessorIDList(idList) )
			{
				int nIDcount = idList.getCount();
				for(int i = 0; i < nIDcount; i++)
				{
					id = *idList[i];

					CModelParameter& modelParam = m_vModelParam[nModelIndex];
					
					CReportParameter::FROM_TO_PROCS _fromToProcs;
					modelParam.GetFromToProcs(_fromToProcs);

					// check if exist in the current list.
					int nProcCount = _fromToProcs.m_vFromProcs.size();
					bool bFind = false;
					for( int j=0; j<nProcCount; j++ )
					{
						if( _fromToProcs.m_vFromProcs[j] == id )
						{
							bFind = true;
							break;
						}
					}
					if( bFind )
						continue;



					// add into the data list
					_fromToProcs.m_vFromProcs.push_back( id );
					modelParam.SetFromToProcs(_fromToProcs);


					// add into gui list
					char szBuf[256];
					id.printID(szBuf);						
					HTREEITEM hTempItem = m_treeProcFromTo.InsertItem(szBuf,hItem);
					m_treeProcFromTo.SetItemData(hTempItem,(DWORD_PTR)(new ItemData(IT_FROMPROC,nProcCount)));
				}

//				m_treeProcFromTo.Expand(m_hItemFrom,TVE_EXPAND);
			}

		}
	}
	if (pItemData->m_itemType == IT_TO)
	{
		CProcesserDialog dlg(m_pModelsManager->GetModelsList().at(nModelIndex)->GetTerminal());
		if(dlg.DoModal() == IDOK)
		{
			ProcessorID id;
			ProcessorIDList idList;
			if( dlg.GetProcessorIDList(idList) )
			{
				int nIDcount = idList.getCount();
				for(int i = 0; i < nIDcount; i++)
				{
					id = *idList[i];
					CModelParameter& modelParam = m_vModelParam[nModelIndex];

					CReportParameter::FROM_TO_PROCS _fromToProcs;
					modelParam.GetFromToProcs(_fromToProcs);

					// check if exist in the current list.
					int nProcCount = _fromToProcs.m_vToProcs.size();
					bool bFind = false;
					for( int j=0; j<nProcCount; j++ )
					{
						if( _fromToProcs.m_vToProcs[j] == id )
						{
							bFind = true;
							break;
						}
					}
					if( bFind )
						continue;

					// add into the data list
					_fromToProcs.m_vToProcs.push_back( id );
					modelParam.SetFromToProcs(_fromToProcs);

					// add into gui list
					char szBuf[256];
					id.printID(szBuf);						
					HTREEITEM hTempItem = m_treeProcFromTo.InsertItem(szBuf,hItem);
					m_treeProcFromTo.SetItemData(hItem,(DWORD_PTR)(new ItemData(IT_FROMPROC,nProcCount)));
				}
			}


		}
	}
		m_treeProcFromTo.Expand(hItem,TVE_EXPAND);
}
void CReportProperty::OnProcessorFromToDelete()
{

	HTREEITEM hSelectItem = m_treeProcFromTo.GetSelectedItem();
	if (hSelectItem == NULL)
		return;
	CString strTest = m_treeProcFromTo.GetItemText(hSelectItem);
	ItemData *pItemData =  (ItemData *)m_treeProcFromTo.GetItemData(hSelectItem);
	if (pItemData == NULL)
	{
		return;
	}
	if (pItemData->m_itemType != IT_FROMPROC && pItemData->m_itemType != IT_TOPROC)
		return;
	int nProcIndex = pItemData->m_nIndex;

	HTREEITEM hFromOrToItem = m_treeProcFromTo.GetParentItem(hSelectItem);
	HTREEITEM hModelItem = m_treeProcFromTo.GetParentItem(hFromOrToItem);
	if (hModelItem == NULL)
		return;

	ItemData *pModelData =  (ItemData *)m_treeProcFromTo.GetItemData(hModelItem);
	if (pModelData == NULL)
		return;
	int nModelIndex = pModelData->m_nIndex;
	CModelParameter& modelParam = m_vModelParam[nModelIndex];
	CReportParameter::FROM_TO_PROCS _fromToProcs;
	modelParam.GetFromToProcs(_fromToProcs);

	if(pItemData->m_itemType == IT_FROMPROC)
	{
		_fromToProcs.m_vFromProcs.erase(_fromToProcs.m_vFromProcs.begin() + nProcIndex);
	}

	if(pItemData->m_itemType == IT_TOPROC)
	{
		_fromToProcs.m_vToProcs.erase(_fromToProcs.m_vToProcs.begin() + nProcIndex);
	}
	modelParam.SetFromToProcs(_fromToProcs);
	LoadProcFromToListByModel(nModelIndex ,hModelItem);
	
	//HTREEITEM hItem = m_treeProcFromTo.GetSelectedItem();

	//if (m_treeProcFromTo.GetParentItem(hItem) == m_hItemFrom )
	//{

	//	int nIndex=m_treeProcFromTo.GetItemData(hItem);
	//	m_fromToProcs.m_vFromProcs.erase(m_fromToProcs.m_vFromProcs.begin()+nIndex);
	//	m_treeProcFromTo.DeleteItem(hItem);
	//	LoadProcFromToList();
	//}

	//if (m_treeProcFromTo.GetParentItem(hItem) == m_hItemTo )
	//{

	//	int nIndex=m_treeProcFromTo.GetItemData(hItem);
	//	m_fromToProcs.m_vToProcs.erase(m_fromToProcs.m_vToProcs.begin()+nIndex);
	//	m_treeProcFromTo.DeleteItem(hItem);
	//	LoadProcFromToList();
	//}

}
void CReportProperty::OnAddArea()
{
	HTREEITEM hItem = m_treeArea.GetSelectedItem();
	if (hItem == NULL)
		return;

	ItemData *pItemData =  (ItemData *)m_treeArea.GetItemData(hItem);
	if (pItemData == NULL || pItemData->m_itemType != IT_MODEL)
		return;

	int nModelIndex = pItemData->m_nIndex;	
	CModelToCompare *pModel  = m_pModelsManager->GetModelsList().at(nModelIndex);

	CDlgSelectArea dlg(pModel->GetTerminal(),this);
	if(dlg.DoModal() == IDOK)
	{
		CString strArea = dlg.GetSelectArea();
		if(strArea.IsEmpty())
			return;
		m_vModelParam[nModelIndex].SetArea(strArea);
		
	 	HTREEITEM hAreaItem = m_treeArea.GetChildItem(hItem);
		if (hAreaItem == NULL)
		{
			hAreaItem = m_treeArea.InsertItem(strArea,hItem);
			m_treeArea.SetItemData(hAreaItem,(DWORD_PTR)new ItemData(IT_AREA,nModelIndex));
		}
		else
		{
			m_treeArea.SetItemText(hAreaItem,strArea);
		}
	}

}
void CReportProperty::OnDelArea()
{

	HTREEITEM hItem = m_treeArea.GetSelectedItem();
	if (hItem == NULL)
		return;

	ItemData *pItemData =  (ItemData *)m_treeArea.GetItemData(hItem);
	if (pItemData == NULL || pItemData->m_itemType != IT_AREA)
		return;

	m_vModelParam[pItemData->m_nIndex].SetArea(_T(""));
	m_treeArea.DeleteItem(hItem);
	delete pItemData;
}
void CReportProperty::OnUpdateUIPaxTypeAdd()
{
	HTREEITEM hItem = m_treePaxType.GetSelectedItem();
	if (hItem == NULL)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONADDFIR,FALSE);

	}
	else
	{
		ItemData *pItemData =  (ItemData *)m_treePaxType.GetItemData(hItem);
		if (pItemData == NULL || pItemData->m_itemType != IT_MODEL)
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONADDFIR,FALSE);
		else
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONADDFIR,TRUE);
	}
}
void CReportProperty::OnUpdateUIProcTypeAdd()
{
	HTREEITEM hItem = m_treeProcType.GetSelectedItem();
	if (hItem == NULL)
	{
		m_ToolBar2.GetToolBarCtrl().EnableButton(ID_BUTTONADDSEC,FALSE);	
		return;
	}
	else
	{
		ItemData *pItemData =  (ItemData *)m_treeProcType.GetItemData(hItem);
		if (pItemData == NULL || pItemData->m_itemType != IT_MODEL)
		{
			m_ToolBar2.GetToolBarCtrl().EnableButton(ID_BUTTONADDSEC,FALSE);	
			return;
		}
	}

	m_ToolBar2.GetToolBarCtrl().EnableButton(ID_BUTTONADDSEC,TRUE);	
}
void CReportProperty::OnUpdateUIFromToAdd()
{
	HTREEITEM hItem = m_treeProcFromTo.GetSelectedItem();
	if (hItem == NULL)
	{
		m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONADDFROMTO,FALSE);	

	}
	else
	{	
		CString strTemp = m_treeProcFromTo.GetItemText(hItem);
		ItemData *pItemData =  (ItemData *)m_treeProcFromTo.GetItemData(hItem);
		if (pItemData == NULL)
		{
			m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONADDFROMTO,FALSE);	
			return;
		}
		if ( pItemData->m_itemType != IT_FROM && pItemData->m_itemType != IT_TO)
			m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONADDFROMTO,FALSE);	
		else
			m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONADDFROMTO,TRUE);	
	}

}
void CReportProperty::OnUpdateUIAreaAdd()
{
	HTREEITEM hItem = m_treeArea.GetSelectedItem();
	if (hItem == NULL)
	{	
		m_ToolBar4.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWAREA,FALSE);
		
	}
	else
	{
		ItemData *pItemData =  (ItemData *)m_treeArea.GetItemData(hItem);
		if (pItemData == NULL || pItemData->m_itemType != IT_MODEL)
			m_ToolBar4.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWAREA,FALSE);		
		else
			m_ToolBar4.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWAREA,TRUE);		
	}
}
void CReportProperty::OnUpdateUIAreaDel()
{
	HTREEITEM hItem = m_treeArea.GetSelectedItem();
	if (hItem == NULL)
	{
		m_ToolBar4.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELAREA,FALSE);	
	}
	else
	{
		ItemData *pItemData =  (ItemData *)m_treeArea.GetItemData(hItem);
		if (hItem == NULL || pItemData->m_itemType != IT_AREA)
			m_ToolBar4.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELAREA,FALSE);				
		else
			m_ToolBar4.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELAREA,TRUE);				
	}


}
void CReportProperty::OnUpdateUIPaxTypeDel()
{
	HTREEITEM hItem = m_treePaxType.GetSelectedItem();
	if (hItem == NULL)
	{	
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFIR,FALSE);		

		return;
	}
	ItemData *pItemData =  (ItemData *)m_treePaxType.GetItemData(hItem);
	if (pItemData == NULL || pItemData->m_itemType != IT_PAXTYPE)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFIR,FALSE);	
		return;
	}
	int nPaxTypeIndex = pItemData->m_nIndex;

	HTREEITEM hParentItem = m_treePaxType.GetParentItem(hItem);
	if (hParentItem == NULL)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFIR,FALSE);	
		return;
	}
	ItemData *pParentData = (ItemData *)m_treePaxType.GetItemData(hParentItem);
	if (pParentData == NULL)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFIR,FALSE);	
		return;
	}
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFIR,TRUE);

}
void CReportProperty::OnUpdateUIProcTypeDel()
{
	HTREEITEM hItem = m_treeProcType.GetSelectedItem();
	if (hItem == NULL)
	{
		m_ToolBar2.GetToolBarCtrl().EnableButton(ID_BUTTONDELETESEC,FALSE);
		return;
	}

	ItemData *pItemData =  (ItemData *)m_treeProcType.GetItemData(hItem);
	if (pItemData == NULL || pItemData->m_itemType != IT_PROCTYPE)
	{
		m_ToolBar2.GetToolBarCtrl().EnableButton(ID_BUTTONDELETESEC,FALSE);
		return;
	}
	int nProcTypeIndex = pItemData->m_nIndex;

	HTREEITEM hParentItem = m_treeProcType.GetParentItem(hItem);
	if (hParentItem == NULL)
	{		
		m_ToolBar2.GetToolBarCtrl().EnableButton(ID_BUTTONDELETESEC,FALSE);
		return;
	}
	ItemData *pParentData = (ItemData *)m_treeProcType.GetItemData(hParentItem);
	if (pParentData == NULL)
	{		
		m_ToolBar2.GetToolBarCtrl().EnableButton(ID_BUTTONDELETESEC,FALSE);
		return;
	}	
	m_ToolBar2.GetToolBarCtrl().EnableButton(ID_BUTTONDELETESEC,TRUE);

}
void CReportProperty::OnUpdateUIFromToDel()
{
	HTREEITEM hSelectItem = m_treeProcFromTo.GetSelectedItem();
	if (hSelectItem == NULL)
	{
		m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFROMTO,FALSE);
		return;
	}
	CString strTest = m_treeProcFromTo.GetItemText(hSelectItem);
	ItemData *pItemData =  (ItemData *)m_treeProcFromTo.GetItemData(hSelectItem);
	if (pItemData == NULL)
	{
		m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFROMTO,FALSE);
		return;
	}
	if (pItemData->m_itemType != IT_FROMPROC && pItemData->m_itemType != IT_TOPROC)
	{
		m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFROMTO,FALSE);
		return;
	}
	int nProcIndex = pItemData->m_nIndex;

	HTREEITEM hFromOrToItem = m_treeProcFromTo.GetParentItem(hSelectItem);
	HTREEITEM hModelItem = m_treeProcFromTo.GetParentItem(hFromOrToItem);
	if (hModelItem == NULL)
	{
		m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFROMTO,FALSE);
		return;
	}

	ItemData *pModelData =  (ItemData *)m_treeProcFromTo.GetItemData(hModelItem);
	if (pModelData == NULL)
	{
		m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFROMTO,FALSE);
		return;
	}
	m_ToolBar3.GetToolBarCtrl().EnableButton(ID_BUTTONDELETEFROMTO,TRUE);
}
void CReportProperty::OnUpdateToolBarUI()
{
	OnUpdateUIProcTypeAdd();
	OnUpdateUIFromToAdd();
	OnUpdateUIAreaAdd();
	OnUpdateUIAreaDel();
	OnUpdateUIPaxTypeDel();
	OnUpdateUIProcTypeDel();
	OnUpdateUIFromToDel();
	OnUpdateUIPaxTypeAdd();
}

void CReportProperty::OnTvnSelchangedTreePaxtype(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	OnUpdateUIPaxTypeAdd();
	OnUpdateUIPaxTypeDel();
	*pResult = 0;
}

void CReportProperty::OnTvnSelchangedTreeProcs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	OnUpdateUIProcTypeAdd();
	OnUpdateUIProcTypeDel();

	*pResult = 0;
}

void CReportProperty::OnTvnSelchangedTreeProcfromto(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	OnUpdateUIFromToAdd();
	OnUpdateUIFromToDel();
	*pResult = 0;
}

void CReportProperty::OnTvnSelchangedTreeArea(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	OnUpdateUIAreaAdd();
	OnUpdateUIAreaDel();

	*pResult = 0;
}

void CReportProperty::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	DeleteTreeData(m_treeProcType);
	DeleteTreeData(m_treeProcFromTo);
	DeleteTreeData(m_treeArea);
	DeleteTreeData(m_treePaxType);
	CDialog::OnCancel();
}

BOOL CReportProperty::CheckUserHasAssignedArea()
{
	std::vector<CModelParameter>::iterator itor = m_vModelParam.begin();
	for(; itor!= m_vModelParam.end(); itor++)
	{
		if(!itor->GetArea().IsEmpty())
		{
			return TRUE;
		}
	}
	return FALSE;
}
