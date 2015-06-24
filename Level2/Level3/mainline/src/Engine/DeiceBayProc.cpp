// DeiceBayProc.cpp: implementation of the DeiceBayProc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DeiceBayProc.h"
#include "../Main/TessellationManager.h"
#include <CommonData/Textures.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static void inline DrawFlatSquare(double width, double length){
	double halfw = 0.5 * width;
	double halfl = 0.5 * length;
	glBegin(GL_QUADS);
	glVertex2d(-halfw,-halfl);
	glVertex2d(halfw, -halfl);
	glVertex2d(halfw,halfl);
	glVertex2d(-halfw,halfl);
	glEnd();
}

DeiceBayProc::DeiceBayProc()
{
	m_nNumber =-1;
	m_dWinspan  = 1000;
	m_dLength = 2000;
	m_bBackup = false;
}

DeiceBayProc::~DeiceBayProc()
{

}

int DeiceBayProc::readSpecialField(ArctermFile& procFile)
{
	float m_float;
	//winspan
	procFile.getFloat(m_float);
	m_dWinspan = m_float;
	//length
	procFile.getFloat(m_float);
	m_dLength = m_float;
	//number
	procFile.getInteger( m_nNumber );
	return TRUE;
}
int DeiceBayProc::writeSpecialField(ArctermFile& procFile) const
{
	//write winspan
	procFile.writeFloat((float)m_dWinspan);
	procFile.writeFloat((float)m_dLength);
	//number
	procFile.writeInt( m_nNumber );
	return TRUE;
}
void DeiceBayProc::DrawSelectArea(double dAlt){
	/*GLdouble buf[256][3];

	CTessellationManager* pTM = CTessellationManager::GetInstance();
	pTM->Init(CTessellationManager::Render);	
	pTM->BeginPolygon();	
	pTM->BeginContour();
	Path * pOutline=this->serviceLocationPath();
	for( int i=0;i<pOutline->getCount();i++){
		Point & _p=pOutline->getPoint(i);
		buf[i][0]=_p.getX();
		buf[i][1]=_p.getY();
		buf[i][2]=0.0;
		pTM->ContourVertex(buf[i]);
	}
	pTM->EndContour();
	pTM->EndPolygon();
	pTM->End();*/
	DrawToScene(dAlt);


}
void DeiceBayProc::DrawToScene(double dAlt){
	///*static const float xcoord[]={0.0002f,0.0f,0.0f,0.0f};
	//static const float ycoord[]={0.0f,0.0002f,0.0f,0.0f};
	//
	//glBindTexture(GL_TEXTURE_2D,texlist[(int)Textures::TextureEnum::Deice_Station]);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	//glTexGenfv(GL_S,GL_OBJECT_PLANE,xcoord);
	//glEnable(GL_TEXTURE_GEN_S);

	//glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	//glTexGenfv(GL_T,GL_OBJECT_PLANE,ycoord);
	//glEnable(GL_TEXTURE_GEN_T);
	//glEnable(GL_TEXTURE_2D);	
	//
	//glColor3d(1.0,1.0,1.0);
	//GLdouble buf[256][3];
	//GLUtesselatorObj *tess;
	//tess=gluNewTess();
	//gluTessCallback(tess, GLU_TESS_BEGIN, (GLvoid (__stdcall *) ( )) &glBegin);    
	//gluTessCallback(tess, GLU_TESS_END, (GLvoid (__stdcall *) ( )) &glEnd);
	//gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (__stdcall *) ( )) &glVertex3dv);
	//gluTessCallback(tess, GLU_TESS_EDGE_FLAG, (GLvoid (__stdcall *) ( )) &glEdgeFlag);
	//gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (__stdcall *) ( )) &combineCallback);
	//gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid (__stdcall *) ( )) &errorCallback);
	//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
	//gluTessBeginPolygon(tess,NULL);
	//gluTessBeginContour(tess);	
	//
	//for( int i=0;i<m_location.getCount();i++){
	//	Point  _p=m_location.getPoint(i);
	//	buf[i][0]=_p.getX();
	//	buf[i][1]=_p.getY();
	//	buf[i][2]=dAlt;

	//	gluTessVertex(tess,buf[i],buf[i]);
	//}
	//gluTessEndContour(tess);
	//gluTessEndPolygon(tess);
	//gluDeleteTess(tess);
	//
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_TEXTURE_GEN_S);
	//glDisable(GL_TEXTURE_GEN_T);*/
	/*static const GLdouble xcoord[]={0.001,0.0,0.0,0.0};
	static const GLdouble ycoord[]={0.0,0.001,0.0,0.0};
	glBindTexture(GL_TEXTURE_2D,texlist[(int)Textures::TextureEnum::Deice_Station]);
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
	Path * pWay=serviceLocationPath();
	for( int i=0;i<pWay->getCount();i++){
		Point & _p=pWay->getPoint(i);
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
	glDisable(GL_TEXTURE_GEN_T);*/

	/*Point serverpt = getServicePoint(0);
	Path& inConstrain = *inConstraint();
	int nCont;
	if( ( nCont = inConstrain.getCount() ) >  1 ){
		ARCVector2 vdir(inConstrain.getPoint(nCont-1));
		vdir -= inConstrain.getPoint(nCont-2);		

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.2f,0.2f);
		glPushMatrix();
		glNormal3i(0,0,1);
		glTranslated(serverpt.getX(),serverpt.getY(),dAlt);
		glRotated(vdir.AngleFromCoorndinateX(),0,0,1);		
		
		DrawFlatSquare(GetLength(),GetWinspan());	
		
		glPopMatrix();
		glDisable(GL_POLYGON_OFFSET_FILL);
	}*/

}
std::vector<CProcessor2*> DeiceBayProc::getInBoundStands(CPROCESSOR2LIST* proclist){
	std::vector<CProcessor2*> inboundstand;
	inboundstand.clear();
	ARCPolygon *poly=new ARCPolygon(ARCPath(&m_location));
	for(size_t i=0;i<proclist->size();i++){
		CProcessor2* pProc2 = proclist->at(i);
		if(pProc2->GetFloor()==this->getFloor()&& pProc2->GetProcessor()->getProcessorType()==StandProcessor){
			if(poly->Contains( ARCPoint2(pProc2->GetProcessor()->getServicePoint(0).getX(),pProc2->GetProcessor()->getServicePoint(0).getY()))){
				inboundstand.push_back(pProc2);
			}
		}
	}
	return inboundstand;
}

ARCVector2 DeiceBayProc::GetDir() 
{
	/*Path& inConstrain = *inConstraint();
	int nCont;
	if( ( nCont = inConstrain.getCount() ) >  1 ){
		ARCVector2 vdir(inConstrain.getPoint(nCont-1));
		vdir -= inConstrain.getPoint(nCont-2);		
		return vdir.Normalize();
	}*/
	return ARCVector2(1,0);
}