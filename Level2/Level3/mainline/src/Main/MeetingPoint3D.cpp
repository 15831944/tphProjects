#include "StdAfx.h"
#include ".\MeetingPoint3D.h"
#include "../InputAirside/MeetingPoint.h"
#include ".\3dView.h"
#include ".\Airside3D.h"
#include <CommonData/3DTextManager.h>
#include "Airport3D.h"


CMeetingPoint3D::CMeetingPoint3D(int _Id)
: ALTObject3D(_Id)
{
	m_pObj = new CMeetingPoint;
	//m_pDisplayProp = new ALTObjectDisplayProp();
}

CMeetingPoint3D::~CMeetingPoint3D(void)
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

void CMeetingPoint3D::DrawOGL(C3DView * pView)
{
	//find the relate taxiway
	CMeetingPoint* pMeetingPoint = getMeetingPoint();
	if(pMeetingPoint)
	{
		int nTaxiID = pMeetingPoint->GetTaxiwayID();
		ALTObject3D* pObj3D = pView->GetAirside3D()->GetActiveAirport()->GetObject3D(nTaxiID);
		if(pObj3D && pObj3D->GetObjectType() == ALT_TAXIWAY)
		{
			CTaxiway3D* pTaxiway3D = (CTaxiway3D*)pObj3D;
			m_pRelateTaxiway = pTaxiway3D;
			if(m_pRelateTaxiway.get() && m_pRelateTaxiway->GetTaxiway() )
			{

				DistanceUnit dWidth = pMeetingPoint->GetRadius();
				ARCPoint3 pos = GetLocation();

				glColor3f(1.0f,0.2f,0.1f);
				glEnable(GL_LIGHTING);	
				glDisable(GL_TEXTURE_2D);
				glPushMatrix();
				glTranslated(pos[VX],pos[VY],0);
				drawCirlePipe(dWidth,100);
				glPopMatrix();

				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				ARCPoint3 markPos; double dRotAngle;double dscale;
				pMeetingPoint->getMarkingPos(m_pRelateTaxiway->GetTaxiway()->GetPath(), markPos,dRotAngle,dscale);

				TEXTMANAGER3D->DrawOutlineText( pMeetingPoint->GetMarking().c_str(),markPos,(float)dRotAngle,(float)dscale); 	
				glEnable(GL_LIGHTING);
				glDisable(GL_CULL_FACE);
			}

		}
		if(CPath2008* pwaitArea = pMeetingPoint->GetPath())
		{
			glColor3f(1.0f,0.2f,0.1f);
			glBegin(GL_LINE_LOOP);
			for(int i=0;i<pwaitArea->getCount();i++)
			{
				glVertex2dv(pwaitArea->getPoint(i).c_v());
			}
			glEnd();
		}

	}
	//ALTObject3D::DrawOGL(m_pView);
}


void CMeetingPoint3D::AddEditPoint(double x, double y, double z)
{
	//ALTObject3D::AddEditPoint(x, y, z);
}

Selectable::SelectType CMeetingPoint3D::GetSelectType() const
{
	return ALTObject3D::GetSelectType();
}
void CMeetingPoint3D::OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz/* = 0*/)
{
	if (m_pObj->GetLocked())
	{
		return;
	}

	CMeetingPoint *pMeetingPoint = getMeetingPoint();
	if(m_pRelateTaxiway.get() && pMeetingPoint)
	{
		DistanceUnit relateDist = pMeetingPoint->GetTaxiwayDivision();
		const CPath2008& taxiPath = m_pRelateTaxiway->GetTaxiway()->GetPath();
		ARCVector3 vDir = taxiPath.GetIndexDir((float)relateDist);
		double d= vDir.dot(ARCVector3(dx,dy,dz)/vDir.Length());
		double realdist = taxiPath.GetIndexDist( (float)relateDist);
		realdist += d;
		pMeetingPoint->SetTaxiwayDivision( taxiPath.GetDistIndex(realdist) );
	}	

}
void CMeetingPoint3D::OnRotate(DistanceUnit dx)
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	//ALTObject3D::OnRotate(dx);
}
void CMeetingPoint3D::AfterMove()
{
	ALTObject3D::AfterMove();
}
ARCPoint3 CMeetingPoint3D::GetLocation() const
{
	//return ALTObject3D::GetLocation();
	CMeetingPoint* pMeetingPoint = getMeetingPoint();
	if(m_pRelateTaxiway.get() && pMeetingPoint)
	{
		const CPath2008& path = m_pRelateTaxiway->GetTaxiway()->GetPath();
		DistanceUnit distInPath = pMeetingPoint->GetTaxiwayDivision();
		CPoint2008 pt = path.GetIndexPoint((float)distInPath);
		return ARCPoint3(pt);
	}
	
	return ARCPoint3(0, 0, 0);
}
ARCPoint3 CMeetingPoint3D::GetSubLocation(int index) const
{
	//return ALTObject3D::GetSubLocation(index);
	return ARCPoint3(0,0,0);
}
// write data to database
void CMeetingPoint3D::FlushChange()
{
	ALTObject3D::FlushChange();
}
// update data from database
void CMeetingPoint3D::Update()
{
	ALTObject3D::Update();
}
// sync geo date for drawing
void CMeetingPoint3D::DoSync()
{
	ALTObject3D::DoSync();
}
CString CMeetingPoint3D::GetDisplayName() const
{
	return ALTObject3D::GetDisplayName();
}

CMeetingPoint* CMeetingPoint3D::getMeetingPoint() const
{
	if(GetObject() && GetObject()->GetType() == ALT_MEETINGPOINT)
	{
		return (CMeetingPoint*)GetObject();
	}
	return NULL;
}

void CMeetingPoint3D::DrawSelect( C3DView * pView )
{
	CMeetingPoint* pStartPos = getMeetingPoint();
	if(pStartPos)
	{
		int nTaxiID = pStartPos->GetTaxiwayID();
		ALTObject3D* pObj3D = pView->GetAirside3D()->GetActiveAirport()->GetObject3D(nTaxiID);
		if(pObj3D && pObj3D->GetObjectType() == ALT_TAXIWAY)
		{
			CTaxiway3D* pTaxiway3D = (CTaxiway3D*)pObj3D;
			m_pRelateTaxiway = pTaxiway3D;

			DistanceUnit dWidth = pStartPos->GetRadius();
			ARCPoint3 pos = GetLocation();

			glPushMatrix();
			glTranslated(pos[VX],pos[VY],0);
			drawCirlePipe(dWidth,dWidth);
			glPopMatrix();
		}
	}
}