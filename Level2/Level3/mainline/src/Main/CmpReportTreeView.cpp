#include "stdafx.h"
#include "TermPlan.h"
#include "CompareReportDoc.h"
#include "CmpReportTreeView.h"

#include "..\compare\ReportDef.h"
#include "..\compare\InputParameter.h"
#include "..\compare\ModelToCompare.h"
#include "..\compare\CompareReportZipManager.h"
#include "..\Main\CompRepLogBar.h"
#include "..\main\resource.h"
#include "..\main\ModelSelectionDlg.h"
#include "..\Main\MainFrm.h"
#include "..\Main\ReportProperty.h"

#include "..\common\elaptime.h"
#include "..\common\SimAndReportManager.h"
#include "..\common\EchoSystem.h"


static const UINT ID_RUN = 101;
static const UINT ID_CANCEL = 102;
static const UINT ID_MAIN_TREE = 103;
static const UINT ID_BTN_MULTI = 104;
static const UINT MENU_ADD_MODEL = 200;
static const UINT MENU_ADD_REPORT = 203;
static const UINT MENU_DELETE_MODEL = 206;
static const UINT MENU_EDIT_REPORT = 207;
static const UINT MENU_DELETE_REPORT = 208;
static const UINT MENU_DEL_ALL_MODEL = 209;
static const UINT MENU_UNAVAILABLE = 220;
static const UINT SAVEPARATOFILE = 221;
static const UINT LOADPARAFROMFILE = 222;
static const UINT SAVEREPORTTOFILE = 223;
static const UINT LOADREPORTFROMFILE = 224;

static const int BUTTON_AREA_HEIGHT = 50;
static const int BUTTON_HEIGHT = 22;
static const int BUTTON_WIDTH = 80;

typedef enum 
{
	CMP_REPORT_TN_INVALID = -1,
	CMP_REPORT_TN_MODEL = 0,
	CMP_REPORT_TN_SIMRESULT_ALL,
	CMP_REPORT_TN_SIMRESULT,
	CMP_REPORT_TN_REPORT
} CMP_REPORT_TREE_NODE_DATA_TYPE;
class CmpReportTreeNodeDataWithType
{
public:
	CMP_REPORT_TREE_NODE_DATA_TYPE m_type;
	DWORD m_data;
	CmpReportTreeNodeDataWithType(){ m_type = CMP_REPORT_TN_INVALID; m_data = NULL; }
	~CmpReportTreeNodeDataWithType(){}
};

const CString strBtnTxt[] =
{
	"Save Para",
	"Load Para",
	"Save Report",
	"Load Report"
};

IMPLEMENT_DYNCREATE(CCmpReportTreeView, CFormView)
BEGIN_MESSAGE_MAP(CCmpReportTreeView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_RUN, OnRun)
	ON_WM_TIMER()

	ON_COMMAND_RANGE(MENU_ADD_MODEL, MENU_UNAVAILABLE, OnChooseMenu)
	ON_COMMAND(ID_REPORT_SAVE_PARA, OnReportSavePara)
	ON_COMMAND(ID_REPORT_LOAD_PARA, OnReportLoadPara)
	ON_COMMAND(ID_REPORT_SAVE_REPORT, OnReportSaveReport)
	ON_COMMAND(ID_REPORT_LOAD_REPORT, OnReportLoadReport)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

CCmpReportTreeView::CCmpReportTreeView()
	: CFormView(CCmpReportTreeView::IDD)
{
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfHeight = 8;
	strcpy(lf.lfFaceName, "MS Sans Serif");
	m_font.CreateFontIndirect(&lf);
	m_pSubMenu = NULL;
	m_pCmpReport = NULL;
}

CCmpReportTreeView::~CCmpReportTreeView()
{
	if(m_font.m_hObject != NULL)
	{
		m_font.DeleteObject();
		m_font.m_hObject = NULL;
	}
}

void CCmpReportTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}
void CCmpReportTreeView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

#ifdef _DEBUG
void CCmpReportTreeView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmpReportTreeView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

int CCmpReportTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect emptyRect;
	emptyRect.SetRectEmpty();
	m_propTree.Create(WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER| TVS_HASBUTTONS | TVS_LINESATROOT | 
		TVS_HASLINES | TVS_DISABLEDRAGDROP | TVS_NOTOOLTIPS ,
		emptyRect, this, ID_MAIN_TREE);
	m_propTree.SetFont(&m_font);

	m_btnMulti.Create(strBtnTxt[0], WS_VISIBLE | WS_CHILD, emptyRect, this, ID_BTN_MULTI);
	m_btnRun.Create(_T("Run"), WS_VISIBLE|WS_CHILD, emptyRect, this, ID_RUN);
	m_btnCancel.Create(_T("Cancel"), WS_VISIBLE|WS_CHILD, emptyRect, this, ID_CANCEL);

	m_btnRun.SetFont(&m_font);
	m_btnCancel.SetFont(&m_font);

	m_nMenu.LoadMenu(IDR_CTX_COMPREPORT);

	return 0;
}


void CCmpReportTreeView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	// CFormView should initialize tree's image list here.
	if (m_imageList.m_hImageList == NULL)
	{
		m_imageList.Create(16,16,ILC_COLOR8|ILC_MASK,0,1);
		CBitmap bmp, bmp2;
		bmp.LoadBitmap(IDB_COOLTREE);
		m_imageList.Add(&bmp,RGB(255,0,255));
		m_imageList.Replace(TIIT_NORMAL, NULL);
	}
	m_propTree.SetImageList(&m_imageList,TVSIL_NORMAL);

	m_btnMulti.SetOperation(0);
	m_btnMulti.SetWindowText(strBtnTxt[0]);
	m_btnMulti.SetType(CCoolBtn::TY_CMPREPORTTREEVIEW);
	CCompareReportDoc* pDoc = (CCompareReportDoc*)GetDocument();
	m_pCmpReport = pDoc->GetCmpReport();
	InitRootItems();
}

void CCmpReportTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if(!IsWindowVisible())
		return;
	UpdateWholeTree();
}

void CCmpReportTreeView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CDocument* pDoc = GetDocument();
	if (::IsWindow(m_propTree.m_hWnd))
		m_propTree.MoveWindow(0, 0, cx, (cy - BUTTON_AREA_HEIGHT));

	int x = 0, y = 0;
	x = cx - 15 - BUTTON_WIDTH;
	y = cy - BUTTON_AREA_HEIGHT + 10;
	if (::IsWindow(m_btnCancel.m_hWnd))
		m_btnCancel.MoveWindow(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
	x = cx - (15 + BUTTON_WIDTH)*2;
	if (::IsWindow(m_btnRun.m_hWnd))
		m_btnRun.MoveWindow(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
	if(::IsWindow(m_btnMulti.m_hWnd))
		m_btnMulti.MoveWindow(10, y, BUTTON_WIDTH+40, BUTTON_HEIGHT);
}

BOOL CCmpReportTreeView::OnEraseBkgnd(CDC* pDC)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.top = rectClient.bottom - BUTTON_AREA_HEIGHT;
	pDC->FillSolidRect(&rectClient, ::GetSysColor(COLOR_BTNFACE));

	return CFormView::OnEraseBkgnd(pDC);
}

void CCmpReportTreeView::InitRootItems()
{
	COOLTREE_NODE_INFO cni;
	InitCooltreeNodeInfo(this, cni);
	m_hBasicInfo = m_propTree.InsertItem(_T("BASIC INFO"), cni, FALSE, FALSE, TVI_ROOT);
	m_hModelRoot = m_propTree.InsertItem(_T("MODELS"),cni, FALSE, FALSE, TVI_ROOT);
	m_hReportRoot = m_propTree.InsertItem(_T("REPORTS"),cni, FALSE, FALSE, TVI_ROOT);
	cni.net = NET_LABLE;
	m_hProjName = m_propTree.InsertItem(_T("Name"), cni, FALSE, FALSE,m_hBasicInfo, TVI_LAST);
	m_hProjDesc = m_propTree.InsertItem(_T("Description"),cni, FALSE, FALSE, m_hBasicInfo, TVI_LAST);
	m_propTree.Expand(m_hBasicInfo, TVE_EXPAND);
}

BOOL CCmpReportTreeView::CheckData()
{
	CCmpReportParameter* pInputParam = m_pCmpReport->GetComparativeProject()->GetInputParam();
	//	Any models
	if (!pInputParam->GetModelsManager()->getCount())
	{
		AfxMessageBox(_T("Please assign one or more models."));
		return FALSE;
	}

	//	Any Reports
	if (!pInputParam->GetReportsManager()->getCount())
	{
		AfxMessageBox(_T("Please assign one or more reports."));
		return FALSE;
	}

	return TRUE;
}
void CCmpReportTreeView::OnRun()
{
	if (CheckData())
	{
		if (m_pCmpReport->SaveProject())
			SetTimer(100, 500, NULL);
		else
			AfxMessageBox(_T("Save data failed!"));
	}
}

static void CALLBACK _ShowCopyInfo(LPCTSTR strPath)
{
	CString fileName;
	CString strFilePath = CString(strPath);
	int nPos = strFilePath.ReverseFind('\\');
	fileName = strFilePath.Mid(nPos + 1);
	CString strMsg;
	strMsg = _T("Copying   ") + fileName;
}
void CCmpReportTreeView::RunCompareReport()
{
	CMainFrame* pFram = (CMainFrame*)AfxGetMainWnd();
	CWnd* pWnd = &(pFram->m_wndCompRepLogBar);
	((CCompRepLogBar*)pWnd)->m_pProj = m_pCmpReport->GetComparativeProject();
	((CCompRepLogBar*)pWnd)->SetParentIndex(1);

	pFram->ShowControlBar((CToolBar*) pWnd, TRUE, FALSE);
	CRect rc1, rc2;
	pFram->m_wndCompRepLogBar.GetWindowRect(&rc1);
	pFram->m_wndCompRepLogBar.GetClientRect(&rc2);
	pFram->ChangeSize(rc1, rc2, 1);

	((CCompRepLogBar*)pWnd)->DeleteLogText();
	m_pCmpReport->Run(this->GetSafeHwnd(), (CCompRepLogBar*)pWnd,_ShowCopyInfo);
	((CCompRepLogBar*)pWnd)->SetProgressPos(0);
	GetDocument()->UpdateAllViews(this);

}

void CCmpReportTreeView::RemoveSubItems(HTREEITEM hItem)
{
	if(hItem == NULL)
		return;
	HTREEITEM hChildItem;
	while((hChildItem = m_propTree.GetChildItem(hItem)) != NULL)
	{
		RemoveSubItems(hChildItem);
		CmpReportTreeNodeDataWithType* pNodeData = (CmpReportTreeNodeDataWithType*)m_propTree.GetItemData(hChildItem);
		if(pNodeData != NULL)
			delete pNodeData;
		m_propTree.DeleteItem(hChildItem);
	}
}

void CCmpReportTreeView::UpdateSubItems(HTREEITEM hItem)
{
	CString strItemText;
	if(hItem == NULL)
		return;
	if(hItem == m_hProjName)
	{
		CString strName = m_pCmpReport->GetComparativeProject()->GetName();
		if(strName.IsEmpty())
		{
			strItemText = "Name";
		}
		else
		{
			strName.MakeUpper();
			strItemText.Format("Name: %s", strName);
		}
		m_propTree.SetItemText(hItem, strItemText);
	}
	else if(hItem == m_hProjDesc)
	{
		CString strDesc = m_pCmpReport->GetComparativeProject()->GetDescription();
		if(strDesc.IsEmpty())
			strItemText = "Description";
		else
			strItemText.Format("Description: %s", strDesc);
		m_propTree.SetItemText(hItem, strItemText);
	}
	else if(hItem == m_hModelRoot)
	{
		RemoveSubItems(m_hModelRoot);
		COOLTREE_NODE_INFO cni;
		InitCooltreeNodeInfo(this, cni);
		CModelsManager* pManager = m_pCmpReport->GetComparativeProject()->GetInputParam()->GetModelsManager();
		
		int modelCount = pManager->getCount();
		for(int i = 0; i < modelCount; i++)
		{
			CModelToCompare* pCmpModel = pManager->getModel(i);
			cni.nt = NT_CHECKBOX;
			CString strModel = "";
			strModel.Format("Model-%d", i);
			HTREEITEM hModel = m_propTree.InsertItem(strModel, cni, pCmpModel->GetChecked(), FALSE, m_hModelRoot);
			cni.nt = NT_NORMAL;
			CmpReportTreeNodeDataWithType* pNodeData = NULL;
			pNodeData = new CmpReportTreeNodeDataWithType();
			pNodeData->m_data = (DWORD)pCmpModel;
			pNodeData->m_type = CMP_REPORT_TN_MODEL;
			m_propTree.SetItemData(hModel, (DWORD)pNodeData);

			cni.net =  NET_SHOW_DIALOGBOX;
			cni.nt = NT_NORMAL;
			CModelToCompare *pModelToCompare = pManager->getModel(i);
			CString strName = pModelToCompare->GetUniqueName();
			strItemText.Format("Name: %s", strName);
			HTREEITEM hItemTemp = m_propTree.InsertItem(strItemText, cni, FALSE, FALSE, hModel);

			strItemText = _T("SimResult");
			HTREEITEM hSimResultItem = m_propTree.InsertItem(strItemText, cni, FALSE, FALSE, hModel);

			cni.nt = NT_CHECKBOX;
			int nSimCount = pModelToCompare->GetSimResultCount();
			for (int j = 0; j < nSimCount; ++j)
			{
				SimResultWithCheckedFlag& simResult = pModelToCompare->GetSimResult(j);
				strItemText = simResult.GetSimResultName();
				HTREEITEM hSimResultN = m_propTree.InsertItem(strItemText, cni, simResult.GetChecked(), FALSE, hSimResultItem);
				pNodeData = new CmpReportTreeNodeDataWithType();
				pNodeData->m_data = (DWORD)(&simResult);
				pNodeData->m_type = CMP_REPORT_TN_SIMRESULT;
				m_propTree.SetItemData(hSimResultN, (DWORD)pNodeData);
			}
			m_propTree.Expand(hSimResultItem, TVE_EXPAND);

			cni.nt = NT_NORMAL;
			CString strPath = pModelToCompare->GetModelLocation();
			int nLenName = strName.GetLength();
			int nLenPath = strPath.GetLength();
			strPath = strPath.Left(nLenPath-nLenName-1);
			strItemText.Format("Path: %s", strPath);
			HTREEITEM hPathItem = m_propTree.InsertItem(strItemText, cni, FALSE, FALSE, hModel);
			m_propTree.Expand(hModel, TVE_EXPAND);
		}
		m_propTree.Expand(m_hModelRoot, TVE_EXPAND);
	}
	else if(hItem == m_hReportRoot)
	{
		RemoveSubItems(m_hReportRoot);
		CSingleReportsManager* pRManager = m_pCmpReport->GetComparativeProject()->GetInputParam()->GetReportsManager();
		int nReportCount = pRManager->getCount();
		
		for(int nReport = 0; nReport < nReportCount; nReport++)
		{
			CReportToCompare& report = pRManager->getReport(nReport);
			COOLTREE_NODE_INFO cni;
			InitCooltreeNodeInfo(this, cni);
			cni.net = NET_SHOW_DIALOGBOX;
			CString strRep = _T("");
			cni.nt = NT_CHECKBOX;
			strRep = report.GetName();
			strRep.MakeUpper();
			HTREEITEM hReport = m_propTree.InsertItem(strRep, cni, report.GetChecked(), FALSE, m_hReportRoot);
			CmpReportTreeNodeDataWithType* pNodeData = new CmpReportTreeNodeDataWithType();
			pNodeData->m_data = (DWORD)(&report);
			pNodeData->m_type = CMP_REPORT_TN_REPORT;
			m_propTree.SetItemData(hReport, (DWORD)pNodeData);

			ReloadReportDetailSubItems(report, hReport);
		}
		m_propTree.Expand(m_hReportRoot, TVE_EXPAND);
	}
//	m_pCmpReport->SetModifyFlag(TRUE);
//	m_pCmpReport->SaveProject();
}

void CCmpReportTreeView::UpdateWholeTree()
{
	UpdateSubItems(m_hProjName);
	UpdateSubItems(m_hProjDesc);
	UpdateSubItems(m_hModelRoot);
	UpdateSubItems(m_hReportRoot);
}

CString CCmpReportTreeView::GetRegularDateTime(LPCTSTR elaptimestr, bool needsec)
{
	CModelsManager* pModelManager = m_pCmpReport->GetComparativeProject()->GetInputParam()->GetModelsManager();

	Terminal *pTerminal = pModelManager->getTerminal();

	if(pTerminal == NULL)
		return "";

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
		retstr.Format("Day%d", _nDtIdx + 1);
	}
	retstr += tstr;

	return retstr;
}

void CCmpReportTreeView::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == 100)
	{
		KillTimer(nIDEvent);
		RunCompareReport();
	}

	CWnd::OnTimer(nIDEvent);
}

void CCmpReportTreeView::AddModel()
{
	CWaitCursor wc;

	CModelsManager* pManager = m_pCmpReport->GetComparativeProject()->GetInputParam()->GetModelsManager();
	CModelSelectionDlg dlg(pManager,this);
	wc.Restore();
	if(dlg.DoModal() == IDOK)
	{
		UpdateSubItems(m_hModelRoot);
		m_pCmpReport->SetModifyFlag(TRUE);
		m_pCmpReport->SaveProject();
	}
}

void CCmpReportTreeView::DeleteSelectedModel()
{			
	HTREEITEM hSelItem = m_propTree.GetSelectedItem();
	HTREEITEM hSubItem = m_propTree.GetChildItem(hSelItem);
	CString strModelName = m_propTree.GetItemText(hSubItem);
	CString strPadding = "Name: ";
	strModelName = strModelName.Right(strModelName.GetLength() - strPadding.GetLength());
	if(m_pCmpReport->GetComparativeProject()->GetInputParam()->DeleteModel(strModelName))
	{
		UpdateSubItems(m_hModelRoot);
		UpdateSubItems(m_hReportRoot);
		m_pCmpReport->SetModifyFlag(TRUE);
		m_pCmpReport->SaveProject();
	}
}

void CCmpReportTreeView::DeleteAllModel()
{
	if(MessageBox("Delete all Models?", NULL, MB_ICONWARNING | MB_YESNO) == IDYES)
	{
		CModelsManager* pModelManager = 
			m_pCmpReport->GetComparativeProject()->GetInputParam()->GetModelsManager();
		MessageBox("TODO: Delete all models.");
		/*pModelManager->RemoveAllModels();*/
		UpdateSubItems(m_hModelRoot);
		UpdateSubItems(m_hReportRoot);
	}
}

void CCmpReportTreeView::AddReport()
{
	CWaitCursor wc;
	CDlgReportProperty dlg(this);
	dlg.m_strProjName = m_pCmpReport->GetComparativeProject()->GetName();
	CModelsManager* pMManager = m_pCmpReport->GetComparativeProject()->GetInputParam()->GetModelsManager();
	CString strError = pMManager->InitTerminal(NULL,dlg.m_strProjName,NULL);
	if (!strError.IsEmpty())
	{
		AfxMessageBox(_T("Cann't load project:") + strError);
		return;
	}

	CSingleReportsManager* pRManager = m_pCmpReport->GetComparativeProject()->GetInputParam()->GetReportsManager();

	dlg.SetManager(pMManager,pRManager);

	dlg.SetProjName(dlg.m_strProjName);
	wc.Restore();
	if(dlg.HasModelInLocation() == CDlgReportProperty::MT_NOMODEL)
	{
		MessageBox("No valid model, please add one at least!");
		return ;
	}

	if(dlg.DoModal() == IDOK)
	{
		const CReportToCompare& report = dlg.GetReport();
		pRManager->AddReport(report);
		UpdateSubItems(m_hReportRoot);
		m_pCmpReport->SetModifyFlag(TRUE);
		m_pCmpReport->SaveProject();
	}
}

void CCmpReportTreeView::EditReport()
{
	CWaitCursor wc;

	BOOL bFound = FALSE;
	HTREEITEM hSelItem = m_propTree.GetSelectedItem();
	CString strReportName = m_propTree.GetItemText(hSelItem);

	CModelsManager* pMManager = m_pCmpReport->GetComparativeProject()->GetInputParam()->GetModelsManager();
	CSingleReportsManager* pRManager = m_pCmpReport->GetComparativeProject()->GetInputParam()->GetReportsManager();
	CReportToCompare* pReport2Cmp =	pRManager->GetReportByName(strReportName);
	if(pReport2Cmp)//check the report exists
	{
		CDlgReportProperty dlg(this);
		dlg.SetManager(pMManager, pRManager);
		dlg.SetProjName(m_pCmpReport->GetComparativeProject()->GetName());
		dlg.SetReport(*pReport2Cmp);
		wc.Restore();

		if(dlg.DoModal() == IDOK)
		{
			const CReportToCompare& report = dlg.GetReport();
			pRManager->UpdateReport(strReportName, report);
			strReportName = report.GetName();
			m_propTree.SetItemText(hSelItem, strReportName.MakeUpper());
			ReloadReportDetailSubItems(report, hSelItem);
			m_pCmpReport->SetModifyFlag(TRUE);
			m_pCmpReport->SaveProject();
		}
	}
}

void CCmpReportTreeView::DeleteReport()
{
	HTREEITEM hSelItem = m_propTree.GetSelectedItem();
	CString strReportName = m_propTree.GetItemText(hSelItem);
	
	CCmpReportParameter* pProjectReportParam = m_pCmpReport->GetComparativeProject()->GetInputParam();
	if(pProjectReportParam)
	{
		pProjectReportParam->DeleteReport(strReportName);
		if(m_pCmpReport->GetFocusReportName().CompareNoCase(strReportName) == 0)
		{
			ChangeFocusReport();
		}
		CCompareReportDoc* pDoc = (CCompareReportDoc*)GetDocument();
		pDoc->UpdateAllViews(this);
	}

	UpdateSubItems(m_hReportRoot);
	m_pCmpReport->SetModifyFlag(TRUE);
	m_pCmpReport->SaveProject();
}

void CCmpReportTreeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint pt = point;
	m_propTree.ScreenToClient(&pt);

	UINT iRet;
	HTREEITEM hCurItem = m_propTree.HitTest(pt, &iRet);
	if(hCurItem == NULL)
		return ;

	if(hCurItem == m_hModelRoot)
	{
		CMenu menuProj;
		menuProj.CreatePopupMenu();
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_ADD_MODEL, _T("Add new model"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_DEL_ALL_MODEL, _T("Delete all models"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
		menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
	}
	if(hCurItem == m_hReportRoot)
	{
		CMenu menuReport;
		menuReport.CreatePopupMenu();
		menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_ADD_REPORT, _T("Add new report"));
		menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
		menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
		menuReport.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
	}
	HTREEITEM hPareItem = m_propTree.GetParentItem(hCurItem);
	if(hPareItem == NULL) return;
	if(hPareItem == m_hModelRoot)
	{
		CMenu menuProj;
		menuProj.CreatePopupMenu();
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_DELETE_MODEL, _T("Delete"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
		menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);		
	}
	if(hPareItem == m_hReportRoot)
	{
		CMenu menuProj;
		menuProj.CreatePopupMenu();
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_EDIT_REPORT, _T("Edit"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_DELETE_REPORT, _T("Delete"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
		menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);		
	}
}

LRESULT CCmpReportTreeView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	CString strItemText;
	if(message == UM_CEW_LABLE_BEGIN)
	{
		HTREEITEM hCurItem=(HTREEITEM)wParam;
		CComparativeProject* pComProj = m_pCmpReport->GetComparativeProject();
		if(m_propTree.GetSelectedItem() == m_hProjName)
		{
			CString strValue = pComProj->GetName();
			m_propTree.GetEditWnd(hCurItem)->SetWindowText(strValue);
			((CEdit*)m_propTree.GetEditWnd(hCurItem))->SetSel(0, -1, true); 
		}
		else if(m_propTree.GetSelectedItem() == m_hProjDesc)
		{
			CString strValue = pComProj->GetDescription();
			m_propTree.GetEditWnd(hCurItem)->SetWindowText(strValue);
			((CEdit*)m_propTree.GetEditWnd(hCurItem))->SetSel(0, -1, true);
		}
	}
	if(message == UM_CEW_LABLE_END)
	{
		HTREEITEM hCurItem = (HTREEITEM)wParam;
		CString strValue=*((CString*)lParam);
		CComparativeProject* pComProj = m_pCmpReport->GetComparativeProject();
		if(hCurItem == m_hProjName)
		{
			CString strOriName = pComProj->GetOriName();
			if(strValue.IsEmpty())
			{
				ReleaseCapture();
				MessageBox("The name is empty, please input the project name.", NULL, MB_OK | MB_ICONWARNING);
				return 0;
			}
			if (strValue.CompareNoCase(strOriName) == 0)
			{
				ReleaseCapture();
				return 0;
			}
			if(m_pCmpReport->ProjExists(strValue))
			{
				ReleaseCapture();
				CString strTemp;
				strTemp.Format(_T("The Comparative Report \"%s\" already exists, please rename."), strValue);
				MessageBox(strTemp, NULL, MB_ICONWARNING | MB_OK);
				return 0;
			}
			strItemText.Format("Name: %s", strValue.MakeUpper());
			m_propTree.SetItemText(hCurItem, strItemText);
			pComProj->SetName(strValue);
		}
		else if(hCurItem == m_hProjDesc)
		{
			if(m_pCmpReport->GetComparativeProject()->GetName().IsEmpty())
			{
				MessageBox("The name is empty, please set the project name!");
				m_propTree.SetItemText(hCurItem, "Description");
				return 0;
			}
			if(strValue.IsEmpty())
				strItemText = "Description";
			else
				strItemText.Format("Description: %s", strValue);
			m_propTree.SetItemText(hCurItem, strItemText);
			pComProj->SetDescription(strValue);
		}
		m_pCmpReport->SetModifyFlag(TRUE);
		m_pCmpReport->SaveProject();
	}
	if(message == UM_CEW_STATUS_CHANGE)
	{
		HTREEITEM hCurItem = (HTREEITEM)wParam;
		CmpReportTreeNodeDataWithType* pNodeData = (CmpReportTreeNodeDataWithType*)m_propTree.GetItemData(hCurItem);
		ASSERT(pNodeData != NULL);
		switch(pNodeData->m_type)
		{
		case CMP_REPORT_TN_REPORT:
			{
				CReportToCompare* pReport = (CReportToCompare*)pNodeData->m_data;
				pReport->SetChecked(!pReport->GetChecked());

				if( pReport->GetChecked() == FALSE &&
					m_pCmpReport->GetFocusReportName().CompareNoCase(pReport->GetName()) == 0)
				{
					ChangeFocusReport();
				}

				CCompareReportDoc* pDoc = (CCompareReportDoc*)GetDocument();
				pDoc->UpdateAllViews(this);
			}
			break;
		case CMP_REPORT_TN_MODEL:
			{
				CModelToCompare* pModel = (CModelToCompare*)pNodeData->m_data;
				pModel->SetChecked(!pModel->GetChecked());
			}
			break;
		case CMP_REPORT_TN_SIMRESULT:
			{
				SimResultWithCheckedFlag* pSimResult = (SimResultWithCheckedFlag*)pNodeData->m_data;
				pSimResult->SetChecked(!pSimResult->GetChecked());
			}
			break;
		default:
			break;
		}
		m_pCmpReport->SetModifyFlag(TRUE);
		m_pCmpReport->SaveProject();
	}
	if(message == WM_LBUTTONDBLCLK)
	{
		HTREEITEM hItem = m_propTree.GetSelectedItem();
		if(hItem == NULL)
			return 0;
		CmpReportTreeNodeDataWithType* pNodeData = (CmpReportTreeNodeDataWithType*)m_propTree.GetItemData(hItem);
		if(pNodeData == NULL)
			return 0;
		switch(pNodeData->m_type)
		{
		case CMP_REPORT_TN_REPORT:
			{
				CReportToCompare* pReport = (CReportToCompare*)pNodeData->m_data;
				if(pReport->GetChecked() == TRUE)
				{
					CString strReport = pReport->GetName();
					m_pCmpReport->SetFocusReportName(strReport);

					CCompareReportDoc* pDoc = (CCompareReportDoc*)GetDocument();
					pDoc->UpdateAllViews(this);
				}
				return 0;
			}
			break;
		default:
			break;
		}
	}
	return CFormView::DefWindowProc(message, wParam, lParam);
}

void CCmpReportTreeView::InitCooltreeNodeInfo(CWnd* pParent,COOLTREE_NODE_INFO& CNI,BOOL bVerify/*=TRUE*/)
{
	CNI.bEnable=TRUE;
	CNI.dwItemData=NULL;
	CNI.fMaxValue=100;
	CNI.fMinValue=0;
	CNI.net=NET_NORMAL;
	CNI.nImage=TIIT_NORMAL;
	CNI.nImageDisable=TIIT_NORMAL_DISABLE;
	CNI.nImageSeled=CNI.nImage;
	CNI.nMaxCharNum=256;
	CNI.nt=NT_NORMAL;
	CNI.pEditWnd=NULL;
	CNI.bVerify=FALSE;
	CNI.clrItem=RGB(0,0,0);
	CNI.bAutoSetItemText=TRUE;
	CFont* pFont = pParent->GetFont();
	if(pFont)
		pFont->GetLogFont(&(CNI.lfontItem));
	else
		memset(&CNI.lfontItem, 0, sizeof(LOGFONT));
	CNI.unMenuID=0;
	CNI.bInvalidData = FALSE;
}

void CCmpReportTreeView::OnChooseMenu(UINT nID)
{
	if(nID == MENU_UNAVAILABLE)
		return;
	int x = 0;
	HTREEITEM hCurItem = m_propTree.GetSelectedItem();
	if(hCurItem == NULL) return;
	if(hCurItem == m_hModelRoot)
	{
		switch(nID)
		{
		case MENU_ADD_MODEL:
			AddModel();
			break;
		case MENU_DEL_ALL_MODEL:
			DeleteAllModel();
			break;
		default:
			break;
		}
	}
	if(hCurItem == m_hReportRoot)
	{
		switch(nID)
		{
		case MENU_ADD_REPORT:
			AddReport();
			break;
		default:
			break;
		}
	}
	HTREEITEM hPareItem = m_propTree.GetParentItem(hCurItem);
	if(hPareItem == m_hModelRoot)
	{
		switch(nID)
		{
		case MENU_DELETE_MODEL:
			DeleteSelectedModel();
			break;
		default:
			break;
		}
	}
	if(hPareItem == m_hReportRoot)
	{
		switch(nID)
		{
		case MENU_EDIT_REPORT:
			EditReport();
			break;
		case MENU_DELETE_REPORT:
			DeleteReport();
			break;
		default:
			break;
		}
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
// save & load para
// save & load report
//////////////////////////////////////////////////////////////////////////
void CCmpReportTreeView::OnReportSavePara() 
{
	m_btnMulti.SetOperation(0);
	m_btnMulti.SetWindowText(strBtnTxt[0]);
	OnClickMultiBtn();
}

void CCmpReportTreeView::OnReportLoadPara() 
{

	m_btnMulti.SetOperation(1);
	m_btnMulti.SetWindowText(strBtnTxt[1]);
	OnClickMultiBtn();
}

void CCmpReportTreeView::OnReportSaveReport() 
{

	m_btnMulti.SetOperation(2);
	m_btnMulti.SetWindowText(strBtnTxt[2]);
	OnClickMultiBtn();
}

void CCmpReportTreeView::OnReportLoadReport() 
{

	m_btnMulti.SetOperation(3);
	m_btnMulti.SetWindowText(strBtnTxt[3]);
	OnClickMultiBtn();
}

void CCmpReportTreeView::OnClickMultiBtn() 
{
	int iOperation = m_btnMulti.GetOperation();
	switch(iOperation)
	{
	case 0:	//SAVEPARATOFILE:
		SavePara();
		break;
	case 1:	//LOADPARAFROMFILE:
		LoadPara();
		break;
	case 2:	//SAVEREPORTTOFILE:
		SaveReport();
		break;
	case 3:	//LOADREPORTFROMFILE:
		LoadReport();
		break;
	default:
		assert(0);
	}
}

void CCmpReportTreeView::SavePara()
{
	CFileDialog savedlg(FALSE,".zip",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.zip)|*.zip||",this, 0, FALSE);
	if(savedlg.DoModal() == IDOK)
	{
		CString strFileName = savedlg.GetPathName();
	//	MessageBox("TODO: Save Parameters.");
		if(CompareReportZipManager::DoExportPara(strFileName,m_pCmpReport->GetComparativeProject()->GetName()))
		{
			AfxMessageBox("Save Report Parameters Successfully",MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			AfxMessageBox("Save Report Parameters Failed",MB_OK|MB_ICONINFORMATION);
		}
// 		CReportParameter* pReportPara = GetReportPara();	
// 		assert(pReportPara);
// 		pReportPara->SaveReportParaToFile(strFileName);
	}
}

void CCmpReportTreeView::LoadPara()
{	
	CFileDialog loaddlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.zip)|*.zip||",this, 0, FALSE);
	if(loaddlg.DoModal() == IDOK)
	{
 		CString strFileName = loaddlg.GetPathName();
		//MessageBox("TODO: Load Parameters.");
		CString strProjectName = m_pCmpReport->GetComparativeProject()->GetName();
		CString strProjectPath = PROJMANAGER->GetAppPath() + _T("\\Comparative Report\\") + strProjectName;
		if(CompareReportZipManager::DoImportPara(strFileName,strProjectName))
		{
			AfxMessageBox("Load Report Parameters Successfully",MB_OK|MB_ICONINFORMATION);
			m_pCmpReport->GetComparativeProject()->GetInputParam()->LoadData(strProjectName,strProjectPath);
			UpdateWholeTree();
			GetDocument()->UpdateAllViews(this);
		}
		else
		{
			AfxMessageBox("Load Report Parameters Failed",MB_OK|MB_ICONINFORMATION);
		}
	}
// 		CReportParameter* pPara = NULL;
// 		try
// 		{
// 			pPara = CReportParameter::LoadReoprtParaFromFile(strFileName, t);
// 		}
// 		catch (FileFormatError* _pError)
// 		{
// 			char szBuf[128];
// 			_pError->getMessage(szBuf);
// 			AfxMessageBox(szBuf, MB_OK|MB_ICONWARNING);
// 			delete _pError;
// 			return;
// 		}
// 		if(pPara)
// 		{
// 			CReportParameter* pReportPara = GetReportParameter();	
// 			assert(pReportPara);
// 
// 			CopyParaData(pPara ,pReportPara);
// 			SetGUIFromPara(GetReportParameter(), GetDocument()->GetTerminal().GetSimReportManager()->GetStartDate());
// 			delete pPara;
// 		}
// 	}

}

void CCmpReportTreeView::SaveReport()
{
// 	if(!m_bCanToSave)
// 	{
// 		AfxMessageBox("Can not save report result!",MB_OK|MB_ICONINFORMATION);
// 		return;
// 	}
// 
 	CString strExten	= "zip";/*getExtensionString();*/
 	CString strFilter	= "Report File(*." + strExten + ")|*." + strExten + "|All Type(*.*)|*.*||";
	CFileDialog savedlg(FALSE,strExten,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter,this, 0 ,FALSE);
	if(savedlg.DoModal() == IDOK)
	{
		CString strFileName = savedlg.GetPathName();
		if(CompareReportZipManager::DoExportProject(strFileName,savedlg.GetFileName(),m_pCmpReport->GetComparativeProject()->GetName()))
		{
			AfxMessageBox("Save Report Successfully",MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			AfxMessageBox("Save Report Failed",MB_OK|MB_ICONINFORMATION);
		}

	//	MessageBox("TODO: Save Report.");
// 		CString strFileName = savedlg.GetPathName();
// 		CopyFile(m_strCurReportFile, strFileName, FALSE);
 	}
}

void CCmpReportTreeView::LoadReport()
{
	CString strExten	= "zip";/*getExtensionString();*/
	CString strFilter	= "Report File(*." + strExten + ")|*." + strExten + "|All Type(*.*)|*.*||";
	CFileDialog loaddlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter,this, 0, FALSE);
// 
	if(loaddlg.DoModal() == IDOK)
	{
 		CString strFileName = loaddlg.GetPathName();
		if(CompareReportZipManager::DoImportProject(strFileName,m_pCmpReport->GetComparativeProject()->GetName()))
		{
			AfxMessageBox("Load Report Successfully",MB_OK|MB_ICONINFORMATION);
			m_pCmpReport->GetComparativeProject()->LoadData();
			UpdateWholeTree();
			GetDocument()->UpdateAllViews(this);
		}
		else
		{
			AfxMessageBox("Load Report Failed",MB_OK|MB_ICONINFORMATION);
		}
//		MessageBox("TODO: LoadReport");
// 		if(!CheckReportFileFormat(strFileName))
// 		{
// 			AfxMessageBox("The report file format is error. Can not load the report!",MB_OK|MB_ICONINFORMATION);
// 			return;
// 		}
// 
// 		GetDocument()->GetARCReportManager().SetLoadReportType(load_by_user);	// 1 = load_by_user
// 		GetDocument()->GetARCReportManager().SetUserLoadReportFile(strFileName);
// 
// 		GetDocument()->UpdateAllViews(NULL);
// 
// 		m_bCanToSave = true;
// 		m_strCurReportFile = strFileName;
	}
}

// If the focus report is deleted, then select the first report of the list as the focus target.
// If there is no report in the list, set focus report null.
void CCmpReportTreeView::ChangeFocusReport()
{
	m_pCmpReport->SetFocusReportName(_T(""));
	CCmpReportParameter* pProjectReportParam = m_pCmpReport->GetComparativeProject()->GetInputParam();
	CSingleReportsManager* pRepManager = pProjectReportParam->GetReportsManager();
	int repCount = pRepManager->getCount();
	for(int i=0; i<repCount; i++)
	{
		CReportToCompare& report = pRepManager->getReport(i);
		if(report.GetChecked() == TRUE)
		{
			CString repName = report.GetName();
			m_pCmpReport->SetFocusReportName(repName);
		}
	}
}

void CCmpReportTreeView::ReloadReportDetailSubItems(const CReportToCompare &report, HTREEITEM hReport)
{
	RemoveSubItems(hReport);
	COOLTREE_NODE_INFO cni;
	InitCooltreeNodeInfo(this, cni);
	cni.nt = NT_NORMAL;
	CReportParamToCompare param = report.GetParameter();
	int iIndex;
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
	default:
		return;
		break;
	}

	CString strItemText1;
	cni.nt = NT_NORMAL;
	CString strTemp = s_szReportCategoryName[iIndex];
	strItemText1.Format("Report Category: %s", strTemp);
	HTREEITEM hRepName = m_propTree.InsertItem(strItemText1, cni, FALSE, FALSE, hReport);
	if(report.GetParameter().GetReportDetail() == REPORT_TYPE_DETAIL)
		strTemp = _T("Detail");
	else
		strTemp = _T("Summary");
	strItemText1.Format("Report Type: %s", strTemp);
	HTREEITEM hIsDetail = m_propTree.InsertItem(strItemText1, cni, FALSE, FALSE, hReport, hRepName);
	strTemp = GetRegularDateTime(param.GetStartTime().printTime());
	strItemText1.Format("Start Time: %s", strTemp);
	HTREEITEM hRepStartTime = m_propTree.InsertItem(strItemText1, cni, FALSE, FALSE, hReport, hIsDetail);
	strTemp = GetRegularDateTime(param.GetEndTime().printTime());
	strItemText1.Format("End Time: %s", strTemp);
	HTREEITEM hRepEndTime = m_propTree.InsertItem(strItemText1, cni, FALSE, FALSE, hReport, hRepStartTime);

	if(report.GetCategory() == ENUM_DISTANCE_REP)
	{
		LONG lInterval;
		param.GetInterval(lInterval);
		strItemText1.Format("Interval: %d", lInterval);
	}
	else
	{
		strTemp = param.GetInterval().printTime();
		strItemText1.Format("Interval: %s", strTemp);
	}
	HTREEITEM hInterval = m_propTree.InsertItem(strItemText1, cni, FALSE, FALSE, hReport, hRepEndTime);

	//write Model Parameter
	std::vector<CModelParameter> vModelParam;
	param.GetModelParameter(vModelParam);
	int nModelParamCount = param.GetModelParameterCount();

	CModelsManager* pModelManager = m_pCmpReport->GetComparativeProject()->GetInputParam()->GetModelsManager();

	for (int i=0; i<nModelParamCount; i++)
	{
		CModelParameter& modelParam = vModelParam[i];
		CString strModelName = pModelManager->getModel(i)->GetModelName();
		HTREEITEM hModelItem = m_propTree.InsertItem(strModelName, cni, FALSE, FALSE, hReport, hInterval);
		if(iIndex == 3)		//	case ENUM_PAXDENS_REP:	iIndex = 3;
		{
			CString strTemp = modelParam.GetArea();
			if(strTemp.IsEmpty())
				strItemText1 = "Areas";
			else
				strItemText1.Format("Areas: %s", strTemp);
			HTREEITEM hAreas = m_propTree.InsertItem(strItemText1, cni, FALSE, FALSE, hModelItem);
		}

		if(report.GetCategory() == ENUM_QUEUETIME_REP ||
			report.GetCategory() == ENUM_DURATION_REP ||
			report.GetCategory() == ENUM_DISTANCE_REP ||
			report.GetCategory() == ENUM_ACOPERATION_REP||
			report.GetCategory() == ENUM_PAXDENS_REP)
		{
			std::vector<CMobileElemConstraint> vPaxType;
			if (modelParam.GetPaxType(vPaxType))
			{
				strTemp = _T("Passanger Type");
				HTREEITEM hPaxItem = m_propTree.InsertItem("Passenger Type", cni, FALSE, FALSE, hModelItem);
				CString strPax;
				for (int i = 0; i < static_cast<int>(vPaxType.size()); i++)
				{
					vPaxType[i].screenPrint(strPax);
					HTREEITEM hPaxItem2 = m_propTree.InsertItem(strPax, cni, FALSE, FALSE, hPaxItem);
				}
				m_propTree.Expand(hPaxItem, TVE_EXPAND);
			}	
		}
		if (report.GetCategory() == ENUM_DURATION_REP ||
			report.GetCategory() == ENUM_DISTANCE_REP)
		{
			// from or to processor
			CReportParameter::FROM_TO_PROCS _fromtoProcs;
			modelParam.GetFromToProcs(_fromtoProcs);

			HTREEITEM hFromToItem = m_propTree.InsertItem("From To Processors", cni, FALSE, FALSE, hModelItem);
			HTREEITEM hFromItem = m_propTree.InsertItem("From", cni, FALSE, FALSE, hFromToItem);
			HTREEITEM hToItem = m_propTree.InsertItem("To", cni, FALSE, FALSE, hFromToItem);

			for (int nFrom = 0; nFrom < (int)_fromtoProcs.m_vFromProcs.size(); ++ nFrom)
			{
				CString strProc = _fromtoProcs.m_vFromProcs.at(nFrom).GetIDString();
				m_propTree.InsertItem(strProc, cni, FALSE, FALSE, hFromItem);
			}

			for (int nTo = 0; nTo < (int)_fromtoProcs.m_vToProcs.size(); ++ nTo)
			{
				CString strProc = _fromtoProcs.m_vToProcs.at(nTo).GetIDString();
				m_propTree.InsertItem(strProc, cni, FALSE, FALSE, hToItem);
			}
		}
		else if(report.GetCategory() != ENUM_ACOPERATION_REP)
		{
			std::vector<ProcessorID> vProcGroup;
			if (modelParam.GetProcessorID(vProcGroup))
			{
				HTREEITEM hProcTypeItem = m_propTree.InsertItem("Processor Type", cni, FALSE, FALSE, hModelItem);
				char szProc[128];
				for (int i = 0; i < static_cast<int>(vProcGroup.size()); i++)
				{
					memset(szProc, 0, sizeof(szProc) / sizeof(char));
					vProcGroup[i].printID(szProc);
					HTREEITEM hProcID = m_propTree.InsertItem(szProc, cni, FALSE, FALSE, hProcTypeItem);
				}
				m_propTree.Expand(hProcTypeItem, TVE_EXPAND);
			}
		}
		m_propTree.Expand(hModelItem, TVE_EXPAND);
	}
}

