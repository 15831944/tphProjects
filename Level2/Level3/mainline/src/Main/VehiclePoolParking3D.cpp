#include "StdAfx.h"
#include "../InputAirside/VehiclePoolParking.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include ".\VehiclePoolParking3D.h"
#include "../Common/ARCPipe.h"
#include ".\3DView.h"
#include "glRender/glTextureResource.h"
#include "glRender/glShapeResource.h"
#include "TessellationManager.h"


CVehiclePoolParking3D::CVehiclePoolParking3D(int nID):ALTObject3D(nID)
{
	m_pObj = new VehiclePoolParking();
	//m_pDisplayProp = new VehiclePoolParkingDisplayProp();
}

CVehiclePoolParking3D::~CVehiclePoolParking3D(void)
{
}

void CVehiclePoolParking3D::DrawOGL( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}

	const CPath2008 & paths = GetVehiclePoolParking()->GetPath();
	if(paths.getCount()<1)return;
	std::vector<ARCVector3> vptsbuf;
	for (int j = 0;j < paths.getCount();j++) {
		vptsbuf.push_back(paths.getPoint(j));
	}
	if(  GetDisplayProp()->m_dpShape.bOn ) {
		glNormal3i(0,0,1);
		glColor4ubv(GetDisplayProp()->m_dpShape.cColor);
		glColor3ub(255,255,0);
		CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBase");
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
		for(size_t i = 0 ;i<vptsbuf.size();i++)
		{
			gluTessVertex(m_pTObj,vptsbuf[i],vptsbuf[i]);	
		}

		gluTessEndContour(m_pTObj);
		gluTessEndPolygon(m_pTObj);


		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_S);
		gluDeleteTess(m_pTObj);
	}

	
}



ARCPoint3 CVehiclePoolParking3D::GetLocation( void ) const
{
	VehiclePoolParking * pStretch = (VehiclePoolParking*)m_pObj.get();
	const CPath2008& path = pStretch->GetPath();

	int nMid = int( (path.getCount()-1) * 0.5);

	if(nMid<path.getCount()-1)
		return (path.getPoint(nMid) + path.getPoint(nMid+1) )*0.5;

	return ARCPoint3(0,0,0);

}
//void CVehiclePoolParking3D::RenderBaseWithSideLine( C3DView * pView )
//{
//	if(IsNeedSync()){
//		DoSync();
//	}	
//
//	VehiclePoolParking * pStretch = GetVehiclePoolParking();
//	if(pStretch->GetPath().getCount()<1)return;
//
//	ARCPipe pipepath (pStretch->GetPath(),pStretch->GetLaneNumber() * pStretch->GetLaneWidth()*1.1);
//	int npippathcnt  = (int)pipepath.m_centerPath.size();
//
//	CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBaseLine");
//
//	if(pTexture)
//		pTexture->Apply();
//
//
//	glBegin(GL_QUADS);
//	ARCVector3 prePointL = pipepath.m_sidePath1[0];
//	ARCVector3 prePointR  = pipepath.m_sidePath2[0];
//	glColor3ub(255,255,255);
//	glNormal3i(0,0,1);
//	double dTexCoordLeft = 0;
//	double dTexCoordRight  =0 ;
//
//	for(int i=1;i<npippathcnt;i ++ )
//	{
//		glTexCoord2d(0,dTexCoordLeft);
//		glVertex3dv(prePointL);
//		glTexCoord2d(1,dTexCoordRight);
//		glVertex3dv(prePointR);			
//
//		DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
//		dTexCoordLeft += dLen * 0.002;
//		dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
//		dTexCoordRight += dLen * 0.002;		
//
//		glTexCoord2d(1,dTexCoordRight);
//		glVertex3dv(pipepath.m_sidePath2[i]);
//		glTexCoord2d(0,dTexCoordLeft);
//		glVertex3dv(pipepath.m_sidePath1[i]);
//
//
//		std::swap(dTexCoordLeft, dTexCoordRight);
//		prePointL = pipepath.m_sidePath1[i];
//		prePointR = pipepath.m_sidePath2[i];
//	}
//	glEnd();
//
//}

VehiclePoolParking * CVehiclePoolParking3D::GetVehiclePoolParking()
{
	return (VehiclePoolParking*) GetObject();
}


void CVehiclePoolParking3D::DrawSelect( C3DView * pView )
{
	DrawOGL(pView);
}
//
//void CVehiclePoolParking3D::RenderBase( C3DView * pView )
//{
//	VehiclePoolParking * pVehiclePoolParking = GetVehiclePoolParking();
//
//	if(!pVehiclePoolParking->GetPath().getCount())
//		return ;
//
//	ARCPipe pipepath (pVehiclePoolParking->GetPath(),pVehiclePoolParking->GetLaneNumber() * pVehiclePoolParking->GetLaneWidth());
//	int npippathcnt  = (int)pipepath.m_centerPath.size();
//
//	CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBase");
//	if(pTexture)
//		pTexture->Apply();
//
//
//	glBegin(GL_QUADS);
//	ARCVector3 prePointL = pipepath.m_sidePath1[0];
//	ARCVector3 prePointR  = pipepath.m_sidePath2[0];
//	glColor3ub(255,255,255);
//	glNormal3i(0,0,1);
//	double dTexCoordLeft = 0;
//	double dTexCoordRight  =0 ;
//
//	for(int i=1;i<npippathcnt;i ++ )
//	{
//		glTexCoord2d(0,dTexCoordLeft);
//		glVertex3dv(prePointL);
//		glTexCoord2d(1,dTexCoordRight);
//		glVertex3dv(prePointR);			
//
//		DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
//		dTexCoordLeft += dLen * 0.002;
//		dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
//		dTexCoordRight += dLen * 0.002;		
//
//		glTexCoord2d(1,dTexCoordRight);
//		glVertex3dv(pipepath.m_sidePath2[i]);
//		glTexCoord2d(0,dTexCoordLeft);
//		glVertex3dv(pipepath.m_sidePath1[i]);
//
//
//		std::swap(dTexCoordLeft, dTexCoordRight);
//		prePointL = pipepath.m_sidePath1[i];
//		prePointR = pipepath.m_sidePath2[i];
//	}
//	glEnd();
//}
//
//void CVehiclePoolParking3D::RenderMarkings( C3DView * pView )
//{
//	VehiclePoolParking * pStretch = GetStretch();
//
//	if(!pStretch->GetPath().getCount())
//		return;
//
//	ARCPipe pipepath (pStretch->GetPath(),pStretch->GetLaneNumber() * pStretch->GetLaneWidth());
//	int npippathcnt  = (int)pipepath.m_centerPath.size();
//
//	CTexture * pTexture = pView->getTextureResource()->getTexture("RwCenter");
//	if(pTexture)
//		pTexture->Apply();
//
//	DistanceUnit offset = 0.1;
//	int lanNum = pStretch->GetLaneNumber();
//
//	glBegin(GL_QUADS);
//	ARCVector3 prePointL = pipepath.m_sidePath1[0];
//	ARCVector3 prePointR  = pipepath.m_sidePath2[0];
//	glColor3ub(255,255,255);
//	glNormal3i(0,0,1);
//	double dTexCoordLeft = 0;
//	double dTexCoordRight  =0 ;
//
//	for(int i=1;i<npippathcnt;i ++ )
//	{
//		glTexCoord2d(0.5 + offset,dTexCoordLeft);
//		glVertex3dv(prePointL);
//		glTexCoord2d(0.5 - offset + lanNum,dTexCoordRight);
//		glVertex3dv(prePointR);			
//
//		DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
//		dTexCoordLeft += dLen * 0.002;
//		dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
//		dTexCoordRight += dLen * 0.002;		
//
//		glTexCoord2d(0.5 - offset + lanNum ,dTexCoordRight);
//		glVertex3dv(pipepath.m_sidePath2[i]);
//		glTexCoord2d(0.5 + offset,dTexCoordLeft);
//		glVertex3dv(pipepath.m_sidePath1[i]);
//
//
//		std::swap(dTexCoordLeft, dTexCoordRight);
//		prePointL = pipepath.m_sidePath1[i];
//		prePointR = pipepath.m_sidePath2[i];
//	}
//	glEnd();
//
//	RenderEditPoint(pView);
//}

void CVehiclePoolParking3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetVehiclePoolParking()->m_path.DoOffset(dx,dy,dz);
}

void CVehiclePoolParking3D::RenderEditPoint( C3DView * pView )
{
	glDisable(GL_TEXTURE_2D);
	if(!IsInEdit()) return;
	if(IsNeedSync()){
		DoSync();
	}	
	for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++){
		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}
}

void CVehiclePoolParking3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetVehiclePoolParking()->GetPath().getCount();

	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this, GetVehiclePoolParking()->m_path, i) );
	}

	ALTObject3D::DoSync();	
}


void CVehiclePoolParking3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	CPath2008& path = const_cast<CPath2008&>(GetVehiclePoolParking()->GetPath());
	path.DoOffset(-center[VX],-center[VY],-center[VZ]);
	path.Rotate(dx);
	path.DoOffset(center[VX],center[VY],center[VZ]);
}