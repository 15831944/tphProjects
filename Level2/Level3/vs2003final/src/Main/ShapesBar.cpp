// ShapesBar.cpp: implementation of the CShapesBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "MainFrm.h"
#include "ShapesBar.h"
#include "ShapesManager.h"
#include "common\WinMsg.h"
#include "Common\ProjectManager.h"
#include "Ini.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CShapesBar, CSizingControlBarG)
	//{{AFX_MSG_MAP(CShapesBar)
	ON_WM_CREATE()
	ON_MESSAGE(WM_SLB_SELCHANGED, OnSLBSelChanged)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapesBar message handlers

int CShapesBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);

	CShape::CShapeList* pSL = SHAPESMANAGER->GetShapeList();
	ASSERT(pSL != NULL);
	CreateOutlookBar(pSL);
	
	

	m_brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

    // Create an 8-point MS Sans Serif font for the list box.
    m_font.CreatePointFont (80, _T ("MS Sans Serif"));

	return 0;
}

LRESULT CShapesBar::OnSLBSelChanged(WPARAM wParam, LPARAM lParam)
{
	// TRACE("CShapesBar::OnSLBSelChanged()\n");
	//ASSERT(GetParent()->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
	CWnd* pWnd = GetParent();
	while(pWnd != NULL && !pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
		pWnd = pWnd->GetParent();
	}
	if(pWnd == NULL) {
		TRACE0("Error: Could not get MainFrame to send message!\n");
		return 0;
	}
	pWnd->SendMessage(WM_SLB_SELCHANGED, wParam, lParam);
	return 0;
}

HBRUSH CShapesBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CSizingControlBarG::OnCtlColor(pDC, pWnd, nCtlColor);
	return m_brush;
}

void CShapesBar::CreateOutlookBar(CShape::CShapeList* pSL)
{
	DWORD dwf =CGfxOutBarCtrl::fAnimation;

	m_wndOutBarCtrl.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this,IDC_LIST_SHAPES, dwf);
	
	m_largeIL.Create(32, 32, ILC_COLOR24|ILC_MASK, 0, 4);
	m_smallIL.Create(16,16, ILC_COLOR24|ILC_MASK,0,4);

	m_wndOutBarCtrl.SetImageList(&m_largeIL, CGfxOutBarCtrl::fLargeIcon);
	m_wndOutBarCtrl.SetImageList(&m_smallIL, CGfxOutBarCtrl::fSmallIcon);
	
	m_wndOutBarCtrl.SetAnimationTickCount(6);

	CString strIniDir=PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\";
	CIniReader IniReader(strIniDir+"Shapes.INI");
	CStringList* pStrL = IniReader.getSectionData("Sections");

	POSITION position;
	CString strLeft,strRight,str;
	int nCount=0;
	for(position = pStrL->GetHeadPosition(); position != NULL; ) 
	{
		str=pStrL->GetNext(position);
		strLeft=IniReader.GetDataLeft(str) ;
		m_wndOutBarCtrl.AddFolder(strLeft, nCount);
		strRight=IniReader.GetDataRight(str);
		int nInt[128];
		int nIntCount=CIniReader::DispartStrToSomeInt(strRight,nInt);
			for(int i=0;i<nIntCount;i++)
			{
				HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,pSL->at(nInt[i])->ImageFileName() , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				CBitmap bm;
				bm.Attach(hBitmap);
				int nR = m_largeIL.Add(&bm,RGB(255,0,255));
				bm.DeleteObject();
				hBitmap = (HBITMAP) ::LoadImage(NULL,pSL->at(nInt[i])->ImageFileName() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				bm.Attach(hBitmap);
				m_smallIL.Add(&bm,RGB(255,0,255));
				bm.DeleteObject();
				m_wndOutBarCtrl.InsertItem(nCount, -1, pSL->at(nInt[i])->Name(),nR, (DWORD)pSL->at(nInt[i]));
			}

			nCount++;
	}	
	m_wndOutBarCtrl.SetSelFolder(0);
}
