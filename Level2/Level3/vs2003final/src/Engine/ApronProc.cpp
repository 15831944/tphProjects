#include "StdAfx.h"
#include ".\apronproc.h"
#include "../Main/TessellationManager.h"

ApronProc::ApronProc(void)
{
}

ApronProc::~ApronProc(void)
{
}
int ApronProc::readSpecialField(ArctermFile& procFile)
{
	//number
	return TRUE;
}
int ApronProc::writeSpecialField(ArctermFile& procFile) const
{
	//number
	return TRUE;
}
void ApronProc::DrawSelectArea(double dAlt){
	GLdouble buf[256][3];

	CTessellationManager* pTM = CTessellationManager::GetInstance();
	pTM->Init(CTessellationManager::Render);	
	pTM->BeginPolygon();	
	pTM->BeginContour();
	Path * pApronway=serviceLocationPath();
	for( int i=0;i<pApronway->getCount();i++){
		Point & _p=pApronway->getPoint(i);
		buf[i][0]=_p.getX();
		buf[i][1]=_p.getY();
		buf[i][2]=0.0;
		pTM->ContourVertex(buf[i]);
	}
	pTM->EndContour();
	pTM->EndPolygon();
	pTM->End();


}

void ApronProc::DrawToScene(double dAlt,const std::vector<GLuint> texlist){

	//static const GLdouble xcoord[]={0.001,0.0,0.0,0.0};
	//static const GLdouble ycoord[]={0.0,0.001,0.0,0.0};
	//glBindTexture(GL_TEXTURE_2D,texlist[(int)Textures::TextureEnum::APRON]);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	//glTexGendv(GL_S,GL_OBJECT_PLANE,xcoord);
	//glEnable(GL_TEXTURE_GEN_S);

	//glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	//glTexGendv(GL_T,GL_OBJECT_PLANE,ycoord);
	//glEnable(GL_TEXTURE_GEN_T);
	//glEnable(GL_TEXTURE_2D);
	//
	//glColor3d(1.0,1.0,1.0);
	//GLdouble buf[256][3];
	//GLUtesselatorObj *tess=gluNewTess();
	//
	//gluTessCallback(tess, GLU_TESS_BEGIN, (GLvoid (__stdcall *) ( )) &glBegin);    
	//gluTessCallback(tess, GLU_TESS_END, (GLvoid (__stdcall *) ( )) &glEnd);
	//gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (__stdcall *) ( )) &glVertex3dv);
	//gluTessCallback(tess, GLU_TESS_EDGE_FLAG, (GLvoid (__stdcall *) ( )) &glEdgeFlag);
	//gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (__stdcall *) ( )) &combineCallback);
	//gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid (__stdcall *) ( )) &errorCallback);
	//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
	//gluTessBeginPolygon(tess,NULL);
	//gluTessBeginContour(tess);	
	//Path * pApronway=this->serviceLocationPath();
	//for( int i=0;i<pApronway->getCount();i++){
	//	Point & _p=pApronway->getPoint(i);
	//	buf[i][0]=_p.getX();
	//	buf[i][1]=_p.getY();
	//	buf[i][2]=0.0;

	//	gluTessVertex(tess,buf[i],buf[i]);
	//}
	//gluTessEndContour(tess);
	//gluTessEndPolygon(tess);
	//gluDeleteTess(tess);
	//
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_TEXTURE_GEN_S);
	//glDisable(GL_TEXTURE_GEN_T);
	static const GLdouble xcoord[]={0.001,0.0,0.0,0.0};
	static const GLdouble ycoord[]={0.0,0.001,0.0,0.0};
	glBindTexture(GL_TEXTURE_2D,texlist[(int)Textures::TextureEnum::APRON]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	glTexGendv(GL_S,GL_OBJECT_PLANE,xcoord);
	glEnable(GL_TEXTURE_GEN_S);

	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	glTexGendv(GL_T,GL_OBJECT_PLANE,ycoord);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_2D);
	GLdouble buf[256][3];
	CTessellationManager* pTM = CTessellationManager::GetInstance();
	pTM->Init(CTessellationManager::Render);	
	pTM->BeginPolygon();	
	pTM->BeginContour();
		Path * pApronway=this->serviceLocationPath();
		for( int i=0;i<pApronway->getCount();i++){
			Point  _p=pApronway->getPoint(i);
			buf[i][0]=_p.getX();
			buf[i][1]=_p.getY();
			buf[i][2]=dAlt;
			pTM->ContourVertex(buf[i]);
		}
		pTM->EndContour();
	pTM->EndPolygon();
	pTM->End();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);


}