// AirsideReportListView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AirsideReportListView.h"
#include ".\airsidereportlistview.h"
#include "TermPlanDoc.h"
#include "../AirsideReport/ReportManager.h"
#include "../MFCExControl/PopupDetailWnd.h"
#include "./AirsideReportGraphView.h"
#include "./AirsideRepControlView.h"
#include "../AirsideReport/FlightActivityReport.h"
#include "../AirsideReport/AirsideTakeoffProcessReport.h"
#include "ViewMsg.h"
#include "../InputAirside/RunwayExit.h"
#include "../AirsideReport/AirsideFlightMutiRunDelayReport.h"
#include "../AirsideReport/AirsideFlightDelayParam.h"
#include "../AirsideReport/AirsideFlightStandOperationReport.h"
#include "../AirsideReport/FlightStandOperationParameter.h"
// CAirsideReportListView

IMPLEMENT_DYNCREATE(CAirsideReportListView, CFormView)

CAirsideReportListView::CAirsideReportListView()
	: CFormView(CAirsideReportListView::IDD)
	, m_sExtension(_T("csv"))
{
}

CAirsideReportListView::~CAirsideReportListView()
{
}

void CAirsideReportListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LISTCTRL, m_lstCtrl);
}

BEGIN_MESSAGE_MAP(CAirsideReportListView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_XLISTCTRL_CHECKBOX_CLICKED,OnXListCtrlCheckBoxClicked)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_AIRSIDEREPORTLIST_MAX, OnAirsidereportlistMax)
	ON_COMMAND(ID_AIRSIDEREPORTLIST_NORMAL, OnAirsidereportlistNormal)
	ON_COMMAND(ID_AIRSIDEREPORTLIST_PRINT, OnAirsidereportlistPrint)
	ON_COMMAND(ID_AIRSIDEREPORTLIST_EXPORT, OnAirsidereportlistExport)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTCTRL, OnColumnclickListCtrl)
END_MESSAGE_MAP()


// CAirsideReportListView diagnostics

#ifdef _DEBUG
void CAirsideReportListView::AssertValid() const
{
	CFormView::AssertValid();
}

void CAirsideReportListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CAirsideReportListView message handlers

void CAirsideReportListView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (::IsWindow(m_lstCtrl.m_hWnd))
	{
		CRect rc;
		GetClientRect(&rc);
		//CSize size;
		//GetSc
		//GetScrollBarSizes(size);
		//rc.right  = size.cx;
		//rc.bottom = size.cy;
		m_lstCtrl.MoveWindow(&rc);

		//m_lstCtrl.MoveWindow(0, 0, cx, cy);
	}
	SetScaleToFitSize(CSize(cx,cy));
}

void CAirsideReportListView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
}

int CAirsideReportListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rtEmpty;
	rtEmpty.SetRectEmpty();
	if(m_lstCtrl.Create( LBS_NOTIFY|LBS_HASSTRINGS|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,
		rtEmpty, this,IDC_LISTCTRL) )
	{
		DWORD dwStyle =dwStyle = m_lstCtrl.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
		m_lstCtrl.SetExtendedStyle( dwStyle );

		m_wndSortableHeaderCtrl.SubclassWindow(m_lstCtrl.GetHeaderCtrl()->m_hWnd );
	}

	return 0;
}

void CAirsideReportListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CTermPlanDoc* pTermPlanDoc = (CTermPlanDoc* )GetDocument();
	ASSERT(pTermPlanDoc);
	// TODO: Add your specialized code here and/or call the base class
	if(lHint == VM_GENERATE_REPORT)
	{
		CString error ;
		if(!pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GenerateCvsFileFormList(*(ArctermFile*)pHint,m_lstCtrl,error))
		{
			MessageBox(error,"Error",MB_OK) ;
			return ;
		}
		else
		{
			MessageBox(_T("Export Report Successful"),"Success",MB_OK) ;
			return ;
		}

	}

	CAirsideReportManager* pReportManager = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager();
	ASSERT(pReportManager);

	CParameters* pParameter = pReportManager->GetParameters();
	ASSERT(pParameter);
	bool bMultipleRun = false;
	if (pReportManager->AirsideMultipleRunReport())
	{
		bMultipleRun = true;
	}
	//report conctrl select change combox show different header and content
	if (lHint == AIRSIDEREPORT_DISLISTVIEW)
	{
		//check whether do multiple run
		if (bMultipleRun)
		{
			std::vector<int> vReportRun;
			if (pParameter->GetReportRuns(vReportRun) == true && pParameter->GetEnableMultiRun())
			{
				if (vReportRun.size() > 1)
				{
					if (vReportRun.size() > 1)
					{
						pReportManager->InitMultiReportList(m_lstCtrl, (int)pHint, &m_wndSortableHeaderCtrl);
						pReportManager->SetMultiReportListContent(m_lstCtrl, (int)pHint);
						return;
					}
				}
			}
		}
		

		if(pReportManager->GetReportType() == Airside_FlightDelay)
		{
			CAirsideFlightDelayReport* pDelayReport = (CAirsideFlightDelayReport*)pReportManager->GetReport();
			pDelayReport->InitResultListHead(m_lstCtrl,pParameter,&m_wndSortableHeaderCtrl);
			pDelayReport->FillResultListContent(m_lstCtrl,pParameter);
		}
		else if (pReportManager->GetReportType() == Airside_TakeoffProcess)
		{
			CAirsideTakeoffProcessReport* pTakeoffReport = (CAirsideTakeoffProcessReport*)pReportManager->GetReport();
			pTakeoffReport->FillListContent(m_lstCtrl,pParameter);
		}
		return;
	}

	if (lHint != AIRSIDEREPORT_SHOWREPORT)
	{
		return;
	}
	if (bMultipleRun)
	{
		std::vector<int> vReportRun;
		if (pParameter->GetReportRuns(vReportRun) == true && pParameter->GetEnableMultiRun())
		{
			if (vReportRun.size() > 1)
			{
				if(pReportManager->GetReportType() == Airside_FlightDelay)
				{
					CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)pParameter;
					if (pDelayPara->getReportType() == ASReportType_Detail)
					{
						pDelayPara->setSubType(CAirsideFlightDelayReport::SRT_DETAIL_SCHEDULEDELAY);
					}
					else 
					{
						pDelayPara->setSubType(CAirsideFlightDelayReport::SRT_SUMMARY_SCHEDULEDELAY);
					}
					pReportManager->InitMultiReportList(m_lstCtrl,1,&m_wndSortableHeaderCtrl);
					pReportManager->SetMultiReportListContent(m_lstCtrl,1);
					return;
				}
				else if (pReportManager->GetReportType() == Airside_StandOperations)
				{
					CFlightStandOperationParameter* pStandPara = (CFlightStandOperationParameter*)pParameter;
					if (pStandPara->getReportType() == ASReportType_Detail)
					{
						pStandPara->setSubType(CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy);
					}
					else 
					{
						pStandPara->setSubType(CAirsideFlightStandOperationReport::ChartType_Summary_SchedUtilization);
					}
					pReportManager->InitMultiReportList(m_lstCtrl,0,&m_wndSortableHeaderCtrl);
					pReportManager->SetMultiReportListContent(m_lstCtrl,0);
					return;
				}
			}
		}
		
	}

	pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->InitReportList(m_lstCtrl, &m_wndSortableHeaderCtrl);
	pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->SetReportListContent(m_lstCtrl);

	//
	//pTermPlanDoc->m_pAirsideReportManager->DisplayReport();
}

LRESULT CAirsideReportListView::OnXListCtrlCheckBoxClicked(WPARAM wParam,LPARAM lParam)
{
	CTermPlanDoc* pTermPlanDoc = (CTermPlanDoc* )GetDocument();
	ASSERT(pTermPlanDoc);

	int nItem=wParam;
	int nSubItem=lParam;
	CRect rectSubItem;

	m_lstCtrl.GetSubItemRect(nItem,nSubItem,LVIR_BOUNDS,rectSubItem);
	m_lstCtrl.ClientToScreen(rectSubItem);

	CPopupDetailWnd* pWnd=new CPopupDetailWnd(this,nItem,nSubItem);

	CAirsideBaseReport* pBaseReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
	if (pBaseReport->GetReportType() == Airside_FlightDelay)
	{
		CAirsideFlightDelayReport* pReport = reinterpret_cast<CAirsideFlightDelayReport*>(pBaseReport);

		vector<CAirsideFlightDelayReport::FltDelayItem>  fltDelayItemTotalDelayResult = pReport->GetTotalResult();
		int nDelayItem = -1;
		nDelayItem = m_lstCtrl.GetItemData(nItem);
		if (nDelayItem >= (int)fltDelayItemTotalDelayResult.size())
			return 0;

		CAirsideFlightDelayReport::FltDelayItem fltDelayItem = fltDelayItemTotalDelayResult.at(nDelayItem); //GetFltTypeDelayResultItem(fltDelayItemTotalDelayResult, nItem);

		//set title
		CString strTitle;
		strTitle.Format(_T("Flight: %s"), m_lstCtrl.GetItemText(nItem, 0));
		pWnd->m_staticPaxID.SetWindowText(strTitle);

		CListCtrl& listCtrl = pWnd->m_listTrack;
		listCtrl.DeleteAllItems();

		while(listCtrl.GetHeaderCtrl()->GetItemCount() >0)
			listCtrl.DeleteColumn(0);

		listCtrl.InsertColumn(0,"Node",LVCFMT_LEFT,140,-1);
		listCtrl.InsertColumn(1,"Expected time",LVCFMT_LEFT,100,-1);
		listCtrl.InsertColumn(2,_T("Actual time"),LVCFMT_LEFT,100,-1);
		listCtrl.InsertColumn(3,_T("Delay"),LVCFMT_LEFT,60,-1);
		listCtrl.InsertColumn(4,_T("Reason"),LVCFMT_LEFT,80,-1);
		listCtrl.InsertColumn(5,_T("Detail"),LVCFMT_LEFT,80,-1);

		int nShowRows = 0;
		for(int i=0; i<(int)fltDelayItem.vNodeDelay.size(); i++)
		{
			CAirsideFlightDelayReport::FltNodeDelayItem nodeDelayItem = fltDelayItem.vNodeDelay[i];
			
			if(nodeDelayItem.delayTime == 0L)
				continue;

			if(nodeDelayItem.nSegment == CAirsideFlightDelayReport::FltDelaySegment_Air && nSubItem != 15)
			{
				continue;

			}
			else if(nodeDelayItem.nSegment == CAirsideFlightDelayReport::FltDelaySegment_Taxi && nSubItem != 16)
			{
				continue;
			}
			else if(nodeDelayItem.nSegment ==CAirsideFlightDelayReport:: FltDelaySegment_Stand && nSubItem != 17)
			{
				continue;
			}
			else if(nodeDelayItem.nSegment == CAirsideFlightDelayReport::FltDelaySegment_Service && nSubItem != 18)
			{
				continue;
			}
			else if(nodeDelayItem.nSegment == CAirsideFlightDelayReport::FltDelaySegment_TakeOff && nSubItem != 19)
			{
				continue;
			}

		
			//node
			listCtrl.InsertItem(nShowRows, nodeDelayItem.m_strResName);

			CString strText;
			ElapsedTime estTime(long(nodeDelayItem.eArriveTime/100.0+0.5));
			//expect time
			strText.Format(_T("Day%d %02d:%02d:%02d"), estTime.GetDay(), estTime.GetHour(), estTime.GetMinute(), estTime.GetSecond());
			listCtrl.SetItemText(nShowRows,1,strText);

			//actual time
			ElapsedTime estActualTime(long(nodeDelayItem.eActArriveTime/100.0+0.5));
			strText.Format(_T("Day%d %02d:%02d:%02d"), estActualTime.GetDay(), estActualTime.GetHour(), estActualTime.GetMinute(), estActualTime.GetSecond());
			listCtrl.SetItemText(nShowRows,2,strText);

			//delay
			ElapsedTime estDelayTime(long(nodeDelayItem.delayTime/100.0+0.5));
			listCtrl.SetItemText(nShowRows,3,estDelayTime.printTime());

			//reason
			switch(nodeDelayItem.nReason)
			{
			case FltDelayReason_Slowed:
				strText.Format(_T("Slowed"));
				break;

			case FltDelayReason_Vectored:
				strText.Format(_T("Vectored"));
				break;

			case FltDelayReason_AirHold:
				strText.Format(_T("Hold"));
				break;

			case FltDelayReason_AltitudeChanged:
				strText.Format(_T("Altitude changed"));
				break;

			case FltDelayReason_SideStep:
				strText.Format(_T("Side step"));
				break;

			case FltDelayReason_Stop:
				strText.Format(_T("Stop"));
				break;

			case FltDelayReason_Service:
				strText.Format(_T("Service"));
				break;

			default:
				strText.Format(_T("Unknown"));
				break;
			}
			listCtrl.SetItemText(nShowRows,4,strText);

			//detail

			listCtrl.SetItemText(nShowRows,5,nodeDelayItem.m_strDetailReason);

			nShowRows += 1;
		}
	}
	else if (pBaseReport->GetReportType() == Airside_FlightActivity)
	{
		CParameters* pParameters = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetParameters();
		ASSERT(pParameters);
 		CFlightActivityReport* pReport = reinterpret_cast<CFlightActivityReport*>(pBaseReport);
		if (pParameters->getReportType() == ASReportType_Detail)
		{
			vector<CFlightDetailActivityResult::FltActItem> fltDelayResult = ((CFlightDetailActivityResult*)pReport->m_pResult)->GetResult();

			//set title
			CString strTitle;

			if (!m_lstCtrl.GetItemText(nItem, 2).IsEmpty())
			{
				strTitle.Format(_T("Flight: %s"), m_lstCtrl.GetItemText(nItem, 2));
			}
			else
			{
				strTitle.Format(_T("Flight: %s"), m_lstCtrl.GetItemText(nItem, 1));
			}
			pWnd->m_staticPaxID.SetWindowText(strTitle);

			CListCtrl& listCtrl = pWnd->m_listTrack;
			listCtrl.DeleteAllItems();

			while(listCtrl.GetHeaderCtrl()->GetItemCount() >0)
				listCtrl.DeleteColumn(0);

			listCtrl.InsertColumn(0,"Node",LVCFMT_LEFT,140,-1);
			listCtrl.InsertColumn(1,"Entry Time",LVCFMT_LEFT,100,-1);

			int nIndex = (int)m_lstCtrl.GetItemData(nItem);
			std::vector<CFlightDetailActivityResult::FltActTraceItem> fltActTraceItem = fltDelayResult[nIndex].vTrace;
			
			int nRow = 0;
			for (int i=0; i<(int)fltActTraceItem.size(); i++)
			{
				CString strObjName = _T("NoResource");
				strObjName = GetObjectName(fltActTraceItem[i].nObjType,fltActTraceItem[i].nObjID);
				if(strObjName.CompareNoCase(_T("NoResource")) == 0)
					continue;

				listCtrl.InsertItem(nRow, strObjName);
				ElapsedTime estEntryTime(fltActTraceItem[i].entryTime);
				listCtrl.SetItemText(nRow, 1, estEntryTime.printTime(1));

				nRow += 1;
			}
		}
	}

	pWnd->SetWindowPos(NULL,rectSubItem.left,rectSubItem.bottom,0,0,SWP_NOSIZE);
	pWnd->ShowWindow(SW_SHOW);

	return 0;
}

CAirsideFlightDelayReport::FltDelayItem CAirsideReportListView::GetFltTypeDelayResultItem(vector<CAirsideFlightDelayReport::FltDelayItem>& fltTypeDelayResult, int nIndex)
{
	map<int ,int> indexMap;

	int nRealIndex = 0;
	int nResultIndex = 0;

	for (int i=0; i<(int)fltTypeDelayResult.size(); i++)
	{
		if (strlen(fltTypeDelayResult[i].fltDesc.arrID) > 0)
		{
			indexMap.insert(make_pair(nRealIndex, nResultIndex));
			nRealIndex++;
		}

		if (strlen(fltTypeDelayResult[i].fltDesc.depID) > 0)
		{
			indexMap.insert(make_pair(nRealIndex, nResultIndex));
			nRealIndex++;
		}

		nResultIndex++;
	}

	int nRealResultIndex = indexMap[nIndex];

	return fltTypeDelayResult.at(nRealResultIndex);
}

CString CAirsideReportListView::GetObjectName(int objectType,int objID)
{
	CString strObjName = _T("NoResource");
	if (objectType == 0)
	{
		ALTObject *pObject = ALTObject::ReadObjectByID(objID);
		if (pObject)
		{
			strObjName = pObject->GetObjNameString();
		}
		delete pObject;
	}
	else if(objectType == 1)
	{
		IntersectionNode intersecNode;
		intersecNode.ReadData(objID);
		strObjName = intersecNode.GetName();
	}
	else if(objectType == 3)
	{
		strObjName = _T("Entry System");
	}
	else if(objectType == 4)
	{
		strObjName = _T("Exit System");
	}
	else if (objectType == 5)
	{
		RunwayExit runwayExit;
		runwayExit.ReadData(objID);
		strObjName = runwayExit.GetIntesectionNode().GetName();
	}

	return strObjName;
}
void CAirsideReportListView::ResetAllContent()
{
	m_lstCtrl.DeleteAllItems();
	while(m_lstCtrl.GetHeaderCtrl()->GetItemCount() >0)
		m_lstCtrl.DeleteColumn(0);
}

void CAirsideReportListView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu = menuPopup.GetSubMenu(79);

	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CAirsideReportListView::OnAirsidereportlistMax()
{
	CRect rectMax;
	GetParentFrame()->GetClientRect(&rectMax);
	rectMax.top+=2;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)(GetDocument());
	CView * pTempView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		if(pTempView->IsKindOf(RUNTIME_CLASS(CAirsideReportGraphView)))
		{
			pTempView->ShowWindow(SW_HIDE);
		}
		if(pTempView->IsKindOf(RUNTIME_CLASS(CAirsideRepControlView)))
		{
			pTempView->ShowWindow(SW_HIDE);
		}
	}
	GetParent()->MoveWindow(&rectMax);
	MoveWindow(&rectMax);
	GetParentFrame()->Invalidate();
}

void CAirsideReportListView::OnAirsidereportlistNormal()
{

	CTermPlanDoc* pDoc = (CTermPlanDoc*)(GetDocument());
	CView * pTempView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		if(pTempView->IsKindOf(RUNTIME_CLASS(CAirsideReportGraphView)))
		{
			pTempView->ShowWindow(SW_SHOW);
		}
		if(pTempView->IsKindOf(RUNTIME_CLASS(CAirsideRepControlView)))
		{
			pTempView->ShowWindow(SW_SHOW);
		}
	}
	GetParentFrame()->ShowWindow(SW_NORMAL);
	GetParentFrame()->ShowWindow(SW_MAXIMIZE);
}


void CAirsideReportListView::OnAirsidereportlistPrint()
{
	m_sHeader="List Header String";
	m_sFoot="List Foot String";
	CDC dc;
	CPrintDialog printDlg(FALSE, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE);
	if (GetListCtrl().GetSelectedCount() == 0)						// if there are no selected items in the list control
		printDlg.m_pd.Flags = printDlg.m_pd.Flags | PD_NOSELECTION; //then disable the selection radio button
	else															//else ...
		printDlg.m_pd.Flags = printDlg.m_pd.Flags | PD_SELECTION;

	if(printDlg.DoModal() == IDCANCEL )
		return;
	dc.Attach( printDlg.GetPrinterDC() );
	dc.m_bPrinting = TRUE;
	CString sTitle;
	sTitle = "Flight Schedule";
	DOCINFO di;
	::ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof( DOCINFO );
	di.lpszDocName = sTitle;

	BOOL bPrintingOK = dc.StartDoc( &di );

	CPrintInfo info;
	info.m_rectDraw.SetRect( 0,0,dc.GetDeviceCaps( HORZRES ),dc.GetDeviceCaps( VERTRES ));

	OnBeginPrinting( &dc, &info );
	//	for( UINT page = info.GetMinPage(); page < info.GetMaxPage() && bPrintingOK; page++ )
	//	{
	//		info.m_nCurPage = page;
	if(printDlg.PrintSelection() == FALSE)	//print all.
		OnPrint( &dc, &info );
	else 
		PrintSelectedItems(&dc, &info);				//print only the selected items.
	bPrintingOK = TRUE;
	//	}
	OnEndPrinting( &dc, &info );

	if( bPrintingOK )
		dc.EndDoc();
	else
		dc.AbortDoc();
	dc.Detach();
}
void CAirsideReportListView::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
	CString line; //This is the print line
	TEXTMETRIC metrics; //Font measurements
	int y = 0; //Current y position on report
	CFont TitleFont; //Font for Title
	CFont HeadingFont; //Font for headings
	CFont DetailFont; //Font for detail lines
	CFont FooterFont; //Font for footer
	//Tab stops at 1 inch, 2.5 inches, and 6.5 inches
	//	int TabStops[] = {100, 200,300,400,500};
	//Tab stops at 3.5 inches and 6.5 inches
	int FooterTabStops[] = {350, 650};
	if (!pInfo || pInfo->m_nCurPage == 1) {
		//Set the recordset at the beginning
	}
	short cxInch=pDC->GetDeviceCaps(LOGPIXELSX); 
	short cyInch=pDC->GetDeviceCaps(LOGPIXELSY); 

#define MYFONTSIZE 14 
#define HFONTNAME "MS Sans Serif"
	//
	int nFontHeight=MulDiv(MYFONTSIZE, -cyInch, 72); 
	if(nFontHeight % 2) nFontHeight++; 

	if(!TitleFont.CreateFont(static_cast<int>(nFontHeight*0.8), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!HeadingFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!DetailFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!FooterFont.CreateFont(nFontHeight/2, 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ;

	// Get the system's default printer's setting
	int	nPageVMargin,nPageHMargin,nPageHeight,nPageWidth;
	nPageVMargin = pDC->GetDeviceCaps(LOGPIXELSY) / 2;
	nPageHMargin = pDC->GetDeviceCaps(LOGPIXELSX) / 2;
	nPageHeight  = pDC->GetDeviceCaps(VERTRES);
	nPageWidth   = pDC->GetDeviceCaps(HORZRES);

	//Capture default settings when setting the footer font	
	CFont* OldFont = (CFont*)pDC->SelectObject(&FooterFont);	
	pDC->GetTextMetrics(&metrics);
	int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;
	pDC->SelectObject(&TitleFont);
	//Retrieve the heading font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the heading line height
	int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	//Set Y to the line height.
	y += static_cast<int>(nLineHeight*1.5+nPageVMargin);
	pDC->TextOut(nPageHMargin, nPageVMargin, m_sHeader);
	//Draw a line under the heading
	pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
	pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
	//Set the Heading font
	pDC->SelectObject(&HeadingFont);
	y += nLineHeight;

	//Format the heading
	// get listctrl's header info
	int nCol = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	HDITEM hi;
	hi.mask = HDI_TEXT ;
	hi.cchTextMax = 19;
	char chBuffer[20];
	hi.pszText = chBuffer;
	int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
	for(int i = 0; i < nCol; i++ )
	{
		::ZeroMemory( chBuffer, 20);
		GetListCtrl().GetHeaderCtrl()->GetItem( i,&hi);
		pDC->TextOut(nPageHMargin+i*nSpace, y,hi.pszText);
	}

	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	y += 2*nLineHeight; //Adjust y position
	//Set the detail font
	pDC->SelectObject(&DetailFont);
	//Retrieve detail font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;

	//Scroll through the list
	int nIndex=1;
	int nRowCount = GetListCtrl().GetItemCount();
	while ( nIndex <= nRowCount ) 
	{
		if (pInfo && y > nPageHeight-nPageVMargin-nFooterHeight-nLineHeight-6) 
		{   // start a new page
			pInfo->SetMaxPage(pInfo->m_nCurPage + 1);
			break;
		}
		//Format the detail line
		for( int nSubitem = 0; nSubitem < nCol; nSubitem++ )
		{

			char chLine[20];
			GetListCtrl().GetItemText(nIndex-1,nSubitem,chLine,19);
			pDC->TextOut(nPageHMargin+nSubitem*nSpace, y,chLine);
		}

		y += nLineHeight; //Adjust y position
		nIndex++; 
	}
	if (pInfo) {
		//Draw a line above the footer
		pDC->MoveTo(nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		pDC->LineTo(nPageWidth-nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		//Set the footer font
		pDC->SelectObject(&FooterFont);
		//Format the footer
		line.Format(" \tPage %d",pInfo->m_nCurPage);
		line = m_sFoot + line;
		//Output the footer at the bottom using tabs
		pDC->TabbedTextOut(nPageHMargin, nPageHeight-nPageVMargin-nFooterHeight, line, 2, FooterTabStops, 0);
	}
	//Restore default settings
	pDC->SelectObject(OldFont);
}
void CAirsideReportListView::PrintSelectedItems(CDC *pDC, CPrintInfo *pInfo)
{
	CString line; //This is the print line
	TEXTMETRIC metrics; //Font measurements
	int y = 0; //Current y position on report
	CFont TitleFont; //Font for Title
	CFont HeadingFont; //Font for headings
	CFont DetailFont; //Font for detail lines
	CFont FooterFont; //Font for footer
	//Tab stops at 1 inch, 2.5 inches, and 6.5 inches
	//	int TabStops[] = {100, 200,300,400,500};
	//Tab stops at 3.5 inches and 6.5 inches
	int FooterTabStops[] = {350, 650};
	if (!pInfo || pInfo->m_nCurPage == 1) {
		//Set the recordset at the beginning
	}
	short cxInch=pDC->GetDeviceCaps(LOGPIXELSX); 
	short cyInch=pDC->GetDeviceCaps(LOGPIXELSY); 

#define MYFONTSIZE 14 
#define HFONTNAME "MS Sans Serif"
	//
	int nFontHeight=MulDiv(MYFONTSIZE, -cyInch, 72); 
	if(nFontHeight % 2) nFontHeight++; 

	if(!TitleFont.CreateFont(static_cast<int>(nFontHeight*0.8), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!HeadingFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!DetailFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!FooterFont.CreateFont(nFontHeight/2, 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ;

	// Get the system's default printer's setting
	int	nPageVMargin,nPageHMargin,nPageHeight,nPageWidth;
	nPageVMargin = pDC->GetDeviceCaps(LOGPIXELSY) / 2;
	nPageHMargin = pDC->GetDeviceCaps(LOGPIXELSX) / 2;
	nPageHeight  = pDC->GetDeviceCaps(VERTRES);
	nPageWidth   = pDC->GetDeviceCaps(HORZRES);

	//Capture default settings when setting the footer font	
	CFont* OldFont = (CFont*)pDC->SelectObject(&FooterFont);	
	pDC->GetTextMetrics(&metrics);
	int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;
	pDC->SelectObject(&TitleFont);
	//Retrieve the heading font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the heading line height
	int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	//Set Y to the line height.
	y += static_cast<int>(nLineHeight*1.5+nPageVMargin);
	pDC->TextOut(nPageHMargin, nPageVMargin, m_sHeader);
	//Draw a line under the heading
	pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
	pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
	//Set the Heading font
	pDC->SelectObject(&HeadingFont);
	y += nLineHeight;

	//Format the heading
	// get listctrl's header info
	int nCol = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	HDITEM hi;
	hi.mask = HDI_TEXT ;
	hi.cchTextMax = 19;
	char chBuffer[20];
	hi.pszText = chBuffer;
	int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
	for(int i = 0; i < nCol; i++ )
	{
		::ZeroMemory( chBuffer, 20);
		GetListCtrl().GetHeaderCtrl()->GetItem( i,&hi);
		pDC->TextOut(nPageHMargin+i*nSpace, y,hi.pszText);
	}

	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	y += 2*nLineHeight; //Adjust y position
	//Set the detail font
	pDC->SelectObject(&DetailFont);
	//Retrieve detail font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;

	//Scroll through the list
	int nIndex=1;
	POSITION ps = GetListCtrl().GetFirstSelectedItemPosition();
	if(ps == NULL) 
		return;
	int curSelectedItem = GetListCtrl().GetNextSelectedItem(ps);
	int nRowCount = GetListCtrl().GetSelectedCount();
	while ( nIndex <= nRowCount ) 
	{
		if (pInfo && y > nPageHeight-nPageVMargin-nFooterHeight-nLineHeight-6) 
		{   // start a new page
			pInfo->SetMaxPage(pInfo->m_nCurPage + 1);
			break;
		}
		//Format the detail line
		for( int nSubitem = 0; nSubitem < nCol; nSubitem++ )
		{

			char chLine[20];
			GetListCtrl().GetItemText(curSelectedItem, nSubitem, chLine, 19);
			pDC->TextOut(nPageHMargin+nSubitem*nSpace, y,chLine);
		}
		curSelectedItem = GetListCtrl().GetNextSelectedItem(ps);
		y += nLineHeight; //Adjust y position
		nIndex++; 
	}
	if (pInfo) {
		//Draw a line above the footer
		pDC->MoveTo(nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		pDC->LineTo(nPageWidth-nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		//Set the footer font
		pDC->SelectObject(&FooterFont);
		//Format the footer
		line.Format(" \tPage %d",pInfo->m_nCurPage);
		line = m_sFoot + line;
		//Output the footer at the bottom using tabs
		pDC->TabbedTextOut(nPageHMargin, nPageHeight-nPageVMargin-nFooterHeight, line, 2, FooterTabStops, 0);
	}
	//Restore default settings
	pDC->SelectObject(OldFont);
}
void CAirsideReportListView::OnAirsidereportlistExport() 
{
// 	if(m_sExtension.IsEmpty())		//If no items listed, then return;
// 		return;
	ASSERT(m_sExtension.IsEmpty() == false);

	CString upExtension = m_sExtension;
	upExtension.MakeUpper();
	CFileDialog filedlg( FALSE,m_sExtension, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report files (*." + m_sExtension + ") | *." + m_sExtension + ";*." + upExtension + "|All type (*.*)|*.*|", NULL, 0, FALSE  );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

// 	CFileFind fileFind;
// 	if(fileFind.FindFile(csFileName))
// 	{
// 		if(MessageBox("File already exists, are you sure to overwrite it?", "Warning!", MB_YESNO) == IDNO)
// 			return;
// 	}
	//CopyFile(sFileName, csFileName, TRUE);
	ArctermFile  _file ;
	if(!_file.openFile(csFileName,WRITE) )
	{
	 	MessageBox(_T("Create File error"),_T("Error"),MB_OK) ;
	 	return  ;
	}
	CString error ;
	CTermPlanDoc* pTermPlanDoc = (CTermPlanDoc* )GetDocument();
	if(pTermPlanDoc == NULL)
		return;

	if(!pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GenerateCvsFileFormList(_file,m_lstCtrl,error))
	{
		MessageBox(error,"Error",MB_OK) ;
	}
	else
	{
		MessageBox(_T("Export Report Successful"),"Success",MB_OK) ;
	}

	// unassign
	// call saveDataSetToOtherFile()
	//	GetInputTerminal()->flightSchedule->saveDataSetToOtherFile( csFileName );
}

void CAirsideReportListView::OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
		{
			m_wndSortableHeaderCtrl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		}
		else
		{
			m_wndSortableHeaderCtrl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		}
		m_wndSortableHeaderCtrl.SaveSortList();
	}	
	*pResult = 0;
}