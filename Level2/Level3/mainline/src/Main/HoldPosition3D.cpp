#include "StdAfx.h"

#include "../InputAirside/HoldPosition.h"
#include "HoldPosition3D.h"
#include ".\3DView.h"
#include "../common/UnitsManager.h"
#include <CommonData/3DTextManager.h>
#include "../Common/ARCPipe.h"
#include "glrender/glTextureResource.h"

const static double dHoldPositionLineWidth = 50;
const static double dDashLineWidth = dHoldPositionLineWidth * 4;
const static double dDashLineSpace = dHoldPositionLineWidth * 2;
//const static double dHoldPositionMarkLen = 305;

void HoldPosition3D::Draw( C3DView* pView )
{

	UpdateLine();

	CTexture * pTexture = pView->getTextureResource()->getTexture("HoldPosition");
	if(pTexture)
		pTexture->Apply();

	glColor3f( 1.0f,1.0f,0.0f);
	glNormal3f(0.0f,0.0f,1.0f);
	CPoint2008 p1 = m_line.GetPoint1();
	CPoint2008 p2 = m_line.GetPoint2();

	DistanceUnit dDist = p1.distance3D(p2);
	CPath2008 linePath;
	linePath.init(2);
	
	const HoldPosition& hold = GetHoldPosition();
	if(hold.IsFirst()) 	
	{
		linePath[0] = p2;
		linePath[1] = p1;
	}
	else
	{
		linePath[0] = p1;
		linePath[1] = p2;
	}

	

	double texcordT = dDist * 0.004;
	ARCPipe pipePath(linePath,300);

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

}

void HoldPosition3D::DrawSelect( C3DView * pView )
{
	glLoadName( pView->GetSelectionMap().NewSelectable(this) );
	Draw(pView);
}


void HoldPosition3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ ) /*= 0*/
{
	if (m_pObject->GetLocked())
	{
		return;
	}
	HoldPosition& hold = GetHoldPosition();
	DistanceUnit RealDist = hold.GetPosition();
	double RelateDist = GetPath().GetDistIndex(RealDist);

	int nid = (int)RelateDist; if(nid == GetPath().getCount()-1) nid --;
	ARCVector3 vdir = GetPath().getPoint(nid +1) - GetPath().getPoint(nid);
	vdir[VZ]= 0;
	vdir.Normalize();
	double d = vdir.dot(ARCVector3(dx,dy,dz))/vdir.Length();
	hold.m_distToIntersect += d;
	double pathLen = GetPath().GetTotalLength();

	if(hold.GetPosition() >pathLen ){
		hold.SetPosition(pathLen);
	}
	if(hold.GetPosition()<0){
		hold.SetPosition(0);
	}
	//hold.UpdateData();

}

ARCPoint3 HoldPosition3D::GetLocation() const
{
	return ( m_line.GetPoint1() + m_line.GetPoint2() ) * 0.5;
}


HoldPosition3D::HoldPosition3D(ALTObject* pTaxiway, int id)
{
	m_pObject = pTaxiway;
	m_nid = id; 
}

void HoldPosition3D::FlushChange()
{
	GetHoldPosition().UpdateData();
}

void HoldPosition3D::AfterMove()
{
	FlushChange();
}