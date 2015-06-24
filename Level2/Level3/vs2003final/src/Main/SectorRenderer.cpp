#include "StdAfx.h"
#include ".\sectorrenderer.h"
#include "../Engine/SectorProc.h"
#include "Processor2.h"
SectorRenderer::SectorRenderer(CProcessor2 * pProc2):ProcessorRenderer(pProc2){
}

SectorRenderer::~SectorRenderer(void)
{
}
void SectorRenderer::Render(C3DView* pView){
	SectorProc * psector = (SectorProc *) m_pProc2->GetProcessor();
	Path path = *(psector->serviceLocationPath());
	if(path.getCount()<1)return;
	double minAlt = psector->getMinAlt();
	double maxAlt = psector->getMaxAlt();

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

	glBegin(GL_LINES);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),maxAlt);	
		glVertex3d(path[i].getX(),path[i].getY(),minAlt);
	}
	glEnd();

	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);

}
void SectorRenderer::RenderSelect(C3DView * pView,int selidx) {
	glLoadName(selidx);
	Render(pView);
}
void SectorRenderer::RenderShadow(C3DView* pView){
	glDisable(GL_LIGHTING);
	SectorProc  * sectorproc = (SectorProc  *) m_pProc2->GetProcessor();

	Path path = * (sectorproc->serviceLocationPath());
	static const ARCColor4 shadowcolor(128,128,128,196);
	glColor4ubv(shadowcolor);
	glBegin(GL_POLYGON);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),0);		
	}
	glEnd();
	glEnable(GL_LIGHTING);
}