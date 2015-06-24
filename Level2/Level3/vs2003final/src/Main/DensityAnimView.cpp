// DensityAnimView.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "ActivityDensityData.h"
#include "ActivityDensityParams.h"
#include "AnimationTimeManager.h"
#include "PBuffer.h"
//#include "glh_extensions.h"
#include "DensityAnimView.h"
#include "ChildFrameDensityAnim.h"
#include "AFTEMSBitmap.h"
#include "Floor2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDensityAnimView

IMPLEMENT_DYNCREATE(CDensityAnimView, CView)

CDensityAnimView::CDensityAnimView()
{
	m_hMapBM = NULL;
	m_hScreenBM = NULL;
	m_pDensities = NULL;
}

CDensityAnimView::~CDensityAnimView()
{
	if(m_hMapBM != NULL) {
		::DeleteObject(m_hMapBM);
		m_hMapBM = NULL;
	}
	if(m_pDensities != NULL) {
		delete [] m_pDensities;
		m_pDensities = NULL;
	}
}


BEGIN_MESSAGE_MAP(CDensityAnimView, CView)
	//{{AFX_MSG_MAP(CDensityAnimView)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ANIMTIME, OnUpdateAnimTime)
	ON_COMMAND(ID_VIEW_PRINT, OnViewPrint)
	ON_COMMAND(ID_VIEW_EXPORT, OnViewExport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDensityAnimView drawing

void CDensityAnimView::OnDraw(CDC* pDC)
{
	CTermPlanDoc* pDoc = GetDocument();
	if(pDoc->m_pADData == NULL)
		return;
	CRect rect;
	GetClientRect(&rect);

	if(m_hMapBM == NULL) {
		GenerateFloorOverlayTexture();
	}
	ASSERT(m_hMapBM != NULL);

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

	if(nextZ < pADData->GetSizeZ()) {

		int sizeXY = pADData->GetSizeX()*pADData->GetSizeY();
		int sizeX = pADData->GetSizeX();
		int sizeY = pADData->GetSizeY();
		float* pPrevDensities = pADData->Get2DData(prevZ);
		float* pNextDensities = pADData->Get2DData(nextZ);
		if(m_pDensities == NULL) {
			m_pDensities = new float[sizeXY];
		}
		for(int i=0; i<sizeXY; i++) {
			m_pDensities[i] = static_cast<float>(d*pNextDensities[i] + (1-d)*pPrevDensities[i]);
			//pDensities[i] = (1-d)*pNextDensities[i] + d*pPrevDensities[i];
		}

		//create the dib section
		BITMAPINFO bmInfo;
		memset(&bmInfo,0,sizeof(BITMAPINFO));
		bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfo.bmiHeader.biWidth = pADData->GetSizeX();
		bmInfo.bmiHeader.biHeight = pADData->GetSizeY();
		bmInfo.bmiHeader.biPlanes = 1;
		bmInfo.bmiHeader.biBitCount = 32;
		bmInfo.bmiHeader.biCompression = BI_RGB;
		bmInfo.bmiHeader.biClrUsed = 0;
		unsigned char* pBits;
		HBITMAP hBM = CreateDIBSection(pDC->GetSafeHdc(), &bmInfo, DIB_RGB_COLORS, (void**) &pBits, NULL, 0);

		//set the bits
		for(i=0; i<sizeXY; i++) {
			//memset(pBits+i*3, pADParams->GetLOSColor(pDensities[i]), sizeof(COLORREF));
			COLORREF col;
			if(m_pDensities[i] != 0)
				col = pADParams->GetLOSColor(1/m_pDensities[i]);
			else
				col = pADParams->GetLOSColor(FLT_MAX);
			pBits[i*4] = GetBValue(col);
			pBits[i*4+1] = GetGValue(col);
			pBits[i*4+2] = GetRValue(col);
			pBits[i*4+3] = 255;
			/*
			pBits[i*4] = 255;//alpha
			pBits[i*4+1] = GetRValue(col);
			pBits[i*4+2] = GetGValue(col);
			pBits[i*4+3] = GetBValue(col);
			*/
		}


		CDC memDC;
		CDC densityBitmapDC;
		densityBitmapDC.CreateCompatibleDC(pDC);
		memDC.CreateCompatibleDC(pDC);

		HBITMAP hOldMemBM = (HBITMAP) ::SelectObject(memDC.GetSafeHdc(),m_hScreenBM);

		HBITMAP hOldBM = (HBITMAP) ::SelectObject(densityBitmapDC.GetSafeHdc(), hBM);

		//densityBitmapDC.FillSolidRect(0,0,sizeX,sizeY,RGB(255,255,255));

		memDC.FillSolidRect(0,0,rect.Width(),rect.Height(),RGB(96,96,96));

		double dDensityAspect = ((double)sizeX)/sizeY;
		double dAOIAspect = ((double) pADParams->GetAOISize()[VX])/pADParams->GetAOISize()[VY];
		double dWndAspect = ((double)rect.Width())/rect.Height();

		if(dWndAspect>dAOIAspect) {
			//window is relatively wider -> fill to height of window
			int nWidth = static_cast<int>(rect.Height()*dAOIAspect);
			VERIFY(memDC.StretchBlt(rect.CenterPoint().x-(nWidth/2),0,nWidth,rect.Height(),&densityBitmapDC,0,0,sizeX,sizeY,SRCCOPY));
			VERIFY(TransparentBlt(memDC.GetSafeHdc(),rect.CenterPoint().x-(nWidth/2),0,nWidth,rect.Height(),m_bitmapDC.GetSafeHdc(),0,0,m_sizeBM.cx,m_sizeBM.cy,RGB(255,0,255)));
		}
		else {
			//window is relatively taller -> fill to width of window
			int nHeight = static_cast<int>(rect.Width()/dAOIAspect);
			VERIFY(memDC.StretchBlt(0,rect.CenterPoint().y-(nHeight/2),rect.Width(),nHeight,&densityBitmapDC,0,0,sizeX,sizeY,SRCCOPY));
			VERIFY(TransparentBlt(memDC.GetSafeHdc(),0,rect.CenterPoint().y-(nHeight/2),rect.Width(),nHeight,m_bitmapDC.GetSafeHdc(),0,0,m_sizeBM.cx,m_sizeBM.cy,RGB(255,0,255)));
		}

		::SelectObject(densityBitmapDC.GetSafeHdc(),hOldBM);
		::DeleteObject(hBM);

		/*
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 0xff;
		bf.AlphaFormat = AC_SRC_ALPHA;
		VERIFY(::AlphaBlend(memDC.GetSafeHdc(),0,0,rect.Width(),rect.Height(),m_bitmapDC.GetSafeHdc(),0,0,m_sizeBM.cx,m_sizeBM.cy,bf));
		*/
		
		VERIFY(pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&memDC,0,0,rect.Width(),rect.Height(),SRCCOPY));
		::SelectObject(memDC.GetSafeHdc(),hOldMemBM);
		::DeleteObject(hOldMemBM);
	}

	//force status bar update
	CChildFrameDensityAnim* pFrame = (CChildFrameDensityAnim*)GetParentFrame();
	pFrame->m_wndStatusBar.OnUpdateCmdUI(pFrame, FALSE);
	/*
	//draw the dib
	CDC bitmapDC;
	bitmapDC.CreateCompatibleDC(pDC);
	HBITMAP hOldBM = (HBITMAP) ::SelectObject(bitmapDC.GetSafeHdc(), hBM);
	pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&bitmapDC,0,0,sizeX,sizeY,SRCCOPY);
	::SelectObject(bitmapDC.GetSafeHdc(),hOldBM);
	::DeleteObject(hBM);

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	::AlphaBlend(pDC->GetSafeHdc(),rect.left,rect.top,rect.Width(),rect.Height(),m_bitmapDC.GetSafeHdc(),0,0,m_sizeBM.cx,m_sizeBM.cy,bf);
	*/
	
}

/////////////////////////////////////////////////////////////////////////////
// CDensityAnimView diagnostics

#ifdef _DEBUG
void CDensityAnimView::AssertValid() const
{
	CView::AssertValid();
}

void CDensityAnimView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTermPlanDoc* CDensityAnimView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDensityAnimView message handlers

void CDensityAnimView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	//// TRACE("Activity Density View of %s activated\n", GetDocument()->GetTitle());
	Invalidate(FALSE);
	//ANIMTIMEMGR->EnrollDocument(GetDocument());
	//ANIMTIMEMGR->EnrollView(this);
}

void CDensityAnimView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
	//// TRACE("Activity Density View of %s de-activated\n", GetDocument()->GetTitle());
	//ANIMTIMEMGR->EnrollDocument(NULL);
	//ANIMTIMEMGR->EnrollView(NULL);
}

BOOL CDensityAnimView::GenerateFloorOverlayTexture()
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL,			
		PFD_TYPE_RGBA,                  // RGBA type
		32,                             // 32-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		8,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		16,                             // 32-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};

	CTermPlanDoc* pDoc = GetDocument();
	CRect rcClient;
	GetClientRect(&rcClient);
	int nTexWidth = rcClient.Width();
	int nTexHeight = rcClient.Height();

	double dWndAspect = ((double)nTexWidth)/nTexHeight;

	CActivityDensityParams* pADParams = &(pDoc->m_adParams);
	CActivityDensityData* pADData = pDoc->m_pADData;
	ARCVector2 vC(pADParams->GetAOICenter());
	ARCVector2 vSize(pADParams->GetAOISize());

	double dAspect = vSize[VX]/vSize[VY];

	if(dWndAspect>=dAspect) {
		//window is relatively wider
		nTexHeight = static_cast<int>(nTexWidth/dAspect);
	}
	else {
		//aoi is relatively wider (window is taller)
		nTexWidth = static_cast<int>(nTexHeight*dAspect);
	}


	//create the dib section
	if(m_hMapBM != NULL) {
		::DeleteObject(m_hMapBM);
		m_hMapBM = NULL;
	}
	ASSERT(m_hMapBM == NULL);

	m_sizeBM = CSize(nTexWidth, nTexHeight);
	BITMAPINFO bmInfo;
	memset(&bmInfo,0,sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = nTexWidth;
	bmInfo.bmiHeader.biHeight = nTexHeight;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biClrUsed = 0;
	unsigned char* pBits;
	CDC* pDC = GetDC();
	ASSERT(pDC != NULL);
	//CDC bitmapDC;
	//bitmapDC.CreateCompatibleDC(pDC);
	m_bitmapDC.CreateCompatibleDC(pDC);
	m_hMapBM = CreateDIBSection(m_bitmapDC.GetSafeHdc(), &bmInfo, DIB_RGB_COLORS, (void**) &pBits, NULL, 0);
	HBITMAP hOldBM = (HBITMAP) ::SelectObject(m_bitmapDC.GetSafeHdc(), m_hMapBM);

	int pixFormat;
	if( !(pixFormat = ::ChoosePixelFormat(m_bitmapDC, &pfd)) )
	{
		MessageBox("ChoosePixelFormat failed");
		return FALSE;
	}

	if(!SetPixelFormat(m_bitmapDC, pixFormat, &pfd))
	{
		MessageBox("SetPixelFormat failed");
		return FALSE;
	}

	HGLRC hRC = wglCreateContext(m_bitmapDC);
	if(!hRC)
		return FALSE;

	if(! ::wglMakeCurrent(m_bitmapDC, hRC))	{// make RC current 
		return FALSE;
	}

	double worldL,worldR,worldB,worldT;
	worldL = vC[VX] - 0.5*vSize[VX];
	worldR = vC[VX] + 0.5*vSize[VX];
	worldT = vC[VY] + 0.5*vSize[VY];
	worldB = vC[VY] - 0.5*vSize[VY];

	ASSERT(pADParams->GetAOIFloor() < static_cast<int>(pDoc->GetCurModeFloor().m_vFloors.size()));
	CFloor2* pFloor = pDoc->GetCurModeFloor().m_vFloors[pADParams->GetAOIFloor()];
	ASSERT(pFloor != NULL);
	if(pFloor->IsCADLoaded() && pFloor->IsCADLayersLoaded() && !pFloor->IsCADProcessed()) {
		//process
		VERIFY(pFloor->ProcessCAD());
	}

	////////////////////////
	GLfloat lightAmbient[] =	{ 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat lightDiffuse[] =	{ 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat lightPosition[] =	{ 0.0f, 0.0f, 2.0f, 0.0f };

	glEnable(GL_DEPTH_TEST);				// enable depth testing
	glShadeModel(GL_SMOOTH);				// enable smooth shading
	glEnable(GL_BLEND);						// for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_NORMALIZE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);		// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);		// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION,lightPosition);	// Position The Light
	glEnable(GL_LIGHT1);								// Enable Light One

	glEnable(GL_COLOR_MATERIAL);

	glViewport(0,0,nTexWidth,nTexHeight);

	//clear the background
	glClearColor( 1.0f, 0.0f, 1.0f, 1.0f );
	//glClearColor( 0.8f, 0.6f, 0.6f, 1.0f );
	glClearDepth(1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//apply view projection and transform
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(worldL, worldR, worldB, worldT, -1, 5000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*
	C3DMath::CVector2D mapoffset = pFloor->MapOffset();
	//translate
	glTranslated(pFloor->MapOffset()[X3D], pFloor->MapOffset()[Y3D], 0.0);
	//rotate
	//glRotated(m_CadFile.dRotation, 0.0, 0.0, 1.0);
	//scale
	double dScale = pFloor->MapScale();
	glScaled(dScale,dScale,dScale);
	C3DMath::CVector2D flooroffset = pFloor->GetFloorOffset();
	glTranslated(pFloor->GetFloorOffset()[X2D], pFloor->GetFloorOffset()[Y2D], 0.0);
	glRotated(pFloor->MapRotation(), 0.0, 0.0, 1.0);
	
	//draw layers
	CFloor2::CFloorLayerList* pFloorLayers =  pFloor->GetFloorLayers();
	for(int _i=0; _i<pFloorLayers->size(); _i++) {
		(pFloorLayers->at(_i))->DrawOGL();
	}
	*/
	ARCVector2 mapoffset(pFloor->MapOffset());
	ARCVector2 flooroffset(pFloor->GetFloorOffset());
	double dScale = pFloor->MapScale();
	glPushMatrix();
	glTranslated(mapoffset[VX]+flooroffset[VX], mapoffset[VY]+flooroffset[VY], 0.0);
	glRotated(pFloor->MapRotation(), 0.0, 0.0, 1.0);
	glScaled(dScale,dScale,dScale);
	//draw layers
	//glEnable(GL_LIGHTING);
	glDisable(GL_LIGHTING);
	CCADFileContent::CFloorLayerList* pFloorLayers =  pFloor->GetFloorLayers();
	for(int _i=0; _i<static_cast<int>(pFloorLayers->size()); _i++) {
		(pFloorLayers->at(_i))->DrawOGL();
	}
	glPopMatrix();
	/*
	//glDisable(GL_LIGHTING);
	glDisable(GL_LIGHTING);
	
	//draw grid
	CFloor2::CGrid* pGrid = pFloor->GetGrid();
	if(pGrid->bVisibility) {
		glDepthMask(FALSE);
		//1) draw lines & subdivs
		double i;
		glBegin(GL_LINES);
		if(pGrid->nSubdivs > 0) {
			COLORREF col = pGrid->cSubdivsColor;
			glColor4ub(GetRValue(col),GetGValue(col),GetBValue(col), 255);
			double step = pGrid->dLinesEvery/(pGrid->nSubdivs+1);
			for(i=step; i<=pGrid->dSize; i+=step) {
				glVertex3d(i, pGrid->dSize, 0.0);
				glVertex3d(i, -pGrid->dSize, 0.0);
				glVertex3d(-i, pGrid->dSize, 0.0);
				glVertex3d(-i, -pGrid->dSize, 0.0);
				glVertex3d(pGrid->dSize, i, 0.0);
				glVertex3d(-pGrid->dSize, i, 0.0);
				glVertex3d(pGrid->dSize, -i, 0.0);
				glVertex3d(-pGrid->dSize, -i, 0.0);
			}
		}
		COLORREF col = pGrid->cLinesColor;
		glColor4ub(GetRValue(col),GetGValue(col),GetBValue(col), 255);
		for(i=pGrid->dLinesEvery; i<=pGrid->dSize; i+=pGrid->dLinesEvery) {
			glVertex3d(i, pGrid->dSize, 0.0);
			glVertex3d(i, -pGrid->dSize, 0.0);
			glVertex3d(-i, pGrid->dSize, 0.0);
			glVertex3d(-i, -pGrid->dSize, 0.0);
			glVertex3d(pGrid->dSize, i, 0.0);
			glVertex3d(-pGrid->dSize, i, 0.0);
			glVertex3d(pGrid->dSize, -i, 0.0);
			glVertex3d(-pGrid->dSize, -i, 0.0);
		}
		glEnd();
		//2) draw axes
		col = pGrid->cAxesColor;
		//glColor4ub(GetRValue(col),GetGValue(col),GetBValue(col),255);
		glColor4ub(0,0,0,255);
		glBegin(GL_LINES);
		glVertex3d(0.0, pGrid->dSize, 0.0);
		glVertex3d(0.0, -pGrid->dSize, 0.0);
		glVertex3d(pGrid->dSize, 0.0, 0.0);
		glVertex3d(-pGrid->dSize, 0.0, 0.0);
		glEnd();
		glDepthMask(TRUE);
	}

	*/
	
	glFinish();
	wglMakeCurrent( NULL, NULL );
	// delete the memory rendering context
	wglDeleteContext(hRC);


	return TRUE;
}

int CDensityAnimView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	CDC* pScreenDC = GetDC();
	GetClientRect(&rcClient);
	if(m_hScreenBM == NULL)
		m_hScreenBM = ::CreateCompatibleBitmap(pScreenDC->GetSafeHdc(),rcClient.Width(),rcClient.Height());
	ASSERT(m_hScreenBM != NULL);

	ANIMTIMEMGR->EnrollDAView(this);
	
	return 0;
}

BOOL CDensityAnimView::OnEraseBkgnd(CDC* pDC) 
{
	// tell Windows not to erase the background	
	return TRUE;
}

void CDensityAnimView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	CDC* pScreenDC = GetDC();

	if(m_hScreenBM != NULL)
		::DeleteObject(m_hScreenBM);
	m_hScreenBM = ::CreateCompatibleBitmap(pScreenDC->GetSafeHdc(),cx,cy);
	ASSERT(m_hScreenBM != NULL);
	
}

void CDensityAnimView::OnDestroy() 
{

	ANIMTIMEMGR->EnrollDAView(NULL);

	CView::OnDestroy();
	
	if(m_hScreenBM != NULL)
		::DeleteObject(m_hScreenBM);
	m_hScreenBM = NULL;
	
}

void CDensityAnimView::OnUpdateAnimTime(CCmdUI *pCmdUI)
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

void CDensityAnimView::OnViewPrint()
{
	CDC dc;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() == IDCANCEL )
		return;
	dc.Attach( printDlg.GetPrinterDC() );
	dc.m_bPrinting = TRUE;
	CString sTitle;
	sTitle = "View";
	DOCINFO di;
	::ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof( DOCINFO );
	di.lpszDocName = sTitle;
	
	BOOL bPrintingOK = dc.StartDoc( &di );
	
	CPrintInfo info;
	info.m_rectDraw.SetRect( 0,0,dc.GetDeviceCaps( HORZRES ),dc.GetDeviceCaps( VERTRES ));
	
	OnBeginPrinting( &dc, &info );
	Invalidate(FALSE);
	OnPrint( &dc, &info );
	bPrintingOK = TRUE;
	OnEndPrinting( &dc, &info );
	
	if( bPrintingOK )
		dc.EndDoc();
	else
		dc.AbortDoc();
	dc.Detach();
}

void CDensityAnimView::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
	//get bitmap of view
	CBitmap bmp;
	CAFTEMSBitmap::SaveWndToBmp(bmp,this);

	//print the bitmap
	// Create a compatible memory DC
	CDC memDC;
	memDC.CreateCompatibleDC( pDC );
	memDC.SelectObject( &bmp );
	
	BITMAP bm;
	bmp.GetBitmap( &bm );

	CRect r = pInfo->m_rectDraw;

	double dBMAspect = ((double)bm.bmWidth)/bm.bmHeight;
	double dPAspect = ((double)r.Width())/r.Height();
	if(dBMAspect > dPAspect) {
		//bm relatively wider
		pDC->StretchBlt(0, 0, r.Width(), static_cast<int>(r.Width()/dBMAspect), &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	}
	else {
		pDC->StretchBlt(0, 0, static_cast<int>(r.Height()*dBMAspect), r.Height(), &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	}
	
	//pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &memDC, 0, 0,SRCCOPY);
	memDC.DeleteDC();
	
}

void CDensityAnimView::OnViewExport()
{
	//get file name
	CFileDialog fileDlg(FALSE, _T("bmp"), _T("*.bmp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), NULL);
	if(fileDlg.DoModal() == IDOK) 
	{
		Invalidate(FALSE);
		//get bitmap of view & save bitmap to disk
		CAFTEMSBitmap::WriteWindowToDIB((LPCTSTR)fileDlg.GetPathName(), this);
	}
}