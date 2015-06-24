#include "StdAfx.h"
#include ".\roadintersection3d.h"
#include ".\3dview.h"
#include "./ChildFrm.h"
#include "Airside3D.h"
#include "../InputAirside/Intersections.h"
#include "../InputAirside/Stretch.h"
#include "./TessellationManager.h"
#include <Common/ShapeGenerator.h>
#include "../InputAirside/ALTObjectDisplayProp.h"
#include "Airport3D.h"
#include "../Common/Pollygon2008.h"

#include ".\glrender\glTextureResource.h"

CRoadIntersection3D::CRoadIntersection3D(int nID): ALTObject3D(nID)
{
	m_pObj = new Intersections();
	//m_pDisplayProp = new RoadIntersectionDisplayProp();

}

CRoadIntersection3D::~CRoadIntersection3D(void)
{
	
}

void CRoadIntersection3D::DrawOGL( C3DView * pView )
{
	RenderSurface(pView);
}

void CRoadIntersection3D::DrawSelect( C3DView * pView )
{
	RenderSurface(pView);
}

ARCPoint3 CRoadIntersection3D::GetLocation( void ) const
{
	return m_vCenter;	
}

struct StretchPortInfo
{
	ARCVector3 vOrien;
	ARCVector3 leftPoint;
	ARCVector3 rightPoint;
};

void CRoadIntersection3D::RenderSurface( C3DView * pView )
{
	double widthofzebra = 20 * SCALE_FACTOR;

	//update relations
	InsecObjectPartVector vRelatStretch = GetRoadIntersection()->GetIntersectionsPart();
	CAirport3D * pAirport = pView->GetParentFrame()->GetAirside3D()->GetAirport(GetRoadIntersection()->getAptID());
	if(pAirport)
	{
		for(int i=0;i<(int)vRelatStretch.size();i++)
		{
			int strechId = vRelatStretch[i].GetObjectID();
			ALTObject3D * pObj3D = pAirport->GetObject3D(strechId);
			if(pObj3D){
				vRelatStretch[i].SetObject(pObj3D->GetObject());
			}
		}
	}
	//render
	//at least 2 link stretches
	if(vRelatStretch.size()<2)
		return;
	//calculate the control points
	/*std::vector<ARCVector3> ctrlpoints;
	std::vector<ARCVector3> orientats;
	std::vector<ARCVector3> ctrlpointsOut;*/
	//check to see the linked procs are in a counterclockwise order
	int ncount = (int)vRelatStretch.size();
	CPollygon2008 poly;
	std::vector<CPoint2008> vPoints;

	for(int i = 0;i< ncount;i++)
	{
		InsecObjectPart& thepart = vRelatStretch[i];

		if(thepart.GetObject() && thepart.GetObject()->GetType() == ALT_STRETCH)
		{
			Stretch* pStretch  = (Stretch*) thepart.GetObject();
			const CPath2008& stretchpath = pStretch->GetPath();

			if( thepart.pos * 2 > stretchpath.getCount()-1 )
			{
				int pcount = (int) stretchpath.getCount();
				vPoints.push_back( stretchpath.getPoint(stretchpath.getCount()-1) );
			}
			else 
			{
				vPoints.push_back( stretchpath.getPoint(0) );
			}
		}
	}
	poly.init(vPoints.size(),&vPoints[0]);

	//update center point 
	m_vCenter = ARCPoint3(0,0,0);
	for(int i=0;i<(int)vPoints.size();i++)
	{	
		m_vCenter += vPoints.at(i);
	}
	m_vCenter *=(1.0/vPoints.size());


	bool bcountclk= poly.IsCountClock();
	////make the linkprocs in CountClockOrder;
	if(!bcountclk)
	{	
		std::reverse(vRelatStretch.begin(),vRelatStretch.end());
	}
	////get the control points of the Intersection
	std::vector<StretchPortInfo> vRelateStretchInfo;
	for(int i=0;i< (int)vRelatStretch.size();i++)
	{
		InsecObjectPart& thepart = vRelatStretch[i];
		if(thepart.GetObject() && thepart.GetObject()->GetType() == ALT_STRETCH)
		{
			Stretch* pStretch  = (Stretch*) thepart.GetObject();
			const CPath2008& stretchpath = pStretch->GetPath();
			DistanceUnit width = pStretch->GetLaneWidth() * pStretch->GetLaneNumber() * 1.1;
			
			StretchPortInfo newInfo;
			if( thepart.pos * 2 > stretchpath.getCount()-1 )
			{
				newInfo.vOrien = stretchpath.GetIndexDir(stretchpath.getCount()-1.0f);
				newInfo.vOrien.Normalize();
				ARCVector3 vDir(-newInfo.vOrien[VY],newInfo.vOrien[VX],0);
				ARCVector3 centerPt = stretchpath.getPoint(stretchpath.getCount()-1);
				newInfo.leftPoint = centerPt + vDir * width * 0.5;
				newInfo.rightPoint = centerPt - vDir * width * 0.5;
			}
			else 
			{
				newInfo.vOrien = stretchpath.GetIndexDir(0.0f) * (-1);
				newInfo.vOrien.Normalize();
				ARCVector3 vDir(-newInfo.vOrien[VY],newInfo.vOrien[VX],0);
				ARCVector3 centerPt = stretchpath.getPoint(0);
				newInfo.leftPoint = centerPt + vDir * width * 0.5;
				newInfo.rightPoint = centerPt - vDir * width * 0.5;
			}
			vRelateStretchInfo.push_back(newInfo);
		}

	}

	//get control points
	const static int smoothPtcnt = 10;	

	std::vector<ARCVector3> ptsbuf;
	ptsbuf.reserve(256);
	for( int i =0; i< (int)vRelateStretchInfo.size() ;i++)
	{
		StretchPortInfo thisInfo = vRelateStretchInfo[i];
		StretchPortInfo nextInfo;
		if(i == vRelateStretchInfo.size()-1)
		{
			nextInfo = vRelateStretchInfo[0];
		}else
		{
			nextInfo = vRelateStretchInfo[i+1];
		}

		ARCVector3 p1 = thisInfo.rightPoint;
		ARCVector3 p2 = nextInfo.leftPoint;
		ARCVector3 v1 = thisInfo.vOrien;
		ARCVector3 v2 = nextInfo.vOrien;
		v1.Normalize();
		v2.Normalize();

		DistanceUnit dist = p1.DistanceTo(p2);        

		std::vector<ARCVector3> _inpoint;
		std::vector<ARCVector3> _outpoint;
		_inpoint.reserve(4);_outpoint.reserve(smoothPtcnt);
		_inpoint.push_back( p1 ); _inpoint.push_back( p1 + thisInfo.vOrien * dist * 0.5 );
		_inpoint.push_back(p2 + nextInfo.vOrien * dist * 0.5 );
		_inpoint.push_back( p2 );

		SHAPEGEN.GenBezierPath(_inpoint,_outpoint,smoothPtcnt);
		ptsbuf.insert(ptsbuf.end(),_outpoint.begin(),_outpoint.end());	
	}


	CTexture * pTexture = pView->getTextureResource()->getTexture("road");
	if(pTexture)
		pTexture->Apply();

	GLenum genmode = GL_OBJECT_LINEAR;
	GLenum curplane = GL_OBJECT_PLANE;
	static float curcoeffs[] = {0.0001f, 0.0f, 0.0f, 0.0f};
	static float curcoefft[] = {0.00f, 0.0001f, 0.0f, 0.0f};	
	//surface display list
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

	gluDeleteTess(m_pTObj);

	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_S);


}