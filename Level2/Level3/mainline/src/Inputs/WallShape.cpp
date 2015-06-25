#include "StdAfx.h"
#include ".\wallshape.h"
#include "../Engine/terminal.h"
#include "../Main/TessellationManager.h"
#include <Common/ShapeGenerator.h>


inline
static void RenderFlatSquare(double x, double y, double z, double size) {
	glPushMatrix();
	glTranslated(x,y,z);
	glScaled(size,size,size);
	glBegin(GL_QUADS);
	glNormal3f(0.0,0.0,1.0);
	glVertex3f(-0.5, -0.5, 0.0);
	glVertex3f(+0.5, -0.5, 0.0);
	glVertex3f(+0.5, +0.5, 0.0);
	glVertex3f(-0.5, +0.5, 0.0);
	glEnd();
	glPopMatrix();
}
inline static void  RenderSolidPath(const std::vector<Point> & path_,DistanceUnit width_ ,DistanceUnit height_)
{
	//calculate the width segment
	int nptCount=path_.size();	
	if(nptCount<2)return;

	ShapeGenerator::widthPath widthpath;
	Path thepath;
	thepath.init((int)path_.size(),&(path_[0]));

	SHAPEGEN.GenWidthPath(thepath,width_,widthpath);

	for(int i=0;i<(int)widthpath.leftpts.size();i++){
		widthpath.rightpts[i][VZ] = 0;
		widthpath.leftpts[i][VZ] =0 ;
	}
	//the ceil
	glPushMatrix();
	glTranslated(0,0,height_);
	glNormal3f(0,0,1);
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<(int)widthpath.leftpts.size();i++){
		glVertex3dv(widthpath.rightpts[i]);
		glVertex3dv(widthpath.leftpts[i]);
	}
	glEnd();
	glPopMatrix();
	
	ARCVector3 vheight( 0,0,height_);
	
	glBegin(GL_QUADS);
	
		int ii=0;
		for(;ii<(int) widthpath.leftpts.size()-1;++ii)
		{
			ARCVector3 v1(widthpath.leftpts[ii],widthpath.leftpts[ii+1]);
			ARCVector3 vnor = v1^ARCVector3(0,0,1);
			vnor.Normalize();

			glNormal3dv(vnor);
			glVertex3dv(widthpath.leftpts[ii] + vheight);glVertex3dv(widthpath.leftpts[ii]);
			glVertex3dv(widthpath.leftpts[ii+1]);glVertex3dv(widthpath.leftpts[ii+1] + vheight);

			glNormal3dv(-vnor);
			glVertex3dv(widthpath.rightpts[ii]);glVertex3dv(widthpath.rightpts[ii] + vheight);
			glVertex3dv(widthpath.rightpts[ii+1] + vheight);glVertex3dv(widthpath.rightpts[ii+1]);
		}
		glNormal3dv( (widthpath.leftpts[ii]- widthpath.leftpts[ii-1]).Normalize() );	
		glVertex3dv(widthpath.rightpts[ii]);glVertex3dv(widthpath.rightpts[ii] + vheight);
		glVertex3dv(widthpath.leftpts[ii] + vheight);glVertex3dv(widthpath.leftpts[ii]);
	
		ARCVector3 vnormal = widthpath.leftpts[0] - widthpath.leftpts[1];
		glNormal3dv(vnormal.Normalize());

		glVertex3dv(widthpath.rightpts[0]);glVertex3dv(widthpath.leftpts[0]);
		glVertex3dv(widthpath.leftpts[0] + vheight);glVertex3dv(widthpath.rightpts[0] + vheight );

	glEnd();

	/*vnormal = widthpath.leftpts[nptCount-1] - widthpath.leftpts[nptCount-2];
	glNormal3dv(-vnormal);
	glVertex3dv(widthpath.rightpts[nptCount-1]);glVertex3dv(widthpath.leftpts[nptCount-1]);
	glVertex3dv(widthpath.leftpts[nptCount-1] + vheight);glVertex3dv(widthpath.rightpts[nptCount-1] + vheight );
	glEnd();
	*/
}

WallShape::WallShape(void)
{
	m_height =300;
	m_width  =20;
	//	 m_strucID.SetStrDict(m_pI)
	//	 m_strucID.setID("UNDEFINED");
	//	 m_NameStr="";
	m_FloorNum=-1;

	m_bInEditMode=false;
	m_bDirtflag=true;

	//m_displaylist=-1;
	//m_floordisplaylist=-1;

	m_path.clear();

	//Default setting
	m_dispProperties.bDisplay[WSDP_DISP_SHAPE] = TRUE;
	m_dispProperties.color[WSDP_DISP_SHAPE] = RGB(255,255,255);
	m_dispProperties.bDisplay[WSDP_DISP_PROCNAME] = FALSE;
	m_dispProperties.color[WSDP_DISP_PROCNAME] = RGB(0,0,0);
}
void WallShape::DrawOGL(bool drawfloor)
{
	if(!m_dispProperties.bDisplay[WSDP_DISP_SHAPE])
		return;

	glDisable(GL_TEXTURE_2D);
	//GenDisplayList();
	//call display list ;
	if(m_bInEditMode){
		//draw outline and edit line;
		glDisable(GL_LIGHTING);	
		glColor3ubv(m_dispProperties.color[WSDP_DISP_SHAPE]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			RenderSolidPath(m_path,m_width,m_height);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for(size_t i=0;i<m_path.size();++i)
		{		
			RenderFlatSquare(m_path[i].getX(),m_path[i].getY(),0.0,m_width);
		}

	}else{
		//draw the solid cube;
		
		glColor3ubv(m_dispProperties.color[WSDP_DISP_SHAPE]);
		RenderSolidPath(m_path,m_width,m_height);
		/*if(drawfloor){
			glCallList(m_floordisplaylist);
			glPushMatrix();
			glTranslated(0,0,GetHeight());
			glCallList(m_floordisplaylist);
			glPopMatrix();
		}*/
		
		
	}

}
void WallShape::DrawSelectArea(){
	//GenDisplayList();
	//call display list ;
	//glCallList(m_displaylist);
	RenderSolidPath(m_path,m_width,m_height);

}
//void WallShape::GenDisplayList()
//{
//	glDeleteLists(m_displaylist,1);
//	m_displaylist=glGenLists(1);
//	glNewList(m_displaylist,GL_COMPILE);
//	//draw the shape here;
//	RenderSolidPath(m_path,m_width,m_height);
//	glEndList();
//	//draw floor
//	/*glDeleteLists(m_floordisplaylist,1);
//	m_floordisplaylist = glGenLists(1);
//	glNewList(m_floordisplaylist,GL_COMPILE);
//	glDisable(GL_CULL_FACE);
//	std::vector<ARCVector3> ptsbuf;
//	ptsbuf.resize(m_path.size());
//	for(size_t i=0;i<m_path.size();i++){
//
//	ptsbuf[i] = ARCVector3(m_path[i].getX(),m_path[i].getY(),0);
//	}
//	glNormal3d(0,0,1);
//	CTessellationManager* pTM = CTessellationManager::GetInstance();
//	pTM->Init(CTessellationManager::Render);	
//	pTM->BeginPolygon();
//	pTM->BeginContour();
//	for(size_t i=0;i<ptsbuf.size();i++){
//	pTM->ContourVertex(ptsbuf[i]);
//	}		
//	pTM->EndContour();
//	pTM->EndPolygon();
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	glEndList();*/
//
//	m_bDirtflag=false;
//}
WallShape::~WallShape(void)
{
	//glDeleteLists(m_displaylist,1);
}

void WallShape::SetTerminal( Terminal* _pTerm )
{ 
	m_pTerm = _pTerm;  
	m_strucID.SetStrDict( _pTerm->inStrDict ); 
}

WallShape* WallShape::NewCopy()
{
	WallShape *pWallCopy = new WallShape;
	*pWallCopy = *this; 
	return pWallCopy;
}

WallShapArray::WallShapArray( const WallShapArray& _arrary )
{

}


WallShape* WallShapArray::FindWallShapeByGuid( const CGuid& guid ) const
{
	int nCount = getCount();
	for(int i=0;i<nCount;i++)
	{
		WallShape* pItem = getItem(i);
		if (pItem->getGuid() == guid)
			return pItem;
	}
	return NULL;
}