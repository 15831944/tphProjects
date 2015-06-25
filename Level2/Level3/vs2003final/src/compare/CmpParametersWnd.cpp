// ParametersWnd.cpp : implementation file
//

#include "stdafx.h"
#include "CmpParametersWnd.h"
#include ".\cmpparameterswnd.h"

#include "CmpReportManager.h"
#include "ReportDef.h"
#include "InputParameter.h"


#include "..\Main\CompRepLogBar.h"
#include "..\main\resource.h"
#include "..\main\ModelSelectionDlg.h"
#include "..\Main\MainFrm.h"
#include "..\Main\ReportProperty.h"

#include "..\common\elaptime.h"
#include "..\common\SimAndReportManager.h"
#include "../compare/ModelToCompare.h"
#include "..\common\EchoSystem.h"

static const UINT ID_RUN = 101;
static const UINT ID_CANCEL = 102;
static const int BUTTON_AREA_HEIGHT = 50;
static const int BUTTON_HEIGHT = 22;
static const int BUTTON_WIDTH = 80;


IMPLEMENT_DYNAMIC(CCmpParametersWnd, CWnd)
CCmpParametersWnd::CCmpParametersWnd()
{
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfHeight = 8;
	strcpy(lf.lfFaceName, "MS Sans Serif");
	m_font.CreateFontIndirect(&lf);

	m_pSubMenu = NULL;

	m_pSelItem = NULL;

	m_pItemBasicInfo = NULL;
	m_pItemName = NULL;
	m_pItemDesc = NULL;

	m_pItemModels = NULL;

	m_pItemReports = NULL;
	m_pItemPassenger = NULL;
	m_pItemProcessor = NULL;
	m_pItemSpace = NULL;
}

CCmpParametersWnd::~CCmpParametersWnd()
{
	if(m_font.m_hObject != NULL)
	{
		m_font.DeleteObject();
		m_font.m_hObject = NULL;
	}
}


BEGIN_MESSAGE_MAP(CCmpParametersWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_RUN, OnRun)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	ON_WM_TIMER()
	
	ON_COMMAND(ID_CRA_ADDNEWMODE, OnCraAddNewModel)
	ON_COMMAND(ID_CRB_DELETE, OnCrbDelete)
	ON_COMMAND(ID_CRC_ADDNEWREPORT, OnCrcAddNewReport)
	ON_COMMAND(ID_CRD_EDIT, OnCrdEditReport)
	ON_COMMAND(ID_CRD_DELETE, OnCrdDelete)
END_MESSAGE_MAP()



// CParametersWnd message handlers


int CCmpParametersWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rtEmpty;
	rtEmpty.SetRectEmpty();

	m_wndPropGrid.Create(rtEmpty, this, 11);

	m_wndPropGrid.SetTheme(xtpGridThemeSimple);
	m_wndPropGrid.ShowHelp(FALSE);
	m_wndPropGrid.SetPropertySort(xtpGridSortNoSort);

	m_btnRun.Create(_T("Run"), WS_VISIBLE|WS_CHILD, rtEmpty, this, ID_RUN);
	m_btnCancel.Create(_T("Cancel"), WS_VISIBLE|WS_CHILD, rtEmpty, this, ID_CANCEL);

	m_btnRun.SetFont(&m_font);
	m_btnCancel.SetFont(&m_font);

	m_nMenu.LoadMenu(IDR_CTX_COMPREPORT);

	return 0;
}

void CCmpParametersWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndPropGrid.m_hWnd))
		m_wndPropGrid.MoveWindow(0, 0, cx, cy - BUTTON_AREA_HEIGHT);

	int x = 0, y = 0;
	x = cx - 15 - BUTTON_WIDTH;
	y = cy - BUTTON_AREA_HEIGHT + 10;
	if (::IsWindow(m_btnCancel.m_hWnd))
		m_btnCancel.MoveWindow(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
	x = cx - (15 + BUTTON_WIDTH)*2;
	if (::IsWindow(m_btnRun.m_hWnd))
		m_btnRun.MoveWindow(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
}

BOOL CCmpParametersWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.top = rectClient.bottom - BUTTON_AREA_HEIGHT;
	pDC->FillSolidRect(&rectClient, ::GetSysColor(COLOR_BTNFACE));

	return CWnd::OnEraseBkgnd(pDC);
}

void CCmpParametersWnd::InitParaWnd()
{
	if (!::IsWindow(m_wndPropGrid.m_hWnd))
		return ;

	m_wndPropGrid.ResetContent();

	m_pItemBasicInfo = m_wndPropGrid.AddCategory(_T("Basic Info"));
	m_pItemName = m_pItemBasicInfo->AddChildItem(new CXTPPropertyGridItem(_T("Name")));
	m_pItemDesc = m_pItemBasicInfo->AddChildItem(new CXTPPropertyGridItem(_T("Description")));
	m_pItemBasicInfo->Expand();

	m_pItemModels = m_wndPropGrid.AddCategory(_T("Models"));

	m_pItemReports = m_wndPropGrid.AddCategory(_T("Reports"));
//	m_pItemPassenger = m_pItemReports->AddChildItem(new CXTPPropertyGridItemCategory(_T("Passenger")));
//	m_pItemProcessor = m_pItemReports->AddChildItem(new CXTPPropertyGridItemCategory(_T("Processor")));
//	m_pItemSpace = m_pItemReports->AddChildItem(new CXTPPropertyGridItemCategory(_T("Space")));
	m_pItemReports->Expand();
}

BOOL CCmpParametersWnd::CheckData()
{
	CInputParameter* pInputParam = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam();
	//	Any models
	if (!pInputParam->GetModelsManagerPtr()->GetModelsList().size())
	{
		AfxMessageBox(_T("Please assign one or more models."));
		return FALSE;
	}

	//	Any Reports
	if (!pInputParam->GetReportsManagerPtr()->GetReportsList().size())
	{
		AfxMessageBox(_T("Please assign one or more reports."));
		return FALSE;
	}

	return TRUE;
}
void CCmpParametersWnd::OnRun()
{
	if (CheckData())
	{
		if (m_pReportManager->GetCmpReport()->SaveProject())
			SetTimer(100, 500, NULL);
		else
			AfxMessageBox(_T("Save data failed!"));
	}
}

CStatusBarXP* __statusBar = NULL;

static void CALLBACK _ShowCopyInfo(LPCTSTR strPath)
{
	CString fileName;
	CString strFilePath = CString(strPath);
	int nPos = strFilePath.ReverseFind('\\');
	fileName = strFilePath.Mid(nPos + 1);
	CString strMsg;
	strMsg = _T("Copying   ") + fileName;
	__statusBar->SetPaneText(0,strMsg);
}
void CCmpParametersWnd::RunCompareReport()
{
	CMainFrame* pFram = (CMainFrame*)AfxGetMainWnd();
	CWnd* pWnd = &(pFram->m_wndCompRepLogBar);
	((CCompRepLogBar*)pWnd)->m_pProj = m_pReportManager->GetCmpReport()->GetComparativeProject();
	((CCompRepLogBar*)pWnd)->SetParentIndex(1);

	pFram->ShowControlBar((CToolBar*) pWnd, TRUE, FALSE);//!bIsShown
	CRect rc1, rc2;
	pFram->m_wndCompRepLogBar.GetWindowRect(&rc1);
	pFram->m_wndCompRepLogBar.GetClientRect(&rc2);
	pFram->ChangeSize(rc1, rc2, 1);

	((CCompRepLogBar*)pWnd)->DeleteLogText();
	__statusBar = &(pFram->m_wndStatusBar);
	m_pReportManager->GetCmpReport()->Run(this->GetSafeHwnd(), (CCompRepLogBar*)pWnd,_ShowCopyInfo);
	((CCompRepLogBar*)pWnd)->SetProgressPos( 0);

	m_pReportManager->DisplayReport();
}

void CCmpParametersWnd::RemoveItem(CXTPPropertyGridItem* pItem)
{
	if(pItem == NULL)
		return ;

	if(pItem->HasChilds())
	{
		CXTPPropertyGridItems* pChilds = pItem->GetChilds();
		for(int i = pChilds->GetCount()-1; i >= 0; i--)
		{
			CString str = pChilds->GetAt(i)->GetCaption();
			if(pChilds->GetAt(i)->HasChilds())
			{
				RemoveItem(pChilds->GetAt(i));
				pChilds->RemoveAt(i);
			}
			else
				pChilds->RemoveAt(i);
		}
	}
}

void CCmpParametersWnd::UpdateParaItem(CXTPPropertyGridItem* pItem)
{
	if(pItem == NULL)
		return;

	if(pItem == m_pItemName)
	{
		pItem->SetValue(m_pReportManager->GetCmpReport()->GetComparativeProject()->GetName());
	}
	else if(pItem == m_pItemDesc)
	{
		pItem->SetValue(m_pReportManager->GetCmpReport()->GetComparativeProject()->GetDescription());
	}
	else if(pItem == m_pItemModels)
	{
		//RemoveItem(m_pItemModels);
		if(pItem->HasChilds())
		{
			CXTPPropertyGridItems* pChilds = pItem->GetChilds();
			for(int i = pChilds->GetCount()-1; i >= 0; i--)
			{
				CString str = pChilds->GetAt(i)->GetCaption();
				RemoveItem(pChilds->GetAt(i));
				pChilds->RemoveAt(i);
			}
		}

		CModelsManager* pManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetModelsManagerPtr();
		for(int i = 0; i < (int)pManager->GetModelsList().size(); i++)
		{
			CString strModel = "";
			strModel.Format("Model-%d", i);
			CXTPPropertyGridItem* pModel = pItem->AddChildItem( new CXTPPropertyGridItemCategory(strModel) );

			CXTPPropertyGridItem* pModelName = pModel->AddChildItem(new CXTPPropertyGridItem(_T("Name")));

			CModelToCompare *pModelToCompare = pManager->GetModelsList().at(i);
			CString strName = pModelToCompare->GetUniqueName();
			pModelName->SetValue( strName );
			pModelName->SetReadOnly(TRUE);

			//add simulation result
			CXTPPropertyGridItem* pSimResultItem = pModel->AddChildItem(new CXTPPropertyGridItem(_T("SimResult")));
			int nSimCount = pModelToCompare->GetSimResultCount();
			CString strSimResult = _T("");
			for (int j = 0; j < nSimCount; ++j)
			{
				strSimResult += pModelToCompare->GetSimResult(j);
				strSimResult += ",";
			}
			strSimResult.TrimRight(_T(","));
			pSimResultItem->SetValue( strSimResult );
			pSimResultItem->SetReadOnly(TRUE);

			CXTPPropertyGridItem* pModelPath = pModel->AddChildItem(new CXTPPropertyGridItem(_T("Path")));
			CString strPath = pModelToCompare->GetModelLocation();
			int nLenName = strName.GetLength();
			int nLenPath = strPath.GetLength();
			strPath = strPath.Left(nLenPath-nLenName-1);
			pModelPath->SetValue( strPath );
			
			pModelPath->SetReadOnly(TRUE);

			pModel->Expand();
		}
		pItem->Expand();
	}
	else if(pItem == m_pItemReports)
	{
		RemoveItem(m_pItemReports);
		//if(pItem->HasChilds())
		//{
		//	CXTPPropertyGridItems* pChilds = pItem->GetChilds();
		//	for(int i = pChilds->GetCount()-1; i >= 0; i--)
		//	{
		//		CString str = pChilds->GetAt(i)->GetCaption();
		//		RemoveItem(pChilds->GetAt(i));
		//	}
		//}
		CReportsManager* pRManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetReportsManagerPtr();
		std::vector<CReportToCompare>& vReports = pRManager->GetReportsList();
		for (int i = 0; i < static_cast<int>(vReports.size()); i++)
		{
			const CReportToCompare& report = vReports.at(i);
			//int nRepNodeIndex = -1;
			//switch (report.GetCategory())
			//{
			//case ENUM_QUEUELENGTH_REP:
			//case ENUM_THROUGHPUT_REP:
			//	nRepNodeIndex = 1;
			//	break;

			//case ENUM_QUEUETIME_REP:
			//case ENUM_DURATION_REP:
			//case ENUM_DISTANCE_REP:
			//	nRepNodeIndex = 0;
			//	break;

			//case ENUM_ACOPERATION_REP:
			//case ENUM_PAXCOUNT_REP:
			//case ENUM_PAXDENS_REP:
			//	nRepNodeIndex = 2;
			//	break;

			//default:
			//	break;
			//}
			//if(nRepNodeIndex < 0)
			//	return;

			//CXTPPropertyGridItems* pChilds = pItem->GetChilds();
			//CXTPPropertyGridItem* pChild = pChilds->GetAt(nRepNodeIndex);
			//pChild = pChild->AddChildItem(new CXTPPropertyGridItemCategory(report.GetName()));
			CXTPPropertyGridItem* pChild = m_pItemReports->AddChildItem(new CXTPPropertyGridItemCategory(report.GetName()));

			CXTPPropertyGridItem* pPItem = NULL;
			CReportParamToCompare param = report.GetParameter();
			CString strTemp;
			strTemp = _T("Report Type");
			pPItem = pChild->AddChildItem(new CXTPPropertyGridItem(strTemp));

			int iIndex = -1;
			switch (report.GetCategory())
			{
			case ENUM_QUEUELENGTH_REP:
				iIndex = 0;
				break;
			case ENUM_QUEUETIME_REP:
				iIndex = 1;
				break;
			case ENUM_THROUGHPUT_REP:
				iIndex = 2;
				break;
			case ENUM_PAXDENS_REP:
				iIndex = 3;
				break;
			case ENUM_PAXCOUNT_REP:
				iIndex = 4;
				break;	
			case ENUM_ACOPERATION_REP:
				iIndex = 5;
				break;
			case ENUM_DURATION_REP:
				iIndex = 6;
				break;
			case ENUM_DISTANCE_REP:
				iIndex = 7;
				break;
			}
			if (iIndex == -1)
			{
				continue;
			}
			strTemp = s_szReportCategoryName[iIndex];
			pPItem->SetValue(strTemp);
			pPItem->SetReadOnly(TRUE);

			strTemp = _T("Start Time");
			pPItem = pChild->AddChildItem(new CXTPPropertyGridItem(strTemp));
			strTemp = GetRegularDateTime(param.GetStartTime().printTime());
			pPItem->SetValue(strTemp);
			pPItem->SetReadOnly(TRUE);

			strTemp = _T("End Time");
			pPItem = pChild->AddChildItem(new CXTPPropertyGridItem(strTemp));
			strTemp = GetRegularDateTime(param.GetEndTime().printTime());
			pPItem->SetValue(strTemp);
			pPItem->SetReadOnly(TRUE);

			strTemp = _T("Interval");
			pPItem = pChild->AddChildItem(new CXTPPropertyGridItem(strTemp));
			strTemp = param.GetInterval().printTime();
			pPItem->SetValue(strTemp);
			pPItem->SetReadOnly(TRUE);

			//write Model Parameter
			std::vector<CModelParameter> vModelParam;
			int nModelParamCount = param.GetModelParameter(vModelParam);
			
			CModelsManager* pModelManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetModelsManagerPtr();

			for (int nModelParam = 0; nModelParam< nModelParamCount; ++nModelParam)
			{
				CModelParameter& modelParam = vModelParam[nModelParam];

				CString strModelName = pModelManager->GetModelsList().at(nModelParam)->GetModelName();

				CXTPPropertyGridItem* pModelItem = pChild->AddChildItem(new CXTPPropertyGridItemCategory(strModelName));
//				pModelItem->SetReadOnly(TRUE);

				if(iIndex == 3)
				{
					strTemp = _T("Areas");
					pPItem = pModelItem->AddChildItem(new CXTPPropertyGridItem(strTemp));
					strTemp = modelParam.GetArea();
					pPItem->SetValue(strTemp);
					pPItem->SetReadOnly(TRUE);
				}

				if(report.GetCategory() == ENUM_QUEUETIME_REP ||
					report.GetCategory() == ENUM_DURATION_REP ||
					report.GetCategory() == ENUM_DISTANCE_REP ||
					report.GetCategory() == ENUM_ACOPERATION_REP||
					report.GetCategory() == ENUM_PAXDENS_REP )
				{
					std::vector<CMobileElemConstraint> vPaxType;
					if (modelParam.GetPaxType(vPaxType))
					{
						strTemp = _T("Passanger Type");
						pPItem = pModelItem->AddChildItem(new CXTPPropertyGridItemCategory(strTemp));

						CString strPax;
						for (int i = 0; i < static_cast<int>(vPaxType.size()); i++)
						{
							vPaxType[i].screenPrint(strPax);
							CXTPPropertyGridItem* pTempItem = pPItem->AddChildItem(new CXTPPropertyGridItem(""));
							pTempItem->SetValue(strPax);
							pTempItem->SetReadOnly(TRUE);
						}
						pPItem->Expand();
					}	
				}
				if (report.GetCategory() == ENUM_DURATION_REP ||
					report.GetCategory() == ENUM_DISTANCE_REP)
				{
					// from or to processor

					CReportParameter::FROM_TO_PROCS _fromtoProcs;
					modelParam.GetFromToProcs(_fromtoProcs);

					strTemp = _T("From To Processors");
					CXTPPropertyGridItem *pFromToItem = pModelItem->AddChildItem(new CXTPPropertyGridItemCategory(strTemp));
					CXTPPropertyGridItem *pFromItem = pFromToItem->AddChildItem(new CXTPPropertyGridItemCategory("From"));
					CXTPPropertyGridItem *pToItem = pFromToItem->AddChildItem(new CXTPPropertyGridItemCategory("To"));

					for (int nFrom = 0; nFrom < (int)_fromtoProcs.m_vFromProcs.size(); ++ nFrom)
					{
						CString strProc = _fromtoProcs.m_vFromProcs.at(nFrom).GetIDString();
						CXTPPropertyGridItem* pTempItem = pFromItem->AddChildItem(new CXTPPropertyGridItem(""));
						pTempItem->SetValue(strProc);
						pTempItem->SetReadOnly(TRUE);
					}
				
					for (int nTo = 0; nTo < (int)_fromtoProcs.m_vToProcs.size(); ++ nTo)
					{
						CString strProc = _fromtoProcs.m_vToProcs.at(nTo).GetIDString();
						CXTPPropertyGridItem* pTempItem = pToItem->AddChildItem(new CXTPPropertyGridItem(""));
						pTempItem->SetValue(strProc);
						pTempItem->SetReadOnly(TRUE);

					}
				}
				else if(report.GetCategory() != ENUM_ACOPERATION_REP)
				{
					std::vector<ProcessorID> vProcGroup;
					if (modelParam.GetProcessorID(vProcGroup))
					{
						char szProc[128];
						strTemp = _T("Processor Type");
						pPItem = pModelItem->AddChildItem(new CXTPPropertyGridItemCategory(strTemp));

						for (int i = 0; i < static_cast<int>(vProcGroup.size()); i++)
						{
							memset(szProc, 0, sizeof(szProc) / sizeof(char));
							vProcGroup[i].printID(szProc);
							CXTPPropertyGridItem* pTempItem = pPItem->AddChildItem(new CXTPPropertyGridItem(""));
							pTempItem->SetValue(szProc);
							pTempItem->SetReadOnly(TRUE);
						}
						pPItem->Expand();
					}
				}



				pModelItem->Expand();
			}

		
			pChild->Expand();
			pChild->GetParentItem()->Expand();
		}
		pItem->Expand();
	}

	m_pReportManager->GetCmpReport()->SetModifyFlag(TRUE);
	m_pReportManager->GetCmpReport()->SaveProject();
}

void CCmpParametersWnd::UpdateParaWnd()
{
	if (!::IsWindow(m_wndPropGrid.m_hWnd))
		return ;

	UpdateParaItem(m_pItemName);
	UpdateParaItem(m_pItemDesc);
	UpdateParaItem(m_pItemModels);
	UpdateParaItem(m_pItemReports);
}

LRESULT CCmpParametersWnd::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
	if(pItem == NULL)
		return 0;

	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		if(pItem == m_pItemName)
		{
			if(m_pReportManager->GetCmpReport()->ProjExists(pItem->GetValue()))
			{
				ReleaseCapture();
				MessageBox("The name is already exists, please rename!");
				pItem->SetValue(m_pReportManager->GetCmpReport()->GetOriginProjName());
				return 0;
			}

			CComparativeProject *pProject = m_pReportManager->GetCmpReport()->GetComparativeProject();

			pProject->SetName(pItem->GetValue());
//			m_pReportManager->GetCmpReport()->GetComparativeProject()->SetName(pItem->GetValue());

		}
		else if(pItem == m_pItemDesc)
		{
			if(m_pReportManager->GetCmpReport()->GetComparativeProject()->GetName().IsEmpty())
			{
				MessageBox("The name is empty, please set the project name!");
				ReleaseCapture();
				pItem->SetValue(_T(""));
				return 0;
			}
			m_pReportManager->GetCmpReport()->GetComparativeProject()->SetDescription(pItem->GetValue());
		}
		m_pReportManager->GetCmpReport()->SetModifyFlag(TRUE);
		m_pReportManager->GetCmpReport()->SaveProject();
	}
	else if(wParam == XTP_PGN_RCLICK)
	{
		m_pSelItem = pItem;
		m_pSelItem->Select();

		CPoint point;
		GetCursorPos(&point);
		CMenu* pMenu=NULL;

		if(pItem != NULL)
		{
			if(pItem == m_pItemModels)
			{
				CComparativeProject *pProject = m_pReportManager->GetCmpReport()->GetComparativeProject();

				if(pProject == NULL ||pProject->GetName().IsEmpty())
				{
					MessageBox("The name is empty, please set the project name!");
					return 0;
				}
				pMenu = m_nMenu.GetSubMenu(0);
			}
			else if(pItem == m_pItemReports)
			{
				if(m_pReportManager->GetCmpReport()->GetComparativeProject()->GetName().IsEmpty())
				{
					MessageBox("The name is empty, please set the project name!");
					return 0;
				}
				pMenu = m_nMenu.GetSubMenu(2);
			}
			else if(pItem->GetParentItem() == m_pItemModels)
			{
				pMenu = m_nMenu.GetSubMenu(1);
			}
			else if(pItem->GetParentItem() != NULL)
			{
				if(pItem->GetParentItem() == m_pItemReports)
				{
					pMenu = m_nMenu.GetSubMenu(3);
				}
			}
		}

		if (pMenu != NULL)
		{
			pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
		}
	}

	return 0;
}

CString CCmpParametersWnd::GetRegularDateTime(LPCTSTR elaptimestr, bool needsec)
{
	CModelsManager* pModelManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetModelsManagerPtr();

	if((int)pModelManager->GetModelsList().size()  == 0)
		return "";

	Terminal *pTerminal = pModelManager->GetModelsList().at(0)->GetTerminal();

	CSimAndReportManager *ptSim = pTerminal->GetSimReportManager();
	CStartDate tstartdate = ptSim->GetStartDate();
	ElapsedTime etime;
	etime.SetTime(elaptimestr);

	CString retstr, tstr;
	bool bAbsDate;
	COleDateTime _tdate, _ttime;
	int _nDtIdx;
	tstartdate.GetDateTime(etime, bAbsDate, _tdate, _nDtIdx, _ttime);
	if(!needsec)
		tstr = _ttime.Format(" %H:%M");
	else
		tstr = _ttime.Format(" %H:%M:%S");
	if(bAbsDate)
	{
		retstr = _tdate.Format("%Y-%m-%d");
	}
	else
	{
		retstr.Format("Day%d", _nDtIdx + 1 );
	}
	retstr += tstr;

	return retstr;
}

void CCmpParametersWnd::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 100)
	{
		KillTimer(nIDEvent);
		RunCompareReport();
	}

	CWnd::OnTimer(nIDEvent);
}

void CCmpParametersWnd::OnCraAddNewModel()
{
	CWaitCursor wc;

	CModelsManager* pManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetModelsManagerPtr();
//	dlg.GetModelsManager().SetModels(pManager->GetModelsList());
	CModelSelectionDlg dlg(pManager,this);
	wc.Restore();
	if(dlg.DoModal() == IDOK)
	{
	//	pManager->SetModels(dlg.GetModelsManager().GetModelsList());

		UpdateParaItem(m_pItemModels);
	}
}

void CCmpParametersWnd::OnCrbDelete() 
{
	CXTPPropertyGridItems* pChilds = m_pSelItem->GetChilds();
	CXTPPropertyGridItem* pChild = pChilds->GetAt(0);

	CString strModelUniqueName =  pChild->GetValue();

	strModelUniqueName.Trim();
	// TODO: Add your command handler code here
	if(m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->DeleteModel(strModelUniqueName))
	{
		UpdateParaItem(m_pItemModels);
		UpdateParaItem(m_pItemReports);
	}
}

void CCmpParametersWnd::OnCrcAddNewReport()
{
	CWaitCursor wc;
	//if (!m_pReportManager->GetCmpReport()->InitTerminal())
	//{
	//	AfxMessageBox(_T("Initialize terminal failed."));
	//	wc.Restore();
	//	return ;
	//}

	CReportProperty dlg(this);
	dlg.m_strProjName = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetName();
//	dlg.m_pTerminal = &(m_pReportManager->GetCmpReport()->GetTerminal());
	CModelsManager* pMManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetModelsManagerPtr();
	CString strError = pMManager->InitTerminal(NULL,dlg.m_strProjName,NULL);
	if (!strError.IsEmpty())
	{
		AfxMessageBox(_T("Cann't load project:") + strError);
		return;
	}
	
	CReportsManager* pRManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetReportsManagerPtr();

	dlg.SetManager(pMManager,pRManager);

	dlg.SetProjName(dlg.m_strProjName);
	wc.Restore();
	if(dlg.HasModelInLocation() == CReportProperty::MT_NOMODEL)
	{
		MessageBox("No valid model, please add one at least!");
		return ;
	}

	if(dlg.DoModal() == IDOK)
	{
		const CReportToCompare& report = dlg.GetReport();

		pRManager->AddReport(report);

		UpdateParaItem(m_pItemReports);
	}
}

void CCmpParametersWnd::OnCrdEditReport()
{
	CWaitCursor wc;
	//if (!m_pReportManager->GetCmpReport()->InitTerminal())
	//{
	//	AfxMessageBox(_T("Initialize terminal failed."));
	//	wc.Restore();
	//	return ;
	//}

	BOOL bFound = FALSE;
	CModelsManager* pMManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetModelsManagerPtr();
	CReportsManager* pRManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetReportsManagerPtr();
	std::vector<CReportToCompare>& vReports = pRManager->GetReportsList();
	std::vector<CReportToCompare>::iterator iter;
	CReportToCompare report;
	for (iter = vReports.begin(); iter != vReports.end(); iter++)
	{
		if (iter->GetName() == m_pSelItem->GetCaption())
		{
			report = *iter;
			bFound = TRUE;
			break;
		}
	}

	if (bFound)
	{
		CReportProperty dlg(this);
		dlg.m_strProjName = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetName();
//		dlg.m_pTerminal = &(m_pReportManager->GetCmpReport()->GetTerminal());
		dlg.SetManager(pMManager,pRManager);

		dlg.SetProjName(dlg.m_strProjName);
		dlg.GetReport() = report;
		wc.Restore();

		if(dlg.DoModal() == IDOK)
		{
			const CReportToCompare& report = dlg.GetReport();
			
			vReports.erase(iter);

			pRManager->AddReport(report);

			UpdateParaItem(m_pItemReports);
		}
	}
}

void CCmpParametersWnd::OnCrdDelete() 
{
	// TODO: Add your command handler code here
	CReportsManager* pRManager = m_pReportManager->GetCmpReport()->GetComparativeProject()->GetInputParam()->GetReportsManagerPtr();

	std::vector<CReportToCompare>& vReports = pRManager->GetReportsList();
	std::vector<CReportToCompare>::iterator iter;
	for (iter = vReports.begin(); iter != vReports.end(); iter++)
	{
		if (iter->GetName() == m_pSelItem->GetCaption())
		{
			vReports.erase(iter);
			break;
	    }
	}

	UpdateParaItem(m_pItemReports);
}