#include "stdafx.h"
#include "FloorPictureCtrl.h"
#include "../Common/ARCMathCommon.h"
#include "../Common/fileman.h"
#include "ShapeRenderer.h"
#include <Il/il.h>
#include <Il/ilut.h>
#include <CXImage/ximage.h>
#define  DEFAULT_SIZE 1024*1024

IMPLEMENT_DYNAMIC(CFloorPictureCtrl, CWnd)

CFloorPictureCtrl::CFloorPictureCtrl()
:m_strPicturePath(_T(""))
,m_dWidth(0.0f)
,m_dHeight(0.0f)
,m_offsetX(0.0f)
,m_offsetY(0.0f)
{
	m_pImage = NULL;
	m_bPicTextureValid = FALSE;
	m_bPicLoaded = FALSE;
	m_bReferenceLine = FALSE;
	m_bLeftButtonDown = FALSE;
	m_iPicTexture = -1;
	m_bReferLine = FALSE;
	m_bScale = FALSE;
	m_bPan = FALSE;
	m_bMove = FALSE;

	m_vLocation[VX] = 0.0;
	m_vLocation[VY] = 0.0;
	m_vLocation[VZ] = 150.0;
	m_vLook = -m_vLocation;
	m_vLook.Normalize();
	m_vUp = ARCVector3(1.0, 0.0, 0.0) ^ m_vLook;
	m_vUp.Normalize();
	m_dFocusDistance = m_vLocation.Magnitude();
}

CFloorPictureCtrl::~CFloorPictureCtrl()
{
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}
BEGIN_MESSAGE_MAP(CFloorPictureCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

int CFloorPictureCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetupScene();
	return 0;
}

void CFloorPictureCtrl::OnDestroy()
{
	wglMakeCurrent(m_pDC->GetSafeHdc(),m_hRC);
	if (glIsTexture(m_iPicTexture))
	{
		glDeleteTextures(1, &m_iPicTexture);
	}

	CWnd::OnDestroy();
	wglMakeCurrent(NULL, NULL);		// make RC non-current
	delete m_pDC;
	m_pDC = NULL;

	wglDeleteContext(m_hRC);		// delete RC
}
void CFloorPictureCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);	
	// TODO: Add your message handler code here	
	if(cy <= 0 || cx <=0)
		return;

	m_dAspect = (double)cx/(double)cy;
	SetupViewport(cx,cy);
}

void CFloorPictureCtrl::Create(CRect rect, CWnd *parent)
{
	CString strClassName = AfxRegisterWndClass(CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, 0);
	CreateEx(0,strClassName,"OpenGL",WS_CLIPSIBLINGS | WS_CLIPCHILDREN| WS_CHILD | WS_VISIBLE ,rect,parent,0);
}

void CFloorPictureCtrl::SetPoint(ARCVector2 pt)
{
	m_pt = pt;
}

void CFloorPictureCtrl::AutoAdjustCameraFocusDistance(double dFocusPlaneAlt)
{
	if(m_vLook[VZ] == 0.0)
		return;

	double res = m_vLook.Magnitude()*(dFocusPlaneAlt-m_vLocation[VZ])/m_vLook[VZ];
	m_dFocusDistance = res;
}

void CFloorPictureCtrl::SetPath(Path path)
{
	m_refLinePath = path;
	m_bReferLine = TRUE;
}
void CFloorPictureCtrl::SetupViewport(int cx,int cy)
{
	if(!::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return;

	glViewport(0,0,cx,cy);
}

int CFloorPictureCtrl::SetupScene()
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

BOOL CFloorPictureCtrl::SetupPixelFormat(HDC hDC, DWORD dwFlags)
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

BOOL CFloorPictureCtrl::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;	
}

void CFloorPictureCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	RenderSecen();
	CWnd::OnPaint();
}

void CFloorPictureCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags,point);
	m_ptFrom = point;
	m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);
	if (m_bReferenceLine && m_PointList.size() >= 2 && ::GetAsyncKeyState(VK_MENU) >= 0)
	{
		m_bLeftButtonDown = FALSE;
		m_bReferenceLine = FALSE;
		Point head, tail;
		head.setPoint(m_PointList.at(0).n[VX]-m_offsetX,m_PointList.at(0).n[VY]-m_offsetY,0.0);
		tail.setPoint(m_PointList.at(1).n[VX]-m_offsetX,m_PointList.at(1).n[VY]-m_offsetY,0.0);
		Point ptList[2];
		ptList[0] = head;
		ptList[1] = tail;
		m_refLine.init(2,ptList);

		Point refHead, refTail;
		refHead.setPoint(m_PointList.at(0).n[VX]-m_offsetX,m_dHeight - (m_PointList.at(0).n[VY]-m_offsetY),0.0);
		refTail.setPoint(m_PointList.at(1).n[VX]-m_offsetX,m_dHeight - (m_PointList.at(1).n[VY]-m_offsetY),0.0);
		Point ptReList[2];
		ptReList[0] = refHead;
		ptReList[1] = refTail;
		m_refLinePath.init(2,ptReList);

		m_PointList.clear();
		GetParent()->SendMessage(PICKFROMMAP,NULL,NULL);
	}

	if (!m_bReferenceLine)
	{
		m_bLeftButtonDown = FALSE;
	}
	ReleaseCapture();
	Invalidate(FALSE);
}


ARCVector3 CFloorPictureCtrl::GetWorldMousePos(const CPoint& pt )
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


void CFloorPictureCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags,point);
	m_bLeftButtonDown = TRUE;
	m_ptMousePos = GetWorldMousePos(point);
	m_ptFrom = point;
	if (m_bReferenceLine && m_PointList.size() < 2)
	{
		ARCVector2 pt(m_ptMousePos.n[VX],m_ptMousePos.n[VY]);
		m_PointList.push_back(pt);
	}
	SetCapture();

	Invalidate(FALSE);
}

void CFloorPictureCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags,point);
	m_bLeftButtonDown = TRUE;
	m_ptMousePos = GetWorldMousePos(point);
	m_ptFrom = point;
	if (m_bReferenceLine && m_PointList.size() < 2)
	{
		ARCVector2 pt(m_ptMousePos.n[VX],m_ptMousePos.n[VY]);
		m_PointList.push_back(pt);
	}
	SetCapture();
	Invalidate(FALSE);
}

void CFloorPictureCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
	m_ptFrom = point;
	m_ptMouseWorldPos = GetWorldMousePos(m_ptFrom);
	if (m_bReferenceLine && m_PointList.size() >= 2 && ::GetAsyncKeyState(VK_MENU) >= 0)
	{
		m_bLeftButtonDown = FALSE;
		m_bReferenceLine = FALSE;
		Point head, tail;
		head.setPoint(m_PointList.at(0).n[VX]-m_offsetX,m_PointList.at(0).n[VY]-m_offsetY,0.0);
		tail.setPoint(m_PointList.at(1).n[VX]-m_offsetX,m_PointList.at(1).n[VY]-m_offsetY,0.0);
		Point ptList[2];
		ptList[0] = head;
		ptList[1] = tail;
		m_refLine.init(2,ptList);

		Point refHead, refTail;
		refHead.setPoint(m_PointList.at(0).n[VX]-m_offsetX,m_dHeight - (m_PointList.at(0).n[VY]-m_offsetY),0.0);
		refTail.setPoint(m_PointList.at(1).n[VX]-m_offsetX,m_dHeight - (m_PointList.at(1).n[VY]-m_offsetY),0.0);
		Point ptReList[2];
		ptReList[0] = refHead;
		ptReList[1] = refTail;
		m_refLinePath.init(2,ptReList);

		m_PointList.clear();
		GetParent()->SendMessage(PICKFROMMAP,NULL,NULL);
	}

	if (!m_bReferenceLine)
	{
		m_bLeftButtonDown = FALSE;
	}
	ReleaseCapture();
	Invalidate(FALSE);
}

void CFloorPictureCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags,point);
	if(point == m_ptFrom)
		return ;

	CPoint mouseDiff = point - m_ptFrom;
	m_ptFrom = point;
	ARCVector3 curWorldPos = GetWorldMousePos(point);
	ARCVector3 posDiff = curWorldPos - m_ptMouseWorldPos; 
	m_ptMouseWorldPos = curWorldPos;

	if((::GetAsyncKeyState(VK_MENU)<0&&(nFlags&MK_LBUTTON)) 
		||(m_bPan&&(nFlags&MK_LBUTTON)))
	{
		m_pt.DoOffset(posDiff.n[VX],posDiff.n[VY]);
		m_offsetX = m_pt.x - m_dWidth/2;
		m_offsetY = m_pt.y - m_dHeight/2;
		m_bPan = TRUE;
	}
	else if((::GetAsyncKeyState(VK_MENU)<0&&(nFlags&MK_RBUTTON))
		||(m_bScale&&(nFlags&MK_LBUTTON)))
	{
		double dAdjFact = m_dFocusDistance/50.0;
		double nOffsetX = 2*mouseDiff.x;
		m_vLocation = m_vLocation + dAdjFact*nOffsetX*m_vLook;
		AutoAdjustCameraFocusDistance(0.0);
	}

	if (m_bReferenceLine && m_bLeftButtonDown)
	{
		Point head, tail;
		if ((m_bPan||m_bScale||m_bMove) && m_refLine.getCount() > 0)
		{
			ARCVector2 pt;
			pt.n[VX] = m_refLine.getPoint(0).getX() + m_offsetX;
			pt.n[VY] = m_refLine.getPoint(0).getY() + m_offsetY;
			m_PointList.clear();
			m_PointList.push_back(pt);
		}

		head.setPoint(m_PointList.at(0).n[VX]-m_offsetX,m_PointList.at(0).n[VY]-m_offsetY,0.0);
		tail.setPoint(curWorldPos.x-m_offsetX,curWorldPos.y-m_offsetY,0.0);
		Point ptList[2];
		ptList[0] = head;
		ptList[1] = tail;
		m_refLine.init(2,ptList);
		m_bScale = FALSE;
		m_bPan = FALSE;
		m_bMove = FALSE;
	}
	Invalidate(FALSE);
}

void CFloorPictureCtrl::RenderSecen()
{	
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	double dZoomFact = m_dFocusDistance/20;
	glOrtho(-40*dZoomFact*m_dAspect,40*dZoomFact*m_dAspect,-40*dZoomFact,40*dZoomFact,0,m_dFocusDistance);
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawPicture();
	DrawReferenceLine();
	::SwapBuffers(m_pDC->GetSafeHdc());	
}

void CFloorPictureCtrl::DrawReferenceLine()
{
	if (m_refLine.getCount() == 2)
	{
		m_offsetX = m_pt.x - m_dWidth/2;
		m_offsetY = m_pt.y - m_dHeight/2;
		Path path = m_refLine;
		path.DoOffset(m_offsetX,m_offsetY,0.0);

		glColor3d(255,0,128);
		ARCGLRender::DrawWidthPath(path,5);
	}
}

void CFloorPictureCtrl::DrawPicture()
{
	if(  !m_bPicLoaded )
	{
		m_bPicLoaded = TRUE;
		GenPicTexture();
	}

	if( m_bPicTextureValid)
	{
		GLenum texTarget = GL_TEXTURE_2D;
		glEnable(texTarget);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glColor4ubv(ARCColor4::VISIBLEREGION);
		glBindTexture(texTarget, m_iPicTexture);		
		glTexParameteri(texTarget,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(texTarget,GL_TEXTURE_WRAP_T,GL_CLAMP);

		glPushMatrix();

		glBegin(GL_POLYGON);
		glTexCoord2d(0,0);
		glVertex3d(m_pt.x-m_dWidth/2,m_pt.y-m_dHeight/2,0.0);
		glTexCoord2d(1,0);
		glVertex3d(m_pt.x+m_dWidth/2,m_pt.y-m_dHeight/2,0.0);
		glTexCoord2d(1,1);
		glVertex3d(m_pt.x+m_dWidth/2,m_pt.y+m_dHeight/2,0.0);
		glTexCoord2d(0,1);
		glVertex3d(m_pt.x-m_dWidth/2,m_pt.y+m_dHeight/2,0.0);
		glEnd();

		glPopMatrix();
		glDisable(texTarget);
	}
}

GLenum getGLFormatEx(BYTE b)
{
	if (b == 2)
	{
		return GL_RGB8;
	}
	else if (b == 4) {
		return GL_RGBA8;
	}

	return b;
}

GLenum getGLFormat(BYTE b)
{
	if (b == 2)
	{
		return GL_RGB;
	}
	else if (b == 4) {
		return GL_RGBA;
	}

	return b;
}

BOOL CFloorPictureCtrl::LoadImage(CString sFileName)
{
	CString ext(FindExtension(sFileName));
	ext.MakeLower();
	if (ext == _T("")) return FALSE;

	if(FileManager::IsFile(sFileName.GetBuffer()) == 0)
		return FALSE;

	int type = CxImage::GetTypeIdFromName(ext);
	m_pImage = new CxImage(sFileName, type);

	if (!m_pImage->IsValid()){
		CString s = m_pImage->GetLastError();
		AfxMessageBox(s);
		delete m_pImage;
		m_pImage = NULL;
		return FALSE;
	}
	return TRUE;
}

CString CFloorPictureCtrl::FindExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return name.Mid(i+1);
		}
	}
	return CString(_T(""));
}

CString CFloorPictureCtrl::RemoveExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return name.Mid(0,i);
		}
	}
	return name;
}

void CFloorPictureCtrl::GenPicTexture()
{
	m_bPicLoaded = TRUE;
	if (glIsTexture(m_iPicTexture))
	{
		glDeleteTextures(1, &m_iPicTexture);
	}

	BOOL bPicValid = LoadImage(m_strPicturePath);

	if(bPicValid)
	{
		ILuint Width = m_pImage->GetWidth();
		ILuint Height = m_pImage->GetHeight();

		GLuint texFormat = 0;
		texFormat = GL_TEXTURE_2D;

		m_dWidth = Width;
		m_dHeight = Height;

		m_dImageHeight = Height;
		m_dImageWidth = Width;
		SetPoint(ARCVector2(0.0,0.0));
		if(m_bReferLine && m_refLinePath.getCount() == 2)
		{
			Point head, tail;
			head.setPoint(m_refLinePath.getPoint(0).getX(),m_dImageHeight-m_refLinePath.getPoint(0).getY(),0.0);
			tail.setPoint(m_refLinePath.getPoint(1).getX(),m_dImageHeight-m_refLinePath.getPoint(1).getY(),0.0);
			Point ptLine[2];
			ptLine[0] = head;
			ptLine[1] = tail;
			m_refLine.init(2,ptLine);
			m_bReferLine = FALSE;
		}

		GLint maxTexWidth =0;
		GLint maxTexHeight =0;
		GLint picWidth = ARCMath::FloorTo2n(Width);
		GLint picHeight =  ARCMath::FloorTo2n(Height);
		while(maxTexHeight==0 && maxTexWidth==0)
		{			
			glTexImage2D(GL_PROXY_TEXTURE_2D, 0,getGLFormatEx(m_pImage->GetColorType()) , \
				picWidth,picHeight,0,getGLFormat(m_pImage->GetColorType())\
				,GL_UNSIGNED_BYTE, 0 );
			glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &maxTexWidth);
			glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &maxTexHeight);
			picHeight/=2;
			picWidth/=2;
			if(picHeight < 2 || picWidth < 2)
				break;
		}

		ILuint scaleWidth = maxTexWidth;
		ILuint scaleHeigh = maxTexHeight; 
		scaleWidth = min( 2048, scaleWidth );
		scaleHeigh = min( 2048, scaleHeigh );

		if(scaleWidth!= Width || scaleHeigh!= Height )
			m_pImage->Resample(scaleWidth,scaleHeigh,2);

		glGenTextures(1, &m_iPicTexture);
		glBindTexture(GL_TEXTURE_2D,m_iPicTexture);

		glTexParameteri(texFormat, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
		glTexParameteri(texFormat, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering

		glTexImage2D(texFormat, 0, 3, m_pImage->GetWidth(),m_pImage->GetHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, m_pImage->GetBits());
		m_bPicTextureValid = glIsTexture(m_iPicTexture);	

	}
	else
	{
		m_bPicTextureValid = FALSE;		
	}

	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}