// MutiRunRepControlView.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "MultiRunRepControlView.h"
#include "TermPlanDoc.h"
#include "../Reports/ReportParaDB.h"
#include "../Reports/ReportParameter.h"
#include ".\multirunrepcontrolview.h"
#include "DlgMultiReportSelectModel.h"
#include "ViewMsg.h"


IMPLEMENT_DYNCREATE(CMultiRunRepControlView, CFormView)

CMultiRunRepControlView::CMultiRunRepControlView()
 : CFormView(IDD_FORMVIEW_MULTIRUNSREPORT_PARAMETER)
{
}

CMultiRunRepControlView::~CMultiRunRepControlView()
{
}

void CMultiRunRepControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUPBOX_REPORTPARAS, m_wndReportParasGroupBox);
	DDX_Control(pDX, IDC_EDIT_REPORTPARAS, m_wndReportParameters);
	DDX_Control(pDX, IDC_GROUPBOX_SIMRESULTS, m_wndSimResultsGroupBox);
	DDX_Control(pDX, IDC_TREE_SIMRESULTS, m_wndTreeSimResults);



//	DDX_Control(pDX, IDC_LIST_SIMRESULTS, m_wndSimResultsList);
//	DDX_Control(pDX, IDC_CHECK_ALLSIMRESULTS, m_wndAllSimResultsCheckBox);
}

BEGIN_MESSAGE_MAP(CMultiRunRepControlView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnApply)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_ALLSIMRESULTS, OnCheckAllSimResults)
	ON_WM_CREATE()
	ON_COMMAND(ID_PROJECT_DATA_ADD,OnToolBarAddProject)
	ON_COMMAND(ID_PROJECT_DATA_DEL,OnToolBarDelProject)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SIMRESULTS, OnTvnSelchangedTreeSimresults)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_DATA_DEL,OnToolBarBtnDel)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_DATA_DEL,OnToolBarBtnAdd)
END_MESSAGE_MAP()

// CMultiRunRepControlView message handlers

void CMultiRunRepControlView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	InitToolBar();

	CTermPlanDoc* pDoc = (CTermPlanDoc*)(GetDocument());

	pDoc->GetARCReportManager().getMultiReport().LoadReport(pDoc->GetARCReportManager().GetMultiRunsReportType(),pDoc->m_ProjInfo.path);
	if (pDoc->GetARCReportManager().getMultiReport().GetCompareModelList().GetCount() == 0)
	{
		//init the current project
		
		CCompareModel model;
		model.SetModelName(pDoc->m_ProjInfo.name);
		model.SetModelLocation(pDoc->m_ProjInfo.path);

		int nCount = GetSimCountOfModel(pDoc->m_ProjInfo.path);
		for (int i =0; i < nCount; ++i)
		{
			CString strRunName;
			strRunName.Format(_T("RUN%d"),i);
			model.AddSimResult(strRunName);
		}
		pDoc->GetARCReportManager().getMultiReport().GetCompareModelList().AddModel(model);
	}

	UpdateControlContents();

	CRect rectClient;
	GetClientRect(&rectClient);
	OnSize(0, rectClient.Width(), rectClient.Height());
	
	m_wndTreeSimResults.EnableMutiTree();

}


void CMultiRunRepControlView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	UpdateControlContents();
}

void CMultiRunRepControlView::UpdateControlContents()
{
	CString strParameters;
	GenerateParametersText(strParameters);
	m_wndReportParameters.SetWindowText(strParameters);

	//// set the content of the simulation result list window
	m_wndTreeSimResults.DeleteAllItems();

	// add sim_reslut to list box
	CTermPlanDoc* pDoc = (CTermPlanDoc*)(GetDocument());
	//int iSimResultIndex = pDoc->GetTerminal().getCurrentSimResult();
	//if (iSimResultIndex >= 0)
	//{
	//	for( int i = 0; i< pDoc->getSimResultCout(); i++ )
	//	{
	//		CString strNum;
	//		strNum.Format("(%d)", i);

	////		// add string to sim_result list
	////		int iListIdx = m_wndSimResultsList.AddString(pDoc->GetTitle() + strNum );
	////		m_wndSimResultsList.SetCheck(iListIdx, BST_CHECKED);
	//	}
	//}
	
	m_wndTreeSimResults.SetRedraw(FALSE);
	{		
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this, cni);

		CMultiRunReport& multiReport = pDoc->GetARCReportManager().getMultiReport();

		CCompareModelList lstCompareModel = multiReport.GetCompareModelList();

		int modelCount = lstCompareModel.GetCount();
		for (int i=0;i < modelCount; ++i)
		{
			CCompareModel compareModel = lstCompareModel.GetModel(i);



			cni.nt = NT_CHECKBOX;
			HTREEITEM hRoot = m_wndTreeSimResults.InsertItem(compareModel.GetModelName(), cni, FALSE);
			m_wndTreeSimResults.SetItemData(hRoot,(DWORD)(int)ITEM_MODEL);
			m_wndTreeSimResults.SetCheckStatus(hRoot,TRUE);

			int nSelCount = compareModel.GetSimResultCount();
			int nSimResultCount = GetSimCountOfModel(compareModel.GetModelLocation());
			for (int j =0; j < nSimResultCount; ++j)
			{
				CString strItemText;
				strItemText.Format(_T("RUN%d"),j);
				HTREEITEM hRunItem =  m_wndTreeSimResults.InsertItem(strItemText, cni, FALSE, FALSE, hRoot);
				m_wndTreeSimResults.SetItemData(hRunItem,(DWORD)(int)ITEM_RUNRES);
				bool bFind = false;
				for (int nSel =0; nSel < nSelCount; ++nSel)
				{
					if (compareModel.GetSimResult(nSel) == strItemText)
					{
						bFind = true;
						break;
					}
				}
				
				if (bFind)
				{
					m_wndTreeSimResults.SetCheckStatus(hRunItem,TRUE);				
				}
			}

	
			m_wndTreeSimResults.Expand(hRoot, TVE_EXPAND);
		}

	}
	m_wndTreeSimResults.SetRedraw(TRUE);

	//m_wndAllSimResultsCheckBox.SetCheck(BST_CHECKED);
}

void CMultiRunRepControlView::OnApply()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)(this->GetDocument());
	std::vector<int>& vSimResultList = pDoc->GetARCReportManager().GetmSelectedMultiRunsSimResults();
	vSimResultList.clear();
	CMultiRunReport& multRunReport = pDoc->GetARCReportManager().getMultiReport();

	CCompareModelList& compareModelList = multRunReport.GetCompareModelList();

	{
	 	HTREEITEM	hRoot = m_wndTreeSimResults.GetRootItem();
		if (hRoot == NULL)
			return;

		int i =	0;
		do 
		{
			
			std::vector<CString> vSelectRunResult;
			HTREEITEM hChildItem = m_wndTreeSimResults.GetChildItem(hRoot);
			int j =0;
			do 
			{
				if (m_wndTreeSimResults.IsCheckItem(hChildItem))
				{
					vSelectRunResult.push_back(m_wndTreeSimResults.GetItemText(hChildItem));
					if(i == 0)
						vSimResultList.push_back(j);
				}

				j += 1;

			} while(hChildItem = m_wndTreeSimResults.GetNextSiblingItem(hChildItem));

			compareModelList.SetModelSelectedSimResult(i,vSelectRunResult);
			i +=1;
		} while(hRoot = m_wndTreeSimResults.GetNextSiblingItem(hRoot));
	}


	//std::vector<int>& vSimResultList = pDoc->GetmSelectedMultiRunsSimResults();
	//vSimResultList.clear();

	//int nSimCount = m_wndSimResultsList.GetCount();
	//for (int i = 0; i < nSimCount; ++i)
	//{
	//	if (m_wndSimResultsList.GetCheck(i) > 0)
	//	{
	//		vSimResultList.push_back(i);
	//	}
	//}
	CReportParameter* pReportParam = pDoc->GetTerminal().m_pReportParaDB->GetParameterbyType(pDoc->GetARCReportManager().GetMultiRunsReportType());
	if (pReportParam == NULL)
		return;
	int iDetailed;
	pReportParam->GetReportType(iDetailed);
	multRunReport.SaveReport(pDoc->GetARCReportManager().GetMultiRunsReportType(),pDoc->m_ProjInfo.path);
	multRunReport.GenerateReport(pDoc->GetARCReportManager().GetMultiRunsReportType(),iDetailed);
	pDoc->UpdateAllViews(this,MULTIREPORT_SHOWREPORT,NULL);
}

void CMultiRunRepControlView::OnSize(UINT nType, int cx, int cy)
{
	if (m_wndReportParameters.GetSafeHwnd() == NULL)
		return;	
	
	// set the position of apply button
	CRect rectApply;
	CWnd* pApplyButton = GetDlgItem(IDC_BUTTON_APPLY);
	pApplyButton->GetClientRect(rectApply);
	rectApply.MoveToXY(cx - 10 - rectApply.Width(), cy - 10 - rectApply.Height());
	pApplyButton->MoveWindow(rectApply);

	// set the report parameters group box
	CRect rectReportParas(0, 0, cx, cy - rectApply.Height());
	rectReportParas.bottom /= 2;
	rectReportParas.DeflateRect(10, 10);
	m_wndReportParasGroupBox.MoveWindow(rectReportParas);

	rectReportParas.DeflateRect(10, 10);
	rectReportParas.top += 5;
	m_wndReportParameters.MoveWindow(rectReportParas);

	CRect rectSimResults(0, 0, cx, cy- rectApply.Height());
	rectSimResults.top += rectSimResults.Height() / 2;
	rectSimResults.bottom -= 10;
	rectSimResults.DeflateRect(10, 10);
	m_wndSimResultsGroupBox.MoveWindow(rectSimResults);

	CRect rectCheckBox(rectSimResults);
	rectCheckBox.top += 15;
	rectCheckBox.bottom = rectCheckBox.top + 20;
	rectCheckBox.left += 10;
	rectCheckBox.right = rectCheckBox.left + 80;
	m_toolBar.MoveWindow(rectCheckBox);

	rectSimResults.DeflateRect(10, 10);
	rectSimResults.top += 30;
	m_wndTreeSimResults.MoveWindow(rectSimResults);

}

void CMultiRunRepControlView::OnCheckAllSimResults()
{
	//int nCheckState = m_wndAllSimResultsCheckBox.GetCheck();

	//int nCount = m_wndSimResultsList.GetCount();
	//for (int i = 0; i < nCount; ++i)
	//	m_wndSimResultsList.SetCheck(i, nCheckState);
}

void CMultiRunRepControlView::GenerateParametersText(CString& strParameters)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)(this->GetDocument());
	Terminal& terminal = pDoc->GetTerminal();

	CReportParameter* pReportParam = terminal.m_pReportParaDB->GetParameterbyType(pDoc->GetARCReportManager().GetMultiRunsReportType());
	if (pReportParam == NULL)
		pReportParam = terminal.m_pReportParaDB->BuildReportParam(pDoc->GetARCReportManager().GetMultiRunsReportType());

	ASSERT(pReportParam != NULL);

	ElapsedTime time;
	long lValue;
	CString strTemp;

	// Start time
	if (pReportParam->GetStartTime(time))
	{
		strTemp = time.printTime();
		strParameters += _T("Start Time: ");
		strParameters += strTemp;
		strParameters += _T("\r\n");
	}

	// End time
	if (pReportParam->GetEndTime(time))
	{
		strTemp = time.printTime();
		strParameters += _T("End Time: ");
		strParameters += strTemp;
		strParameters += _T("\r\n");
	}

	// Interval
	if (pReportParam->GetInterval(lValue))
	{
		strTemp.Format(_T("%d"), lValue);
		strParameters += _T("Interval: ");
		strParameters += strTemp;
		strParameters += _T("\r\n");
	}
/*
	// Threshold
	if (pReportParam->GetThreshold(lValue))
	{
		strTemp.Format(_T("%d"), lValue);
		strParameters += _T("Threshold: ");
		strParameters += strTemp;
		strParameters += _T("\r\n");
	}
*/
	// Passenger type
	std::vector<CMobileElemConstraint> vPaxType;
	if (pReportParam->GetPaxType(vPaxType))
	{
		if (!vPaxType.empty())
		{
			strParameters += _T("Passenger Type: ");
			strParameters += _T("\r\n");
			for (size_t i = 0; i < vPaxType.size(); i++)
			{
				vPaxType[i].screenPrint(strTemp);
				strParameters += _T("    ");
				strParameters += strTemp;
				strParameters += _T("\r\n");
			}
		}
	}

	// Processor group
	std::vector<ProcessorID> vProcGroup;
	if (pReportParam->GetProcessorGroup(vProcGroup))
	{
		if (!vProcGroup.empty())
		{
			strParameters += _T("Processors: ");
			strParameters += _T("\r\n");
			for (size_t i = 0; i < vProcGroup.size(); i++)
			{
				strParameters += _T("    ");
				strParameters += vProcGroup[i].GetIDString();
				strParameters += _T("\r\n");
			}
		}
	}

	// Areas
	std::vector<CString> vAreas;
	if (pReportParam->GetAreas(vAreas))
	{
		if (!vAreas.empty())
		{
			strParameters += _T("Areas: ");
			strParameters += _T("\r\n");
			for (size_t i = 0; i < vAreas.size(); i++)
			{
				strParameters += _T("    ");
				strParameters += vAreas[i];
				strParameters += _T("\r\n");
			}
		}
	}

	// Portals
	std::vector<CString> vPortals;
	if (pReportParam->GetPortals(vPortals))
	{
		if (!vPortals.empty())
		{
			strParameters += _T("Portals: ");
			strParameters += _T("\r\n");
			for (size_t i = 0; i < vPortals.size(); i++)
			{
				strParameters += _T("    ");
				strParameters += vPortals[i];
				strParameters += _T("\r\n");
			}
		}
	}

	// From to Processors
	CReportParameter::FROM_TO_PROCS fromToProcs;
	if (pReportParam->GetFromToProcs(fromToProcs))
	{
		if (!fromToProcs.m_vFromProcs.empty())
		{
			strParameters += _T("From Processors: ");
			strParameters += _T("\r\n");
			for (size_t i = 0; i < fromToProcs.m_vFromProcs.size(); i++)
			{
				strParameters += _T("    ");
				strParameters += fromToProcs.m_vFromProcs[i].GetIDString();
				strParameters += _T("\r\n");
			}
		}

		if (!fromToProcs.m_vToProcs.empty())
		{
			strParameters += _T("To Processors: ");
			strParameters += _T("\r\n");
			for (size_t i = 0; i < fromToProcs.m_vToProcs.size(); i++)
			{
				strParameters += _T("    ");
				strParameters += fromToProcs.m_vToProcs[i].GetIDString();
				strParameters += _T("\r\n");
			}
		}
	}

}

BOOL CMultiRunRepControlView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CMultiRunRepControlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE|CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_ADDDEL_PROJECT))
	{
		return -1;
	}

	return 0;
}
void CMultiRunRepControlView::InitToolBar()
{
	m_toolBar.GetToolBarCtrl().EnableButton(ID_PROJECT_DATA_ADD,TRUE);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_PROJECT_DATA_DEL,FALSE);
}
void CMultiRunRepControlView::OnToolBarAddProject()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)(this->GetDocument());
	CDlgMultiReportSelectModel modelSelDialog(pDoc->GetARCReportManager().getMultiReport().GetCompareModelList());

	if(modelSelDialog.DoModal()==IDOK)
	{
		pDoc->GetARCReportManager().getMultiReport().GetCompareModelList() = modelSelDialog.m_lstCompareModel;
		UpdateControlContents();
	}

}
void CMultiRunRepControlView::OnToolBarDelProject()
{
	HTREEITEM hSelItem = m_wndTreeSimResults.GetSelectedItem();
	if (hSelItem == NULL)
		return;
	long nItemType = (long)m_wndTreeSimResults.GetItemData(hSelItem);
	if (nItemType == ITEM_MODEL)
	{
		if (m_wndTreeSimResults.GetRootItem() != hSelItem)
		{
			CString strItemText = m_wndTreeSimResults.GetItemText(hSelItem);
			CTermPlanDoc* pDoc = (CTermPlanDoc*)(this->GetDocument());
			pDoc->GetARCReportManager().getMultiReport().GetCompareModelList().DelModel(strItemText);

			m_wndTreeSimResults.DeleteItem(hSelItem);
		}
	}

}
int  CMultiRunRepControlView::GetSimCountOfModel(CString strPath)
{
	const static int BufferSize = 10240;

	strPath +=_T("\\INPUT\\SimAndReport.sim");

	std::ifstream fpIn;
	fpIn.open(strPath,std::ios::in);
	if (fpIn.bad())
	{
		return 0;
	}
	char szBuffer[BufferSize] = {0};
	fpIn.getline(szBuffer,BufferSize);
	CString strFileHeader(szBuffer);
	int nPos =  strFileHeader.Find(",");

	CString strVersion = strFileHeader.Mid(nPos+1);

	double fVersion = ::atof(strVersion);
	if (fVersion <= 2.41 ) //version 2.40
	{
		for (int i=0; i< 5; ++i)
		{
			fpIn.getline(szBuffer,BufferSize);
		}

		//simresults
		memset(szBuffer,0,BufferSize);

		fpIn.getline(szBuffer,BufferSize);

		int nSimResults= ::atoi(szBuffer);
		if (nSimResults ==0)
		{
			return 0;
		}
		//sim result
		memset(szBuffer,0,BufferSize);
		fpIn.getline(szBuffer,BufferSize);
		int nSimResult = atoi(szBuffer);

		return nSimResult;

	}
	return 0;

}
void CMultiRunRepControlView::OnTvnSelchangedTreeSimresults(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItemSeled=pNMTreeView->itemNew.hItem;
	if(hItemSeled)
	{
		long nItemType = (long)m_wndTreeSimResults.GetItemData(hItemSeled);
		if (nItemType == ITEM_MODEL)
		{
			m_toolBar.GetToolBarCtrl().EnableButton(ID_PROJECT_DATA_DEL,TRUE);
		}
		else
		{
			m_toolBar.GetToolBarCtrl().EnableButton(ID_PROJECT_DATA_DEL,FALSE);
		}
	}
	*pResult = 0;
}
void CMultiRunRepControlView::OnToolBarBtnDel(CCmdUI* pCmdUI)
{
	HTREEITEM hItemSeled= m_wndTreeSimResults.GetSelectedItem();
	if(hItemSeled)
	{
		long nItemType = (long)m_wndTreeSimResults.GetItemData(hItemSeled);
		//MessageBox("error position");
		if (nItemType == ITEM_MODEL)
		{
			pCmdUI->Enable(TRUE);
			//m_toolBar.GetToolBarCtrl().EnableButton(ID_PROJECT_DATA_DEL,TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
			//m_toolBar.GetToolBarCtrl().EnableButton(ID_PROJECT_DATA_DEL,FALSE);
		}
	}


}
void CMultiRunRepControlView::OnToolBarBtnAdd(CCmdUI* pCmdUI)
{
			m_toolBar.GetToolBarCtrl().EnableButton(ID_PROJECT_DATA_ADD,TRUE);



}