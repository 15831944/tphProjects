// AircraftOpenGLCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AircraftOpenGLCtrl.h"
#include "../InputOnBoard/AircraftLayoutEditContext.h"
#include "../InputOnBoard/CInputOnboard.h"
#include "../Main/LayOutFrame.h"
#include "../Main/TermPlanDoc.h"
#include "../Main/MainFrm.h"
#include "../Common/ARCVector.h"
#include "../InputOnboard/CInputOnboard.h"
#include "../Common/AirportDatabase.h"
#include "../InputOnboard/Seat.h"
#include "../InputOnboard/AircraftPlacements.h"
#include "../InputOnboard/Door.h"
#include "../InputOnboard/CabinCrewGeneration.h"
#include <CommonData/3DTextManager.h>
#include "../InputOnboard/AircaftLayOut.h"
#include "onboard/AircraftLayoutEditor.h"

#include <Common/OleDragDropManager.h>

// CAircraftLayoutCtrl
#define  GL_PI 3.1415f
using namespace Ogre;

#pragma warning (disable : 4996)

IMPLEMENT_DYNAMIC(CAircraftLayoutCtrl, CWnd)
CAircraftLayoutCtrl::CAircraftLayoutCtrl(CabinCrewGeneration* CrewPaxList,BOOL bInit,int nFlightID)
:m_CrewPaxList(CrewPaxList)
,m_bInit(bInit)
{
	m_layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(nFlightID);
	ASSERT(m_layout);
	m_pDeckManager = m_layout->GetDeckManager();
}

CAircraftLayoutCtrl::~CAircraftLayoutCtrl()
{
}

BEGIN_MESSAGE_MAP(CAircraftLayoutCtrl, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CAircraftLayoutCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	int nCount = (int)m_vGLCtrl.size();
	for (int i = 0 ; i < nCount; i++)
	{
		m_vGLCtrl[i]->MoveWindow(CRect(0,i*cy/nCount,cx,(i+1)*cy/nCount),true);
	}
}

void CAircraftLayoutCtrl::UpdateDeckLayoutOutLine()
{
	CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(1);
	CMainFrame* pMainFram = (CMainFrame*)(GetParent()->GetParent());
	CMDIChildWnd* pMDIActive = (CMDIChildWnd*)pMainFram->MDIGetActive();
	if (pMDIActive == NULL)
		return ;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	/*CAircraftModel acModel ;
	acModel.SetType(_layout->GetAcType());
	acModel.Load(pDoc->GetInputOnboard()->GetAirportDB()->getFolder());
	_layout->UpdateDecksOutline(acModel);*/
}

void CAircraftLayoutCtrl::CreateDeckLayout2D(CRect rect, CWnd *parent)
{
	CString strClassName = AfxRegisterWndClass(CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, 0);
	CreateEx(0,strClassName,"OpenGL",WS_CLIPSIBLINGS | WS_CLIPCHILDREN| WS_CHILD | WS_VISIBLE ,rect,parent,0);

	UpdateDeckLayoutOutLine();

	int nCount = m_pDeckManager->getCount();
	for (int i = 0; i < nCount; i++)
	{
		CAircraftOpenGLCtrl* glCtrl = new CAircraftOpenGLCtrl(m_pDeckManager,m_pDeckManager->getItem(i),m_layout,m_CrewPaxList,m_bInit);
		glCtrl->Create(CRect(rect.left,(i*rect.Height())/nCount,rect.Width(),rect.Height()/nCount),this);
		addItem(glCtrl);
	}
}

void CAircraftLayoutCtrl::addItem(CAircraftOpenGLCtrl* openGLCtrl)
{
	ASSERT(openGLCtrl);
	m_vGLCtrl.push_back(openGLCtrl);
}

void CAircraftLayoutCtrl::OnUpdate()
{
	for (int i = 0; i < (int)m_vGLCtrl.size(); i++)
	{
		 m_vGLCtrl[i]->Invalidate(FALSE);
	}
}
 const CAircraftOpenGLCtrl* CAircraftLayoutCtrl::findItem(CDeck* pDeck)
 {
 	for (int i = 0; i < (int)m_vGLCtrl.size(); i++)
 	{
 		if (pDeck == m_vGLCtrl[i]->getDeck())
 		{
 			return m_vGLCtrl[i];
 		}
 	}
 	return NULL;
 }
//CAircraftOpenGLCtrl
////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CAircraftOpenGLCtrl, CView)
CAircraftOpenGLCtrl::CAircraftOpenGLCtrl(CDeckManager* pDeckManage,CDeck* pDeck,CAircaftLayOut* _layout,CabinCrewGeneration* CrewPaxList,BOOL bInit)
:m_pDC(NULL)
,m_hRC(NULL)
,m_iPixelFormat(0)
,m_dAspect(0.0)
,m_ptFrom(0,0)
,m_bLeftButtonDown(FALSE)
,m_layout(_layout)
,m_pDeck(pDeck)
,m_pCrewPostion(NULL)
,m_bInit(bInit)
,m_CrewPaxList(CrewPaxList)
,m_pDeckManager(pDeckManage)
{
	m_pSeatSet = getSeatSet(m_layout);
	m_DoorSet = &getDoorSet(m_layout);
}

CAircraftOpenGLCtrl::~CAircraftOpenGLCtrl()
{
}


BEGIN_MESSAGE_MAP(CAircraftOpenGLCtrl, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEACTIVATE()
	ON_COMMAND(ID_DELETE_POSITION,OnRemovePostion)
END_MESSAGE_MAP()

void CAircraftOpenGLCtrl::DrawPosition(CCrewPosition* pCrewPosition)
{
	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	Point pt;
	if (m_bInit)
	{
		pt = pCrewPosition->GetInitPos();
	}
	else
	{
		pt = pCrewPosition->GetFinalPoint();
	}

	glLoadName(pCrewPosition->GetID());
	glBegin(GL_POLYGON);
	glColor3f(0.4f,1.f,0.8f);
	for (GLfloat angle = 0.0f; angle <= (2.0f*GL_PI)*3.0f; angle += 0.1f)
	{
		x = static_cast<GLfloat>(pt.getX() + 50*sin(angle));
		y = static_cast<GLfloat>(pt.getY() + 50*cos(angle));
		glVertex2f(x,y);
	}
	glEnd();
}

int CAircraftOpenGLCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return 0;
}
// CAircraftOpenGLCtrl message handlers
int CAircraftOpenGLCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetupScene();
	TEXTMANAGER3D->Initialize(m_pDC);
	m_oleDropTarget.Register(this);
	return 0;
}

LRESULT CAircraftOpenGLCtrl::DrawCrewPostion(WPARAM wParam, LPARAM lParam)
{
	Point* point = (Point*)lParam;
	CPoint p = GetMessagePos();
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	return 0;
}

CSeatDataSet* CAircraftOpenGLCtrl::getSeatSet(CAircaftLayOut* _layout)
{
	ASSERT(_layout);
	CAircraftPlacements* pPlacements = _layout->GetPlacements();
	ASSERT(pPlacements);
	//CAircraftSeatsManager* pSeatManager = pPlacements->GetSeatManager();
	//ASSERT(pSeatManager);
	return pPlacements->GetSeatData();
}

CDoorDataSet& CAircraftOpenGLCtrl::getDoorSet(CAircaftLayOut* _layout)
{
	ASSERT(_layout);
	CAircraftPlacements* pPlacements = _layout->GetPlacements();
	ASSERT(pPlacements);
	return *(pPlacements->GetDoorData());
}

void CAircraftOpenGLCtrl::Create(CRect rect, CWnd *parent)
{
 	CString strClassName = AfxRegisterWndClass(CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, 0);
 	CreateEx(0,strClassName,"OpenGL",WS_CLIPSIBLINGS | WS_CLIPCHILDREN| WS_CHILD | WS_VISIBLE ,rect,parent,0);
}

void CAircraftOpenGLCtrl::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);	
	// TODO: Add your message handler code here	
	if(cy <= 0 || cx <=0)
		return;
	
	SetupViewport(cx,cy);
}

BOOL CAircraftOpenGLCtrl::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;	
}

float CAircraftOpenGLCtrl::getDeckScale(int cx,int cy)
{
	double dMaxScale = 0.0;
	int nCount = m_pDeckManager->getCount();
	for (int i = 0; i < nCount; i++)
	{
		CDeck* pDeck = m_pDeckManager->getItem(i);

		double dLength = pDeck->mRectMax[VY] - pDeck->mRectMin[VY];
		double dWidth = pDeck->mRectMax[VX] - pDeck->mRectMin[VX];
		double dScaleLen = dLength*0.7 /cy;
		double dScaleWidth = dWidth*0.7/cx;
		double dScale = MAX(dScaleLen,dScaleWidth);
		if (dMaxScale < dScale)
		{
			dMaxScale = dScale;
		}
	}
	return static_cast<float>(dMaxScale);
}

void CAircraftOpenGLCtrl::SetupViewport(int cx,int cy)
{
	if(!::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return;

	glViewport(0,0,cx,cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	double dScale = getDeckScale(cx,cy);
	
	m_dAspect = (double) cx/ (double)cy;
	gluOrtho2D(-cx*dScale,cx*dScale,-cy*dScale, cy*dScale);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int CAircraftOpenGLCtrl::SetupScene()
{
	m_pDC = new CClientDC(this);
	if(!m_pDC)
		return -1;

	if(!SetupPixelFormat(m_pDC->GetSafeHdc(), PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER /*| PFD_SWAP_COPY*/))
		return -1;

	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if(!m_hRC)
		return -1;

	if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))	{// make RC current 
		return -1;
	}
	
	return 0;
}

BOOL CAircraftOpenGLCtrl::SetupPixelFormat(HDC hDC, DWORD dwFlags)
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
			1,                              // version number
			0,			
			PFD_TYPE_RGBA,                  // RGBA type
			32,                             // 24-bit color depth
			0, 0, 0, 0, 0, 0,               // color bits ignored
			0,                              // no alpha buffer
			0,                              // shift bit ignored
			0,                              // no accumulation buffer
			0, 0, 0, 0,                     // accum bits ignored
			24,                             // 32-bit z-buffer
			1,                              // one stencil buffer
			0,                              // no auxiliary buffer
			PFD_MAIN_PLANE,                 // main layer
			0,                              // reserved
			0, 0, 0                         // layer masks ignored
	};

	pfd.dwFlags = dwFlags;

	if( !(m_iPixelFormat = ::ChoosePixelFormat(hDC, &pfd)) )
	{
		MessageBox("ChoosePixelFormat failed");
		return FALSE;
	}

	if(!SetPixelFormat(hDC, m_iPixelFormat, &pfd))
	{
		MessageBox("SetPixelFormat failed");
		return FALSE;
	}

	// TRACE("Pixel Format %d was selected\n", m_iPixelFormat);

	return TRUE;
}

void CAircraftOpenGLCtrl::RenderText()
{
	ARCVector3 vDeckPos(m_pDeck->mRectMax[VX]+20.0,(m_pDeck->mRectMin[VY]+m_pDeck->mRectMax[VY])/2,0.0);
	TEXTMANAGER3D->DrawBitmapText(m_pDeck->GetName().GetBuffer(256),vDeckPos);

	for (int i = 0; i < m_DoorSet->GetItemCount(); i++)
	{
		CDoor* pDoor = m_DoorSet->GetItem(i);
		if (pDoor->GetDeck() == m_pDeck)
		{
			ARCVector3 vWorldPos;
			vWorldPos = pDoor->GetPosition();
			TEXTMANAGER3D->DrawBitmapText(pDoor->GetIDName().GetIDString().GetBuffer(256),vWorldPos);
		}
	}
}

void CAircraftOpenGLCtrl::RenderDeck(CDeck* pDeck)
{
	ASSERT(pDeck);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat dx = static_cast<GLfloat>(-(pDeck->mRectMin[VX]+pDeck->mRectMax[VX])/2);
	GLfloat dy =  static_cast<GLfloat>(-(pDeck->mRectMin[VY]+pDeck->mRectMax[VY])/2);
	glTranslatef(dx,dy,0.0f);
	glBegin(GL_LINE_LOOP);
	glLineWidth(2.0f);
	glColor3f(0.0f,0.0f,0.0f);
	for (int i = 0; i < (int)pDeck->GetOutline().size(); i++)
	{
		ARCPoint2 point = pDeck->GetOutline().at(i);
		glVertex2f(static_cast<GLfloat>(point.n[VX]),static_cast<GLfloat>(point.n[VY]));
	}
	glEnd();


	for (int j = 0; j < m_CrewPaxList->getDataCount(); j++)
	{
		CabinCrewPhysicalCharacteristics* pCabinCrew = m_CrewPaxList->getData(j);
		ASSERT(pCabinCrew);
		CCrewPosition* pCrewPosition = pCabinCrew->GetCrewPosition();
		if (pCrewPosition->GetDeck() == pDeck)
		{
			DrawPosition(pCrewPosition);
		}
	}

	//draw seat
	for (int j = 0; j < m_pSeatSet->GetItemCount(); j++)
	{
		CSeat* pSeat = m_pSeatSet->GetItem(j);
		ASSERT(pSeat);
		if (pSeat->GetDeck() == pDeck)
		{
			RenderSeat(pSeat);
		}
	}
	//draw door
	for (int j = 0; j < m_DoorSet->GetItemCount(); j++)
	{
		CDoor* pDoor = m_DoorSet->GetItem(j);
		ASSERT(pDoor);
		if (pDoor->GetDeck() == pDeck)
		{
			RenderDoor(pDoor);
		}
	}
	glColor3f(0.0f,0.0f,0.0f);
	RenderText();
}

void CAircraftOpenGLCtrl::RenderSeat(CSeat* pSeat)
{
	ASSERT(pSeat);
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
		glColor3f(0.2f,0.2f,0.2f);
		glVertex2f(static_cast<GLfloat>(pSeat->getLeftTop().n[VX]),static_cast<GLfloat>(pSeat->getLeftTop().n[VY]));
		glVertex2f(static_cast<GLfloat>(pSeat->getRightTop().n[VX]),static_cast<GLfloat>(pSeat->getRightTop().n[VY]));
		glVertex2f(static_cast<GLfloat>(pSeat->getRightBottom().n[VX]),static_cast<GLfloat>(pSeat->getRightBottom().n[VY]));
		glVertex2f(static_cast<GLfloat>(pSeat->getLeftBottom().n[VX]),static_cast<GLfloat>(pSeat->getLeftBottom().n[VY]));
	glEnd();
}

void CAircraftOpenGLCtrl::RenderDoor(CDoor* pDoor)
{
	ASSERT(pDoor);
	glLineWidth(1.0f);
	GLfloat x = static_cast<GLfloat>(pDoor->GetPosition().n[VX]);
	GLfloat y = static_cast<GLfloat>(pDoor->GetPosition().n[VY]);
	
	glBegin(GL_QUADS);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex2f(x-50.0f,y-10.0f);
		glVertex2f(x+50.0f,y-10.0f);
		glVertex2f(x+50.0f,y+10.0f);
		glVertex2f(x-50.0f,y+10.0f);
	glEnd();
}

void CAircraftOpenGLCtrl::RenderSecen()
{	
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
  	glMatrixMode(GL_MODELVIEW);
  	glLoadIdentity();
  	glClearColor(1.0f,1.0f,1.0f,1.0f);
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderDeck(m_pDeck);
	::SwapBuffers(m_pDC->GetSafeHdc());	
}

void CAircraftOpenGLCtrl::OnDraw(CDC* pDC)
{
	RenderSecen();
	CView::OnDraw(pDC);
}

void CAircraftOpenGLCtrl::SelectScene(int x,int y)
{
	if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return;
	//init selection buf
	static GLuint selectBuf[1024];
	GLint hits;
	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(1024, selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);	

	//draw
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((GLdouble) x, (GLdouble) viewport[3]- y, 2,2, viewport);
	double dScale = getDeckScale(viewport[2],viewport[3]);
	gluOrtho2D(-viewport[2]*dScale,viewport[2]*dScale,-viewport[3]*dScale, viewport[3]*dScale);
	RenderDeck(m_pDeck);
	hits = glRenderMode(GL_RENDER);

	if (hits == 1)
	{
		m_pCrewPostion = m_CrewPaxList->getCrewPosition(selectBuf[3]);
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void CAircraftOpenGLCtrl::OnRemovePostion()
{
	if (m_pCrewPostion)
	{
		if (m_bInit)
		{
			m_pCrewPostion->SetInitPos(Point(0.0,0.0,0.0));
			m_pCrewPostion->SetInit(FALSE);
		}
		else
		{
			m_pCrewPostion->SetFinalPoint(Point(0.0,0.0,0.0));
			m_pCrewPostion->SetFinal(FALSE);
		}
		m_pCrewPostion = NULL;
	}
}

void CAircraftOpenGLCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnRButtonDown(nFlags, point);
	m_pCrewPostion = NULL;
	m_ptMousePos = GetWorldMousePos(point);
	if(::GetAsyncKeyState(VK_MENU)>=0) 
	{
		SelectScene(point.x,point.y);
	}
	CPoint pt(point);
	ClientToScreen(&pt);
	CNewMenu menu;

	SelectScene(point.x,point.y);
	if (m_pCrewPostion)
	{
		menu.LoadMenu(IDR_MENU_CREWPOSITION);
		menu.GetSubMenu(0)->EnableMenuItem(ID_DELETE_POSITION,MF_BYCOMMAND|MF_ENABLED);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,pt.x,pt.y,this); 
	}
	
	Invalidate(FALSE);
}

void CAircraftOpenGLCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnLButtonDown(nFlags, point);
	m_pCrewPostion = NULL;
	m_ptMousePos = GetWorldMousePos(point);
	m_ptFrom = point;
	m_bLeftButtonDown = TRUE;
	SetCapture();
 	if(::GetAsyncKeyState(VK_MENU)>=0) 
 	{
 		SelectScene(point.x,point.y);
 	}
	Invalidate(FALSE);
}

void CAircraftOpenGLCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnLButtonUp(nFlags, point);
	m_ptFrom = point;

	m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);
	m_bLeftButtonDown = FALSE;
	m_pCrewPostion = NULL;
	ReleaseCapture();
	Invalidate(FALSE);
}

void CAircraftOpenGLCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnMouseMove(nFlags, point);
	if(point == m_ptFrom)
		return ;

	CPoint mouseDiff = point - m_ptFrom;
	m_ptFrom = point;	

	ARCVector3 curWorldPos = GetWorldMousePos(point);
	ARCVector3 posDiff = curWorldPos - m_ptMouseWorldPos; 
	m_ptMouseWorldPos = curWorldPos;
	if (::GetAsyncKeyState(VK_MENU)>=0)
	{
		if (m_bLeftButtonDown && m_pCrewPostion)
		{
			if (m_bInit)
			{
				m_pCrewPostion->SetInitPos(Point(curWorldPos.n[VX],curWorldPos.n[VY],0.0));
			}
			else
			{
				m_pCrewPostion->SetFinalPoint(Point(curWorldPos.n[VX],curWorldPos.n[VY],0.0));
			}
		}
	}
	Invalidate(FALSE);
}

CAircraftLayoutEditContext* CAircraftOpenGLCtrl::GetEditContext() const
{
	CMainFrame* pMainFram = (CMainFrame*)(GetParent()->GetParent());
	CMDIChildWnd* pMDIActive = (CMDIChildWnd*)pMainFram->MDIGetActive();
	if (pMDIActive == NULL)
		return NULL;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();

	InputOnboard* pInputOnboard= NULL;
	if(pDoc)
		return pDoc->GetAircraftLayoutEditor();

	return NULL;

}

ARCVector3 CAircraftOpenGLCtrl::GetWorldMousePos( const CPoint& pt )
{
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	GLint realy = viewport[3] - pt.y;
	ARCVector3 pt1;
	gluUnProject((GLdouble) pt.x, (GLdouble)realy, 0.0, mvmatrix, projmatrix, viewport, &pt1[VX], &pt1[VY], &pt1[VZ]);
	::wglMakeCurrent(NULL, NULL);
	return ARCVector3(pt1.n[VX],pt1.n[VY],0.0f);
}


DROPEFFECT CAircraftOpenGLCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	RawDragDropDataItem dataItem;
	if (OleDragDropManager::GetInstance().GetRawDragDropData(dataItem, pDataObject))
	{
		m_pDragShape = *(CCrewPosition**) dataItem.GetData();
		return DROPEFFECT_COPY;
	}
	return DROPEFFECT_NONE;
}

DROPEFFECT CAircraftOpenGLCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	OnMouseMove(dwKeyState, point);

	if(m_pDragShape != NULL)
		return DROPEFFECT_COPY;
	else		
		return DROPEFFECT_NONE;
}

void CAircraftOpenGLCtrl::OnDragLeave() 
{
	CView::OnDragLeave();
	if(m_pDragShape != NULL) {
		m_pDragShape = NULL;
	}
}


BOOL CAircraftOpenGLCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CView::OnDrop(pDataObject, dropEffect, point);

	RawDragDropDataItem dataItem;
	if (OleDragDropManager::GetInstance().GetRawDragDropData(dataItem, pDataObject))
	{
		m_pDragShape = *(CCrewPosition**) dataItem.GetData();
		if (m_pDragShape)
		{
			ARCVector3 mpos = GetWorldMousePos(point);
			ARCVector3 mpos2D(mpos[VX], mpos[VY],0);
			if (m_bInit)
			{
				m_pDragShape->SetInitPos(Point(mpos[VX],mpos[VY],0.0));
				m_pDragShape->SetInit(TRUE);
			}
			else
			{
				m_pDragShape->SetFinalPoint(Point(mpos[VX],mpos[VY],0.0));
				m_pDragShape->SetFinal(TRUE);
			}
			m_pDragShape->SetDeck(m_pDeck);
			((CAircraftLayoutCtrl*)GetParent())->OnUpdate();
			return TRUE;
		}
	}
	return FALSE;
}