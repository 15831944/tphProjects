#include "StdAfx.h"
#include ".\laneadapterrenderer.h"
#include "../Engine/StretchProc.h"
#include "../Engine/LaneAdapterProc.h"
#include "./3DView.h"
#include "./TessellationManager.h"
#include <Common/ShapeGenerator.h>
#include "./Common/pollygon.h"
#include "./TermPlanDoc.h"
#include "./SelectionHandler.h"
#include "../Common/math/tCubicSpline.h"

GLdouble LaneAdapterRenderer::m_dthickness = 0.1 * SCALE_FACTOR;
GLfloat LaneAdapterColor[3] = { 0.5f,0.5f,0.5f };

LaneAdapterRenderer::LaneAdapterRenderer(CProcessor2 * pProc2) : ProcessorRenderer(pProc2)
{
	m_pTexture = NULL;
	m_iboundinglist = 0;
	m_isurdisplist = 0;
	Update();
}

LaneAdapterRenderer::~LaneAdapterRenderer(void)
{
	glDeleteLists(m_isurdisplist,1);
	glDeleteLists(m_iboundinglist,1);
	if(!m_pTexture) m_pTexture->unRef();
}

void LaneAdapterRenderer::GenDisplaylist(C3DView* pView)
{
	double dthickness = StretchProc::lanethickness;
	
	/*std::vector<StretchProc*> pProcs; pProcs.resize(linkProcs.size());
	StretchProc * pStrech1 = (StretchProc*)linkProcs[0].pProc;
	StretchProc * pStrech2 = (StretchProc*)linkProcs[linkProcs.size()-1].pProc;
	*/
	

	////generate smooth point
	//ARCVector3 p1,p2;          //center point
	//ARCVector3 Lp1,Lp2,Rp1,Rp2;     //side points
	//ARCVector3 v1,v2;               //orient vector
	//double width1,width2; 
	//
	//LandProcPartArray linkProcs =((LandfieldProcessor*) getProcessor())->GetLinkedProcs();
	//if(linkProcs.size() <2 )return;
	////left points
	//StretchProc * pStrech1 = (StretchProc*)linkProcs[0].pProc;
	//StretchProc * pStrech2 = (StretchProc*)linkProcs[linkProcs.size()-1].pProc;


	//ARCPath3 path1 = pStrech1->GetPath();
	//ARCPath3 path2 = pStrech2->GetPath();
	//width1 = pStrech1->GetLaneNum() * pStrech1->GetLaneWidth();
	//width2 = pStrech2->GetLaneWidth() *pStrech2->GetLaneNum();

	//GLfloat t1,t2;
	//t1 = (GLfloat) pStrech1->GetLaneNum();
	//t2 = (GLfloat) pStrech2->GetLaneNum();

	//if( linkProcs[0].part/2 < 1 )
	//{
	//	p1 = path1[0];
	//	v1 = path1[1];
	//}
	//else
	//{
	//	p1 = path1[path1.size()-1];
	//	v1 = path1[path1.size()-2];
	//}

	//if(linkProcs[linkProcs.size()-1].part/2 <1)
	//{
	//	p2 = path2[0];
	//	v2 = path2[1];
	//}
	//else
	//{
	//	p2 = path2[path2.size()-1];
	//	v2 = path2[path2.size()-2];
	//}
	//
	//CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide);
	//p1[VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude( p1[VZ] ) + StretchProc::lanethickness;
	//p2[VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude( p2[VZ] ) + StretchProc::lanethickness;
	//v1[VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude( v1[VZ] ) + StretchProc::lanethickness;
	//v2[VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude( v2[VZ] ) + StretchProc::lanethickness;
	//
	//ARCPath3 path;
	//path.resize(4);
	//path[0]=v1;path[1]= p1;path[2]=p2;path[3]=v2;
	//typedef tCubicSpline<ARCPath3::value_type> CubicSpline;

	//int interpoltCnt = StretchProc::interpoltCnt;
	//int nPtCnt = path.size();
	//int nPtCntNew = interpoltCnt * (nPtCnt-1) +1;
	//CubicSpline splineC;

	//splineC.SetControlPoints(path.begin(),path.end());

	//path.resize(nPtCntNew);
	//for(int i=0;i<nPtCntNew;i++){
	//	float t = (float)i/interpoltCnt;
	//	path[i] = splineC.eval(t);
 //	}

	//ShapeGenerator::WidthPipePath widpath;
	//ShapeGenerator::GenWidthPipePath(path,1,widpath);
	//
	//for(size_t i=0;i<path.size();i++){
	//	ARCVector3 v = widpath.leftpath[i] - widpath.rightpath[i];
	//	ARCVector3 vcenter = 0.5 * (widpath.leftpath[i] + widpath.rightpath[i]);
	//	float t =(float)i/(path.size()-1);
	//	double len = width1 * (1-t) + width2 * t;
	//
	//	widpath.leftpath[i] = vcenter + v *0.5 * len ;
	//	widpath.rightpath[i] = vcenter - v * 0.5 * len; 
	//}
	//

	


	//const static int SmoothPointCount =15; 
	//v1.Normalize();v2.Normalize();
	//ARCVector3 widv1 = ARCVector3(-v1[VY],v1[VX],0);
	//ARCVector3 widv2 = ARCVector3(-v2[VY],v2[VX],0);
	//Lp1 = p1 + 0.5 * width1 * widv1;
	//Rp1 = p1 - 0.5 * width1 * widv1;
	//Lp2 = p2 + 0.5 * width2 * widv2;
	//Rp2 = p2 - 0.5 * width2 * widv2;
	//double len = Lp1.DistanceTo(Lp2);

	//v1 *= (len * 0.5);
	//v2 *= (len * 0.5);
	//ARCPath3 input; input.reserve(4);
	//input.push_back(Lp1);
	//input.push_back(Lp1 + v1);
	////input.push_back(0.5*(Lp1+Lp2));
	//input.push_back(Lp2 - v2);
	//input.push_back(Lp2);
	//SHAPEGEN.GenBezierPath(input,widePath.leftpts,SmoothPointCount);
	////
	//input.clear();
	//input.push_back(Rp1);
	//input.push_back(Rp1 + v1);
	////input.push_back(0.5*(Rp1+ Rp2));
	//input.push_back(Rp2 - v2);
	//input.push_back(Rp2);
	//SHAPEGEN.GenBezierPath(input,widePath.rightpts,SmoothPointCount);
	

	//draw the generate block
	//glDeleteLists(m_isurdisplist,1);
	//m_isurdisplist = glGenLists(1);
	//glNewList(m_isurdisplist,GL_COMPILE);

	//GLfloat sL=0;GLfloat sR=0;   //texcoord s;
	////GLfloat s = 0;

	//GLfloat t;

	//int nptCnt =(int) widePath.leftpts.size() ;
	//for(int i=0;i<nptCnt-1;i++)
	//{
	//	//if(i < nptCnt/2)
	//		//t = t1;
	//	//else t = t2;
	//	t = 1.0;
	//	glColor3fv(LaneAdapterColor);
	//	glBegin(GL_QUAD_STRIP);
	//	glTexCoord2f(sL,0);	glVertex3dv(widePath.leftpts[i]);
	//	glTexCoord2f(sR,t);  glVertex3dv(widePath.rightpts[i]);
	//	//s += (GLfloat)( 0.5*(widePath.leftpts[i] + widePath.leftpts[i+1] ) ).DistanceTo( 0.5*(widePath.rightpts[i] + widePath.rightpts[i+1] ) );
	//	sL += (GLfloat) (widePath.leftpts[i].DistanceTo(widePath.leftpts[i+1]) / SCALE_FACTOR); 
	//	sR += (GLfloat) (widePath.rightpts[i].DistanceTo(widePath.rightpts[i+1]) / SCALE_FACTOR);
	//	glTexCoord2f(sL,0);	glVertex3dv(widePath.leftpts[i+1]);
	//	glTexCoord2f(sR,t);	glVertex3dv(widePath.rightpts[i+1]);
	//	std::swap(sL,sR);

	//	glEnd();
	//}	
	//glEndList();

	//glDeleteLists(m_iboundinglist,1);
	//m_iboundinglist = glGenLists(1);
	//glNewList(m_iboundinglist,GL_COMPILE);
	//glEnable(GL_LIGHTING);
	//glBegin(GL_QUAD_STRIP);
	//ARCVector3 thickVect(0,0,-m_dthickness);
	//for(int i=0;i<nptCnt-1;i++)
	//{
	//	ARCVector3 _normal(widePath.leftpts[i] - widePath.leftpts[i+1]);
	//	glNormal3d( -_normal[VY],_normal[VX],_normal[VZ] );
	//	glVertex3dv(widePath.leftpts[i]);glVertex3dv(widePath.leftpts[i]+thickVect);
	//	glVertex3dv(widePath.leftpts[i+1]);glVertex3dv(widePath.leftpts[i+1]+thickVect);
	//}
	//glEnd();
	//glBegin(GL_QUAD_STRIP);
	//for(int i=0;i<nptCnt-1;i++)
	//{
	//	ARCVector3 _normal(widePath.rightpts[i] - widePath.rightpts[i+1]);
	//	glNormal3d( -_normal[VY],_normal[VX],_normal[VZ] );
	//	glVertex3dv(widePath.rightpts[i]);glVertex3dv(widePath.rightpts[i]+thickVect);
	//	glVertex3dv(widePath.rightpts[i+1]);glVertex3dv(widePath.rightpts[i+1]+thickVect);
	//}
	//glEnd();
	//glDisable(GL_LIGHTING);
	//glEndList();	

	//m_pMesh = new Renderer::Mesh();Renderer::Mesh *pMesh  = m_pMesh.get();
	//m_pMesh->name = "main";
	//
	//int mainVertexCnt = 2; int sideVertexCnt = 6;
	//	
	//Renderer::Material laneadpaterMat;
	//laneadpaterMat.texture = "LaneAdapter";
	//pMesh->materials["LaneAdapter"] = laneadpaterMat;

	//pMesh->vertexs.resize(path.size() * mainVertexCnt);
	//for(size_t i=0;i<path.size();i++){
	//	pMesh->vertexs[i*2].point = widpath.leftpath[i];
	//	pMesh->vertexs[i*2].texcord = ARCVector2(0,i);
	//	pMesh->vertexs[i*2+1].point = widpath.rightpath[i];
	//	pMesh->vertexs[i*2+1].texcord = ARCVector2(1,i);
	//}
	////faces
	//pMesh->faces.resize(path.size()-1);
	//for(size_t i=0;i<path.size()-1;i++){
	//	pMesh->faces[i].vertexs.resize(4);
	//	pMesh->faces[i].vertexs[0]= i*mainVertexCnt;
	//	pMesh->faces[i].vertexs[1]= i*mainVertexCnt+1;
	//	pMesh->faces[i].vertexs[2]=i*mainVertexCnt +3;
	//	pMesh->faces[i].vertexs[3]=i*mainVertexCnt+2 ;
	//	pMesh->faces[i].faceType = GL_QUADS;
	//	pMesh->faces[i].material = "LaneAdapter";
	//}
	//pMesh->build(pView->getTextureResource());

}

void LaneAdapterRenderer::Render(C3DView* pView)
{

	if(NeedSync())
	{
		GenDisplaylist(pView);
		DoSync();
	}

	//if(!m_pTexture)	
	//	m_pTexture  = &( pView->getTextureRes().RefTextureByType(OglTextureResource::LaneAdapterTexture) );

	////ASSERT()
	//m_pTexture->Activate();	
	////glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	////	glCallList(m_isurdisplist);
	//m_pMesh->
	//m_pTexture->DeActivate();
	/*glCallList(m_iboundinglist);
	glPushMatrix();
	glTranslated(0,0,-m_dthickness);
	glCallList(m_isurdisplist);
	glPopMatrix();*/
	Renderer::glRenderMesh(m_pMesh.get());

	//glDisable(GL_TEXTURE_2D);

}
void LaneAdapterRenderer::RenderSelect(C3DView * pView,int selidx)
{
	if(NeedSync())
	{
		GenDisplaylist(pView);
		DoSync();
	}

	glLoadName(NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_MAIN,selidx,0));
	glCallList(m_isurdisplist);
}