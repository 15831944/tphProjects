// FrameCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
//#include "Frame.h"
#include "FrameCtrl.h"
#include "XPStyle\MemDC.h"
#include "XPStyle\NewMenu.h"
#include "common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl
CFrameCtrl::CFrameCtrl()
{
	m_lNextID=0;
	m_bPopupingMenu=FALSE;
	m_bSelectMenu=FALSE;
	m_pKeyFrameHot=NULL;
	m_bMouseOver=FALSE;
	m_ToolTip.Create(this);
	m_bOverlapable=FALSE;
	m_bSnap=FALSE;
	m_nSnapValue=5;
	m_pKeyFrameHot=NULL;
	m_pWndScrollBar=NULL;
	m_bInitControl=FALSE;
	m_bDrawRectModified=TRUE;
	m_nScale=100;
	m_bDragging=FALSE;
	m_bRulerModeIsOnlySecond=FALSE;
	m_arrayKeyFrame.Add(&m_curTimeFrame);
	ASSERT(SetTimeRange(0,60*10,FALSE));
	m_curTimeFrame.SetTime(m_lTimeBegin,this);

	m_bAutoGoto = FALSE;

	m_pMovie=NULL;

}

CFrameCtrl::~CFrameCtrl()
{
	m_pKeyFrameHot=NULL;
	delete m_pWndScrollBar;
	int nCount=m_arrayKeyFrame.GetSize();
	//First is CurTimeFrame
	for(int i=1;i<nCount;i++)
	{
		delete m_arrayKeyFrame[i];
	}
	m_arrayKeyFrame.RemoveAll();
	m_arrayKeyFrameSeled.RemoveAll();
}


BEGIN_MESSAGE_MAP(CFrameCtrl, CStatic)
	//{{AFX_MSG_MAP(CFrameCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_KEYFRAME_ADD,OnKeyFrameAdd)
	ON_COMMAND(ID_KEYFRAME_DEL,OnKeyFrameDel)
	ON_COMMAND(ID_KEYFRAME_GOTO,OnKeyFrameGoto)
	ON_COMMAND(ID_VIEW_ZOOMIN,OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT,OnViewZoomOut)
	ON_COMMAND(ID_RULER_MS,OnRulerMS)
	ON_COMMAND(ID_RULER_S,OnRulerS)
	ON_COMMAND(ID_SNAP,OnSnap)
	ON_COMMAND(ID_SNAP_SELECT,OnSnapSelect)
	ON_MESSAGE(UM_SELCHAGNE_SCALE_COMBOBOX,OnSelChangeScale)
	ON_COMMAND_RANGE(ID_SNAP_2,ID_SNAP_120,OnSnapRange)
	ON_MESSAGE (WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE (WM_MOUSEHOVER, OnMouseHover)
	ON_COMMAND_RANGE(0,255,OnKeyFrameMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl message handlers

void CFrameCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//-----------------------------------------------------------------------
	m_lMouseDownPos=point.x;
	m_bPopupingMenu=FALSE;
	
	OnMouseLeave(NULL,NULL);	
	Client2Doc(m_lMouseDownPos);
	KeyFrame* pKeyFrame=HitTest(point,TRUE,TRUE,!(nFlags&MK_CONTROL));
	if(pKeyFrame)
	{
		CNewMenu menu;
		menu.CreatePopupMenu();
		long lTime1=LogicPos2Time(m_lMouseDownPos-2);
		long lTime2=LogicPos2Time(m_lMouseDownPos+2);
		int lMenuID=1;
		m_arrayKeyFrameMenu.RemoveAll();
		for(long i=1;i<m_arrayKeyFrame.GetSize();i++)
		{
			long lTime=m_arrayKeyFrame[i]->GetTime();
			if(lTime>lTime1&&lTime<lTime2)
			{
				CString strName;
				int nID=m_arrayKeyFrame[i]->GetID();
				strName.Format("Key Frame: %d  [%s]",nID,Time2Str(lTime));
				menu.AppendMenu(MF_STRING,lMenuID,strName);
				m_arrayKeyFrameMenu.Add(m_arrayKeyFrame[i]);
				lMenuID++;
			}
		}
		if(menu.GetMenuItemCount()>1)
		{
			m_bPopupingMenu=TRUE;
			CPoint pt=point;
			ClientToScreen(&pt);
			menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, this);
			m_bPopupingMenu=FALSE;
		}
		else
		{
			menu.DestroyMenu();
			m_bDragging=TRUE;
			SetCapture();
		}
	}
	else
	{
		long lDoc=point.x;
		Client2Doc(lDoc);
		long lTime=LogicPos2Time(lDoc);
		SetCurrentTime(lTime);
		SelectKeyFrame(NULL);
	}

	
	CStatic::OnLButtonDown(nFlags, point);
}

void CFrameCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect	actual ;
	GetClientRect(&actual) ;
	CMemDC	mdc(&dc, actual) ;
	mdc.SaveDC();
	Draw(&mdc);
	mdc.RestoreDC(-1);
}

void CFrameCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    if (!m_bMouseOver)
	{
        m_bMouseOver = TRUE;
		
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof (tme);
        tme.dwFlags = TME_HOVER | TME_LEAVE;
        tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = 0;
        _TrackMouseEvent (&tme);
	}
	if(m_bDragging&&m_arrayKeyFrameSeled.GetSize()>0)
	{
		long lCurPos=point.x;
		Client2Doc(lCurPos);
		long lOffsetX=lCurPos-m_lMouseDownPos;
		// TRACE("lOffsetX=%d\n",lOffsetX);
		long lOffsetPos;//Residual Value
		long lOffsetTime=LogicPos2Time(lOffsetX,&lOffsetPos)-m_lTimeBegin;
		//*********************>=1 second************************
		if(abs(lOffsetTime)>=(m_bSnap?m_nSnapValue:1))
		{
			long lTime=m_arrayKeyFrameSeled[0]->GetTime()+lOffsetTime;
			//Check Overlapable
			if(!(!m_bOverlapable&&IsRepeat(lTime)))
			{
				m_arrayKeyFrameSeled[0]->SetTime(lTime,this);
				Refresh();
				if(m_arrayKeyFrameSeled[0]->GetID() == 0) //is the current time "keyframe"
					GetParent()->SendMessage(FCM_NOTIFYSELECT_TIME, (WPARAM) lTime, (LPARAM) m_bAutoGoto);
				else
					GetParent()->SendMessage(FCM_NOTIFYMOVE_KF, (WPARAM) lTime-lOffsetTime, (LPARAM) lTime);
				m_lMouseDownPos=lCurPos-lOffsetPos;//Sub Residual Value
			}
		}
		//Scroll Window
		int nPosScroll=m_pWndScrollBar->GetScrollPos();
		long nOffsetScroll=70;
		long lTimeOffset=m_lLogicWidth/100;
		if(lOffsetX>0)
		{
			m_bMovingLeft=TRUE;
			if(point.x+nOffsetScroll>m_rectFrame.right)
				m_pWndScrollBar->SetScrollPos(nPosScroll+lTimeOffset);
		}
		else
		{
			m_bMovingLeft=FALSE;
			if(point.x-nOffsetScroll<m_rectFrame.left)
				m_pWndScrollBar->SetScrollPos(nPosScroll-lTimeOffset);
		}
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CFrameCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	ModifyStyle(0,SS_NOTIFY);
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAIN)||!m_wndToolBar.CreateComboBox())
	{
		TRACE0("Failed to create toolbar\n");
		return ;      // fail to create
	}
	int nIndex=m_wndToolBar.CommandToIndex (ID_SNAP);
	m_wndToolBar.SetButtonStyle (nIndex, 
		m_wndToolBar.GetButtonStyle(nIndex)|TBSTYLE_DROPDOWN);
	nIndex=m_wndToolBar.CommandToIndex (ID_TICK_WIDTH);
	m_wndToolBar.SetButtonStyle(nIndex, 
		m_wndToolBar.GetButtonStyle(nIndex)|TBSTYLE_DROPDOWN);
	
	EnableTBButton(1,FALSE);
	m_wndToolBar.GetToolBarCtrl().HideButton(ID_RULER_S,TRUE);
	m_wndToolBar.GetToolBarCtrl().HideButton(ID_RULER_MS,TRUE);
	m_wndToolBar.GetToolBarCtrl().HideButton(ID_TICK_WIDTH,TRUE);
	m_wndToolBar.GetToolBarCtrl().HideButton(ID_SNAP,TRUE);
	SetRulerTickMode();
	CRect rect;
	GetClientRect(rect);
	m_wndToolBar.MoveWindow(0,0,rect.Width(),TOOLBAR_HEIGHT);
	CreateScrollBar(rect);
	SetScrollBarInfo();
	m_wndToolBar.AddItem2CMBWithAI(static_cast<int>(m_fWidthPerSecond), TRUE);
	
	m_bInitControl=TRUE;

	CStatic::PreSubclassWindow();
}

void CFrameCtrl::DrawRuler(CDC *pDC,long nVisibleAreaLeft,long nVisibleAreaRight)
{
	CPen penGray(PS_SOLID,1,RGB(128,128,128));
	if(m_bEnable==FALSE)
	{
		pDC->SelectObject(&penGray);
		pDC->SetTextColor(RGB(128,128,128));
	}
	//Account Logic Width Per Hour---------------------------------------------------
	int nLeft,nLeft2,nTop=m_rectRuler.top;
	int nHeight=m_rectRuler.Height();
	int nBottom=m_rectRuler.bottom;
	
	CRect rectLogic=m_rectDraw;
	rectLogic.right=m_lLogicWidth;
	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->Rectangle(rectLogic);
	pDC->SelectStockObject(LTGRAY_BRUSH);
	rectLogic=m_rectRuler;
	rectLogic.right=m_lLogicWidth;
	pDC->Rectangle(rectLogic);


	//Draw Big Tick-----------------------------------------------------------------
	CString strTick;
	int nWidthText;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(GetParent()->GetFont());
	int nLeftLast=INT_MIN;
	BOOL bDrawText;
	for(int i=0;i<m_lSegmentNum;i++)
	{
		nLeft=i*m_nSegmentWidth;
		if(nLeft>m_lLogicWidth) break;

/*
		strTick=Time2Str(LogicPos2Time(nLeft));
		nWidthText=pDC->GetTextExtent(strTick).cx;
		nLeft2=nLeft-nWidthText/2;
		bDrawText=nLeft2-10>nLeftLast;
		if(bDrawText)
			nLeftLast=nLeft+nWidthText;*/


		if(nLeft<nVisibleAreaLeft)
			continue;
		if(nLeft>nVisibleAreaRight)
			break;


		pDC->MoveTo(nLeft,nTop+nHeight/2);
		pDC->LineTo(nLeft,nBottom);
		
/*
		//Draw TickText
		if(bDrawText)
		{
			pDC->TextOut(nLeft2,nTop,strTick);
		}*/

	}

	//Draw Small Tick---------------------------------------------------------------------
	int nSubSugmentNum=0;
	for(int l=20;l>0;l--)
	{
		if(m_nSegmentWidth%l==0)
		{
			nSubSugmentNum=l;
			break;
		}
	}

	long nTickNum=m_lSegmentNum*nSubSugmentNum;
	int nWidthPerTick=m_lLogicWidth/nTickNum;
	nLeftLast=INT_MIN;
	for(int j=0;j<nTickNum;j++)
	{
		nLeft=j*nWidthPerTick;
		strTick=Time2Str(LogicPos2Time(nLeft));
		nWidthText=pDC->GetTextExtent(strTick).cx;
		nLeft2=nLeft-nWidthText/2;
		bDrawText=nLeft2-10>nLeftLast;
		if(bDrawText)
			nLeftLast=nLeft+nWidthText;
		if(nLeft<nVisibleAreaLeft)
			continue;
		if(nLeft>nVisibleAreaRight)
			break;

		//Draw TickText
		if(bDrawText)
		{
			pDC->MoveTo(nLeft,int(nTop+nHeight*((j%5==0)?(0.67):(0.857))));
			pDC->LineTo(nLeft,nBottom);
			pDC->TextOut(nLeft2,nTop,strTick);
		}
		else
		{
			pDC->MoveTo(nLeft,int(nTop+nHeight*((j%5==0)?(0.67):(0.857))));
			pDC->LineTo(nLeft,nBottom);
		}
	}
	//add by hans 9-21
	// Draw the day ----------------------------------------------------------------------
	//get the days of the range
	CPen penDay(PS_DOT,1,RGB(255,0,0));
	pDC->SetTextColor(RGB(255,0,0));
	CPen *pOldPenDay = pDC->SelectObject(&penDay);
	long nDays = m_lTimeRange/86400 ;
	for (int ii =1 ; ii < nDays ; ii++)
	{
		strTick.Empty();
		nLeft = Time2LogicPos (ii*86400) ;
		if(nLeft<nVisibleAreaLeft)
			continue;

		if(nLeft>nVisibleAreaRight)
			break;
		strTick.Format("Day %d",ii + 1);
		pDC->TextOut(nLeft,nTop,strTick);
	}
	pDC->SelectObject(pOldPenDay);


	//Draw dash line (tick) on the frame area---------------------------------------------
	CPen pen(PS_DOT,1,RGB(192,192,192));
	CPen* pOldPen=pDC->SelectObject(&pen);
	nTop=m_rectFrame.top;
	nBottom=m_rectFrame.bottom;

	for(int k=0;k<m_lSegmentNum;k++)
	{
		nLeft=k*m_nSegmentWidth;
		if(nLeft<nVisibleAreaLeft)
			continue;
		if(nLeft>nVisibleAreaRight)
			break;
		pDC->MoveTo(nLeft,nTop);
		pDC->LineTo(nLeft,nBottom);
	}
	pDC->SelectObject(pOldPen);
}

BOOL CFrameCtrl::SetTimeRange(long lTimeBegin, long lTimeEnd, BOOL bRefresh)
{
	long nTimeRange=lTimeEnd-lTimeBegin;
	if(nTimeRange%60!=0)
		return FALSE;
	m_lTimeBegin=lTimeBegin;
	m_lTimeEnd=lTimeEnd;
	m_lTimeRange=m_lTimeEnd-m_lTimeBegin;

	return TRUE;
}

void CFrameCtrl::Draw(CDC* pDC)
{
	//Draw Background(White)
	if(m_bDrawRectModified)
	{
		m_rectRuler=m_rectFrame=m_rectDraw=GetDrawRect();
		m_rectRuler.bottom=m_rectRuler.top+RULER_HEIGHT;
		m_rectFrame.top=m_rectRuler.bottom;
	}
	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->Rectangle(m_rectDraw);
	
	int nPosScrollBar=m_pWndScrollBar->GetScrollPos();
	pDC->SetWindowOrg(nPosScrollBar,0);
	CRect rectWnd;
	GetClientRect(rectWnd);
	
	long nVisibleAreaLeft=nPosScrollBar;
	long nVisibleAreaRight=nVisibleAreaLeft+rectWnd.Width();
	
	DrawRuler(pDC,nVisibleAreaLeft,nVisibleAreaRight);
	if(!m_bDragging&&m_pKeyFrameHot)
		m_pKeyFrameHot->DrawHot(pDC,this);
	for(int i=1;i<m_arrayKeyFrame.GetSize();i++)
	{
		m_arrayKeyFrame[i]->Draw(pDC,this,nVisibleAreaLeft,nVisibleAreaRight);
	}
	m_curTimeFrame.Draw(pDC,this,nVisibleAreaLeft,nVisibleAreaRight);
	if(m_pKeyFrameHot&&m_bDragging)
	{
		m_pKeyFrameHot->DrawTip(pDC,this);
	}
	pDC->SetWindowOrg(0,0);

	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Rectangle(m_rectDraw);
}

void CFrameCtrl::CreateScrollBar(CRect rect)
{
	if(m_pWndScrollBar==NULL)
		m_pWndScrollBar=new CScrollBar;

	if(m_pWndScrollBar&&m_pWndScrollBar->m_hWnd==NULL)
	{
		m_pWndScrollBar->Create(WS_CHILD|WS_VISIBLE|
			SBS_HORZ|SBS_BOTTOMALIGN,CRect(CPoint(0,rect.bottom-32),
			CSize(rect.Width(),32)),this,ID_SCROLLBAR);
	}
	
	
}

CRect CFrameCtrl::GetDrawRect()
{
	if(!m_bInitControl)
		return CRect(0,0,0,0);
	CRect rectClient,rectWnd,rectScrollBar;
	GetClientRect(rectWnd);
	m_pWndScrollBar->GetClientRect(rectScrollBar);
	int nTop=TOOLBAR_HEIGHT;
	int nBottom=rectWnd.bottom-rectScrollBar.Height();
	int nLeft=0;
	int nRight=rectWnd.right;
	m_bDrawRectModified=FALSE;
	rectClient=CRect(nLeft,nTop,nRight,nBottom);
	return rectClient;
}

void CFrameCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Get the minimum and maximum scroll-bar positions.
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos); 
	maxpos = pScrollBar->GetScrollLimit();
	
	// Get the current position of scroll box.
	int curpos = pScrollBar->GetScrollPos();
	
	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		curpos = minpos;
		break;
		
	case SB_RIGHT:      // Scroll to far right.
		curpos = maxpos;
		break;
		
	case SB_ENDSCROLL:   // End scroll.
		break;
		
	case SB_LINELEFT:      // Scroll left.
		{
			SCROLLINFO   info;
			pScrollBar->GetScrollInfo(&info, SIF_ALL);
			if (curpos > minpos)
				curpos-=info.nMax/1000;
		}
		break;
		
	case SB_LINERIGHT:   // Scroll right.
		{
			SCROLLINFO   info;
			pScrollBar->GetScrollInfo(&info, SIF_ALL);
			if (curpos < maxpos)
				curpos+=info.nMax/1000;
		}
		break;
		
	case SB_PAGELEFT:    // Scroll one page left.
		{
			// Get the page size. 
			SCROLLINFO   info;
			pScrollBar->GetScrollInfo(&info, SIF_ALL);
			
			if (curpos > minpos)
				curpos = max(minpos, curpos - (int) info.nPage);
		}
		break;
		
	case SB_PAGERIGHT:      // Scroll one page right.
		{
			// Get the page size. 
			SCROLLINFO   info;
			pScrollBar->GetScrollInfo(&info, SIF_ALL);
			
			if (curpos < maxpos)
				curpos = min(maxpos, curpos + (int) info.nPage);
		}
		break;
		
	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		curpos = nPos;      // of the scroll box at the end of the drag operation.
		break;
		
	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;
	}
	
	// Set the new position of the thumb (scroll box).
	pScrollBar->SetScrollPos(curpos);
	Refresh();	
	CStatic::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFrameCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	m_bDrawRectModified=TRUE;
	// TODO: Add your message handler code here
	CRect rect;
	GetClientRect(rect);
	
	m_wndToolBar.MoveWindow(0,0,rect.Width(),TOOLBAR_HEIGHT);
	m_pWndScrollBar->GetClientRect(rect);
	m_pWndScrollBar->SetWindowPos(NULL,0,cy-rect.Height(),cx,rect.Height(),SWP_NOACTIVATE);
	Refresh();	
}

BOOL CFrameCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CFrameCtrl::Doc2Client(long &nDoc)
{
	int nPosScrollBar=m_pWndScrollBar->GetScrollPos();
	nDoc-=nPosScrollBar;
}

void CFrameCtrl::Client2Doc(long &nClient)
{
	int nPosScrollBar=m_pWndScrollBar->GetScrollPos();
	nClient+=nPosScrollBar;
}

CString CFrameCtrl::Time2Str(long lTime)
{
	CString strText;
	if(m_bRulerModeIsOnlySecond)
	{
		strText.Format("%d",lTime);
	}
	else
	{
		CTimeSpan ts(lTime);
		strText=ts.Format("%H:%M:%S");
		if(lTime==86400 *3 )
			strText="72:00:00";
	}
	return strText;
}

LRESULT CFrameCtrl::OnSelChangeScale(WPARAM wParam,LPARAM lParam)
{
	SetTimeRange(m_lTimeBegin,m_lTimeEnd);
	SetTimeScale(wParam);

	
	int nCount=m_wndToolBar.m_cmbScale.GetCount();
	int nCurSel=m_wndToolBar.m_cmbScale.GetCurSel();
	if(nCurSel==0)
	{
		EnableTBButton(5,FALSE);
		EnableTBButton(4,TRUE);
	}
	else if(nCurSel==nCount-1)
	{
		EnableTBButton(4,FALSE);
		EnableTBButton(5,TRUE);
	}
	else
	{
		EnableTBButton(4,TRUE);
		EnableTBButton(5,TRUE);
	}
	return TRUE;
}

void CFrameCtrl::MoveCurTimeKeyFrame2Center()
{
	//Move Current-Time Key-Frame to Center
	CRect rect ;
	GetClientRect(rect);
	long lWidth=rect.Width()/2;
	long lTime=m_curTimeFrame.GetTime();
	long lPos=Time2LogicPos(lTime);
	m_pWndScrollBar->SetScrollPos(lPos-lWidth);
	
}

void CFrameCtrl::SetScrollBarInfo()
{
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(ScrollInfo);     // size of this structure
	ScrollInfo.fMask = SIF_ALL;                 // parameters to set
	ScrollInfo.nMin = 0;                        // minimum scrolling position
	ScrollInfo.nMax = m_lLogicWidth+m_lLogicWidth/100;            // maximum scrolling position
	ScrollInfo.nPage = m_lLogicWidth/100;        // the page size of the scroll box
	ScrollInfo.nPos = 0;                        // initial position of the scroll box
	ScrollInfo.nTrackPos = 0;                   // immediate position of a scroll box that the user is dragging
	m_pWndScrollBar->SetScrollInfo(&ScrollInfo);
	
}

long CFrameCtrl::Time2LogicPos(long lTime)
{
	return long((lTime-m_lTimeBegin)*m_fWidthPerSecond+0.5);
}

long CFrameCtrl::LogicPos2Time(long lPos,long* plOffsetPos)
{
	float lTime=lPos/m_fWidthPerSecond+m_lTimeBegin;
	if(plOffsetPos)
		*plOffsetPos=0;
	return long(lTime+0.5);
}

BOOL CFrameCtrl::SetCurrentTime(long lTime, BOOL bRefresh)
{
	BOOL b=m_curTimeFrame.SetTime(lTime,this);
	
	if(b&&bRefresh)
		Refresh();

	GetParent()->SendMessage(FCM_NOTIFYSELECT_TIME, (WPARAM) lTime, (LPARAM) m_bAutoGoto);

	return b;
}

void CFrameCtrl::Refresh()
{
	CRect rectClip=m_rectDraw;
	InvalidateRect(rectClip);
}

CRect CFrameCtrl::GetSnapRectKeyFrame(long lLeft)
{
	CRect rect;
	int nTop=m_rectFrame.top;
	int nBottom=m_rectFrame.bottom;
	rect.left=lLeft-2;
	rect.right=lLeft+3;
	int nHeight=(nBottom-nTop)/3;
	rect.top=nTop+nHeight;
	rect.bottom=nBottom-nHeight;
	return rect;
}



BOOL CFrameCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point ;
	GetCursorPos(&point);
	ScreenToClient(&point);
	KeyFrame* pKeyFrame=HitTest(point);
	m_pKeyFrameHot=pKeyFrame;
	Refresh();
	if(pKeyFrame&&(!m_bPopupingMenu))
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));	
		return TRUE;
	}

	// TODO: Add your message handler code here and/or call default
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CFrameCtrl::OnViewZoomIn()
{
	int nCount=m_wndToolBar.m_cmbScale.GetCount();
	int nCurSel=m_wndToolBar.m_cmbScale.GetCurSel();
	if(nCurSel<nCount-1)
	{
		nCurSel++;
		m_wndToolBar.m_cmbScale.SetCurSel(nCurSel);
		m_wndToolBar.OnSelChangeZoomComboBox();
		Refresh();
		EnableTBButton(5,TRUE);
	}
	else
		EnableTBButton(4,FALSE);

}

void CFrameCtrl::OnViewZoomOut()
{
	int nCount=m_wndToolBar.m_cmbScale.GetCount();
	int nCurSel=m_wndToolBar.m_cmbScale.GetCurSel();
	if(nCurSel>0)
	{
		nCurSel--;
		m_wndToolBar.m_cmbScale.SetCurSel(nCurSel);
		m_wndToolBar.OnSelChangeZoomComboBox();
		Refresh();
		EnableTBButton(4,TRUE);
	}
	else
		EnableTBButton(5,FALSE);
		
}

void CFrameCtrl::OnKeyFrameGoto()
{
	m_bAutoGoto = !m_bAutoGoto;
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_KEYFRAME_GOTO, m_bAutoGoto);
		
}

void CFrameCtrl::OnKeyFrameAdd()
{
	if(!m_bOverlapable&&IsRepeat(m_curTimeFrame.GetTime(),FALSE))
	{
		AfxMessageBox("Please place key frame on another place!");
		return;
	}
	KeyFrame* pNewKeyFrame=new KeyFrame(++m_lNextID,m_curTimeFrame.GetTime());
	m_arrayKeyFrame.Add(pNewKeyFrame);
	SelectKeyFrame(pNewKeyFrame);
	Refresh();
	GetParent()->SendMessage(FCM_NOTIFYINSERT_KF, (WPARAM) pNewKeyFrame->GetTime(), (LPARAM)pNewKeyFrame);
}

void CFrameCtrl::OnKeyFrameDel()
{
	int nDeleteIdx=-1;
	for(int j=0;j<m_arrayKeyFrameSeled.GetSize();j++)
	{
		for(int i=0;i<m_arrayKeyFrame.GetSize();i++)
		{
			if(m_arrayKeyFrameSeled[j]==m_arrayKeyFrame[i])
			{
				nDeleteIdx = i;
				break;
			}
		}
		if(nDeleteIdx!=-1)
			break;
	}

	GetParent()->SendMessage(FCM_NOTIFYDELETE_KF, (WPARAM) m_arrayKeyFrame[nDeleteIdx]->GetTime(), NULL);
	delete m_arrayKeyFrame[nDeleteIdx];
	m_arrayKeyFrame.RemoveAt(nDeleteIdx);
	
	SelectKeyFrame(NULL);
	Refresh();
	
}

CFrameCtrl::KeyFrame* CFrameCtrl::HitTest(CPoint point, BOOL bSeled,BOOL bRefresh,BOOL bClearSelected)
{

	for(int i=0;i<m_arrayKeyFrame.GetSize();i++)
	{
		CRect rectKeyframe=m_arrayKeyFrame[i]->GetRoundBox(this,TRUE);//GetSnapRectKeyFrame(lPos);
		if(rectKeyframe.PtInRect(point))
		{
			if(bSeled)
			{
				if(bClearSelected==FALSE&&m_arrayKeyFrame[i]==(&m_curTimeFrame))
				{
				}
				else
				{
					SelectKeyFrame(m_arrayKeyFrame[i],bClearSelected);
				}
			}
			
			if(bRefresh)
				Refresh();
			return m_arrayKeyFrame[i];
		}
	}
	return NULL;
}

void CFrameCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bDragging&&m_arrayKeyFrame.GetSize()>0)
	{
		m_bDragging=FALSE;
		m_bSelectMenu=FALSE;
		ReleaseCapture();	
		Refresh();
	}
	CStatic::OnLButtonUp(nFlags, point);
}

void CFrameCtrl::SelectKeyFrame(KeyFrame *pKeyFrame,BOOL bClear)
{
	if(bClear)
	{
		for(int i=0;i<m_arrayKeyFrameSeled.GetSize();i++)
			m_arrayKeyFrameSeled[i]->SetSelected(FALSE);
		m_arrayKeyFrameSeled.RemoveAll();
	}
	if(pKeyFrame)
	{
		pKeyFrame->SetSelected(TRUE);
		m_arrayKeyFrameSeled.Add(pKeyFrame);
		EnableTBButton(1,pKeyFrame!=&m_curTimeFrame);
	}
	else
		EnableTBButton(1,FALSE);

	
}

void CFrameCtrl::EnableTBButton(int nIndex, BOOL bEnable)
{
	TBBUTTON tbbtn;
	m_wndToolBar.GetToolBarCtrl().GetButton(nIndex,&tbbtn);
	m_wndToolBar.GetToolBarCtrl().EnableButton(tbbtn.idCommand,bEnable);
}

void CFrameCtrl::SetRulerTickMode(BOOL bOnlySecond)
{
	int nID7=ID_RULER_MS;
	int nID8=ID_RULER_S;

	if(m_bRulerModeIsOnlySecond=bOnlySecond)
	{
		m_wndToolBar.GetToolBarCtrl().CheckButton(nID8);
		m_wndToolBar.GetToolBarCtrl().CheckButton(nID7,FALSE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().CheckButton(nID7);
		m_wndToolBar.GetToolBarCtrl().CheckButton(nID8,FALSE);
	}
	Refresh();
}

void CFrameCtrl::OnRulerS()
{
	SetRulerTickMode(TRUE);
}

void CFrameCtrl::OnRulerMS()
{
	SetRulerTickMode();
}

void CFrameCtrl::OnSnapSelect()
{
    CRect rcCaller;
    // Get rect of toolbar item
    m_wndToolBar.GetItemRect (m_wndToolBar.CommandToIndex (ID_SNAP), rcCaller);
    m_wndToolBar.ClientToScreen (rcCaller);
    rcCaller.top++;
    rcCaller.bottom--;
	
    CNewMenu menu;
	menu.LoadMenu(IDR_SNAP_SELECT);
	CNewMenu* pMenu=(CNewMenu*)(menu.GetSubMenu(0));
	for(int i=0;i<pMenu->GetMenuItemCount();i++)
		pMenu->CheckMenuItem(pMenu->GetMenuItemID(i),MF_UNCHECKED );
	
	switch(m_nSnapValue)
	{
	case 2:
		pMenu->CheckMenuItem(ID_SNAP_2,MF_CHECKED );
		break;
	case 5:
		pMenu->CheckMenuItem(ID_SNAP_5,MF_CHECKED );
		break;
	case 10:
		pMenu->CheckMenuItem(ID_SNAP_10,MF_CHECKED );
		break;
	case 20:
		pMenu->CheckMenuItem(ID_SNAP_20,MF_CHECKED );
		break;
	case 30:
		pMenu->CheckMenuItem(ID_SNAP_30,MF_CHECKED );
		break;
	case 60:
		pMenu->CheckMenuItem(ID_SNAP_60,MF_CHECKED );
		break;
	case 120:
		pMenu->CheckMenuItem(ID_SNAP_120,MF_CHECKED );
		break;
	}
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,rcCaller.left+22,rcCaller.bottom, this);
	
}

void CFrameCtrl::OnSnapRange(UINT nID)
{
	switch(nID)
	{
	case ID_SNAP_2:
		m_nSnapValue=2;
		break;
	case ID_SNAP_5:
		m_nSnapValue=5;
		break;
	case ID_SNAP_10:
		m_nSnapValue=10;
		break;
	case ID_SNAP_20:
		m_nSnapValue=20;
		break;
	case ID_SNAP_30:
		m_nSnapValue=30;
		break;
	case ID_SNAP_60:
		m_nSnapValue=60;
		break;
	case ID_SNAP_120:
		m_nSnapValue=120;
		break;
	}
}

void CFrameCtrl::OnSnap()
{
	m_bSnap=!m_bSnap;
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_SNAP,m_bSnap);
}

void CFrameCtrl::SetOverlapable(BOOL bOverlapable)
{
	m_bOverlapable=bOverlapable;
}

BOOL CFrameCtrl::IsRepeat(long lTime,BOOL bCheckCurTimeFrame)
{
	for(int i=0;i<m_arrayKeyFrame.GetSize();i++)
	{
		if(bCheckCurTimeFrame)
		{
			if(m_arrayKeyFrame[i]->GetTime()==lTime)
				return TRUE;
		}
		else
		{
			if((m_arrayKeyFrame[i]!=&m_curTimeFrame)&&m_arrayKeyFrame[i]->GetTime()==lTime)
				return TRUE;
		}
	}
	return FALSE;
}

LRESULT CFrameCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = FALSE;
	m_pKeyFrameHotSeled=NULL;
	m_ToolTip.Close();
	
    return 0;
}


LRESULT CFrameCtrl::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof (tme);
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.hwndTrack = m_hWnd;
    //tme.dwHoverTime = HOVER_DEFAULT;
	tme.dwHoverTime = 0;
    _TrackMouseEvent (&tme);
	//
	// Get the nearest item in the list box to the mouse cursor
	//
	CPoint point;
	point.x = LOWORD (lParam);
	point.y	= HIWORD (lParam);
	BOOL	bOutSide = FALSE;

	KeyFrame* pKeyFrameTemp =HitTest(point);
	
	// Display the tool tip text only if the previous and current
	// item are different.
	if(pKeyFrameTemp&&m_pKeyFrameHotSeled != pKeyFrameTemp)
	{
		m_pKeyFrameHotSeled=pKeyFrameTemp;
		CString strTip,strTemp;
		if(m_pKeyFrameHotSeled->GetID()==0)
			strTemp="Type : Current Time Frame\n";
		else
			strTemp="Type : Key Frame\n";
		strTip.Format("ID     : %d\n",m_pKeyFrameHotSeled->GetID());
		strTip+="Time : "+Time2Str(m_pKeyFrameHotSeled->GetTime())+"\n";
		//strTip+="Run Time: "+Time2Str( m_pKeyFrameHotSeled->GetTime() -  )+"\n";
		if(m_pKeyFrameHotSeled->GetID()>0)
		{
			int nIndex=m_pMovie->FindRelationKeyFrame((void*)m_pKeyFrameHotSeled);
			if(nIndex>0)
			{
				strTip+="Run-Time : "+m_pMovie->GetKFRunTimeFormated(nIndex)+"\n";
				strTip+="Run-Time To Previous KeyFrame : "+m_pMovie->GetKFRunTimeFormated(nIndex,nIndex-1)+"\n";
			}
			else if(nIndex==0)
			{
				CString strTime=m_pMovie->GetKFRunTimeFormated(nIndex);
				strTip+="Run-Time : "+strTime+"\n";
				strTip+="Run-Time To Previous Key Frame : "+strTime+"\n";
			}
		}
		strTip+=strTemp;
		ShowToolTip(strTip);
	}
	if(pKeyFrameTemp==NULL)
	{
		m_ToolTip.Close();
		m_pKeyFrameHotSeled=NULL;
	}

    return 0;
}

void CFrameCtrl::ShowToolTip(LPCTSTR string)
{
	m_ToolTip.Close();
	
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	point.Offset(16,0);
	m_ToolTip.SetText(string);	
	point.y-=20;
	m_ToolTip.Show(point);
	
}

void CFrameCtrl::SetTimeScale(int nTimeScale,BOOL bRefresh,BOOL bSelComboBox)
{
	m_nScale=nTimeScale;
	CRect rectClient;
	GetClientRect(rectClient);		
	int nWidth=rectClient.Width();
	//m_fWidthPerSecond=nWidth/float(m_nScale);
	//absolute time
	//relative time

//	m_lLogicWidth=3*24*60*60*nWidth/m_nScale;
	
	//modify by hans 9-20-2005
	m_lLogicWidth = (m_lTimeRange/m_nScale) * nWidth;
	m_lSegmentNum=24;
	m_nSegmentWidth=m_lLogicWidth/m_lSegmentNum;
	m_nSegmentWidth=m_nSegmentWidth/10*10;
	m_lLogicWidth=m_nSegmentWidth*m_lSegmentNum;
	m_fWidthPerSecond=m_lLogicWidth/float(m_lTimeRange);
	if(m_pWndScrollBar&&m_pWndScrollBar->m_hWnd)
		SetScrollBarInfo();
	MoveCurTimeKeyFrame2Center();
	if(bSelComboBox)
	{
		m_wndToolBar.SetSelect(m_nScale);
	}
	if(bRefresh)
		Refresh();
	/*
	long nTimeRange=lTimeEnd-lTimeBegin;
	if(nTimeRange%60!=0)
	return FALSE;
	m_lTimeBegin=lTimeBegin;
	m_lTimeEnd=lTimeEnd;
	m_lTimeRange=m_lTimeEnd-m_lTimeBegin;
	m_lSegmentNum=m_lTimeRange/RULER_TICK_NUM_PER_BIG_TICK;
	m_nSegmentWidth=RULER_TICK_NUM_PER_BIG_TICK*m_fWidthPerSecond;
	m_lLogicWidth=m_nSegmentWidth*m_lSegmentNum;//20(Pixel/Tick)*6(Tick)*
	if(m_pWndScrollBar&&m_pWndScrollBar->m_hWnd)
	SetScrollBarInfo();
	if(bRefresh)
	Refresh();
	
	 */
}

void CFrameCtrl::OnKeyFrameMenu(UINT nID)
{
	m_arrayKeyFrameSeled.RemoveAll();
	m_arrayKeyFrameSeled.Add(m_arrayKeyFrameMenu[nID-1]);
	m_bDragging=TRUE;
	SetCapture();
				
}

void CFrameCtrl::DeleteAllKeyFrames()
{
/*
	m_arrayKeyFrame.RemoveAll();
	SelectKeyFrame(NULL);
*/

	//Modifyed by Tim-----------------------------------
	m_pKeyFrameHot=NULL;
	int nCount=m_arrayKeyFrame.GetSize();
	//First is CurTimeFrame
	for(int i=1;i<nCount;i++)//First is Current Time KeyFrame,So Begin From 1
	{
		delete m_arrayKeyFrame[i];
	}
	m_arrayKeyFrame.RemoveAt(1,nCount-1);
	m_arrayKeyFrameSeled.RemoveAll();
	m_lNextID=0;
}

void* CFrameCtrl::InsertKeyFrame(long nTime)
{
	nTime = nTime/100;
	if(!m_bOverlapable&&IsRepeat(nTime,FALSE))
	{
		AfxMessageBox("Please place key frame on another place!");
		return NULL;
	}
	KeyFrame* pNewKeyFrame=new KeyFrame(++m_lNextID,nTime);
	m_arrayKeyFrame.Add(pNewKeyFrame);
	//Modifyed by Tim-----------------------------------
	SelectKeyFrame(pNewKeyFrame);
	Refresh();
	return (void*)pNewKeyFrame;
	//GetParent()->SendMessage(FCM_NOTIFYINSERT_KF, (WPARAM) pNewKeyFrame->GetTime(), NULL);
}

void CFrameCtrl::EnableCtrl(BOOL bEnable)
{
	m_bEnable=bEnable;
	EnableWindow(m_bEnable);
	Refresh();
}

void CFrameCtrl::OnEnable(BOOL bEnable) 
{
	CStatic::OnEnable(bEnable);
	m_pWndScrollBar->EnableWindow(m_bEnable);
	m_wndToolBar.EnableWindow(m_bEnable);
	CWnd* pWnd=m_wndToolBar.GetWindow(GW_CHILD);
	while(pWnd)
	{
		pWnd->EnableWindow(m_bEnable);
		pWnd=pWnd->GetWindow(GW_HWNDNEXT);
	}
	int nBtnCount=m_wndToolBar.GetToolBarCtrl().GetButtonCount();
	TBBUTTON tbBtn;
	for(int i=0;i<nBtnCount;i++)
	{
		m_wndToolBar.GetToolBarCtrl().GetButton(i,&tbBtn);			
		m_wndToolBar.GetToolBarCtrl().EnableButton(tbBtn.idCommand,m_bEnable);
	}
	if(m_bEnable)
	{
		if(m_arrayKeyFrameSeled.GetSize()==0)
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_KEYFRAME_DEL,FALSE);
		if(m_wndToolBar.m_cmbScale.GetCurSel()==m_wndToolBar.m_cmbScale.GetCount()-1)
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VIEW_ZOOMIN,FALSE);
		if(m_wndToolBar.m_cmbScale.GetCurSel()==0)
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VIEW_ZOOMOUT,FALSE);
	}
}

void CFrameCtrl::SetMovie(CMovie *pMovie)
{
	m_pMovie=pMovie;
}

void CFrameCtrl::SetStartDate(CStartDate &startDate)
{
	m_startDate = startDate;
}
