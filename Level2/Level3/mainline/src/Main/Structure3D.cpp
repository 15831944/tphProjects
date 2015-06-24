#include "StdAfx.h"
#include ".\structure3d.h"
#include "../InputAirside/Structure.h"
#include "Airport3D.h"
#include "Airside3D.h"
#include "../InputAirside/Runway.h"
#include "../InputAirside/Taxiway.h"
#include "Runway3D.h"
#include "3DView.h"
#include "ChildFrm.h"
#include "OffsetMap.h"
#include "../Common/DistanceLineLine.h"

CStructure3D::CStructure3D(int nID) :ALTObject3D(nID)
{		
	m_pObj  = new Structure();
	//m_pDisplayProp = new StructureDisplayProp();
}

CStructure3D::~CStructure3D(void)
{
}

void CStructure3D::DrawOGL( C3DView * pView )
{
	Render( pView );
}

void CStructure3D::DrawSelect( C3DView * pView )
{
	if( !GetDisplayProp()->m_dpShape.bOn )
	{
		return ;
	}

	
	const CPath2008& structPath = GetStructure()->GetPath();
	
	if(structPath.getCount() < 3)
		return;

	CAirside3D* pAirside3D = pView->GetParentFrame()->GetAirside3D();
	ASSERT(pAirside3D);
	CAirport3D * pAirport3D = pAirside3D->GetActiveAirport();
	ASSERT(pAirport3D);
	DistanceUnit dALt = pAirport3D->m_altAirport.getElevation();
	for(int i =0 ;i< structPath.getCount(); i++)
	{
		CPoint2008 pointI = structPath.getPoint(i);
		CPoint2008 pointNext = structPath.getPoint( (i+1)%structPath.getCount() );

		UserInputData * pUseInput = GetStructure()->getUserData(i);
		UserInputData * pUseInputNext = GetStructure()->getUserData( (i+1)%structPath.getCount() );

		glBegin(GL_QUADS);
		glVertex3d(pointI.getX(), pointI.getY(), pUseInput->dElevation - dALt);
		glVertex3d(pointI.getX(), pointI.getY(), pUseInput->dElevation + pUseInput->dStructHeight - dALt);
		glVertex3d(pointNext.getX(), pointNext.getY(),  pUseInputNext->dElevation + pUseInputNext->dStructHeight - dALt);
		glVertex3d(pointNext.getX(), pointNext.getY(),  pUseInputNext->dElevation- dALt);
		glEnd();
	}
	glBegin(GL_POLYGON);
	for(int i =0;i< structPath.getCount();i++)
	{
		CPoint2008 pointI = structPath.getPoint(i);
		UserInputData * pUseInput = GetStructure()->getUserData(i);
		glVertex3d(pointI.getX(), pointI.getY(), pUseInput->dElevation + pUseInput->dStructHeight- dALt);
	}
	glEnd();
}


void CStructure3D::Render( C3DView * pView )
{	
	
	if( !GetDisplayProp()->m_dpShape.bOn)
	{
		return ;
	}

	ASSERT(GetStructure());
	if(!GetStructure())return;

	glColor4ubv(GetDisplayProp()->m_dpShape.cColor);
	glEnable(GL_LIGHTING);
	const CPath2008& structPath = GetStructure()->GetPath();
	int nPtCount = structPath.getCount();
	ASSERT(nPtCount == GetStructure()->getUserDataCount());
	ASSERT(nPtCount+1 == GetStructure()->getStructFaceCount());
	
	UpdateTexture();
	CAirside3D* pAirside3D = pView->GetParentFrame()->GetAirside3D();
	ASSERT(pAirside3D);
	CAirport3D * pAirport3D = pAirside3D->GetActiveAirport();
	ASSERT(pAirport3D);
	DistanceUnit dALt = pAirport3D->m_altAirport.getElevation();
	for(int i =0 ;i< structPath.getCount(); i++)
	{
		CPoint2008 pointI = structPath.getPoint(i);
		CPoint2008 pointNext = structPath.getPoint( (i+1)%structPath.getCount() );
        
		UserInputData * pUseInput = GetStructure()->getUserData(i);
		UserInputData * pUseInputNext = GetStructure()->getUserData( (i+1)%structPath.getCount() );
		StructFace* pFace = GetStructure()->getStructFace(i);

		glDisable(GL_TEXTURE_2D);
		CTexture* pTexture = getTextureByFileName(pFace->strPicPath);
		if(pTexture && pFace->bShow ){
			pTexture->Apply();	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		}

		ARCVector3 vDir = pointNext - pointI;
		//if(i < structPath.getCount()/2 )
			glNormal3d(vDir[VY], -vDir[VX],0);
		//else
			//glNormal3d(-vDir[VY],vDir[VX],0);

		glBegin(GL_QUADS);
			glTexCoord2d(0,0);
			glVertex3d(pointI.getX(), pointI.getY(), pUseInput->dElevation - dALt);

			glTexCoord2d(0,1);
			glVertex3d(pointI.getX(), pointI.getY(), pUseInput->dElevation + pUseInput->dStructHeight - dALt);

			glTexCoord2d(1,1);
			glVertex3d(pointNext.getX(), pointNext.getY(),  pUseInputNext->dElevation + pUseInputNext->dStructHeight - dALt);
			
			glTexCoord2d(1,0);
			glVertex3d(pointNext.getX(), pointNext.getY(),  pUseInputNext->dElevation - dALt);
		glEnd();
	}

	ARCVector3 minPt, maxPt;
	minPt = structPath.getPoint(0);
	maxPt = structPath.getPoint(0);
	for(int i =1;i< structPath.getCount();i++)
	{
		ARCVector3 pt = structPath.getPoint(i);
		minPt[VX] = min( minPt[VX],pt[VX] );
		minPt[VY] = min( minPt[VY],pt[VY] );
		minPt[VZ] = min( minPt[VZ],pt[VZ] );

		maxPt[VX] = max(maxPt[VX], pt[VX] );
		maxPt[VY] = max(maxPt[VY], pt[VY] );
		maxPt[VZ] = max(maxPt[VZ], pt[VZ] );
	}

	//render top
	glDisable(GL_TEXTURE_2D);
	StructFace* pFace = GetStructure()->getStructFace(nPtCount);
	CTexture* pTexture = getTextureByFileName(pFace->strPicPath);
	if(pTexture && pFace->bShow ){
		pTexture->Apply();	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	
	glBegin(GL_POLYGON);
	glNormal3d(0,0,1);
	for(int i =0;i< structPath.getCount();i++)
	{
		CPoint2008 pointI = structPath.getPoint(i);
		UserInputData * pUseInput = GetStructure()->getUserData(i);
		double dTexU = (pointI.getX() - minPt[VX])/(maxPt[VX]-minPt[VX]);
		double dTexV = (pointI.getY() - minPt[VY])/(maxPt[VY]-minPt[VY]);
		glTexCoord2d(dTexU,dTexV);
		glVertex3d(pointI.getX(), pointI.getY(), pUseInput->dElevation + pUseInput->dStructHeight - dALt);
	}
	glEnd();

	UpdateIntersectionPoints(pView);
	//render obstruction points
	
	glColor3ub(255,0,0);
	glBegin(GL_LINES);
	for(int i=0;i<structPath.getCount();++i)
	{
		CPoint2008 vPt = structPath.getPoint(i);
		UserInputData * pUseInput = GetStructure()->getUserData(i);
		DistanceUnit dHeight = m_vObstructionHeights[i];
		if( dHeight > pUseInput->dElevation - dALt && dHeight < pUseInput->dElevation + pUseInput->dStructHeight - dALt)
		{
			glVertex3d(vPt.getX(), vPt.getY(), dHeight);
			glVertex3d(vPt.getX(), vPt.getY(),  pUseInput->dElevation + pUseInput->dStructHeight - dALt);
		}
		
	}
	glEnd();
	

	std::vector<ARCVector3> vtopVertexs;
	glDepthMask(GL_FALSE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0,-1.0);
	glBegin(GL_QUADS);
	for(int i=0;i<structPath.getCount();++i)
	{
		int nextIdx = (i+1)%structPath.getCount();
		CPoint2008 vPt = structPath.getPoint(i);
		CPoint2008 vPt2 = structPath.getPoint(nextIdx);

		ARCVector3 vDir = vPt2 - vPt;
		if(i < structPath.getCount()/2 )
			glNormal3d(vDir[VY], -vDir[VX],0);
		else
			glNormal3d(-vDir[VY],vDir[VX],0);

		UserInputData * pUseInput = GetStructure()->getUserData(i);
		UserInputData * pUserInputNext = GetStructure()->getUserData( nextIdx );

		bool bheight1blow= false;
		bool bheight2blew = false;
		DistanceUnit dHeight = m_vObstructionHeights[i];
		DistanceUnit dHeight2 = m_vObstructionHeights[nextIdx];
		DistanceUnit dMaxHeight = pUseInput->dElevation + pUseInput->dStructHeight - dALt;
		DistanceUnit dMaxHeight2 = pUserInputNext->dElevation + pUserInputNext->dStructHeight - dALt;

		if( dHeight < dMaxHeight )
		{		
			bheight1blow = true;
		}
		if( dHeight2 < dMaxHeight2 )
		{			
			bheight2blew = true;
		}	

		if(bheight1blow)
		{
			glVertex3d(vPt.getX(), vPt.getY(), dHeight);
			glVertex3d(vPt.getX(), vPt.getY(),  dMaxHeight);
			vtopVertexs.push_back( ARCVector3(vPt.getX(), vPt.getY(),  dMaxHeight) );	
		}
		if( (bheight1blow&&!bheight2blew) || (!bheight1blow && bheight2blew) )
		{
			DistanceUnit dRat = abs(dMaxHeight - dHeight)/abs(dMaxHeight2 - dHeight2 );
			dRat = 1.0/(1.0+dRat);
			ARCVector3 vMid = ARCVector3(vPt.getX(),vPt.getY(),dMaxHeight) * (dRat) + ARCVector3(vPt2.getX(),vPt2.getY(),dMaxHeight2)*(1-dRat);
			glVertex3dv(vMid.n);
			glVertex3dv(vMid.n);
			vtopVertexs.push_back(vMid);
		}
		if(bheight2blew)
		{
			glVertex3d(vPt2.getX(), vPt2.getY(), dMaxHeight2);
			glVertex3d(vPt2.getX(), vPt2.getY(), dHeight2);
			vtopVertexs.push_back( ARCVector3(vPt2.getX(), vPt2.getY(),  dMaxHeight2) );
		}
		
	}		
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3d(0,0,1);
	for(int i =0;i<(int)vtopVertexs.size();++i)
	{
		glVertex3dv(vtopVertexs[i].n);
	}
	glEnd();

	glDepthMask(GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);
}




Structure * CStructure3D::GetStructure() const
{
	return (Structure*)GetObject();
}

ARCPoint3 CStructure3D::GetLocation( void ) const
{
	const CPath2008& path = GetStructure()->GetPath();
	ARCPoint3 location(0,0,0);
	for(int i =0 ;i<path.getCount();i++){
		location += path.getPoint(i);
	}
	location /= path.getCount();
	return location; 
}

void CStructure3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetStructure()->GetPath().DoOffset(dx,dy,dz);
}

void CStructure3D::RenderShadow( C3DView * pView )
{

}

void CStructure3D::Update()
{
	try
	{
		GetStructure()->ReadObject(m_nID);
		GetDisplayProp()->ReadData(m_nID);
		UpdateTexture();
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}
	SyncDateType::Update();
}

void CStructure3D::FlushChange()
{
	try
	{
		GetStructure()->UpdatePath();
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}	
}

CTexture* CStructure3D::getTextureByFileName( const CString& strFile )
{
	for(int i=0;i<(int)m_vTextures.size();++i)
	{
		CTexture* pTexture = m_vTextures[i].get();
		if(pTexture->getFileName() == strFile )
			return pTexture;
	}
	return NULL;
}

CTexture* CStructure3D::NewTexture( const CString& strFile )
{
	CTexture* pTexture = new CTexture(strFile);
	m_vTextures.push_back(pTexture);
	return pTexture;
}

void CStructure3D::UpdateTexture()
{
	int nFaceCount = GetStructure()->getStructFaceCount();
	for(int i =0 ;i< nFaceCount; i++)
	{
		StructFace* pFace = GetStructure()->getStructFace(i);
		if(pFace->bShow)
		{
			CTexture* pTexture = getTextureByFileName(pFace->strPicPath);
			if(!pTexture)
			{
				pTexture = new CTexture(pFace->strPicPath);			
				m_vTextures.push_back(pTexture);
			}
		}
	}
}
//
//StructureDisplayProp* CStructure3D::GetDisplayProp() const
//{
//	return (StructureDisplayProp*)m_pDisplayProp.get();
//}

void CStructure3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	GetStructure()->GetPath().DoOffset(-center[VX],-center[VY],-center[VZ]);
	GetStructure()->GetPath().Rotate(dx);
	GetStructure()->GetPath().DoOffset(center[VX],center[VY],center[VZ]);
}




void CStructure3D::UpdateIntersectionPoints( C3DView* pView )
{
	
	CAirside3D* pAirside3D = pView->GetParentFrame()->GetAirside3D();
	CAirport3D * pAirport3D = pAirside3D->GetActiveAirport();
	DistanceUnit dALt = pAirport3D->m_altAirport.getElevation();
	int nPointCnt = GetStructure()->GetPath().getCount();
	m_vObstructionHeights.clear();
	m_vObstructionHeights.resize(nPointCnt, ARCMath::DISTANCE_INFINITE);
	//find the first intersections
	for(size_t i=0;i< pAirport3D->m_vRunways.size(); i++)
	{
		CRunway3D* pRunway3D = (CRunway3D*) pAirport3D->m_vRunways[i].get();
		Runway* pRunway = pRunway3D->GetRunway();
		ARCVector3 vPos = GetLocation();

		ARCVector3 vPoint1ToPos = vPos - pRunway->GetPath().getPoint(0);
		ARCVector3 vPoint2ToPos = vPos - pRunway->GetPath().getPoint(1);

		double cosAngleOfDirRunway1 = pRunway->GetDir(RUNWAYMARK_FIRST).GetCosOfTwoVector( vPoint1ToPos  ) ;
		double cosAngleOfDirRunway2 = pRunway->GetDir(RUNWAYMARK_SECOND).GetCosOfTwoVector( vPoint2ToPos );


		//check the if center point is in the which side
		if(cosAngleOfDirRunway1 > 0 && cosAngleOfDirRunway2  < 0 ) //means point behind the end 
		{
			if(pRunway->GetMark1Show())
			{
				OffsetHightMap offsetHightMap;
				offsetHightMap.BuildHeightMap(pRunway->GetObstruction(RUNWAYMARK_FIRST).ThresHold);

				for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
				{
					ARCVector3 ptTpPoint1 = GetStructure()->GetPath().getPoint(ptIdx) - pRunway->GetPath().getPoint(1);
					double cosAngleOfDirRunway1 = pRunway->GetDir(RUNWAYMARK_SECOND).GetCosOfTwoVector( ptTpPoint1 );
					DistanceUnit doffset = ptTpPoint1.Length() * abs(cosAngleOfDirRunway1);
					if(doffset < offsetHightMap.getMaxOffset() )
						m_vObstructionHeights[ptIdx] = offsetHightMap.getOffsetHeight(doffset);
					else
						m_vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
				}
				if(isObstructionsValid(dALt))
					return;
			}

			if(pRunway->GetMark2Show())
			{
				OffsetHightMap offsetHightMap;
				offsetHightMap.BuildHeightMap(pRunway->GetObstruction(RUNWAYMARK_SECOND).ThresHold);

				for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
				{
					ARCVector3 ptTpPoint1 = GetStructure()->GetPath().getPoint(ptIdx) - pRunway->GetPath().getPoint(0);
					double cosAngleOfDirRunway1 = pRunway->GetDir(RUNWAYMARK_FIRST).GetCosOfTwoVector( ptTpPoint1 );
					DistanceUnit doffset = ptTpPoint1.Length() * abs(cosAngleOfDirRunway1);
					if(doffset < offsetHightMap.getMaxOffset() )
						m_vObstructionHeights[ptIdx] = offsetHightMap.getOffsetHeight(doffset);
					else
						m_vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
				}
				if(isObstructionsValid(dALt))
					return;
			}

		}
		if(cosAngleOfDirRunway1 >0 && cosAngleOfDirRunway2 > 0) //means point at the side
		{			
			if( pRunway->GetMark1Show())
			{
				OffsetHightMap ofsetHiehgMapRw1;
				ofsetHiehgMapRw1.BuildHeightMap(pRunway->GetObstruction(RUNWAYMARK_FIRST).Lateral);

				for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
				{
					ARCVector3 ptToPoint1 = GetStructure()->GetPath().getPoint(ptIdx) - pRunway->GetPath().getPoint(0);
					double cosAngleOfDirRunway1 = pRunway->GetDir(RUNWAYMARK_FIRST).GetCosOfTwoVector( ptToPoint1  );
					DistanceUnit dOffset = ptToPoint1.Length() * sqrt(1-cosAngleOfDirRunway1*cosAngleOfDirRunway1);
					dOffset -= pRunway->GetWidth() * 0.5;
					if(dOffset < ofsetHiehgMapRw1.getMaxOffset() )				
						m_vObstructionHeights[ptIdx] = ofsetHiehgMapRw1.getOffsetHeight(dOffset);
					else
						m_vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
				}
				if( isObstructionsValid(dALt) )
					return;
			}
			
			//runway 2
			if( pRunway->GetMark2Show() )
			{
				OffsetHightMap ofsetheightMapRw2;
				ofsetheightMapRw2.BuildHeightMap(pRunway->GetObstruction(RUNWAYMARK_SECOND).Lateral);
				for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
				{
					ARCVector3 ptToPoint1 = GetStructure()->GetPath().getPoint(ptIdx) - pRunway->GetPath().getPoint(0);
					double cosAngleOfDirRunway1 = pRunway->GetDir(RUNWAYMARK_FIRST).GetCosOfTwoVector( ptToPoint1  );
					DistanceUnit dOffset = ptToPoint1.Length() * sqrt(1-cosAngleOfDirRunway1*cosAngleOfDirRunway1);
					dOffset -= pRunway->GetWidth() * 0.5;
					if(dOffset < ofsetheightMapRw2.getMaxOffset() )				
						m_vObstructionHeights[ptIdx] = ofsetheightMapRw2.getOffsetHeight(dOffset);
					else
						m_vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
				}
				if( isObstructionsValid(dALt) )
					return;
			}			

		}
		if( cosAngleOfDirRunway1 < 0 && cosAngleOfDirRunway2 > 0) //means point behind the threshold
		{
			if( pRunway->GetMark1Show() )
			{
				OffsetHightMap offsetHightMap;
				offsetHightMap.BuildHeightMap(pRunway->GetObstruction(RUNWAYMARK_FIRST).ThresHold);
				
				for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
				{
					ARCVector3 ptTpPoint1 = GetStructure()->GetPath().getPoint(ptIdx) - pRunway->GetPath().getPoint(0);
					double cosAngleOfDirRunway1 = pRunway->GetDir(RUNWAYMARK_FIRST).GetCosOfTwoVector( ptTpPoint1 );
					DistanceUnit doffset = ptTpPoint1.Length() * abs(cosAngleOfDirRunway1);
					if(doffset < offsetHightMap.getMaxOffset() )
						m_vObstructionHeights[ptIdx] = offsetHightMap.getOffsetHeight(doffset);
					else
						m_vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
				}
				if(isObstructionsValid(dALt))
					return;

			}
			if( pRunway->GetMark2Show() )
			{
				OffsetHightMap offsetHightMap;
				offsetHightMap.BuildHeightMap( pRunway->GetObstruction(RUNWAYMARK_SECOND).ThresHold );

				for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
				{
					ARCVector3 ptTpPoint1 = GetStructure()->GetPath().getPoint(ptIdx) - pRunway->GetPath().getPoint(1);
					double cosAngleOfDirRunway1 = pRunway->GetDir(RUNWAYMARK_SECOND).GetCosOfTwoVector( ptTpPoint1 );
					DistanceUnit doffset = ptTpPoint1.Length() * abs(cosAngleOfDirRunway1);
					if(doffset < offsetHightMap.getMaxOffset() )
						m_vObstructionHeights[ptIdx] = offsetHightMap.getOffsetHeight(doffset);
					else
						m_vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
				}
				if(isObstructionsValid(dALt))
					return;
			}
		}		
		//update obstruction	
	}
	//
	for(size_t i=0;i< pAirport3D->m_vTaxways.size();i++)
	{
		CTaxiway3D * pTaxiway3D = (CTaxiway3D*)pAirport3D->m_vTaxways[i].get();
		if(pTaxiway3D && pTaxiway3D->GetTaxiway() && pTaxiway3D->GetTaxiway()->GetShow() )
		{
			OffsetHightMap offsetHightMap;
			offsetHightMap.BuildHeightMap( pTaxiway3D->GetTaxiway()->GetObjSurface() );

			for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
			{
				CPoint2008 pt = GetStructure()->GetPath().getPoint(ptIdx);
				DistancePointPath3D distPtLine;
				DistanceUnit doffset = sqrt(distPtLine.GetSquared(pt, pTaxiway3D->GetTaxiway()->GetPath()));
				doffset -= pTaxiway3D->GetTaxiway()->GetWidth() * 0.5;
				if(doffset < offsetHightMap.getMaxOffset() )
					m_vObstructionHeights[ptIdx] = offsetHightMap.getOffsetHeight(doffset);
				else
					m_vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
			}
			if(isObstructionsValid(dALt))
				return;	
		}		
	}
}

bool CStructure3D::isObstructionsValid(DistanceUnit dAlt) const
{
	int nPtCount = GetStructure()->GetPath().getCount();
	for(int i=0;i<nPtCount;++i)
	{
		UserInputData * pUseInput = GetStructure()->getUserData(i);
		if(  m_vObstructionHeights[i] < pUseInput->dElevation + pUseInput->dStructHeight - dAlt)
			return true;
	}

	return false;
}