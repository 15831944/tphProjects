// ToolBar24X24.cpp: implementation of the CToolBar24X24 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "ToolBar24X24.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolBar24X24::CToolBar24X24()
{ 
	m_nCountNotButton = 0; 
}

CToolBar24X24::~CToolBar24X24()
{
	int nSize=m_titleList.GetSize();
	for(int i=0;i<nSize;i++)
	{
		delete m_titleList[i];
	}
	m_titleList.RemoveAll();
}

BEGIN_MESSAGE_MAP(CToolBar24X24, CToolBar)
	//{{AFX_MSG_MAP(CToolBar24X24)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CToolBar24X24::Create(CWnd *pParent, UINT nID, DWORD dwStyleAppend)
{
	return CToolBar::CreateEx(pParent,TBSTYLE_FLAT, CBRS_SIZE_DYNAMIC| CBRS_TOOLTIPS | CBRS_GRIPPER | CBRS_BORDER_3D|WS_VISIBLE|WS_CHILD|CBRS_TOP|dwStyleAppend,CRect(0,0,0,0),nID);
}

void CToolBar24X24::AddButtons(int nBtnCount, TBBUTTON *pTBB,int cx)
{
	GetToolBarCtrl().AddButtons(nBtnCount, pTBB); 
	if(cx!=-1)
	{
		TBBUTTONINFO tbbi;
		tbbi.cbSize=sizeof(TBBUTTONINFO);
		tbbi.dwMask=TBIF_SIZE ;
		tbbi.cx =cx; 
		for(int i=0;i<nBtnCount;i++)
			GetToolBarCtrl().SetButtonInfo(pTBB[i].idCommand,&tbbi) ;
	}

}

void CToolBar24X24::AddButton(TBBUTTON *pTBB,int cx,BOOL bTempButton)
{
	AddButtons(1, pTBB,cx);
	
	if(bTempButton)
		m_nCountNotButton++;
}

void CToolBar24X24::AddButtonSep()
{
	TBBUTTON ButtonInfo;
	memset(&ButtonInfo, 0, sizeof(TBBUTTON));
	ButtonInfo.iBitmap = 0; 
	ButtonInfo.idCommand = -1;
	ButtonInfo.fsState = TBSTATE_ENABLED;
	ButtonInfo.fsStyle = TBSTYLE_SEP ;
	ButtonInfo.dwData = NULL;
	ButtonInfo.iString = NULL;
	AddButton(&ButtonInfo,1,FALSE); 
}

void CToolBar24X24::InitImageList(UINT nIDImageNormal, UINT nIDImageHot, UINT nIDImageDisable, COLORREF clrMask)
{
	CBitmap bmLNormal;
	m_ImageListNormal.Create(24,24,ILC_COLOR24 | ILC_MASK,0,1);
	bmLNormal.LoadBitmap(nIDImageNormal);
	m_ImageListNormal.Add(&bmLNormal, clrMask);

	CBitmap bmLHot;
	m_ImageListHot.Create(24,24,ILC_COLOR24 | ILC_MASK,0,1);
	bmLHot.LoadBitmap(nIDImageHot);
	m_ImageListHot.Add(&bmLHot, clrMask);

	CBitmap bmLDisable;
	m_ImageListDisable.Create(24,24,ILC_COLOR24 | ILC_MASK,0,1);
	bmLDisable.LoadBitmap(nIDImageDisable);
	m_ImageListDisable.Add(&bmLDisable, clrMask);
}

int CToolBar24X24::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetSizes(CSize(32,32), CSize(24,24));
	InitFont(80,"MS Sans Serif");
	EnableDocking(CBRS_ALIGN_ANY );
	return 0;
}

void CToolBar24X24::SetImageList()
{
	GetToolBarCtrl().SetImageList(&m_ImageListNormal);
	GetToolBarCtrl().SetHotImageList(&m_ImageListHot);
	GetToolBarCtrl().SetDisabledImageList(&m_ImageListDisable);

}

void CToolBar24X24::InitFont(int nPointSize, LPCTSTR lpszFaceName)
{
    m_Font.DeleteObject(); 
    m_Font.CreatePointFont (nPointSize,lpszFaceName);
}

void CToolBar24X24::AddComboBox(CComboBox* pComboBox, CSize size, UINT nID,DWORD dwStyleAppend)
{
	AddButtonAndResize(nID,size.cx);
	
	CRect rect,rectLast;
	int nIndex=CommandToIndex(TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton);
	GetItemRect(nIndex, &rect);
	rect.top = 6 ;
	rect.bottom =rect.top +size.cy ;
	pComboBox->Create(WS_CHILD | WS_VISIBLE |dwStyleAppend, rect, this, nID);
	pComboBox->SetFont(&m_Font);

	m_nCountNotButton++; 
}

void CToolBar24X24::AddTextComboBox(CString strTitle, CComboBox *pComboBox, CSize sizeEx, UINT nID, DWORD dwStyleAppend)
{
	AddButtonAndResize(nID,sizeEx.cx);	
	
	CRect rect,rectLast;
	int nIndex=CommandToIndex(TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton); 
	GetItemRect(nIndex, &rect);
	
	CStatic* pStatic=new CStatic;
	CSize size=GetTextSize(strTitle);
	
	CRect rectStatic=rect;
	CRect rectComboBox=rect;
	rectStatic.right = rectStatic.left + size.cx ;
	rectStatic.top =9;
	rectStatic.bottom =rectStatic.top+size.cy;

	rectComboBox.left = rectStatic.right;
	rectComboBox.right = rectComboBox.left + sizeEx.cx - rectStatic.Width();
	rectComboBox.top = 6;
	rectComboBox.bottom =rectStatic.top +size.cy*10; 

	pStatic->Create(strTitle,WS_CHILD|WS_VISIBLE,rectStatic,this) ;
	pStatic->SetFont(&m_Font);
	m_titleList.Add(pStatic);
	
	pComboBox->Create(WS_CHILD | WS_VISIBLE |dwStyleAppend, rectComboBox, this, nID);
	pComboBox->SetFont(&m_Font);

	m_nCountNotButton++;
}

CSize CToolBar24X24::GetTextSize(CString str)
{
	CWindowDC dc(NULL);
	CFont* pOldFont=dc.SelectObject(&m_Font)  ;
	CSize size=dc.GetTextExtent(str) ;
	dc.SelectObject(pOldFont) ;
	return size;
}

void CToolBar24X24::InitTBBUTTON(TBBUTTON& tbb)
{
	memset(&tbb, 0, sizeof(TBBUTTON));
	tbb.iBitmap = 0;
	tbb.idCommand = 0;

	//this may cause a problem ( getting button behind nonstandard  button on toolbar,
	// when cursor move to this button ,it prepare to push down ),
	//so need set this state to gray or 0;
	tbb.fsState = TBSTATE_ENABLED;

	tbb.fsStyle = TBSTYLE_BUTTON;
	tbb.dwData = NULL;
	tbb.iString = NULL;
}

void CToolBar24X24::AddButtonAndResize(UINT nID,int cx)
{
		TBBUTTON tbb;
		CToolBar24X24::InitTBBUTTON(tbb) ;
		tbb.iBitmap =-1;
		tbb.idCommand =TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton;
		AddButton(&tbb,cx);
		
		TBBUTTONINFO tbbi;
		
		tbbi.cbSize=sizeof(TBBUTTONINFO);
		tbbi.dwMask=TBIF_SIZE ;
		tbbi.cx =cx; 
		
		//int nIndex =GetToolBarCtrl().CommandToIndex(nID);
		//SetButtonInfo(nIndex, nID, TBBS_SEPARATOR, cx);
		///*
		GetToolBarCtrl().SetButtonInfo(TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton,&tbbi) ;
		//*/
		
}

void CToolBar24X24::AppendImageList(UINT nIDImageNormal, UINT nIDImageHot, UINT nIDImageDisable, COLORREF clrMask)
{
	CBitmap bmLNormal;
	bmLNormal.LoadBitmap(nIDImageNormal);
	m_ImageListNormal.Add(&bmLNormal, clrMask);
	bmLNormal.DeleteObject();

	CBitmap bmLHot;
	bmLHot.LoadBitmap(nIDImageHot);
	m_ImageListHot.Add(&bmLHot, clrMask);
	bmLHot.DeleteObject();

	CBitmap bmLDisable;
	bmLDisable.LoadBitmap(nIDImageDisable);
	m_ImageListDisable.Add(&bmLDisable, clrMask);
	bmLDisable.DeleteObject();
}


