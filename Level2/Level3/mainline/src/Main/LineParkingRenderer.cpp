#include "StdAfx.h"

#include ".\lineparkingrenderer.h"
#include "./OglTextureResource.h"
#include "./TermPlanDoc.h"
#include "./3DView.h"
#include "./SelectionHandler.h"
#include "./Processor2.h"
#include "../Engine/LineParkingProc.h"
#include <Common/ShapeGenerator.h>
#include "./../Engine/StretchProc.h"

LineParkingRenderer::LineParkingRenderer(CProcessor2*pProc2): ProcessorRenderer(pProc2)
{
	m_pTexture =  NULL;
	Update();
}

LineParkingRenderer::~LineParkingRenderer(void)
{
	glDeleteLists(m_ndisplist,1);
	m_pTexture->unRef();
}

void LineParkingRenderer::GenerateDispList(C3DView * pView)
{
	//calculate the path
	LandProcPartArray linkProcs =((LineParkingProc*) getProcessor())->GetLinkedProcs();
	if(linkProcs.size() <2) return;
	LandProcPart stretchposFrom = linkProcs[0];
	LandProcPart stretchposTo = linkProcs[linkProcs.size()-1];
	
	if( stretchposFrom.pProc != stretchposTo.pProc )return;

	LandfieldProcessor * pProc  = (LandfieldProcessor*)stretchposFrom.pProc;
	LandfieldProcessor * pParking = (LandfieldProcessor*)getProcessor();
	int stretchPartFrom = stretchposFrom.part;
	int stretchPartTo = stretchposTo.part;

	ARCPath3 stretchPath;
	 
	if( stretchPartFrom% 2 ==0 )
		stretchPath = ((StretchProc*)pProc)->getStretchPipePath().leftpath;
	else
		stretchPath = ((StretchProc*)pProc)->getStretchPipePath().rightpath;

	if(stretchPath.size()<3)
	{
		stretchPartFrom %= 2;
		stretchPartTo %= 2;
	}
	
	

	ARCVector3 v1,v2;Point intertPtFrom,intertPtTo;
	ARCVector3 pFrom,pTo;
	
	ARCPath3 parkingPath = pParking->GetPath();
	pFrom = parkingPath[0];
	pTo = parkingPath[parkingPath.size()-1];

	v1 = stretchPath[stretchPartFrom/2];
	v2 = stretchPath[stretchPartFrom/2+1];
	
	intertPtFrom = Line( Point(v1[VX],v1[VY],v1[VZ]),Point(v2[VX],v2[VY],v2[VZ]) ).getProjectPoint(Point(pFrom[VX],pFrom[VY],pFrom[VZ]) );
	
	v1 = stretchPath[stretchPartTo/2];
	v2 = stretchPath[stretchPartTo/2+1];

	intertPtTo = Line( Point(v1[VX],v1[VY],v1[VZ]),Point(v2[VX],v2[VY],v2[VZ]) ).getProjectPoint(Point(pTo[VX],pTo[VY],pTo[VZ]) );
	
	double parkingFromDist = stretchPath.getPointDistance(ARCVector3(intertPtFrom));
	double parkingToDist = stretchPath.getPointDistance(ARCVector3(intertPtTo)) ;
	double parkinglen = parkingToDist - parkingFromDist ;
	int parkingNum = pParking->GetSpotNumber();
	double spotLen = parkinglen/parkingNum;

	
	
	ARCPath3 parkingpath;


	ShapeGenerator::widthPath widePath;
	//get the control points
	for(int i=0;i<parkingNum+1;i++)
	{
		parkingpath.push_back( stretchPath.getDistancePoint(parkingFromDist + i * spotLen ));
	}

	
	//
	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide);
	for(size_t i=0;i<parkingpath.size();i++)
	{
		parkingpath[i][VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude( parkingpath[i][VZ] );
	}
	

	SHAPEGEN.GenWidthPath(parkingpath.getPath(), pParking->GetSpotWidth()*2,widePath );

	//drawing the blocks
	glDeleteLists(m_ndisplist,1);
	m_ndisplist = glGenLists(1);
	glNewList(m_ndisplist,GL_COMPILE);

	GLfloat t = 1.0f;

	GLfloat sL=0;GLfloat sR=0;   //texcoord s;

	int nptCnt =(int) widePath.leftpts.size() ;
	
	if( (spotLen>0)^(stretchPartFrom% 2 ==1) )
	{
		for(int i=0;i<nptCnt-1;i++)
		{		
			glColor3f(1.0f,1.0f,1.0f);		
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);	glVertex3dv(parkingpath[i]);
			glTexCoord2f(0,1);  glVertex3dv(widePath.leftpts[i]);		
			glTexCoord2f(1,1);	glVertex3dv(widePath.leftpts[i+1]);
			glTexCoord2f(1,0);	glVertex3dv(parkingpath[i+1]);				
			glEnd();		
		}
	}
	else
	{
		for(int i=0;i<nptCnt-1;i++)
		{		
			glColor3f(1.0f,1.0f,1.0f);		
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);	glVertex3dv(parkingpath[i]);
			glTexCoord2f(0,1);  glVertex3dv(widePath.rightpts[i]);		
			glTexCoord2f(1,1);	glVertex3dv(widePath.rightpts[i+1]);
			glTexCoord2f(1,0);	glVertex3dv(parkingpath[i+1]);				
			glEnd();		
		}
	}
	
	


	glEndList();

	

}

void LineParkingRenderer::Render(C3DView * pView)
{
	if(NeedSync())
	{
		GenerateDispList(pView);
		DoSync();
	}

	if(!m_pTexture)
		m_pTexture  = &( pView->getTextureRes().RefTextureByType(OglTextureResource::LineParkingTexture) );
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-2.0,-2.0);	
	m_pTexture->Activate();	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glCallList(m_ndisplist);
	glDisable(GL_POLYGON_OFFSET_FILL);	
	m_pTexture->DeActivate();

	/*LineParkingProc  * pProc = (LineParkingProc * )m_pProc2->GetProcessor();
	ARCPath3 r1,r2;
	pProc->GetParkingPath(r1,r2);
	glBegin(GL_QUAD_STRIP);
	for(size_t i=0;i<r1.size();i++){
		glVertex3dv(r1[i]);
		glVertex3dv(r2[i]);
	}	
	glEnd();*/
	
}

void LineParkingRenderer::RenderSelect(C3DView * pView,int selidx)
{
	if(NeedSync())
	{
		GenerateDispList(pView);
		DoSync();
	}
	glLoadName( NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_MAIN,selidx,0) );
	glCallList(m_ndisplist);
}