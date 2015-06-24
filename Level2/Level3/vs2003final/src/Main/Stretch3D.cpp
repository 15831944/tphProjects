#include "StdAfx.h"
#include "../InputAirside/Stretch.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include ".\stretch3d.h"
#include "../Common/ARCPipe.h"
#include ".\3DView.h"
#include "../InputAirside/IntersectedStretch.h"
#include "../Common/GetIntersection.h"
#include "../Main/TermPlanDoc.h"
#include <CommonData/3DTextManager.h>
#include ".\glrender\glTextureResource.h"


struct FilletDistInStretch
{
	FilletDistInStretch(int nStretchID){ m_nStretchID = nStretchID; }

	int m_nStretchID;

	bool operator()(CIntersectedStretch3DPoint* pPt1, CIntersectedStretch3DPoint* pPt2)const
	{
		ASSERT(pPt1 && pPt2);
		if(pPt1->GetIntersectionNode().GetDistance(m_nStretchID) < pPt2->GetIntersectionNode().GetDistance(m_nStretchID) )
		{
			return true;
		}
		if( pPt1->GetIntersectionNode().GetDistance(m_nStretchID) > pPt2->GetIntersectionNode().GetDistance(m_nStretchID) )
		{
			return false;
		}
		return pPt1->GetDistInObj() < pPt2->GetDistInObj();
	}
};

CStretch3D::CStretch3D(int nID):ALTObject3D(nID)
{
	m_pObj = new Stretch();
	//m_pDisplayProp = new StretchDisplayProp();
 //   m_dThick = 300.0;
}

CStretch3D::~CStretch3D(void)
{
}

void CStretch3D::DrawOGL( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}

	if(GetStretch()->GetPath().getCount()<1)return;

	glEnable(GL_BLEND);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_LIGHTING);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	RenderBaseWithSideLine(pView);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-4.0,-4.0);
	glDepthMask(GL_FALSE);	
	//
	RenderBaseWithSideLine(pView);	//get data	
	//RenderBase(pView);
	//
	RenderMarkings(pView);	
	glDepthMask(GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);

//	glDisable(GL_TEXTURE_2D);
}



ARCPoint3 CStretch3D::GetLocation( void ) const
{
	Stretch * pStretch = (Stretch*)m_pObj.get();
	const CPath2008& path = pStretch->GetPath();

	int nMid = int( (path.getCount()-1) * 0.5);

	if(nMid<path.getCount()-1)
		return (path.getPoint(nMid) + path.getPoint(nMid+1) )*0.5;

	return ARCPoint3(0,0,0);

}
void CStretch3D::GetCullPath(CPath2008& path, Stretch* pStretch)
{
	if( IsOnMove() || IsInEdit())
	{
		path = pStretch->GetPath();	
	}
	else
	{
		DistanceUnit fromDist = 0 ;
		DistanceUnit toDist = pStretch->GetPath().GetTotalLength(); 

		std::sort(m_vStretchPoints.begin(),m_vStretchPoints.end(),FilletDistInStretch(pStretch->getID()));
		if(m_vStretchPoints.size())
		{
			//begin
			CIntersectedStretch3DPoint * pStretchBegin = *m_vStretchPoints.begin();
			if(pStretchBegin->GetIntersectionNode().GetStretchIntersectItem().size()<=2)
			{
				if( pStretchBegin->GetIntersectionNode().GetDistance(pStretch->getID()) <  pStretchBegin->GetDistInObj() )
				{
					fromDist = pStretchBegin->GetDistInObj();
				}
			}

			//end
			CIntersectedStretch3DPoint * pStretchEnd = *m_vStretchPoints.rbegin();
			if(pStretchEnd->GetIntersectionNode().GetStretchIntersectItem().size()<=2)
			{
				if(pStretchEnd->GetIntersectionNode().GetDistance(pStretch->getID()) > pStretchEnd->GetDistInObj() )
				{
					toDist = pStretchEnd->GetDistInObj();
				}
			}

		}
		path = GetSubPath(pStretch->GetPath(), fromDist, toDist);
	}
}

void CStretch3D::RenderBaseWithSideLine( C3DView * pView )
{
	if (!IsInEdit())
	{
		glDisable(GL_BLEND)			;
	}
	glEnable(GL_LIGHTING);
	if(IsNeedSync()){
		DoSync();
	}	
    BOOL bFlags = FALSE;
	Stretch * pStretch = GetStretch();
	if(pStretch->GetPath().getCount()<1)return;

	CPath2008 cullPath;
	cullPath.clear();

	GetCullPath(cullPath,pStretch);
	if (cullPath.getCount()<2)
	{
		cullPath = pStretch->GetPath();
	}
	
    CPath2008 path = airsideThick.ModifyPathAlt(cullPath,pView);
	CPath2008 belowPath;
	airsideThick.AddThickness(path,belowPath);
    m_path = belowPath;
	ARCPipe pipepath (path,pStretch->GetLaneNumber() * pStretch->GetLaneWidth()*1.1);
	ARCPipe pipepath1 (belowPath,pStretch->GetLaneNumber() * pStretch->GetLaneWidth()*1.1);

	int npippathcnt  = (int)pipepath.m_centerPath.size();

	CTexture * pTexture = pView->getTextureResource()->getTexture("roadsideline");
	if(pTexture)
		pTexture->Apply();
	double totaLength=0.0;
    for (int i=0; i<npippathcnt-1;i++)
    {
		totaLength += pipepath.m_sidePath1[i].DistanceTo(pipepath.m_sidePath1[i+1]);
    }

	glColor4d(1.0,1.0,1.0,0.4);
	glBegin(GL_QUADS);
	ARCVector3 prePointL = pipepath.m_sidePath1[0];
	ARCVector3 prePointR  = pipepath.m_sidePath2[0];

	ARCVector3 prePointL1 = pipepath1.m_sidePath1[0];
	ARCVector3 prePointR1 = pipepath1.m_sidePath2[0];

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
//	if (pView->GetDocument()->m_bIsOpenStencil)
//	{
		airsideThick.RenderWall(pView,path,pStretch->GetLaneNumber() * pStretch->GetLaneWidth()*1.1);
//	}
    airsideThick.RenderBridge(pView,belowPath,pStretch->GetLaneNumber() * pStretch->GetLaneWidth()*1.1);
	glDisable(GL_LIGHTING);
	if (!IsInEdit())
	{
		glEnable(GL_BLEND)			;
	}
}


Stretch * CStretch3D::GetStretch()
{
	return (Stretch*) GetObject();
}


void CStretch3D::DrawSelect( C3DView * pView )
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

void CStretch3D::RenderBase( C3DView * pView )
{
	if (!IsInEdit())
	{
		glDisable(GL_BLEND)			;
	}
	glEnable(GL_LIGHTING);
	Stretch * pStretch = GetStretch();

	if(!pStretch->GetPath().getCount())
		return ;

	CPath2008 cullPath;
	GetCullPath(cullPath,pStretch);
	if (cullPath.getCount()<2)
	{
		cullPath = pStretch->GetPath();
	}

    CPath2008 path =	airsideThick.ModifyPathAlt(cullPath,pView);
	CPath2008 belowPath;
	airsideThick.AddThickness(path,belowPath);
	ARCPipe pipepath (path,pStretch->GetLaneNumber() * pStretch->GetLaneWidth());
	int npippathcnt  = (int)pipepath.m_centerPath.size();
	
	CTexture * pTexture = pView->getTextureResource()->getTexture("road");
	if(pTexture)
		pTexture->Apply();

	double totaLength=0.0;
	for (int i=0; i<npippathcnt-1;i++)
	{
		totaLength +=pipepath.m_sidePath1[i].DistanceTo(pipepath.m_sidePath1[i+1]);
	}
	glBegin(GL_QUADS);
	ARCVector3 prePointL = pipepath.m_sidePath1[0];
	ARCVector3 prePointR  = pipepath.m_sidePath2[0];

     glColor4d(1.0,1.0,1.0,0.4);


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
		glTexCoord2d(0,dTexCoordLeft);
		glVertex3dv(prePointL);
		glTexCoord2d(1,dTexCoordRight);
		glVertex3dv(prePointR);			
		
		DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
		dTexCoordLeft += dLen/totaLength;/* * 0.002*/
		dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
		dTexCoordRight += dLen/totaLength;		
		
		glTexCoord2d(1,dTexCoordRight);
		glVertex3dv(pipepath.m_sidePath2[i]);
		glTexCoord2d(0,dTexCoordLeft);
		glVertex3dv(pipepath.m_sidePath1[i]);


		std::swap(dTexCoordLeft, dTexCoordRight);
		prePointL = pipepath.m_sidePath1[i];
		prePointR = pipepath.m_sidePath2[i];
	}
	glEnd();
	glDisable(GL_LIGHTING);
	if (!IsInEdit())
	{
		glEnable(GL_BLEND)			;
	}

}

void CStretch3D::RenderMarkings( C3DView * pView )
{
	if (!glIsEnabled(GL_BLEND))
	{
		glEnable(GL_BLEND);
	}
	glEnable(GL_LIGHTING);
	Stretch * pStretch = GetStretch();

	if(!pStretch->GetPath().getCount())
		return;

	CPath2008 cullPath;

	GetCullPath(cullPath,pStretch);
	if (cullPath.getCount()<2)
	{
		cullPath = pStretch->GetPath();
	}

	CPath2008 path =	airsideThick.ModifyPathAlt(cullPath,pView);
	CPath2008 belowPath;
	airsideThick.AddThickness(path,belowPath);

	ARCPipe pipepath (path,pStretch->GetLaneNumber() * pStretch->GetLaneWidth());
	ARCPipe pipepath1 (belowPath,pStretch->GetLaneNumber() * pStretch->GetLaneWidth());

	int npippathcnt  = (int)pipepath.m_centerPath.size();

	CTexture * pTexture = pView->getTextureResource()->getTexture("RwCenter");
	if(pTexture)
		pTexture->Apply();

	DistanceUnit offset = 0.1;
	int lanNum = pStretch->GetLaneNumber();

	glBegin(GL_QUADS);
	ARCVector3 prePointL = pipepath.m_sidePath1[0];
	ARCVector3 prePointR  = pipepath.m_sidePath2[0];

	ARCVector3 prePointL1 = pipepath1.m_sidePath1[0];
	ARCVector3 prePointR1 = pipepath1.m_sidePath2[0];

//	glColor4d(1.0,1.0,1.0,0.0);
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

void CStretch3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetStretch()->m_path.DoOffset(dx,dy,dz);
}

void CStretch3D::RenderEditPoint( C3DView * pView )
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

void CStretch3D::RenderEditPointText()
{
	glDisable(GL_TEXTURE_2D);
    glColor3ub(0,0,0);
	if (!IsInEdit()) 
		return;
	CString strNum;
	CString objectName = GetStretch()->GetObjNameString();
	Path path = GetStretch()->GetPath();
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

void CStretch3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetStretch()->GetPath().getCount();

	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,GetStretch()->m_path,i) );
	}

	ALTObject3D::DoSync();	
}

void CStretch3D::SnapTo( const ALTObject3DList& RwOrTaxiList )
{

	CPoint2008 headPoint = GetStretch()->getPath().getPoint(0);
	CPoint2008 snapHead  = headPoint;
	DistanceUnit minDistHead =ARCMath::DISTANCE_INFINITE;
	DistanceUnit minDistTail = ARCMath::DISTANCE_INFINITE;
	bool bToSnapHead = false;
	bool bToSnapTail = false;

	int nPtCnt = GetStretch()->getPath().getCount();
	CPoint2008 tailPoint = GetStretch()->getPath().getPoint( nPtCnt -1);
	CPoint2008 snapTail = tailPoint;

	for(ALTObject3DList::const_iterator itr = RwOrTaxiList.begin();itr!=RwOrTaxiList.end();itr++){
		const ALTObject3D * pObj3D  = (*itr).get();
		if(pObj3D == this) continue;
		CPath2008 otherPath;
		DistanceUnit otherWidth;

		if( pObj3D->GetObjectType() == ALT_STRETCH )
		{
			Stretch* pStretch = (Stretch*)(pObj3D->GetObject());	
			otherPath = pStretch->getPath();
			otherWidth = pStretch->GetLaneNumber()*pStretch->GetLaneWidth();
		}	
		else {
			continue;
		}

		CPath2008 curPath = GetStretch()->getPath();
		if (curPath.IsSameAs(otherPath))
			continue;

		{ //head point
			CPoint2008 snapPt;
			if( otherPath.GetSnapPoint(otherWidth, headPoint, snapPt) )
			{
				DistanceUnit snapDist = snapPt.distance3D( headPoint) ;
				if( snapDist < minDistHead )
				{
					minDistHead = snapDist;
					snapHead = snapPt;
					bToSnapHead = true;
				}
			}
		}
		{ // tail point 
			CPoint2008 snapPt;
			if( otherPath.GetSnapPoint(otherWidth, tailPoint, snapPt))
			{
				DistanceUnit snapDist = snapPt.distance3D( tailPoint );
				if(snapDist < minDistTail )
				{
					minDistTail  = snapDist;
					snapTail = snapPt;
					bToSnapTail = true;
				}
			}
		}
	}	

	if( bToSnapHead )
	{
		GetStretch()->getPath()[0] = snapHead;
	}
	if( bToSnapTail )
	{
		GetStretch()->getPath()[nPtCnt-1] = snapTail;
	}

}


void CStretch3D::SetRelatFillets( const CIntersectedStretch3DInAirport& vStretchs )
{
	m_vStretchPoints.clear();
	stetchInAirport = vStretchs;
	for(int i =0 ;i< vStretchs.GetCount(); i++ )
	{
		CIntersectedStretch3D* pFillet3D = vStretchs.GetStretch3D(i);
		if( pFillet3D->m_interStretch.GetObject1ID() == GetStretch()->getID() )
		{
			m_vStretchPoints.push_back(pFillet3D->m_fristPoint.get());		
		}

		if( pFillet3D->m_interStretch.GetObject2ID() == GetStretch()->getID() )
		{
			m_vStretchPoints.push_back(pFillet3D->m_secondPoint.get());
		}
	}
}

void CStretch3D::RenderOneStencil()
{
	Stretch * pStretch = GetStretch();
	if (!pStretch->GetPath().getCount())
		return;

	if(IsNeedSync()){
		DoSync();
	}	

	ARCPipe pipepath (m_path,pStretch->GetLaneNumber() * pStretch->GetLaneWidth()*1.1);

	for (int i=0;i<m_path.getCount()-1;i++)
	{
		ARCVector3 vPrePoint1(pipepath.m_sidePath1[i][VX],pipepath.m_sidePath1[i][VY],0.0);
		ARCVector3 vAftPoint1(pipepath.m_sidePath1[i+1][VX],pipepath.m_sidePath1[i+1][VY],0.0);
		ARCVector3 vPrePoint2(pipepath.m_sidePath2[i][VX],pipepath.m_sidePath2[i][VY],0.0);
		ARCVector3 vAftPoint2(pipepath.m_sidePath2[i+1][VX],pipepath.m_sidePath2[i+1][VY],0.0);

		//if (i>0&&i<m_path.getCount()-2)
		//{
		//	ARCVector3 tempVectorPre1 = (vPrePoint1-vPrePoint2).Normalize();
		//	ARCVector3 tempVectorAft1 = (vAftPoint1-vAftPoint2).Normalize();

		//	vPrePoint1 += tempVectorPre1*1300.0;
		//	vAftPoint1 += tempVectorAft1*1300.0;

		//	ARCVector3 tempVectorPre2 = (vPrePoint2-vPrePoint1).Normalize();
		//	ARCVector3 tempVectorAft2 = (vAftPoint2-vAftPoint1).Normalize();

		//	vPrePoint2 += tempVectorPre2*1300.0;
		//	vAftPoint2 += tempVectorAft2*1300.0;
		//}

		if (pipepath.m_centerPath[i][VZ]<0.0 && pipepath.m_centerPath[i+1][VZ]<0.0)
		{	
			glBegin(GL_QUADS);
			glVertex2d(vPrePoint1[VX],vPrePoint1[VY]);
			glVertex2d(vPrePoint2[VX],vPrePoint2[VY]);
			glVertex2d(vAftPoint2[VX],vAftPoint2[VY]);
			glVertex2d(vAftPoint1[VX],vAftPoint1[VY]);	
			glEnd();
		}
		else if (pipepath.m_centerPath[i][VZ]<0.0&& pipepath.m_centerPath[i+1][VZ]>0.0)
		{
			ARCVector3 vVectorSide1(0.0,0.0,0.0);
			ARCVector3 vVectorSide2(0.0,0.0,0.0);
			airsideThick.GetHorizonPoint(pipepath.m_sidePath1[i+1],pipepath.m_sidePath1[i],vVectorSide1);
			airsideThick.GetHorizonPoint(pipepath.m_sidePath2[i+1],pipepath.m_sidePath2[i],vVectorSide2);
			glBegin(GL_QUADS);
			glVertex2d(vVectorSide1[VX],vVectorSide1[VY]);
			glVertex2d(vVectorSide2[VX],vVectorSide2[VY]);
			glVertex2d(pipepath.m_sidePath2[i][VX],pipepath.m_sidePath2[i][VY]);
			glVertex2d(pipepath.m_sidePath1[i][VX],pipepath.m_sidePath1[i][VY]);
			glEnd();
		}
		else if (pipepath.m_centerPath[i][VZ]>0.0&&pipepath.m_centerPath[i+1][VZ]<0.0)
		{
			ARCVector3 vVectorSide1(0.0,0.0,0.0);
			ARCVector3 vVectorSide2(0.0,0.0,0.0);
			airsideThick.GetHorizonPoint(pipepath.m_sidePath1[i],pipepath.m_sidePath1[i+1],vVectorSide1);
			airsideThick.GetHorizonPoint(pipepath.m_sidePath2[i],pipepath.m_sidePath2[i+1],vVectorSide2);
			glBegin(GL_QUADS);
			glVertex2d(vVectorSide1[VX],vVectorSide1[VY]);
			glVertex2d(vVectorSide2[VX],vVectorSide2[VY]);
			glVertex2d(pipepath.m_sidePath2[i+1][VX],pipepath.m_sidePath2[i+1][VY]);
			glVertex2d(pipepath.m_sidePath1[i+1][VX],pipepath.m_sidePath1[i+1][VY]);
			glEnd();
		}
	}

}

void CStretch3D::AddEditPoint(double x, double y, double z)
{
	int recordPos = -1;
	CPath2008 &stretchPath = GetStretch()->getPath();
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

void CStretch3D::FlushChange()
{
	try
	{
		GetStretch()->UpdatePath();
		UpdateSync();
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}
}
