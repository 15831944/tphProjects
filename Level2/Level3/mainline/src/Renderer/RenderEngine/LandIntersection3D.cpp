#include "StdAfx.h"
#include ".\landintersection3d.h"
#include <common/Path2008.h>
#include "landside/LandsideIntersectionNode.h"
#include "common/ARCVector.h"
#include "Tesselator.h"
#include <common/BizierCurve.h>
#include "CommonShapes.h"
#include "common/ARCPath.h"
#include "common/ARCPipe.h"
#include "landside/LandsideQueryData.h"
#include ".\SceneNodeQueryData.h"
#include "LandsideTrafficLight3D.h"
#include "LandsideStretch3D.h"
#include "landside/ILandsideEditContext.h"
#include "Main/LandsideEditContext.h"
#include "Landside/InputLandside.h"
#include "Landside/LandsideLayoutOptions.h"

#define MESH_TRAFFICLIGHT "light.mesh"
#define MESH_TRAFFICLIGHT_PLAN "light_plan.mesh"
#define MESH_TRAFFICLIGHT_LEFT_CROSS "left_cross_light.mesh"
#define MESH_TRAFFICLIGHT_LEFT_CROSS_PLAN "left_cross_light_plan.mesh"
#define MESH_TRAFFICLIGHT_RIGHT_CROSS "right_cross_light.mesh"
#define MESH_TRAFFICLIGHT_RIGHT_CROSS_PLAN "right_cross_light_plan.mesh"

#define NODE_MAT _T("IntersectionNode/Base")
#define NODE_MAT_EDIT _T("IntersectionNode/BaseTrasparent")
#define NODE_LINKAGE_MAT _T("IntersectionNode/Linkage")

//#define N_LINAGE_COUNT 5
/*static CString NODE_LINKAGE_MAT=[N_LINAGE_COUNT]  ={
	_T("IntersectionNode/Linkage0"),
	_T("IntersectionNode/Linkage1"),
	_T("IntersectionNode/Linkage2"),
	_T("IntersectionNode/Linkage3"),
	_T("IntersectionNode/Linkage4")
};*/

#define NODE_LINAGESELECTED_MAT "IntersectionNode/LinkageSelected"

const static int nSmoothCount = 10;


using namespace Ogre;

class RightSideBuilder
{
public:
	double sc_SideHeight;
	double sc_Thickness;
	
	CPoint2008 center;

	void GetRightSidePts(const ARCPipe& pipe , size_t i, Vector3* scPts,Real* vLen)
	{
		Vector3 ptCener = OGREVECTOR(pipe.m_centerPath[i]);
		Vector3 vHightOffset = Vector3(0,0,sc_SideHeight);
		Vector3 vThickOffset = Vector3(0,0,sc_Thickness);

		{
			scPts[0] = OGREVECTOR(pipe.m_centerPath[i]); //left pt 
			
			scPts[1] = scPts[0] + vHightOffset;
			scPts[2]  = OGREVECTOR(pipe.m_sidePath2[i])+ vHightOffset;
			scPts[3]  = OGREVECTOR(pipe.m_sidePath2[i])- vThickOffset;
			scPts[4]  = scPts[0] - vThickOffset;

			vLen[0] = 0;
			for(int i=0;i<3;i++)
				vLen[i+1] = vLen[i] + (scPts[i] - scPts[i+1]).length();
		}		
	}

	void Build(ManualObject* pObj,const ARCPipe& pipe)
	{
		size_t nPtCount = pipe.m_centerPath.size();
		if(nPtCount<2)
			return;

		Vector3 scPts[5];
		Real dTexV=0;
		Real dTexU[5];
		GetRightSidePts(pipe,0,scPts,dTexU);
		int dPtStart = 0;
		for(size_t i=1;i<pipe.m_centerPath.size();i++)
		{
			Vector3 scPtsThis[5];
			Real dTexUThis[5];

			GetRightSidePts(pipe,i,scPtsThis, dTexUThis);
			Real dTexVThis = dTexV + (scPtsThis[0] - scPts[0]).length();

			for(int j=0;j<4;j++)
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
			memcpy(scPts,scPtsThis,sizeof(scPts));		
			memcpy(dTexU,dTexUThis,sizeof(dTexU));
			dTexV = dTexVThis;
		}

	}
};



void BuildNodeSide(ManualObject* pObj, const ARCVectorList& vpath, const StretchSideInfo& port1, const StretchSideInfo& port2, BOOL bInEdit)
{
	bool bDrawHightside = false;
	double dwidFrom,dwidTo;
	double dThick = 0;
	double dHeight =0;
	if(port1.IsHightWay())
	{
		dwidFrom = LandsideStretch3D::ls_dDefaultHighwaySideWidth;
		bDrawHightside = true;		
		dThick = LandsideStretch3D::ls_dDefaultHighwayBaseThick;
		dHeight = LandsideStretch3D::ls_dDefaultHighwaySideHeight;
	}
	else 
	{
		dwidFrom = port1.GetSideWidth();
		if(port2.IsHightWay())
		{
			dwidFrom = LandsideStretch3D::ls_dDefaultHighwaySideWidth;
			bDrawHightside = true;

			dThick = LandsideStretch3D::ls_dDefaultHighwayBaseThick;
			dHeight = LandsideStretch3D::ls_dDefaultHighwaySideHeight;
		}
		else
		{
			dwidTo = port2.GetSideWidth();
			dThick = LandsideStretch3D::ls_dDefaultStreetBaseThick;
			dHeight = LandsideStretch3D::ls_dDefaultStreetSideHeight;
		}
	}
	
	

	ARCPath3 path = vpath;
	ARCPipe pipe = bDrawHightside?ARCPipe(path,dwidFrom*2):ARCPipe(path,dwidFrom*2,dwidTo*2);
	//fix begin point endpoint
	if(!bDrawHightside)
	{
		ARCVector3 vDir = port2.mLeft - port2.mCenter;
		vDir.SetLength(dwidTo);
		*pipe.m_sidePath2.rbegin() = port2.mLeft + vDir;
	}
	//  

	
	//draw right side only
	RightSideBuilder rightside;
	rightside.sc_SideHeight = dHeight;
	rightside.sc_Thickness = dThick;
	pObj->begin(LandsideStretch3D::GetSideMat(bDrawHightside,bInEdit));
	rightside.Build(pObj,pipe);
	pObj->end();
}

//get side info 
void GetSnapMap(LandsideIntersectionNode* pNode, StretchPortList& spanMap,ILandsideEditContext*pCtx, CPoint2008& ptCenter,bool& bHasStreet)
{
	for(int i=0;i<pNode->getLinageCount();i++)
	{
		LaneLinkage& laneLink = pNode->getLinkage(i);
		StretchSideInfo end;		
		if( laneLink.mLaneExit.GetStretchSideInfo(end) 
			&& std::find(spanMap.begin(),spanMap.end(),end)==spanMap.end() )
		{	
			spanMap.push_back(end);
		}		
		if( laneLink.mLaneEntry.GetStretchSideInfo(end)
			&& std::find(spanMap.begin(),spanMap.end(),end)==spanMap.end() )
		{
			spanMap.push_back(end);
		}
	}

	bHasStreet = false;
	//draw	
	for(StretchPortList::const_iterator itr = spanMap.begin();itr!=spanMap.end();++itr)
	{
		const StretchSideInfo& end = *itr;
		ptCenter=ptCenter+end.mCenter;

		if(!end.IsHightWay())
			bHasStreet = true;
	}
	if(spanMap.empty())
		return;
	ptCenter *= (1.0/spanMap.size());
	std::sort(spanMap.begin(),spanMap.end(),StretchSideInfo::StretchOrder(ptCenter) );
	//
	pCtx->ConvertToVisualPos(ptCenter);
	for(size_t i=0;i<spanMap.size();i++){
		StretchSideInfo& info = spanMap[i];
		pCtx->ConvertToVisualPos(info.mCenter);
		pCtx->ConvertToVisualPos(info.mLeft);
		pCtx->ConvertToVisualPos(info.mRight);
		info.mCenter= info.mCenter - ptCenter;
		info.mLeft= info.mLeft - ptCenter;
		info.mRight = info.mRight - ptCenter;
	}

}


PROTYPE_INSTANCE(LandIntersection3D)
void LandIntersection3D::OnUpdate3D()
{
	LandsideIntersectionNode* pIntesct = getIntersection();
	if(!pIntesct)
		return;
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx)
		return;

	SceneManager* pScene = GetScene();																
	ManualObject* p3DObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),pScene);
	//
	CPoint2008 ptC;
	if (pIntesct->is1stEdit)
	{
		SetEditMode(true);
		pIntesct->is1stEdit = false;
	}

	//
	StretchPortList snapMap;
	bool bStreet;
	GetSnapMap(pIntesct,snapMap,pCtx, ptC,bStreet);
	p3DObj->clear();
	DrawLinkNodes(snapMap,p3DObj,bStreet,IsInEditMode());
	if (pIntesct->IsNewObject())
		//AddObject(p3DObj,true);
		AddObject(p3DObj,2,true);
	else
		AddObject(p3DObj,1,true);
	//
	RemoveChild();

	UpdateLinkages3D(ptC);

	UpdateTrafficLight3D(ptC);

	SetPosition(ARCVector3(ptC));	
}




void LandIntersection3D::OnUpdateEditMode( BOOL b )
{
	SetEditMode(b);	
	OnUpdate3D();
}

LandsideIntersectionNode* LandIntersection3D::getIntersection()
{
	if(ILayoutObject* pObj = getLayOutObject())
	{
		if(pObj->GetType()==ALT_LINTERSECTION)
			return (LandsideIntersectionNode*)pObj;
	}
	return NULL;
}

void LandIntersection3D::UpdateLinkages3D(const CPoint2008& ptC)
{
	LandsideIntersectionNode* pIntesct = getIntersection();
	if(!pIntesct)
		return;

	//RemoveChild();
	if(!IsInEditMode())
		return;

	for(int i=0;i<pIntesct->getLinageCount();i++)
	{
		IntersectionLaneLinkage3D link3D = GetLinkage3D(i);
		link3D.Update3D(pIntesct,i,ptC);
		link3D.AttachTo(*this);
	}
}

IntersectionLaneLinkage3D LandIntersection3D::GetLinkage3D( int idx )
{
	CString sName;
	sName.Format("%sLink%d",GetIDName(),idx);
	return OgreUtil::createOrRetrieveSceneNode(sName,GetScene());
}
void LandIntersection3D::UpdateTrafficLight3D(CPoint2008 ptC)
{
	LandsideIntersectionNode* pIntesct = getIntersection();
	if(!pIntesct)
		return;
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx)
		return;

	bool bLeftDrive = pCtx->isLeftDrive();

	//LandsideIntersectionNode::LaneNodeGroupMap &map_laneGroup=pIntesct->GetLaneGroupMap();
	//LandsideIntersectionNode::Iter_LaneGroupMap it_LaneGroup;
	//for (it_LaneGroup=map_laneGroup.begin();it_LaneGroup!=map_laneGroup.end();it_LaneGroup++)
	//{
	//	LaneNode laneNode=it_landsideStretchGroup->first;
	//	LandsideStretch* pStretch = (LandsideStretch*)laneNode.m_pStretch.get();
	LandsideIntersectionNode::LandsideStretchGroupMap &map_landsideStretchGroup=pIntesct->GetLandsideStretchGroupMap();
	LandsideIntersectionNode::Iter_LandsideStretchGroupMap it_landsideStretchGroup;
	for (it_landsideStretchGroup=map_landsideStretchGroup.begin();it_landsideStretchGroup!=map_landsideStretchGroup.end();it_landsideStretchGroup++)
	{
		//LaneNode laneNode;
		LandsideStretch* pStretch=it_landsideStretchGroup->first;
		/////////////////////////////////

		/////////////////////////////////

		CPoint2008 pos2D;
		CPoint2008 pos3D;
		ARCVector3 ptDir;
		CPath2008& path = pStretch->getControlPath().getBuildPath();
		double dLaneWidth = pStretch->getLaneWidth();
		int nLaneCount = pStretch->getLaneNum();
		int nPtCnt = path.getCount();	
		LandsideLane& lane = pStretch->getLane(0);
		if(pStretch->getLane(0).m_dir == LandsideLane::_negative)
		//if( (laneNode.ntype== laneNode._lane_entry && lane.m_dir==LandsideLane::_positive)
		//	|| (laneNode.ntype == laneNode._lane_exit && lane.m_dir == LandsideLane::_negative))
		{
			ptDir = path[1] - path[0];
			CPoint2008 ptCenter = path[0];
			ptDir = ptDir.PerpendicularLCopy();
			ptDir.z= (0);
			/*ptDir.SetLength(0.1);
			pos2D = ptCenter+ ptDir;*/
			ptDir.SetLength(0.5 * nLaneCount * dLaneWidth);
			pos3D = ptCenter+ ptDir;
			ptDir.SetLength(0.5 * (nLaneCount+1) * dLaneWidth);
			pos2D = ptCenter+ ptDir;
		}
		else
		{
			ptDir = path[nPtCnt-1] - path[nPtCnt-2];
			CPoint2008 ptCenter = path[nPtCnt-1];
			ptDir = ptDir.PerpendicularLCopy();
			ptDir.z = (0);
			/*ptDir.SetLength(0.1);
			pos2D = ptCenter+ ptDir;*/
			ptDir.SetLength(0.5 * nLaneCount * dLaneWidth);
			pos3D = ptCenter+ ptDir;
			ptDir.SetLength(0.5 * (nLaneCount+1) * dLaneWidth);
			pos2D = ptCenter+ ptDir;
		}
		ptDir.RotateXY(180);
	
		//3D traffic light
		CString sName;
		sName.Format("%s-TrafficLight3DNode-%d",GetIDName(), pStretch->getID());
		LandsideTrafficLight3D light3D = OgreUtil::createOrRetrieveSceneNode(sName, GetScene());
		if (pIntesct->getNodeType() == LandsideIntersectionNode::_Signalized) 
		{			
			//int groupSize = (int)it_LaneGroup->second.size();
			int groupSize = (int)it_landsideStretchGroup->second.size();
			CString sMesh;
			switch (groupSize)
			{
				case 1:
					sMesh = MESH_TRAFFICLIGHT;
					break;
				case 2:
					if(bLeftDrive == true)
						sMesh = MESH_TRAFFICLIGHT_LEFT_CROSS;
					else
						sMesh = MESH_TRAFFICLIGHT_RIGHT_CROSS;
					break;
				default:
					break;
			}

			pCtx->ConvertToVisualPos(pos3D);
			CPoint2008 position = CPoint2008(pos3D-ptC);
			light3D.SetLocationData(position,ptDir,pStretch->getID(),/*laneNode.nLaneIdx,*/groupSize,pStretch->getLaneWidth(),sName,sMesh);
			light3D.Update3D();	
			light3D.SetVisible(true);
			light3D.AttachTo(*this);
			light3D.SetScale(ARCVector3(2,2,2));
		}
		else
			light3D.SetVisible(false);
			
		//2D traffic light
		BOOL isHide = pCtx->GetbHideTrafficLight();
		if(!isHide)
		{
			CString sName;
			sName.Format("%s-TrafficLight2DNode-%d",GetIDName(), pStretch->getID());
			LandsideTrafficLight3D light2D = OgreUtil::createOrRetrieveSceneNode(sName, GetScene());
			if (pIntesct->getNodeType() == LandsideIntersectionNode::_Signalized) 
			{				
				//int groupSize = (int)it_LaneGroup->second.size();
				int groupSize = (int)it_landsideStretchGroup->second.size();
				CString sMesh;
				switch (groupSize)
				{
					case 1:
						sMesh = MESH_TRAFFICLIGHT_PLAN;
						break;
					case 2:
						if(bLeftDrive == true)
							sMesh = MESH_TRAFFICLIGHT_LEFT_CROSS_PLAN;
						else
							sMesh = MESH_TRAFFICLIGHT_RIGHT_CROSS_PLAN;
						break;
					default:
						break;
				}

				pCtx->ConvertToVisualPos(pos2D);
				CPoint2008 position = CPoint2008(pos2D-ptC);
				light2D.SetLocationData(position,ptDir,pStretch->getID(),/*laneNode.nLaneIdx,*/groupSize,pStretch->getLaneWidth(),sName,sMesh);
				light2D.Update3D();	
				light2D.SetVisible(true);
				light2D.AttachTo(*this);
				light2D.SetScale(ARCVector3(10,10,1));
			}
			else
				light2D.SetVisible(false);
		}
	}
}
/*
void LandIntersection3D::BuildPole(ManualObject* pObj,CPoint2008 stretchLeftPos,CPoint2008 stretchRightPos,CPoint2008 dir)
{
	const String& sMat =_T("TrafficLight/Pole");
	pObj->begin(sMat);

	stretchLeftPos+=dir*8;
	stretchRightPos+=dir*8;

	pObj->position(stretchLeftPos.x,stretchLeftPos.y,stretchLeftPos.z);//0
	pObj->position(stretchRightPos.x,stretchRightPos.y,stretchRightPos.z);//1
	pObj->position(stretchRightPos.x,stretchRightPos.y,stretchRightPos.z+20);//2
	pObj->position(stretchLeftPos.x,stretchLeftPos.y,stretchLeftPos.z+20);//3


	stretchLeftPos-=dir*16;
	stretchRightPos-=dir*16;

	pObj->position(stretchLeftPos.x,stretchLeftPos.y,stretchLeftPos.z);//4
	pObj->position(stretchRightPos.x,stretchRightPos.y,stretchRightPos.z);//5
	pObj->position(stretchRightPos.x,stretchRightPos.y,stretchRightPos.z+20);//6
	pObj->position(stretchLeftPos.x,stretchLeftPos.y,stretchLeftPos.z+20);//7

	pObj->quad(0,1,2,3);
	pObj->quad(4,7,6,5);
	pObj->quad(3,2,6,7);
	pObj->quad(0,4,5,1);
	pObj->quad(0,3,7,4);
	pObj->quad(1,5,6,2);
	pObj->end();

}*/

void LandIntersection3D::DrawLinkNodes(const StretchPortList& spanMap, ManualObject* pObj,bool bStreet,BOOL bInEdit )
{
	ARCVectorList vList;	
	ARCVectorList vCtrlPts;
	vCtrlPts.resize(4);
	ARCVectorList vOutPts;
	vOutPts.reserve(nSmoothCount);

	for(size_t i=0;i<spanMap.size();i++)
	{
		size_t iNext = (i+1)%spanMap.size();

		const StretchSideInfo& end = spanMap[i];
		const StretchSideInfo& endNext = spanMap[iNext];

		double dDist= end.mRight.distance3D(endNext.mLeft)*0.5;
		vCtrlPts[0]= end.mRight;
		vCtrlPts[1] = end.mRight + end.mDir*dDist;
		vCtrlPts[2] = endNext.mLeft + endNext.mDir*dDist;
		vCtrlPts[3] = endNext.mLeft;		
		BizierCurve::GenCurvePoints(vCtrlPts,vOutPts,nSmoothCount);
		vList.insert(vList.end(),vOutPts.begin(),vOutPts.end());
		BuildNodeSide(pObj, vOutPts,end, endNext,bInEdit);

	}
	ARCVectorListList listlist;
	listlist.push_back(vList);

	{
		CTesselator tessor;
		tessor.MakeMeshPositive(listlist,Vector3::UNIT_Z);
		tessor.BuildMeshToObject(pObj,bInEdit?NODE_MAT_EDIT:NODE_MAT,Vector3::UNIT_Z,ColourValue::White);
	}
	{

		CTesselator tessor;
		tessor.MakeMeshNegative(listlist,-Vector3::UNIT_Z);
		double dThick = bStreet?LandsideStretch3D::ls_dDefaultStreetBaseThick:LandsideStretch3D::ls_dDefaultHighwayBaseThick;
		tessor.BuildMeshToObject(pObj,bInEdit?NODE_MAT_EDIT:NODE_MAT,-Vector3::UNIT_Z,ColourValue::White,-dThick);
	}	
}
//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(IntersectionLaneLinkage3D)
void IntersectionLaneLinkage3D::Update3D( LandsideIntersectionNode* pNode, int iLinkIdx ,const CPoint2008& ptCenter)
{
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx)return;

	SetQueryData(pNode,iLinkIdx);


	ManualObject* pObj= OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
	pObj->clear();


	LaneLinkage& linkage = pNode->getLinkage(iLinkIdx);	

	
	CPath2008 linkpath;
	if( linkage.GetPath(linkpath) )
	{
		pCtx->ConvertToVisualPath(linkpath);
		CommonShapes arrowshape(pObj);
		if (linkage.isSelected)
			arrowshape.msMat = NODE_LINAGESELECTED_MAT;
		else
		{
			
			//int nLColor = MAX(0,linkage.getGroupID())%N_LINAGE_COUNT;
			arrowshape.msMat = NODE_LINKAGE_MAT;//[nLColor];
		}
		linkpath.DoOffset(-ptCenter.x,-ptCenter.y,-ptCenter.z);
		arrowshape.DrawPipeArrow(linkpath,50,200);
		AddObject(pObj,2);
	}
	
	/*CPoint2008 exitPos, exitDir;
	CPoint2008 entryPos,entryDir;
	if( linkage.mLaneExit.GetPosDir(exitPos,exitDir) 
		&&linkage.mLaneEntry.GetPosDir(entryPos,entryDir) )
	{		
		double dDist = entryPos.distance3D(exitPos)*0.5;
		
		ARCVectorList vCtrlPts;
		vCtrlPts.resize(4);

		vCtrlPts[0]= exitPos - ptCenter;
		vCtrlPts[1] = exitPos + exitDir*dDist- ptCenter;
		vCtrlPts[2] = entryPos - entryDir*dDist- ptCenter;
		vCtrlPts[3] = entryPos- ptCenter;

		ARCVectorList vOutPts;
		BizierCurve::GenCurvePoints(vCtrlPts,vOutPts,nSmoothCount);

		CommonShapes arrowshape(pObj);
		arrowshape.msMat = NODE_LINKAGE_MAT;
		arrowshape.DrawPipeArrow(vOutPts,100,200);

		AddObject(pObj,2);
	}*/



}

void IntersectionLaneLinkage3D::SetQueryData( LandsideIntersectionNode* pNode, int iLinkIdx )
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyLandsideItemType,_LaneLinkage);
		pData->SetData(KeyAltObjectPointer, (int)pNode);
		pData->SetData(KeyItemIndex,iLinkIdx);
	}
}

LandsideIntersectionNode* IntersectionLaneLinkage3D::GetNode() const
{
	LandsideIntersectionNode* pPtr = NULL;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyAltObjectPointer,(int&)pPtr);
	}
	return pPtr;
}

int IntersectionLaneLinkage3D::GetIndex() const
{
	int idx = -1;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyItemIndex,idx);
	}
	return idx;
}
