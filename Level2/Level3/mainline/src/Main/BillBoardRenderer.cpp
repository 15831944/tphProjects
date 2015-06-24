#include "StdAfx.h"
#include ".\billboardrenderer.h"
#include "../common/ARCVector.h"
#include <CommonData/3DTextManager.h>
#include "../common/ARCColor.h"
#include "./OglTextureResource.h"
#include "PBuffer.h"
#include "3DView.h"
#define BUFFERSIZE 512


GLenum BillBoardRenderer::m_targetTexture = GL_TEXTURE_2D;



BillBoardRenderer::BillBoardRenderer(BillboardProc * _pProc,C3DView* _pView):m_pProc(_pProc),m_pView(_pView)
{
	m_iBMpStamp = -1;
	m_iTextStamp = -1;
	m_bUseText = m_pProc->useText();
	pTexture = NULL;
}

BillBoardRenderer::~BillBoardRenderer(void)
{
	ReleaseTexture();
	
}

void BillBoardRenderer::ReleaseTexture()
{
	glDeleteTextures(1,&m_iTextureId);
	if(pTexture && !m_bUseText)
	{
//		m_pView->getTextureRes().ReleaseTexture(pTexture->getName());
	}
}

void BillBoardRenderer::Render(C3DView * pview)
{
	//Generate the shape
	Path * _path = m_pProc->serviceLocationPath();
	if(_path->getCount()<2)return;
	
	Path * exArea = m_pProc->GetExpsurePath();
	if(!exArea)return;
	
	Point exposPt(0.0,0.0,0.0);
	if(exArea->getCount()>0)
	{
		exposPt = exArea->getPoint(exArea->getCount()/2);
	}
	
	
	DistanceUnit height=m_pProc->getHeight();
//	double halfangle = 0.5*m_pProc->getAngle();
	
	ARCVector2 widthorien;
	widthorien.reset( _path->getPoint(0),_path->getPoint(1));
	
	ARCVector2 orien;
	orien.reset(_path->getPoint(0),exposPt);
	ARCPoint3 p1,p2;

	if( (widthorien^orien)[VZ]<0 )
	{
		p1 = ARCPoint3(_path->getPoint(0));
		p2 = ARCPoint3(_path->getPoint(1));
		widthorien.Rotate(90);
	}else
	{
		p1 = ARCPoint3(_path->getPoint(1));
		p2 = ARCPoint3(_path->getPoint(0));
		widthorien.Rotate(-90);
	}
	widthorien.SetLength(orien.Length());

	orien = widthorien;
	widthorien.Normalize();

	double halfthick = 0.5* m_pProc->getThickness();
	
	
	ARCPoint3 renderPoints[8];
	renderPoints[0] = p1 + ARCPoint3( widthorien[VX]*halfthick,widthorien[VY]*halfthick,0);
	renderPoints[1] = p2 + ARCPoint3( widthorien[VX]*halfthick,widthorien[VY]*halfthick,0);
	renderPoints[2] = renderPoints[0]+ARCPoint3(0,0,height);
	renderPoints[3] = renderPoints[1]+ARCPoint3(0,0,height);
	renderPoints[4] = p1 + ARCPoint3( -widthorien[VX]*halfthick,-widthorien[VY]*halfthick,0);
	renderPoints[5] = p2 + ARCPoint3( -widthorien[VX]*halfthick,-widthorien[VY]*halfthick,0);
	renderPoints[6] = renderPoints[4] + ARCPoint3(0,0,height);
	renderPoints[7] = renderPoints[5] + ARCPoint3(0,0,height);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
		glNormal3d(0,0,1);
		glVertex3dv(renderPoints[0]);glVertex3dv(renderPoints[4]);glVertex3dv(renderPoints[5]);glVertex3dv(renderPoints[1]);
		widthorien.Rotate(90);
		glNormal3dv(widthorien);
		glVertex3dv(renderPoints[1]);glVertex3dv(renderPoints[5]);glVertex3dv(renderPoints[7]);glVertex3dv(renderPoints[3]);
		glNormal3d(0,0,1);	
		glVertex3dv(renderPoints[3]);glVertex3dv(renderPoints[7]);glVertex3dv(renderPoints[6]);glVertex3dv(renderPoints[2]);
		glNormal3dv(-widthorien);
		glVertex3dv(renderPoints[2]);glVertex3dv(renderPoints[6]);glVertex3dv(renderPoints[4]);glVertex3dv(renderPoints[0]);
		
	glEnd();
	glBegin(GL_QUADS);
		glNormal3dv(orien);
		glVertex3dv(renderPoints[4]);glVertex3dv(renderPoints[6]);glVertex3dv(renderPoints[7]);glVertex3dv(renderPoints[5]);
	glEnd();
	

	if(!BuildTexture())return;
	
	
	glEnable(m_targetTexture);
	if(m_pProc->useBitmap())
	{
		pTexture->Activate();	
	}
	else 
		glBindTexture(m_targetTexture,m_iTextureId);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	
	glBegin(GL_QUADS);
		glTexCoord2d(w,1-c);	glVertex3dv(renderPoints[0]);
		glTexCoord2d(w,c);  glVertex3dv(renderPoints[2]);
		glTexCoord2d(1-w,c);glVertex3dv(renderPoints[3]);
		glTexCoord2d(1-w,1-c);	glVertex3dv(renderPoints[1]);
	glEnd();
	glDisable(m_targetTexture);
	//draw vision area
	//ARCPoint3 areaPts[2];
	
	
	/*int numpts = (int)halfangle /5;
	glBegin(GL_LINE_STRIP);
	glVertex3dv(renderPoints[0]);
	for(int i=0;i<=numpts;i++)
	{
		ARCVector2 orien_x = orien; orien_x.Rotate(halfangle - halfangle*2.0*i/numpts);
		double namuda = double(i)/numpts;
		ARCPoint3 areapt_x = ARCVector3(orien_x[VX],orien_x[VY],0) + (1-namuda)*renderPoints[0] + namuda * renderPoints[1];
		glVertex3dv(areapt_x);
	}	

	glVertex3dv(renderPoints[1]);
	glEnd();*/
	
}

bool BillBoardRenderer::LoadFileTexture(CString filename)
{
	pTexture =  & (m_pView->getTextureRes()).RefTextureByName(filename);
	//m_iTextureId = pTexture->getTexId();
	w= 0;c =1;
	return true;
}

bool BillBoardRenderer::needRegenTexture()
{
	
	if( m_bUseText != m_pProc->useText() )
	{
		return true;
	}
	else
	{
		if( m_pProc->useBitmap() )
		{
			return m_pProc->getBmpModStamp() != m_iBMpStamp;
		}
		if( m_pProc->useText() )
		{
			return m_pProc->getTextModStamp()!= m_iTextStamp;
		}
		return false;		
	}
	
}

bool BillBoardRenderer::BuildTexture()
{
	
	if( needRegenTexture() )
	{
		ReleaseTexture();
		if(m_pProc->useBitmap())
		{
			//LoadBmpTexture( m_pProc->getBitmapPath().GetBuffer());
			LoadFileTexture( m_pProc->getBitmapPath().GetBuffer() );
		}

		if(m_pProc->useText())
		{
           MakeTextTexture( m_pProc->getText(),BUFFERSIZE,BUFFERSIZE );
		}
		DoSync();
	}
	return true;
}
void BillBoardRenderer::DoSync()
{
	m_iTextStamp = m_pProc->getTextModStamp();
	m_iBMpStamp = m_pProc->getBmpModStamp();
	m_bUseText = m_pProc->useText();
}


bool BillBoardRenderer::MakeTextTexture(CString text,int width,int height)
{
	glGenTextures(1, &m_iTextureId);
	static PBuffer pbuffer(BUFFERSIZE,BUFFERSIZE,0);

	HGLRC hglrc = wglGetCurrentContext(); //save the current context and dc
	HDC hdc = wglGetCurrentDC();
	
	
	static bool pbufferInited = false;

	if(!pbufferInited)
	{
		pbufferInited = pbuffer.Initialize(hdc,true);
	}
	pbuffer.MakeCurrent();
	GLubyte* pix = new GLubyte[3*(pbuffer.width)*(pbuffer.height)];	

	double m_nTextureSizeWorld = 512;
	
	glClearColor( 0.0f,0.0f,0.0f,1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0 , m_nTextureSizeWorld, -m_nTextureSizeWorld, m_nTextureSizeWorld, -1, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
	
	//glDisable(GL_DEPTH_TEST);

	glColor3f(1.0f,1.0f,1.0f);
	
	/*glBegin(GL_LINES);

	glVertex2d(0,0);
	glVertex2d(0,300);

	glEnd();*/
	//TEXTMANAGER3D->DrawFormattedBitmapText(text,ARCVector2(0,0));

	TEXTMANAGER3D->DrawOutlineText(text,ARCVector3(0,0,0),0,100);

	glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pix);

	//glBindTexture(m_targetTexture, m_iTextureId);
	
	//glGetTexImage(m_targetTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) pix);	
	
	wglMakeCurrent( hdc, hglrc );

	glTexParameteri(m_targetTexture,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(m_targetTexture,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//glTexImage2D(m_targetTexture,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,0);
	glBindTexture(m_targetTexture, m_iTextureId);
	gluBuild2DMipmaps(m_targetTexture, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) pix);
	w= 0;c =1;
	delete[] pix;
	return true;
	
	
	
}