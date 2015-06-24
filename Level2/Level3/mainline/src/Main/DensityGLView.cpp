// DensityGLView.cpp: implementation of the CDensityGLView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DensityGLView.h"
#include "termplan.h"
#include "3DView.h"
#include "TermPlanDoc.h"
#include "Floor2.h"
#include "AnimationTimeManager.h"
#include "ActivityDensityData.h"
#include "ActivityDensityParams.h"
#include "3DCamera.h"
//#include "glh_extensions.h"
#include "common\ARCColor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDensityGLView

IMPLEMENT_DYNCREATE(CDensityGLView, CView)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDensityGLView::CDensityGLView()
{
	m_bBusy = FALSE;
	m_iPixelFormat = 0;
	m_pCamera = NULL;
	m_pCellColors = NULL;
}

CDensityGLView::~CDensityGLView()
{
	delete [] m_pCellColors;
	m_pCellColors = NULL;
}

BEGIN_MESSAGE_MAP(CDensityGLView, CView)
	//{{AFX_MSG_MAP(CDensityGLView)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_PRINT, OnViewPrint)
	ON_COMMAND(ID_VIEW_EXPORT, OnViewExport)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ANIMTIME, OnUpdateAnimTime)
END_MESSAGE_MAP()


void CDensityGLView::OnDraw(CDC* pDC)
{
	if(m_bBusy)
		return;

	CTermPlanDoc* pDoc = GetDocument();
	if(pDoc->m_pADData == NULL)
		return;

	m_bBusy = TRUE;

	CActivityDensityParams* pADParams = &(pDoc->m_adParams);
	CActivityDensityData* pADData = pDoc->m_pADData;
	AnimationData* pAnimData = &(pDoc->m_animData);


	//get time fraction
	double dT = ((double) (pAnimData->nLastAnimTime - pAnimData->nAnimStartTime)) / (pAnimData->nAnimEndTime - pAnimData->nAnimStartTime);
	double dZ = dT*pADData->GetSizeZ();
	int prevZ = static_cast<int>(floor(dZ));
	int nextZ = static_cast<int>(ceil(dZ));
	// TRACE("prevZ: %d, nextZ: %d\n", prevZ, nextZ);
	//calc interpolation fraction
	double d = (dZ-prevZ)/1.0;
	// TRACE("d = %.2f\n",d);

	int sizeXY = pADData->GetSizeX()*pADData->GetSizeY();
	int sizeX = pADData->GetSizeX();
	int sizeY = pADData->GetSizeY();

	if(nextZ < pADData->GetSizeZ()) {

		int sizeXY = pADData->GetSizeX()*pADData->GetSizeY();
		int sizeX = pADData->GetSizeX();
		int sizeY = pADData->GetSizeY();

		float* pPrevDensities = pADData->Get2DData(prevZ);
		float* pNextDensities = pADData->Get2DData(nextZ);
		if(m_pCellColors == NULL) {
			m_pCellColors = new ARCColor4[sizeXY];
		}
		float fDensity = 0;
		for(int i=0; i<sizeXY; i++) {
			fDensity = static_cast<float>(d*pNextDensities[i] + (1-d)*pPrevDensities[i]);
			if(fDensity != 0)
				m_pCellColors[i] = pADParams->GetLOSColor(1/fDensity);
			else
				m_pCellColors[i] = pADParams->GetLOSColor(FLT_MAX);

			m_pCellColors[i][ALPHA] = 128;
		}

		const ARCVector2& vAOICenter = pADParams->GetAOICenter();
		const ARCVector2& vAOISize = pADParams->GetAOISize();
		const ARCVector2& vBLCorner = vAOICenter - 0.5*vAOISize;

		const float stepX = static_cast<float>(vAOISize[VX]/sizeX);
		const float stepY = static_cast<float>(vAOISize[VY]/sizeY);

		//Make RC current
		if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC)) {
			ASSERT(FALSE);
			return;
		}

		//clear to background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		m_pCamera->ApplyGLProjection(m_dAspect);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		m_pCamera->ApplyGLView();

		/////////////////////////////////
		//render scene

		glDisable(GL_DEPTH_TEST);
		//render active floor
		glPushMatrix();
		C3DView * pView = GetDocument()->Get3DView();
		pDoc->GetCurModeFloor().m_vFloors[pDoc->m_nActiveFloor]->DrawOGL(pView, 0.0);
		glPopMatrix();

		if(nextZ < pADData->GetSizeZ()) {
			glBegin(GL_QUADS);
			for(int j=0; j<sizeY; j++) {
				for(int i=0; i<sizeX; i++) {
					glColor4ubv(m_pCellColors[i+sizeX*j]);
					glVertex2f(static_cast<float>(vBLCorner[VX]+i*stepX), static_cast<float>(vBLCorner[VY]+j*stepY));
					glVertex2f(static_cast<float>(vBLCorner[VX]+(i+1)*stepX), static_cast<float>(vBLCorner[VY]+j*stepY));
					glVertex2f(static_cast<float>(vBLCorner[VX]+(i+1)*stepX), static_cast<float>(vBLCorner[VY]+(j+1)*stepY));
					glVertex2f(static_cast<float>(vBLCorner[VX]+i*stepX), static_cast<float>(vBLCorner[VY]+(j+1)*stepY));
				}
			}
			glEnd();
		}

		glEnable(GL_DEPTH_TEST);
		/////////////////////////////////

		//swap buffers (done)
		::SwapBuffers(m_pDC->GetSafeHdc());

	}
	else {

		//draw blank screen
		if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC)) {
			ASSERT(FALSE);
			return;
		}

		//clear to background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//swap buffers (done)
		::SwapBuffers(m_pDC->GetSafeHdc());
	}

	m_bBusy = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CDensityGLView diagnostics

#ifdef _DEBUG
void CDensityGLView::AssertValid() const
{
	CView::AssertValid();
}

void CDensityGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTermPlanDoc* CDensityGLView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDensityGLView message handlers

void CDensityGLView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	Invalidate(FALSE);
}

void CDensityGLView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
}

int CDensityGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pDC = new CClientDC(this);
	if(!m_pDC)
		return -1;
	
	if(!SetupPixelFormat(m_pDC->GetSafeHdc(), PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER /*| PFD_SWAP_COPY*/))
		return -1;
	
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if(!m_hRC)
		return -1;

	if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))	{// make RC current 
		return FALSE;
	}

	glewInit();
	// Get the entry points for the extensions.
	/*if(!glh_init_extensions(" WGL_ARB_pbuffer "
							" WGL_ARB_pixel_format ")) {
		
		AfxMessageBox("Fatal Error: Essential OpenGL extensions (WGL_ARB_pbuffer & WGL_ARB_pixel_format) are not supported by your drivers.", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	if( !glh_init_extensions(" GL_NV_texture_rectangle ")) {
		if( !glh_init_extensions(" GL_EXT_texture_rectangle ")) {
			AfxMessageBox("Fatal Error: A critical OpenGL extension is not supported by your drivers.", MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}
	}*/

	InitOGL(); // Initialize OpenGL

	//create 2D camera
	m_pCamera = new C3DCamera(C3DCamera::parallel);
	
	//init default camera
	CTermPlanDoc* pDoc = GetDocument();
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);
	const ARCVector2& vAOICenter = pADParams->GetAOICenter();
	const ARCVector2& vAOISize = pADParams->GetAOISize();

	m_pCamera->SetCamera(ARCVector3(vAOICenter,10000),ARCVector3(0,0,-1), ARCVector3(0,1,0) );
	m_pCamera->AutoAdjustCameraFocusDistance(0);

	ANIMTIMEMGR->EnrollDAView(this);

	//share lists
	C3DView* p3DView = GetDocument()->Get3DView();
	if(p3DView != NULL) {
		::wglShareLists(p3DView->GetHGLRC(), m_hRC);
	}
	
	return 0;
}

BOOL CDensityGLView::OnEraseBkgnd(CDC* pDC) 
{
	// tell Windows not to erase the background	
	return TRUE;
}

void CDensityGLView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if(cy <= 0 || cx <=0)
		return;
	m_dAspect = (double) cx / (double) cy;
	SetupViewport(cx, cy);
}

void CDensityGLView::OnDestroy() 
{
	delete m_pCamera;
	m_pCamera = NULL;

	ANIMTIMEMGR->EnrollDAView(NULL);

	CView::OnDestroy();

	wglMakeCurrent(NULL, NULL);		// make RC non-current
	delete m_pDC;
	m_pDC = NULL;
	wglDeleteContext(m_hRC);		// delete RC
	m_hRC = NULL;
}

void CDensityGLView::OnViewPrint()
{
}

void CDensityGLView::OnViewExport()
{
}

BOOL CDensityGLView::SetupViewport(int cx, int cy)
{
	if(!::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return FALSE;
	glViewport(0, 0, cx, cy);
	::wglMakeCurrent(NULL, NULL);
	return TRUE;
}

BOOL CDensityGLView::SetupPixelFormat(HDC hDC, DWORD dwFlags)
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
		0,                              // no stencil buffer
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

	// TRACE("Pixel Format %d was selected for Density View\n", m_iPixelFormat);

	return TRUE;
}

BOOL CDensityGLView::InitOGL()
{
	GLfloat lightAmbient[] =	{ 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat lightDiffuse[] =	{ 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat lightPosition[] =	{ 0.0f, 0.0f, 2.0f, 0.0f };

	glClearColor(ARCColor3::GREY_BACKGROUND_FLOAT, ARCColor3::GREY_BACKGROUND_FLOAT, ARCColor3::GREY_BACKGROUND_FLOAT, 1.0f);	// specify gray as clear color
	glClearDepth(1.0f);						// back of buffer is clear depth

	glEnable(GL_DEPTH_TEST);				// enable depth testing
	glShadeModel(GL_SMOOTH);				// enable smooth shading
	glEnable(GL_BLEND);						// for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_NORMALIZE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);		// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);		// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION,lightPosition);	// Position The Light
	glEnable(GL_LIGHT1); // Enable Light One

	glEnable(GL_COLOR_MATERIAL);

	return TRUE;
}

void CDensityGLView::OnUpdateAnimTime(CCmdUI *pCmdUI)
{
	if(GetDocument()->m_eAnimState != CTermPlanDoc::anim_none) {
		long nTime = GetDocument()->m_animData.nLastAnimTime;
		int nHour = nTime / 360000;
		int nMin = (nTime - nHour*360000)/6000;
		int nSec = (nTime - nHour*360000 - nMin*6000)/100;
		CString s(_T(""));
		s.Format("\t%02d:%02d:%02d", nHour, nMin, nSec);
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}

void CDensityGLView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(point == m_ptFrom)
		return;

	CPoint _diff = point - m_ptFrom;
	ARCVector2 diff(_diff.x, _diff.y);
	diff[VY] = -diff[VY];
	m_ptFrom = point;
	
	if(::GetAsyncKeyState(VK_MENU)<0) {//ALT is pressed
		if(nFlags&MK_LBUTTON) {//L-button = pan
			diff = 2*diff;
			m_pCamera->Pan(diff);
			m_pCamera->AutoAdjustCameraFocusDistance(0.0);
			Invalidate(FALSE);
		}
		else if(nFlags&MK_RBUTTON) {//R-button = zoom
			m_pCamera->Zoom(2*diff[VX]);
			m_pCamera->AutoAdjustCameraFocusDistance(0.0);
			Invalidate(FALSE);
		}
		else if(nFlags&MK_MBUTTON) {//M-button = tumble
			diff = 2*diff;
			m_pCamera->Tumble(diff);
			Invalidate(FALSE);
		}
	}
}