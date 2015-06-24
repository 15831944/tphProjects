#include "StdAfx.h"
#include ".\stretchrenderer.h"
#include <Common/ShapeGenerator.h>
#include "./LandsideDocument.h"
#include "../Common/ARCColor.h"
#include "./OglTextureResource.h"
#include "./TermPlanDoc.h"
#include "./3DView.h"
#include "./SelectionHandler.h"
#include "./Processor2.h"
#include "./ShapeRenderer.h"
#include "../Common/math/tCubicSpline.h"
#include "glrender/REColor.h"
#include "glrender/glTextureResource.h"
#include "glrender/REMesh.h"
#include "../Main/ChildFrm.h"



StretchRenderer::StretchRenderer(CProcessor2* pProc2):ProcessorRenderer(pProc2)
{
	m_ndisplist = -1;
	m_nboundList = -1;
	m_pTexture = NULL;
	Update();
}

StretchRenderer::~StretchRenderer(void)
{
	glDeleteLists(m_ndisplist,1);
	glDeleteLists(m_nboundList,1);
	
}

void StretchRenderer::Render(C3DView* pView)
{

	glCullFace(GL_BACK);
	////path points
	StretchProc * pProc = (StretchProc*) getProcessor();	
	
	if(NeedSync())
	{
		GenerateDispList(pView);	
		DoSync();
	}	
	
	//glEnd();
	glEnable(GL_LIGHTING);
	Renderer::glRenderMesh(m_pMesh.get());


	
	//render the cylinda  and the base 
	GLUquadricObj * pObj = gluNewQuadric();
	ARCPath3 thePath = pProc->GetPath();
	ShapeGenerator::WidthPipePath  widepath;
	ShapeGenerator::GenWidthPipePath(thePath,pProc->GetLaneNum()* pProc->GetLaneWidth(),widepath);

	ARCVector3 squad[8];
	double baseheight = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(0);
	for(int i=0; i< (int)thePath.size(); i++ ){
		
		for(int j =0;j<pProc->GetLaneNum();j++){
			float t = (j+0.5f)/pProc->GetLaneNum();
			ARCVector3 pos = widepath.leftpath[i] * (1-t) + widepath.rightpath[i] * t;
			if(pos[VZ]<0.3)continue;
			pos[VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(pos[VZ]);

			glColor3ubv(ARCColor3::WHITE);
			glPushMatrix();
			glTranslated(pos[VX],pos[VY],baseheight);
			gluCylinder(pObj,150,150,pos[VZ]-50-baseheight,36,12);
			glPopMatrix();
		}

	}
	gluDeleteQuadric(pObj);

	ARCPath3 path = pProc->GetPath();
	if(GetSelectMode() == ProcessorRenderer::SelectCtrlPoints )
	{

		for(int i=0;i<(int)path.size();i++)
		{					
			glPushMatrix();
			ARCVector3 pos = path.at(i);
			pos[VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(pos[VZ]);
			glTranslated(pos[VX],pos[VY],pos[VZ]);			
			glPopMatrix();			
		}		
	}
}

void StretchRenderer::DoSync()
{
	m_bNeedSync = false;
}


void StretchRenderer::GenerateDispList(C3DView * pView)
{
	const static double lanesidewidth = StretchProc::lanesidewidth; 
	const static double dthickness = StretchProc::lanethickness;

	StretchProc * pProc = (StretchProc*) getProcessor();	
	/*ARCPath3 path = pProc->GetPath();
	int nptCnt = (int) path.size();*/
	/*for(int i=0;i<nptCnt;i++ )
	{
		path[i][VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(path[i][VZ]);
	}*/
	ShapeGenerator::WidthPipePath lanepipepath =  pProc->getStretchPipePath();	
	
	int nptCnt = (int) lanepipepath.leftpath.size();
	for(int i=0;i<nptCnt;i++ ){
		lanepipepath.leftpath[i][VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(lanepipepath.leftpath[i][VZ]);//+dthickness;
		lanepipepath.rightpath[i][VZ] =  pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(lanepipepath.rightpath[i][VZ]);//;+dthickness;
	}

	m_pMesh = new Renderer::Mesh();  m_pMesh->nextMesh = new Renderer::Mesh(); m_pMesh->nextMesh->nextMesh = new Renderer::Mesh();
	Renderer::Mesh * pMesh = m_pMesh.get();
	Renderer::Mesh * pSideMesh = m_pMesh->nextMesh.get();
	Renderer::Mesh * pFenceMesh = pSideMesh->nextMesh.get();

	pMesh->name = "main";
	pSideMesh->name = "side";
	pFenceMesh->name = "fence";
	//Generate vertex
	int mainVertexCnt = pProc->GetLaneNum() +1;
	int sideVertexCnt = 6;
	pMesh->vertexs.resize( mainVertexCnt * nptCnt );
	pSideMesh->vertexs.resize(sideVertexCnt * nptCnt);

	Renderer::Vertex newvex;

	double halfStretchWidth =0.5 * pProc->GetLaneNum() * pProc->GetLaneWidth();
	double roadlength = 0;

	ARCVector3 precenter = 0.5 * (lanepipepath.leftpath[0] + lanepipepath.rightpath[0]);
	
	for(int i=0;i<nptCnt;++i){
		
		ARCVector3 v1, v2 ;
		v1 = lanepipepath.leftpath[i]; v2 = lanepipepath.rightpath[i];
		ARCVector3 vdir = v1 - v2; 		
		vdir.Normalize();

		ARCVector3 vcenter = 0.5 * (v1 + v2);
		
		roadlength += vcenter.DistanceTo(precenter);

		// main mesh vertex		
		double len = halfStretchWidth ; 
		for(int j=0;j<pProc->GetLaneNum()+1;j++){
			pMesh->vertexs[i * mainVertexCnt +j].point =  vcenter  + vdir * len;
			double texcord = -0.5 +j;
			if(j==0) texcord += 0.1;
			if(j==pProc->GetLaneNum()) texcord -= 0.1;
			pMesh->vertexs[i * mainVertexCnt + j].texcord = ARCVector2(roadlength/700,texcord);
			len -= pProc->GetLaneWidth();
		}
		precenter = vcenter;
		
		//side Mesh vertex
		{
			
			pSideMesh->vertexs[i* sideVertexCnt].point = vcenter + halfStretchWidth * vdir;
			pSideMesh->vertexs[i* sideVertexCnt].texcord = ARCVector2(0,i);
			pSideMesh->vertexs[i* sideVertexCnt+1].point = vcenter + (halfStretchWidth + lanesidewidth) * vdir;
			pSideMesh->vertexs[i* sideVertexCnt+1].texcord = ARCVector2(1,i);
			pSideMesh->vertexs[i* sideVertexCnt+2].point = vcenter - (halfStretchWidth ) * vdir;
			pSideMesh->vertexs[i* sideVertexCnt+2].texcord = ARCVector2(0,i);
			pSideMesh->vertexs[i* sideVertexCnt+3].point = vcenter - (halfStretchWidth + lanesidewidth ) * vdir;
			pSideMesh->vertexs[i* sideVertexCnt+3].texcord = ARCVector2(1,i);
			pSideMesh->vertexs[i* sideVertexCnt+4].point = pSideMesh->vertexs[i* sideVertexCnt+1].point - ARCVector3(0,0,dthickness);
			pSideMesh->vertexs[i* sideVertexCnt+5].point = pSideMesh->vertexs[i* sideVertexCnt+3].point - ARCVector3(0,0,dthickness);
			//thickness
		}
	}	
	
	Renderer::Material laneMat;
	laneMat.texture = "Lane";
	pMesh->materials["Lane"] = laneMat;
	laneMat.texture = "LaneWithMark";
	pMesh->materials["LaneWithMark"] = laneMat;

	// main mesh face 
	pMesh->faces.resize((nptCnt-1)*pProc->GetLaneNum());
	for(int i=0;i<nptCnt-1;i++){
		for(int j=0;j<pProc->GetLaneNum();j++){
			int idx = i * pProc->GetLaneNum() + j;
			int vexid = i * pProc->GetLaneNum() + i + j;
			pMesh->faces[idx].vertexs.resize(4);
			pMesh->faces[idx].vertexs[0] = vexid; pMesh->faces[idx].vertexs[1] = vexid +1; pMesh->faces[idx].vertexs[2] = vexid + 1 + mainVertexCnt; pMesh->faces[idx].vertexs[3] = vexid + mainVertexCnt;
			if( i<2 || i > (nptCnt-4) )
				pMesh->faces[idx].material = "LaneWithMark";
			else
				pMesh->faces[idx].material = "Lane";

			pMesh->faces[idx].faceType = GL_QUADS;
		}		
	}

	laneMat.texture = "StertchSide";
	pSideMesh->materials["StertchSide"] = laneMat;
	//side mesh face
	int faceCnt = 3;
	pSideMesh->faces.resize((nptCnt-1)*faceCnt);
	for(int i=0;i<nptCnt-1;i++){
		int idx = i * faceCnt;
		int vexidx = i * sideVertexCnt;
		pSideMesh->faces[idx].vertexs.resize(4);
		pSideMesh->faces[idx].vertexs[0] = vexidx +1 ; pSideMesh->faces[idx].vertexs[1] = vexidx ;pSideMesh->faces[idx].vertexs[2] = vexidx + sideVertexCnt;   pSideMesh->faces[idx].vertexs[3] = vexidx + sideVertexCnt +1;
		pSideMesh->faces[idx].material = "StertchSide";
		pSideMesh->faces[idx].faceType = GL_QUADS;

		idx ++; vexidx += 2;
		pSideMesh->faces[idx].vertexs.resize(4);
		pSideMesh->faces[idx].vertexs[0] = vexidx; pSideMesh->faces[idx].vertexs[1] = vexidx+1 ;pSideMesh->faces[idx].vertexs[2] = vexidx + sideVertexCnt+1;   pSideMesh->faces[idx].vertexs[3] = vexidx + sideVertexCnt;
		pSideMesh->faces[idx].material = "StertchSide";
		pSideMesh->faces[idx].faceType = GL_QUADS;

		idx ++ ; vexidx = i * sideVertexCnt;
		pSideMesh->faces[idx].vertexs.resize(8);
		pSideMesh->faces[idx].vertexs[0] =vexidx + 1;		pSideMesh->faces[idx].vertexs[1] = vexidx + 1 + sideVertexCnt;
		pSideMesh->faces[idx].vertexs[2] =vexidx + 4;		pSideMesh->faces[idx].vertexs[3] = vexidx + 4 + sideVertexCnt;
		pSideMesh->faces[idx].vertexs[4] =vexidx + 5;		pSideMesh->faces[idx].vertexs[5] = vexidx + 5 + sideVertexCnt;
		pSideMesh->faces[idx].vertexs[6] =vexidx + 3;		pSideMesh->faces[idx].vertexs[7] = vexidx + 3 + sideVertexCnt;
		pSideMesh->faces[idx].material = "StertchSide";
		pSideMesh->faces[idx].faceType = GL_QUAD_STRIP;
	}
	// add the fence mesh
	int vexperPt = 8;
	int facesperPt = 2;
	pFenceMesh->vertexs.resize( (nptCnt -1)* vexperPt);
	pFenceMesh->faces.resize(facesperPt * (nptCnt -1) );
	Renderer::Material mat;
	mat.texture = "Roadbed";
	pFenceMesh->materials["Roadbed"] = mat;
	mat.texture = "Fence";
	pFenceMesh->materials["Fence"] = mat;


	double lastrelatheight =100;
	for(int i=0;i<nptCnt-1;i++){
		ARCVector3 v1, v2 ; ARCVector3 v3, v4;
		v1 = lanepipepath.leftpath[i]; v2 = lanepipepath.rightpath[i];
		v3 = lanepipepath.leftpath[i+1]; v4 = lanepipepath.rightpath[i+1];
		ARCVector3 vdir1 = v1 - v2;
		ARCVector3 vdir2 = v3 - v4;
		vdir1.Normalize(); vdir2.Normalize();

		ARCVector3 vcenter1 = 0.5 * (v1 + v2);
		ARCVector3 vcenter2 = 0.5 * (v3 +v4);

		double relateheight = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleFloorAltitude(vcenter1[VZ])/100.0;
		int vexid = i * vexperPt;
		int faceid = i * facesperPt;
		
		double drate = 1.0;
		pFenceMesh->faces[faceid].vertexs.resize(4);pFenceMesh->faces[faceid+1].vertexs.resize(4);
		
		double baseheight =  pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(0);
		
		if( (relateheight) < 0.3){
			
			double bheight = v1[VZ] - baseheight; if(bheight<0)bheight = 0;
			double dlen = drate * bheight + halfStretchWidth + lanesidewidth;
			ARCVector3 vex = vcenter1 + dlen * vdir1;  if(bheight>0)vex[VZ] = baseheight;
			pFenceMesh->vertexs[vexid].point = vex; pFenceMesh->vertexs[vexid].texcord = ARCVector2(i,0);
			dlen = halfStretchWidth + lanesidewidth;
			pFenceMesh->vertexs[vexid +1].point = vcenter1 + dlen * vdir1; pFenceMesh->vertexs[vexid+1].texcord = ARCVector2(i,1);

			bheight = v2[VZ] - baseheight; if(bheight<0)bheight = 0;
			dlen = drate * bheight + halfStretchWidth + lanesidewidth;
			vex = vcenter1 - dlen * vdir1; if(bheight>0)vex[VZ] = baseheight;
			pFenceMesh->vertexs[vexid + 2].point = vex; pFenceMesh->vertexs[vexid+2].texcord = ARCVector2(i,0);
			dlen = halfStretchWidth + lanesidewidth;
			vex = vcenter1 - dlen * vdir1;
			pFenceMesh->vertexs[vexid + 3].point = vex; pFenceMesh->vertexs[vexid+3].texcord = ARCVector2(i,1);

			

			bheight = v3[VZ] - baseheight; if(bheight<0)bheight = 0;
			dlen = drate * bheight + halfStretchWidth + lanesidewidth;
			vex = vcenter2 + dlen * vdir2; if(bheight>0)vex[VZ] = baseheight ;
			pFenceMesh->vertexs[vexid + 4].point = vex;		 pFenceMesh->vertexs[vexid+4].texcord = ARCVector2(i+1,0);	
			dlen = halfStretchWidth + lanesidewidth;
			vex = vcenter2 + dlen * vdir2;
			pFenceMesh->vertexs[vexid + 5].point = vex;		 pFenceMesh->vertexs[vexid+5].texcord = ARCVector2(i+1,1);

			bheight = v4[VZ] - baseheight; if(bheight<0)bheight = 0;
			dlen  = drate * bheight + halfStretchWidth + lanesidewidth;
			vex = vcenter2 - dlen * vdir2; if(bheight>0)vex[VZ] = baseheight;
			pFenceMesh->vertexs[vexid +6].point = vex;     pFenceMesh->vertexs[vexid+6].texcord = ARCVector2(i+1,0);
			dlen = halfStretchWidth + lanesidewidth;
			vex = vcenter2 - dlen * vdir2 ;
			pFenceMesh->vertexs[vexid + 7].point = vex;     pFenceMesh->vertexs[vexid+7].texcord = ARCVector2(i+1,1);


			pFenceMesh->faces[faceid].vertexs[0] = vexid; pFenceMesh->faces[faceid].vertexs[1] = vexid +1;
			pFenceMesh->faces[faceid].vertexs[2] = vexid+4; pFenceMesh->faces[faceid].vertexs[3] = vexid+5;
			pFenceMesh->faces[faceid].material = "Roadbed";
			faceid++;
			pFenceMesh->faces[faceid].vertexs[0] = vexid + 3; pFenceMesh->faces[faceid].vertexs[1] = vexid +2;
			pFenceMesh->faces[faceid].vertexs[2] = vexid+7; pFenceMesh->faces[faceid].vertexs[3] = vexid+6;
			pFenceMesh->faces[faceid].material = "Roadbed";

			if(lastrelatheight >= 0.3) {
				Renderer::Face newFace;
				newFace.vertexs.resize(4);
				newFace.vertexs[0] = vexid; newFace.vertexs[1] = vexid +1;newFace.vertexs[2]= vexid +2; newFace.vertexs[3] = vexid +3;
				newFace.material = "Roadbed";
				pFenceMesh->faces.push_back( newFace );
			}

		}else {
			double fenceheight = 150;
			double dlen = halfStretchWidth + lanesidewidth;
			ARCVector2 texcord;
			ARCVector3 vex = vcenter1 + dlen * vdir1;  vex[VZ] += fenceheight;
			pFenceMesh->vertexs[vexid].point = vex;
			pFenceMesh->vertexs[vexid].texcord = ARCVector2(i,0);
			pFenceMesh->vertexs[vexid +1].point = vcenter1 + dlen * vdir1;	
			pFenceMesh->vertexs[vexid+1].texcord = ARCVector2(i,1);
			
			vex = vcenter1 - dlen * vdir1; vex[VZ] += fenceheight;
			pFenceMesh->vertexs[vexid + 2].point = vex;
			pFenceMesh->vertexs[vexid + 2].texcord = ARCVector2(i,0);
			vex = vcenter1 - dlen * vdir1;
			pFenceMesh->vertexs[vexid + 3].point = vex;		
			pFenceMesh->vertexs[vexid + 3].texcord = ARCVector2(i,1);
			
			vex = vcenter2 + dlen * vdir2; vex[VZ] += fenceheight ;
			pFenceMesh->vertexs[vexid + 4].point = vex;		
			pFenceMesh->vertexs[vexid + 4].texcord = ARCVector2(i+1,0);	
			vex = vcenter2 + dlen * vdir2;
			pFenceMesh->vertexs[vexid + 5].point = vex;		
			pFenceMesh->vertexs[vexid + 5].texcord = ARCVector2(i+1,1);	
		
			vex = vcenter2 - dlen * vdir2; vex[VZ] += fenceheight;
			pFenceMesh->vertexs[vexid +6].point = vex;	
			pFenceMesh->vertexs[vexid + 6].texcord = ARCVector2(i+1,0);	
			vex = vcenter2 - dlen * vdir2 ;
			pFenceMesh->vertexs[vexid + 7].point = vex;
			pFenceMesh->vertexs[vexid + 7].texcord = ARCVector2(i+1,1);	
			
			pFenceMesh->faces[faceid].vertexs[0] = vexid; pFenceMesh->faces[faceid].vertexs[1] = vexid +1;
			if(relateheight - (int)relateheight < 0.1) {
				pFenceMesh->faces[faceid].vertexs[2] = vexid; pFenceMesh->faces[faceid].vertexs[3] = vexid+1;
			}else{
				pFenceMesh->faces[faceid].vertexs[2] = vexid+4; pFenceMesh->faces[faceid].vertexs[3] = vexid+5;
			}
			pFenceMesh->faces[faceid].material = "Fence";
			faceid++;
			pFenceMesh->faces[faceid].vertexs[0] = vexid + 3; pFenceMesh->faces[faceid].vertexs[1] = vexid +2;
			if(relateheight - (int)relateheight < 0.1) {
				pFenceMesh->faces[faceid].vertexs[2] = vexid+3; pFenceMesh->faces[faceid].vertexs[3] = vexid+2;
			}else{
				pFenceMesh->faces[faceid].vertexs[2] = vexid+7; pFenceMesh->faces[faceid].vertexs[3] = vexid+6;
			}
			pFenceMesh->faces[faceid].material = "Fence";

			if(lastrelatheight<0.3){
				Renderer::Face newFace;
				newFace.vertexs.resize(4);
				int lastvexid = vexid - 4 ;
				newFace.vertexs[0] = lastvexid; newFace.vertexs[1] = lastvexid +1;newFace.vertexs[2]= lastvexid +2; newFace.vertexs[3] = lastvexid +3;
				newFace.material = "Roadbed";
				pFenceMesh->faces.push_back( newFace );
			}
		}


		lastrelatheight = relateheight;
	}


	pMesh->build(pView->getTextureResource());
	
}
void StretchRenderer::RenderSelect(C3DView * pView,int selidx)
{	
	
	StretchProc * pProc = (StretchProc*) getProcessor();
	
	ARCPath3 path = pProc->GetPath();
	if(GetSelectMode() == ProcessorRenderer::SelectCtrlPoints )
	{
		
		for(int i=0;i<(int)path.size();i++)
		{		
			int subselid = i;
			glLoadName(NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_POINT,selidx,subselid));
			glPushMatrix();
			ARCVector3 pos = path.at(i);
			pos[VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(pos[VZ]);
			glTranslated(pos[VX],pos[VY],pos[VZ]);
			
			glPopMatrix();			
		}		
	}
	
	ShapeGenerator::WidthPipePath widePath = pProc->getStretchPipePath();	

	int nptCnt = (int) widePath.leftpath.size();
	for(int i=0;i<nptCnt;i++ ){
		widePath.leftpath[i][VZ] = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(widePath.leftpath[i][VZ]);
		widePath.rightpath[i][VZ] =  pView->GetDocument()->GetFloorByMode(EnvMode_LandSide).getVisibleAltitude(widePath.rightpath[i][VZ]);
	}	

	
	if(nptCnt ==2 )
	{
		ARCPath3::iterator itr;
		itr =widePath.leftpath.begin(); itr++;
		widePath.leftpath.insert(itr,0.5*( widePath.leftpath[0] + widePath.leftpath[1] ) );
		itr = widePath.rightpath.begin();itr++;
		widePath.rightpath.insert(itr,0.5*( widePath.rightpath[0] + widePath.rightpath[1] ) );
		nptCnt ++;
	}
	for( int i =0;i<nptCnt-1;i++)
		{		
			ARCVector3 centerPoint1 = 0.5*(widePath.leftpath[i] + widePath.rightpath[i] );
			ARCVector3 centerPoint2 = 0.5*( widePath.leftpath[i+1] + widePath.rightpath[i+1] );

			int subselid  = i* 2 ;	
			glLoadName( NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_MAIN,selidx,subselid) );
			glBegin(GL_QUADS);
			glVertex3dv(widePath.leftpath[i]);
			glVertex3dv(widePath.leftpath[i+1] );
			glVertex3dv( centerPoint2 );
			glVertex3dv( centerPoint1 );
			glEnd();
			subselid = i*2 +1;
			glLoadName( NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_MAIN,selidx,subselid) );
			glBegin(GL_QUADS);
			glVertex3dv(widePath.rightpath[i]);
			glVertex3dv(widePath.rightpath[i+1] );
			glVertex3dv( centerPoint2 );
			glVertex3dv( centerPoint1 );
			glEnd();		

		}
	
	
}
