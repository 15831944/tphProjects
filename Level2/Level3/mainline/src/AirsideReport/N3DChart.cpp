// N3DChart.cpp : implementation file
//
#include "stdafx.h"
#include "N3DChart.h" 

/////ARC 3DChart Color
#define N3DChart_COLOR_RED        (RGB(255,0,0))
#define N3DChart_COLOR_GREEN      (RGB(0,255,0))
#define N3DChart_COLOR_BLUE       (RGB(0,0,255))
#define N3DChart_COLOR_YELLOW     (RGB(255,255,0))
#define N3DChart_COLOR_PURPLE     (RGB(255,0,255))
#define N3DChart_COLOR_TURQUOISE  (RGB(0,255,255))
#define N3DChart_COLOR_BLACK      (RGB(0,0,0))


IMPLEMENT_DYNAMIC(CN3DChart, CWnd)
CN3DChart::CN3DChart()
{
}

CN3DChart::~CN3DChart()
{
}

BEGIN_MESSAGE_MAP(CN3DChart, CWnd)
END_MESSAGE_MAP()

// CN3DChart message handlers

BOOL CN3DChart::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	BOOL bReturn = FALSE;
	bReturn = CreateControl(__uuidof(NChart),lpszWindowName,dwStyle,rect,pParentWnd, nID);
	if (bReturn )
	{
		// Query for the IDualChart - it is faster than the disp interface
		IDualChart *pDualChart = 0;
		if (FAILED(GetControlUnknown()->QueryInterface(__uuidof(IDualChart), (void**)&pDualChart)))
		{
			bReturn = FALSE;
			m_pDualChart = 0;			
		}
		else
		{
			// the pointer is addrefed (QueryInterface was called) - we attach it to a smart pointer and
			// thus does not forget to call Release when the object is destroyed
			m_pDualChart.Attach(pDualChart); 
			InitializeN3DChartState();
		}
	}

	return (bReturn);
}

bool CN3DChart::InitializeN3DChartState(void)
{
	bool bReturn = false;

	if(m_pDualChart) 
	{

		m_pDualChart->PutRedraw(VARIANT_FALSE);

		TCHAR szFileName[MAX_PATH];
		GetModuleFileName(NULL, szFileName, MAX_PATH);
		CString strFilePath(szFileName);
		strFilePath = strFilePath.Left(strFilePath.ReverseFind('\\'));
		strFilePath += _T("\\ChartState.nsv");

		_variant_t varFilePath(strFilePath);
		m_pDualChart->ImportExport()->Load(&varFilePath);

		SetMouseMode();
		m_pDualChart->PutRedraw(VARIANT_TRUE);
		bReturn = true;
	}
	else
		bReturn = false;

	return (bReturn);
}

bool CN3DChart::SetMouseMode(int iMouseDragMode /*= 0*/)
{//mouse event,0 - disable;1 - zoom;2 - move;3 - trackball;4 - ContextMenu;
	bool bReturn = false;

	if(m_pDualChart) 
	{
		switch(iMouseDragMode)
		{
		case 0://ArcMouseDragMode_Disable:		
		default:	
			m_pDualChart->EventManager()->PutMouseDragMode(mdDisabled);
			break;
		case 1://ArcMouseDragMode_Zoom:
			m_pDualChart->EventManager()->PutMouseDragMode(mdZoom);
			break;
		case 2://ArcMouseDragMode_Move:
			m_pDualChart->EventManager()->PutMouseDragMode(mdOffset);
			break;
		case 3://ArcMouseDragMode_Trackball:
			m_pDualChart->EventManager()->PutMouseDragMode(mdTrackBall);
			break;
		case 4:
			m_pDualChart->EventManager()->PutRMouseButtonClick(mmContextMenu);
			break;
		} 
		bReturn = true;
	}
	else
		bReturn = false;
    
	return (bReturn);
}


bool CN3DChart::PrintN3DChart(void)
{//print 3DChart	
	bool bReturn = false;

	if(m_pDualChart) 
	{
		m_pDualChart->PrintManager()->ShowPrintPreview();
		bReturn = true;
	}
	else
		bReturn = false;

	return (bReturn);
}

bool CN3DChart::ExportToBMPFile(CString strFilePath,BOOL bUseWindowDimensions /*= TRUE*/,long lWidth/* = 500L*/,long lHeight/* = 400L*/,long lBitsPerPixel/* = 24L*/)
{//export chart to bitmap file
	bool bReturn = false;

	if(m_pDualChart) 
	{
		VARIANT vFile;
		vFile.vt = VT_BSTR;
		vFile.bstrVal = _bstr_t(strFilePath );
		if(VARIANT_TRUE == m_pDualChart->ImportExport()->SaveBitmapImage(&vFile,bUseWindowDimensions?VARIANT_TRUE:VARIANT_FALSE,lWidth,lHeight,lBitsPerPixel) )
			bReturn = true;
		else
			bReturn = false;
	}
	else
		bReturn = false;

	return (bReturn);
}

bool CN3DChart::DrawChart(C2DChartData & c2dChartData)
{//draw chart
	bool bReturn = false;
	if(c2dChartData.m_vr2DChartData.empty())
		return (false);

	long lCategoriesCount = 0,lYCount = 0,lZCount = 0,lIndex = 0;

	if(m_pDualChart) 
	{
		m_pDualChart->PutRedraw(VARIANT_FALSE);	

		/////add data into chart
		m_pDualChart->Clear();
		m_pDualChart->Categories()->RemoveAll();
		m_pDualChart->Series()->RemoveAll();
		m_pDualChart->Levels()->RemoveAll();

		//2D 3DChart,always has 1 series.
		m_pDualChart->Series()->Add(_bstr_t(_T("")),VARIANT_FALSE);

		CString strMaxLen = _T("");
		//add categories.
		for (std::vector <CString >::const_iterator citrXTick = c2dChartData.m_vrXTickTitle.begin();\
			citrXTick != c2dChartData.m_vrXTickTitle.end();++ citrXTick) {
				m_pDualChart->Categories()->Add(_bstr_t(*citrXTick), VARIANT_TRUE);
				if(strMaxLen.GetLength() < (*citrXTick).GetLength())
					strMaxLen = (*citrXTick);
			}

			double dMinValue = 0x0FFFFFFF;
			double dMaxValue = -0x0FFFFFFF;
			lCategoriesCount = m_pDualChart->Categories()->GetCount();
			long lCategoriesIndex = 0;
			long lLevelsIndex = 0;
			for (std::vector <std::vector <double> >::const_iterator citr3DChartData = c2dChartData.m_vr2DChartData.begin();\
				citr3DChartData != c2dChartData.m_vr2DChartData.end();++ citr3DChartData,lLevelsIndex++) {
					m_pDualChart->Levels()->Add(_bstr_t(_T("")),VARIANT_FALSE);
					lCategoriesIndex = 0;
					for (std::vector <double>::const_iterator citr3DChartDataItem = (*citr3DChartData).begin();\
						citr3DChartDataItem != (*citr3DChartData).end();++ citr3DChartDataItem) {
							if(lCategoriesCount <= 0 && lLevelsIndex <= 0){
								m_pDualChart->Categories()->Add(_bstr_t(_T("")), VARIANT_TRUE);
							}
							m_pDualChart->DataAt(lCategoriesIndex++,0,lLevelsIndex)->PutValue1((*citr3DChartDataItem));
							if(*citr3DChartDataItem > dMaxValue)
								dMaxValue = *citr3DChartDataItem;
							if(*citr3DChartDataItem < dMinValue)
								dMinValue = *citr3DChartDataItem;
						}
				}

				m_pDualChart->Axes(atLeftAxis)->PutVisible(VARIANT_FALSE);
				m_pDualChart->Axes(atFrontAxis)->PutVisible(VARIANT_FALSE);
				m_pDualChart->Axes(atSeriesAxis)->PutVisible(VARIANT_FALSE);
				/////set x axis' title
				m_pDualChart->Axes(atCategoriesAxis)->PutVisible(VARIANT_TRUE);
				m_pDualChart->Axes(atCategoriesAxis)->PutTitle(_bstr_t(c2dChartData.m_strXtitle) );
				m_pDualChart->Axes(atCategoriesAxis)->TitleTextEx()->PutOriginDistance(0.12f);
				m_pDualChart->Axes(atCategoriesAxis)->SetMajorShowAtWall(cwtBack, VARIANT_TRUE);
				m_pDualChart->Axes(atCategoriesAxis)->SetMajorShowAtWall(cwtFloor, VARIANT_TRUE);
				m_pDualChart->Axes(atCategoriesAxis)->MajorLineBorder()->PutColor(RGB(0,0,0));
				m_pDualChart->Axes(atCategoriesAxis)->TextEx()->PutOrientation(0L);
				{
					IFontDisp *pFontDisp = 0;
					m_pDualChart->Axes(atCategoriesAxis)->TitleTextEx()->get_Font(&pFontDisp);
					CComQIPtr<IFont> pFont(pFontDisp);
					pFont->put_Charset(0) ;
					pFont->put_Bold(TRUE);
					pFont->put_Italic(FALSE);
					pFont->put_Name(_bstr_t("MS Shell Dlg"));
					HDC hDC = ::GetDC(NULL);
					long lCaps = ::GetDeviceCaps(hDC, LOGPIXELSY);
					CY fontSize;
					fontSize.Hi = 0;
					fontSize.Lo = (long)(5 * (((float)720000) / lCaps));
					pFont->put_Size(fontSize);
					pFont->put_Strikethrough(FALSE);
					pFont->put_Underline(FALSE); 
					pFont->put_Weight(200);	 
					{
						m_pDualChart->Axes(atCategoriesAxis)->Step = FALSE;
						m_pDualChart->Axes(atCategoriesAxis)->PutStep(strMaxLen.GetLength()*fontSize.Lo*2);//adjust tick
					}
				}

				/////set y axis' title
				m_pDualChart->Axes(atBackAxis)->PutVisible(VARIANT_TRUE);
				m_pDualChart->Axes(atRightAxis)->PutVisible(VARIANT_TRUE);
				m_pDualChart->Axes(atBackAxis)->PutTitle(_bstr_t(c2dChartData.m_strYtitle) );
				m_pDualChart->Axes(atBackAxis)->PutFormatString( _bstr_t(_T("%V(0,TURE,1)")) );
				m_pDualChart->Axes(atRightAxis)->PutFormatString( _bstr_t(_T("%V(0,TURE,1)")) );
				m_pDualChart->Axes(atBackAxis)->TitleTextEx()->PutOriginDistance(0.12f);
				m_pDualChart->Axes(atBackAxis)->SetMajorShowAtWall(cwtBack, VARIANT_TRUE);
				m_pDualChart->Axes(atRightAxis)->SetMajorShowAtWall(cwtBack, VARIANT_TRUE);
				m_pDualChart->Axes(atBackAxis)->MajorLineBorder()->PutColor(RGB(0,0,0));
				m_pDualChart->Axes(atRightAxis)->MajorLineBorder()->PutColor(RGB(0,0,0)); 
				m_pDualChart->Axes(atBackAxis)->Min = dMinValue;
				m_pDualChart->Axes(atBackAxis)->Max = dMaxValue;
				if(dMaxValue - dMinValue < 10)
					m_pDualChart->Axes(atBackAxis)->AutoStep = TRUE;
				else
					m_pDualChart->Axes(atBackAxis)->Step = 5;
				m_pDualChart->Axes(atRightAxis)->Min = dMinValue;
				m_pDualChart->Axes(atRightAxis)->Max = dMaxValue;
				if(dMaxValue - dMinValue < 10)
					m_pDualChart->Axes(atRightAxis)->AutoStep = TRUE;
				else
					m_pDualChart->Axes(atRightAxis)->Step = 5;
				/*m_pDualChart->Axes(atBackAxis)->RoundToTick = TRUE;
				m_pDualChart->Axes(atRightAxis)->RoundToTick = TRUE;*/
				{
					IFontDisp *pFontDisp = 0;
					m_pDualChart->Axes(atBackAxis)->TitleTextEx()->get_Font(&pFontDisp);
					CComQIPtr<IFont> pFont(pFontDisp);
					pFont->put_Charset(0) ;
					pFont->put_Italic(FALSE);
					pFont->put_Name(_bstr_t("MS Shell Dlg"));
					HDC hDC = ::GetDC(NULL);
					long lCaps = ::GetDeviceCaps(hDC, LOGPIXELSY);
					CY fontSize;
					fontSize.Hi = 0;
					fontSize.Lo = (long)(5 * (((float)720000) / lCaps));
					pFont->put_Size(fontSize);
					pFont->put_Strikethrough(FALSE);
					pFont->put_Underline(FALSE); 
					pFont->put_Weight(200);
				}

				/////set chart's title 
				m_pDualChart->LabelCollection()->RemoveAll();
				IDualLabelPtr pHeaderTitle = m_pDualChart->LabelCollection()->AddHeader(_bstr_t(c2dChartData.m_strChartTitle));
				IDualLabelPtr pFootererTitle = m_pDualChart->LabelCollection()->AddFooter(_bstr_t(c2dChartData.m_strFooter));
				pHeaderTitle->PutOriginOffsetY(4.0f);
				pFootererTitle->PutOriginOffsetX(2.0f);
				pFootererTitle->PutOriginOffsetY(98.0f);
				{
					//header
					IFontDisp *pFontDisp = 0;
					pHeaderTitle->TextEx()->get_Font(&pFontDisp);
					CComQIPtr<IFont> pFont(pFontDisp);
					pFont->put_Charset(0);
					pFont->put_Italic(FALSE);
					pFont->put_Name(_bstr_t("Arial"));
					HDC hDC = ::GetDC(NULL);
					long lCaps = ::GetDeviceCaps(hDC, LOGPIXELSY);
					CY fontSize;
					fontSize.Hi = 0;
					fontSize.Lo = (long)(30 * (((float)720000) / lCaps));
					pFont->put_Size(fontSize);
					pFont->put_Strikethrough(FALSE);
					pFont->put_Underline(FALSE);
					pFont->put_Weight(800);
				}
				{
					//footer
					IFontDisp *pFontDisp = 0;
					pFootererTitle->TextEx()->get_Font(&pFontDisp);
					CComQIPtr<IFont> pFont(pFontDisp);
					pFont->put_Charset(0);
					pFont->put_Italic(FALSE);
					pFont->put_Name(_bstr_t("Arial"));
					HDC hDC = ::GetDC(NULL);
					long lCaps = ::GetDeviceCaps(hDC, LOGPIXELSY);
					CY fontSize;
					fontSize.Hi = 0;
					fontSize.Lo = (long)(6 * (((float)720000) / lCaps));
					pFont->put_Size(fontSize);
					pFont->put_Strikethrough(FALSE);
					pFont->put_Underline(FALSE);
					pFont->put_Weight(200);
				}

				/////background color 
				COLORREF crColor = RGB(255,255,255);//RGB(143,143,243);	// 	
				m_pDualChart->Background()->FillEffect()->PutType(fetSolidColor);
				m_pDualChart->Background()->FillEffect()->SetSolidColor(crColor);
				m_pDualChart->Walls(cwtBack)->FillEffect()->PutType(fetSolidColor);
				m_pDualChart->Walls(cwtBack)->FillEffect()->SetSolidColor(crColor);

				//wall visible is false
				m_pDualChart->Walls(cwtLeft)->PutVisible(VARIANT_FALSE);
				m_pDualChart->Walls(cwtBack)->PutVisible(VARIANT_FALSE);
				m_pDualChart->Walls(cwtRight)->PutVisible(VARIANT_FALSE);
				m_pDualChart->Walls(cwtFront)->PutVisible(VARIANT_FALSE);
				m_pDualChart->Walls(cwtFloor)->PutVisible(VARIANT_FALSE);

				/////chart fill the client.
				m_pDualChart->LeftMargin = 10;
				m_pDualChart->TopMargin = 8;
				m_pDualChart->RightMargin = 88;
				m_pDualChart->BottomMargin = 90;		



				//present groups
				m_pDualChart->PresentGroups()->RemoveAll();
				m_vr3DChartType.push_back(Arc3DChartType_2D_Bar);
				m_pDualChart->PresentGroups()->Add(pgtBar);
				COLORREF color = RGB(143,143,243);	
				m_pDualChart->PresentGroups()->GetCommonAt(0)->UniformAppearances()->RemoveAll();
				if(c2dChartData.m_vrLegend.empty())
				{
					IDualAppearancePtr ptrAppearance  = m_pDualChart->PresentGroups()->GetCommonAt(0)->UniformAppearances()->Add(_bstr_t(_T("")));			
					color = GetN3DChartColor();	
					ptrAppearance->FillEffect()->SetSolidColor(color);
					ptrAppearance->Material()->SetAmbientDiffuseColor(color);
					ptrAppearance->Border()->PutColor(color);
					m_pDualChart->PresentGroups()->GetCommonAt(0)->PutVisible(VARIANT_TRUE);
					m_pDualChart->PresentGroups()->GetCommonAt(0)->PutShowInLegend(VARIANT_FALSE); 
					/////show legend.
					m_pDualChart->Legend()->PutLegendMode(lmDisabled);
				}
				else
				{
					long lColorIndex = 0;
					for (std::vector <CString >::const_iterator citrLegend = c2dChartData.m_vrLegend.begin();\
						citrLegend != c2dChartData.m_vrLegend.end();++citrLegend) {
							IDualAppearancePtr ptrAppearance  = m_pDualChart->PresentGroups()->GetCommonAt(0)->UniformAppearances()->Add(_bstr_t(*citrLegend));
							color = GetN3DChartColor(lColorIndex++);	
							ptrAppearance->FillEffect()->SetSolidColor(color);
							ptrAppearance->Material()->SetAmbientDiffuseColor(color);
							ptrAppearance->Border()->PutColor(color);
						}	
						m_pDualChart->PresentGroups()->GetCommonAt(0)->PutVisible(VARIANT_TRUE);
						m_pDualChart->PresentGroups()->GetCommonAt(0)->PutShowInLegend(VARIANT_TRUE);
						/////show legend.
						m_pDualChart->Legend()->PutLegendMode(lmAutomatic);
						m_pDualChart->Legend()->PutMarginX(98);
						m_pDualChart->Legend()->PutMarginY(45);
				}
				m_pDualChart->PresentGroups()->GetCommonAt(0)->PutLegendMode(pglmUniformAppearances);
				m_pDualChart->PresentGroups()->GetCommonAt(0)->DisplayOnAxis(atLeftAxis,VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(0)->DisplayOnAxis(atFrontAxis,VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(0)->DisplayOnAxis(atBackAxis,VARIANT_TRUE);
				m_pDualChart->PresentGroups()->GetCommonAt(0)->DisplayOnAxis(atRightAxis,VARIANT_TRUE);
				if((int)c2dChartData.m_vr2DChartData.size() > 1)
				{
					((IDualBarPresentGroupPtr)(m_pDualChart->PresentGroups()->GetAt(0)) )->WidthPercent  = 70;
					((IDualBarPresentGroupPtr)(m_pDualChart->PresentGroups()->GetAt(0)) )->GapPercent = 20;
				}
				m_pDualChart->Get3DView()->SetPredefinedProjection(pp2D);//for 2D
				m_pDualChart->Get3DView()->put_ProjectionType(ptOrthogonal);

				m_pDualChart->PutRedraw(VARIANT_TRUE );
				bReturn = true;
	}
	else
		bReturn = false;

	return (bReturn);
}

COLORREF CN3DChart::GetN3DChartColor(long lIndex /*= 0L*/)
{
	COLORREF color = RGB(143,143,243);
	switch(lIndex)
	{
	case 0:
		color = N3DChart_COLOR_RED;
		break;
	case 1:
		color = N3DChart_COLOR_GREEN;
		break;
	case 2:
		color = N3DChart_COLOR_BLUE;
		break;
	case 3:
		color = N3DChart_COLOR_YELLOW;
		break;
	case 4:
		color = N3DChart_COLOR_PURPLE;
		break;
	case 5:
		color = N3DChart_COLOR_TURQUOISE;
		break;
	case 6:
		color = N3DChart_COLOR_BLACK;
		break;
	default:
		int iRed = rand()%255,iGreen = rand()%255,iBlue = rand()%255;	
		color = RGB(iRed,iGreen,iBlue);
		break;
	}	
	return (color);
}

bool CN3DChart::AddNN3DChartPresentGroupType(Arc3DChartType actype3DChart)
{
	std::vector<Arc3DChartType>::iterator itrChartTypeExistent;
	itrChartTypeExistent = std::find(m_vr3DChartType.begin(),m_vr3DChartType.end(),actype3DChart);
	if(itrChartTypeExistent != m_vr3DChartType.end())return (true);
    
	bool bReturn = false; 
 	switch (actype3DChart)
	{
	default://default is 2D Bar
	case Arc3DChartType_2D_Bar://ArcChartType_2D_Bar:
		{ 
			if(m_pDualChart) 
			{				
				m_vr3DChartType.push_back(actype3DChart);
				m_pDualChart->PresentGroups()->Add(pgtBar);	
				COLORREF color = RGB(143,143,243);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->UniformAppearances()->RemoveAll();
				long lCount = m_pDualChart->PresentGroups()->GetCommonAt(0)->UniformAppearances()->GetCount();//need add one PresentGroups at least.
				for (long lIndex = 0;lIndex < lCount;lIndex++) {
					_bstr_t bstrLabel = m_pDualChart->PresentGroups()->GetCommonAt(0)->UniformAppearances()->GetAt(lIndex)->GetLabel();
					IDualAppearancePtr ptrAppearance  = m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->UniformAppearances()->Add(bstrLabel);
					color = GetN3DChartColor(lIndex);
					ptrAppearance->FillEffect()->SetSolidColor(color);
					ptrAppearance->Material()->SetAmbientDiffuseColor(color);
					ptrAppearance->Border()->PutColor(color);
				}	
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->PutVisible(VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->PutShowInLegend(VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->PutLegendMode(pglmUniformAppearances);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->DisplayOnAxis(atLeftAxis,VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->DisplayOnAxis(atFrontAxis,VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->DisplayOnAxis(atBackAxis,VARIANT_TRUE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->DisplayOnAxis(atRightAxis,VARIANT_TRUE); 
				m_pDualChart->Get3DView()->SetPredefinedProjection(pp2D);//for 2D
				m_pDualChart->Get3DView()->put_ProjectionType(ptOrthogonal);
				bReturn = true;
			}
			else
				bReturn = false; 

			break;
		}
	case Arc3DChartType_2D_Line://ArcChartType_2D_Line:
		{
			if(m_pDualChart) 
			{
				m_vr3DChartType.push_back(actype3DChart);
				m_pDualChart->PresentGroups()->Add(pgtLine);
				COLORREF color = RGB(143,143,243);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->UniformAppearances()->RemoveAll();
				long lCount = m_pDualChart->PresentGroups()->GetCommonAt(0)->UniformAppearances()->GetCount();
				for (long lIndex = 0;lIndex < lCount;lIndex++) {
					_bstr_t bstrLabel = m_pDualChart->PresentGroups()->GetCommonAt(0)->UniformAppearances()->GetAt(lIndex)->GetLabel();
					IDualAppearancePtr ptrAppearance  = m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->UniformAppearances()->Add(bstrLabel);
					m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->UniformAppearances()->Add(bstrLabel);
					color = GetN3DChartColor(lIndex);
					ptrAppearance->FillEffect()->SetSolidColor(color);
					ptrAppearance->Material()->SetAmbientDiffuseColor(color);
					ptrAppearance->Border()->PutColor(color);
				}	
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->PutVisible(VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->PutShowInLegend(VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->PutLegendMode(pglmUniformAppearances);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->DisplayOnAxis(atLeftAxis,VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->DisplayOnAxis(atFrontAxis,VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->DisplayOnAxis(atBackAxis,VARIANT_TRUE);
				m_pDualChart->PresentGroups()->GetCommonAt(m_pDualChart->PresentGroups()->GetCount() -1)->DisplayOnAxis(atRightAxis,VARIANT_TRUE);
				m_pDualChart->Get3DView()->SetPredefinedProjection(pp2D);//for 2D
				m_pDualChart->Get3DView()->put_ProjectionType(ptOrthogonal);
				bReturn = true;
			}
			else
				bReturn = false;

			break;
		}
	}

	return (bReturn);
}

//change chart type 
bool CN3DChart::ShowN3DChartType(Arc3DChartType actype3DChart /*= CARC3DChart::ArcChartType_2D_Bar*/)
{
	bool bReturn = false; 

	AddNN3DChartPresentGroupType(actype3DChart);

	if(m_pDualChart) 
	{
		long lIndex = 0;
		for (std::vector < Arc3DChartType>::const_iterator citr3DChartType = m_vr3DChartType.begin();\
			citr3DChartType != m_vr3DChartType.end();++citr3DChartType,++lIndex)
		{
			if((*citr3DChartType) == actype3DChart)
			{
				m_pDualChart->PresentGroups()->GetCommonAt(lIndex)->PutVisible(VARIANT_TRUE);
				m_pDualChart->PresentGroups()->GetCommonAt(lIndex)->PutShowInLegend(VARIANT_TRUE);
			}
			else
			{
				m_pDualChart->PresentGroups()->GetCommonAt(lIndex)->PutVisible(VARIANT_FALSE);
				m_pDualChart->PresentGroups()->GetCommonAt(lIndex)->PutShowInLegend(VARIANT_FALSE);
			}
		}
	}
	else
		bReturn = false;

	return (bReturn);
}

