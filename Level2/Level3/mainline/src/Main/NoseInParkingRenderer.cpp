#include "StdAfx.h"
#include ".\noseinparkingrenderer.h"
#include "./OglTextureResource.h"
#include "./TermPlanDoc.h"
#include "./3DView.h"
#include "./SelectionHandler.h"
#include "./Processor2.h"
#include "../Engine/NoseInParkingProc.h"
#include <Common/ShapeGenerator.h>
#include "./../Engine/StretchProc.h"
#include "./../Engine/LandfieldProcessor.h"



NoseInParkingRenderer::NoseInParkingRenderer(CProcessor2 * pProc2): ProcessorRenderer(pProc2)
{
	m_pTexture =  NULL;
	Update();
}

NoseInParkingRenderer::~NoseInParkingRenderer(void)
{
	glDeleteLists(m_ndisplist,1);
	m_pTexture->unRef();
}

void NoseInParkingRenderer::GenerateDispList(C3DView * pView)
{
	//calculate the path
	LandProcPartArray linkProcs =((NoseInParkingProc*) getProcessor())->GetLinkedProcs();
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



	ARCPath3 parkingLine;


	
	//get the control points
	for(int i=0;i<parkingNum+1;i++)
	{
		parkingLine.push_back( stretchPath.getDistancePoint(parkingFromDist + i * spotLen ));
	}


	//
	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide);
	for(size_t i=0;i<parkingLine.size();i++)
	{
		parkingLine[i][VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude( parkingLine[i][VZ] );
	}
	
	
	ShapeGenerator::WidthPipePath widePath;
	ShapeGenerator::GenWidthPipePath(parkingLine, pParking->GetSpotLength() * 2,widePath );
	ARCPath3 sidePath; sidePath.reserve(parkingLine.size());

	double rotateAngle = ARCMath::DegreesToRadians( pParking->GetSpotSlopAnlge() );
	
	if( (spotLen>0)^(stretchPartFrom%2 == 0) )
		sidePath = widePath.rightpath;
	else
		sidePath = widePath.leftpath;

	ARCVector3 v;
	for(int i=0;i<(int)parkingLine.size();i++)
	{
		v = sidePath[i] - parkingLine[i];
		v = ARCVector3( v[VX]*cos(rotateAngle) - v[VY]*sin(rotateAngle), v[VY]*cos(rotateAngle)+v[VX]*sin(rotateAngle),0);
		sidePath[i] = parkingLine[i] + v;
		//sidePath.push_back(otherPt);

	}
	//drawing the blocks
	glDeleteLists(m_ndisplist,1);
	m_ndisplist = glGenLists(1);
	glNewList(m_ndisplist,GL_COMPILE);

	GLfloat t = 1.0f;

	GLfloat sL=0;GLfloat sR=0;   //texcoord s;

	int nptCnt =(int) sidePath.size();
	for(int i=0;i<nptCnt-1;i++)
	{
		glColor3f(1.0f,1.0f,1.0f);		
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);	glVertex3dv(parkingLine[i]);
		double cosof2v =( (parkingLine[i+1]-parkingLine[i]).GetCosOfTwoVector(sidePath[i]-parkingLine[i]) );
		double ratio = 0.5*(parkingLine[i+1]-parkingLine[i]).Magnitude() * cosof2v / pParking->GetSpotLength();
		glTexCoord2f(0,1);  glVertex3dv(sidePath[i] + (sidePath[i]-parkingLine[i])*ratio );		
		glTexCoord2f(1,1);	glVertex3dv(sidePath[i+1] - (sidePath[i+1]-parkingLine[i+1])*ratio );
		glTexCoord2f(1,0);	glVertex3dv(parkingLine[i+1]);				
		glEnd();
	}
		

	/*if( (spotLen>0)^(stretchPartFrom% 2 ==0) )
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
	else
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
	}*/

	glEndList();

}

void NoseInParkingRenderer::Render(C3DView * pView)
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
	if(m_pTexture)m_pTexture->Activate();	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glCallList(m_ndisplist);
	glDisable(GL_POLYGON_OFFSET_FILL);

	m_pTexture->DeActivate();

	/*NoseInParkingProc  * pProc = (NoseInParkingProc * )m_pProc2->GetProcessor();
	ARCPath3 r1,r2;
	pProc->GetParkingPath(r1,r2);
	glBegin(GL_QUAD_STRIP);
	for(size_t i=0;i<r1.size();i++){
		glVertex3dv(r1[i]);
		glVertex3dv(r2[i]);
	}	
	glEnd();*/
	
}

void NoseInParkingRenderer::RenderSelect(C3DView * pView,int selidx)
{
	if(NeedSync())
	{
		GenerateDispList(pView);
		DoSync();
	}
	glLoadName( NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_MAIN,selidx,0) );
	glCallList(m_ndisplist);
}