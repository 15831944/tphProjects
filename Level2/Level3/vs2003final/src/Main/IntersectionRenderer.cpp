#include "StdAfx.h"
#include ".\intersectionrenderer.h"
#include "../Engine/StretchProc.h"
#include "../Engine/IntersectionProc.h"
#include "./3DView.h"
#include "./TessellationManager.h"
#include <Common/ShapeGenerator.h>
#include "./Common/pollygon.h"
#include "./TermPlanDoc.h"
#include "./SelectionHandler.h"
#include "glrender/REColor.h"

GLdouble IntersectionRenderer::m_dthickness = 10;
IntersectionRenderer::IntersectionRenderer(CProcessor2* pProc2) : ProcessorRenderer(pProc2)
{
	m_pTexture = NULL;
	m_pZebraTexture = NULL;
	m_iboundinglist = -1;
	m_isurdisplist = -1;
	m_iZebralist = -1;
}

IntersectionRenderer::~IntersectionRenderer(void)
{
	glDeleteLists(m_isurdisplist,1);
	glDeleteLists(m_iboundinglist,1);
	glDeleteLists(m_iZebralist,1);
	if(!m_pTexture) m_pTexture->unRef();
	if(!m_pZebraTexture) m_pZebraTexture->unRef();
}

void IntersectionRenderer::GenDisplaylist(C3DView* pView)
{
	double widthofzebra = 20 * SCALE_FACTOR;
	LandProcPartArray linkProcs =((IntersectionProc*) getProcessor())->GetLinkedProcs();
	if(linkProcs.size() <2 )return;
	ASSERT(linkProcs.size() > 1 );
	//calculate the control points
	LandProcPartArray::iterator itr;
	std::vector<ARCVector3> ctrlpoints;
	std::vector<ARCVector3> orientats;
	std::vector<ARCVector3> ctrlpointsOut;
	//check to see the linked procs are in a counterclockwise order
	int ncount = (int)linkProcs.size();
	Pollygon poly;
	Point * pts = new Point[linkProcs.size()];	

	for(int i = 0;i< ncount;i++)
	{
		LandfieldProcessor* pStretch  = (LandfieldProcessor*) linkProcs[i].pProc;
		const ARCPath3& stretchpath = pStretch->GetPath();

		if( linkProcs[i].part / 2 >= 1 )
		{
			int pcount = (int) stretchpath.size();
			pts[i] = Point( stretchpath[pcount-1][VX],stretchpath[pcount-1][VY],stretchpath[pcount-1][VZ] );
		}
		else 
		{
			pts[i] = Point ( stretchpath[0][VX],stretchpath[0][VY],stretchpath[0][VZ]);
		}  
	}
	poly.init(linkProcs.size(),pts);
	delete[] pts;
	bool bcountclk= poly.IsCountClock();
	//make the linkprocs in CountClockOrder;
	if(!bcountclk)
	{	
		std::reverse(linkProcs.begin(),linkProcs.end());
	}
	//get the control points of the Intersection
	std::vector<int> laneNumbers;
	for( itr = linkProcs.begin();itr!=linkProcs.end();itr++ )
	{
		StretchProc* pStretch  = (StretchProc*) (*itr).pProc;
		if(!pStretch)continue;
		const ARCPath3& stretchpath = pStretch->GetPath();
		ARCVector3 v1, centerPt;                //v1 : stretch orientation ,centerPt : center point of stretch
		if( (*itr).part / 2 >= 1 )
		{
			int pcount = (int) stretchpath.size();
			v1 = ARCVector3( stretchpath[pcount-2] ,stretchpath[pcount-1] );
			centerPt = stretchpath[pcount-1];
		}
		else 
		{
			v1 = ARCVector3(stretchpath[1],stretchpath[0]);
			centerPt = stretchpath[0];
		}
		v1.Normalize();        
		orientats.push_back(v1);	
		double halfwidth = 0.5* pStretch->GetLaneWidth() * pStretch->GetLaneNum();
		laneNumbers.push_back(pStretch->GetLaneNum());
		ARCVector3 v2 = ARCVector3( -v1[VY],v1[VX],0);	
		widthofzebra = pStretch->GetLaneWidth() * 0.5;
		ctrlpointsOut.push_back( centerPt +  v2 * halfwidth  );
		ctrlpointsOut.push_back( centerPt -  halfwidth * v2  );
		ctrlpoints.push_back( centerPt +  v2 * halfwidth + v1*widthofzebra );
		ctrlpoints.push_back( centerPt -  halfwidth * v2 + v1*widthofzebra );
	}

	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_LandSide);
	for(size_t i=0;i<ctrlpoints.size();i++)
	{
		ctrlpoints[i][VZ] = floors.getVisibleAltitude(ctrlpoints[i][VZ]);
		ctrlpointsOut[i][VZ] = floors.getVisibleAltitude(ctrlpointsOut[i][VZ]);
	}
	//generate the zebras
	glDeleteLists(m_iZebralist,1);
	m_iZebralist = glGenLists(1);
	glNewList(m_iZebralist,GL_COMPILE);
	for(int i=0;i<(int)ctrlpointsOut.size()-1;i++)
	{
		GLfloat t = (GLfloat) laneNumbers[i/2];//( (ctrlpointsOut[i+1] - ctrlpointsOut[i]).Magnitude() /(50*SCALE_FACTOR) );
		glBegin(GL_QUADS);
			glTexCoord2f(0,0); glVertex3dv(ctrlpointsOut[i]);
			glTexCoord2f(0,t); glVertex3dv(ctrlpointsOut[i+1]);			
			glTexCoord2f(t,t); glVertex3dv(ctrlpoints[i+1]);
			glTexCoord2f(t,0); glVertex3dv(ctrlpoints[i]);			
		glEnd();
		i++;
	}
	glEndList();
	
	
	//generate the intersection center
	const static smoothPtcnt = 10;	

	std::vector<ARCVector3> ptsbuf;
	ptsbuf.reserve(256);
	for( size_t i =0; i< orientats.size() ;i++)
	{
		//calculate centerpoint
		ARCVector3 p1 = ctrlpoints[2*i+1];ARCVector3 p2;
		if(i<orientats.size()-1)
			p2 = ctrlpoints[2*i+2];
		else 
			p2 = ctrlpoints[0];
		ARCVector3 v1 = orientats[i];ARCVector3 v2;
		if( i<orientats.size()-1)
			v2 = orientats[i+1];
		else 
			v2 = orientats[0];

		ARCVector3 v3 = p2 - p1;
		double ratio = v1.GetCosOfTwoVector(v2);    
		ratio = sqrt( 1 - ratio * ratio ) ;
		if(ratio<1.0) ratio = 1.0;
		ratio = v3.Magnitude() / ratio;               // length/sin(a)
		
		double d1,d2;
		d1 = v2.GetCosOfTwoVector(v3);    //cos(a)
		d1 = sqrt(1- d1*d1);              //sin(a)
		d1 = d1 * ratio;

		d2 = v1.GetCosOfTwoVector(v3);
		d2 = sqrt(1-d2*d2);
		d2 = d2 * ratio;

		v1 *= d1; v2 *= d2;

		std::vector<ARCVector3> _inpoint;
		std::vector<ARCVector3> _outpoint;
		_inpoint.reserve(3);_outpoint.reserve(smoothPtcnt);
		_inpoint.push_back( p1 ); _inpoint.push_back( (p1+v1+p2+v2)*0.5 );
		_inpoint.push_back( p2 );

		//for(int j=0;j<3;j++)
		//ptsbuf.push_back(_inpoint[j]);
		SHAPEGEN.GenBezierPath(_inpoint,_outpoint,smoothPtcnt);
		for(int j=0;j<smoothPtcnt;j++)
			ptsbuf.push_back( _outpoint[j] ); 
	}

	GLenum genmode = GL_OBJECT_LINEAR;
	GLenum curplane = GL_OBJECT_PLANE;
	static float curcoeffs[] = {0.0001f, 0.0f, 0.0f, 0.0f};
	static float curcoefft[] = {0.00f, 0.0001f, 0.0f, 0.0f};	
	//surface display list
	glDeleteLists(m_isurdisplist,1);
	m_isurdisplist = glGenLists(1);
	glNewList(m_isurdisplist,GL_COMPILE);
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,genmode);
	glTexGenfv(GL_S,curplane,curcoeffs);
	glEnable(GL_TEXTURE_GEN_S);	
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,genmode);
	glTexGenfv(GL_T,curplane,curcoefft);
	glEnable(GL_TEXTURE_GEN_T);	

	GLUtesselator *m_pTObj;	
	m_pTObj = gluNewTess();
	gluTessCallback(m_pTObj, GLU_TESS_BEGIN, (_GLUfuncptr) &glBegin);    
	gluTessCallback(m_pTObj, GLU_TESS_END, (_GLUfuncptr) &glEnd);
	gluTessCallback(m_pTObj, GLU_TESS_VERTEX, (_GLUfuncptr) &glVertex3dv);
	gluTessCallback(m_pTObj, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
	gluTessCallback(m_pTObj, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
	gluTessCallback(m_pTObj, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
	gluTessProperty(m_pTObj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);	
	gluTessNormal(m_pTObj,0.0,0.0,1.0);
	gluTessBeginPolygon(m_pTObj, NULL);
	gluTessBeginContour(m_pTObj);	
	for(size_t i = 0 ;i<ptsbuf.size();i++)
		gluTessVertex(m_pTObj,ptsbuf[i],ptsbuf[i]);	

	gluTessEndContour(m_pTObj);
	gluTessEndPolygon(m_pTObj);


	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_S);
	glEndList();

	
	glDeleteLists(m_iboundinglist,1);
	m_iboundinglist = glGenLists(1);
	glNewList(m_iboundinglist,GL_COMPILE);
	glBegin(GL_QUAD_STRIP);
		for(size_t i=0;i<ptsbuf.size();i++)
		{
			glVertex3dv( ptsbuf[i] );
			glVertex3dv( ptsbuf[i] + ARCVector3(0,0,-m_dthickness));
		}
		glVertex3dv( ptsbuf[0] );
		glVertex3dv( ptsbuf[0] + ARCVector3(0,0,-m_dthickness) );
	glEnd();
	glEndList();
}

void IntersectionRenderer::Render(C3DView* pView)
{
	
	if(NeedSync())
	{
		GenDisplaylist(pView);
		DoSync();
	}

	glColor3ubv(Renderer::Color::WHITE);
	if(!m_pTexture)	
		m_pTexture  = &( pView->getTextureRes().RefTextureByType(OglTextureResource::IntersectionTexture) );
	
	//ASSERT()
	m_pTexture->Activate();		
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);		
	glCallList(m_isurdisplist);
		
	m_pTexture->DeActivate();

	if(!m_pZebraTexture) 
		m_pZebraTexture = &(pView->getTextureRes().RefTextureByType(OglTextureResource::ZebraTexture));
	
	m_pZebraTexture->Activate();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	
		glCallList(m_iZebralist);
	m_pZebraTexture->DeActivate();
	glCallList(m_iboundinglist);
	glPushMatrix();
	glTranslated(0,0,-m_dthickness);
	glCallList(m_isurdisplist);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

}
void IntersectionRenderer::RenderSelect(C3DView * pView,int selidx)
{
	if(NeedSync())
	{
		GenDisplaylist(pView);
		DoSync();
	}
	
	glLoadName(NewGenerateSelectionID(SELTYPE_LANDSIDEPROCESSOR,SELSUBTYPE_MAIN,selidx,0));
	glCallList(m_isurdisplist);
}