#include "stdafx.h"
#include "Test3DView.h"


IMPLEMENT_DYNCREATE(CTest3DView, CView)

BEGIN_MESSAGE_MAP(CTest3DView, CView)
	// Standard printing commands
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CTest3DView::OnDraw( CDC* pDC )
{
	
	// TODO: add draw code for native data here
	// Make the rendering context current
	if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC)) {
		ASSERT(FALSE);
		return;
	}




	::SwapBuffers(m_pDC->GetSafeHdc());	

	//m_bBusy = FALSE;	
}



BOOL SetupPixelFormat(HDC hDC, DWORD dwFlags)
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
	int m_iPixelFormat;

	if( !(m_iPixelFormat = ::ChoosePixelFormat(hDC, &pfd)) )
	{
		//MessageBox("ChoosePixelFormat failed");
		return FALSE;
	}

	if(!SetPixelFormat(hDC, m_iPixelFormat, &pfd))
	{
		//MessageBox("SetPixelFormat failed");
		return FALSE;
	}

	// TRACE("Pixel Format %d was selected\n", m_iPixelFormat);

	return TRUE;
}


int CTest3DView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
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

	return 0;
}

BOOL CTest3DView::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

CTest3DView::CTest3DView()
{
	m_pDC = NULL;
}
