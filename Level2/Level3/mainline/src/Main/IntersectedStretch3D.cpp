#include "StdAfx.h"
#include ".\intersectedstretch3d.h"
#include "ShapeRenderer.h"
#include <Common/ShapeGenerator.h>
#include "../InputAirside/IntersectionNodesInAirport.h"
#include "3DView.h"
#include "../InputAirside/Stretch.h"
#include "../Common/ARCPipe.h"
#include "../Common/line2008.h"
#include "../Common/point2008.h"
#include "../InputAirside/IntersectItem.h"
#include "TermPlanDoc.h"
#include "glrender/glTextureResource.h"

/************************************************************************/
/*                      CIntersectedStretch3DPoint                      */
/************************************************************************/

CIntersectedStretch3DPoint::CIntersectedStretch3DPoint(IntersectedStretch& stretch, int mid):m_stretch3D(stretch),m_index(mid)
{

}
CIntersectedStretch3DPoint::~CIntersectedStretch3DPoint()
{

}

ARCPoint3 CIntersectedStretch3DPoint::GetLocation()const
{
	if (m_index == 0)
	{
		return m_stretch3D.GetFristIntersectedStretchPointPos();
	}
	else
	{
		return m_stretch3D.GetSecondIntersectedStretchPointPos();
	}
}

IntersectedStretchPoint& CIntersectedStretch3DPoint::GetIntersectedStretchPt()
{
	if (m_index==0)
	{
		return m_stretch3D.GetFristIntersectedStretchPoint();
	}
	else
	{
		return m_stretch3D.GetSecondIntersectedStretchPoint();
	}
}

void CIntersectedStretch3DPoint::DrawOGL(C3DView* pView)
{

	ARCPoint3 location = GetLocation();
	glColor3ub(255,0,0);
    DrawFlatSquare(location[VX],location[VY],location[VZ],250);

}

void CIntersectedStretch3DPoint::OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */)
{
	CPoint2008 vPtDir;
	vPtDir = ( (m_index ==0)? m_stretch3D.GetFristStretchDir() : m_stretch3D.GetSecondStretchDir() );

	CPoint2008 vOff(dx,dy,dz);

	double cosZeta = vOff.GetCosOfTwoVector(vPtDir);

	DistanceUnit doffset = cosZeta * vOff.length();

//	GetIntersectedStretchPt().SetDisToUser( GetIntersectedStretchPt().GetDistToUser() + doffset);
}

void CIntersectedStretch3DPoint::AfterMove()
{
	m_stretch3D.SaveDataToDB();
}

Selectable::SelectType CIntersectedStretch3DPoint::GetSelectType() const
{
	return INTERSECTED_POINT;
}

IntersectionNode& CIntersectedStretch3DPoint::GetIntersectionNode()
{
	return m_stretch3D.GetIntersectionNode();
}

DistanceUnit CIntersectedStretch3DPoint::GetDistInObj() const
{
	if(m_index==0)
	{
		return m_stretch3D.GetFristPointDisInPath();
	}
	else
	{
		return m_stretch3D.GetSecondPointDisInPath();
	}
}



/************************************************************************/
/*                     CIntersectedStretch3D                            */
/************************************************************************/

CIntersectedStretch3D::CIntersectedStretch3D(IntersectedStretch& interStretch):m_interStretch(interStretch)
{
	m_fristPoint = new CIntersectedStretch3DPoint(m_interStretch,0);
	m_secondPoint = new CIntersectedStretch3DPoint(m_interStretch,1);
	GenSmoothStretchPath();
}

CIntersectedStretch3D::~CIntersectedStretch3D(void)
{
}

void CIntersectedStretch3D::GenSmoothStretchPath()
{
	std::vector<CPoint2008> vCtrlPoints;
	std::vector<ARCVector3> vInputPoints;
	//m_vPath.clear();

	if (m_interStretch.GetControlPoint(vCtrlPoints) && vCtrlPoints.size()>=2)
	{
		int elementCount = static_cast<int>(vCtrlPoints.size());
		ARCVector3 vBegin = vCtrlPoints[1]-vCtrlPoints[0];
		ARCVector3 vEnd = vCtrlPoints[elementCount-2]-vCtrlPoints[elementCount-1];
		DistanceUnit disTance = vCtrlPoints[0].distance3D(vCtrlPoints[elementCount-1]);
		vBegin.SetLength(disTance*0.5);
		vEnd.SetLength(disTance*0.5);
		vInputPoints.reserve(4);
		vInputPoints.push_back(vCtrlPoints[0]);
		vInputPoints.push_back(vCtrlPoints[0] + vBegin);
		vInputPoints.push_back(vCtrlPoints[elementCount-1] +vEnd);
		vInputPoints.push_back(vCtrlPoints[elementCount-1]);

		m_vPath.clear();
		SHAPEGEN.GenBezierPath(vInputPoints,m_vPath,10);

		ARCVector3 m_fristExt = m_interStretch.GetExtentFristPoint();
		ARCVector3 m_secondExt = m_interStretch.GetExtentSecondPoint();

		m_vPath.insert(m_vPath.begin(),m_fristExt);
		m_vPath.push_back(m_secondExt);
	}
}

void CIntersectedStretch3D::DrawLine(C3DView* pView,bool bEdit)
{
	DistanceUnit slwidth = 50;	

	ShapeGenerator::WidthPipePath widthpath;	

//	if(bEdit)
//		GenSmoothStretchPath();

	SHAPEGEN.GenWidthPipePath(m_vPath,slwidth,widthpath);

	glNormal3f(0,0,1);
	glColor3ub(255,0,0);
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<(int)widthpath.leftpath.size();i++){
		glVertex3dv(widthpath.leftpath[i]);
		glVertex3dv(widthpath.rightpath[i]);
	}	
	glEnd();
}

void CIntersectedStretch3D::DrawOGL(C3DView* pView)
{
	glColor3ub(255,255,255);
	ShapeGenerator::WidthPipePath widthPipePath;
	ShapeGenerator::WidthPipePath widthPipePath1;
//	GenSmoothStretchPath();

	//if (m_vPath[0][VZ]!=m_vPath[(int)m_vPath.size()-1][VZ])
	//{
	//	return;
	//}
    ARCPath3 abovePath = airsideThick.ModifyPathAlt(m_vPath,pView);
	ARCPath3 belowPath = airsideThick.AddThickness(abovePath);
	if (m_interStretch.GetFristWidth()==m_interStretch.GetSecondWidth())
	{
		SHAPEGEN.GenWidthPipePath(abovePath,m_interStretch.GetMinWidth(),widthPipePath);
		SHAPEGEN.GenWidthPipePath(belowPath,m_interStretch.GetMinWidth(),widthPipePath1);
	}
	else
	{
		SHAPEGEN.GenWidthPipePath(abovePath,m_interStretch.GetFristWidth(),m_interStretch.GetSecondWidth(),widthPipePath);
		SHAPEGEN.GenWidthPipePath(belowPath,m_interStretch.GetFristWidth(),m_interStretch.GetSecondWidth(),widthPipePath1);
	}

//	SHAPEGEN.GenWidthPipePath(abovePath,m_interStretch.GetMinWidth(),widthPipePath);

	double dLenside1 =0; double dLenside2 = 0;

	ARCVector3 prePoint1 = widthPipePath.leftpath[0];
	ARCVector3 prePoint2 = widthPipePath.rightpath[0];

	ARCVector3 prePointL1 = widthPipePath1.leftpath[0];
	ARCVector3 prePointR2 = widthPipePath1.rightpath[0];

	int npippathcnt = (int)widthPipePath.leftpath.size();

	double texcord1 = 0.0;
	double texcord2 = 0.0;
    ARCVector3 vNormalize(0.0,0.0,0.0);
//	glCullFace(GL_BACK);
	glBegin(GL_QUADS);	
	{	
		////above
		for( int i =1; i < npippathcnt;++i)
		{	
			glNormal3d(0.0,0.0,1.0);
			glTexCoord2d(0,texcord1);
			glVertex3dv(prePoint1);
			glTexCoord2d(1,texcord2);
			glVertex3dv(prePoint2);


			double dLen1 = prePoint1.DistanceTo(widthPipePath.leftpath[i]);
			double dLen2 = prePoint2.DistanceTo(widthPipePath.rightpath[i]);

			texcord1 += dLen1 * 0.0000002;
			texcord2 += dLen2 * 0.0000002;		

			glTexCoord2d(1,texcord2);
			glVertex3dv(widthPipePath.rightpath[i]);	
			glTexCoord2d(0,texcord1);
			glVertex3dv(widthPipePath.leftpath[i]);	

			prePoint1 = widthPipePath.leftpath[i];
			prePoint2 = widthPipePath.rightpath[i];

			std::swap(texcord1,texcord2);
		}

	}

	glEnd();


	//below
	glBegin(GL_QUADS);
	{
		texcord1 = 0.0;
		texcord2 = 0.0;
		ARCVector3 vNormalize(0.0,0.0,0.0);
		for ( int i=1; i<npippathcnt; i++)
		{

	        glNormal3d(0.0,0.0,1.0);
			glTexCoord2d(0,texcord1);
			glVertex3dv(prePointL1);
			glTexCoord2d(1,texcord2);
			glVertex3dv(prePointR2);


			double dLen1 = prePointL1.DistanceTo(widthPipePath1.leftpath[i]);
			double dLen2 = prePointR2.DistanceTo(widthPipePath1.rightpath[i]);

			texcord1 += dLen1 * 0.0000002;
			texcord2 += dLen2 * 0.0000002;		

			glTexCoord2d(1,texcord2);
			glVertex3dv(widthPipePath1.rightpath[i]);	
			glTexCoord2d(0,texcord1);
			glVertex3dv(widthPipePath1.leftpath[i]);	

			prePointL1 = widthPipePath1.leftpath[i];
			prePointR2 = widthPipePath1.rightpath[i];

			std::swap(texcord1,texcord2);
		}
	}
	glEnd();
    airsideThick.RenderSidePath(widthPipePath.leftpath,widthPipePath.rightpath,widthPipePath1.leftpath,widthPipePath1.rightpath);
	
}

void CIntersectedStretch3D::RenderOneStencil()
{
	ShapeGenerator::WidthPipePath widthPipePath;
	SHAPEGEN.GenWidthPipePath(m_vPath, m_interStretch.GetMinWidth(),widthPipePath);
	int elementCount = (int)widthPipePath.leftpath.size();
	for(int i=0; i<elementCount; i++)
	{
		if (widthPipePath.leftpath[i][VZ]<0.0||widthPipePath.rightpath[i+1][VZ]<0.0)
		{
			glBegin(GL_QUADS);
			glVertex2d(widthPipePath.leftpath[i][VX],widthPipePath.leftpath[i][VY]);
			glVertex2d(widthPipePath.rightpath[i][VX],widthPipePath.rightpath[i][VY]);
			glVertex2d(widthPipePath.rightpath[i+1][VX],widthPipePath.rightpath[i+1][VY]);
			glVertex2d(widthPipePath.leftpath[i+1][VX],widthPipePath.leftpath[i+1][VY]);
		    glEnd();
		}
	}
}

void CIntersectedStretch3D::DrawEditPoint(C3DView* pView,bool bSelect)
{
	//if(bSelect)
	//{
	//	glLoadName( pView->GetSelectionMap().NewSelectable( m_fristPoint.get() ) );
	//}
	//m_fristPoint->DrawOGL(pView);
	//if(bSelect)
	//{
	//	glLoadName( pView->GetSelectionMap().NewSelectable( m_secondPoint.get() ) );
	//}
	//m_secondPoint->DrawOGL(pView);
}

void CIntersectedStretch3D::DrawSelect(C3DView* pView)
{
//	glLoadName(pView->GetSelectionMap().NewSelectable(this));
//	DrawOGL(pView);
}

Selectable::SelectType CIntersectedStretch3D::GetSelectType() const
{
	return INTERSECTED_STRETCH;
}

ARCPoint3 CIntersectedStretch3D::GetLocation() const
{
//	IntersectedStretch interStretch = m_interStretch;
	return (m_fristPoint->GetLocation() + m_secondPoint->GetLocation()/* + interStretch.GetIntersectNode().GetPosition()*/) /3.0;
}


/************************************************************************/
/*                     CIntersectedStretch3DInAirport                   */
/************************************************************************/

CIntersectedStretch3DInAirport::CIntersectedStretch3DInAirport()
{

}

CIntersectedStretch3DInAirport::~CIntersectedStretch3DInAirport()
{

}

IntersectedStretchInAirport CIntersectedStretch3DInAirport::GetIntersectedStretchInAirport()const
{
	return m_intersectedStretchInAirport;
}

void CIntersectedStretch3DInAirport::DrawOGL(C3DView* pView)
{
	CTexture * pTexture = pView->getTextureResource()->getTexture("road");
	if(pTexture) pTexture->Apply();

	glColor3ub(255,255,255);
	glNormal3i(0,0,1);
//	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CCW);
	glEnable(GL_LIGHTING);
	for(int i=0;i< (int)m_vintersectedStretch3D.size();i++)
	{
		CIntersectedStretch3D * pIntersectedStretch3D = m_vintersectedStretch3D.at(i).get();

		pIntersectedStretch3D->DrawOGL(pView);
	}


//	glDisable(GL_CULL_FACE);
//	DrawLane(pView);

//	DrawLine(pView);
}

void CIntersectedStretch3DInAirport::DrawSelect(C3DView* pView)
{
	//if (IsInEdit())
	//{
		//for (int i=0; i<static_cast<int>(m_vintersectedStretch3D.size()); i++)
		//{   
		//	CIntersectedStretch3D * pIntersectedStretch3D = m_vintersectedStretch3D.at(i).get();
		//	if (IsInEdit())
		//	{
		//	pIntersectedStretch3D->DrawEditPoint(pView,TRUE);
		//	}
//			pIntersectedStretch3D->DrawSelect(pView);
		//}
	//}	


}

void CIntersectedStretch3DInAirport::DrawLine(C3DView* pView)
{
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_LIGHTING);
	//glColor3f(1.0,0.0,0.0);
 //   if(IsInEdit())
 //   {
 //   	for (int i=0; i<static_cast<int>(m_vintersectedStretch3D.size()); i++)
 //   	{   
	//    	CIntersectedStretch3D * pIntersectedStretch3D = m_vintersectedStretch3D.at(i).get();
	//		pIntersectedStretch3D->DrawEditPoint(pView);
	//    	pIntersectedStretch3D->DrawLine(pView);
	//    }

	//}


}


void CIntersectedStretch3DInAirport::InitIntersectedStrechInAirport(int nAirport,const IntersectionNodesInAirport& nodeList)
{
	m_bInEdit = false;
	m_iInEdit = -1;
	m_nAirport = nAirport;
	m_intersectedStretchInAirport.ReadData(nAirport,nodeList);
	m_vintersectedStretch3D.clear();
	for(int i=0; i<m_intersectedStretchInAirport.GetCount();i++)
	{
		CIntersectedStretch3D * pIntersectedStretch3D = new CIntersectedStretch3D(m_intersectedStretchInAirport.ItemAt(i));
		m_vintersectedStretch3D.push_back(pIntersectedStretch3D);
	}
	GetPositionOfLane();
}

void CIntersectedStretch3DInAirport::UpdateIntersectedStretchInAirport(const IntersectionNodesInAirport& nodeList,const std::vector<int>& vChangeNodeIndexs)
{
	m_vintersectedStretch3D.clear();

	m_intersectedStretchInAirport.UpdateStretch(m_nAirport,nodeList, vChangeNodeIndexs);

	for (int i=0; i<(int)m_intersectedStretchInAirport.GetCount(); i++)
	{
		CIntersectedStretch3D * pIntersectedStretch3D = new CIntersectedStretch3D(m_intersectedStretchInAirport.ItemAt(i));
		m_vintersectedStretch3D.push_back(pIntersectedStretch3D);
	}
	
	GetPositionOfLane();
}

CIntersectedStretch3D * CIntersectedStretch3DInAirport::GetInEditFillet()
{
	for(int i =0 ;i< (int)m_vintersectedStretch3D.size();i++)
	{
		CIntersectedStretch3D * pStretch3D = m_vintersectedStretch3D.at(i).get();
		if(pStretch3D->m_interStretch.GetID() == m_iInEdit)
		{
			return pStretch3D;
		}
	}
	return NULL;
}

void CIntersectedStretch3DInAirport::GetPositionOfLane()
{
	vLane.first.clear();
	vLane.second.clear();
	BOOL bSelectFirst, bSelectSecond;
	if (m_intersectedStretchInAirport.GetCount()<1)
	{
		return;
	}
	if (vLane.first.size()<1)
	{
		int id = m_intersectedStretchInAirport.ItemAt(0).GetFristIntersectedStretchPoint().GetIntersectItemID();
		IntersectedStretchPoint pInterstertch = m_intersectedStretchInAirport.ItemAt(0).GetFristIntersectedStretchPoint();
		vLane.first.push_back(id);
		vLane.second.push_back(pInterstertch);
	}
	for (int i=0; i<(int)m_intersectedStretchInAirport.GetCount();i++)
	{
			IntersectedStretch& pIntersected = m_intersectedStretchInAirport.ItemAt(i);
			int pointId1 = pIntersected.GetFristIntersectedStretchPoint().GetIntersectItemID();
			int pointId2 = pIntersected.GetSecondIntersectedStretchPoint().GetIntersectItemID();
			//IntersectItem * pItem1 = pIntersected.GetIntersectionNode().GetItemByID(pointId1);
			//IntersectItem * pItem2 = pIntersected.GetIntersectionNode().GetItemByID(pointId2);
			DistanceUnit dis1 = pIntersected.GetFristIntersectedStretchPoint().GetDistToIntersect()/*+pIntersected.GetFristIntersectedStretchPoint().GetDistToUser()*/;
			DistanceUnit dis2 = pIntersected.GetSecondIntersectedStretchPoint().GetDistToIntersect()/*+pIntersected.GetSecondIntersectedStretchPoint().GetDistToUser()*/;
			bSelectFirst = TRUE;
			for (int j=0; j<(int)vLane.first.size(); j++)
			{
				DistanceUnit dis  = vLane.second[j].GetDistToIntersect()/* + vLane.second[j].GetDistToUser()*/;
				if (vLane.first[j]==pointId1)
				{
					if ((dis1<0&&dis<0)||(dis1>0&&dis>0))
					{
						if (fabs(dis1)>fabs(dis))
						{
							vLane.second[j] = (pIntersected.GetFristIntersectedStretchPoint());
						}
						bSelectFirst = FALSE;
						break;
					}	

				}
			}

            if(bSelectFirst == TRUE)
			{
				vLane.first.push_back(pointId1);
				vLane.second.push_back((pIntersected.GetFristIntersectedStretchPoint()));
			}

			bSelectSecond = TRUE;
			for (int j=0; j<(int)vLane.first.size(); j++)
			{
				DistanceUnit dis  = vLane.second[j].GetDistToIntersect() /*+ vLane.second[j].GetDistToUser()*/;
				if (vLane.first[j]==pointId2)
				{
					if ((dis2<0&&dis<0)||(dis2>0&&dis>0))
					{
						if (fabs(dis2)>fabs(dis))
							vLane.second[j] = (pIntersected.GetSecondIntersectedStretchPoint());
						bSelectSecond = FALSE;
						break;
					}
				}
			}

            if(bSelectSecond == TRUE)
			{
				vLane.first.push_back(pointId2);
				vLane.second.push_back((pIntersected.GetSecondIntersectedStretchPoint()));
			}
	}
}

void CIntersectedStretch3DInAirport::DrawLane(C3DView* pView)
{
	glColor3f(1.0,1.0,1.0);
	glNormal3f(0.0,0.0,1.0);
	if (vLane.first.size()<1)
		return;
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(-1.0,-1.0);
	for (int i=0; i<(int)vLane.first.size();i++)
	{
		DistanceUnit dis  = vLane.second[i].GetDistToIntersect()/* + vLane.second[i].GetDistToUser()*/;
		for (int j=0; j<m_intersectedStretchInAirport.GetCount(); j++)
		{
			IntersectedStretch& pIntersected = m_intersectedStretchInAirport.ItemAt(j);
			int id1 = pIntersected.GetFristIntersectedStretchPoint().GetIntersectItemID();
			int id2 = pIntersected.GetSecondIntersectedStretchPoint().GetIntersectItemID();
			DistanceUnit dis1 = pIntersected.GetFristIntersectedStretchPoint().GetDistToIntersect()/*+pIntersected.GetFristIntersectedStretchPoint().GetDistToUser()*/;
			DistanceUnit dis2 = pIntersected.GetSecondIntersectedStretchPoint().GetDistToIntersect()/*+pIntersected.GetSecondIntersectedStretchPoint().GetDistToUser()*/;
			if (vLane.first[i] == id1 && dis == dis1)
			{
				IntersectItem * pItem = m_intersectedStretchInAirport.ItemAt(j).GetIntersectionNode().GetItemByID(id1);
				DistanceUnit disTance = vLane.second[i].GetDistToIntersect()/* + vLane.second[i].GetDistToUser()*/ + pItem->GetDistInItem();
				ALTObject * pObj = pItem->GetObject();
				CPath2008 pStretch = ((Stretch*)pObj)->GetPath();
				ARCPoint3 location = pStretch.GetDistPoint(disTance);
				double relatlen = pStretch.GetDistIndex(disTance);

				int npt = (int)relatlen;
				int nptnext;
				if(npt >= pStretch.getCount() -1 ){
					nptnext = pStretch.getCount() -2;
					npt = nptnext +1;
				}else {
					nptnext = npt +1;
				}

				ARCVector3 dir = pStretch.getPoint(npt) - pStretch.getPoint(nptnext);
				dir.z = (0);
				dir.Normalize();
				dir.RotateXY(90);
				dir.SetLength(((Stretch*)pObj)->GetLaneWidth()*((Stretch*)pObj)->GetLaneNumber()*0.5);
				CLine2008 line(location+dir, location-dir);
				CTexture * pTexture = pView->getTextureResource()->getTexture("Zebra");
				if(pTexture)
					pTexture->Apply();
				glColor3f(1.0,1.0,1.0);
				glNormal3f(0.0,0.0,1.0);

				CPoint2008 p1 = line.GetPoint1();
				CPoint2008 p2 = line.GetPoint2();

				DistanceUnit distance = p1.distance3D(p2);
				CPath2008  linePath;
				linePath.init(2);

				linePath[0]=p1;
				linePath[1]=p2;

				ARCPipe pipePath(linePath,300);
				double texcordT = distance * 0.004;

				glBegin(GL_QUAD_STRIP);
				glTexCoord2d(0,0);
				glVertex3dv(pipePath.m_sidePath1[0]);
				glTexCoord2d(0,1);
				glVertex3dv(pipePath.m_sidePath2[0]);

				glTexCoord2d(texcordT,0);
				glVertex3dv(pipePath.m_sidePath1[1]);
				glTexCoord2d(texcordT,1);
				glVertex3dv(pipePath.m_sidePath2[1]);
				glEnd();
				if(pTexture)
					pTexture->UnApply(); 
				break;
			}
			else if (vLane.first[i] == id2 && dis == dis2)
			{
				IntersectItem * pItem = m_intersectedStretchInAirport.ItemAt(j).GetIntersectionNode().GetItemByID(id2);
				DistanceUnit disTance = vLane.second[i].GetDistToIntersect()/* + vLane.second[i].GetDistToUser() */+ pItem->GetDistInItem();
				ALTObject * pObj = pItem->GetObject();
				CPath2008 pStretch = ((Stretch*)pObj)->GetPath();
				ARCPoint3 location = pStretch.GetDistPoint(disTance);
				double relatlen = pStretch.GetDistIndex(disTance);

				int npt = (int)relatlen;
				int nptnext;
				if(npt >= pStretch.getCount() -1 ){
					nptnext = pStretch.getCount() -2;
					npt = nptnext +1;
				}else {
					nptnext = npt +1;
				}

				ARCVector3 dir = pStretch.getPoint(npt) - pStretch.getPoint(nptnext);
				dir.z = (0);
				dir.Normalize();
				dir.RotateXY(90);
				dir.SetLength(((Stretch*)pObj)->GetLaneWidth()*((Stretch*)pObj)->GetLaneNumber()*0.5);
				CLine2008 line(location+dir, location-dir);
				CTexture * pTexture = pView->getTextureResource()->getTexture("Zebra");
				if(pTexture)
					pTexture->Apply();
				glColor3f(1.0,1.0,1.0);
				glNormal3f(0.0,0.0,1.0);

				CPoint2008 p1 = line.GetPoint1();
				CPoint2008 p2 = line.GetPoint2();

				DistanceUnit distance = p1.distance3D(p2);
				CPath2008  linePath;
				linePath.init(2);

				linePath[0]=p1;
				linePath[1]=p2;

				ARCPipe pipePath(linePath,300);
				double texcordT = distance * 0.004;

				glBegin(GL_QUAD_STRIP);
				glTexCoord2d(0,0);
				glVertex3dv(pipePath.m_sidePath1[0]);
				glTexCoord2d(0,1);
				glVertex3dv(pipePath.m_sidePath2[0]);

				glTexCoord2d(texcordT,0);
				glVertex3dv(pipePath.m_sidePath1[1]);
				glTexCoord2d(texcordT,1);
				glVertex3dv(pipePath.m_sidePath2[1]);
				glEnd();
				if(pTexture)
					pTexture->UnApply();  
				break;
			}
		}
	}
//	glDisable(GL_POLYGON_OFFSET_FILL);
}

void CIntersectedStretch3DInAirport::RenderStencil()
{
	for(int i=0;i< (int)m_vintersectedStretch3D.size();i++)
	{
		CIntersectedStretch3D * pIntersectedStretch3D = m_vintersectedStretch3D.at(i).get();

		pIntersectedStretch3D->RenderOneStencil();
	}
}
