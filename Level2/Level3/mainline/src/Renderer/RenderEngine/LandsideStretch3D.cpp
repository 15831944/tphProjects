#include "StdAfx.h"
#include <common/ray.h>
#include "Landside\LandsideStretch.h"
#include "SceneState.h"
#include ".\landsidestretch3d.h"
#include <common/ARCPath.h>
#include "./ShapeBuilder.h"
#include "landside/ILandsideEditContext.h"
#include "Render3DScene.h"
#include "landside/LandsideEditCommand.h"
#include "SceneNodeQueryData.h"
#include <landside/LandsideQueryData.h>
#include <common/ARCStringConvert.h>
#include "CommonShapes.h"
#include <common/ARCPipe.h>
#include "MaterialDef.h"
#include "Mouse3DState.h"
#include "ManualObjectUtil.h"
//#include <common/ARCConsole.h>

#define STRETCH_BASE_MAT "Stretch/RoadBase"
#define STRETCH_MARKLINE_MAT "Stretch/MarkLine"
#define STRETCH_SIDEWALK_MAT "Stretch/SideWalk"


#define STRETCH_BASE_MAT_SEL "Stretch/RoadBaseTransparent"

CString LandsideStretch3D::sMarkLaneMat[] = 
{
	"Stretch/MarkC",
	"Stretch/MarkLC",
	"Stretch/MarkLCR",
	"Stretch/MarkCR",
	"Stretch/MarkR",
	"Stretch/MarkL",
};

using namespace Ogre;

String LandsideStretch3D::mat_highwayside  = "Stretch/HighWaySide";
String LandsideStretch3D::mat_streetwalkside = "Stretch/StreetWalkSide";

String LandsideStretch3D::mat_highwayside_blend = "Stretch/HighWaySideBlend";
String LandsideStretch3D::mat_hightwayFoot = "Stretch/HightWayFoot";
String LandsideStretch3D::mat_hightwayFoot_blend = "Stretch/HightWayFootBlend";
String LandsideStretch3D::mat_streetwalkside_blend = "Stretch/StreetWalkSideBlend";

double LandsideStretch3D::GetMousePosInStretch( const MouseSceneInfo& mouseInfo )
{
	LandsideStretch* pStretch = getStretch();
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx || !pStretch)
		return 0;

	const Ray& sceneray = mouseInfo.mSceneRay;

	Point Origin = Point(sceneray.getOrigin().x,sceneray.getOrigin().y,sceneray.getOrigin().z );
	Point Direction = Point(sceneray.getDirection().x,sceneray.getDirection().y,sceneray.getDirection().z);
	ARCRay ray(Origin,Direction);

	CPath2008& path2008 = pStretch->getControlPath().getBuildPath();
	CPath2008 VisualPath(path2008);
	pCtx->ConvertToVisualPath(VisualPath);
	Path path = VisualPath;
	DistanceRay3Path3 dsitraypath(ray,path);
	dsitraypath.GetSquared(0);
	return VisualPath.GetIndexDist(dsitraypath.m_fPathParameter);
}



static void BuildRoadSurface(int nLaneNum, ManualObject* pObj, const ARCPipe& pipe,bool bEdit)
{
	static const double sc_dCenterLineTexBias = 0.1;
	CommonShapes cShape3D(pObj);
	cShape3D.msMat = bEdit?STRETCH_BASE_MAT_SEL:STRETCH_BASE_MAT;
	cShape3D.BuildTexturePipe2D(pipe,0.5 + sc_dCenterLineTexBias, 0.5 - sc_dCenterLineTexBias + nLaneNum);
}


class ShapeSide
{
public:
	Real sc_SideWidth;
	Real sc_SideHeight;
	Real sc_Thickness;
	BOOL bHighway;
	BOOL bEdit;

	void initHightWay(BOOL isEdit)
	{
		bHighway = TRUE;
		bEdit = isEdit;

		sc_SideHeight = LandsideStretch3D::ls_dDefaultHighwaySideHeight;
		sc_SideWidth = LandsideStretch3D::ls_dDefaultHighwaySideWidth;
		sc_Thickness = LandsideStretch3D::ls_dDefaultHighwayBaseThick;
	}
	void initStreet(double dwideSide, BOOL isEdit)
	{
		bHighway = FALSE;
		bEdit = isEdit;
		sc_SideHeight = LandsideStretch3D::ls_dDefaultStreetSideHeight;
		sc_SideWidth =dwideSide; 
		sc_Thickness = LandsideStretch3D::ls_dDefaultStreetBaseThick;
	}

	void GetSidePts(const ARCPipe& pipe, int i, Vector3* scPts, Real* vLen)
	{
		////			2--1		5--6
		///				|  |		|  |
		////			|  0        4  |
		////			3--------------7
		Vector3 ptCener = OGREVECTOR(pipe.m_centerPath[i]);
		Vector3 vHightOffset = Vector3(0,0,sc_SideHeight);
		Vector3 vThickOffset = Vector3(0,0,sc_Thickness);

		{
			scPts[0] = OGREVECTOR(pipe.m_sidePath1[i]); //left pt    
			Vector3 vSpanDir = (scPts[0]- ptCener).normalisedCopy();
			scPts[1] = scPts[0] + vHightOffset;
			scPts[2]  = scPts[1] + vSpanDir * sc_SideWidth;
			scPts[3]  = scPts[0] + vSpanDir* sc_SideWidth - vThickOffset;

			vLen[0] = 0;
			for(int i=0;i<3;i++)
				vLen[i+1] = vLen[i] + (scPts[i] - scPts[i+1]).length();
		}

		///
		{
			scPts[4] = OGREVECTOR(pipe.m_sidePath2[i]);
			Vector3 vSpanDir = (scPts[4]-ptCener).normalisedCopy();

			scPts[5] = scPts[4] + vHightOffset;
			scPts[6] = scPts[5] + vSpanDir*sc_SideWidth;
			scPts[7] = scPts[4] + vSpanDir*sc_SideWidth - vThickOffset;

			vLen[4] = 0;
			for(int i=4;i<7;i++)
				vLen[i+1] = vLen[i] + (scPts[i] - scPts[i+1]).length();
		}
	}

	void GetFootingPts(const Vector3& pos, const Vector3& dir, double dWidth , Vector3 scPts[][4], const Vector3& vCener, int& nPtLayer)
	{
		const Real dFootingLen = 160;
		const Real dFootingWid = dWidth*0.75;
		const Real dFootingHead = 100;

		
		Vector3 spanDir = dir.crossProduct(Vector3::UNIT_Z);
		Vector3 vSpan = spanDir*dFootingWid*0.5;
		Vector3 vLen = dir*dFootingLen*0.5;
		Vector3 vSpanBig = spanDir* dWidth * 0.5;

		Vector3 vThickOffset = -Vector3(0,0,sc_Thickness);
		//
		Vector3* scPt0 = scPts[0];
		scPt0[0] = pos - vSpanBig - vLen + vThickOffset;
		scPt0[1] = pos + vSpanBig - vLen + vThickOffset; 
		scPt0[2] = pos + vSpanBig + vLen + vThickOffset;
		scPt0[3] = pos - vSpanBig + vLen + vThickOffset; 	


		nPtLayer= 1;
		//
		Real dminH = MIN( MIN(scPt0[0].z, scPt0[1].z), MIN(scPt0[3].z, scPt0[2].z) );
		dminH = MIN(dminH, pos.z - dFootingHead - sc_Thickness);		
		Vector3* scPt1 = scPts[1];
		scPt1[0] = pos - vSpan - vLen; scPt1[0].z = dminH; 
		scPt1[1] = pos + vSpan - vLen; scPt1[1].z = dminH; 
		scPt1[2] = pos + vSpan + vLen; scPt1[2].z = dminH; 
		scPt1[3] = pos - vSpan + vLen; scPt1[3].z = dminH; 
		nPtLayer++;

		if(dminH>0)
		{			
			Vector3* scPt2 = scPts[2];
			scPt2[0] = scPt1[0]; scPt2[0].z = 0;
			scPt2[1] = scPt1[1]; scPt2[1].z = 0; 
			scPt2[2] = scPt1[2]; scPt2[2].z = 0; 
			scPt2[3] = scPt1[3]; scPt2[3].z = 0; 
			nPtLayer++;
		}		

		for(int i=0;i<nPtLayer;i++)
		{
			for(int j=0;j<4;j++)
				scPts[i][j] -= vCener;
		}
	}

	static void drawQuad(Vector3* scPts, Real* dTexU, Real dTexV, ManualObject* pObj, int& dPtAdd, bool bFront)
	{
		Vector3 norm = (scPts[1]-scPts[0]).crossProduct(scPts[2]-scPts[1]).normalisedCopy();
		if(!bFront)
			norm = -norm;

		for(int i=0;i<8;i++)
		{
			pObj->position(scPts[i]);
			pObj->textureCoord(dTexU[i],dTexV);
			pObj->normal(norm);			
		}
		if(bFront)
		{
			pObj->quad(0+dPtAdd,1+dPtAdd,2+dPtAdd,3+dPtAdd);	
			pObj->quad(7+dPtAdd,6+dPtAdd,5+dPtAdd,4+dPtAdd);	
			pObj->quad(0+dPtAdd,3+dPtAdd,7+dPtAdd,4+dPtAdd);
		}
		else
		{
			pObj->quad(3+dPtAdd,2+dPtAdd,1+dPtAdd,0+dPtAdd);	
			pObj->quad(4+dPtAdd,5+dPtAdd,6+dPtAdd,7+dPtAdd);	
			pObj->quad(4+dPtAdd,7+dPtAdd,3+dPtAdd,0+dPtAdd);
		}
		dPtAdd+=8;
	}

	static void drawPip3D(ManualObject* pObj,Vector3 scPtsList[][4], int nLayerCount, int& dPtStart)
	{

		Real dTexV = 0;		

		std::vector<Real> dTexU;
		dTexU.resize(4);
		dTexU[0] = 0;
		for(int i=1;i<4;i++)
			dTexU[i] = dTexU[i-1] + (scPtsList[0][i] - scPtsList[0][i-1]).length();

 		for(int i=1;i<nLayerCount;i++)
		{
			Vector3* scPts = scPtsList[i-1];
			Vector3* scPtsThis = scPtsList[i];
			
			Real dTexVThis = dTexV+ (scPtsThis[0] - scPts[0]).length();

			std::vector<Real> dTexUThis; 
			dTexUThis.resize(4);
		
			dTexUThis[0] = 0;
			for(int j=1;j<4;j++)
				dTexUThis[j] = dTexUThis[j-1] + (scPtsThis[j] - scPtsThis[j-1]).length();	

			for(int j=0;j<4;j++)
			{
				int nextJ = (j+1)%4;
				Vector3 vLenDir = scPtsThis[j] - scPts[j];				
				dTexUThis[j] = dTexU[j] + vLenDir.length();
				
				Vector3 norm =  vLenDir.crossProduct(scPtsThis[nextJ] - scPtsThis[j]).normalisedCopy();			

				pObj->position(scPts[j]);pObj->textureCoord(dTexU[j],dTexV);pObj->normal(norm); 
				pObj->position(scPtsThis[j]); pObj->textureCoord(dTexUThis[j],dTexVThis);
				pObj->position(scPtsThis[nextJ]);pObj->textureCoord(dTexUThis[nextJ],dTexVThis);
				pObj->position(scPts[nextJ]); pObj->textureCoord(dTexU[nextJ],dTexV);
				pObj->quad(dPtStart,dPtStart+1,dPtStart+2,dPtStart+3);		

				dPtStart+=4;

				dTexU = dTexUThis;
				dTexV = dTexVThis;
			}		
		
		}
	}

	void DrawHightWayFooting(ManualObject* pObj, const CPath2008& path,double dWidth, const CPoint2008& center )
	{
		int nPtCount = path.getCount();
		ASSERT(nPtCount>=2);

		double dFootStrp = 3000;
		const static double dMinFootHight = 250;
		double nPipeLen = path.GetTotalLength();
		int nFootCount = nPipeLen/dFootStrp;
		Vector3 vCenter = OGREVECTOR(center);

		bool bObjBegined = false;
		int nPtAdd = 0;

		for(int i=1;i<=nFootCount;i++)
		{
			float ptidx = path.GetDistIndex(i*dFootStrp);
			Vector3 vPos = OGREVECTOR(path.GetIndexPoint(ptidx));
			Vector3 vdir = OGREVECTOR(path.GetIndexDir(ptidx)).normalisedCopy();
			Vector3 scPts[3][4];
			int nPtLayer = 0;

			if(vPos.z > dMinFootHight)
			{
				GetFootingPts(vPos,vdir,dWidth,scPts,vCenter,nPtLayer);
				if(nPtLayer<2)
					continue;

				if(!bObjBegined)
				{	
					pObj->begin(bEdit?LandsideStretch3D::mat_hightwayFoot_blend:LandsideStretch3D::mat_hightwayFoot);
					bObjBegined = true;
				}				
				drawPip3D(pObj,scPts,nPtLayer,nPtAdd);				
			}

		}
		if(bObjBegined)
			pObj->end();
	}

	
	void BuildHightwaySide(ManualObject* pObj, const ARCPipe& pipe)
	{
		int nPtCount = (int)pipe.m_centerPath.size();
		ASSERT(nPtCount>=2);

		Vector3 scPts[8];
		Real dTexU[8];
		GetSidePts(pipe,0,scPts, dTexU);
		Real dTexV = 0;		
		int dPtStart = 0;
		
		const String& sMat = LandsideStretch3D::GetSideMat(bHighway,bEdit);
		pObj->begin(sMat);
		//draw face
		drawQuad(scPts,dTexU,dTexV, pObj,dPtStart,true);			
		//
		for(int i=1;i<nPtCount;i++)
		{
			Vector3 scPtsThis[8];
			Real dTexUThis[8];
			Real dTexVThis;
			GetSidePts(pipe,i,scPtsThis,dTexUThis);
			//get texV					
			dTexVThis = dTexV + (scPtsThis[0] - scPts[0]).length();
			

			//side left
			for(int j=0;j<3;j++)
			{
				Vector3 vWidthDir = scPts[j+1]-scPts[j];
				Vector3 vlengthDir1 = scPtsThis[j] - scPts[j];
				Vector3 norm  = -vWidthDir.crossProduct(vlengthDir1).normalisedCopy();

				pObj->position(scPts[j]);pObj->textureCoord(dTexU[j],dTexV);pObj->normal(norm); 
				pObj->position(scPtsThis[j]); pObj->textureCoord(dTexUThis[j],dTexVThis);
				pObj->position(scPtsThis[j+1]);pObj->textureCoord(dTexUThis[j+1],dTexVThis);
				pObj->position(scPts[j+1]); pObj->textureCoord(dTexU[j+1],dTexV);
				pObj->quad(dPtStart,dPtStart+1,dPtStart+2,dPtStart+3);		

				dPtStart+=4;
			}

			//side right
			for(int j=4;j<7;j++)
			{
				Vector3 vWidthDir = scPts[j+1]-scPts[j];
				Vector3 vlengthDir1 = scPtsThis[j] - scPts[j];
				Vector3 norm  = vWidthDir.crossProduct(vlengthDir1).normalisedCopy();

				pObj->position(scPts[j]);pObj->textureCoord(dTexU[j],dTexV);pObj->normal(norm); 
				pObj->position(scPtsThis[j]); pObj->textureCoord(dTexUThis[j],dTexVThis);
				pObj->position(scPtsThis[j+1]);pObj->textureCoord(dTexUThis[j+1],dTexVThis);
				pObj->position(scPts[j+1]); pObj->textureCoord(dTexU[j+1],dTexV);
				pObj->quad(dPtStart,dPtStart+3,dPtStart+2,dPtStart+1);		

				dPtStart+=4;
			}
			//bottom
			{
				Vector3 vWidthDir = scPts[7]-scPts[3];
				Vector3 vlengthDir = scPtsThis[3] - scPts[3];

				Vector3 norm  = -vWidthDir.crossProduct(vlengthDir).normalisedCopy();
				pObj->position(scPts[3]);pObj->textureCoord(dTexU[3],dTexV);pObj->normal(norm); 
				pObj->position(scPtsThis[3]); pObj->textureCoord(dTexUThis[3],dTexVThis);
				pObj->position(scPtsThis[7]);pObj->textureCoord(dTexUThis[7],dTexVThis);
				pObj->position(scPts[7]); pObj->textureCoord(dTexU[7],dTexV);
				pObj->quad(dPtStart,dPtStart+1,dPtStart+2,dPtStart+3);	

				dPtStart+=4;
			}

			memcpy(scPts,scPtsThis,sizeof(scPts));		
			memcpy(dTexU,dTexUThis,sizeof(dTexU));
			dTexV = dTexVThis;
		}

		//draw back
		drawQuad(scPts,dTexU,dTexV, pObj,dPtStart,false);	
		pObj->end();
	}
};



static const double sc_dStretchThickness = 100.0;
static void BuildStetch3D(LandsideStretch* pStretch, ManualObject* pObj, const CPath2008& path,ILandsideEditContext* pCtx, BOOL bEdit)
{
	pObj->clear();	

	int nCount = path.getCount();
	ARCPath3 drawPath;
	for(int i=0;i<nCount;i++)
	{
		drawPath.push_back(path.getPoint(i));
	}
	{
		static const double sc_dCenterLineTexBias = 0.1;

		double basewidth = pStretch->getLaneWidth()*pStretch->getLaneNum();
		//draw base
		double dWidth = pStretch->getLaneNum()*pStretch->getLaneWidth();
		ARCPipe pipe(drawPath,basewidth);
		CommonShapes cShape3D(pObj);
		cShape3D.msMat = bEdit?STRETCH_BASE_MAT_SEL:STRETCH_BASE_MAT;
		
		cShape3D.BuildTexturePipe2D(pipe,0.5 + sc_dCenterLineTexBias, 0.5 - sc_dCenterLineTexBias + pStretch->getLaneNum());
		//draw mark
		const static double dmarkOffset = pStretch->getLaneWidth()*0.5 + 200;
		const static double dmarklength = pStretch->getLaneWidth();
		for(int i=0;i<pStretch->getLaneNum();i++)
		{
			LandsideLane& pLane = pStretch->getLane(i);
			ARCPath3 lanepath;
			double dLanPos = (i+0.5)*pStretch->getLaneWidth();
			LandsideStretch3D::GetPipeMidPath(pipe,lanepath,dLanPos);
			CString sMat = LandsideStretch3D::sMarkLaneMat[pLane.m_turnType];
			if ( pLane.m_dir == LandsideLane::_positive)
			{
				float idxPos = lanepath.GetDistIndex(lanepath.GetLength() - dmarkOffset);
				ARCVector3 vPos = lanepath.GetIndexPos(idxPos);
				ARCVector3 vDir = lanepath.GetIndexDir(idxPos);
				CommonShapes shapes(pObj);shapes.msMat =sMat;
				shapes.DrawSquare(vPos,vDir,pStretch->getLaneWidth(),dmarklength);
			}
			else
			{
				float idxPos = lanepath.GetDistIndex(dmarkOffset);
				ARCVector3 vPos = lanepath.GetIndexPos(idxPos);
				ARCVector3 vDir = -lanepath.GetIndexDir(idxPos);
				CommonShapes shapes(pObj);shapes.msMat =sMat;
				shapes.DrawSquare(vPos,vDir,pStretch->getLaneWidth(),dmarklength);
			}			
		}
		//draw text mark
		if(pStretch->getLaneNum()>1)
		{
			ManualObjectUtil manObj(pObj);
			manObj.beginText(pStretch->getLaneWidth());
			for(int i=0;i<pStretch->getLaneNum();i++)
			{
				LandsideLane& pLane = pStretch->getLane(i);
				ARCPath3 lanepath;
				double dLanPos = (i+0.5)*pStretch->getLaneWidth();
				LandsideStretch3D::GetPipeMidPath(pipe,lanepath,dLanPos);
				double idxPos = lanepath.GetDistIndex(lanepath.GetLength()*0.5);
				ARCVector3 vPos = lanepath.GetIndexPos(idxPos);
				ARCVector3 vDir = lanepath.GetIndexDir(idxPos);
				manObj.setIdentTransform();
				manObj.rotate(ARCVector3::UNIT_Y,vDir);			
				manObj.translate(vPos);
				manObj.drawCenterAligmentText(ARCStringConvert::toString(i+1));
			}
			manObj.end();
		}
		
		//draw side
		ShapeSide shapeside;
		if(pStretch->getStretchType() == LandsideStretch::_street)
		{
			shapeside.initStreet(pStretch->getSideWalkWidth(), bEdit);				
		}	
		else
			shapeside.initHightWay(bEdit);
		shapeside.BuildHightwaySide(pObj,pipe);
		//draw sidewalk
		//shapeside.DrawHightWayFooting(pObj,path,basewidth,center);	
	}

}
PROTYPE_INSTANCE(LandsideStretch3D)

void LandsideStretch3D::OnUpdate3D()
{
	LandsideStretch* pStretch = getStretch();	
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx || !pStretch)
		return;

	BOOL bEdit = IsInEditMode();
	CPoint2008 centerPt;
	if(!pStretch->getCenter(centerPt))
		return;
	

	CPath2008 path = pStretch->getControlPath().getBuildPath();
	pCtx->ConvertToVisualPos(centerPt);
	pCtx->ConvertToVisualPath(path);
	path.DoOffset(-centerPt.x,-centerPt.y,-centerPt.z);
	
	SceneManager* pScene = GetScene();																
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),pScene);

	
	BuildStetch3D(pStretch, pObj,path,pCtx,bEdit);
	if(pStretch->IsNewObject())
		AddObject(pObj,2);
	else
		AddObject(pObj,1);
	//update control path
	LandsideObjectControlPath3D ctrlpath = GetControlPath3D();

	if(IsInEditMode())
	{		
		ctrlpath.Update3D(pStretch,pCtx,centerPt,path);
		ctrlpath.AttachTo(*this);
	}
	else 
		ctrlpath.Detach();
	
	//set position
	SetPosition(ARCVector3(centerPt));		

}


void LandsideStretch3D::OnUpdateBePickMode( IRenderSceneMouseState* pState )
{
	RemoveChild();	
	if(!pState)
		return ;
	if(pState->toLinkstretch()
		|| pState->toSelectLaneNodes() )
	{
		ShowLaneNode3D();
	}

	if(pState->toPickStretchSides())
	{
		ShowStretchSideNode3D();
	}
}








LandsideStretch* LandsideStretch3D::getStretch()
{
	return (LandsideStretch*)getLayOutObject();
}


LandsideObjectControlPath3D LandsideStretch3D::GetControlPath3D()
{
	CString sControlPath = GetIDName() + "ControlPath";
	LandsideObjectControlPath3D path3d = OgreUtil::createOrRetrieveSceneNode(sControlPath,GetScene());
	path3d.Init(getStretch());
	return path3d;
}

void LandsideStretch3D::OnUpdateEditMode( BOOL b )
{	
	//
	SetEditMode(b);	
	OnUpdate3D();
	//
}


double LandsideStretch3D::GetMousePosIndexInStretchControlPath( const MouseSceneInfo& mouseInfo )
{
	LandsideStretch* pStretch = getStretch();
	if(!pStretch)
		return 0;

	const Ray& sceneray = mouseInfo.mSceneRay;

	Point Origin = Point(sceneray.getOrigin().x,sceneray.getOrigin().y,sceneray.getOrigin().z );
	Point Direction = Point(sceneray.getDirection().x,sceneray.getDirection().y,sceneray.getDirection().z);
	ARCRay ray(Origin,Direction);

	CPath2008 path2008 = pStretch->getControlPath().getPath();
	Path path = path2008;
	DistanceRay3Path3 dsitraypath(ray,path);
	dsitraypath.GetSquared(0);
	return dsitraypath.m_fPathParameter;	
}


Ogre::String& LandsideStretch3D::GetSideMat( BOOL bHighway, BOOL bEdit )
{
	return bHighway?(bEdit?mat_highwayside_blend:mat_highwayside):(bEdit?mat_streetwalkside_blend:mat_streetwalkside);
}

void LandsideStretch3D::ShowLaneNode3D()
{
	LandsideStretch* pStretch = getStretch();
	CPoint2008 cp = CPoint2008(GetPosition());
	ILandsideEditContext* pEditCtx = GetLandsideEditContext();
	if(!pEditCtx)return;

	for(int i=0;i<pStretch->getLaneNum();i++)
	{
		CPoint2008 ptPos;ARCVector3 ptDir;			
		{		
			CString strName;
			strName.Format("%s(%dentry)",GetIDName(),i);
			LaneLinkageNode3D entrynode = OgreUtil::createOrRetrieveSceneNode(strName,GetScene());

			LaneNode laneEntry;
			laneEntry.m_pStretch = pStretch;
			laneEntry.nLaneIdx = i;
			laneEntry.ntype = LaneNode::_lane_entry;			

			if(laneEntry.GetPosDir(ptPos,ptDir))
			{
				pEditCtx->ConvertToVisualPos(ptPos);
				entrynode.Update3D(laneEntry);
				entrynode.AttachTo(*this);
				entrynode.SetPosition(ptPos-cp);
				entrynode.SetDirection(ptDir,ARCVector3(0,0,1));
			}
		}
		{
			CString strName;
			strName.Format("%s(%dexit)",GetIDName(),i);
			LaneLinkageNode3D exitnode = OgreUtil::createOrRetrieveSceneNode(strName,GetScene());			

			LaneNode laneExit;
			laneExit.m_pStretch = pStretch;
			laneExit.nLaneIdx = i;
			laneExit.ntype = LaneNode::_lane_exit;

			if(laneExit.GetPosDir(ptPos,ptDir))
			{
				pEditCtx->ConvertToVisualPos(ptPos);
				exitnode.Update3D(laneExit);
				exitnode.AttachTo(*this);
				exitnode.SetPosition(ptPos-cp);		
				exitnode.SetDirection(ptDir,ARCVector3(0,0,1));
			}

		}	

	}
}

void LandsideStretch3D::ShowStretchSideNode3D()
{
	LandsideStretch* pStretch = getStretch();
	CPoint2008 cp = CPoint2008(GetPosition());
	ILandsideEditContext* pEditCtx = GetLandsideEditContext();
	if(!pEditCtx)return;

		
	CString strName;
	strName.Format("%s-head",GetIDName());
	StretchSideNode3D entrynode = OgreUtil::createOrRetrieveSceneNode(strName,GetScene());
	
	StretchSideInfo endpt;
	if( pStretch->GetHeadInfo(endpt) )
	{
		CPoint2008 endPos = endpt.mCenter;
		pEditCtx->ConvertToVisualPos(endPos);
		entrynode.Update3D(endpt);
		entrynode.AttachTo(*this);
		entrynode.SetPosition(endPos-cp);
		entrynode.SetDirection(endpt.mDir,ARCVector3(0,0,1));
	}

	strName.Format("%s-trail",GetIDName());
	StretchSideNode3D tnode = OgreUtil::createOrRetrieveSceneNode(strName,GetScene());
	if( pStretch->GetTrailInfo(endpt) )
	{
		CPoint2008 endPos = endpt.mCenter;
		pEditCtx->ConvertToVisualPos(endPos);
		tnode.Update3D(endpt);
		tnode.AttachTo(*this);
		tnode.SetPosition(endPos-cp);
		tnode.SetDirection(endpt.mDir,ARCVector3(0,0,1));
	}

	
	
}



double LandsideStretch3D::ls_dDefaultHighwayBaseThick = 30;
double LandsideStretch3D::ls_dDefaultStreetBaseThick = 15;
double LandsideStretch3D::ls_dDefaultHighwaySideWidth = 20;
double LandsideStretch3D::ls_dDefaultHighwaySideHeight = 100;
double LandsideStretch3D::ls_dDefaultStreetSideHeight = 10;

//////////////////////////////////////////////////////////////////////////
#define SphereMesh _T("Sphere.mesh")

PROTYPE_INSTANCE(LaneLinkageNode3D)

void LaneLinkageNode3D::Update3D( LaneNode& node )
{
	setNodeData(node);

	Entity* pEnt =  OgreUtil::createOrRetrieveEntity(GetIDName(),SphereMesh,GetScene());
	AddObject(pEnt,2);
	ARCColor3 color = (node.IsLaneEntry())?ARCColor3(255,0,0):ARCColor3(0,0,255);
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(color));
}

#define KeyLaneIndex "LaneIndex"
#define KeyLaneNodeType "LaneNodeType"
#define KeyStretchSideType "StrethcSideType"

bool LaneLinkageNode3D::getNodeData( LaneNode& node )
{
	if(SceneNodeQueryData* pqdata = getQueryData())
	{
		LandsideStretch* pStretch = NULL;
		int nLandIdx = -1;
		int nEntryExit = LaneNode::_lane_entry;
		if( pqdata->GetData(KeyAltObjectPointer,(int&)pStretch)
			&& pqdata->GetData(KeyLaneIndex,nLandIdx)
			&& pqdata->GetData(KeyLaneNodeType,nEntryExit) )
		{
			node.m_pStretch = pStretch;
			node.nLaneIdx = nLandIdx;
			node.ntype = (LaneNode::Type)nEntryExit;
			return true;
		}

	}
	return false;
}



void LaneLinkageNode3D::setNodeData( LaneNode& node )
{
	if(SceneNodeQueryData* pqdata = InitQueryData())
	{
		pqdata->SetData(KeyAltObjectPointer,(int)node.m_pStretch.get());
		pqdata->SetData(KeyLaneIndex,node.nLaneIdx);
		pqdata->SetData(KeyLaneNodeType,node.ntype);
	}
}


//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(LandsideObjectControlPath3D)
void LandsideObjectControlPath3D::SetQueryData(LandsideFacilityLayoutObject* pStreth)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pStreth);
		pData->SetData(KeyLandsideItemType, _ObjectControlPath);
	}
}

void LandsideObjectControlPath3D::Update3D( LandsideFacilityLayoutObject* pStreth,ILandsideEditContext* pCtx,const CPoint2008& ptCenter, const CPath2008& path )
{
	ILandsideObjectControlPath* pControlPath = pStreth->GetObjectControlPath();
	if(!pControlPath)
		return;

	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
	
	pObj->clear();	

	CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,path,ARCColor3(255,0,0));
	AddObject(pObj,2);

	//update control points
	RemoveChild();
	for(int i=0;i<pControlPath->getControlPath().getCount();i++)
	{		
		LandsideControlPoint3D ctrlPt = GetControlPoint3D(i);
		ctrlPt.SetQueryData(pStreth,i);
		ctrlPt.Update3D();
		CPoint2008 pt = pControlPath->getControlPath().getPoint(i).m_pt;
		pCtx->ConvertToVisualPos(pt);
		ctrlPt.SetPosition(pt-ptCenter);
		ctrlPt.AttachTo(*this);
	}	

}	

LandsideControlPoint3D LandsideObjectControlPath3D::GetControlPoint3D( int idx )
{
	CString sPt;
	sPt.Format(_T("%s%d"),GetIDName(),idx);
	return OgreUtil::createOrRetrieveSceneNode(sPt,GetScene());
}

void LandsideObjectControlPath3D::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{
	LandsideFacilityLayoutObject* pStretch = getLayoutObject();
	if(!pStretch)return ;
	ILandsideObjectControlPath* pCtrlPath = pStretch->GetObjectControlPath();
	if(!pCtrlPath) return;

	const Ray& sceneray = mouseInfo.mSceneRay;

	Point Origin = Point(sceneray.getOrigin().x,sceneray.getOrigin().y,sceneray.getOrigin().z );
	Point Direction = Point(sceneray.getDirection().x,sceneray.getDirection().y,sceneray.getDirection().z);
	ARCRay ray(Origin,Direction);

	const CPath2008& path2008 = pCtrlPath->getControlPath().getBuildPath();
	Path path = path2008;
	DistanceRay3Path3 dsitraypath(ray,path);
	dsitraypath.GetSquared(0);
	double dPos =  dsitraypath.m_fPathParameter;	
	CPoint2008 pt = path2008.GetIndexPoint(dPos);

	if(SceneNodeQueryData* pData = InitQueryData())
	{
		CPoint2008 pt = pCtrlPath->getControlPath().getBuildPath().GetIndexPoint(dPos);
		pData->SetData(KeyPosIndexInStretchBuildPath,dPos);
		pData->SetData(KeyRClickWorldPos, ARCStringConvert::toString(pt) );
	}
}

LandsideFacilityLayoutObject* LandsideObjectControlPath3D::getLayoutObject()
{
	LandsideFacilityLayoutObject* pObj = NULL;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyAltObjectPointer,(int&)pObj);
	}
	return pObj;
}

void LandsideObjectControlPath3D::Init(LandsideFacilityLayoutObject* pStretch)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pStretch);
		pData->SetData(KeyLandsideItemType, _ObjectControlPath);		
	}
}

//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(LandsideControlPoint3D)

void LandsideControlPoint3D::Update3D()
{
	Entity* pEnt =  OgreUtil::createOrRetrieveEntity(GetIDName(),SphereMesh,GetScene());
	
	ARCColor3 color = ARCColor3(255,191,0);
	if(IsStraightPoint())
		color = ARCColor3(98,250,191);	
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(color));
	AddObject(pEnt,2);
}

void LandsideControlPoint3D::SetQueryData( LandsideFacilityLayoutObject* pobj, int idx )
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pobj);
		pData->SetData(KeyLandsideItemType, _ObjectControlPoint);
		pData->SetData(KeyControlPointIndex,idx);		
	}
}

LandsideFacilityLayoutObject* LandsideControlPoint3D::getLayoutObject()const
{
	LandsideFacilityLayoutObject* pObj = NULL;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyAltObjectPointer,(int&)pObj);
	}
	return pObj;
}


static bool GetMouseDiff(const Vector3&ptworldPos,const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo, Vector3& mousediff  )
{
	BOOL bVertical = FALSE;

	if( abs(curMouseInfo.mSceneRay.getDirection().dotProduct(Vector3::UNIT_Z) )  < 0.3 )
		bVertical = TRUE;


	if(!bVertical)
	{
		Plane p(Vector3::UNIT_Z, ptworldPos);			
		std::pair<bool,Real> ret1 = lastMouseInfo.mSceneRay.intersects(p);
		std::pair<bool,Real> ret2 = curMouseInfo.mSceneRay.intersects(p);
		if(ret1.first && ret2.first)
		{
			mousediff = curMouseInfo.mSceneRay.getPoint(ret2.second) - lastMouseInfo.mSceneRay.getPoint(ret1.second);
			mousediff.z = 0;
			return true;
		}
	}
	else
	{			
		Vector3 vDir = lastMouseInfo.mSceneRay.getDirection().crossProduct(Vector3::UNIT_Z).crossProduct(Vector3::UNIT_Z);
		Plane p(vDir,ptworldPos);
		std::pair<bool,Real> ret1 = lastMouseInfo.mSceneRay.intersects(p);
		std::pair<bool,Real> ret2 = curMouseInfo.mSceneRay.intersects(p);
		if(ret1.first && ret2.first)
		{
			mousediff = curMouseInfo.mSceneRay.getPoint(ret2.second) - lastMouseInfo.mSceneRay.getPoint(ret1.second);
			mousediff.x = 0;
			mousediff.y = 0;
			return true;
		}
	}
	return false;
}

void LandsideControlPoint3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	SetInDragging(TRUE);

	LandsideFacilityLayoutObject* pObj = getLayoutObject();
	if( ILandsideObjectControlPath* pControlPath = pObj->GetObjectControlPath() )
	{
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,plandsideCtx->getInput());	
		plandsideCtx->StartEditOp(pModCmd);
		OnDrag(lastMouseInfo,curMouseInfo);
	}

}

void LandsideControlPoint3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;
	

	LandsideFacilityLayoutObject* pObj = getLayoutObject();
	if( ILandsideObjectControlPath* pControlPath = pObj->GetObjectControlPath() )
	{
		int nPtIdx = getPointIndex();

		Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

		//Vector3 mousediff ;
		//CPoint2008 orignPt = pControlPath->getControlPath().getPoint(nPtIdx).m_pt;
		//Vector3 ptworldPos = Vector3(orignPt.x,orignPt.y, orignPt.z);	

		//if( GetMouseDiff(ptworldPos,lastMouseInfo,curMouseInfo,mousediff) )
		{
			ControlPath& ctrlpath = pControlPath->getControlPath();
			ctrlpath.doPointOffset(nPtIdx, CPoint2008(mousediff.x,mousediff.y,mousediff.z));	
			plandsideCtx->NotifyViewCurentOperation();
		}		
	}
	
}

void LandsideControlPoint3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

bool LandsideControlPoint3D::IsDraggable()const
{
	return true;
}

int LandsideControlPoint3D::getPointIndex() const
{
	int PtIdx = -1;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyControlPointIndex,PtIdx);
	}
	return PtIdx;
}

void LandsideControlPoint3D::OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
{
	ILandsideEditContext* pLandside = getRenderScene()->GetEditContext()->toLandside();
	if(!pLandside)
		return;

	LandsideFacilityLayoutObject* pStretch = getLayoutObject();
	int idxPt = getPointIndex();
	if(ILandsideObjectControlPath* pctrlPath = pStretch->GetObjectControlPath() )
	{
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pStretch,pLandside->getInput());	
		pLandside->StartEditOp(pModCmd); 
		pctrlPath->getControlPath().flipPointType(idxPt);
		pLandside->NotifyViewCurentOperation();		
		pLandside->DoneCurEditOp(); 
	}

}

bool LandsideControlPoint3D::IsStraightPoint() const
{
	LandsideFacilityLayoutObject * pStretch = getLayoutObject();
	int idxPt = getPointIndex();
	if(ILandsideObjectControlPath* pctrlPath = pStretch->GetObjectControlPath() )
	{
		ControlPath& cpath = pctrlPath->getControlPath();
		return cpath.getPoint(idxPt).m_Type == ControlPoint::_Straight;
	}
	return false;
}

void LandsideStretch3D::GetPipeMidPath( const ARCPipe& pipe, ARCPath3& path, double dDist )
{
	int nCount = (int)pipe.m_centerPath.size();
	path.resize(nCount);

	for(int i=0;i<nCount;i++)
	{
		double dRat =dDist/pipe.m_dWidth;
		path[i] = pipe.m_sidePath1[i] * (1.0-dRat) + pipe.m_sidePath2[i]*dRat; 
	}
}


bool StretchSideInfo::IsHightWay() const
{
	if(pStretch && pStretch->GetType()==ALT_LSTRETCH)
	{
		LandsideStretch* pS = (LandsideStretch*)pStretch;
		return pS->getStretchType() == LandsideStretch::_highway;
	}
	return false;
}

double StretchSideInfo::GetSideWidth() const
{
	if(pStretch && pStretch->GetType()==ALT_LSTRETCH)
	{
		LandsideStretch* pS = (LandsideStretch*)pStretch;
		return pS->getSideWalkWidth();
	}
	return 0;
}

void StretchSideNode3D::Update3D( StretchSideInfo& side )
{
	setNodeData(side);
	Entity* pEnt =  OgreUtil::createOrRetrieveEntity(GetIDName(),SphereMesh,GetScene());
	AddObject(pEnt,2);
	ARCColor3 color = (side.isHead())?ARCColor3(255,0,0):ARCColor3(0,0,255);
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(color));
}

bool StretchSideNode3D::getNodeData( StretchSideInfo& side )
{
	if(SceneNodeQueryData* pqdata = getQueryData())
	{
		LandsideStretch* pStretch = NULL;
		int nLandIdx = -1;
		int nSide=0;
		if( pqdata->GetData(KeyAltObjectPointer,(int&)pStretch)			
			&& pqdata->GetData(KeyStretchSideType,nSide) )
		{
			side.pStretch = pStretch;			
			side.mSide = (em_StretchSide)nSide;
			return true;
		}

	}
	return false;
}

void StretchSideNode3D::setNodeData( const StretchSideInfo& side )
{

	if(SceneNodeQueryData* pqdata = InitQueryData())
	{
		pqdata->SetData(KeyAltObjectPointer,(int)side.pStretch);	
		pqdata->SetData(KeyStretchSideType,side.mSide);
	}
}

