#include "StdAfx.h"
#include "cmpdisplay.h"
#include "CmpParametersWnd.h"
#include "Cmp3DChartWnd.h" 
#include "CmpReportFrameWnd.h"
#include "CmpReportCtrl.h"
#include "ComparativeList.h"
#include "..\Main\ComparativePlot.h"

CCmpDisplay::CCmpDisplay()
{
	m_pReportFrameWnd = NULL;
	m_pReportManager = NULL;
	m_chartType = CMPBar_2D;
}

CCmpDisplay::~CCmpDisplay()
{
	if (m_pReportFrameWnd == NULL)
		return;

	HWND hReportFrameWnd;
	m_pReportFrameWnd->GetReportFramPtr()->get_MainWindow((OLE_HANDLE*)&hReportFrameWnd);
	if (!::IsWindow(hReportFrameWnd))
		return;

	// parameters window
	if (::IsWindow(m_wndParas.m_hWnd))
	{
		m_pReportFrameWnd->GetReportFramPtr()->DestroyChildWindow((OLE_HANDLE)m_wndParas.m_hWnd);
	}

	// list window
	if (::IsWindow(m_wndListCtrl.m_hWnd))
	{
		m_pReportFrameWnd->GetReportFramPtr()->DestroyChildWindow((OLE_HANDLE)m_wndListCtrl.m_hWnd);
	}

	// 3d chart
	if (m_wnd3dChart.Get3dChartPtr().GetInterfacePtr() != NULL)
	{
		m_pReportFrameWnd->GetReportFramPtr()->DetachChildWindow((OLE_HANDLE)m_wnd3dChart.Get3DChartWnd());
		m_wnd3dChart.DestroyWindow();
	}
	int i = 0;
	for(i = 0; i < (int)m_vPWndListCtrl.size(); i++)
	{
		if(::IsWindow(m_vPWndListCtrl[i]->m_hWnd))
			m_pReportFrameWnd->GetReportFramPtr()->DestroyChildWindow((OLE_HANDLE)m_vPWndListCtrl[i]->m_hWnd);
		delete m_vPWndListCtrl[i];
	}
	m_vPWndListCtrl.clear();
	for(i = 0; i < (int)m_vPWnd3dChart.size(); i++)
	{
		if(::IsWindow(m_vPWnd3dChart[i]->m_hWnd))
			m_pReportFrameWnd->GetReportFramPtr()->DestroyChildWindow((OLE_HANDLE)m_vPWnd3dChart[i]->m_hWnd);
		delete m_vPWnd3dChart[i];
	}	
	m_vPWnd3dChart.clear(); 
}

BOOL CCmpDisplay::InitAllWnds()
{
	if (m_pReportFrameWnd == NULL)
		return FALSE;

	HWND hReportFrameWnd;
	m_pReportFrameWnd->GetReportFramPtr()->get_MainWindow((OLE_HANDLE*)&hReportFrameWnd);
	if (!::IsWindow(hReportFrameWnd))
		return FALSE;

	CWnd* pReportFrameWnd = CWnd::FromHandle(hReportFrameWnd);
	CRect rtEmpty;
	rtEmpty.SetRectEmpty();
	bool bHasChildWindow = true;

	// parameters window
	if (!::IsWindow(m_wndParas.m_hWnd))
	{
		if (!m_wndParas.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rtEmpty, pReportFrameWnd, 11))
			return FALSE;
		m_pReportFrameWnd->GetReportFramPtr()->AppendWindow(_bstr_t("Parameters"), (OLE_HANDLE)m_wndParas.m_hWnd, _bstr_t("Parameters"));	
		bHasChildWindow = false;
	}

	// list window
	if (!::IsWindow(m_wndListCtrl.m_hWnd))
	{
		if (!m_wndListCtrl.Create(WS_CHILD|WS_VISIBLE||LVS_REPORT, rtEmpty, pReportFrameWnd, 11))
			return FALSE;

		m_pReportFrameWnd->GetReportFramPtr()->AppendWindow(_bstr_t("Data List"),
			(OLE_HANDLE)m_wndListCtrl.m_hWnd,
			_bstr_t("Data List"));
	}

	// 3d chart
	if (!::IsWindow(m_wnd3dChart.Get3DChartWnd()))
	{
		if (!m_wnd3dChart.Create(NULL, WS_CHILD|WS_VISIBLE, rtEmpty, pReportFrameWnd, 11))
			return FALSE;

		m_pReportFrameWnd->GetReportFramPtr()->AppendWindow(_bstr_t("Data Graph"),
			(OLE_HANDLE)m_wnd3dChart.Get3DChartWnd(),
			_bstr_t("Data Graph"));
	}	

	if (!bHasChildWindow)
	{
		m_pReportFrameWnd->GetReportFramPtr()->SplitClient((OLE_HANDLE)m_wndParas.m_hWnd, (OLE_HANDLE)m_wndListCtrl.m_hWnd,
			ReportFrameLib::targetLeft, 200);
	}

	m_wndParas.SetReportManager(m_pReportManager);
	m_wndParas.InitParaWnd();

	return TRUE;
}

void CCmpDisplay::UpdateAllWnds()
{
	int i = 0;
	for(i = 0; i < (int)m_vPWndListCtrl.size(); i++)
	{
		m_pReportFrameWnd->GetReportFramPtr()->DestroyChildWindow((OLE_HANDLE)m_vPWndListCtrl[i]->m_hWnd);
		delete m_vPWndListCtrl[i];
	}
	m_vPWndListCtrl.clear();
	for(i = 0; i < (int)m_vPWnd3dChart.size(); i++)
	{
		m_pReportFrameWnd->GetReportFramPtr()->DetachChildWindow((OLE_HANDLE)m_vPWnd3dChart[i]->Get3DChartWnd());
		m_vPWnd3dChart[i]->DestroyWindow();
		delete m_vPWnd3dChart[i];
	}	
	m_vPWnd3dChart.clear(); 

	UpdateParaWnd();
	UpdateListWnd();
	Update3DChart();

	if(m_vPWnd3dChart.size() <= 0 && m_vPWndListCtrl.size() <= 0)
		return;

	//delete all windows
	if(::IsWindow(m_wndListCtrl.m_hWnd))
		m_pReportFrameWnd->GetReportFramPtr()->DestroyChildWindow((OLE_HANDLE)m_wndListCtrl.m_hWnd);
	if(::IsWindow(m_wnd3dChart.Get3DChartWnd()))
	{
		m_pReportFrameWnd->GetReportFramPtr()->DetachChildWindow((OLE_HANDLE)m_wnd3dChart.Get3DChartWnd());
		m_wnd3dChart.DestroyWindow();
	}

	CComparativeProject* pCompProj = m_pReportManager->GetCmpReport()->GetComparativeProject();
	CInputParameter* inputParam = pCompProj->GetInputParam();
	CReportsManager* reportsManager = inputParam->GetReportsManagerPtr();
	std::vector<CReportToCompare>& vReportList = reportsManager->GetReportsList();

	for(int i = 0; i < (int)vReportList.size()&& i <static_cast<int>(m_vPWndListCtrl.size()); i++)
	{
		m_pReportFrameWnd->GetReportFramPtr()->AppendWindow(_bstr_t(vReportList[i].GetName()+" Data Graph"),
			(OLE_HANDLE)m_vPWnd3dChart[i]->Get3DChartWnd(),
			_bstr_t(vReportList[i].GetName()+" Data Graph"));

		m_pReportFrameWnd->GetReportFramPtr()->AppendWindow(_bstr_t(vReportList[i].GetName()+" Data List"),
			(OLE_HANDLE)m_vPWndListCtrl[i]->m_hWnd,
			_bstr_t(vReportList[i].GetName()+" Data List"));
	}

	m_pReportFrameWnd->GetReportFramPtr()->SplitClient((OLE_HANDLE)m_wndParas.m_hWnd, (OLE_HANDLE)m_vPWnd3dChart[0]->Get3DChartWnd(),
		ReportFrameLib::targetLeft, 200);

	return;
}

void CCmpDisplay::UpdateParaWnd()
{
	if (!::IsWindow(m_wndParas.m_hWnd))
		return;

	m_wndParas.UpdateParaWnd();
}

void CCmpDisplay::UpdateListWnd()
{		
	static bool bRefresh = true;

	CComparativeProject* pCompProj = NULL;
	pCompProj = m_pReportManager->GetCmpReport()->GetComparativeProject();
	if(pCompProj == NULL)
	{
		bRefresh = true;
		return ;
	}

	CInputParameter* inputParam = pCompProj->GetInputParam();
	CReportsManager* reportsManager = inputParam->GetReportsManagerPtr();
	std::vector<CReportToCompare>& vReportList = reportsManager->GetReportsList();

	CModelsManager* modelsManager = inputParam->GetModelsManagerPtr();
	std::vector<CModelToCompare *>& vModelList = modelsManager->GetModelsList();

	const CComparativeReportResultList &crrList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vReport = crrList.GetReportResult();

	for(int i = 0; i < static_cast<int>(vReport.size()); i++)
	{
		HWND hReportFrameWnd;
		m_pReportFrameWnd->GetReportFramPtr()->get_MainWindow((OLE_HANDLE*)&hReportFrameWnd);
		if (!::IsWindow(hReportFrameWnd))
			return ;

		CWnd* pReportFrameWnd = CWnd::FromHandle(hReportFrameWnd);
		CRect rtEmpty;
		rtEmpty.SetRectEmpty();

		CListCtrl* pListCtrl = new CListCtrl;
		if (!pListCtrl->Create(WS_CHILD|WS_VISIBLE||LVS_REPORT, rtEmpty, pReportFrameWnd, 11+i))
			return ;
		pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES);

		CComparativeList cmpList(m_pReportManager->GetCmpReport()->GetTerminal(), *pListCtrl, vModelList);
		cmpList.RefreshData(*vReport[i]);
		m_vPWndListCtrl.push_back(pListCtrl);
	}
}

void CCmpDisplay::Change3DChart(ThreeDChartType enmChartType)
{
	m_chartType = enmChartType;

	CComparativeProject* pCompProj = NULL;
	pCompProj = m_pReportManager->GetCmpReport()->GetComparativeProject();
	if(pCompProj == NULL)
		return ;

	CInputParameter* inputParam = pCompProj->GetInputParam();
	CReportsManager* reportsManager = inputParam->GetReportsManagerPtr();
	std::vector<CReportToCompare>& vReportList = reportsManager->GetReportsList();

	CModelsManager* modelsManager = inputParam->GetModelsManagerPtr();
	std::vector<CModelToCompare *>& vModelList = modelsManager->GetModelsList();

	const CComparativeReportResultList &crrList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vReport = crrList.GetReportResult();

	for(int i = 0; i < (int)m_vPWnd3dChart.size(); i++)
	{
		N3DCHARTLib::IDualChartPtr& pChart = m_vPWnd3dChart[i]->Get3dChartPtr();
		CComparativePlot plot(enmChartType, pChart, vModelList);
		plot.Draw3DChart(*vReport[i]);
	}
}

void CCmpDisplay::Update3DChart()
{
	CComparativeProject* pCompProj = NULL;
	pCompProj = m_pReportManager->GetCmpReport()->GetComparativeProject();
	if(pCompProj == NULL)
		return ;

	CInputParameter* inputParam = pCompProj->GetInputParam();
	CReportsManager* reportsManager = inputParam->GetReportsManagerPtr();
	std::vector<CReportToCompare>& vReportList = reportsManager->GetReportsList();

	CModelsManager* modelsManager = inputParam->GetModelsManagerPtr();
	std::vector<CModelToCompare *>& vModelList = modelsManager->GetModelsList();

	const CComparativeReportResultList &crrList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vReport = crrList.GetReportResult();

	for(int i = 0; i < static_cast<int>(vReport.size()); i++)
	{
		HWND hReportFrameWnd;
		m_pReportFrameWnd->GetReportFramPtr()->get_MainWindow((OLE_HANDLE*)&hReportFrameWnd);
		if (!::IsWindow(hReportFrameWnd))
			return ;

		CWnd* pReportFrameWnd = CWnd::FromHandle(hReportFrameWnd);
		CRect rtEmpty;
		rtEmpty.SetRectEmpty();

		CCmp3DChartWnd* p3dChart = new CCmp3DChartWnd;
		if (!p3dChart->Create(NULL, WS_CHILD|WS_VISIBLE, rtEmpty, pReportFrameWnd, 11+i))
			return ;

		CComparativePlot plot(m_chartType, p3dChart->Get3dChartPtr(), vModelList);
		plot.Draw3DChart(*vReport[i]);

		m_vPWnd3dChart.push_back(p3dChart);
	}
}

void CCmpDisplay::PrintGraph()
{
	if (!::IsWindow(m_wnd3dChart.GetSafeHwnd()))
		return;

	m_wnd3dChart.Get3dChartPtr()->PrintManager()->ShowPrintPreview();

	return;
}

void CCmpDisplay::ExportGraph(BSTR bstrFileName)
{
	if (!::IsWindow(m_wnd3dChart.GetSafeHwnd()))
		return;

	N3DCHARTLib::IDualChartPtr& pChart = m_wnd3dChart.Get3dChartPtr();
	N3DCHARTLib::IDualImportExport &pImportExport = pChart->ImportExport();
	_variant_t vSavePath(bstrFileName);
	_variant_t vUseWindowDimension(true);

	pImportExport.SaveBitmapImage(&vSavePath,true,800,600,24);	

}
void CCmpDisplay::PrintListData()
{
	if (!::IsWindow(m_wndListCtrl.m_hWnd))
		return;
	
	CString m_sHeader ="List Header String";
	CString m_sFoot = "List Foot String";

	CDC dc;
	CPrintDialog printDlg(FALSE, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE);

	//...
	if (printDlg.DoModal() == IDCANCEL)
		return;
	dc.Attach(printDlg.GetPrinterDC());
	CString strTitle = "TEST FOR PRINTING";
	if(!dc.StartDoc((LPCTSTR)strTitle))
	{
		return;
	}

	OnDrawListData(&dc,NULL);
	dc.EndDoc();

	dc.Detach();

}

void CCmpDisplay::OnDrawListData(CDC *pDC, PRINTDLGA *pInfo)
{
	TEXTMETRIC metrics;

	CFont TitleFont; //Font for Title
	CFont HeadingFont; //Font for headings
	CFont DetailFont; //Font for detail lines
	CFont FooterFont; //Font for footer

	//Tab stops at 3.5 inches and 6.5 inches
//	int FooterTabStops[] = {350, 650};

	short cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
	short cyInch = pDC->GetDeviceCaps(LOGPIXELSY);


//creat font

	#define MYFONTSIZE 14 
	#define HFONTNAME "MS Sans Serif"

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

	int	nPageVMargin,nPageHMargin,nPageHeight,nPageWidth;
	nPageVMargin = pDC->GetDeviceCaps(LOGPIXELSY) / 2;
	nPageHMargin = pDC->GetDeviceCaps(LOGPIXELSX) / 2;
	nPageHeight  = pDC->GetDeviceCaps(VERTRES);
	nPageWidth   = pDC->GetDeviceCaps(HORZRES);

	CFont* OldFont = pDC->SelectObject(&FooterFont);	
	pDC->GetTextMetrics(&metrics);

	int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;
	pDC->SelectObject(&TitleFont);

	//Retrieve the heading font measurements
	pDC->GetTextMetrics(&metrics);

	//Compute the heading line height
	int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	
	//calculate the pages 
	int nTotalCount = m_wndListCtrl.GetItemCount();
	int nItemPerPage = (int)(nPageHeight - 2*nPageHMargin -nFooterHeight -6)/nLineHeight;
	int nPages = (int)nTotalCount/nItemPerPage + 1;
	
	//print the data page by page
	int nCurPage = 0;
	int nPrintItemPos = 0;

	for (nCurPage = 0; nCurPage<nPages ;nCurPage++)
	{
		pDC->StartPage();

		int y = 0; //Current y position on report
		//Set Y to the line height.
		y += static_cast<int>(nLineHeight*1.5+nPageVMargin);
		
		CString m_sHeader = "List Data";
		pDC->TextOut(nPageHMargin, nPageVMargin, m_sHeader);
		//Draw a line under the heading
		pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
		pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
		//Set the Heading font
		pDC->SelectObject(&HeadingFont);
		y += nLineHeight;

		//Format the heading
		// get listctrl's header info
		int nCol = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
		HDITEMA hi;
		hi.mask = HDI_TEXT ;
		hi.cchTextMax = 19;
		char chBuffer[20];
		hi.pszText = chBuffer;
		int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
		for(int i = 0; i < nCol; i++ )
		{
			::ZeroMemory( chBuffer, 20);
			m_wndListCtrl.GetHeaderCtrl()->GetItem( i,&hi);
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

		while ( nPrintItemPos <= (nCurPage+1)*nItemPerPage) 
		{
			//Format the detail line
			for( int nSubitem = 0; nSubitem < nCol; nSubitem++ )
			{

				char chLine[20];
				m_wndListCtrl.GetItemText(nPrintItemPos,nSubitem,chLine,19);
				pDC->TextOut(nPageHMargin+nSubitem*nSpace, y,chLine);
			}

			y += nLineHeight; //Adjust y position
			nPrintItemPos++; 
			if (nPrintItemPos+1 > nTotalCount)
				break;
		}

		CString line;
		//Draw a line above the footer
		pDC->MoveTo(nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		pDC->LineTo(nPageWidth-nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		//Set the footer font
		pDC->SelectObject(FooterFont);
		//Format the footer
		line.Format("Page %d  Total %d",nCurPage + 1,nPages);
	
		SIZE sizeOfString = pDC->GetTextExtent(line);
		pDC->TextOut(nPageWidth - nPageHMargin - sizeOfString.cx ,nPageHeight-nPageVMargin-nFooterHeight,line);
		pDC->EndPage();
	}
	//Restore default settings
	pDC->SelectObject(OldFont);	

}
