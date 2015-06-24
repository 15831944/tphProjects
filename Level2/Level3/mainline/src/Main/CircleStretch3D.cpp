#include "StdAfx.h"
#include ".\circlestretch3d.h"
#include "../InputAirside/CircleStretch.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include "../Common/ARCPipe.h"
#include ".\3DView.h"
#include "../Main/TermPlanDoc.h"
#include <CommonData/3DTextManager.h>
#include "glrender/glTextureResource.h"
CCircleStretch3D::CCircleStretch3D(int nID):ALTObject3D(nID)
{
	m_pObj = new CircleStretch();
	//m_pDisplayProp = new StretchDisplayProp();
}

CCircleStretch3D::~CCircleStretch3D(void)
{
}



CircleStretch * CCircleStretch3D::GetCircleStretch()
{
	return (CircleStretch*) GetObject();
}


void CCircleStretch3D::RenderBaseWithSideLine( C3DView * pView )
{
	if (!IsInEdit())
	{
		glDisable(GL_BLEND);
	}	
	if(IsNeedSync()){
		DoSync();
	}	
	BOOL bFlags = FALSE;
	CircleStretch * pCirStretch = GetCircleStretch();
	if(pCirStretch->GetPath().getCount()<1)return;

	CPath2008 m_Repath,m_temp;
    m_temp = pCirStretch->getPath();
	m_temp.PathSupplement(m_Repath);
	CPath2008 path = airsideThick.ModifyPathAlt(m_Repath,pView);
	CPath2008 belowPath;
	airsideThick.AddThickness(path,belowPath);
	m_path = belowPath;
	ARCPipe pipepath (path,pCirStretch->GetLaneNumber() * pCirStretch->GetLaneWidth()*1.1);
	ARCPipe pipepath1 (belowPath,pCirStretch->GetLaneNumber() * pCirStretch->GetLaneWidth()*1.1);

	int npippathcnt  = (int)pipepath.m_centerPath.size();
	if (npippathcnt<2)
		return;

	CTexture * pTexture = pView->getTextureResource()->getTexture("roadsideline");
	if(pTexture)
		pTexture->Apply();
	double totaLength=0.0;
	for (int i=0; i<npippathcnt-1;i++)
	{
		totaLength += pipepath.m_sidePath1[i].DistanceTo(pipepath.m_sidePath1[i+1]);
	}
    totaLength += pipepath.m_sidePath1[npippathcnt-1].DistanceTo(pipepath.m_sidePath1[0]);
	glBegin(GL_QUADS);
	ARCVector3 prePointL = pipepath.m_sidePath1[0];
	ARCVector3 prePointR  = pipepath.m_sidePath2[0];

	ARCVector3 prePointL1 = pipepath1.m_sidePath1[0];
	ARCVector3 prePointR1 = pipepath1.m_sidePath2[0];

	glColor4d(1.0,1.0,1.0,0.4);


	double dTexCoordLeft = 0;
	double dTexCoordRight  =0 ;
	ARCVector3 vNormalize(0.0,0.0,0.0);
	//above path
	{
		for(int i=1;i<npippathcnt;i ++ )
		{
			if (prePointL[VZ]==0.0&&pipepath.m_sidePath1[i][VZ]==0.0)
			{	
				glNormal3d(0.0,0.0,1.0);
			}
			else
			{
				airsideThick.CalculateNormalize(pipepath.m_sidePath1,pipepath.m_sidePath2,vNormalize,i,FALSE);
				glNormal3d(vNormalize[VX],vNormalize[VY],vNormalize[VZ]);
			}
			glTexCoord2d(0,dTexCoordLeft);
			glVertex3dv(prePointL);
			glTexCoord2d(1,dTexCoordRight);
			glVertex3dv(prePointR);			

			DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
			dTexCoordLeft += dLen/totaLength;// * 0.002;
			dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
			dTexCoordRight += dLen/totaLength;// * 0.002;		

			glTexCoord2d(1,dTexCoordRight);
			glVertex3dv(pipepath.m_sidePath2[i]);
			glTexCoord2d(0,dTexCoordLeft);
			glVertex3dv(pipepath.m_sidePath1[i]);


			std::swap(dTexCoordLeft, dTexCoordRight);
			prePointL = pipepath.m_sidePath1[i];
			prePointR = pipepath.m_sidePath2[i];
		}
		
	}
	glEnd();

	//below path
	glBegin(GL_QUADS);
	{
		dTexCoordLeft = 0;
		dTexCoordRight = 0;
		ARCVector3 vNormalize(0.0,0.0,0.0);
		glColor4d(1.0,1.0,1.0,0.6);
		for (int i=1; i<npippathcnt; i++)
		{
			{
				airsideThick.CalculateNormalize(pipepath1.m_sidePath1,pipepath1.m_sidePath2,vNormalize,i,TRUE);
				glNormal3d(vNormalize[VX],vNormalize[VY],vNormalize[VZ]);
				glTexCoord2d(0,dTexCoordLeft);
				glVertex3dv(prePointL1);
				glTexCoord2d(1,dTexCoordRight);
				glVertex3dv(prePointR1);			

				DistanceUnit dLen = prePointL1.DistanceTo(pipepath1.m_sidePath1[i]);	
				dTexCoordLeft += dLen/totaLength;// * 0.002;
				dLen = prePointR1.DistanceTo(pipepath1.m_sidePath2[i]);
				dTexCoordRight += dLen/totaLength;// * 0.002;		

				glTexCoord2d(1,dTexCoordRight);
				glVertex3dv(pipepath1.m_sidePath2[i]);
				glTexCoord2d(0,dTexCoordLeft);
				glVertex3dv(pipepath1.m_sidePath1[i]);


				std::swap(dTexCoordLeft, dTexCoordRight);
				prePointL1 = pipepath1.m_sidePath1[i];
				prePointR1 = pipepath1.m_sidePath2[i];
			}

		}

	}
	glEnd();

	airsideThick.RenderSidePath(pView,pipepath,pipepath1);
	airsideThick.RenderWall(pView,path,pCirStretch->GetLaneNumber() * pCirStretch->GetLaneWidth()*1.1);
	airsideThick.RenderBridge(pView,belowPath,pCirStretch->GetLaneNumber() * pCirStretch->GetLaneWidth()*1.1);
	if (!IsInEdit())
	{
		glEnable(GL_BLEND);
	}	
}

void CCircleStretch3D::DrawSelect( C3DView * pView )
{
	//DrawOGL(pView);
	if(IsNeedSync()){
		DoSync();
	}

	RenderBaseWithSideLine(pView);

	if(IsInEdit())
	{
		RenderEditPoint(pView);
	}
}


void CCircleStretch3D::RenderBase( C3DView * pView )
{
	if (!IsInEdit())
	{
		glDisable(GL_BLEND);
	}	

	glEnable(GL_LIGHTING);
	if(IsNeedSync()){
		DoSync();
	}	
	BOOL bFlags = FALSE;
	CircleStretch * pCirStretch = GetCircleStretch();
	if(pCirStretch->GetPath().getCount()<1)return;

	CPath2008 m_Repath,m_temp;
	m_temp = pCirStretch->getPath();
	m_temp.PathSupplement(m_Repath);
	CPath2008 path = airsideThick.ModifyPathAlt(m_Repath,pView);
	CPath2008 belowPath;
	airsideThick.AddThickness(path,belowPath);
	m_path = belowPath;
	ARCPipe pipepath (path,pCirStretch->GetLaneNumber() * pCirStretch->GetLaneWidth()*1.1);
	ARCPipe pipepath1 (belowPath,pCirStretch->GetLaneNumber() * pCirStretch->GetLaneWidth()*1.1);

	int npippathcnt  = (int)pipepath.m_centerPath.size();
	if (npippathcnt<2)
		return;

	CTexture * pTexture = pView->getTextureResource()->getTexture("roadsideline");
	if(pTexture)
		pTexture->Apply();
	double totaLength=0.0;
	for (int i=0; i<npippathcnt-1;i++)
	{
		totaLength += pipepath.m_sidePath1[i].DistanceTo(pipepath.m_sidePath1[i+1]);
	}
	totaLength += pipepath.m_sidePath1[npippathcnt-1].DistanceTo(pipepath.m_sidePath1[0]);
	glBegin(GL_QUADS);
	ARCVector3 prePointL = pipepath.m_sidePath1[0];
	ARCVector3 prePointR  = pipepath.m_sidePath2[0];

	ARCVector3 prePointL1 = pipepath1.m_sidePath1[0];
	ARCVector3 prePointR1 = pipepath1.m_sidePath2[0];

	if (!IsInEdit())
	{
		glColor3d(1.0,1.0,1.0);
	}	
	else
	{
		glColor4d(1.0,1.0,1.0,0.4);
	}

	double dTexCoordLeft = 0;
	double dTexCoordRight  =0 ;
	ARCVector3 vNormalize(0.0,0.0,0.0);
	//above path
	{
		for(int i=1;i<npippathcnt;i ++ )
		{
			if (prePointL[VZ]==0.0&&pipepath.m_sidePath1[i][VZ]==0.0)
			{	
				glNormal3d(0.0,0.0,1.0);
			}
			else
			{
				airsideThick.CalculateNormalize(pipepath.m_sidePath1,pipepath.m_sidePath2,vNormalize,i,FALSE);
				glNormal3d(vNormalize[VX],vNormalize[VY],vNormalize[VZ]);
			}
			glTexCoord2d(0,dTexCoordLeft);
			glVertex3dv(prePointL);
			glTexCoord2d(1,dTexCoordRight);
			glVertex3dv(prePointR);			

			DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
			dTexCoordLeft += dLen/totaLength;// * 0.002;
			dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
			dTexCoordRight += dLen/totaLength;// * 0.002;		

			glTexCoord2d(1,dTexCoordRight);
			glVertex3dv(pipepath.m_sidePath2[i]);
			glTexCoord2d(0,dTexCoordLeft);
			glVertex3dv(pipepath.m_sidePath1[i]);


			std::swap(dTexCoordLeft, dTexCoordRight);
			prePointL = pipepath.m_sidePath1[i];
			prePointR = pipepath.m_sidePath2[i];
		}

	}
	glEnd();
    glDisable(GL_LIGHTING);
	if (!IsInEdit())
	{
		glEnable(GL_BLEND);
	}	
}



void CCircleStretch3D::RenderMarkings( C3DView * pView )
{
	glEnable(GL_LIGHTING);
	CircleStretch * pCirStretch = GetCircleStretch();

	if(!pCirStretch->GetPath().getCount())
		return;

	CPath2008 m_Repath,m_temp;
	m_temp = pCirStretch->getPath();
	m_temp.PathSupplement(m_Repath);
	if (m_Repath.getCount()<3)
	{
		return;
	}

	CPath2008 path = airsideThick.ModifyPathAlt(m_Repath,pView);
	CPath2008 belowPath;
	airsideThick.AddThickness(path,belowPath);

	ARCPipe pipepath (path,pCirStretch->GetLaneNumber() * pCirStretch->GetLaneWidth());
	ARCPipe pipepath1 (belowPath,pCirStretch->GetLaneNumber() * pCirStretch->GetLaneWidth());

	int npippathcnt  = (int)pipepath.m_centerPath.size();

	CTexture * pTexture = pView->getTextureResource()->getTexture("RwCenter");
	if(pTexture)
		pTexture->Apply();

	DistanceUnit offset = 0.1;
	int lanNum = pCirStretch->GetLaneNumber();

	glBegin(GL_QUADS);
	ARCVector3 prePointL = pipepath.m_sidePath1[0];
	ARCVector3 prePointR  = pipepath.m_sidePath2[0];

	ARCVector3 prePointL1 = pipepath1.m_sidePath1[0];
	ARCVector3 prePointR1 = pipepath1.m_sidePath2[0];

	glColor3ub(255,255,255);
	double dTexCoordLeft = 0;
	double dTexCoordRight  =0 ;
	ARCVector3 vNormalize(0.0,0.0,0.0);
	for(int i=1;i<npippathcnt;i ++ )
	{
		if (prePointL[VZ]==0.0&&pipepath.m_sidePath1[i][VZ]==0.0)
		{	
			glNormal3d(0.0,0.0,1.0);
		}
		else
		{
			airsideThick.CalculateNormalize(pipepath.m_sidePath1,pipepath.m_sidePath2,vNormalize,i,FALSE);
			glNormal3d(vNormalize[VX],vNormalize[VY],vNormalize[VZ]);
		}
		glTexCoord2d(0.5 + offset,dTexCoordLeft);
		glVertex3dv(prePointL);
		glTexCoord2d(0.5 - offset + lanNum,dTexCoordRight);
		glVertex3dv(prePointR);			

		DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
		dTexCoordLeft += dLen * 0.002;
		dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
		dTexCoordRight += dLen * 0.002;		

		glTexCoord2d(0.5 - offset + lanNum ,dTexCoordRight);
		glVertex3dv(pipepath.m_sidePath2[i]);
		glTexCoord2d(0.5 + offset,dTexCoordLeft);
		glVertex3dv(pipepath.m_sidePath1[i]);


		std::swap(dTexCoordLeft, dTexCoordRight);
		prePointL = pipepath.m_sidePath1[i];
		prePointR = pipepath.m_sidePath2[i];
	}

	dTexCoordLeft = 0;
	dTexCoordRight = 0;

	glEnd();
	glDisable(GL_LIGHTING);

	RenderEditPoint(pView);
	RenderEditPointText();
}

void CCircleStretch3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetCircleStretch()->m_path.DoOffset(dx,dy,dz);
}

void CCircleStretch3D::RenderEditPoint( C3DView * pView )
{
	glDisable(GL_TEXTURE_2D);
	glColor3ub(255,255,255);
	if(!IsInEdit()) return;
	//	if(IsNeedSync()){
	DoSync();
	//	}	
	for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++){
		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}
}

void CCircleStretch3D::RenderEditPointText()
{
	glDisable(GL_TEXTURE_2D);
	glColor3ub(0,0,0);
	if (!IsInEdit()) 
		return;
	CString strNum;
	CString objectName = GetCircleStretch()->GetObjNameString();
	Path path = GetCircleStretch()->GetPath();
	for (int i=0; i<path.getCount(); i++)
	{
		strNum.Format("%d",i+1);
		CString pointName = objectName + strNum;
		Point editPoint = path.getPoint(i);
		ARCVector3 vWorldPos(editPoint.getX(),editPoint.getY(),editPoint.getZ());
		TEXTMANAGER3D->DrawBitmapText(pointName,vWorldPos);
	}
	glEnable(GL_TEXTURE_2D);
}

void CCircleStretch3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetCircleStretch()->GetPath().getCount();

	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,GetCircleStretch()->m_path,i) );
	}

	ALTObject3D::DoSync();	
}


void CCircleStretch3D::AddEditPoint(double x, double y, double z)
{
	int recordPos = -1;
	CPath2008 &stretchPath = GetCircleStretch()->getPath();
	int nCount = stretchPath.getCount();
	CPoint2008 newPoint;
	double minDis = 10000.0;
	for (int i=0; i<nCount-1; i++)
	{
		CPoint2008 pPrev = stretchPath[i];
		CPoint2008 pNext = stretchPath[i+1];
		DistanceUnit dist = abs((pNext.getY()-pPrev.getY())*(x-pPrev.getX())-(y-pPrev.getY())*(pNext.getX()-pPrev.getX()))/pPrev.getDistanceTo(pNext);
		if(dist<minDis)
		{
			CLine2008 proLine(pPrev, pNext);
			CPoint2008 proPoint = proLine.getProjectPoint(CPoint2008(x,y,z));
			if(proPoint.within(pPrev, pNext))
			{
				newPoint = proPoint;
				minDis = dist;
				recordPos = i;
			}
		}
	}

	if(recordPos<0)
	{
		stretchPath.insertPointAfterAt(newPoint, 0);
	}
	else
	{
		stretchPath.insertPointAfterAt(newPoint, recordPos);
	}

	UpdateSync();

}

void CCircleStretch3D::FlushChange()
{
	try
	{
		GetCircleStretch()->UpdatePath();
		UpdateSync();
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}
}



ARCPoint3 CCircleStretch3D::GetLocation( void ) const
{
	CircleStretch * pCirStretch = (CircleStretch*)m_pObj.get();
	const CPath2008& path = pCirStretch->GetPath();

	int nMid = int( (path.getCount()-1) * 0.5);

	if(nMid<path.getCount()-1)
		return (path.getPoint(nMid) + path.getPoint(nMid+1) )*0.5;

	return ARCPoint3(0,0,0);

}