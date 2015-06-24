#include "StdAfx.h"
#include ".\startposition3d.h"
#include "../InputAirside/StartPosition.h"
#include ".\3dView.h"
#include ".\Airside3D.h"
#include <CommonData/3DTextManager.h>
#include "Airport3D.h"
CStartPosition3D::CStartPosition3D(int _Id)
: ALTObject3D(_Id)
{
	m_pObj = new CStartPosition;
	//m_pDisplayProp = new ALTObjectDisplayProp();
}

CStartPosition3D::~CStartPosition3D(void)
{
}

static void drawCirlePipe(double dRadius, double width)
{
	ARCVector2 pOutStart, pInStart;
	ARCVector2 pOut, pIn;
	
	pOutStart = ARCVector2(1,0)*dRadius;
	pInStart = ARCVector2(1,0)*(dRadius-width);

	glNormal3d(0,0,1);
	
	const int nDividCnt = 50;
	
	glBegin(GL_QUAD_STRIP);
		glVertex2d(pInStart[VX],pInStart[VY]);
		glVertex2d(pOutStart[VX],pOutStart[VY]);
		for(int i=1;i<nDividCnt;i++)
		{
			double drad = ARCMath::DegreesToRadians(i*360/nDividCnt);
			ARCVector2 vdir =  ARCVector2(cos(drad),sin(drad));
			pOut = vdir * dRadius;
			pIn = vdir * (dRadius-width);
			glVertex2d(pIn[VX],pIn[VY]);
			glVertex2d(pOut[VX],pOut[VY]);
		}
		glVertex2d(pInStart[VX],pInStart[VY]);
		glVertex2d(pOutStart[VX],pOutStart[VY]);
	glEnd();
}

void CStartPosition3D::DrawOGL(C3DView * pView)
{
	//find the relate taxiway
	CStartPosition* pStartPos = getStartPos();
	if(pStartPos)
	{
		int nTaxiID = pStartPos->GetTaxiwayID();
		ALTObject3D* pObj3D = pView->GetAirside3D()->GetActiveAirport()->GetObject3D(nTaxiID);
		if(pObj3D && pObj3D->GetObjectType() == ALT_TAXIWAY)
		{
			CTaxiway3D* pTaxiway3D = (CTaxiway3D*)pObj3D;
			mpRelateTaxiway = pTaxiway3D;
			if(mpRelateTaxiway.get() && mpRelateTaxiway->GetTaxiway() )
			{

				DistanceUnit dWidth = pStartPos->GetRadius();
				ARCPoint3 pos = GetLocation();

				glColor3f(1,1,1);
				glEnable(GL_LIGHTING);	
				glDisable(GL_TEXTURE_2D);
				glPushMatrix();
				glTranslated(pos[VX],pos[VY],0);
				drawCirlePipe(dWidth,100);
				glPopMatrix();

				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				ARCPoint3 markPos; double dRotAngle;double dscale;
				pStartPos->getMarkingPos(mpRelateTaxiway->GetTaxiway()->GetPath(), markPos,dRotAngle,dscale);

				TEXTMANAGER3D->DrawOutlineText( pStartPos->GetMarking().c_str(),markPos,(float)dRotAngle,(float)dscale); 	
				glEnable(GL_LIGHTING);
				glDisable(GL_CULL_FACE);
			}

		}
	}
	//ALTObject3D::DrawOGL(m_pView);
}


void CStartPosition3D::AddEditPoint(double x, double y, double z)
{
	//ALTObject3D::AddEditPoint(x, y, z);
}

Selectable::SelectType CStartPosition3D::GetSelectType() const
{
	return ALTObject3D::GetSelectType();
	//return ALT_STARTPOSITION;
}
void CStartPosition3D::OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz/* = 0*/)
{
	if (m_pObj->GetLocked())
	{
		return;
	}

	CStartPosition *pStartPos = getStartPos();
	if(mpRelateTaxiway.get() && pStartPos)
	{
		DistanceUnit relateDist = pStartPos->GetTaxiwayDivision();
		const CPath2008& taxiPath = mpRelateTaxiway->GetTaxiway()->GetPath();
		ARCVector3 vDir = taxiPath.GetIndexDir((float)relateDist);
		double d= vDir.dot(ARCVector3(dx,dy,dz)/vDir.Length());
		double realdist = taxiPath.GetIndexDist( (float)relateDist);
		realdist += d;
		pStartPos->SetTaxiwayDivision( taxiPath.GetDistIndex(realdist) );

	}	

}
void CStartPosition3D::OnRotate(DistanceUnit dx)
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	//ALTObject3D::OnRotate(dx);
}
void CStartPosition3D::AfterMove()
{
	ALTObject3D::AfterMove();
}
ARCPoint3 CStartPosition3D::GetLocation() const
{
	//return ALTObject3D::GetLocation();
	CStartPosition* pStartPos = getStartPos();
	if(mpRelateTaxiway.get() && pStartPos)
	{
		const CPath2008& path = mpRelateTaxiway->GetTaxiway()->GetPath();
		DistanceUnit distInPath = pStartPos->GetTaxiwayDivision();
		CPoint2008 pt = path.GetIndexPoint((float)distInPath);
		return ARCPoint3(pt);
	}
	
	return ARCPoint3(0, 0, 0);
}
ARCPoint3 CStartPosition3D::GetSubLocation(int index) const
{
	//return ALTObject3D::GetSubLocation(index);
	return ARCPoint3(0,0,0);
}
// write data to database
void CStartPosition3D::FlushChange()
{
	ALTObject3D::FlushChange();
}
// update data from database
void CStartPosition3D::Update()
{
	ALTObject3D::Update();
}
// sync geo date for drawing
void CStartPosition3D::DoSync()
{
	ALTObject3D::DoSync();
}
CString CStartPosition3D::GetDisplayName() const
{
	return ALTObject3D::GetDisplayName();
}

CStartPosition* CStartPosition3D::getStartPos() const
{
	if(GetObject() && GetObject()->GetType() == ALT_STARTPOSITION)
	{
		return (CStartPosition*)GetObject();
	}
	return NULL;
}

void CStartPosition3D::DrawSelect( C3DView * pView )
{
	CStartPosition* pStartPos = getStartPos();
	if(pStartPos)
	{
		int nTaxiID = pStartPos->GetTaxiwayID();
		ALTObject3D* pObj3D = pView->GetAirside3D()->GetActiveAirport()->GetObject3D(nTaxiID);
		if(pObj3D && pObj3D->GetObjectType() == ALT_TAXIWAY)
		{
			CTaxiway3D* pTaxiway3D = (CTaxiway3D*)pObj3D;
			mpRelateTaxiway = pTaxiway3D;

			DistanceUnit dWidth = pStartPos->GetRadius();
			ARCPoint3 pos = GetLocation();

			glPushMatrix();
			glTranslated(pos[VX],pos[VY],0);
			drawCirlePipe(dWidth,dWidth);
			glPopMatrix();
		}
	}
}