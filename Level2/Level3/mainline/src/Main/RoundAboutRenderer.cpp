#include "StdAfx.h"
#include "../Engine/RoundAboutProc.h"
#include ".\roundaboutrenderer.h"
#include "../Engine/StretchProc.h"

#include "./3DView.h"
#include "./TessellationManager.h"
#include <Common/ShapeGenerator.h>
#include "./Common/pollygon.h"
#include "./TermPlanDoc.h"
#include "./SelectionHandler.h"
GLdouble RoundAboutRenderer::m_dthickness = 10;
GLfloat RoundAboutColor[3] = { 0.5f,0.5f,0.5f };

RoundAboutRenderer::RoundAboutRenderer(CProcessor2* pProc2) : ProcessorRenderer(pProc2)
{
	m_pTexture = NULL;
	m_iboundinglist = 0;
	m_isurdisplist = 0;
}

RoundAboutRenderer::~RoundAboutRenderer(void)
{
	glDeleteLists(m_isurdisplist,1);
	glDeleteLists(m_iboundinglist,1);
	if(!m_pTexture) m_pTexture->unRef();
}

void RoundAboutRenderer::Render(C3DView* pView)
{
	if(NeedSync())
	{
		GenDisplaylist(pView);
		DoSync();
	}
	
	if(!m_pTexture)
		m_pTexture  = &( pView->getTextureRes().RefTextureByType(OglTextureResource::StretchTexture) );
	
	m_pTexture->Activate();		
	glCallList(m_isurdisplist);

	m_pTexture->DeActivate();
	/*glCallList(m_iboundinglist);
	glPushMatrix();
	glTranslated(0,0,-m_dthickness);
	glCallList(m_isurdisplist);
	glPopMatrix();*/



}
void RoundAboutRenderer::RenderSelect(C3DView * pView,int selidx)
{

	if(NeedSync())
	{
		GenDisplaylist(pView);
		DoSync();
	}
	glLoadName( NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_MAIN,selidx,0) );
	glCallList(m_isurdisplist);
}

void RoundAboutRenderer::GenDisplaylist(C3DView* pView)
{
	
	RoundAboutProc * pRAProc = (RoundAboutProc*) getProcessor();	
	double rbhalfwidth = 0.5 * pRAProc->GetLaneNum() * pRAProc->GetLaneWidth();	

	ARCPath3 smoothCtrlPts = pRAProc->GetPath();
	for(size_t i=0;i<smoothCtrlPts.size();i++)
	{
		smoothCtrlPts[i][VZ] =pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(smoothCtrlPts[i][VZ]);
	}
	//ARCPath3 smoothPath;
	//SHAPEGEN.GenSmoothPath(ctrlpoints,smoothPath,1 * SCALE_FACTOR);
	ShapeGenerator::widthPath widePath;
	SHAPEGEN.GenWidthPath(smoothCtrlPts.getPath(), rbhalfwidth*2,widePath );
	ASSERT(widePath.leftpts.size() == widePath.rightpts.size() );
	
	widePath.leftpts[widePath.leftpts.size()-1] = (widePath.leftpts[0]);
	widePath.rightpts[widePath.rightpts.size()-1] = (widePath.rightpts[0]);
	//draw the generate block
	glDeleteLists(m_isurdisplist,1);
	m_isurdisplist = glGenLists(1);
	glNewList(m_isurdisplist,GL_COMPILE);
	
	GLfloat t = (GLfloat)pRAProc->GetLaneNum(); //texcoord t

	GLfloat sL=0;GLfloat sR=0;   //texcoord s;
	
	int nptCnt =(int) widePath.leftpts.size() ;
	for(int i=0;i<nptCnt-1;i++)
	{
		glColor3fv(RoundAboutColor);
		glBegin(GL_QUAD_STRIP);
		glTexCoord2f(sL,0);	glVertex3dv(widePath.leftpts[i]);
		glTexCoord2f(sR,t); glVertex3dv(widePath.rightpts[i]);
		
		sL += (GLfloat) (widePath.leftpts[i].DistanceTo(widePath.leftpts[i+1]) / (2*SCALE_FACTOR) ); 
		sR += (GLfloat) (widePath.rightpts[i].DistanceTo(widePath.rightpts[i+1]) / (2*SCALE_FACTOR) );
		glTexCoord2f(sL,0);	glVertex3dv(widePath.leftpts[i+1]);
		glTexCoord2f(sR,t);	glVertex3dv(widePath.rightpts[i+1]);
		std::swap(sL,sR);
		
		glEnd();
	}	
	glEndList();

	glDeleteLists(m_iboundinglist,1);
	m_iboundinglist = glGenLists(1);
	glNewList(m_iboundinglist,GL_COMPILE);
	glEnable(GL_LIGHTING);
	glBegin(GL_QUAD_STRIP);
	ARCVector3 thickVect(0,0,-m_dthickness);
	for(int i=0;i<nptCnt-1;i++)
	{
		ARCVector3 _normal(widePath.leftpts[i] - widePath.leftpts[i+1]);
		glNormal3d( -_normal[VY],_normal[VX],_normal[VZ] );
		glVertex3dv(widePath.leftpts[i]);glVertex3dv(widePath.leftpts[i]+thickVect);
		glVertex3dv(widePath.leftpts[i+1]);glVertex3dv(widePath.leftpts[i+1]+thickVect);
	}
	glEnd();
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<nptCnt-1;i++)
	{
		ARCVector3 _normal(widePath.rightpts[i] - widePath.rightpts[i+1]);
		glNormal3d( -_normal[VY],_normal[VX],_normal[VZ] );
		glVertex3dv(widePath.rightpts[i]);glVertex3dv(widePath.rightpts[i]+thickVect);
		glVertex3dv(widePath.rightpts[i+1]);glVertex3dv(widePath.rightpts[i+1]+thickVect);
	}
	glEnd();
	glDisable(GL_LIGHTING);
	glEndList();	
	
	
	
}
