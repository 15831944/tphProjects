// IconComboBox.cpp: implementation of the CIconComboBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IconComboBox.h"
#include "resource.h"
#include "common\WinMsg.h"
#include "Dib.h"
#include "Common\ProjectManager.h"
#include "ShapesManager.h"
#include "INI.h"
#include "common\WinMsg.h"
#include "Common\ProjectManager.h"
#include ".\iconcombobox.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const char* CIconComboBox::s_szShapeLabel[] = 
{
	"Default"					,		// 0
	"Flat Square"				,		// 1
	"Table"						,		// 2
	"Cube"						,		// 3
	"Cylinder"					,		// 4
	"Security Arch"				,		// 5
	"CTX 5500"					,		// 6
	"Table 3'x6'"				,		// 7
	"Security Guard"			,		// 8
	"Police Officer"			,		// 9
	"Woman Sitting"				,		// 10
	"Ticket Counter 1"			,		// 11
	"Ticket Counter 2"			,		// 12
	"Ticket Counter 3"			,		// 13
	"Ticket Counter 4"			,		// 14
	"4 seats"					,		// 15
	"Cylinder 2"				,		// 16
	"Small Disk"				,		// 17
	"Large Disk"				,		// 18
	"BillBoard"					,		// 19
	"AT"						,		// 20
	"Cobra"						,		// 21
	"Cobra Exit Belt"			,		// 22
	"Streamline"				,		// 23
	"TRX"						,		// 24
	"Cobra Exit Belt Mirrored"	,		// 25
	"Cobra Mirrored"			,		// 26
	"Body Scanner"				,		// 27
};

const char* CIconComboBox::s_szShapePath[] = 
{
	"square24"					,		// 0
	"square100"					,		// 1
	"table_of"					,		// 2
	"CUBE100"					,		// 3
	"CYL"						,		// 4
	"security_arch"				,		// 5
	"CTX5500"					,		// 6
	"table3by6"					,		// 7
	"SecurityGuard"				,		// 8
	"copeye"					,		// 9
	"readingscene"				,		// 10
	"3dTickCounter_1"			,		// 11
	"3dTickCounter_2"			,		// 12
	"3dTickCounter_3"			,		// 13
	"3dTickCounter_4"			,		// 14
	"airportseats"				,		// 15
	"CYL2"						,		// 16
	"disk24"					,		// 17
	"disk100"					,		// 18
	"billboard"					,		// 19
	"AT"						,		// 20
	"Cobra"						,		// 21
	"Cobra_Exit_Belt"			,		// 22
	"Streamline"				,		// 23
	"TRX"						,		// 24
	"Cobra_Exit_Belt_Mirrored"	,		// 25
	"Cobra_Mirrored"			,		// 26
	"BodyScanner"				,		// 27
};

CIconComboBox::CIconComboBox()
{
	m_nIndex = -1;
}

CIconComboBox::~CIconComboBox()
{

}


BEGIN_MESSAGE_MAP(CIconComboBox, CComboBox)
	ON_MESSAGE(WM_INPLACE_COMBO, OnChangeIcon)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CIconComboBox, CComboBox)

void CIconComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpmis)
{
	lpmis->itemHeight = 80;
}

void CIconComboBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS->CtlType == ODT_COMBOBOX);	
	
	//Create A CDC from the SDK struct passed in
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	
	//If the item is selected, draw the selection background
	if ((lpDIS->itemState & ODS_SELECTED) &&                         
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected - draw selection rectangle
		COLORREF crHilite = GetSysColor(COLOR_HIGHLIGHT);
		CBrush br(crHilite);
		pDC->FillRect(&lpDIS->rcItem, &br);
	}
	
	//If the item is not selected, draw a white background
	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// Item has been de-selected -- remove selection rectangle
		CBrush br(RGB(255, 255, 255));
		pDC->FillRect(&lpDIS->rcItem, &br);
	}
	
	//Draw the icon
	if (m_nIndex > -1)
	{
		CDC dcMem;
		CRect rc(lpDIS->rcItem);
		dcMem.CreateCompatibleDC(pDC);
		
		if (m_bitmaps[m_nIndex].GetSafeHandle())
		{
			HBITMAP hOldBitmap = (HBITMAP)dcMem.SelectObject(&m_bitmaps[m_nIndex]);
			CRect rcBitmap(rc.Width() / 2 - 16, (rc.Height() - 25) / 2 - 16, rc.Width() / 2 + 16,
				(rc.Height() - 25) / 2 + 16);
			::TransparentBlt(pDC->m_hDC, rcBitmap.left, rcBitmap.top, 
				rcBitmap.Width(), rcBitmap.Height(), 
				dcMem.m_hDC, 0, 0, 32, 32, RGB(255, 0, 255));
			//dcMem.DrawText(s_szShapeLabel[m_nIndex], rc, DT_CENTER | DT_BOTTOM);
			//m_imgList.Draw(pDC, m_nIndex, CPoint(0, 0), ILD_NORMAL);
			
			//pDC->StretchBlt(0, 0, rc.Width() + 10, rc.Height() - 25, &dcMem, 0, 0, 32, 32, SRCCOPY);

			//dcMem.SelectObject(hOldBitmap);
	
			int nOldMode = pDC->SetBkMode(TRANSPARENT);
			rc.top = rc.bottom - 25;
			pDC->DrawText(s_szShapeLabel[m_nIndex], rc, DT_CENTER | DT_BOTTOM | DT_WORDBREAK);
			pDC->SetBkMode(nOldMode);
		}
	}
}

void CIconComboBox::PreSubclassWindow()
{
	//Let the parent do its thing
	CComboBox::PreSubclassWindow();
	
	//combo box must manage the strings
	ASSERT(GetWindowLong(m_hWnd, GWL_STYLE) & CBS_HASSTRINGS);
	
	//combo box must be owner draw variable
	ASSERT(GetWindowLong(m_hWnd, GWL_STYLE) & CBS_OWNERDRAWVARIABLE);
	
	//Set the Height of the combo box to just contain one small icon
	::SendMessage(m_hWnd, CB_SETITEMHEIGHT, (WPARAM)-1, 60L);

	LoadAllBitmaps();
}

int CIconComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CIconComboBox::DisplayListWnd()
{
	if(!m_listWnd.GetSafeHwnd() ) 
	{
		m_listWnd.SetImageDir(CString("\\Databases\\Shapes"));
		m_listWnd.SetLabelString(s_szShapeLabel);
		if (!CreateListWnd())
			return;
		m_listWnd.ShouldHideParent(FALSE);
	}
	
	CRect rect(0,0,200,200);
	CalculateDroppedRect (&rect);
	m_listWnd.SetWindowPos (&wndNoTopMost, rect.left, rect.top,
		LISTWND_WIDTH_OLD,LISTWND_HEIGHT_OLD, SWP_SHOWWINDOW );
	m_listWnd.m_listCtrlType.SetFocus();
}

void CIconComboBox::HideCtrl()
{
	//ShowWindow(SW_HIDE);
}

void CIconComboBox::CalculateDroppedRect(LPRECT lpDroppedRect)
{
	_ASSERTE (lpDroppedRect);
	
	if (!lpDroppedRect)
	{
		return;
	}
	
	CRect rectCombo;
	GetWindowRect(&rectCombo);
	
	//adjust to either the top or bottom
	int DropTop = rectCombo.bottom;
	int ScreenHeight = GetSystemMetrics (SM_CYSCREEN);
	if ((DropTop + 200) > ScreenHeight)
	{
		DropTop = rectCombo.top - 200;
	}
	
	//adjust to either the right or left
	int DropLeft = rectCombo.left;
	int ScreenWidth = GetSystemMetrics (SM_CXSCREEN);
//	if ((DropLeft + 200) > ScreenWidth)
	//	{
	//		DropLeft = rectCombo.right - 200;
	//	}
	

	if ((DropLeft + 450) > ScreenWidth)
	{
		DropLeft = rectCombo.right - 450;
	}
	
	lpDroppedRect->left  = DropLeft;
	lpDroppedRect->top   = DropTop;
	lpDroppedRect->bottom = DropTop +400;
	lpDroppedRect->right  = DropLeft + 350;
}

BOOL CIconComboBox::CreateListWnd()
{
	CRect listRect (0,0, 200, 200);
	
	//can't have a control Id with WS_POPUP style
	LPCTSTR className=AfxRegisterWndClass(CS_DBLCLKS, 
		AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(LTGRAY_BRUSH), 0);
    if (! m_listWnd.CreateEx ( 0,
		className, NULL,
		WS_POPUP | WS_BORDER  ,
		listRect, this, 0, NULL))
	{
		return FALSE;
	}
	m_listWnd.m_pParent=this;
	m_listWnd.SetFocus();
	
	
	return TRUE;
}


LRESULT CIconComboBox::OnChangeIcon(WPARAM wParam, LPARAM lParam)
{
	m_nIndex = lParam;
	//Invalidate();
	return 0;
}

BOOL CIconComboBox::LoadAllBitmaps()
{
	CBitmap*    pImage = NULL;	
	HBITMAP     hBitmap = NULL;
	HPALETTE    hPal = NULL;
	HDC			hMemDC = NULL;	
	HGDIOBJ		hOldObj = NULL;
	
	CFile		ImgFile;
	CDib		dib;
	CString		strPath;
	//int			nWidth, nHeight;

	//m_imgList.Create(32, 32, ILC_COLOR24|ILC_MASK, 0, 4);

	/*
	CShape::CShapeList* pSL = SHAPESMANAGER->GetShapeList();

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
		//m_wndOutBarCtrl.AddFolder(strLeft, nCount);
		strRight=IniReader.GetDataRight(str);
		int nInt[128];
		int nIntCount=CIniReader::DispartStrToSomeInt(strRight,nInt);
		for(int i=0;i<nIntCount;i++)
		{
			HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,pSL->at(nInt[i])->ImageFileName() , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			m_bitmaps[nCount++].Attach(hBitmap);
		}
	}*/	
	
	
	// draw the thumbnails
	for(int i = 0; i < sizeof(s_szShapePath) / sizeof(s_szShapePath[0]); i++)
	{		
		// load the bitmap
		strPath = PROJMANAGER->GetAppPath() +
			_T("\\Databases\\Shapes\\") + s_szShapePath[i] + _T(".bmp");
		/*ImgFile.Open(strPath, CFile::modeRead);

		
		dib.Read(ImgFile);
		ImgFile.Close();
		
		// borrow our dib header to create our thumbnail bitmap
		nWidth = dib.m_pBMI->bmiHeader.biWidth;
		nHeight = dib.m_pBMI->bmiHeader.biHeight;
		dib.m_pBMI->bmiHeader.biWidth = THUMBNAIL_WIDTH;
		dib.m_pBMI->bmiHeader.biHeight = THUMBNAIL_HEIGHT;
		
		// create thumbnail bitmap section
		hBitmap = ::CreateDIBSection(NULL, 
			dib.m_pBMI, 
			DIB_RGB_COLORS, 
			NULL, 
			NULL, 
			0); 
		
		// restore dib header
		dib.m_pBMI->bmiHeader.biWidth = nWidth;
		dib.m_pBMI->bmiHeader.biHeight = nHeight;
		
		// select thumbnail bitmap into screen dc
		hMemDC = ::CreateCompatibleDC(NULL);	
		hOldObj = ::SelectObject(hMemDC, hBitmap);
		
		// grayscale image, need palette
		if(dib.m_pPalette != NULL)
		{
			hPal = ::SelectPalette(hMemDC, (HPALETTE)dib.m_pPalette->GetSafeHandle(), FALSE);
			::RealizePalette(hMemDC);
		}
		
		// set stretch mode
		::SetStretchBltMode(hMemDC, COLORONCOLOR);
		
		// populate the thumbnail bitmap bits
		
		::StretchDIBits(hMemDC, 0, 0, 
			THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, 
			0, 0, 
			dib.m_pBMI->bmiHeader.biWidth, 
			dib.m_pBMI->bmiHeader.biHeight, 
			dib.m_pBits, 
			dib.m_pBMI, 
			DIB_RGB_COLORS, 
			SRCCOPY);
		
		// restore DC object
		::SelectObject(hMemDC, hOldObj);
		
		// restore DC palette
		if(dib.m_pPalette != NULL)
			::SelectPalette(hMemDC, (HPALETTE)hPal, FALSE);
		
		// clean up
		::DeleteObject(hMemDC);
		
		// attach the thumbnail bitmap handle to an CBitmap object
		//m_bitmaps[i].Attach(hBitmap);*/

		hBitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(), strPath, IMAGE_BITMAP, 0, 0, 
			LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);

		m_bitmaps[i].Attach(hBitmap);
		/*
		CBitmap bm;
		bm.Attach(hBitmap);
		m_imgList.Add(&bm, RGB(255, 0, 255));*/
	}

	return TRUE;
}


void CIconComboBox::SetSelectedIndex(int index)
{
	m_nIndex = index;
}

void CIconComboBox::SetSelectedIndex(const char* pszName)
{
	int nIndex = -1;
	for (int i = 0; i < sizeof(s_szShapeLabel) / sizeof(s_szShapeLabel[0]); i++)
	{
		if (strcmp(s_szShapeLabel[i], pszName) == 0)
		{
			nIndex = i;
			break;
		}
	}

	if (nIndex != -1)
	{
		m_nIndex = nIndex;
	}
}

int CIconComboBox::GetSelectedIndex(void)
{
	return m_nIndex;
}
