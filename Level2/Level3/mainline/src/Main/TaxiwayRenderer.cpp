#include "StdAfx.h"
#include ".\taxiwayrenderer.h"
#include <Common/ShapeGenerator.h>
#include "./Engine/TaxiwayProc.h"
#include "Processor2.h"
#include <CommonData/3DTextManager.h>
#include "3DView.h"
#include ".\glrender\glTextureResource.h"
TaxiwayRenderer::TaxiwayRenderer(CProcessor2 * pProc2):ProcessorRenderer(pProc2)
{
}

TaxiwayRenderer::~TaxiwayRenderer(void)
{
}
void TaxiwayRenderer::Render(C3DView* pView){

	TaxiwayProc * ptaxiway = (TaxiwayProc*) m_pProc2->GetProcessor();
	//draw block	
	if(ptaxiway->m_vSegments.size()==0)ptaxiway->initSegments();
	else ptaxiway->CalculateSegment();

	Path * centerPath=ptaxiway->serviceLocationPath();
	int nCount=(int)ptaxiway->m_vSegments.size();
	ASSERT(nCount == centerPath->getCount());
	DistanceUnit p1LX,p1LY,p1RX,p1RY,p2LX,p2LY,p2RX,p2RY;

	//draw Surface of the taxiway;
	CTexture * pTexture = pView->getTextureResource()->getTexture("Taxiway");
	if(pTexture) pTexture->Apply();
	GLdouble scoord[]={0.001,0.0,0.0};
	GLdouble tcoord[]={0.0,0.001,0.0};

	
	Point centerPoint=centerPath->getPoint(0);
	TaxiwayProc::TAXIWAYSEGMENT segment=ptaxiway->m_vSegments[0];
	p1LX=segment.ptdir[VX]*segment.length_left + centerPoint.getX();
	p1LY=segment.ptdir[VY]*segment.length_left + centerPoint.getY();
	p1RX=-segment.ptdir[VX]*segment.length_right + centerPoint.getX();
	p1RY=-segment.ptdir[VY]*segment.length_right + centerPoint.getY();

	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,1.0f);	
	for(int i=1;i<nCount;i++){
		segment=ptaxiway->m_vSegments[i];
		centerPoint=centerPath->getPoint(i);
		p2LX=segment.ptdir[VX]*segment.length_left + centerPoint.getX();
		p2LY=segment.ptdir[VY]*segment.length_left + centerPoint.getY();
		p2RX=-segment.ptdir[VX]*segment.length_right + centerPoint.getX();
		p2RY=-segment.ptdir[VY]*segment.length_right + centerPoint.getY();
		double s,t;
		s=scoord[0]*p1LX+scoord[1]*p1LY;
		t=tcoord[0]*p1LY+tcoord[1]*p1LY;
		glTexCoord2d(s,t);
		glVertex3d(p1LX,p1LY,0);

		s=scoord[0]*p1RX+scoord[1]*p1RY;
		t=tcoord[0]*p1RX+tcoord[1]*p1RY;
		glTexCoord2d(s,t);
		glVertex3d(p1RX,p1RY,0);

		s=scoord[0]*p2RX+scoord[1]*p2RY;
		t=tcoord[0]*p2RX+tcoord[1]*p2RY;
		glTexCoord2d(s,t);
		glVertex3d(p2RX,p2RY,0);

		s=scoord[0]*p2LX+scoord[1]*p2LY;
		t=tcoord[0]*p2LX+tcoord[1]*p2LY;
		glTexCoord2d(s,t);
		glVertex3d(p2LX,p2LY,0);

		p1LX=p2LX;
		p1LY=p2LY;
		p1RX=p2RX;
		p1RY=p2RY;

	}

	glEnd();
	glDisable(GL_TEXTURE_2D);
	

}
void TaxiwayRenderer::RenderSelect(C3DView * pView,int selidx){


}
void TaxiwayRenderer::RenderSegmentDir(C3DView * pView)const{
	//glEnable(GL_LIGHTING);
	//glDisable(GL_TEXTURE_2D);
	//TaxiwayProc * ptaxiway = (TaxiwayProc*) m_pProc2->GetProcessor();
	//TaxiwayProc::SegmentList & segments = ptaxiway->GetSegmentList();
	//for(int i=0;i<(int)segments.size();++i){
	//	ARCVector2 p1 = segments[i].StartPoint;
	//	ARCVector2 p2 = segments[i].EndPoint;
	//	ARCVector2 centerPoint = (p1 +p2 ) * 0.5;
	//	ARCVector2 dir = ARCVector2(p2,p1);dir.SetLength(ptaxiway->GetWidth()*0.5f );
	//	ARCVector2 olddir = dir;
	//	ARCVector2 diroffset = dir;
	//	if(TaxiwayProc::NegativeDirection == segments[i].emType || 
	//		TaxiwayProc::Bidirection == segments[i].emType){
	//			glBegin(GL_TRIANGLES);
	//			glVertex2dv(centerPoint + diroffset);
	//			dir.Rotate(165);glVertex2dv(centerPoint + dir + diroffset);			
	//			dir.Rotate(30);glVertex2dv(centerPoint + dir + diroffset);
	//			glEnd();
	//		}

	//		if(TaxiwayProc::PositiveDirection == segments[i].emType || 
	//			TaxiwayProc::Bidirection == segments[i].emType){
	//				dir = olddir;
	//				glBegin(GL_TRIANGLES);
	//				glVertex2dv(centerPoint -diroffset);
	//				dir.Rotate(345);glVertex2dv(centerPoint + dir - diroffset);
	//				//glVertex2dv(centerPoint);
	//				dir.Rotate(30);glVertex2dv(centerPoint + dir -diroffset);
	//				glEnd();

	//			}

	//}

}


void TaxiwayRenderer::RenderMarkLine(C3DView *pView){
	//draw centerline'
	//DistanceUnit slhight=0.3*100;
	glEnable(GL_LIGHTING);
	DistanceUnit slwidth=0.2*100;	
	TaxiwayProc * ptaxiway = (TaxiwayProc*) m_pProc2->GetProcessor();

	glDisable(GL_TEXTURE_2D);
	ShapeGenerator::widthPath widthpath;
	SHAPEGEN.GenWidthPath(*ptaxiway->serviceLocationPath(),slwidth*2,widthpath);
	
	glColor3ub(255,255,0);
	
	glNormal3f(0,0,1);
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<(int)widthpath.leftpts.size();i++){
		glVertex3dv(widthpath.leftpts[i]);
		glVertex3dv(widthpath.rightpts[i]);
	}	
	glEnd();

	
	//draw direction 
	////glDisable(GL_LIGHTING);
	
	//render marking s		
	string str=ptaxiway->GetMarking();									
	double _angle;	
	Point pos;					
	ptaxiway->getMarkingPosition(pos,_angle);

	glDisable(GL_LIGHTING);
	glColor3ubv(m_pProc2->m_dispProperties.color[PDP_DISP_SHAPE]);
	DistanceUnit width= ptaxiway->GetWidth();
	TEXTMANAGER3D->DrawOutlineText(CString(str.c_str()),ARCVector3(pos.getX(),pos.getY(),0.0),(float)_angle,(float)width);
	glEnable(GL_LIGHTING);
	
}