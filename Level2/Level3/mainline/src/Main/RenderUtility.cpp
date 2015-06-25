#include "StdAfx.h"
#include "../Common/ARCPath.h"
#include "../Common/ARCPipe.h"
#include "../Engine/BridgeConnector.h"
#include ".\renderutility.h"
#include "../Common/ARCColor.h"
#include "TermPlanDoc.h"
#include "../Results/BridgeConnectorLog.h"
#include "Floor2.h"

CRenderUtility::CRenderUtility(void)
{
}

CRenderUtility::~CRenderUtility(void)
{
}
//circle datas
class CircleData
{
public:
	CircleData()
	{
		vDatas.reserve(101);
		vDatas.push_back(ARCVector2(1,0));
		const static double rad = ARCMath::DegreesToRadians(360.0/40);
		for(int i=1;i<40;i++)
		{
			vDatas.push_back(ARCVector2(cos(rad*i),sin(rad*i)));
		}
		vDatas.push_back(ARCVector2(1,0));
	}
	static CircleData& Instance(){
		static CircleData data;
		return data;
	}
	std::vector< ARCVector2> vDatas;
	const int GetCount()const{ return (int)vDatas.size(); }
	const ARCVector2& Get(int idx)const{ return vDatas[idx]; }
		
};

static void DrawCircle(bool bHeight = false,bool bSide = false, double dHeight = 0.0)
{
	glNormal3d(0,0,1);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0,0,0);
	for(int i=0;i<CircleData::Instance().GetCount();i++)
	{
		const ARCVector2& vec = CircleData::Instance().Get(i);
		glVertex3d(vec[VX],vec[VY],0);
	}
	glEnd();

	if(bHeight)
	{
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(0,0,dHeight);
		for(int i=0;i<CircleData::Instance().GetCount();i++)
		{
			const ARCVector2& vec = CircleData::Instance().Get(i);
			glVertex3d(vec[VX],vec[VY],dHeight);
		}
		glEnd();

		if(bSide)
		{
			glBegin(GL_QUAD_STRIP);
			for(int i=0;i<CircleData::Instance().GetCount();i++)
			{
				const ARCVector2& vec = CircleData::Instance().Get(i);
				glNormal3d(vec[VX],vec[VY],0);
				glVertex3d(vec[VX],vec[VY],dHeight);
				glVertex3d(vec[VX],vec[VY],0);
			}
			glEnd();
		}

	}

	
}

static void DrawSquare(double dwidth, double dheight, const Point& vFrom,const Point& vTo, bool bOnlySide = false)
{
	

	CPath2008 path;
	path.init(2);
	path[0] = CPoint2008(vFrom.getX(),vFrom.getY(),vFrom.getZ());
	path[1] = CPoint2008(vTo.getX(),vTo.getY(),vTo.getZ());

	ARCPipe pipe(path,dwidth);
	
	if(!bOnlySide)
	{
	//bottom
	glNormal3d(0,0,1);
	glBegin(GL_QUADS);
		glVertex3dv(pipe.m_sidePath1[0]);
		glVertex3dv(pipe.m_sidePath2[0]);
		glVertex3dv(pipe.m_sidePath2[1]);
		glVertex3dv(pipe.m_sidePath1[1]);
	glEnd();
	//top
	glPushMatrix();
	glTranslated(0,0,dheight);
	glNormal3d(0,0,1);
	glBegin(GL_QUADS);
		glVertex3dv( pipe.m_sidePath1[0]);
		glVertex3dv( pipe.m_sidePath2[0]);
		glVertex3dv( pipe.m_sidePath2[1]);
		glVertex3dv( pipe.m_sidePath1[1]);
	glEnd();
	glPopMatrix();
	}

	//left
	ARCVector3 vNorm = pipe.m_sidePath1[0]-vFrom;
	vNorm.Normalize();
	glNormal3dv(vNorm);
	glBegin(GL_QUADS);
		glVertex3dv( pipe.m_sidePath1[1] );
		glVertex3dv( pipe.m_sidePath1[0] );
		glVertex3dv( pipe.m_sidePath1[0] + ARCVector3(0,0,dheight) );
		glVertex3dv( pipe.m_sidePath1[1] + ARCVector3(0,0,dheight) );
	glEnd();

	//right
	vNorm = pipe.m_sidePath2[0] - vFrom;
	vNorm.Normalize();
	glNormal3dv(vNorm);
	glBegin(GL_QUADS);
		glVertex3dv(pipe.m_sidePath2[0]);
		glVertex3dv(pipe.m_sidePath2[1]);
		glVertex3dv(pipe.m_sidePath2[1] + ARCVector3(0,0,dheight) );
		glVertex3dv(pipe.m_sidePath2[0] + ARCVector3(0,0,dheight) );
	glEnd();


}

static void DrawSquare(double dwidth, double dheight, const ARCVector3& vFrom,const ARCVector3& vTo, bool bOnlySide = false)
{
	DrawSquare(dwidth,dheight,Point(vFrom[VX],vFrom[VY],vFrom[VZ]),Point(vTo[VX],vTo[VY],vTo[VZ]),bOnlySide);
}

void DrawBridgeConnector( const CPoint2008& vFrom,const CPoint2008& vTo1,double dwidth,double dheight,double dLen )
{	
	ARCVector3 cDir = ARCVector3(vTo1 - vFrom);	
	cDir.Normalize();

	double ConPart = 100+40+20+40+10;	
	//glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);
	//DrawSquare(dwidth,dheight,vFrom, vFrom+cDir*10 );
	DrawSquare(dwidth,dheight,vFrom+cDir*(dLen-ConPart), vFrom+cDir*(dLen-ConPart+100) );
	DrawSquare(dwidth,dheight,vFrom+cDir*(dLen-ConPart+140), vFrom+cDir*(dLen-ConPart+160) );
	DrawSquare(dwidth,dheight,vFrom+cDir*(dLen-ConPart+200), vFrom+cDir*dLen );

	for(double dAccLen = 0;dAccLen<dLen-ConPart;dAccLen+=150)
	{
		DrawSquare(dwidth-10,dheight-5,vFrom+cDir*dAccLen, vFrom+cDir*(dAccLen+10),true);
	}

	glPushMatrix();
	glTranslated(0,0,dheight-10);
	DrawSquare(dwidth-10,5,vFrom+cDir*10,vFrom+ cDir*(dLen-ConPart) );
	glPopMatrix();
	DrawSquare(dwidth-10,5,vFrom+cDir*10,vFrom+ cDir*(dLen-ConPart) );
	//draw connector side to flight
	GLfloat color[4] = { 0.4f,0.4f,0.4f,1 };
	glColor4fv(color);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);
	glPushMatrix();
	glTranslated(0,0,5);
	DrawSquare(dwidth-10,dheight-10,vFrom+cDir*(dLen-ConPart),vFrom+cDir*dLen);
	glPopMatrix();

	

	//draw blender square
	//glColor4f(1,1,1,0.5);
	//glDepthMask(GL_FALSE);
	//DrawSquare(dwidth-11,dheight-11,vFrom, vFrom+cDir*(dLen-ConPart),true );

	//glDepthMask(GL_TRUE);
}

void DrawBridgeConnector( const CPoint2008& vFrom,const CPoint2008& vTo1,const CPoint2008& vTo2, double dwidth,double dheight,double dLen )
{
	DrawBridgeConnector( vFrom,vTo1,dwidth,dheight,dLen );
////render line
	glBegin(GL_LINES);
		glVertex3d(vFrom.getX(),vFrom.getY(),vFrom.getZ());
		ARCVector3 vDir1 = ARCVector3(vTo2-vFrom);
		CPoint2008 vDir = CPoint2008(vDir1);
		vDir.length(dLen);
		vDir+=vFrom;
		glVertex3d(vDir.getX(),vDir.getY(),vDir.getZ());
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(vFrom.getX(),vFrom.getY(),vFrom.getZ());
	 vDir = CPoint2008(vTo1-vFrom);
	vDir.length(dLen);
	vDir+=vFrom;
	glVertex3d(vDir.getX(),vDir.getY(),vDir.getZ());
	glEnd();
	glBegin(GL_LINE_STRIP);
		for(int i=0;i<=10;i++)
		{
			double dR = i/10.0;
			CPoint2008 vD = vTo1*(1-dR)+vTo2*dR;
			vD -= vFrom;
			vD.length(dLen);
			vD+=vFrom;
			glVertex3d(vD.getX(),vD.getY(),vD.getZ());
		}
	glEnd();
}

void SelectBridgeConnector( const Point& vFrom,const Point& vTo1, const Point& vTo2,double dwidth,double dheight,double dLen  )
{
	Point cDir = (vTo1 - vFrom);	
	cDir.Normalize();
	DrawSquare(dwidth,dheight,vFrom,vFrom+cDir*dLen);
}


void CRenderUtility::DrawBridgeConPtr( const BridgeConnector* pConnector, CTermPlanDoc* pDoc , const ARCColor3& color,double dAlt )
{
	//if(pConnector && pConnector->GetConnectPointCount()>idx)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		const BridgeConnector::ConnectPoint& p = pConnector->m_connectPoint;
		//glPushMatrix();
		//glTranslated(0,0,dAlt);
		glColor3ubv(color);

		if(pDoc->m_eAnimState != CTermPlanDoc::anim_none)
		{
			long nTime = pDoc->m_animData.nLastAnimTime;
			CBridgeConnectorLog* pLog = pDoc->GetTerminal().m_pBridgeConnectorLog;
			if(pLog)
			{
				int nLogCount = pLog->getCount();
				for(int i=0;i<nLogCount;i++)
				{
					CBridgeConnectorLogEntry bridgeElment;
					BridgeConnectorDescStruct bds;
					pLog->getItem(bridgeElment,i);
					bridgeElment.SetOutputTerminal(&(pDoc->GetTerminal()));
					bridgeElment.SetEventLog(pDoc->GetTerminal().m_pBridgeConnectorEventLog);
					bridgeElment.initStruct(bds);

					
					/*CString sLift;
					sLift.Format("%d", idx );*/
					ProcessorID bridgeID;
					bridgeID.setIDS(bds.name);
					if( !(bridgeID == *pConnector->getID()) )
						continue;

					//if(bds.indexNum != idx)continue;
					//if(bds.startTime>nTime)continue;
					//if(bds.endTime<nTime)continue;

					BridgeConnectorEventStruct besA,besB;
					bool bRet = bridgeElment.getEventsAtTime(nTime,besA,besB);
					if(bRet)
					{
					
						float besBZ, besAZ;
						int w = besB.time - besA.time;
						int d = besB.time - nTime;
						double r = ((double) d) / w;
						float xpos = static_cast<float>((1-r)*besB.x + r*besA.x);
						float ypos = static_cast<float>((1-r)*besB.y + r*besA.y);
						float zPos;
					
						if (besB.state == 'C'|| besB.state == 'D')//state 'C' and 'D' is terminal mode
						{
							besBZ = static_cast<float>(besB.z);
						}
						else
						{
							besBZ = static_cast<float>(dAlt);
						}

						if (besA.state == 'C' || besA.state == 'D')//state 'C' and 'D' is terminal mode
						{
							besAZ = static_cast<float>(besA.z);
						}
						else
						{
							besAZ = static_cast<float>(dAlt);
						}
						
						zPos = static_cast<float>((1-r)*besBZ + r*besAZ);
						 
						CPoint2008 pStartPos = CPoint2008(p.m_Location.getX(),p.m_Location.getY(),dAlt);						
						CPoint2008 pEndPos = CPoint2008(xpos,ypos,zPos);
						DrawBridgeConnector(pStartPos,pEndPos,p.m_dWidth,250,(pEndPos-pStartPos).Length() );
						return;
					}			

				}
			}
		}
		
		glColor3ubv(color);
		CPoint2008 pStartPos = CPoint2008(p.m_Location.getX(),p.m_Location.getY(),dAlt);		
		CPoint2008 pEndPos = CPoint2008(p.m_dirFrom.getX(),p.m_dirFrom.getY(),dAlt);		
		DrawBridgeConnector(pStartPos,pEndPos,p.m_dWidth,250,p.m_dLength);

		//glPopMatrix();

	}
	//glColor3ubv(color);
	//glPushMatrix();
	//glTranslated(p.m_Location.getX(),p.m_Location.getY(),p.m_Location.getZ());
	//glScaled(p.m_dWidth,p.m_dWidth,0);
	//DrawCircle(true,false,200);
	//glPopMatrix();
}

void CRenderUtility::SelectBridgeConPtr( const BridgeConnector::ConnectPoint& p,double dAlt )
{
	glPushMatrix();
	glTranslated(0,0,dAlt);SelectBridgeConnector(p.m_Location,p.m_dirFrom,p.m_dirTo,p.m_dWidth,250,p.m_dLength);	
	glPopMatrix();
}

void CRenderUtility::DrawStair( const CPoint2008& vFrom, const CPoint2008& vTo,double dwidth )
{
	glColor3ub(255,250,250);
	CPath2008 path;
	path.init(2);
	path[0] = CPoint2008(vFrom.getX(),vFrom.getY(),vFrom.getZ());
	path[1] = CPoint2008(vTo.getX(),vTo.getY(),vTo.getZ());	
	ARCPipe pipe(path,dwidth);
	//
	DrawSquare(5,50,pipe.m_sidePath1[0],pipe.m_sidePath1[1]);
	DrawSquare(5,50,pipe.m_sidePath2[0],pipe.m_sidePath2[1]);


	DrawSquare(dwidth,1,vFrom,vTo);

}