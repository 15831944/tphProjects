#include "StdAfx.h"
#include ".\holdrenderer.h"
#include "../Engine/HoldProc.h"
#include "Processor2.h"

HoldRenderer::HoldRenderer(CProcessor2 * pProc2) :ProcessorRenderer(pProc2){
}

HoldRenderer::~HoldRenderer(void)
{
}


void HoldRenderer::Render(C3DView* pView){

	
	HoldProc  * holdproc = (HoldProc  *) m_pProc2->GetProcessor();

	Path path = holdproc->getPath();
	
	if(path.getCount()<1)return;
	
	double minAlt = holdproc->getMinAltitude() ;
	double maxAlt = holdproc->getMaxAltitude() ;
	glDisable(GL_LIGHTING);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0,1.0);
	glDepthMask(FALSE);
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),minAlt);
		glVertex3d(path[i].getX(),path[i].getY(),maxAlt);
	}
	glVertex3d(path[0].getX(),path[0].getY(),minAlt);
	glVertex3d(path[0].getX(),path[0].getY(),maxAlt);
	glEnd();

	glBegin(GL_POLYGON);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),minAlt);		
	}
	glEnd();

	glBegin(GL_POLYGON);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),maxAlt);		
	}
	glEnd();
	glDepthMask(TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glDisable(GL_BLEND);
	glBegin(GL_LINE_LOOP);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),minAlt);		
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),maxAlt);		
	}
	glEnd();
	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	//shadow

}

void HoldRenderer::RenderShadow(C3DView* pView){
glDisable(GL_LIGHTING);
	HoldProc  * holdproc = (HoldProc  *) m_pProc2->GetProcessor();

	Path path = holdproc->getPath();
	static const ARCColor4 shadowcolor(128,128,128,196);
	glColor4ubv(shadowcolor);
	glBegin(GL_POLYGON);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),0);		
	}
	glEnd();
	glEnable(GL_LIGHTING);
}
void HoldRenderer::RenderSelect(C3DView * pView,int selidx){
	glLoadName(selidx);
	Render(pView);

}
