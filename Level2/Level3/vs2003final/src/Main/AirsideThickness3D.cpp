#include "StdAfx.h"
#include ".\airsidethickness3d.h"
#include "../Common/ARCPath.h"
#include "Common/Path2008.h"
#include "../Common/ARCVector.h"
#include ".\3DView.h"
#include "../Common/ARCPipe.h"
#include "../Main/TermPlanDoc.h"
#include "glrender/glTextureResource.h"
CAirsideThickness3D::CAirsideThickness3D(void)
{
	m_dThick = 100;
}

CAirsideThickness3D::~CAirsideThickness3D(void)
{
}

double CAirsideThickness3D::GetThickness()
{
	return m_dThick;
}

CPath2008 CAirsideThickness3D::ModifyPathAlt(CPath2008& path,C3DView* pView)
{
	int elementCount = path.getCount();
	DistanceUnit floatPoint = 100.0;
	CPoint2008 *vPoint = path.getPointList();
	CPoint2008 *m_vPoint = new CPoint2008[elementCount]; 
	for (int i=0; i<elementCount; i++)
	{
		CPoint2008& tempPoint = vPoint[i];
		m_vPoint[i].setPoint(tempPoint.getX(),tempPoint.getY(),tempPoint.getZ()*pView->GetDocument()->m_iScale);
	}
	CPath2008 m_Path;
	m_Path.init(elementCount,m_vPoint);
	delete [] m_vPoint;
	return m_Path;
}

ARCPath3 CAirsideThickness3D::ModifyPathAlt(ARCPath3& path,C3DView* pView)
{
	std::vector<ARCVector3> vVector3;
	CPath2008 temPath = path.getPath();
	int elementCount = temPath.getCount();
	DistanceUnit floatPoint = 100.0;
	CPoint2008 *vPoint = temPath.getPointList();
	CPoint2008 *m_vPoint = new CPoint2008[elementCount]; 
	for (int i=0; i<elementCount; i++)
	{
		if(i<=(elementCount/2))
		{
			CPoint2008& tempPoint = vPoint[i];
			m_vPoint[i].setPoint(tempPoint.getX(),tempPoint.getY(),tempPoint.getZ()*pView->GetDocument()->m_iScale);
		}
		else
		{
			CPoint2008& tempPoint = vPoint[i];
			m_vPoint[i].setPoint(tempPoint.getX(),tempPoint.getY(),tempPoint.getZ()*pView->GetDocument()->m_iScale);
		}

		vVector3.push_back(ARCVector3(m_vPoint[i]));
	}
	delete [] m_vPoint;
	return ARCPath3(vVector3);
}

CPath2008 CAirsideThickness3D::ReviewPath(const CPath2008& path)
{
	int elementCount = path.getCount();
	DistanceUnit floatPoint = 100.0;
	const CPoint2008 *vPoint = path.getPointList();
	CPoint2008 *m_vPoint = new CPoint2008[elementCount]; 
	for (int i=0; i<elementCount; i++)
	{
		if(i<=(elementCount/2)&&i>(elementCount/3))
		{
			const CPoint2008& tempPoint = vPoint[i];
			m_vPoint[i].setPoint(tempPoint.getX(),tempPoint.getY(),tempPoint.getZ()-floatPoint*i);
		}
		else if(i>(elementCount/2)&&i<(elementCount*2/3))
		{
			const CPoint2008& tempPoint = vPoint[i];
			m_vPoint[i].setPoint(tempPoint.getX(),tempPoint.getY(),tempPoint.getZ()-floatPoint*(elementCount-i));
		}
		else
		{
			const CPoint2008& tempPoint = vPoint[i];
			m_vPoint[i].setPoint(tempPoint.getX(),tempPoint.getY(),tempPoint.getZ());
		}

	}
	CPath2008 m_Path;
	m_Path.init(elementCount,m_vPoint);
	delete [] m_vPoint;
	return m_Path;
}

void CAirsideThickness3D::AddThickness(const CPath2008& path,CPath2008& newPath)
{
	int elementCount = path.getCount();
	DistanceUnit floatPoint = 100.0;
	const CPoint2008 *vPoint = path.getPointList();
	CPoint2008 *m_vPoint = new CPoint2008[elementCount]; 
	for (int i=0; i<elementCount; i++)
	{
		m_vPoint[i].setPoint(vPoint[i].getX(),vPoint[i].getY(),vPoint[i].getZ()-m_dThick);
	}
	newPath.init(elementCount,m_vPoint);
	delete [] m_vPoint;
}

ARCPath3 CAirsideThickness3D::AddThickness(ARCPath3& path)
{
	std::vector<ARCVector3> vVector3;
	CPath2008 temPath = path.getPath();
	int elementCount = temPath.getCount();
	DistanceUnit floatPoint = 100.0;
	CPoint2008 *vPoint = temPath.getPointList();
	CPoint2008 *m_vPoint = new CPoint2008[elementCount]; 
	for (int i=0; i<elementCount; i++)
	{
		CPoint2008& tempPoint = vPoint[i];
		m_vPoint[i].setPoint(tempPoint.getX(),tempPoint.getY(),tempPoint.getZ()-m_dThick);
		vVector3.push_back(ARCVector3(m_vPoint[i]));
	}
	delete [] m_vPoint;
	return ARCPath3(vVector3);
}

ARCPath3 CAirsideThickness3D::AddWallAlt(ARCPath3& path)
{
	std::vector<ARCVector3> vVector3;
	CPath2008 temPath = path.getPath();
	int elementCount = temPath.getCount();
	DistanceUnit floatPoint = 100.0;
	CPoint2008 *vPoint = temPath.getPointList();
	CPoint2008 *m_vPoint = new CPoint2008[elementCount]; 
	for (int i=0; i<elementCount; i++)
	{
		CPoint2008& tempPoint = vPoint[i];
		{
			m_vPoint[i].setPoint(tempPoint.getX(),tempPoint.getY(),0.0);
		}

		vVector3.push_back(ARCVector3(m_vPoint[i]));
	}
	delete [] m_vPoint;
	return ARCPath3(vVector3);
}

void CAirsideThickness3D::RenderSidePath(C3DView* pView, const ARCPipe& abovePipe, const ARCPipe& belowPipe)
{
	CTexture * pTexture = pView->getTextureResource()->getTexture("road");

	if(pTexture)
		pTexture->Apply();
	glColor3ub(255,255,255);
	RenderSubSidePath(abovePipe.m_sidePath1,belowPipe.m_sidePath1,TRUE);
	RenderSubSidePath(abovePipe.m_sidePath2,belowPipe.m_sidePath2,FALSE);
	RenderHeadTrailPath(abovePipe, belowPipe);
}

void CAirsideThickness3D::RenderSidePath(const ARCPath3& aboveLeft, const ARCPath3& aboveRight, const ARCPath3& belowLeft, const ARCPath3& belowRight)
{
//	glEnable(GL_LIGHTING);

//	glDisable(GL_CULL_FACE);
	int lCount = aboveLeft.size();
	int rCount = aboveRight.size();
	DistanceUnit disL = aboveLeft[0].DistanceTo(aboveLeft[lCount-1]);
	DistanceUnit disR = aboveRight[0].DistanceTo(aboveRight[rCount-1]);
	if (disL >= disR)
	{
		RenderSubSidePath(aboveRight,belowRight,FALSE);
	}
	else
	{
		RenderSubSidePath(aboveLeft,belowLeft,TRUE);
	}
//	glEnable(GL_CULL_FACE);
//	glDisable(GL_LIGHTING);
}
void CAirsideThickness3D::RenderSubSidePath(const ARCPath3& abovePath, const ARCPath3& belowPath,BOOL bFlag)
{   

	int elementCount = (int)abovePath.size();
	glBegin(GL_QUADS);
	ARCVector3 prePointL = abovePath[0];
	ARCVector3 prePointR = belowPath[0];

	double totaLength=0.0;
	for (int i=0;i<elementCount-1;i++)
	{
		totaLength += abovePath[i].DistanceTo(abovePath[i+1]);
	}
	double dTexCoordLeft = 0.0;
	double dTexCoordRight = 0.0;
	ARCVector3 vNormalize(0.0,0.0,0.0);
	for (int i=1;i<elementCount;i++)
	{
		{
			CalculateNormalize(abovePath,belowPath,vNormalize,i,bFlag);
			glNormal3d(vNormalize[VX],vNormalize[VY],vNormalize[VZ]);
			glTexCoord2d(0,dTexCoordLeft);
			glVertex3dv(prePointL);
			glTexCoord2d(1,dTexCoordRight);
			glVertex3dv(prePointR);

			DistanceUnit dLen = prePointL.DistanceTo(abovePath[i]);	
			dTexCoordLeft += dLen/totaLength;// * 0.002;
			dLen = prePointR.DistanceTo(belowPath[i]);
			dTexCoordRight += dLen/totaLength;// * 0.002;		

			glTexCoord2d(1,dTexCoordRight);
			glVertex3dv(belowPath[i]);
			glTexCoord2d(0,dTexCoordLeft);
			glVertex3dv(abovePath[i]);
		}
 		std::swap(dTexCoordLeft,dTexCoordRight);
		prePointL = abovePath[i];
		prePointR = belowPath[i];


	}
	glEnd();	
}

void CAirsideThickness3D::RenderHeadTrailPath(const ARCPipe& abovePipe, const ARCPipe& belowPipe)
{
	int nLastCount = (int)abovePipe.m_centerPath.size()-1;
	//Head
	{
		ARCVector3 vNormalize1(0.0,0.0,0.0); 
		ARCVector3 vTemp1 = abovePipe.m_sidePath1[0] - abovePipe.m_sidePath2[0];
		ARCVector3 vTemp2 = belowPipe.m_sidePath2[0] - abovePipe.m_sidePath1[0];
		vNormalize1 = cross(vTemp1.Normalize(),vTemp2.Normalize()).Normalize();
		glNormal3d(vNormalize1[VX],vNormalize1[VY],vNormalize1[VZ]);
		glBegin(GL_QUADS);
		glTexCoord2d(0.0,0.0);
		glVertex3dv(abovePipe.m_sidePath1[0]);
		glTexCoord2d(1.0,0.0);
		glVertex3dv(belowPipe.m_sidePath1[0]);
		glTexCoord2d(1.0,1.0);
		glVertex3dv(belowPipe.m_sidePath2[0]);
		glTexCoord2d(0.0,1.0);
		glVertex3dv(abovePipe.m_sidePath2[0]);
		glEnd();
	}


	//Trail
	{
		ARCVector3 vNormalize2(0.0,0.0,0.0); 
		ARCVector3 vTemp1 = abovePipe.m_sidePath1[nLastCount] - abovePipe.m_sidePath2[nLastCount];
		ARCVector3 vTemp2 = belowPipe.m_sidePath2[nLastCount] - abovePipe.m_sidePath1[nLastCount];
		vNormalize2 = cross(vTemp2.Normalize(),vTemp1.Normalize()).Normalize();
		glNormal3d(vNormalize2[VX],vNormalize2[VY],vNormalize2[VZ]);
		glBegin(GL_QUADS);
		glTexCoord2d(0.0,0.0);
		glVertex3dv(abovePipe.m_sidePath1[nLastCount]);
		glTexCoord2d(1.0,0.0);
		glVertex3dv(belowPipe.m_sidePath1[nLastCount]);
		glTexCoord2d(1.0,1.0);
		glVertex3dv(belowPipe.m_sidePath2[nLastCount]);
		glTexCoord2d(0.0,1.0);
		glVertex3dv(abovePipe.m_sidePath2[nLastCount]);
		glEnd();
	}
}

void CAirsideThickness3D::CalculateNormalize(const ARCPath3& abovePath, const ARCPath3& belowPath,ARCVector3& vNormal,int index,BOOL bFlag)
{
	if (index<1)
		return;
	ARCVector3 vTemp1(0.0,0.0,0.0);
	ARCVector3 vTemp2(0.0,0.0,0.0);
	if (abovePath.size()==index)
	{
		vTemp1 = abovePath[0] - abovePath[index];
		vTemp2 = belowPath[0] - abovePath[0];
	}
	else
	{
		vTemp1 = abovePath[index] - abovePath[index-1];
		vTemp2 = belowPath[index] - abovePath[index];
	}

	if (bFlag==TRUE)
	{
		vNormal = cross(vTemp1.Normalize(),vTemp2.Normalize()).Normalize();
	}
	else
	{
		vNormal = cross(vTemp2.Normalize(),vTemp1.Normalize()).Normalize();
	}
}
void CAirsideThickness3D::RenderPathWall(C3DView* pView, const ARCPipe& abovePipe)
{

}
void CAirsideThickness3D::RenderSubPathWall(const ARCPath3& subPath)
{

}

void CAirsideThickness3D::SetWallPath(const CPath2008& path, CPath2008& bottomPath, CPath2008& toPath)
{
	int elementCount = path.getCount();
	DistanceUnit floatPoint = 100.0;
	const CPoint2008* vPoint = path.getPointList();
	CPoint2008 m_vPoint; 
	std::vector<CPoint2008> boPoint,toPoint;
	for (int i=0; i<elementCount; i++)
	{
		if (vPoint[i].getZ()!=0.0)
		{
			m_vPoint.setPoint(vPoint[i].getX(),vPoint[i].getY(),vPoint[i].getZ()-m_dThick);
			boPoint.push_back(m_vPoint);
			m_vPoint.setPoint(vPoint[i].getX(),vPoint[i].getY(),0.0);
			toPoint.push_back(m_vPoint);
		}
	}
	int boCount = boPoint.size();
	int toCount = toPoint.size();

	ASSERT(boCount==toCount);

	bottomPath.init(boCount,&(*boPoint.begin()));
	toPath.init(toCount,&(*toPoint.begin()));
}

void CAirsideThickness3D::GetHorizonPoint(const ARCVector3& vVector0, const ARCVector3& vVector1, ARCVector3& vVector)
{
	double Delta = vVector1[VZ]/(vVector1[VZ]-vVector0[VZ]);
	vVector = vVector0 * Delta + vVector1*(1-Delta);
}

void CAirsideThickness3D::RenderWall(C3DView* pView,const CPath2008& path,double pathWidth)
{

	CTexture * pTexture = pView->getTextureResource()->getTexture("stretch");

	if(pTexture)
		pTexture->Apply();
	glColor3ub(255,255,255);
	glFrontFace(GL_CCW);
	ARCPipe pipePath(path,pathWidth);
	double dTexCoordLeft=0.0,dTexCoordRight=0.0;
	for(int i=0; i<(int)pipePath.m_centerPath.size()-1;i++)
	{					
		ARCVector3 vPrePoint1(pipePath.m_sidePath1[i][VX],pipePath.m_sidePath1[i][VY],0.0);
    	ARCVector3 vAftPoint1(pipePath.m_sidePath1[i+1][VX],pipePath.m_sidePath1[i+1][VY],0.0);
	    ARCVector3 vPrePoint2(pipePath.m_sidePath2[i][VX],pipePath.m_sidePath2[i][VY],0.0);
    	ARCVector3 vAftPoint2(pipePath.m_sidePath2[i+1][VX],pipePath.m_sidePath2[i+1][VY],0.0);
		// side1
		glEnable(GL_CULL_FACE);
   		glCullFace(GL_BACK);

		{

			ARCVector3 vNormal(0.0,0.0,0.0);

			if (pipePath.m_sidePath1[i][VZ]<0.0&&pipePath.m_sidePath1[i+1][VZ]<0.0)
	   		{
	        	glBegin(GL_QUADS);
				vNormal = cross((pipePath.m_sidePath1[i]-vPrePoint1).Normalize(),(vAftPoint1-vPrePoint1).Normalize()).Normalize();
				glNormal3d(vNormal[VX],vNormal[VY],vNormal[VZ]);
				glTexCoord2d(0,dTexCoordLeft);
				glVertex3d(vPrePoint1[VX],vPrePoint1[VY],vPrePoint1[VZ]);
				glTexCoord2d(1,dTexCoordRight);
				glVertex3d(pipePath.m_sidePath1[i][VX],pipePath.m_sidePath1[i][VY],pipePath.m_sidePath1[i][VZ]);

				DistanceUnit dLen = vPrePoint1.DistanceTo(vAftPoint1);
				dTexCoordLeft += dLen*0.002;
				dLen = pipePath.m_sidePath1[i].DistanceTo(pipePath.m_sidePath1[i+1]);
				dTexCoordRight += dLen*0.002;

				glTexCoord2d(1,dTexCoordRight);
				glVertex3d(pipePath.m_sidePath1[i+1][VX],pipePath.m_sidePath1[i+1][VY],pipePath.m_sidePath1[i+1][VZ]);
				glTexCoord2d(0,dTexCoordLeft);
				glVertex3d(vAftPoint1[VX],vAftPoint1[VY],vAftPoint1[VZ]);
				glEnd();
			}
			else if (pipePath.m_sidePath1[i][VZ]<0.0&&pipePath.m_sidePath1[i+1][VZ]>=0.0)//||(pipePath.m_sidePath1[i][VZ]>0.0&&pipePath.m_sidePath2[i+1][VZ]<0.0))
			{
				glBegin(GL_TRIANGLES);
				vNormal = cross((pipePath.m_sidePath1[i]-vPrePoint1).Normalize(),(vAftPoint1-vPrePoint1).Normalize()).Normalize();
				glNormal3d(vNormal[VX],vNormal[VY],vNormal[VZ]);
				ARCVector3 vVector(0.0,0.0,0.0);
				GetHorizonPoint(pipePath.m_sidePath1[i],pipePath.m_sidePath1[i+1],vVector);
				glTexCoord2d(0,/*dTexCoordLeft*/0);
				glVertex3d(vVector[VX],vVector[VY],vVector[VZ]);	
				glTexCoord2d(0,/*dTexCoordLeft*/1);
				glVertex3d(vPrePoint1[VX],vPrePoint1[VY],vPrePoint1[VZ]);
				glTexCoord2d(1,/*dTexCoordRight*/0);
				glVertex3d(pipePath.m_sidePath1[i][VX],pipePath.m_sidePath1[i][VY],pipePath.m_sidePath1[i][VZ]);

				//DistanceUnit dLen = vPrePoint1.DistanceTo(vAftPoint1);
				//dTexCoordLeft += dLen*0.002;
				//dLen = pipePath.m_sidePath1[i].DistanceTo(pipePath.m_sidePath1[i+1]);
				//dTexCoordRight += dLen*0.002;

				//glTexCoord2d(1,dTexCoordRight);
				//glVertex3d(pipePath.m_sidePath1[i+1][VX],pipePath.m_sidePath1[i+1][VY],pipePath.m_sidePath1[i+1][VZ]);

				glEnd();

			}
			else if (pipePath.m_sidePath1[i][VZ]>=0.0&&pipePath.m_sidePath1[i+1][VZ]<0.0)
			{
				glBegin(GL_TRIANGLES);
//				vNormal = cross((pipePath.m_sidePath1[i]-vPrePoint1).Normalize(),(vAftPoint1-vPrePoint1).Normalize()).Normalize();
				vNormal = cross((pipePath.m_sidePath1[i+1]-vPrePoint1).Normalize(),(vAftPoint1-vPrePoint1).Normalize()).Normalize();
				glNormal3d(vNormal[VX],vNormal[VY],vNormal[VZ]);
				ARCVector3 vVector(0.0,0.0,0.0);
				GetHorizonPoint(pipePath.m_sidePath1[i],pipePath.m_sidePath1[i+1],vVector);
				glTexCoord2d(0,/*dTexCoordLeft*/0);
				glVertex3d(vVector[VX],vVector[VY],vVector[VZ]);	

				glTexCoord2d(1,/*dTexCoordRight*/0);
				glVertex3d(pipePath.m_sidePath1[i+1][VX],pipePath.m_sidePath1[i+1][VY],pipePath.m_sidePath1[i+1][VZ]);
				glTexCoord2d(0,/*dTexCoordLeft*/1);
				glVertex3d(vAftPoint1[VX],vAftPoint1[VY],vAftPoint1[VZ]);
				//DistanceUnit dLen = vPrePoint1.DistanceTo(vAftPoint1);
				//dTexCoordLeft += dLen*0.002;
				//dLen = pipePath.m_sidePath1[i].DistanceTo(pipePath.m_sidePath1[i+1]);
				//dTexCoordRight += dLen*0.002;

				//glTexCoord2d(1,dTexCoordRight);
				//glVertex3d(pipePath.m_sidePath1[i+1][VX],pipePath.m_sidePath1[i+1][VY],pipePath.m_sidePath1[i+1][VZ]);

				glEnd();
			}
		}

		glDisable(GL_CULL_FACE);

        //side2
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		{
			ARCVector3 vNormal(0.0,0.0,0.0);

			if (pipePath.m_sidePath1[i][VZ]<0.0&&pipePath.m_sidePath1[i+1][VZ]<0.0)
			{
				glBegin(GL_QUADS);
				vNormal = cross((vAftPoint2-vPrePoint2).Normalize(),(pipePath.m_sidePath2[i]-vPrePoint2).Normalize()).Normalize();
				glNormal3d(vNormal[VX],vNormal[VY],vNormal[VZ]);
				glTexCoord2d(0,dTexCoordLeft);
				glVertex3d(vPrePoint2[VX],vPrePoint2[VY],0.0);
				glTexCoord2d(1,dTexCoordRight);
				glVertex3d(pipePath.m_sidePath2[i][VX],pipePath.m_sidePath2[i][VY],pipePath.m_sidePath2[i][VZ]);

				DistanceUnit dLen = vPrePoint2.DistanceTo(vAftPoint2);
				dTexCoordLeft += dLen*0.002;
				dLen = pipePath.m_sidePath2[i].DistanceTo(pipePath.m_sidePath2[i+1]);
				dTexCoordRight += dLen*0.002;

				glTexCoord2d(1,dTexCoordRight);
				glVertex3d(pipePath.m_sidePath2[i+1][VX],pipePath.m_sidePath2[i+1][VY],pipePath.m_sidePath2[i+1][VZ]);
				glTexCoord2d(0,dTexCoordLeft);
				glVertex3d(vAftPoint2[VX],vAftPoint2[VY],vAftPoint2[VZ]);
				glEnd();
			}
			else if (pipePath.m_sidePath1[i][VZ]<0.0&&pipePath.m_sidePath1[i+1][VZ]>=0.0)
			{
				glBegin(GL_TRIANGLES);
				vNormal = cross((vAftPoint2-vPrePoint2).Normalize(),(pipePath.m_sidePath2[i]-vPrePoint2).Normalize()).Normalize();
				glNormal3d(vNormal[VX],vNormal[VY],vNormal[VZ]);
				ARCVector3 vVector(0.0,0.0,0.0);
				GetHorizonPoint(pipePath.m_sidePath2[i],pipePath.m_sidePath2[i+1],vVector);
				glTexCoord2d(0,/*dTexCoordLeft*/0);
				glVertex3d(vVector[VX],vVector[VY],vVector[VZ]);
				glTexCoord2d(0,/*dTexCoordLeft*/1);
				glVertex3d(vPrePoint2[VX],vPrePoint2[VY],vPrePoint2[VZ]);
				glTexCoord2d(1,/*dTexCoordRight*/0);
				glVertex3d(pipePath.m_sidePath2[i][VX],pipePath.m_sidePath2[i][VY],pipePath.m_sidePath2[i][VZ]);

				//DistanceUnit dLen = vPrePoint1.DistanceTo(vAftPoint1);
				//dTexCoordLeft += dLen*0.002;
				//dLen = pipePath.m_sidePath1[i].DistanceTo(pipePath.m_sidePath1[i+1]);
				//dTexCoordRight += dLen*0.002;

				//glTexCoord2d(1,dTexCoordRight);
				//glVertex3d(pipePath.m_sidePath1[i+1][VX],pipePath.m_sidePath1[i+1][VY],pipePath.m_sidePath1[i+1][VZ]);

				glEnd();
			}
			else if (pipePath.m_sidePath1[i][VZ]>=0.0&&pipePath.m_sidePath2[i+1][VZ]<0.0)
			{
				glBegin(GL_TRIANGLES);
				vNormal = cross((vAftPoint2-vPrePoint2).Normalize(),(pipePath.m_sidePath2[i+1]-vPrePoint2).Normalize()).Normalize();
				glNormal3d(vNormal[VX],vNormal[VY],vNormal[VZ]);
				ARCVector3 vVector(0.0,0.0,0.0);
				GetHorizonPoint(pipePath.m_sidePath2[i],pipePath.m_sidePath2[i+1],vVector);
				glTexCoord2d(0,/*dTexCoordLeft*/0);
				glVertex3d(vVector[VX],vVector[VY],vVector[VZ]);	

				glTexCoord2d(1,/*dTexCoordRight*/0);
				glVertex3d(pipePath.m_sidePath2[i+1][VX],pipePath.m_sidePath2[i+1][VY],pipePath.m_sidePath2[i+1][VZ]);
				glTexCoord2d(0,/*dTexCoordLeft*/1);
				glVertex3d(vAftPoint2[VX],vAftPoint2[VY],vAftPoint2[VZ]);
				//DistanceUnit dLen = vPrePoint1.DistanceTo(vAftPoint1);
				//dTexCoordLeft += dLen*0.002;
				//dLen = pipePath.m_sidePath1[i].DistanceTo(pipePath.m_sidePath1[i+1]);
				//dTexCoordRight += dLen*0.002;

				//glTexCoord2d(1,dTexCoordRight);
				//glVertex3d(pipePath.m_sidePath1[i+1][VX],pipePath.m_sidePath1[i+1][VY],pipePath.m_sidePath1[i+1][VZ]);

				glEnd();
			}
		}

		glDisable(GL_CULL_FACE);

	}
}

void CAirsideThickness3D::RenderCube(const CPath2008& path, int index,double pathWidth,ARCVector3& vHeight)
{
	if (index<1||index>=path.getCount()-1)
		return;

	ARCPipe pipePath(path,pathWidth);
	ARCVector3 vLeft = pipePath.m_sidePath1[index];
	ARCVector3 vRight = pipePath.m_sidePath2[index];

    ARCVector3 vTempLeft = (pipePath.m_sidePath1[index+1]-pipePath.m_sidePath1[index]).Normalize();
	ARCVector3 vTempRight = (pipePath.m_sidePath2[index]-pipePath.m_sidePath2[index-1]).Normalize();

	ARCVector3 vLeftCube1 = vLeft + vTempLeft * pathWidth/2;
	ARCVector3 vLeftCube2 = vRight + vTempLeft * pathWidth/2;
    

	ARCVector3 vRightCube1 = vLeft - vTempRight * pathWidth/2;
	ARCVector3 vRightCube2 = vRight - vTempRight * pathWidth/2;


	if (vRightCube1[VZ]<=vLeftCube1[VZ])
	{
		if (vRightCube1[VZ]<=vLeft[VZ])	
			vHeight = vRightCube1;
		else
			vHeight = vLeft;
	}
	else
	{
		if (vLeftCube1[VZ]<=vLeft[VZ])
			vHeight = vLeftCube1;
		else
			vHeight = vLeft;
	}	
	
	ARCVector3 vLeftCube3 = vRight;
	ARCVector3 vLeftCube4 = vLeft;
	ARCVector3 vLeftCube5(vLeftCube1[VX],vLeftCube1[VY],vHeight[VZ]-300.0);
	ARCVector3 vLeftCube6(vLeftCube2[VX],vLeftCube2[VY],vHeight[VZ]-300.0);
	ARCVector3 vLeftCube7(vLeftCube3[VX],vLeftCube3[VY],vHeight[VZ]-300.0);
	ARCVector3 vLeftCube8(vLeftCube4[VX],vLeftCube4[VY],vHeight[VZ]-300.0);

	ARCVector3 vRightCube3 = vRight;
	ARCVector3 vRightCube4 = vLeft;
	ARCVector3 vRightCube5(vLeftCube1[VX],vLeftCube1[VY],vHeight[VZ]-300.0);
	ARCVector3 vRightCube6(vLeftCube2[VX],vLeftCube2[VY],vHeight[VZ]-300.0);
	ARCVector3 vRightCube7(vLeftCube3[VX],vLeftCube3[VY],vHeight[VZ]-300.0);
	ARCVector3 vRightCube8(vLeftCube4[VX],vLeftCube4[VY],vHeight[VZ]-300.0);


	//glBegin(GL_QUAD_STRIP);
	//glVertex3dv(vLeftCube8);
	//glVertex3dv(vLeftCube4);
	//glVertex3dv(vLeftCube5);
	//glVertex3dv(vLeftCube1);
	//glVertex3dv(vLeftCube6);
	//glVertex3dv(vLeftCube2);
	//glVertex3dv(vLeftCube7);
	//glVertex3dv(vLeftCube3);
	//glVertex3dv(vLeftCube8);
	//glVertex3dv(vLeftCube4);
	//glEnd();

	//glBegin(GL_QUAD_STRIP);
	//glVertex3dv(vRightCube8);
	//glVertex3dv(vRightCube4);
	//glVertex3dv(vRightCube5);
	//glVertex3dv(vRightCube1);
	//glVertex3dv(vRightCube6);
	//glVertex3dv(vRightCube2);
	//glVertex3dv(vRightCube7);
	//glVertex3dv(vRightCube3);
	//glVertex3dv(vRightCube8);
	//glVertex3dv(vRightCube4);
	//glEnd();
	//




	glBegin(GL_QUADS);
	ARCVector3 vNormalL1 = cross((vLeftCube2-vLeftCube1).Normalize(),(vLeftCube4-vLeftCube1).Normalize()).Normalize(); 
	glNormal3d(vNormalL1[VX],vNormalL1[VY],vNormalL1[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vLeftCube1);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vLeftCube2);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vLeftCube3);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vLeftCube4);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalL2 = cross((vLeftCube8-vLeftCube5).Normalize(),(vLeftCube6-vLeftCube5).Normalize()).Normalize(); 
	glNormal3d(vNormalL2[VX],vNormalL2[VY],vNormalL2[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vLeftCube5);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vLeftCube6);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vLeftCube7);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vLeftCube8);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalL3 = cross((vLeftCube6-vLeftCube5).Normalize(),(vLeftCube1-vLeftCube5).Normalize()).Normalize(); 
	glNormal3d(vNormalL3[VX],vNormalL3[VY],vNormalL3[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vLeftCube1);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vLeftCube5);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vLeftCube6);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vLeftCube2);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalL4 = cross((vLeftCube4-vLeftCube3).Normalize(),(vLeftCube7-vLeftCube3).Normalize()).Normalize(); 
	glNormal3d(vNormalL4[VX],vNormalL4[VY],vNormalL4[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vLeftCube3);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vLeftCube7);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vLeftCube4);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vLeftCube8);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalL5 = cross((vLeftCube4-vLeftCube1).Normalize(),(vLeftCube5-vLeftCube1).Normalize()).Normalize(); 
	glNormal3d(vNormalL5[VX],vNormalL5[VY],vNormalL5[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vLeftCube1);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vLeftCube4);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vLeftCube8);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vLeftCube5);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalL6 = cross((vLeftCube6-vLeftCube2).Normalize(),(vLeftCube3-vLeftCube2).Normalize()).Normalize(); 
	glNormal3d(vNormalL6[VX],vNormalL1[VY],vNormalL1[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vLeftCube2);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vLeftCube6);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vLeftCube7);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vLeftCube3);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalR1 = cross((vRightCube2-vRightCube1).Normalize(),(vRightCube4-vRightCube1).Normalize()).Normalize(); 
	glNormal3d(vNormalR1[VX],vNormalR1[VY],vNormalR1[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vRightCube1);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vRightCube2);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vRightCube3);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vRightCube4);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalR2 = cross((vRightCube8-vRightCube5).Normalize(),(vRightCube6-vRightCube5).Normalize()).Normalize(); 
	glNormal3d(vNormalR2[VX],vNormalR2[VY],vNormalR2[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vRightCube5);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vRightCube6);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vRightCube7);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vRightCube8);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalR3 = cross((vRightCube6-vRightCube5).Normalize(),(vRightCube1-vRightCube5).Normalize()).Normalize(); 
	glNormal3d(vNormalR3[VX],vNormalR3[VY],vNormalR3[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vRightCube1);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vRightCube5);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vRightCube6);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vRightCube2);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalR4 = cross((vRightCube3-vRightCube4).Normalize(),(vRightCube8-vRightCube4).Normalize()).Normalize(); 
	glNormal3d(vNormalR4[VX],vNormalR4[VY],vNormalR4[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vRightCube3);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vRightCube7);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vRightCube4);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vRightCube8);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalR5 = cross((vRightCube4-vRightCube1).Normalize(),(vRightCube5-vRightCube1).Normalize()).Normalize(); 
	glNormal3d(vNormalR5[VX],vNormalR5[VY],vNormalR5[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vRightCube1);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vRightCube4);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vRightCube8);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vRightCube5);
	glEnd();

	glBegin(GL_QUADS);
	ARCVector3 vNormalR6 = cross((vRightCube6-vRightCube2).Normalize(),(vRightCube3-vRightCube2).Normalize()).Normalize(); 
	glNormal3d(vNormalR6[VX],vNormalR6[VY],vNormalR6[VZ]);
	glTexCoord2d(0.0,0.0);
	glVertex3dv(vRightCube2);
	glTexCoord2d(1.0,0.0);
	glVertex3dv(vRightCube6);
	glTexCoord2d(1.0,1.0);
	glVertex3dv(vRightCube7);
	glTexCoord2d(0.0,1.0);
	glVertex3dv(vRightCube3);
	glEnd();

}

void CAirsideThickness3D::RenderCylinder(const CPath2008& path, int index, double pathWidth,const ARCVector3& vHeight)
{
	ARCPipe pipePath(path,pathWidth);
	ARCVector3 vCenter = pipePath.m_centerPath[index];
	GLUquadricObj * quadObj = gluNewQuadric();
	glPushMatrix();
	gluQuadricDrawStyle(quadObj,GL_FILL);
	gluQuadricNormals(quadObj,GL_SMOOTH);
	gluQuadricOrientation(quadObj,GLU_OUTSIDE);
	gluQuadricTexture(quadObj,GL_TRUE);
	glColor3ub(255,255,255);
//	glLoadIdentity();
	glTranslated(vCenter[VX],vCenter[VY],0.0);
	gluCylinder(quadObj,pathWidth/10,pathWidth/10,vHeight[VZ],20,8);
	glPopMatrix();
//	gluDeleteQuadric(quadObj);
}


void CAirsideThickness3D::RenderBridge(C3DView* pView, const CPath2008& path, double pathWidth)
{
	CTexture * pTexture = pView->getTextureResource()->getTexture("cylinder");

	if(pTexture)
		pTexture->Apply();
	for (int i=0; i<path.getCount()-1; i++)
	{
		ARCVector3 vHeight(0.0,0.0,0.0);
		if (path.getPoint(i).getZ()>0.0)
		{
			RenderCube(path,i,pathWidth,vHeight);
			RenderCylinder(path,i,pathWidth,vHeight);
		}
	}
}

