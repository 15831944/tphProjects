#include "StdAfx.h"
#include ".\landsideroundaboout3d.h"
#include "landside\LandsideRoundabout.h"
#include "CommonShapes.h"
#include <common/ARCPipe.h>
#include "MaterialDef.h"
#include "LandsideStretch3D.h"
#include "Common/BizierCurve.h"
#include "Landside/LandsideStretch.h"
#include "SceneNodeQueryData.h"
#include "landside/ILandsideEditContext.h"
#include "Render3DScene.h"
#include "SceneState.h"
#include "landside/LandsideEditCommand.h"

using namespace Ogre;
#define STRETCH_BASE_MAT_SEL "Stretch/RoadBaseTransparent"
#define STRETCH_BASE_MAT "Stretch/RoadBase"

PROTYPE_INSTANCE(LandsideRoundAboout3D)
void BuildCurveLink(ManualObject* pObj, StretchSideInfo& sport, LandsideRoundabout* pabout,BOOL bEdit)
{
	double dRad = 0.5*(pabout->getOutRadius()+pabout->getInnerRadius());
	DistanceUnit dTurnDist = 1000;
	double dTurnAngle = ARCMath::RadiansToDegrees(dTurnDist/dRad);//5 degree
	dTurnAngle = MIN(20,dTurnAngle);

	bool bClockFillet = (pabout->isClockwise() == sport.isHead());
	if(bClockFillet)
		dTurnAngle= -dTurnAngle;

	ARCVector3 dir = sport.mCenter;

	

	double dLen = dRad/Math::Cos(Degree(dTurnAngle));
	//double dDirExtLen = dLen*Math::Sin(Degree(dTurnAngle));
	CPoint2008 EndPt = CPoint2008(dir.GetRotateXY(dTurnAngle).GetLengtDir(dRad));
	CPoint2008 dEndDir = CPoint2008(dir.GetRotateXY(dTurnAngle).PerpendicularLCopy());
	if(bClockFillet)
		dEndDir = -dEndDir;

	CPoint2008 startPt = CPoint2008(sport.mCenter);
	
	double dEndWidth = pabout->getOutRadius()-pabout->getInnerRadius();
	double dStartWidth = sport.mRight.distance3D(sport.mLeft);


	double distToEnd = startPt.distance(EndPt);
	//use bezier curver
	//get four control points
	std::vector<CPoint2008> vCtrlPts;
	vCtrlPts.reserve(4);
	vCtrlPts.push_back(startPt + sport.mDir.GetLengtDir(0.05*distToEnd) );
	vCtrlPts.push_back(startPt + sport.mDir.GetLengtDir(distToEnd*0.4));
	vCtrlPts.push_back(EndPt+dEndDir.getLengtDir(distToEnd*0.4));
	vCtrlPts.push_back(EndPt+dEndDir.getLengtDir(distToEnd*0.05));

	std::vector<CPoint2008> vOutput;
	BizierCurve::GenCurvePoints(vCtrlPts,vOutput,10);
	vOutput.insert(vOutput.begin(),startPt);
	vOutput.push_back(EndPt);

	CPath2008 path;
	path.init((int)vOutput.size(),&vOutput[0]);
	ARCPipe cpipe(path,dStartWidth,dEndWidth);
	static const double sc_dCenterLineTexBias = 0.1;
	CommonShapes cShape3D(pObj);
	cShape3D.msMat = bEdit?STRETCH_BASE_MAT_SEL:STRETCH_BASE_MAT;
	cShape3D.BuildTexturePipe2D(cpipe,0.5 + sc_dCenterLineTexBias, 0.5 - sc_dCenterLineTexBias +  pabout->getLaneNumber());

}



void LandsideRoundAboout3D::OnUpdate3D()
{
	LandsideRoundabout* pabout = getAbout();
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	CPoint2008 cp;
	if(!pabout->getPosition(cp))
		return;
	pCtx->ConvertToVisualPos(cp);

	UpdateShape(cp, IsInEditMode());
	UpdateRadiusEdit(IsInEditMode());	

	SetPosition(ARCVector3(cp));

}

void DrawCircleLine(double dRad, ManualObject* pobj,CString smat)
{
	const static int NsliceNum = 20;

	double dDegreeStep  = 360/NsliceNum;
	
	pobj->begin(smat.GetString(),RenderOperation::OT_LINE_STRIP);
	for(int i=0;i<NsliceNum;i++)
	{
		double dcos = Math::Cos(Degree(i*dDegreeStep));
		double dsin = Math::Sin(Degree(i*dDegreeStep));
		pobj->position(dcos*dRad,dsin*dRad,0);
	}
	pobj->position(dRad,0,0);
	pobj->end();
}



LandsideRoundabout* LandsideRoundAboout3D::getAbout()
{
	return (LandsideRoundabout*)getLayOutObject();
}


//
void LandsideRoundAboout3D::UpdateRadiusEdit( BOOL bShow )
{
	LandsideRoundabout* pabout =getAbout();
	RemoveChild();				
	if(bShow)
	{
		//add control nodes
		CString sInner;
		sInner.Format("%s-innerNode",GetName().GetString());
		AboutRadiusControlPoint3D innerNode = CreateGetNameNode(sInner);
		//innerNode.SetPosition(ARCVector3(pabout->getInnerRadius(),0,0));
		innerNode.OnUpdate3D(*this,true);
		innerNode.AttachTo(*this);

		CString sOutter;
		sOutter.Format("%s-outNode",GetName().GetString());
		AboutRadiusControlPoint3D outNode  = CreateGetNameNode(sOutter);
		outNode.OnUpdate3D(*this,false);
		//outNode.SetPosition(ARCVector3(pabout->getOutRadius(),0,0));
		outNode.AttachTo(*this);
	}
	
}

void LandsideRoundAboout3D::UpdateShape(const CPoint2008& ptC, BOOL bEdit )
{
	LandsideRoundabout* pAbout = getAbout();
	ILandsideEditContext* pCtx = GetLandsideEditContext();

	SceneManager* pScene = GetScene();																
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),pScene);
	pObj->clear();

	double dDegreeStep  = 360/LandsideRoundabout::NsliceNum;
	if(pAbout->isClockwise())
		dDegreeStep=-dDegreeStep; //clock wise

	double dInRad = pAbout->m_dInRadius;
	double dOutRad = pAbout->m_dOutRadius;
	int nLaneNum=pAbout->getLaneNumber();

	ARCPath3 centerPath;
	centerPath.reserve(LandsideRoundabout::NsliceNum);

	double dRadC = (dInRad+dOutRad)*0.5;
	double dWidth = dOutRad - dInRad;
	double dChord = sqrt((Math::Cos(Degree(dDegreeStep))*dRadC-dRadC)*(Math::Cos(Degree(dDegreeStep))*dRadC-dRadC)\
		+(Math::Sin(Degree(dDegreeStep))*dRadC)*(Math::Sin(Degree(dDegreeStep))*dRadC));
	for(int i=0;i<LandsideRoundabout::NsliceNum;i++)
	{
		double dcos = Math::Cos(Degree(i*dDegreeStep));
		double dsin = Math::Sin(Degree(i*dDegreeStep));
		centerPath.push_back( ARCVector3(dcos*dRadC,dsin*dRadC,0) );
	}

	ARCPipe cpipe(centerPath,dWidth,true);		
	static const double sc_dCenterLineTexBias = 0.1;
	CommonShapes cShape3D(pObj);
	cShape3D.msMat = bEdit?STRETCH_BASE_MAT_SEL:STRETCH_BASE_MAT;
	cShape3D.BuildTexturePipe2D(cpipe,0.5 + sc_dCenterLineTexBias, 0.5 - sc_dCenterLineTexBias +  nLaneNum);

	//draw mark
	double dmarkOffset;
	const static double dmarklength =  pAbout->getLaneWidth();
	for (int i=0;i<pAbout->getLaneNumber();i++)
	{
		ARCPath3 lanepath;
		double dLanPos = (i+0.5)*pAbout->getLaneWidth();
		LandsideRoundAboout3D::GetPipeMidPath(cpipe,lanepath,dLanPos);
		dmarkOffset = lanepath.GetLength();
		CString sMat = LandsideStretch3D::sMarkLaneMat[0];
		float idxPos;
		ARCVector3 vPos;
		ARCVector3 vDir;
		if ( !pAbout->isClockwise())
		{
			for(int j=0;j<3;j++)
			{
				idxPos = lanepath.GetDistIndex(lanepath.GetLength() - j/3.0*dmarkOffset);
				vPos = lanepath.GetIndexPos(idxPos);
				vDir = lanepath.GetIndexDir(idxPos);
				CommonShapes shapes(pObj);shapes.msMat =sMat;
				shapes.DrawSquare(vPos,vDir,pAbout->getLaneWidth(),dmarklength);
			}
		}
		else
		{
			for(int j=0;j<3;j++)
			{
				idxPos = lanepath.GetDistIndex(j/3.0*dmarkOffset);
				vPos = lanepath.GetIndexPos(idxPos);
				vDir = lanepath.GetIndexDir(idxPos);
				CommonShapes shapes(pObj);shapes.msMat =sMat;
				shapes.DrawSquare(vPos,vDir,pAbout->getLaneWidth(),dmarklength);
			}
		}	
	}
	//
	for(int i=0;i< pAbout->getLinkStretchCount();i++)
	{
		StretchSide& side = pAbout->getLinkStretch(i);
		LandsideStretch* pStretch= (LandsideStretch*)side.m_pStretch.get();
		StretchSideInfo port;
		if(pStretch && pStretch->GetSideInfo(side.ntype,port))
		{
			pCtx->ConvertToVisualPos(port.mCenter);
			pCtx->ConvertToVisualPos(port.mLeft);
			pCtx->ConvertToVisualPos(port.mRight);
			port.mCenter = port.mCenter - ptC;
			port.mRight = port.mRight - ptC;
			port.mLeft = port.mLeft - ptC;
			BuildCurveLink(pObj,port,pAbout,bEdit);
		}
	}
	if(pAbout->IsNewObject())
		AddObject(pObj,2);
	else
		AddObject(pObj,1);

}

void LandsideRoundAboout3D::GetPipeMidPath( const ARCPipe& pipe, ARCPath3& path, double dDist )
{
	int nCount = (int)pipe.m_centerPath.size();
	path.resize(nCount);

	for(int i=0;i<nCount;i++)
	{
		double dRat =dDist/pipe.m_dWidth;
		path[i] = pipe.m_sidePath1[i] * (1.0-dRat) + pipe.m_sidePath2[i]*dRat; 
	}
}


//////////////////////////////////////////////////////////////////////////
#define PTTYPE "pointtype"
#define S_OBJECTID "pobject"
void AboutRadiusControlPoint3D::OnUpdate3D( LandsideRoundAboout3D& parent,bool bInner )
{
	LandsideRoundabout*pabout = parent.getAbout();
	if(!pabout)
		return;
	//draw 
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(PTTYPE, bInner);		
		pData->SetData(S_OBJECTID,(int)pabout);		
	}
	//
	/*Entity* pEnt =  OgreUtil::createOrRetrieveEntity(GetIDName(),SphereMesh,GetScene());
	ARCColor3 color = ARCColor3(255,191,0);
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(color));
	AddObject(pEnt,2);*/

	CString editShape;
	editShape.Format("%s-circle",GetName().GetString());
	ManualObject* pmobj = OgreUtil::createOrRetrieveManualObject(editShape,GetScene());
	if(pmobj)
	{
		double drad = bInner?pabout->getInnerRadius():pabout->getOutRadius();
		pmobj->clear();
		DrawCircleLine(drad,pmobj,"");
		AddObject(pmobj,2);
	}
}
void AboutRadiusControlPoint3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideRoundabout *pabout = getAbout();
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pabout,plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
	
}

BOOL AboutRadiusControlPoint3D::isInnerRadius() const
{
	if(SceneNodeQueryData*pData = getQueryData() )
	{
		BOOL isInner = TRUE;
		if(pData->GetData(PTTYPE,isInner))
		{
			return isInner;
		}
	}
	return TRUE;
}

LandsideRoundabout* AboutRadiusControlPoint3D::getAbout()
{
	if(SceneNodeQueryData*pData = getQueryData() )
	{
		LandsideRoundabout* pabout = NULL;
		if(pData->GetData(S_OBJECTID,(int&)(pabout)) )
		{
			return pabout;
		}
	}
	return NULL;
}

void AboutRadiusControlPoint3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;


	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
	LandsideRoundabout *pabout = getAbout();
	CPoint2008 pC;
	if(!pabout->getPosition(pC))
		return;

	Vector3 vDir = curMouseInfo.mWorldPos-Vector3(pC.x,pC.y,0);
	double diff = mousediff.dotProduct(vDir.normalisedCopy());
	
	if(isInnerRadius())
	{
		double dendRad = pabout->getInnerRadius()+diff;
		dendRad = MIN(pabout->getOutRadius(),dendRad);
		dendRad = MAX(0,dendRad);
		pabout->setInnerRadius(dendRad);
	}
	else
	{
		double dendRad = pabout->getOutRadius()+diff;
		dendRad = MAX(dendRad,pabout->getInnerRadius());
		pabout->setOutRadius(dendRad);
	}
	plandsideCtx->NotifyViewCurentOperation();	
	
}

void AboutRadiusControlPoint3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

bool AboutRadiusControlPoint3D::IsDraggable()const
{
	return true;
}


PROTYPE_INSTANCE(AboutRadiusControlPoint3D)