#include "StdAfx.h"
#include ".\fillettaxiway3d.h"

#include "..\InputAirside\FiletTaxiway.h"
#include "3dview.h"
#include <Common/ShapeGenerator.h>
#include "ALTObject3D.h"
#include "ShapeRenderer.h"
#include "glrender/glTextureResource.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
CFilletPoint3D::CFilletPoint3D( FilletTaxiway& filletTaxiway, int idx ) : m_filletTx(filletTaxiway),m_idx(idx)
{

}

ARCPoint3 CFilletPoint3D::GetLocation() const
{
	FilletTaxiway _fillet = m_filletTx;
	if(m_idx == 0 )
	{ 
		return _fillet.GetFilletPoint1Pos();
	}
	else
		return _fillet.GetFilletPoint2Pos();
}

void CFilletPoint3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	CPoint2008 vPtDir;
	vPtDir = ( (m_idx ==0)? m_filletTx.GetFilletPoint1Dir() : m_filletTx.GetFilletPoint2Dir() );

	CPoint2008 vOff(dx,dy,dz);

	double cosZeta = vOff.GetCosOfTwoVector(vPtDir);

	DistanceUnit doffset = cosZeta * vOff.length();

	GetFilletPt().SetUsrDist( GetFilletPt().GetUsrDist() + doffset);
}

void CFilletPoint3D::AfterMove()
{
	m_filletTx.SaveDataToDB();
}

void CFilletPoint3D::DrawOGL( C3DView * pView )
{
	ARCPoint3 loc = GetLocation();
	DrawFlatSquare(loc[VX],loc[VY],loc[VZ],250);
}

Selectable::SelectType CFilletPoint3D::GetSelectType() const
{
	return FILLET_POINT;
}

FilletPoint& CFilletPoint3D::GetFilletPt()
{
	if(m_idx == 0)
	{
		return m_filletTx.GetFilletPoint1();
	}
	else
	{
		return m_filletTx.GetFilletPoint2();
	}
}

IntersectionNode& CFilletPoint3D::GetIntersectionNode() 
{
	return m_filletTx.GetIntersectNode();
}

DistanceUnit CFilletPoint3D::GetDistInObj() const
{
	if(m_idx==0)
	{
		return m_filletTx.GetFilletPt1Dist();
	}
	else
	{
		return m_filletTx.GetFilletPt2Dist();
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
	
void CFilletTaxiway3D::RenderLine( C3DView * pView,bool bEdit )
{
	if(!m_filetTaxway.IsActive())
		return;
	
	DistanceUnit slwidth = 50;	

	ShapeGenerator::WidthPipePath widthpath;	

	if(bEdit)
		GenSmoothPath();

	if(m_vPath.size()<2)
		return;

	SHAPEGEN.GenWidthPipePath(m_vPath,slwidth,widthpath);

	glNormal3f(0,0,1);
	glColor3ub(255,255,0);
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<(int)widthpath.leftpath.size();i++){
		glVertex3dv(widthpath.leftpath[i]);
		glVertex3dv(widthpath.rightpath[i]);
	}	
	glEnd();

}

CFilletTaxiway3D::CFilletTaxiway3D( FilletTaxiway& filetTaxiway ) : m_filetTaxway(filetTaxiway)
{
	m_filetPt1 = new CFilletPoint3D(m_filetTaxway,0);
	m_filetPt2 = new CFilletPoint3D(m_filetTaxway,1);
	m_bInEditMode = false;
	GenSmoothPath();
}

ARCPoint3 CFilletTaxiway3D::GetLocation() const
{
	FilletTaxiway _filet = m_filetTaxway;
	return (m_filetPt1->GetLocation() + m_filetPt2->GetLocation() + ARCPoint3(_filet.GetIntersectNode().GetPosition()) ) /3.0;
}

void CFilletTaxiway3D::RenderSurface(  C3DView * pView ,bool bEdit)
{
	if(!m_filetTaxway.IsActive())
		return;

	if(bEdit){
		GenSmoothPath();
	}

	if(m_vPath.size()<2)
		return ;

	ShapeGenerator::WidthPipePath widthpath;

	SHAPEGEN.GenWidthPipePath(m_vPath,m_filetTaxway.GetWidth()*1.05,widthpath);

	double dLenside1 =0; double dLenside2 = 0;
	ARCVector3 prePoint1 = widthpath.leftpath[0];
	ARCVector3 prePoint2 = widthpath.rightpath[0];
	
	int npippathcnt = (int)widthpath.leftpath.size();
	double texcord1 = 0;
	double texcord2 = 0;
	glBegin(GL_QUADS);	

	for(int i =1; i < npippathcnt;++i){	

		glTexCoord2d(0,texcord1);
		glVertex3dv(prePoint1);
		glTexCoord2d(1,texcord2);
		glVertex3dv(prePoint2);


		double dLen1 = prePoint1.DistanceTo(widthpath.leftpath[i]);
		double dLen2 = prePoint2.DistanceTo(widthpath.rightpath[i]);

		texcord1 += dLen1 * 0.0002;
		texcord2 += dLen2 * 0.0002;		

		glTexCoord2d(1,texcord2);
		glVertex3dv(widthpath.rightpath[i]);	
		glTexCoord2d(0,texcord1);
		glVertex3dv(widthpath.leftpath[i]);	

		prePoint1 = widthpath.leftpath[i];
		prePoint2 = widthpath.rightpath[i];

		std::swap(texcord1,texcord2);
	}
	glEnd();	

}

Selectable::SelectType CFilletTaxiway3D::GetSelectType() const
{
	return FILLET_TAXIWAY;
}

void CFilletTaxiway3D::RenderEditPoint( C3DView * pView, bool bSelect )
{
	{		
		if(bSelect)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( m_filetPt1.get() ) );
		}
		m_filetPt1->DrawOGL(pView);
		if(bSelect)
		{
			glLoadName( pView->GetSelectionMap().NewSelectable( m_filetPt2.get() ) );
		}
		m_filetPt2->DrawOGL(pView);
	}
}

void CFilletTaxiway3D::DrawSelect( C3DView *pView )
{
	if(!m_filetTaxway.IsActive())
		return;

	glLoadName(pView->GetSelectionMap().NewSelectable(this) );
	

	ShapeGenerator::WidthPipePath widthpath;

	SHAPEGEN.GenWidthPipePath(m_vPath,m_filetTaxway.GetWidth()*0.2,widthpath);

	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<(int)widthpath.leftpath.size();i++){
		glVertex3dv(widthpath.leftpath[i]);
		glVertex3dv(widthpath.rightpath[i]);
	}	
	glEnd();

}

void CFilletTaxiway3D::GenSmoothPath()
{

	std::vector<CPoint2008> vCtrlPts;
	m_filetTaxway.GetControlPoints(vCtrlPts);	


	const int nCtrlCnt = vCtrlPts.size();
	if(nCtrlCnt < 2 )
		return ;
	
	m_vPath.clear();
	if(nCtrlCnt==2)
	{
		m_vPath.push_back(vCtrlPts[0]);
		m_vPath.push_back(vCtrlPts[1]);
	}
	else
	{
		std::vector<ARCVector3> _inPts;	
		_inPts.reserve(4);
		_inPts.push_back(vCtrlPts[0]);

		DistanceUnit dist = vCtrlPts[0].distance3D( vCtrlPts[nCtrlCnt-1] );
		ARCVector3 vBegin = vCtrlPts[1] - vCtrlPts[0];  
		if(vBegin.Length()>ARCMath::EPSILON)
		{
			vBegin.SetLength(dist*.5);
			_inPts.push_back(vCtrlPts[0] + vBegin);
		}
		ARCVector3 vEnd = vCtrlPts[nCtrlCnt-2] - vCtrlPts[nCtrlCnt-1];
		if(vEnd.Length()>ARCMath::EPSILON)
		{
			vEnd.SetLength(dist*.5);
			_inPts.push_back(vCtrlPts[nCtrlCnt-1] + vEnd);
		}
		_inPts.push_back(vCtrlPts[nCtrlCnt-1] );

		if(_inPts.size()>2)
		{
			SHAPEGEN.GenBezierPath(_inPts,m_vPath,10);
		}
		else
		{
			m_vPath.push_back(vCtrlPts[0]);
			m_vPath.push_back(vCtrlPts[nCtrlCnt-1]);
		}
		//ARCVector3 fromExt = m_filetTaxway.GetFilletPoint1Ext();
		//ARCVector3 toExt = m_filetTaxway.GetFilletPoint2Ext();
		//m_vPath.insert(m_vPath.begin(),fromExt);
		//m_vPath.push_back(toExt);
	}

}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void CFilletTaxiway3DInAirport::UpdateFillets( const IntersectionNodesInAirport& nodelist,const std::vector<int>& vChangeNodeIndexs )
{
	m_vFillTaxiway3D.clear();

	m_vFilletTaxiways.UpdateFillets( m_nARPID, nodelist, vChangeNodeIndexs);

	for(int i=0;i<m_vFilletTaxiways.GetCount();i++)
	{
		if( m_vFilletTaxiways.ItemAt(i).IsActive())
		{			
			CFilletTaxiway3D * pNewFillet3D = new CFilletTaxiway3D(m_vFilletTaxiways.ItemAt(i));
			m_vFillTaxiway3D.push_back(pNewFillet3D);
		}
	}
}


void CFilletTaxiway3DInAirport::RenderSurface( C3DView * pView )
{
	CTexture * pTexture = pView->getTextureResource()->getTexture("Taxiway");
	if(pTexture) pTexture->Apply();

	glColor3ub(255,255,255);
	glNormal3i(0,0,1);

	for(int i=0;i< (int)m_vFillTaxiway3D.size();i++)
	{
		CFilletTaxiway3D * pfillet3D = m_vFillTaxiway3D.at(i).get();
		
		pfillet3D->RenderSurface(pView, IsInEdit() );
	}
}

void CFilletTaxiway3DInAirport::RenderLine( C3DView * pView, bool bSelect )
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	for(int i=0;i< (int)m_vFillTaxiway3D.size();i++)
	{
		CFilletTaxiway3D * pfillet3D = m_vFillTaxiway3D.at(i).get();
		pfillet3D->RenderLine(pView, IsInEdit());
	}

	if(IsInEdit())
	{
		CFilletTaxiway3D * pfillet3D = GetInEditFillet();
		if(pfillet3D)
			pfillet3D->RenderEditPoint(pView,bSelect);
	}

}

void CFilletTaxiway3DInAirport::DrawSelect( C3DView * pView )
{
	if(IsInEdit())
	{
		CFilletTaxiway3D * pfillet3D = GetInEditFillet();
		if(pfillet3D)
			pfillet3D->RenderEditPoint(pView,TRUE);
	}
	for(int i=0;i< (int)m_vFillTaxiway3D.size();i++)
	{
		CFilletTaxiway3D * pfillet3D = m_vFillTaxiway3D.at(i).get();
		pfillet3D->DrawSelect(pView);
	}

}

CFilletTaxiway3D* CFilletTaxiway3DInAirport::FindFillet3D( const FilletTaxiway& filet )
{
	for(int i=0;i< (int)m_vFillTaxiway3D.size();i++)
	{
		CFilletTaxiway3D * pfillet3D = m_vFillTaxiway3D.at(i).get();
		if(pfillet3D->m_filetTaxway.IsOverLapWith(filet))
		{
			return pfillet3D;
		}
	}
	return NULL;
}

void CFilletTaxiway3DInAirport::Init( int nARPID, const IntersectionNodesInAirport& nodeList )
{
	m_nARPID = nARPID;
	m_vFilletTaxiways.ReadData(nARPID,nodeList);
	m_bInEdit = false;
	m_iInEditFillet = -1;

	m_vFillTaxiway3D.clear();
	for(int i=0;i<m_vFilletTaxiways.GetCount();i++)
	{
		if( m_vFilletTaxiways.ItemAt(i).IsActive())
		{			
			CFilletTaxiway3D * pNewFillet3D = new CFilletTaxiway3D(m_vFilletTaxiways.ItemAt(i));
			m_vFillTaxiway3D.push_back(pNewFillet3D);
		}
	}
}

CFilletTaxiway3D * CFilletTaxiway3DInAirport::GetInEditFillet()
{
	for(int i =0 ;i< (int)m_vFillTaxiway3D.size();i++)
	{
		CFilletTaxiway3D * pfillet3D = m_vFillTaxiway3D.at(i).get();
		if(pfillet3D->m_filetTaxway.GetID() == m_iInEditFillet)
		{
			return pfillet3D;
		}
	}
	return NULL;
}