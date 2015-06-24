#include "StdAfx.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include ".\reportingarea3d.h"
#include ".\3DView.h"
#include "Airside3D.h"
#include "Airport3D.h"
#include "TessellationManager.h"

CReportingArea3D::CReportingArea3D(int nID):ALTObject3D(nID)
{
	m_pObj = new CReportingArea();
	//m_pDisplayProp = new ReportingAreaDisplayProp();
}

CReportingArea3D::~CReportingArea3D(void)
{
}

void CReportingArea3D::DrawOGL( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}

	if(!GetDisplayProp()->m_dpShape.bOn) return;

	if(IsSelected()) glLineWidth(3.0f);

	ARCColor4 cColor = GetDisplayProp()->m_dpShape.cColor;
	cColor[ALPHA]= 255/4;
	glColor4ubv(cColor);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	Render(pView);	
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.0f,-1.0f);	
	RenderLine(pView);		
	glDisable(GL_POLYGON_OFFSET_LINE);

	glEnable(GL_LIGHTING);

	if(IsSelected()) glLineWidth(1.0f);

	//const CPath2008 & paths = GetReportingArea()->GetPath();
	//if(paths.getCount()<1)return;
	//std::vector<ARCVector3> vptsbuf;
	//for (int j = 0;j < paths.getCount();j++) {
	//	vptsbuf.push_back(paths.getPoint(j));
	//}
	//if( m_pDisplayProp->m_dpShape.bOn ) {
	//	glNormal3i(0,0,1);
	//	glColor4ubv(m_pDisplayProp->m_dpShape.cColor);
	//	glColor3ub(255,255,0);
	//	CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBase");
	//	if(pTexture)
	//		pTexture->Apply();

	//
	//}


}



ARCPoint3 CReportingArea3D::GetLocation( void ) const
{
	CReportingArea * pArea = (CReportingArea*)m_pObj.get();
	const CPath2008& path = pArea->GetPath();

	int nMid = int( (path.getCount()-1) * 0.5);

	if(nMid<path.getCount()-1)
		return (path.getPoint(nMid) + path.getPoint(nMid+1) )*0.5;

	return ARCPoint3(0,0,0);

}


CReportingArea * CReportingArea3D::GetReportingArea()
{
	return (CReportingArea*) GetObject();
}


void CReportingArea3D::DrawSelect( C3DView * pView )
{
	DrawOGL(pView);
}

void CReportingArea3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetReportingArea()->m_path.DoOffset(dx,dy,dz);
}

void CReportingArea3D::RenderLine( C3DView * pView )
{
	CReportingArea * pObj = (CReportingArea*)m_pObj.get();
	CPath2008 path = pObj->GetPath();
	glBegin(GL_LINE_LOOP);
	for(int i=0;i<path.getCount();++i)
	{
		glVertex3d(path[i].getX(),path[i].getY(),0);		
	}
	glEnd();
}

void CReportingArea3D::Render( C3DView * pView )
{

	CReportingArea * pObj = (CReportingArea*)m_pObj.get();

	const CPath2008& path = pObj->GetPath();
	std::vector<ARCVector3> vptsbuf;
	for (int j = 0;j < path.getCount();j++) {
		vptsbuf.push_back(path.getPoint(j));
	}
	CAirside3D* pAirside = pView->GetAirside3D();
	//double dAirportAlt = pAirside->GetActiveAirport()->m_altAirport.getElevation();

	if(path.getCount()<1)return;
	double dAlt = 0.0;

		GLenum genmode = GL_OBJECT_LINEAR;
		GLenum curplane = GL_OBJECT_PLANE;	

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
		for(size_t i = 0 ;i<vptsbuf.size();i++)
		{
			gluTessVertex(m_pTObj,vptsbuf[i],vptsbuf[i]);	
		}

		gluTessEndContour(m_pTObj);
		gluTessEndPolygon(m_pTObj);	
		gluDeleteTess(m_pTObj);	

}

void CReportingArea3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetReportingArea()->GetPath().getCount();

	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this, GetReportingArea()->m_path, i) );
	}

	ALTObject3D::DoSync();	
}


void CReportingArea3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	CPath2008& path = const_cast<CPath2008&>(GetReportingArea()->GetPath());
	path.DoOffset(-center[VX],-center[VY],-center[VZ]);
	path.Rotate(dx);
	path.DoOffset(center[VX],center[VY],center[VZ]);
}