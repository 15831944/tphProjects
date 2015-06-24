#include "StdAfx.h"
#include ".\turnoffrenderer.h"

#include "./3DView.h"
#include "./TessellationManager.h"
#include <Common/ShapeGenerator.h>
#include "./Common/pollygon.h"
#include "./TermPlanDoc.h"
#include "./SelectionHandler.h"
#include "../Engine/StretchProc.h"
#include "../Engine/TurnOffProc.h"
#include "glRender/glTextureResource.h"
#include "glRender/glShapeResource.h"

GLdouble TurnOffRenderer::m_dthickness = 10;
GLfloat TurnOffRenderColor[3] = { 0.5f,0.5f,0.5f };
TurnOffRenderer::TurnOffRenderer(CProcessor2*pProc2):ProcessorRenderer(pProc2)
{
	m_pTexture = NULL;
	m_iboundinglist = 0;
	m_isurdisplist = 0;
}

TurnOffRenderer::~TurnOffRenderer(void)
{
	
}
void TurnOffRenderer::Render(C3DView* pView)
{
	if(NeedSync())
	{
		GenDisplaylist(pView);
		DoSync();
	}

	if(!m_pTexture)
		m_pTexture  = pView->getTextureResource()->getTexture("Lawn1");
	if(m_pTexture)
		m_pTexture->Apply();	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	
	glCallList(m_isurdisplist);

	m_pTexture->UnApply();
	/*glCallList(m_iboundinglist);
	glPushMatrix();
	glTranslated(0,0,-m_dthickness);
	glCallList(m_isurdisplist);
	glPopMatrix();*/
}

void TurnOffRenderer::RenderSelect(C3DView * pView,int selidx)
{
	if(NeedSync())
	{
		GenDisplaylist(pView);
		DoSync();
	}
	glLoadName( NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_MAIN,selidx,0) );
	glCallList(m_isurdisplist);
}
void TurnOffRenderer::GenDisplaylist(C3DView* pView)
{
	//get the two linked stretch
	LandProcPartArray& linkProcs =((TurnOffProc*) getProcessor())->GetLinkedProcs();
	if(linkProcs.size() <2 )return;
	ASSERT(linkProcs.size()>1);
	TurnOffProc * pTurnOff = (TurnOffProc*)getProcessor();
	LandfieldProcessor * pStretchFrom;
	LandfieldProcessor* pStretchTo;
	pStretchFrom = (LandfieldProcessor*) linkProcs[0].pProc;
	pStretchTo = (LandfieldProcessor*) linkProcs[linkProcs.size()-1].pProc;
	int StretchPartFrom = linkProcs[0].part; 
	int StretchPartTo = linkProcs[linkProcs.size()-1].part;
	ARCVector3 pFrom,pTo;
	
	ARCPath3 turnoffPath = pTurnOff->GetPath();
	ASSERT(turnoffPath.size()>1);
	pFrom = turnoffPath[0];
	pTo = turnoffPath[turnoffPath.size()-1];
	
	ARCVector3 strethV1,strethV2;
	
	//get the control points
	ARCPath3 controlPoints = turnoffPath;
	ARCVector3 p1,p2;
	ARCVector3 v1,v2;
	ARCVector3 orien;
	ARCVector3 interVec;
	double halfStretchWid = 0.5 * pStretchFrom->GetLaneWidth() * pStretchFrom->GetLaneNum();
	ARCPath3 stretchPath = pStretchFrom->GetPath();
	if(stretchPath.size()<3)
	{
		StretchPartFrom %= 2;
	}
	v1 = stretchPath[StretchPartFrom/2];
	v2 = stretchPath[StretchPartFrom/2+1];
	Point interPt  = Line( Point(v1[VX],v1[VY],v1[VZ]),Point(v2[VX],v2[VY],v2[VZ]) ).getProjectPoint(Point(pFrom[VX],pFrom[VY],pFrom[VZ]) );
	orien = ARCVector3(v1,v2);
	orien.Normalize();
	strethV1 = orien;
	double flag  = ( StretchPartFrom%2 == 0 )?-1.0:1.0;
	orien = ARCVector3(-orien[VY],orien[VX],0.0);
	orien.Normalize();
	interVec = ARCVector3(interPt);
	interVec += halfStretchWid * orien * flag;
	controlPoints[0] = (interVec);
	
	//to 
	halfStretchWid = 0.5 * pStretchTo->GetLaneWidth() * pStretchTo->GetLaneNum();
	stretchPath = pStretchTo->GetPath();
	if(stretchPath.size()<3)
	{
		StretchPartTo %= 2 ;
	}
	v1 = stretchPath[StretchPartTo/2];
	v2 = stretchPath[StretchPartTo/2+1];
	interPt  = Line( Point(v1[VX],v1[VY],v1[VZ]),Point(v2[VX],v2[VY],v2[VZ]) ).getProjectPoint(Point(pTo[VX],pTo[VY],pTo[VZ]) );
	orien = ARCVector3(v1,v2);
	orien.Normalize();
	strethV2 = orien;
	flag  = ( StretchPartTo%2 == 0 )?-1.0:1.0;
	orien = ARCVector3(-orien[VY],orien[VX],0.0);
	orien.Normalize();
	interVec = ARCVector3(interPt);
	interVec += halfStretchWid * orien * flag;
	controlPoints[controlPoints.size()-1] = (interVec);
	

	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide);
	for(size_t i=0;i<controlPoints.size();i++)
	{
		controlPoints[i][VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude( controlPoints[i][VZ] );
	}
	//smooth the path
	ARCPath3 smoothPath;
	SHAPEGEN.GenSmoothPath(controlPoints,smoothPath,1 * SCALE_FACTOR);	
	
	
	ShapeGenerator::widthPath widePath;
	SHAPEGEN.GenWidthPath(smoothPath.getPath(), pTurnOff->GetLaneWidth(),widePath );
	
	//adjust the wide path
	v1 = ARCVector3(widePath.leftpts[0],widePath.rightpts[0]);
	v2 = ARCVector3(widePath.leftpts[widePath.leftpts.size()-1],widePath.rightpts[widePath.rightpts.size()-1]);
	
	double CosOfVec = v1.GetCosOfTwoVector(strethV1);
	if( abs(CosOfVec) < 0.000001 )     //avoid div zero
	{
		CosOfVec = CosOfVec< 0? -1.0:1.0;
	}
	widePath.leftpts[0] = controlPoints[0] - strethV1 * pTurnOff->GetLaneWidth() * 0.5 / CosOfVec ;
	widePath.rightpts[0] = controlPoints[0] + strethV1 * pTurnOff->GetLaneWidth() * 0.5 / CosOfVec ; 
	CosOfVec = v2.GetCosOfTwoVector(strethV2);
	if(  abs(CosOfVec) < 0.000001 )
	{
		CosOfVec = CosOfVec< 0? -1.0:1.0;
	}
	widePath.leftpts[widePath.leftpts.size()-1]= controlPoints[controlPoints.size()-1] - strethV2 * pTurnOff->GetLaneWidth() * 0.5 / CosOfVec;
	widePath.rightpts[widePath.rightpts.size()-1] =controlPoints[controlPoints.size()-1] + strethV2 * pTurnOff->GetLaneWidth() * 0.5 / CosOfVec; 

	//draw the generate block
	glDeleteLists(m_isurdisplist,1);
	m_isurdisplist = glGenLists(1);
	glNewList(m_isurdisplist,GL_COMPILE);

	GLfloat t = 1.0f;

	GLfloat sL=0;GLfloat sR=0;   //texcoord s;

	int nptCnt =(int) widePath.leftpts.size() ;
	for(int i=0;i<nptCnt-1;i++)
	{
		glColor3fv(TurnOffRenderColor);
		glBegin(GL_QUAD_STRIP);
		glTexCoord2f(sL,0);	glVertex3dv(widePath.leftpts[i]);
		glTexCoord2f(sR,t); glVertex3dv(widePath.rightpts[i]);

		sL += (GLfloat) (widePath.leftpts[i].DistanceTo(widePath.leftpts[i+1]) / SCALE_FACTOR); 
		sR += (GLfloat) (widePath.rightpts[i].DistanceTo(widePath.rightpts[i+1]) / SCALE_FACTOR);
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