#if !defined(AFX_FRAMECTRL_H__20EB7F7C_944E_4A32_8BDE_17ED069311C2__INCLUDED_)
#define AFX_FRAMECTRL_H__20EB7F7C_944E_4A32_8BDE_17ED069311C2__INCLUDED_

#include "XPStyle\ToolbarXP.h"	// Added by ClassView
#include "XPStyle\ToolTip2.h"
#include "../Common/StartDate.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl window
#define ID_SCROLLBAR						200
#define TOOLBAR_HEIGHT						31
#define RULER_HEIGHT						24

#define ID_SNAP_SELECT DROPDOWN(ID_SNAP)
#define ID_TICK_WIDTH_SELECT DROPDOWN(ID_TICK_WIDTH)
#include <afxtempl.h>
#include "XPStyle\tooltip2.h"	// Added by ClassView
#include "Movie.h"
class CFrameCtrl : public CStatic
{
		class KeyFrame
		{
		// Construction
		public:
			KeyFrame(long nID)
			{
				m_lID=nID;
				m_bSelected=FALSE;
				m_lTime=0;
			}
			KeyFrame(long nID,long lTime,BOOL bSelected=TRUE)
			{
				m_lID=nID;
				m_bSelected=bSelected;
				m_lTime=lTime;
			}
			virtual void Draw(CDC* pDC,CFrameCtrl* pFrameCtrl,long nVisibleAreaLeft,long nVisibleAreaRight)
			{
				CPen pen(PS_SOLID,(!pFrameCtrl->m_bDragging&&m_bSelected)?2:1,RGB(0,0,255));
				DrawEx(pDC,pFrameCtrl,pen,nVisibleAreaLeft,nVisibleAreaRight);
			}
			virtual void DrawHot(CDC* pDC,CFrameCtrl* pFrameCtrl)
			{
				CPen pen(PS_SOLID,1,RGB(0,0,0));
				CPoint ptOrg=pDC->GetWindowOrg();
				pDC->SetWindowOrg(CPoint(ptOrg.x-1,ptOrg.y-1));
				DrawEx(pDC,pFrameCtrl,pen);
				pDC->SetWindowOrg(ptOrg);
			}
			virtual void DrawEx(CDC* pDC,CFrameCtrl* pFrameCtrl,CPen& pen,long nVisibleAreaLeft=INT_MIN,long nVisibleAreaRight=INT_MAX)
			{
				long lLeft=pFrameCtrl->Time2LogicPos(m_lTime);//m_lCurrentTimePos;
				if(lLeft<nVisibleAreaLeft)
					return;
				if(lLeft>nVisibleAreaRight)
					return;
				CPen* pOlePen=pDC->SelectObject(&pen);
				int nTop=pFrameCtrl->m_rectFrame.top;
				int nBottom=pFrameCtrl->m_rectFrame.bottom;
				pDC->MoveTo(lLeft,nTop);
				pDC->LineTo(lLeft,nBottom);
				CRect rect=GetRoundBox(pFrameCtrl,FALSE);//pFrameCtrl->GetSnapRectKeyFrame(lLeft);
				pDC->SelectStockObject(WHITE_BRUSH);
				pDC->Rectangle(rect);
				pDC->SelectObject(pOlePen);
			}
			virtual CRect GetRoundBox(CFrameCtrl* pFrameCtrl,BOOL bClient=FALSE)
			{
				CRect rect;
				long lLeft=pFrameCtrl->Time2LogicPos(m_lTime);
				if(bClient)
					pFrameCtrl->Doc2Client(lLeft);
				int nTop=pFrameCtrl->m_rectFrame.top;
				int nBottom=pFrameCtrl->m_rectFrame.bottom;
				rect.left=lLeft-2;
				rect.right=lLeft+3;
				int nHeight=(nBottom-nTop)/3;
				rect.top=nTop+nHeight;
				rect.bottom=nBottom-nHeight;
				return rect;
				
			}
			void DrawTip(CDC* pDC,CFrameCtrl* pFrameCtrl)
			{
				long lLeft=pFrameCtrl->Time2LogicPos(m_lTime);
				int nTop=pFrameCtrl->m_rectFrame.top;
				nTop+=6;
				CString strTime=pFrameCtrl->Time2Str(m_lTime);
				CSize size=pDC->GetTextExtent(strTime);
				if(pFrameCtrl->m_bMovingLeft)
					lLeft-=(10+size.cx);
				else
					lLeft+=10;
				CRect rect(CPoint(lLeft,nTop),size);
				rect.InflateRect(3,1,3,1);
				pDC->SetBkMode(TRANSPARENT);
				CBrush brushToolTip(GetSysColor(COLOR_INFOBK));
				CBrush* pOldBrush = pDC->SelectObject(&brushToolTip);
				pDC->Rectangle(rect);
				pDC->DrawText(strTime,rect,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
				pDC->SelectObject(pOldBrush);
				
			}
			inline BOOL SetTime(long lTime,CFrameCtrl* pFrameCtrl)
			{
				if(lTime<pFrameCtrl->m_lTimeBegin||lTime>pFrameCtrl->m_lTimeEnd)
					return FALSE;
				m_lTime=lTime;
				return TRUE;
			}
			inline long GetTime()
			{
				return m_lTime;
			}
			inline long GetID()
			{
				return m_lID;
			}
			inline void SetSelected(BOOL bSeled)
			{
				m_bSelected=bSeled;
			}
		// Attributes
		protected:
			BOOL m_bSelected;
			long m_lID;
			long m_lTime;
		};
		class CCurTimeFrame:public KeyFrame
		{
		// Construction
		public:
			CCurTimeFrame():KeyFrame(0)
			{};
			virtual void Draw(CDC* pDC,CFrameCtrl* pFrameCtrl,long nVisibleAreaLeft,long nVisibleAreaRight)
			{
				CPen pen(PS_SOLID,1,RGB(255,0,0));
				DrawEx(pDC,pFrameCtrl,pen,nVisibleAreaLeft,nVisibleAreaRight);
			};
			virtual void DrawEx(CDC* pDC,CFrameCtrl* pFrameCtrl,CPen& pen,long nVisibleAreaLeft=INT_MIN,long nVisibleAreaRight=INT_MAX)
			{
				long lLeft=pFrameCtrl->Time2LogicPos(m_lTime);//m_lCurrentTimePos;
				if(lLeft<nVisibleAreaLeft)
					return;
				if(lLeft>nVisibleAreaRight)
					return;
				CPen* pOlePen=pDC->SelectObject(&pen);
				int nTop=pFrameCtrl->m_rectRuler.top;
				int nBottom=pFrameCtrl->m_rectFrame.bottom;
				pDC->MoveTo(lLeft,nTop);
				pDC->LineTo(lLeft,nBottom);
				CRect rect=GetRoundBox(pFrameCtrl,FALSE);//pFrameCtrl->GetSnapRectKeyFrame(lLeft);
				CBrush brushRed(RGB(192,192,192));
				CBrush* pOldBrush=pDC->SelectObject(&brushRed);
				CPoint pt[6];
				GetSlideShapePoint(rect,pt);
				pDC->Polygon(pt,5);
				CPen penLight(PS_SOLID,1,RGB(225,225,225));
				pDC->SelectObject(&penLight);
				pDC->Polyline(pt,4);
				CPen penDark(PS_SOLID,1,RGB(64,64,64));
				pDC->SelectObject(&penDark);
				pDC->Polyline(pt+3,3);
				pDC->SelectObject(pOlePen);
				pDC->SelectObject(pOldBrush);
			}

			void GetSlideShapePoint(CRect rect,CPoint* pt)
			{
				int nOffsetH=rect.Height()/5;
				int nOffsetW=rect.Width()/2;
				pt[0]=CPoint(rect.left+nOffsetW,rect.bottom);
				pt[1]=CPoint(rect.left,rect.bottom-nOffsetH);
				pt[2]=CPoint(rect.left,rect.top);
				pt[3]=CPoint(rect.right,rect.top);
				pt[4]=CPoint(rect.right,rect.bottom-nOffsetH);
				pt[5]=pt[0];
				
			}
			virtual CRect GetRoundBox(CFrameCtrl* pFrameCtrl,BOOL bClient=FALSE)
			{
				CRect rect;
				long lLeft=pFrameCtrl->Time2LogicPos(m_lTime);
				if(bClient)
					pFrameCtrl->Doc2Client(lLeft);
				int nTop=pFrameCtrl->m_rectRuler.top;
				int nBottom=pFrameCtrl->m_rectRuler.bottom;
				rect.left=lLeft-4;
				rect.right=lLeft+4;
				rect.top=nTop+1;
				rect.bottom=nBottom;
				rect.OffsetRect(0,pFrameCtrl->m_rectRuler.Height()/2);
				return rect;
				
			}
		};
// Construction
public:
	CFrameCtrl();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	long m_lNextID;
	void SetMovie(CMovie* pMovie);
	CMovie* m_pMovie;
	BOOL m_bEnable;
	void EnableCtrl(BOOL bEnable=TRUE);
	BOOL m_bPopupingMenu;
	BOOL m_bSelectMenu;
	void SetTimeScale(int nTimeScale,BOOL bRefresh=TRUE,BOOL bSelComboBox=FALSE);
	void ShowToolTip(LPCTSTR string);
	BOOL IsRepeat(long lTime,BOOL bCheckCurTimeFrame=TRUE);
	void SetOverlapable(BOOL bOverlapable);
	void SetRulerTickMode(BOOL bOnlySecond=FALSE);
	void EnableTBButton(int nIndex,BOOL bEnable);
	void SelectKeyFrame(KeyFrame* pKeyFrame,BOOL bClear=TRUE);
	KeyFrame* HitTest(CPoint point,BOOL bSeled=FALSE,BOOL bRefresh=FALSE,BOOL bClearSelected=TRUE);
	void MoveCurTimeKeyFrame2Center();
	CRect GetSnapRectKeyFrame(long lLeft);
	void Refresh();
	BOOL SetCurrentTime(long nTime,BOOL bRefresh=TRUE);
	long LogicPos2Time(long lPos,long* plOffsetPos=NULL);
	long Time2LogicPos(long lTime);
	void SetScrollBarInfo();
	CString Time2Str(long lTime);
	void Client2Doc(long &lClient);
	void Doc2Client(long &lDoc);
	void DeleteAllKeyFrames();
	void* InsertKeyFrame(long nTime);
	//-------------------------------------------------------------------------------------
	CToolTip2 m_ToolTip;
	KeyFrame* m_pKeyFrameHot;
	BOOL m_bMovingLeft;//Is move to left?
	KeyFrame* m_pKeyFrameHotSeled;
	BOOL m_bMouseOver;
	CArray<KeyFrame*,KeyFrame*> m_arrayKeyFrame;
	CArray<KeyFrame*,KeyFrame*> m_arrayKeyFrameMenu;
	CCurTimeFrame m_curTimeFrame;
	CArray<KeyFrame*,KeyFrame*> m_arrayKeyFrameSeled;
	BOOL m_bOverlapable;//Is overlapable?
	int m_nSnapValue;
	BOOL m_bSnap;
	BOOL m_bRulerModeIsOnlySecond;//Ruler display mode

	int m_nSegmentWidth;//Width Per Segment
	long m_lTimeRange;//Total time range
	long m_lSegmentNum;//Segment Number
	long m_lLogicWidth;//Total Ruler Logic Width
	float m_fWidthPerSecond;
	long m_lTimeEnd;//End Time
	long m_lTimeBegin;//Begin Time

	long m_lMouseDownPos;
	BOOL m_bDragging;
	int m_nScale;//View Zoom Scale
	CRect m_rectDraw;
	CRect m_rectRuler;
	CRect m_rectFrame;
	BOOL m_bDrawRectModified;
	BOOL m_bInitControl;
	CScrollBar* m_pWndScrollBar;
	CToolBarXP m_wndToolBar;
	BOOL m_bAutoGoto;
	//-------------------------------------------------------------------------------------
	CRect GetDrawRect();
	void CreateScrollBar(CRect rect);
	void Draw(CDC* pDC);
	BOOL SetTimeRange(long lTimeBegin,long lTimeEnd,BOOL bRefresh=TRUE);
	void DrawRuler(CDC* pDC,long nVisibleAreaLeft,long nVisibleAreaRight);


	virtual ~CFrameCtrl();
	// the simulation start date 
	CStartDate m_startDate;

	void SetStartDate(CStartDate &startDate);

	// Generated message map functions
protected:
	//{{AFX_MSG(CFrameCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	afx_msg LRESULT OnSelChangeScale(WPARAM wParam,LPARAM lParam);
	afx_msg void OnKeyFrameAdd();
	afx_msg void OnKeyFrameDel();
	afx_msg void OnKeyFrameGoto();
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnRulerS();
	afx_msg void OnRulerMS();
	afx_msg void OnSnap();
	afx_msg void OnSnapSelect();
	afx_msg void OnSnapRange(UINT nID);
	afx_msg void OnKeyFrameMenu(UINT nID);
	LRESULT OnMouseHover (WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMECTRL_H__20EB7F7C_944E_4A32_8BDE_17ED069311C2__INCLUDED_)
