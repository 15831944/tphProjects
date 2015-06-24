// FlowChartPane.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FlowChartPane.h"
#include "MathematicView.h"
#include "TermPlanDoc.h"
#include "XPStyle\NewMenu.h"
#include "MainFrm.h"
#include <Shlwapi.h>
#include "DlgAttrOfLink.h"
#include "DotNetTabWnd.h"
#include "ProcessFlowView.h"

#include <Common/OleDragDropManager.h>

using namespace FLOWCHARTLib;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlowChartPane
#define IDC_FLOWCHART 10

IMPLEMENT_DYNCREATE(CFlowChartPane, CView)

CFlowChartPane::CFlowChartPane()
{
	m_curItem = NULL;
	m_pFlowVect = NULL;
	m_strFlowName = "";
	m_strProcessName = "";
	m_pMyDocument = NULL;
}

CFlowChartPane::~CFlowChartPane()
{
	m_pMyDocument = NULL;
	m_pDocument = NULL;
}


BEGIN_MESSAGE_MAP(CFlowChartPane, CView)
	//{{AFX_MSG_MAP(CFlowChartPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FLOWCHART_ADDITEM, OnAddItem)
	ON_COMMAND(ID_FLOWCHART_EDITITEM, OnEditItem)
	ON_COMMAND(ID_FLOWCHART_DELITEM, OnDelItem)
	ON_COMMAND(ID_FLOWCHART_PROPERTY, OnProp)
	ON_COMMAND(ID_FLOWCHART_PRINT, OnPrint)
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CFlowChartPane, CView)
	ON_EVENT(CFlowChartPane, IDC_FLOWCHART, 0x1, OnBoxDeleted, VTS_DISPATCH)
	ON_EVENT(CFlowChartPane, IDC_FLOWCHART, 0x2, OnArrowDeleted, VTS_DISPATCH)
	ON_EVENT(CFlowChartPane, IDC_FLOWCHART, 8, OnArrowCreated, VTS_DISPATCH)
	ON_EVENT(CFlowChartPane, IDC_FLOWCHART, 18, OnArrowClicked, VTS_DISPATCH VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CFlowChartPane, IDC_FLOWCHART, 19, OnArrowDblClicked, VTS_DISPATCH VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CFlowChartPane, IDC_FLOWCHART, 20, OnBoxClicked, VTS_DISPATCH VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CFlowChartPane, IDC_FLOWCHART, 21, OnBoxDblClicked, VTS_DISPATCH VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CFlowChartPane, IDC_FLOWCHART, 0x3d, OnArrowOrgChanged, VTS_DISPATCH VTS_DISPATCH)
	ON_EVENT(CFlowChartPane, IDC_FLOWCHART, 0x3e, OnArrowDestChanged, VTS_DISPATCH VTS_DISPATCH)
END_EVENTSINK_MAP()
/////////////////////////////////////////////////////////////////////////////
// CFlowChartPane drawing

void CFlowChartPane::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CFlowChartPane diagnostics

#ifdef _DEBUG
void CFlowChartPane::AssertValid() const
{
	CView::AssertValid();
}

void CFlowChartPane::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFlowChartPane message handlers
// ProcessFlowView.cpp : implementation file
//


int CFlowChartPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;


	if (!m_wndFlowChart.Create(_T(""), WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_FLOWCHART))
		return -1;

	m_oleDropTarget.Register(this);

	return 0;
}


void CFlowChartPane::OnBoxClicked(LPDISPATCH pBox, long button, long x, long y)
{
	m_wndFlowChart.GetFlowChartPtr()->ClearSelection();
	m_wndFlowChart.GetFlowChartPtr()->AddToSelection(pBox);
	m_curItem = pBox;

	if(button == mbRight)
	{
		CPoint pt;
		::GetCursorPos(&pt);

		CNewMenu menu;
		if (!menu.LoadMenu(IDR_CTX_FLOWCHARTITEM))
			return;
		CNewMenu* pPopup = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		ASSERT(pPopup);
		pPopup->DeleteMenu(ID_FLOWCHART_ADDITEM, MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_FLOWCHART_PROPERTY, MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_FLOWCHART_PRINT, MF_BYCOMMAND);
		//pPopup->ModifyODMenu(_T("Edit Process"), ID_FLOWCHART_EDITITEM, NULL);
		pPopup->SetMenuText(ID_FLOWCHART_DELITEM, _T("Delete Process"), MF_BYCOMMAND);
		pPopup->SetMenuText(ID_FLOWCHART_EDITITEM, _T("Edit Process"), MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_FLOWCHART_EDITITEM, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
			pt.x, pt.y, this);
	}
}

void CFlowChartPane::OnArrowClicked(LPDISPATCH pBox, long button, long x, long y)
{
	m_wndFlowChart.GetFlowChartPtr()->ClearSelection();
	m_wndFlowChart.GetFlowChartPtr()->AddToSelection(pBox);
	m_curItem = pBox;

	if(button == mbRight)
	{
		CPoint pt;
		::GetCursorPos(&pt);

		CNewMenu menu;
		if (!menu.LoadMenu(IDR_CTX_FLOWCHARTITEM))
			return;
		CNewMenu* pPopup = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		pPopup->DeleteMenu(ID_FLOWCHART_ADDITEM, MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_FLOWCHART_PROPERTY, MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_FLOWCHART_PRINT, MF_BYCOMMAND);
		pPopup->SetMenuText(ID_FLOWCHART_DELITEM, _T("Delete Linkage"), MF_BYCOMMAND);
		pPopup->SetMenuText(ID_FLOWCHART_EDITITEM, _T("Edit Linkage"), MF_BYCOMMAND);
		pPopup->EnableMenuItem(ID_FLOWCHART_PROPERTY, MF_BYCOMMAND | MF_ENABLED);
		ASSERT(pPopup);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
			pt.x, pt.y, this);
	}
}

void CFlowChartPane::OnBoxDblClicked(LPDISPATCH box, long button, long x, long y)
{
/*
	CLSID clsid = __uuidof(PropPageActiveItem);
	LPUNKNOWN pInterface = (LPUNKNOWN)m_wndFlowChart.GetFlowChartPtr().GetInterfacePtr();
	OleCreatePropertyFrame(m_hWnd, 0, 0, L"Box item", 1, &pInterface, 1, &clsid, 
						   GetSystemDefaultLCID(), 0, NULL);
*/
}

void CFlowChartPane::OnArrowOrgChanged(LPDISPATCH arrow, LPDISPATCH obj)
{
	IArrowItemPtr pArrowItem = (IArrowItemPtr)arrow;
	IBoxItemPtr pBox = NULL;
	obj->QueryInterface(__uuidof(IBoxItem), (void**)&pBox);
	if(pBox != NULL)
	{
		CString strText = "";
		strText.Format("%s", (char*)pArrowItem->OriginBox->Text);
		strText.Format("%s", (char*)pArrowItem->DestinationBox->Text);
		strText.Format("%s", (char*)pBox->Text);

		MathLink link;	
		link.name.first = (char*)pArrowItem->OriginBox->Text;
		link.name.second = (char*)pArrowItem->DestinationBox->Text;
		link.percent = ::atoi((char*)pArrowItem->Text);
		GetCurMathFlowPtr()->AddLink(link);	
		pArrowItem->OriginBox->FillColor = m_color;

		link.name.first = (char*)pBox->Text;
		GetCurMathFlowPtr()->RemoveLink(link.name.first, link.name.second);

		SetArrowsOfBoxColor(pBox);
		//set box color
		SetBoxColor(pBox);
		SetArrowsOfBoxColor(pArrowItem->OriginBox);
		SaveFlowChartData();
	}
}

void CFlowChartPane::OnArrowDestChanged(LPDISPATCH arrow, LPDISPATCH obj)
{
	IArrowItemPtr pArrowItem = (IArrowItemPtr)arrow;
	IBoxItemPtr pBox = NULL;
	obj->QueryInterface(__uuidof(IBoxItem), (void**)&pBox);
	if(pBox != NULL)
	{
		CString strText = "";
		strText.Format("%s", (char*)pArrowItem->OriginBox->Text);
		strText.Format("%s", (char*)pArrowItem->DestinationBox->Text);
		strText.Format("%s", (char*)pBox->Text);

		MathLink link;	
		link.name.first = (char*)pArrowItem->OriginBox->Text;
		link.name.second = (char*)pArrowItem->DestinationBox->Text;
		link.percent = ::atoi((char*)pArrowItem->Text);
		GetCurMathFlowPtr()->AddLink(link);	
		pArrowItem->DestinationBox->FillColor = m_color;

		link.name.second = (char*)pBox->Text;
		GetCurMathFlowPtr()->RemoveLink(link.name.first, link.name.second);
	
		//set box color
		SetBoxColor(pBox);	
		SaveFlowChartData();
	}
}

void CFlowChartPane::OnArrowDblClicked(LPDISPATCH arrow, long button, long x, long y)
{
	IArrowItemPtr pArrow(m_wndFlowChart.GetFlowChartPtr()->GetActiveArrow());
	MathLink * pMathLink = GetCurMathFlowPtr()->GetLink((char*)pArrow->OriginBox->Text,
														(char*)pArrow->DestinationBox->Text);
	if (pMathLink == NULL)
		return;

	CDlgAttrOfLink dlg;
	dlg.m_nRate = pMathLink->percent;
	dlg.m_nDistance = pMathLink->distance;
	if (dlg.DoModal() == IDOK)
	{
		pMathLink->percent = dlg.m_nRate;
		pMathLink->distance = dlg.m_nDistance;

		CString strRate;
		strRate.Format("%d%%", dlg.m_nRate);
		pArrow->Text = _bstr_t(strRate);

		m_curItem = (LPDISPATCH)arrow;
		SaveFlowChartData();
	}
}

void CFlowChartPane::OnAddItem()
{
	
}

void CFlowChartPane::OnEditItem()
{
/*	CLSID clsid = __uuidof(PropPageActiveItem);
	LPUNKNOWN pInterface = (LPUNKNOWN)m_wndFlowChart.GetFlowChartPtr().GetInterfacePtr();
	OleCreatePropertyFrame(m_hWnd, 0, 0, L"Item", 1, &pInterface, 1, &clsid, GetSystemDefaultLCID(), 0, NULL);
	*/
	OnArrowDblClicked(m_curItem, (long)0, (long)0, (long)0);
}

void CFlowChartPane::OnBoxDeleted(LPDISPATCH pBox)
{
/*	IBoxItemPtr pBoxItem = (IBoxItemPtr)pBox;
	char* pszText = pBoxItem->Text;
	CString strText = "";
	strText.Format("%s", pszText);
	UINT nCount = pBoxItem->OutgoingArrows;
	nCount = pBoxItem->IncomingArrows;
*/
}
void CFlowChartPane::OnArrowDeleted(LPDISPATCH arrow)
{
/*	IArrowItemPtr pArrowItem = (IArrowItemPtr)arrow;
	char* pszText = pArrowItem->OriginBox->Text;
	CString strText = "";
	strText.Format("%s", pszText);
	pszText = pArrowItem->DestinationBox->Text;
	strText.Format("%s", pszText);
	*/
}

void CFlowChartPane::OnDelItem()
{
	if (!m_curItem)
		return;
	
	IBoxItemPtr pBox = NULL;
	IArrowItemPtr pArrow = NULL;
	m_curItem->QueryInterface(__uuidof(IBoxItem), (void**)&pBox);
	if(pBox != NULL)
	{
		if( (pBox->Text != _bstr_t("") ) && (GetCurMathFlowPtr() != NULL) )
		{
			GetCurMathFlowPtr()->RemoveProcess((char*)pBox->Text);	
//			GetDocumentPtr()->SetModifiedFlag(TRUE);
			GetDocumentPtr()->UpdateAllViews(NULL);
			
			IArrowsPtr pArrows = pBox->OutgoingArrows;
			LONG lCount = pBox->OutgoingArrows->count;
			LONG i = 0;
			//if the the number of incoming arrows of the destination box is one and the number of 
			//outgoing arrows of the destination box is zero, set the box color is red,
			for( i=0; i<lCount; i++)
			{
				IBoxItem* pBoxItem = pArrows->GetItem(i)->DestinationBox;
				char* pszText = pBoxItem->Text;
				if(pBoxItem->IncomingArrows->count == 1 && pBoxItem->OutgoingArrows->count == 0)
				{
					pBoxItem->FillColor = RGB(255, 0, 0);
				}
			}
			
			pArrows = pBox->IncomingArrows;
			lCount = pBox->IncomingArrows->count;
			//check the incoming arrows of the box deleted, 
			for( i=0; i<lCount; i++)
			{
				IBoxItemPtr pBoxItem = pArrows->Item[0]->OriginBox;
				char* pszText = pBoxItem->Text;
				//count the value of all the arrows of the parent box of the current box,
				std::vector<MathLink> vlink;
				GetCurMathFlowPtr()->OutLink((char*)pBoxItem->Text, vlink);
				int nCount = 0;
				long lSize = vlink.size();
				for(int ii=0; ii<lSize; ii++)
					nCount += vlink[ii].percent;
				//delete the arrow linking the current and its parent
				m_wndFlowChart.GetFlowChartPtr()->DeleteItem((LPDISPATCH)(pBox->IncomingArrows->Item[0]));
				//if the parent box has no incoming arrows and the outgoing arrows,set it color red,
				if( (pBoxItem->IncomingArrows->count == 0 && pBoxItem->OutgoingArrows->count == 0))
					pBoxItem->FillColor = RGB(255, 0, 0);
				//set the color of the out going arrows of the 
				for(long ia=0; ia<lSize; ia++)
				{
					if(nCount != 100)
						pBoxItem->OutgoingArrows->Item[ia]->Color = RGB(255, 0, 0);
					else
						pBoxItem->OutgoingArrows->Item[ia]->Color = RGB(0, 0, 0);
				}
			}
			//delete the current box
			m_wndFlowChart.GetFlowChartPtr()->DeleteItem(m_curItem);
			SaveFlowChartData();
		}
	}
	else
	{
		m_curItem->QueryInterface(__uuidof(IArrowItem), (void**)&pArrow);
		if(pArrow != NULL && GetCurMathFlowPtr() != NULL)
		{
			MathLink link;
			link.name.first = (char*)pArrow->OriginBox->Text;
			link.name.second = (char*)pArrow->DestinationBox->Text;
			GetCurMathFlowPtr()->RemoveLink(link.name.first, link.name.second);

			//get the out links of the origin box of the arrow deleted, 
			std::vector<MathLink> vlink;
			IBoxItem* pOriginBox = pArrow->OriginBox;
			GetCurMathFlowPtr()->OutLink((char*)pOriginBox->Text, vlink);
			int nCount = 0;
			long lSize = vlink.size();
			//get the count of all the value of the allows 
			long i = 0;
			for(i=0; i<lSize; i++)
				nCount += vlink[i].percent;

			IBoxItem* pDestBox = pArrow->DestinationBox;

			m_wndFlowChart.GetFlowChartPtr()->DeleteItem(m_curItem);
			
			//set the color of the out going arrows of the origin box
			IArrowsPtr pArrows = pOriginBox->OutgoingArrows;
			for(i=0; i<lSize; i++)
			{
				if(nCount != 100)
					pArrows->Item[i]->Color = RGB(255, 0, 0);
				else
					pArrows->Item[i]->Color = RGB(0, 0, 0);
			}
		
			//set box color
			SetBoxColor(pOriginBox);
			SetBoxColor(pDestBox);
			SaveFlowChartData();
		}
	}


	m_curItem = NULL;
}

void CFlowChartPane::OnProp()
{
/*	CLSID clsid = __uuidof(PropPageGeneral);
	LPUNKNOWN pInterface = (LPUNKNOWN)m_wndFlowChart.GetFlowChartPtr().GetInterfacePtr();
	OleCreatePropertyFrame(m_hWnd, 0, 0, L"Flow Chart", 1, &pInterface, 1, &clsid, 
						   GetSystemDefaultLCID(), 0, NULL);*/
	OnArrowDblClicked(m_curItem, (long)0, (long)0, (long)0);
}

void CFlowChartPane::OnPrint()
{
	IPrintOptionsPtr pPrintOption =  m_wndFlowChart.GetFlowChartPtr()->PrintOptions;
	pPrintOption->Interior = VARIANT_TRUE;
	pPrintOption->Images = VARIANT_TRUE;
	pPrintOption->Frame = VARIANT_TRUE;
	pPrintOption->Header = VARIANT_FALSE;
	pPrintOption->Shadows = VARIANT_TRUE;
	pPrintOption->Title = _bstr_t("Test print page");
	m_wndFlowChart.GetFlowChartPtr()->PreviewDiagram();
}

void CFlowChartPane::OnArrowCreated(LPDISPATCH pBox)
{
	IArrowItemPtr pArrowItem(pBox);
	

	if (pArrowItem->OriginBox == pArrowItem->DestinationBox)
	{
		m_wndFlowChart.GetFlowChartPtr()->DeleteItem(pArrowItem);
	}
	else if(GetCurMathFlowPtr() != NULL)
	{
		MathLink link;
		link.name.first = (char*)pArrowItem->OriginBox->Text;
		link.name.second = (char*)pArrowItem->DestinationBox->Text;
		
		IArrowsPtr pArrows = pArrowItem->OriginBox->OutgoingArrows;
		char pszText[10];
		std::vector<MathLink> vLink;
		GetCurMathFlowPtr()->OutLink(link.name.first, vLink);
		int iValue = 100 / (vLink.size()+1);
		for(int i=0; i<static_cast<int>(vLink.size()); i++)
		{
			vLink[i].percent = iValue;
			GetCurMathFlowPtr()->UpdateLink(vLink[i]);
			sprintf((char*)pszText, "%d%%\0", iValue);
			pArrows->Item[i]->Text = _bstr_t(pszText);
		}
		
		link.percent = 100 - iValue*vLink.size();
		
		sprintf((char*)pszText, "%d%%\0", link.percent);
		pArrowItem->Text = _bstr_t(pszText);
		GetCurMathFlowPtr()->AddLink(link);

		SetBoxColor(pArrowItem->OriginBox);
		SetBoxColor(pArrowItem->DestinationBox);
		SetArrowsOfBoxColor(pArrowItem->OriginBox);
		
		m_curItem = (LPDISPATCH)pArrowItem;

		SaveFlowChartData();
	}
}

DROPEFFECT CFlowChartPane::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	DragDropDataItem dataItem;
	if (OleDragDropManager::GetInstance().GetDragDropData(dataItem, pDataObject)
		&& dataItem.GetDataType() == DragDropDataItem::Type_TerminalShapePF)
	{
		CShape::SHAPEINFO* pShapeInfo = (CShape::SHAPEINFO*)dataItem.GetItem(0).GetData();
		m_strProcessName = (CString)pShapeInfo->name;
		if(!m_strProcessName.IsEmpty() && GetCurMathFlowPtr() && !GetCurMathFlowPtr()->GetFlowName().IsEmpty())
		{
			bool bFind = CMathFlow::HasProc( GetCurMathFlowPtr()->GetFlow(), m_strProcessName.GetBuffer(m_strProcessName.GetLength()) );
			m_strProcessName.ReleaseBuffer();
			if(!bFind)
			{
				return DROPEFFECT_COPY;
			}
		}
	}
	m_strProcessName = "";
	return DROPEFFECT_NONE;
}

DROPEFFECT CFlowChartPane::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	OnMouseMove(dwKeyState, point);
	
	if(m_strProcessName == "")
		return DROPEFFECT_NONE;
	else
		return DROPEFFECT_COPY;
}

void CFlowChartPane::OnDragLeave() 
{
	CView::OnDragLeave();

	if(m_strProcessName != "")
		m_strProcessName = "";
	
}

BOOL CFlowChartPane::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CView::OnDrop(pDataObject, dropEffect, point);

	DragDropDataItem dataItem;
	if (OleDragDropManager::GetInstance().GetDragDropData(dataItem, pDataObject)
		&& dataItem.GetDataType() == DragDropDataItem::Type_TerminalShapePF)
	{
		CShape::SHAPEINFO* pShapeInfo = (CShape::SHAPEINFO*)dataItem.GetItem(0).GetData();
		m_strProcessName = (CString)pShapeInfo->name;
		if(!m_strProcessName.IsEmpty() && GetCurMathFlowPtr() && !GetCurMathFlowPtr()->GetFlowName().IsEmpty())
		{
			bool bFind = CMathFlow::HasProc( GetCurMathFlowPtr()->GetFlow(), m_strProcessName.GetBuffer(m_strProcessName.GetLength()) );
			m_strProcessName.ReleaseBuffer();
			if(!bFind)
			{
				GetCurMathFlowPtr()->AddProcess(m_strProcessName.GetBuffer(m_strProcessName.GetLength()));
				FLOWCHARTLib::IFlowChartPtr pInterface = m_wndFlowChart.GetFlowChartPtr();
				int x, y;
				x = (point.x >= 50) ? (point.x-50) : 1;
				y = (point.y >= 50) ? (point.y-50) : 1;
				m_curItem = (LPDISPATCH) pInterface->CreateBox(x, y, 80, 40);
				pInterface->ActiveBox->Text = _bstr_t(m_strProcessName); 
				m_color = pInterface->ActiveBox->FillColor;
				pInterface->ActiveBox->FillColor = RGB(255,0,0);
			
// 				GetDocumentPtr()->SetModifiedFlag(TRUE);
				GetDocumentPtr()->UpdateAllViews(NULL);
				GetDocumentPtr()->SaveMathematicData();
				SaveFlowChartData();
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CFlowChartPane::SetMathFlowPtr(const std::vector<CMathFlow>* pFlowVect)
{
	m_pFlowVect = pFlowVect;
}

BOOL CFlowChartPane::SaveFlowChartData()
{
	ASSERT(m_strFlowChartFilePath.GetLength() > 0);
	
	try
	{
		FLOWCHARTLib::IFlowChartPtr pInterface = m_wndFlowChart.GetFlowChartPtr();
		if(pInterface == NULL)
			return FALSE;
		if(pInterface->boxes->count != 0 || pInterface->Arrows->count != 0)
		{
			m_wndFlowChart.GetFlowChartPtr()->SaveToFile(_bstr_t(m_strFlowChartFilePath), FALSE);
		}
		else
		{
			::DeleteFile(m_strFlowChartFilePath);
		}
	}
	catch (_com_error& e)
	{
		AfxMessageBox(e.Description());
		return FALSE;
	}

	CTermPlanDoc* pDoc = GetDocumentPtr();
	if (pDoc != NULL)
		pDoc->SaveMathematicData();
	
	return TRUE;
}

void CFlowChartPane::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	if (::IsWindow(m_wndFlowChart.m_hWnd))
	{
		m_wndFlowChart.MoveWindow(0, 0, cx, cy);
	}
	
}

void CFlowChartPane::SetFlowName(const CString& strName)
{
	m_strFlowName = strName;
	
	CString strPath = GetDocumentPtr()->m_ProjInfo.path;
	m_strFlowChartFilePath.Format("%s\\MatheMatic\\%s.fcs", strPath, m_strFlowName);//_T("\\TempFlowChart.fcs");
}

void CFlowChartPane::InitFlowChart()
{
	IFlowChartPtr& pFlowChart = m_wndFlowChart.GetFlowChartPtr();
	try
	{
		if (PathFileExists(m_strFlowChartFilePath))
		{
			pFlowChart->LoadFromFile(_bstr_t(m_strFlowChartFilePath));
		}
		pFlowChart->AlignToGrid = VARIANT_FALSE;
		pFlowChart->KbdActive = VARIANT_TRUE;
		pFlowChart->ShowShadows = VARIANT_TRUE;
		if (pFlowChart->Graphics->EngineType != geGdiPlus)
		{
			pFlowChart->Graphics->StartUp(geGdiPlus);
			pFlowChart->Graphics->AntiAliasing = VARIANT_TRUE;
		}
		
		pFlowChart->Behavior = bhCreateArrow;
	}
	catch (_com_error& e)
	{
		AfxMessageBox(e.Description());
	}
}
CMathFlow* CFlowChartPane::GetCurMathFlowPtr()
{
	ASSERT(m_pFlowVect != NULL);

	for( int i = 0; i<static_cast<int>(m_pFlowVect->size()); i++)			
	{
		if(strnicmp((LPCTSTR)(*m_pFlowVect)[i].GetFlowName(), (LPCTSTR)m_strFlowName, m_strFlowName.GetLength()) == 0)
		{
			return (CMathFlow*)&((*m_pFlowVect)[i]);		
		}
	}

	return NULL;
}
void CFlowChartPane::OnClose()
{
	CView::OnClose();
}
void CFlowChartPane::OnDestroy() 
{
	// TODO: Add your message handler code here
	CView::OnDestroy();

	m_oleDropTarget.Revoke();

	BOOL bFind = FALSE;
	CDocument* pDoc = GetDocumentPtr();
	POSITION pos = pDoc->GetFirstViewPosition();
	CView * pView = NULL;
	while(pos)
	{
		pView = pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CProcessFlowView)))
		{
			char* pszText = m_strFlowName.GetBuffer(m_strFlowName.GetLength());
			pView->SendMessage(WM_USER+100, (WPARAM)pszText, 0);
			m_strFlowName.ReleaseBuffer();
			bFind = TRUE;
		}
	}


	SaveFlowChartData();
}

BOOL CFlowChartPane::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CView::DestroyWindow();
}

void CFlowChartPane::OnCancelMode() 
{
	CView::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CFlowChartPane::SetBoxColor(LPDISPATCH pBox)
{
	IBoxItemPtr pBoxPtr = (IBoxItemPtr)pBox;
	if(pBoxPtr->OutgoingArrows->count == 0 
		&& pBoxPtr->IncomingArrows->count == 0)
	{
		pBoxPtr->FillColor = RGB(255, 0, 0);
	}
	else
	{
		pBoxPtr->FillColor = m_color;
	}
}

void CFlowChartPane::SetArrowsOfBoxColor(LPDISPATCH pBox)
{
	IBoxItemPtr pBoxPtr = (IBoxItemPtr)pBox;
	//get the out links of the origin box of the arrow deleted, 
	std::vector<MathLink> vlink;
	GetCurMathFlowPtr()->OutLink((char*)pBoxPtr->Text, vlink);
	int nCount = 0;
	long lSize = vlink.size();
	//get the count of all the value of the allows 
	long i = 0;
	for(i=0; i<lSize; i++)
		nCount += vlink[i].percent;

	//set the color of the out going arrows of the origin box
	IArrowsPtr pArrows = pBoxPtr->OutgoingArrows;
	for(i=0; i<lSize; i++)
	{
		if(nCount != 100)
			pArrows->Item[i]->Color = RGB(255, 0, 0);
		else
			pArrows->Item[i]->Color = RGB(0, 0, 0);
	}
}


BOOL CFlowChartPane::DeleteProcess(const CString &strName)
{
	IFlowChartPtr& pFlowChart = m_wndFlowChart.GetFlowChartPtr();

	IBoxesPtr pBoxes = pFlowChart->boxes;

	UINT nCount = pBoxes->count;
	
	IBoxItemPtr pBoxItem = NULL;

	for(int i=0; i<static_cast<int>(nCount); i++)
	{
		pBoxItem = pBoxes->Item[i];
		if( strcmp( strName, pBoxItem->Text ) == 0  )
		{
			m_curItem = (LPDISPATCH)pBoxItem;
			OnDelItem();
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFlowChartPane::UpdateProcess(const CString &strOld, const CString &strNew)
{
	IFlowChartPtr& pFlowChart = m_wndFlowChart.GetFlowChartPtr();

	IBoxesPtr pBoxes = pFlowChart->boxes;

	UINT nCount = pBoxes->count;
	
	IBoxItemPtr pBoxItem = NULL;

	for(int i=0; i<static_cast<int>(nCount); i++)
	{
		pBoxItem = pBoxes->Item[i];
		if( strcmp( pBoxItem->Text, strOld ) == 0 )
		{
			pBoxItem->Text = (_bstr_t)(LPCTSTR)strNew;
			GetCurMathFlowPtr()->RenameProcess((LPCTSTR)strOld, (LPCTSTR)strNew);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFlowChartPane::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_DELETE && pMsg->hwnd == m_wndFlowChart.m_hWnd)
		{
			OnDelItem();
		}
	}

	return CView::PreTranslateMessage(pMsg);
}
