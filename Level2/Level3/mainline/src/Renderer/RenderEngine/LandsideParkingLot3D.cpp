#include "StdAfx.h"
#include ".\landsideparkinglot3d.h"
#include "Tesselator.h"

#include "ShapeBuilder.h"
#include "common/ARCPath.h"
#include "ShapeBuilder.h"
#include <common/ARCPipe.h>
#include "landside/ILandsideEditContext.h"
#include "landside/LandsideQueryData.h"
#include ".\SceneNodeQueryData.h"
#include "LandIntersection3D.h"
#include "MaterialDef.h"
#include "SceneState.h"
#include "Render3DScene.h"
#include "landside/LandsideEditCommand.h"
#include <common/ARCStringConvert.h>
#include "CommonShapes.h"
#include <common/Ray.h>
//#include <common/ARCConsole.h>
using namespace Ogre;
#define PARK_AREA_MAT "Parkinglot/Area"
#define PARK_AREA_MAT_EDIT "parkingLot/AreaEdit"
#define PARK_AREA_MAT_SELECTED "parkingLot/AreaSelected"
#define PARK_ZEBRA_MAT "ParkingLot/ZebraLine"
#define PARK_ZEBRA_MAT_SELECTED "ParkingLot/ZebraLineSelected"
#define PARK_GATE_MESH "ParkingLot_Gate.mesh"
#define PARK_DRIVEPIPE_MARK_MAT "ParkingLot/MarkC"
#define PARK_DRIVEPIPE_MARKDOWN_MAT "ParkingLot/MarkC_Down"
#define PARK_SPACE_MAT "ParkingLot/ParkSpace"
#define PARK_SPACEBASE_MAT "ParkingLot/ParkSpaceBase"

#define LINE_WIDTH 20
#define PARK_ENTRY_MAT "Parkinglot/Entry"
#define FLOORMAT_OUTLINE _T("BaseFloorOutLine")


#define PARK_ZEBRA_LINE_WIDTH 64
#include "ManualObjectUtil.h"
#include "CustomizedRenderQueue.h"
//


void RenderPallaleSpace(int nLine,ParkingSpace& space ,ManualObject* pObj, ManualObject* pSelectObj, const CPoint2008& ptCenter)
{
	//ColourValue c = space.m_bSelected?ColourValue::Red:ColourValue::White;
	//c.a = 0.8;
	CPath2008 ctrlPath = space.m_ctrlPath;
	ctrlPath.DoOffset(-ptCenter.getX(),-ptCenter.getY(),0);
	double dLength = ctrlPath.GetTotalLength();
	int nSpotCount = dLength/space.m_dLength;

	ARCPath3 pathleft;	ARCPath3 pathRight;
	pathleft.reserve(nSpotCount+1);
	pathRight.reserve(nSpotCount+1);

	{//draw visible object
		ManualObjectUtil drawobj(pObj);	
		drawobj.clear();


		Painter painter;
		painter.UseVertexColor(space.m_bSelected?ARCColor4(255,0,0,200):ARCColor4(255,255,255,200));
		painter.msMat = PARK_SPACE_MAT;
		drawobj.beginTriangleList(painter);

		ARCPath3 pathCenter;
		pathCenter.resize(2);
		for(int i=0;i<=nSpotCount;i++)
		{

			double distInpath = space.m_dLength*i;
			ARCVector3 ptC = ctrlPath.GetDistPoint(distInpath);
			ARCVector3 ptDir = ctrlPath.GetDistDir(distInpath);
			ptDir = ptDir.PerpendicularLCopy();
			ptDir.SetLength(space.m_dWidth*0.5);
			pathCenter[0] = ptC + ptDir;
			pathCenter[1] = ptC- ptDir;

			drawobj.drawPath(ARCPipe(pathCenter,LINE_WIDTH));

			pathleft.push_back(pathCenter[0]);
			pathRight.push_back(pathCenter[1]);

		}

		drawobj.drawPath(ARCPipe(pathleft,LINE_WIDTH));
		drawobj.drawPath(ARCPipe(pathRight,LINE_WIDTH));

		drawobj.end();
		//draw mark
		drawobj.beginText(100);

		drawobj.setColor(ARCColor4(255,255,0,255));
		ARCVector3 markCenter =  (pathRight[0] + pathRight[1]+pathleft[0]+pathleft[1])*.25;
		ARCVector3 dir = pathRight[1] - pathRight[0];
		drawobj.setIdentTransform();
		drawobj.rotate(ARCVector3::UNIT_X,-dir.Normalize());
		drawobj.translate(markCenter);
		drawobj.drawCenterAligmentText( ARCStringConvert::toString(nLine+1) );

		drawobj.setColor(ARCColor4(255,255,255,255));
		for(int i=0;i<nSpotCount;i++)
		{
			ARCVector3 markCenter =  (pathRight[i] + pathRight[i+1]+pathleft[i]+pathleft[i+1])*0.25;
			ARCVector3 dir = pathleft[i] - pathRight[i];
			drawobj.setIdentTransform();
			drawobj.rotate(ARCVector3::UNIT_X,-dir.Normalize());
			drawobj.translate(markCenter);
			drawobj.drawCenterAligmentText( ARCStringConvert::toString(i+1) );
		}
		drawobj.end();
	}
	

	{//draw select obj

		ManualObjectUtil selObj(pSelectObj); 
		selObj.clear();
		selObj.beginTriangleList(Painter());
		selObj.drawPipe(pathleft,pathRight);
		selObj.end();
	}
};


void RenderAngleSpace(int nLine,ParkingSpace& space ,ManualObject* pObj,ManualObject* pSelectObj, const CPoint2008& ptCenter)
{
	CPath2008 ctrlPath = space.m_ctrlPath;
	ctrlPath.DoOffset(-ptCenter.getX(),-ptCenter.getY(),0);
	double dLength = ctrlPath.GetTotalLength();
	int nSpotCount = dLength/space.m_dWidth;
	double dAngle = 0;
	if(space.m_type == ParkingSpace::_angle)
		dAngle = space.m_dAngle;

	//calculate all spots
	ARCPath3 pathleft;	ARCPath3 pathRight;
	pathleft.reserve(nSpotCount+1);
	pathRight.reserve(nSpotCount+1);
	for(int i=0;i<=nSpotCount;i++)
	{
		double distInpath = space.m_dWidth*i;
		ARCVector3 ptC = ctrlPath.GetDistPoint(distInpath);
		ARCVector3 ptDir = ctrlPath.GetDistDir(distInpath);

		ARCVector2 dir(ptDir);
		dir = dir.PerpendicularLCopy();
		dir.Rotate(dAngle);
		dir.SetLength(space.m_dLength);		

		pathleft.push_back(ptC);
		pathRight.push_back(ptC + ARCVector3(dir,0));
	}

	//draw basement
	Painter painter;
	ManualObjectUtil drawobj(pObj);
	drawobj.clear();
	{//draw basement
		//painter.msMat = PARK_SPACEBASE_MAT;
		//drawobj.beginTriangleList(painter);
		//drawobj.drawPipe(pathRight,pathleft);
		//drawobj.end();
	}
	if(nSpotCount>0)
	{ //draw mark object
		painter.msMat = PARK_SPACE_MAT;
		painter.UseVertexColor(space.m_bSelected?ARCColor4(255,0,0,200):ARCColor4(255,255,255,200));
		ARCPath3 path;
		path.resize(2);
		//begin
		drawobj.beginTriangleList(painter);
		for(int i=0;i<=nSpotCount;i++)
		{
			path[0]=pathRight[i];
			path[1]=pathleft[i];
			drawobj.drawPath(ARCPipe(path,LINE_WIDTH));
		}
		if (space.m_opType ==  ParkingSpace::OperationType::_backup)
		{	
			drawobj.drawPath(ARCPipe(ctrlPath,LINE_WIDTH));
		}
		drawobj.end();
		//draw text
		drawobj.beginText(100);
		drawobj.setColor(ARCColor4(255,255,0,255));
		ARCVector3 markCenter =  (pathRight[0] + pathRight[1]+pathleft[0]+pathleft[1])*.25;
		ARCVector3 dir = pathRight[1] - pathRight[0];
		drawobj.setIdentTransform();
		drawobj.rotate(ARCVector3::UNIT_X,-dir.Normalize());
		drawobj.translate(markCenter);
		drawobj.drawCenterAligmentText( ARCStringConvert::toString(nLine+1) );

		drawobj.setColor(ARCColor4(255,255,255,255));
		for(int i=0;i<nSpotCount;i++)
		{
			ARCVector3 markCenter =  (pathRight[i] + pathRight[i+1])*.5;
			ARCVector3 dir = pathRight[i+1] - pathRight[i];
			drawobj.setIdentTransform();
			drawobj.rotate(ARCVector3::UNIT_X,-dir.Normalize());
			drawobj.translate(markCenter);
			drawobj.drawCenterAligmentText( ARCStringConvert::toString(i+1) );
		}
		drawobj.end();

	}
	{//draw select object
		ManualObjectUtil selObj(pSelectObj); 
		selObj.clear();
		selObj.beginTriangleList(Painter());
		//selObj.drawPipe(pathleft,pathRight);
		selObj.drawPipe(pathRight,pathleft);
		selObj.end();
	}
}


//void RenderPallaleSpaceSelect(ParkingSpace& space ,ManualObject* pObj, const CPoint2008& ptCenter)
//{
//	pObj->clear();
//	CPath2008 ctrlPath = space.m_ctrlPath;
//	ctrlPath.DoOffset(-ptCenter.getX(),-ptCenter.getY(),0);	
//	ProcessorBuilder::DrawColorPath(pObj,ColourValue::White, ctrlPath,space.m_dWidth);
//}
//
//void RenderAngleSpaceSelect(ParkingSpace& space ,ManualObject* pObj, const CPoint2008& ptCenter)
//{
//	pObj->clear();
//	CPath2008 ctrlPath = space.m_ctrlPath;
//	ctrlPath.DoOffset(-ptCenter.getX(),-ptCenter.getY(),0);
//	double dAngle = 0;
//	if(space._angle)
//		dAngle = space.m_dAngle;
//
//	CPath2008 pathSide;
//	pathSide.Resize(ctrlPath.getCount());
//	for(int i=0;i<pathSide.getCount();i++){
//		ARCVector3 ptDir = ctrlPath.GetIndexDir(i);
//		ARCVector2 dir(ptDir);
//		dir = dir.PerpendicularLCopy();
//		dir.Rotate(dAngle);
//		dir.SetLength(space.m_dLength);		
//		pathSide[i] = ctrlPath.getPoint(i) + ARCVector3(dir,0);
//	}
//	//draw rects
//	OgrePainter painter;
//	CShapeBuilder::DrawPipe(pObj,pathSide, ctrlPath,painter);
//}


bool getAnglePath(CPath2008& hostPath, CPath2008& anglePath, int i, double length)
{
	if( i < 1 || i > hostPath.getCount())
		return false;

	anglePath.init(3);
	anglePath[1] = hostPath[i];

	CPoint2008 ldir(hostPath[i] - hostPath[i-1]);
	ldir.Normalize();
	anglePath[0] = hostPath[i] - ldir*(length/2);

	CPoint2008 rdir(hostPath[i+1] - hostPath[i]);
	rdir.Normalize();
	anglePath[2] = hostPath[i] + rdir*(length/2);

	return true;
}

void RenderDrivePipe(const CPath2008& drawPath,ParkingDrivePipe& pipe, ManualObject* pObj,bool bLeftDrive, BOOL IsInEditMode)
{

	Painter painter;
	painter.UseVertexColor(ARCColor4(255,255,255,200));

	if(pipe.m_bSelected){
		painter.msMat = _T("ParkingLot/DrivePipeSelected");	
	}
	else if(!IsInEditMode){
		painter.msMat = _T("ParkingLot/DrivePipe");	
		painter.mbAutoTextCoord = true;
	}
	else{
		painter.msMat = _T("ParkingLot/DrivePipeEditMode");	
	}

	pObj->setRenderQueueGroup(RenderObject_Surface);
	ManualObjectUtil drawObj(pObj);
	drawObj.clear();	
	if(drawPath.getCount()<2)
		return;

	drawObj.beginTriangleList(painter);
	drawObj.drawPath(ARCPipe(drawPath,pipe.m_width),0,pipe.m_nLaneNum);
	drawObj.end();
	

	//ARCPipe drawPipe(drawPath, pipe.m_width);
	//ProcessorBuilder::DrawColorPath(pObj, drawPipe, painter.msmat.c_str(), painter.mColor);

	//CShapeBuilder::DrawPipeOutLine(pObj,drawPipe, OgrePainter().UseColor(ColourValue::Black).SetMat(VERTEXCOLOR_LIGHTOFF_NOCULL) );	



	if(pipe.m_nLaneNum<=0)
		return;
	if(drawPath.getCount()<2)
		return;

	double dLen  = drawPath.GetTotalLength();
	ARCVector3 vDirCenter =  drawPath.GetDistDir(dLen/2.0);
	ARCVector3 vDirL = vDirCenter.PerpendicularLCopy().SetLength(pipe.m_width/2);
	ARCVector3 vPosCenter = drawPath.GetDistPoint(dLen/2.0);
	ARCVector3 vPos1 = vPosCenter - vDirL;
	ARCVector3 vPos2 = vPosCenter + vDirL;
	
	for(int i=0;i<pipe.m_nLaneNum;i++)
	{

		//ARCVector3 vPosStart(drawPath[0]);
		
		double drat = (i+.5)/pipe.m_nLaneNum;

		//vPosStart = drawPipe.m_sidePath1[0]*(1.0-drat) + drawPipe.m_sidePath2[0]*drat;

		bool bresver = false;
		if(pipe.m_nType==ParkingDrivePipe::_bidirectional )
		{
			if( !bLeftDrive )
			{
				bresver  = (i*2>=pipe.m_nLaneNum);
			}
			else
				bresver = (i*2<pipe.m_nLaneNum);
		}


		ARCVector3 vPosStart = vPos1*(1-drat) +vPos2*drat;

		CommonShapes shapesEnd(pObj);shapesEnd.msMat =PARK_DRIVEPIPE_MARK_MAT;
		shapesEnd.DrawSquare(vPosStart,bresver?-vDirCenter:vDirCenter,pipe.m_width/pipe.m_nLaneNum,pipe.m_width);


		/*ARCVector3 vDirStart(drawPath[1] - drawPath[0]);

		CommonShapes shapes(pObj);shapes.msMat =PARK_DRIVEPIPE_MARK_MAT;
		shapes.DrawSquare(vPosStart+ vDirStart.Normalize()*(pipe.m_width/2),bresver?-vDirStart:vDirStart,pipe.m_width/pipe.m_nLaneNum,pipe.m_width);

		int nPtCnt = drawPath.getCount();

		ARCVector3 vDirEnd(drawPath[nPtCnt-1] - drawPath[nPtCnt-2]);

		ARCVector3 vPosEnd = drawPipe.m_sidePath1[nPtCnt-1]*(1-drat) + drawPipe.m_sidePath2[nPtCnt-1]*drat;
		vPosEnd -= vDirEnd.Normalize()*(pipe.m_width/2);

		CommonShapes shapesEnd(pObj);shapesEnd.msMat =PARK_DRIVEPIPE_MARK_MAT;
		shapesEnd.DrawSquare(vPosEnd,bresver?-vDirEnd:vDirEnd,pipe.m_width/pipe.m_nLaneNum,pipe.m_width);*/

	}


}

void GetSnapMap(ParkLotEntryExit& entexit,StretchPortList& spanMap,ILandsideEditContext*pCtx,const CPoint2008& ptCenter,bool& bHasStreet,double levelHeight)
{
	if(entexit.m_line.getCount()<2)
		return;

	CPoint2008 levelCenter = ptCenter;
	levelCenter.z = levelHeight;
	//	
	CPoint2008 nodecenter;	
	for(int i=0;i<entexit.getNodeCount();i++)
	{
		LaneNode& lanenode = entexit.getNode(i);
		StretchSideInfo end;		
		if( lanenode.GetStretchSideInfo(end) 
			&& std::find(spanMap.begin(),spanMap.end(),end)==spanMap.end() )
		{	
			pCtx->ConvertToVisualPos(end.mCenter);
			pCtx->ConvertToVisualPos(end.mLeft);
			pCtx->ConvertToVisualPos(end.mRight);
		
			spanMap.push_back(end);
			nodecenter = nodecenter+ end.mCenter;

			if(!end.IsHightWay())
				bHasStreet = true;
		}			
	}
	if(spanMap.empty())
		return;

	nodecenter *=(1.0/spanMap.size());

	StretchSideInfo mDoorPort;		
	mDoorPort.mCenter =  (entexit.m_line[0] + entexit.m_line[1])*0.5;
	mDoorPort.mCenter.z = levelHeight;
	mDoorPort.mDir = (nodecenter-mDoorPort.mCenter).Normalize();

	//+ CPoint2008(0,0,dheight)
	bool bLeft = mDoorPort.mDir.cross(entexit.m_line[0] - mDoorPort.mCenter).z > 0;
	if(bLeft){
		mDoorPort.mLeft = entexit.m_line[0];
		mDoorPort.mRight = entexit.m_line[1];
	}
	else
	{
		mDoorPort.mLeft = entexit.m_line[0];
		mDoorPort.mRight = entexit.m_line[1];
	}
	mDoorPort.mLeft.z = levelHeight;
	mDoorPort.mRight.z = levelHeight;
	mDoorPort.pStretch=NULL;
	spanMap.push_back(mDoorPort);

	//
	//sort 
	CPoint2008 linkCenter;
	for(StretchPortList::const_iterator itr = spanMap.begin();itr!=spanMap.end();++itr)
	{
		const StretchSideInfo& end = *itr;
		linkCenter= linkCenter+(end.mCenter);			
	}
	if(spanMap.empty())
		return;
	linkCenter *= (1.0/spanMap.size());
	std::sort(spanMap.begin(),spanMap.end(),StretchSideInfo::StretchOrder(linkCenter) );

	//convert height
	for(size_t i=0;i<spanMap.size();i++){ 

		StretchSideInfo& info = spanMap[i];
		info.mCenter= info.mCenter - levelCenter;
		info.mLeft= info.mLeft - levelCenter;
		info.mRight = info.mRight - levelCenter;
	}

}


void ParkingLotEntry3DNode::RenderEntryExit(ParkLotEntryExit& entexit,const CPoint2008& ptCenter,double _height)
{
	BOOL bEdit = IsInEditMode();
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
	pObj->clear();

	CPath2008& ctrlPath = entexit.m_line;
	if(ctrlPath.getCount()<2)return;
	
	ARCVector3 p1 = ctrlPath[0] - ptCenter;
	ARCVector3 p2 = ctrlPath[1] - ptCenter;
	p1[VZ] = 0;
	p2[VZ] = 0;

	ARCVector3  pDir = p1- p2;
	ARCVector3 pDriveDir = pDir.PerpendicularLCopy().Normalize();


	if(entexit.m_nInOutPort < 1) 
		entexit.m_nInOutPort = 2;

	for (int i = 0; i <= entexit.m_nInOutPort; i++)
	{
		CString sName = pObj->getName().c_str();
		sName.AppendFormat("-port%d", i);
		C3DNodeObject gateNode1 = OgreUtil::createOrRetrieveSceneNode(sName,GetScene());
		Ogre::Entity* gateEntity1 = OgreUtil::createOrRetrieveEntity(sName, PARK_GATE_MESH,GetScene());

		if (i == entexit.m_nInOutPort) {
			gateNode1.SetPosition(p2+i*pDir/entexit.m_nInOutPort+ pDriveDir*160);
			gateNode1.SetDirection(pDir, ARCVector3(0.0, 0.0, 1.0));
		}
		else {
			gateNode1.SetPosition(p2+i*pDir/entexit.m_nInOutPort);
			gateNode1.SetDirection(-pDir, ARCVector3(0.0, 0.0, 1.0));
		}
		gateNode1.AddObject(gateEntity1,1);
		gateNode1.AttachTo(*this);
	}


	ARCPath3 path;
	path.resize(2);
	path[0] = p1+pDriveDir*32;
	path[1] = p2+pDriveDir*32;
	ARCPipe drawPipe(path, PARK_ZEBRA_LINE_WIDTH);

	if(entexit.m_bSelected)
	{
		pObj->begin(PARK_ZEBRA_MAT_SELECTED,RenderOperation::OT_TRIANGLE_STRIP);
		entexit.m_bSelected=false;
	}else
	{
		pObj->begin(PARK_ZEBRA_MAT,RenderOperation::OT_TRIANGLE_STRIP);
	}
		
	double dMatWidth = PARK_ZEBRA_LINE_WIDTH/64;
	double dMatLen = drawPipe.m_centerPath.GetLength() / 256;
	Vector3 prepos1 = OGREVECTOR(drawPipe.m_sidePath1[0]);
	Vector3 prepos2 = OGREVECTOR(drawPipe.m_sidePath2[0]);

	Vector3 pos1 = OGREVECTOR(drawPipe.m_sidePath1[1]);
	Vector3 pos2 = OGREVECTOR(drawPipe.m_sidePath2[1]);
	Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

	pObj->position(prepos1);pObj->textureCoord(dMatWidth,0);pObj->normal(norm);
	pObj->position(prepos2);pObj->textureCoord(0,0);

	pObj->position(pos1);pObj->textureCoord(dMatWidth,dMatLen);
	pObj->position(pos2);pObj->textureCoord(0,dMatLen);
	pObj->end();

	
	//draw link to stretch
	StretchPortList spanMap;
	bool bHasStreet;
	GetSnapMap(entexit,spanMap,GetLandsideEditContext(),ptCenter,bHasStreet,_height );
	if(!spanMap.empty())
		LandIntersection3D::DrawLinkNodes(spanMap,pObj,bHasStreet,bEdit);		

	AddObject(pObj,2);
	
}
//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(LandsideParkingLot3D)

void LandsideParkingLot3D::SetQueryData( LandsideParkingLot* pLot )
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pLot);		
		pData->SetData(KeyLandsideItemType, _LayoutObject);
	}
}

LandsideParkingLot* LandsideParkingLot3D::getLot()
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		LandsideParkingLot* pLot  = NULL;
		pData->GetData(KeyAltObjectPointer,(int&)pLot);
		return pLot;
	}
	return NULL;
}



void LandsideParkingLot3D::OnUpdate3D()
{
	RemoveChild();
	ILayoutObject*pObj = getLayOutObject();
	if(pObj->GetType()!=ALT_LPARKINGLOT)
		return;
	LandsideParkingLot* pLot = (LandsideParkingLot*)pObj;
	//draw area	
	CPoint2008 ptCenter;
	if(!pLot->GetCenter(ptCenter))
		return;

	ILandsideEditContext* pCtx = GetLandsideEditContext();
	pCtx->ConvertToVisualPos(ptCenter);
	
	//update baselevel
	//CString sbaseLevel;
	//sbaseLevel.Format("%s-baselevel",GetIDName());
	//ParkingLotLevel3DNode baseleveNode = OgreUtil::createOrRetrieveSceneNode(sbaseLevel,GetScene());
	////OnUpdate3D();
	//baseleveNode.UpdateLevel(pLot,-1,ptCenter);	
	////baseleveNode.SetPosition(ARCVector3(0,0,pLot->m_baselevel.m_dHeight));
	//baseleveNode.AttachTo(*this);	
	//baseleveNode.SetVisible(pLot->m_baselevel.m_bVisible);
	
	//update other level
	for(int i=0;i<pLot->GetLevelCount();i++)
	{
		CString sOtherLevel;
		sOtherLevel.Format("%s-other level%d",GetIDName(),i);
		ParkingLotLevel3DNode otherlevelnode = OgreUtil::createOrRetrieveSceneNode(sOtherLevel,GetScene());
		otherlevelnode.UpdateLevel(pLot,(int)i, ptCenter);

		double dHeight = pCtx->GetVisualHeight(pLot->GetLevelHeight(i));

		otherlevelnode.SetPosition( ARCVector3(0,0,dHeight-ptCenter.z) );
		otherlevelnode.AttachTo(*this);
		otherlevelnode.SetVisible(pLot->GetLevel(i)->m_bVisible);

	}

	SetPosition(ptCenter);
}

//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(ParkingLotLevel3DNode)
//bool ParkingLotLevel3DNode::UpdateBaseLevel( LandsideParkingLot* pLot, CPoint2008& ptCenter,BOOL InEdit )
//{
//
//	
//	
//	return true;
//}

void ParkingLotLevel3DNode::UpdateLevel(LandsideParkingLot* pLot, int nLevelIndex, const CPoint2008& ptCenter )
{
// 	CPoint2008 ptCenter;
 	LandsideParkingLotLevel* level =  pLot->GetLevel(nLevelIndex);
	if(!level)
		return;
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx) return;

	SetQueryData(pLot,nLevelIndex);
	RemoveAllObject();
	RemoveChild();


	double dLevelVisualHeight = pCtx->GetVisualHeight(level->m_dHeight);

	BOOL InEdit = IsInEditMode();
	{//draw area

		ParkingLotArea3DNode areaNode= GetAreaNode();
		areaNode.Build3D(pLot,nLevelIndex,  ptCenter);
	}	

	{//draw park space
		for(int i=0;i<level->m_parkspaces.getCount();i++)
		{
			CString sName;
			sName.Format("%s-space(%d)",GetIDName(),i);
			ParkingLotParkingSpace3DNode space3DNode=OgreUtil::createOrRetrieveSceneNode(sName,GetScene());
			
			space3DNode.SetQueryData(pLot,nLevelIndex,i);
			space3DNode.Build3D(pLot,nLevelIndex,i,ptCenter);
			space3DNode.AttachTo(*this);			
		}		
	}
	//draw drive pipe
	{
		for (int i = 0; i < level->m_dirvepipes.getCount(); i++)
		{
			CString sName;
			sName.Format("%s-DrivePipe(%d)",GetIDName(), i);
			ParkingDrivePipe& pipe = level->m_dirvepipes.getPipe(i);

			ParkingLotDrivePipe3DNode pipe3DNode=OgreUtil::createOrRetrieveSceneNode(sName,GetScene());

			pipe3DNode.SetQueryData(pLot,nLevelIndex,i);
			pipe3DNode.Build3D(pLot,nLevelIndex,i,ptCenter,dLevelVisualHeight);			
			pipe3DNode.AttachTo(*this);			
		}

	}
	//draw entry exit
	{
		for(int i=0;i<level->m_entryexits.getCount();i++)
		{
			CString sName;
			sName.Format("%s-entry(%d)",GetIDName(),i);
			ParkingLotEntry3DNode entry3DNode= OgreUtil::createOrRetrieveSceneNode(sName,GetScene());
			entry3DNode.SetQueryData(pLot,nLevelIndex,i);
			entry3DNode.Buil3D(pLot,nLevelIndex,i, ptCenter);
			entry3DNode.AttachTo(*this);
		}
	}
	
}

void ParkingLotLevel3DNode::SetQueryData( LandsideParkingLot* pLot, int nLevel )
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pLot);		
		pData->SetData(KeyLandsideItemType, _LayoutObject);
		pData->SetData(KeyLevelIndex,nLevel);
	}
}

emLandsideItemType ParkingLotLevel3DNode::getType() const
{
	return _ParkingLotLevel;
}



void ParkingLotLevel3DNode::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideParkingLot* pLot = getLot();
	if(!pLot) return;	

	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLot, plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void ParkingLotLevel3DNode::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx =GetLandsideEditContext();
	if(!plandsideCtx)return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideParkingLot* pLot = getLot();
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
	pLot->DoOffset(ARCVector3(mousediff.x, mousediff.y, mousediff.z));

	plandsideCtx->NotifyViewCurentOperation();
}
void ParkingLotLevel3DNode::EndDrag()
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}
bool ParkingLotLevel3DNode::IsDraggable()const
{
	if(LandsideParkingLot* pParkingLot = getLot())
	{
		return !pParkingLot->GetLocked();
	}
	return false;
}

LandsideParkingLotLevel* ParkingLotLevel3DNode::getLevel()
{
	LandsideParkingLotLevel* pLevel = NULL;
	if(SceneNodeQueryData *pData = getQueryData())
	{
		int nLevelIndex;
		if( pData->GetData(KeyLevelIndex,nLevelIndex) ){
			pLevel =  getLot()->GetLevel(nLevelIndex);
			
		}
	}
	return pLevel;
}

CRenderScene3DNode ParkingLotLevel3DNode::GetAreaNode()
{
	CString sAreaNodeName = GetIDName()+"-area";
	CRenderScene3DNode node = OgreUtil::createOrRetrieveSceneNode(sAreaNodeName,GetScene());
	node.AttachTo(*this);
	return node;
}

//////////////////////////////////////////////////////////////////////////
static bool GetPosMouseDiff(const Vector3&ptworldPos,const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo, Vector3& mousediff  )
{
	BOOL bVertical = FALSE;

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
	return false;
}
PROTYPE_INSTANCE(ParkingLotParkingSpace3DNode)
emLandsideItemType ParkingLotParkingSpace3DNode::getType()const
{
	return _ParkingLotSpace;
}

void ParkingLotParkingSpace3DNode::Build3D(LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex,const CPoint2008& ptCenter)
{
	RemoveChild();
	LandsideParkingLotLevel* pLevel = pLot->GetLevel(nLevelIndex);
	if(!pLevel)
		return;
	ParkingSpace& space = pLevel->m_parkspaces.getSpace(iAreaIndex);
	if(space.m_ctrlPath.getCount()<2)
		return;

	//render object
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
	ManualObject* pselObj = OgreUtil::createOrRetrieveManualObject( GetIDName()+"select", GetScene());

	
	RenderSpace(space,iAreaIndex,pObj,pselObj,ptCenter);

	AddObject(pObj,0,true);
	AddObject(pselObj,2,false);
	space.m_bSelected = false;
	//OgreUtil::SetMaterial(pObj,PARK_SPACE_MAT);
	//pObj->setMaterialName(PARK_SPACE_MAT);
	//
	if(IsInEditMode())
	{
		CString s = GetIDName();
		s.Append(_T("CtrlPath"));
		ParkingSpaceControlPath3D ctrlpath = OgreUtil::createOrRetrieveSceneNode( s,GetScene());
		ctrlpath.Update3D(pLot,nLevelIndex,iAreaIndex,ptCenter);
		ctrlpath.AttachTo(*this);
	}
}

void ParkingLotParkingSpace3DNode::OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
{
	FlipEditMode();
	OnUpdate3D();
}
void ParkingLotParkingSpace3DNode::SetQueryData(LandsideParkingLot* pLot, int nLevel,int nIndex)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer,(int)pLot);
		pData->SetData(KeyLevelIndex,nLevel);
		pData->SetData(KeyControlPathIndex,nIndex);
		pData->SetData(KeyLandsideItemType, _LayoutObject);
		pData->SetData(KeyParkLotItemType, _ParkingLotSpace);
	}
}


//dynamic drag
void ParkingLotParkingSpace3DNode::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	if(LandsideParkingLot* pLot = getLot())
	{	
		SetInDragging(TRUE);
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLot,plandsideCtx->getInput());	
		plandsideCtx->StartEditOp(pModCmd);
		OnDrag(lastMouseInfo,curMouseInfo);
	}
}


void ParkingLotParkingSpace3DNode::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	//check 
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)	return;
	LandsideParkingLot* pLot = getLot();
	if(!pLot) return;

	SceneNodeQueryData *pData = getQueryData();
	if(!pData) return;

	int nSpaceIndex;
	int nLeveIndex;
	if(!pData->GetData(KeyLevelIndex,nLeveIndex))
		return;
	if(!pData->GetData(KeyControlPathIndex,nSpaceIndex))
		return;
	LandsideParkingLotLevel* level = pLot->GetLevel(nLeveIndex);
	if(!level)
		return;
	ParkingSpace* pSpace = level->m_parkspaces.getSpacePtr(nSpaceIndex);
	if(!pSpace)
		return;


	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
	CPath2008& path = pSpace->m_ctrlPath;
	{					
		path.DoOffset(mousediff.x,mousediff.y,0);
		OnUpdate3D();
		plandsideCtx->UpdateDrawing();
	}
		

	
}

void ParkingLotParkingSpace3DNode::EndDrag()
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx) return;	

	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();	
	SetInDragging(FALSE);
}
bool ParkingLotParkingSpace3DNode::IsDraggable()const
{
	return IsInEditMode()==TRUE;
}

void ParkingLotParkingSpace3DNode::OnUpdate3D()
{	
	if(SceneNodeQueryData* pData = getQueryData())
	{
		LandsideParkingLot* pLot = NULL;
		int nLevelIdx, nPipeIdx;
		CPoint2008 ptCenter;	
		
		if(!pData->GetData(KeyAltObjectPointer,(int&)pLot))
			return;
		if(!pData->GetData(KeyLevelIndex,nLevelIdx))
			return;
		if(!pData->GetData(KeyControlPathIndex,nPipeIdx))
			return;

		if(!pLot->GetCenter(ptCenter))
			return;
		
		LandsideParkingLotLevel* plevel = pLot->GetLevel(nLevelIdx);
		if(!plevel)		return;

		
		Build3D(pLot,nLevelIdx,nPipeIdx,ptCenter);
	}
}

void ParkingLotParkingSpace3DNode::RenderSpace( ParkingSpace& space, int iIndex, Ogre::ManualObject* pObj, ManualObject* pselObj, const CPoint2008& ptCenter )
{
	if(space.m_type == ParkingSpace::_parallel)
	{
		RenderPallaleSpace(iIndex,space,pObj,pselObj,ptCenter);
	}
	else
	{
		RenderAngleSpace(iIndex,space,pObj,pselObj,ptCenter);
	}
}

//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(ParkingLotEntry3DNode)
emLandsideItemType ParkingLotEntry3DNode::getType()const
{
	return _ParkingLotEntry;
}

void ParkingLotEntry3DNode::Buil3D(LandsideParkingLot* pLot,int nLevelIndex,int nEntryIndex, const CPoint2008& ptCenter)
{
	SetQueryData(pLot,nLevelIndex,nEntryIndex);
	RemoveChild();

	LandsideParkingLotLevel* pLevel = pLot->GetLevel(nLevelIndex);
	if(!pLevel)		return;
	ParkLotEntryExit* pEntry =  pLevel->m_entryexits.getEntryPtr(nEntryIndex);
	if(!pEntry)		return;
	ILandsideEditContext* pctx = GetLandsideEditContext();
	if(!pctx)  return;

    double dVisualH = pctx->GetVisualHeight(pLevel->m_dHeight);
	
	RenderEntryExit(*pEntry, ptCenter,dVisualH);
	

	if(IsInEditMode())
	{
		CString sParkSpacePath = GetIDName() + _T("CtrlPath");
		ParkingLotEntryControlPath3D ctrlpath3d = OgreUtil::createOrRetrieveSceneNode(sParkSpacePath,GetScene());
		ctrlpath3d.Update3D(pLot,nLevelIndex,nEntryIndex,ptCenter);
		ctrlpath3d.AttachTo(*this);
	}
}
void ParkingLotEntry3DNode::OnUpdateEditMode(BOOL b)
{
	SetEditMode(b);
	((LandsideParkingLot3D)(GetParent().GetParent())).OnUpdate3D();
}
void ParkingLotEntry3DNode::OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
{
	FlipEditMode();
}
void ParkingLotEntry3DNode::SetQueryData(LandsideParkingLot* pLot, int nLevel,int nIndex)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer,(int)pLot);
		pData->SetData(KeyLevelIndex,nLevel);
		pData->SetData(KeyControlPathIndex,nIndex);
		pData->SetData(KeyLandsideItemType, _LayoutObject);
		pData->SetData(KeyParkLotItemType, _ParkingLotEntry);
	}
}


//dynamic drag
void ParkingLotEntry3DNode::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	if (!IsInEditMode() || !IsDraggable())
	{
		return;
	}
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)return;

	if(LandsideParkingLot* pLot = getLot())
	{		
		SetInDragging(TRUE);
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLot,plandsideCtx->getInput());	
		plandsideCtx->StartEditOp(pModCmd);
		OnDrag(lastMouseInfo,curMouseInfo);
	}
}
void ParkingLotEntry3DNode::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	if(LandsideParkingLot* pLot = getLot())
	{		
		if(SceneNodeQueryData *pData = getQueryData())
		{
			ParkLotEntryExit *pEntry  = NULL;
			pData->GetData(KeyParkingLotEntry,(int&)pEntry);
			if(pEntry)
			{
				Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
				CPath2008& path = pEntry->m_line;
			/*	CPoint2008 orignPt = path[0];
				Vector3 ptworldPos = Vector3(orignPt.x,orignPt.y, orignPt.z);	

				if( GetPosMouseDiff(ptworldPos,lastMouseInfo,curMouseInfo,mousediff) )*/
				{					
					path.DoOffset(mousediff.x,mousediff.y,0);
					plandsideCtx->NotifyViewCurentOperation();
				}
			}		

		}
	}
}

void ParkingLotEntry3DNode::EndDrag()
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)return;	
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}
bool ParkingLotEntry3DNode::IsDraggable()const
{
	if(LandsideParkingLot* pParkingLot = getLot())
	{
		return !pParkingLot->GetLocked();
	}
	return false;
}

void ParkingLotEntry3DNode::OnUpdate3D()
{
	LandsideParkingLot* pLot = NULL;
	int nLevelIdx, nPipeIdx;

	if(SceneNodeQueryData* pData = getQueryData()){
		if(!pData->GetData(KeyAltObjectPointer,(int&)pLot))
			return;
		if(!pData->GetData(KeyLevelIndex,nLevelIdx))
			return;
		if(!pData->GetData(KeyControlPathIndex,nPipeIdx))
			return;

		CPoint2008 ptCenter = CPoint2008(GetParent().GetParent().GetPosition());

		Buil3D(pLot,nLevelIdx,nPipeIdx,ptCenter);
	}
}

//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(ParkingLotArea3DNode)
emLandsideItemType ParkingLotArea3DNode::getType()const
{
	return _ParkingLotArea;
}

void ParkingLotArea3DNode::OnUpdate3D() //draw selectable areas
{
	LandsideParkingLot* pLot = NULL;
	int nLevel =0 ;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		if(!pData->GetData(KeyAltObjectPointer,(int&)pLot))
			return;
		if(!pData->GetData(KeyLevelIndex,nLevel))
			return;
		if(!pLot)
			return;


		CRenderScene3DNode node = GetParent().GetParent(); 
		if(nLevel==-1){	
			node.OnUpdate3D();
			return;
		}

		CPoint2008 ptC = CPoint2008(node.GetPosition());
		Build3D(pLot,nLevel,ptC);
	}	
}


void ParkingLotArea3DNode::OnUpdateEditMode(BOOL b)
{
	SetEditMode(b);
	OnUpdate3D();	
}
void ParkingLotArea3DNode::OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
{
	FlipEditMode();
}
void ParkingLotArea3DNode::SetQueryData(LandsideParkingLot* pLot, int nLevel)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer,(int)pLot);
		pData->SetData(KeyLevelIndex,nLevel);
		//pData->SetData(KeyControlPathIndex,nIndex);
		pData->SetData(KeyLandsideItemType, _LayoutObject);
		pData->SetData(KeyParkLotItemType, _ParkingLotArea);
	}
}


//dynamic drag
void ParkingLotArea3DNode::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	if (!IsInEditMode() || !IsDraggable())
	{
		return;
	}
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)	return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	if(LandsideParkingLot* pLot = getLot())
	{		

		SetInDragging(TRUE);
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLot,plandsideCtx->getInput());	
		plandsideCtx->StartEditOp(pModCmd);
		OnDrag(lastMouseInfo,curMouseInfo);
	}
}
void ParkingLotArea3DNode::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	if(LandsideParkingLot* pLot = getLot())
	{		
		if(SceneNodeQueryData *pData = getQueryData())
		{
			ParkingLotAreas::Area *pArea  = NULL;
			pData->GetData(KeyParkingArea,(int&)pArea);
			if(pArea)
			{
				Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
				CPath2008& path = pArea->mpoly;
		/*		CPoint2008 orignPt = path[0];
				Vector3 ptworldPos = Vector3(orignPt.x,orignPt.y, orignPt.z);	

				if( GetPosMouseDiff(ptworldPos,lastMouseInfo,curMouseInfo,mousediff) )*/
				{					
					path.DoOffset(mousediff.x,mousediff.y,0);
					plandsideCtx->NotifyViewCurentOperation();
				}
			}		

		}
	}
}

void ParkingLotArea3DNode::EndDrag()
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)return;	
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}
bool ParkingLotArea3DNode::IsDraggable()const
{
	if(LandsideParkingLot* pParkingLot = getLot())
	{
		return !pParkingLot->GetLocked();
	}
	return false;

}


void ParkingLotArea3DNode::BuildArea(LandsideParkingLotLevel* plevel,const CPoint2008& ptCenter)
{
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
	pObj->clear();
	//use tesslator
	CTesselator visTess;
	CTesselator invisTess;
	{
		ARCVectorListList visOutline;
		ARCVectorListList invisOutline;
		for(int i=0;i<(int)plevel->m_areas.getCount();i++)
		{
			CPollygon2008 poly = plevel->m_areas.getArea(i).mpoly;
			poly.DoOffset(-ptCenter.x,-ptCenter.y,0);
			ARCVectorList vlist= CTesselator::GetAntiClockVectorList(poly);		
			if(plevel->m_areas.getArea(i).bVisble)
				visOutline.push_back(vlist);	
			else
				invisOutline.push_back(vlist);
		}
		visTess.MakeOutLine(visOutline,Vector3::UNIT_Z);
		invisTess.MakeOutLine(invisOutline,Vector3::UNIT_Z);
	}

	//all Merge tess
	CTesselator allMergeoutline;
	CTesselator allMergeMesh;
	CTesselator allMergeMeshBack;
	{
		ARCVectorListList visOutline;
		for(int i=0;i<visTess.mCurMesh.getCount();i++)
		{
			visOutline.push_back(visTess.mCurMesh.getSection(i)->m_vPoints );
		}
		for(int i=0;i<invisTess.mCurMesh.getCount();i++)
		{
			ARCVectorList vlist = invisTess.mCurMesh.getSection(i)->m_vPoints;
			std::reverse(vlist.begin(),vlist.end());
			visOutline.push_back( vlist );
		}
		allMergeoutline.MakeOutLine(visOutline,Vector3::UNIT_Z);
		allMergeMesh.MakeMeshPositive(visOutline,Vector3::UNIT_Z);
		allMergeMeshBack.MakeMeshNegative(visOutline,-Vector3::UNIT_Z);
	}
	double dThick =  plevel->m_dThickness;

	BOOL bInEdit= IsInEditMode();

	
	ColourValue regionColor(ARCColor3::GREY_VISIBLEREGION_FLOAT,ARCColor3::GREY_VISIBLEREGION_FLOAT,ARCColor3::GREY_VISIBLEREGION_FLOAT);
	//build obj
	allMergeMesh.BuildMeshToObject(pObj,bInEdit?PARK_AREA_MAT_EDIT:PARK_AREA_MAT,Vector3::UNIT_Z,regionColor,0); //top
	
	if(!bInEdit)
	{
		allMergeMeshBack.BuildMeshToObject(pObj,PARK_AREA_MAT,-Vector3::UNIT_Z,regionColor, -dThick); //bottom
		allMergeoutline.BuildSideToObject(pObj,PARK_AREA_MAT,regionColor,-dThick);
	}

	float c = 0.25;
	allMergeoutline.BuildOutlineToObject(pObj,FLOORMAT_OUTLINE,ColourValue(c,c,c), -dThick);
	AddObject(pObj,1);
}

void ParkingLotArea3DNode::Build3D( LandsideParkingLot* pLot, int nLevel,const CPoint2008& ptCenter )
{
	SetQueryData(pLot,nLevel);
	LandsideParkingLotLevel*plevel = pLot->GetLevel(nLevel);
	if(!plevel)
		return;

	BuildArea(plevel,ptCenter);
	RemoveChild();
	if(IsInEditMode())
	{
		for(int i=0;i<plevel->m_areas.getCount();i++){
			CString sAreaId = GetIDName();
			sAreaId.AppendFormat(_T("CtrlPath%d"),i);
			ParkingLotAreaControlPath3D ctrlpath3d = OgreUtil::createOrRetrieveSceneNode(sAreaId,GetScene());
			ctrlpath3d.Update3D(pLot,nLevel,i,ptCenter);
			ctrlpath3d.AttachTo(*this);
		}
	}
	
}



//ParkingLotAreas::Area * ParkingLotArea3DNode::getArea()
//{
//	LandsideParkingLotLevel* pLevel = NULL;
//	if(SceneNodeQueryData *pData = getQueryData())
//	{
//		int nLevelIndex;
//		if( pData->GetData(KeyLevelIndex,nLevelIndex) ){
//			pLevel =  getLot()->GetLevel(nLevelIndex);
//			int nIdx;
//			if( pData->GetData(KeyControlPathIndex,nIdx)){
//				return pLevel->m_areas.getAreaPtr(nIdx);
//			}
//		}
//	}
//	return NULL;
//}

//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(ParkingLotDrivePipe3DNode)

void ParkingLotDrivePipe3DNode::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	if (!IsInEditMode() || !IsDraggable())
	{
		return;
	}
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	if(LandsideParkingLot* pLot = getLot())
	{
		SetInDragging(TRUE);
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLot,plandsideCtx->getInput());	
		plandsideCtx->StartEditOp(pModCmd);
		OnDrag(lastMouseInfo,curMouseInfo);
	}
}

void ParkingLotDrivePipe3DNode::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)	return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	if(ParkingDrivePipe* pPipe = getPipe())
	{
		CPath2008& path = pPipe->m_ctrlPath;	
		Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
		path.DoOffset(mousediff.x,mousediff.y,0);
		OnUpdate3D();
		plandsideCtx->UpdateDrawing();
	}
}

void ParkingLotDrivePipe3DNode::EndDrag()
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}



emLandsideItemType ParkingLotDrivePipe3DNode::getType()const
{
	return _ParkingLotDrivePipe;
}

void ParkingLotDrivePipe3DNode::OnUpdate3D()
{
	LandsideParkingLot* pLot = NULL;
	int nLevelIdx, nPipeIdx;
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx) return;
	
	if(SceneNodeQueryData* pData = getQueryData())
	{
		if(!pData->GetData(KeyAltObjectPointer,(int&)pLot))
			return;
		if(!pData->GetData(KeyLevelIndex,nLevelIdx))
			return;
		if(!pData->GetData(KeyControlPathIndex,nPipeIdx))
			return;

		LandsideParkingLotLevel* plevel = pLot->GetLevel(nLevelIdx);
		if(!plevel) return;
	
		CPoint2008 ptCenter = CPoint2008(GetParent().GetParent().GetPosition());
		double dLevelVisualHeight = pCtx->GetVisualHeight(plevel->m_dHeight);

		Build3D(pLot,nLevelIdx,nPipeIdx,ptCenter,dLevelVisualHeight);
	}
}


void ParkingLotDrivePipe3DNode::SetQueryData(LandsideParkingLot* pLot, int nLevel,int nIndex)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer,(int)pLot);
		pData->SetData(KeyLevelIndex,nLevel);
		pData->SetData(KeyControlPathIndex,nIndex);
		pData->SetData(KeyLandsideItemType, _LayoutObject);
		pData->SetData(KeyParkLotItemType, _ParkingLotDrivePipe);
	}
}

//dynamic drag

bool ParkingLotDrivePipe3DNode::IsDraggable()const
{
	if(LandsideParkingLot* pParkingLot = getLot())
	{
		return !pParkingLot->GetLocked();
	}
	return false;
}

void ParkingLotDrivePipe3DNode::Build3D( LandsideParkingLot* pLot,int nLevelIndex,int nPipeIndex, const CPoint2008& ptCenter ,double dLevelHeight)
{
	SetQueryData(pLot,nLevelIndex,nPipeIndex);
	RemoveChild();

	LandsideParkingLotLevel* pLevel = pLot->GetLevel(nLevelIndex);
	if(!pLevel)
		return;
	ParkingDrivePipe* pPipe =  pLevel->m_dirvepipes.getPipePtr(nPipeIndex);
	if(!pPipe)
		return;
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx) return;

	//get pipe visual path
	CPath2008 drawPath = pPipe->GetPathRealZ(pLot,nLevelIndex);
	pCtx->ConvertToVisualPath(drawPath);
	drawPath.DoOffset(-ptCenter.x,-ptCenter.y,-dLevelHeight);
	
	//render 
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
	RenderDrivePipe(drawPath, *pPipe, pObj,pCtx->isLeftDrive(), IsInEditMode());	
	AddObject(pObj,2);
	pPipe->m_bSelected =false;
	//
	if (IsInEditMode())
	{
		CString sDrivePopeCtrlPt = GetIDName() + _T("CtrlPoint");
		ParkingLotDrivePipeControlPath3D ctrlpaht3d = OgreUtil::createOrRetrieveSceneNode(sDrivePopeCtrlPt, GetScene());
		drawPath.DoOffset(0,0,1);
		ctrlpaht3d.Update3D(pLot,nLevelIndex, nPipeIndex, drawPath);
		ctrlpaht3d.AttachTo(*this);
	}
}

void ParkingLotDrivePipe3DNode::OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
{
	FlipEditMode();
	OnUpdate3D();
}

ParkingDrivePipe* ParkingLotDrivePipe3DNode::getPipe()
{
	LandsideParkingLot* pLot = NULL;
	int nLevelIdx, nPipeIdx;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		if(!pData->GetData(KeyAltObjectPointer,(int&)pLot))
			return NULL;
		if(!pData->GetData(KeyLevelIndex,nLevelIdx))
			return NULL;
		if(!pData->GetData(KeyControlPathIndex,nPipeIdx))
			return NULL;

		LandsideParkingLotLevel* plevel = pLot->GetLevel(nLevelIdx);
		if(!plevel)
			return NULL;

		ParkingDrivePipe* pPipe =  plevel->m_dirvepipes.getPipePtr(nPipeIdx);
		return pPipe;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(ParkingLotAreaControlPath3D)
PROTYPE_INSTANCE(ParkinglotAreaControlPoint3D)

PROTYPE_INSTANCE(ParkingSpaceControlPath3D)
PROTYPE_INSTANCE(ParkingSpaceControlPoint3D)

PROTYPE_INSTANCE(ParkingLotEntryControlPath3D)
PROTYPE_INSTANCE(ParkingLotEntryControlPoint3D)

void ParkingLotControlPath3D::Update3D( LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex,const CPoint2008& ptCenter)
{
	//set data
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pLot);
		pData->SetData(KeyLevelIndex,nLevelIndex);
		pData->SetData(KeyControlPathIndex,iAreaIndex);
		pData->SetData(KeyLandsideItemType,getType());
	}

	///	x	-9.2559629542272619e+061	double

	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());	

	pObj->clear();
	CPath2008 path = getPath(pLot,nLevelIndex,iAreaIndex);
	path.DoOffset(-ptCenter.x,-ptCenter.y,-ptCenter.z);


	double dLevelHeight=pLot->GetLevelHeight(nLevelIndex);
	for (int i=0;i<path.getCount();i++)
	{
		int nLevelOffset=path[i].z>0?path[i].z+0.5:path[i].z-0.5;
		int nPointLevel=nLevelIndex+nLevelOffset;

		double dPointHeight = pLot->GetLevelHeight(nPointLevel);
		path[i].z  = dPointHeight - dLevelHeight;
	}

	CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,path,ARCColor3(255,0,0));
	AddObject(pObj,2);
	//update control points
	RemoveChild();
	UpdatePoints(pLot,nLevelIndex,iAreaIndex, path);
}

CRenderScene3DNode ParkingLotControlPath3D::GetControlPoint3D( int idx )
{
	CString sPt;
	sPt.Format(_T("%s%d"),GetIDName(),idx);
	return OgreUtil::createOrRetrieveSceneNode(sPt,GetScene());
}




void ParkingLotControlPoint3D::Update3D( LandsideParkingLot* pLot, int nLevelIndex,int nPathIndex, int iPtIdx ,CPath2008& path)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pLot);
		pData->SetData(KeyControlPathIndex,nPathIndex);
		pData->SetData(KeyLandsideItemType,getType());
		pData->SetData(KeyControlPointIndex,iPtIdx);
		pData->SetData(KeyLevelIndex,nLevelIndex);
		pData->SetData(KeyParkingLotCtrlPath,(int)&path);
	}
//
	Entity* pEnt =  OgreUtil::createOrRetrieveEntity(GetIDName(),SphereMesh,GetScene());

	ARCColor3 color = ARCColor3(255,0,0);	
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(color));
	AddObject(pEnt,2);
}
void ParkingLotControlPoint3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	if (!IsDraggable())
	{
		return;
	}
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)	return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	SetInDragging(TRUE);

	
	if(LandsideParkingLot* pLot = getLot())
	{		
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLot,plandsideCtx->getInput());	
		plandsideCtx->StartEditOp(pModCmd);
		OnDrag(lastMouseInfo,curMouseInfo);
	}
}

void ParkingLotControlPoint3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;


	if(LandsideParkingLot* pLot = getLot())
	{		
		int nPtIdx = getPointIndex();
		int nAreaIDx = getPathIndex();

		Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
		CPath2008& path = getPath(pLot,nAreaIDx);
		/*Vector3 mousediff ;
		CPath2008& path = getPath(pLot,nAreaIDx);
		CPoint2008 orignPt = path[nPtIdx];
		Vector3 ptworldPos = Vector3(orignPt.x,orignPt.y, orignPt.z);	

		if( GetPosMouseDiff(ptworldPos,lastMouseInfo,curMouseInfo,mousediff) )*/
		{
			path[nPtIdx] = path[nPtIdx]  + CPoint2008(mousediff.x,mousediff.y,0);
			//plandsideCtx->NotifyViewCurentOperation();
			CRenderScene3DNode node = GetParent().GetParent();
			node.OnUpdate3D();
			plandsideCtx->UpdateDrawing();
		}		
	}
}

emLandsideItemType ParkinglotAreaControlPoint3D::getType() const
{
	return _ParkingLotAreaPoint;
}

// CPath2008& ParkinglotAreaControlPoint3D::getPath( LandsideParkingLot* pLot,int nPathIndex )
// {
// 	return pLot->m_baselevel.m_areas.getArea(nPathIndex).mpoly;
// }

void ParkingLotControlPoint3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)
		return;	
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}
bool ParkingLotControlPoint3D::IsDraggable()const
{
	if(LandsideParkingLot* pParkingLot = getLot())
	{
		return !pParkingLot->GetLocked();
	}
	return false;

}
int ParkingLotControlPoint3D::getPointIndex() const
{
	int ptidx = 0;
	if(SceneNodeQueryData*pData = getQueryData())
	{
		pData->GetData(KeyControlPointIndex,ptidx);
	}
	return ptidx;
}

int ParkingLotControlPoint3D::getPathIndex() const
{
	int ptidx = 0;
	if(SceneNodeQueryData*pData = getQueryData())
	{
		pData->GetData(KeyControlPathIndex,ptidx);
	}
	return ptidx;
}

CPath2008& ParkingLotControlPoint3D::getPath( LandsideParkingLot* pLot,int nPathIndex )
{
// 	ASSERT(FALSE);

	if(SceneNodeQueryData *pData = getQueryData())
	{
		CPath2008 *path;
		pData->GetData(KeyParkingLotCtrlPath,(int&)path);
		return *path;
	}
	static CPath2008 path;
	return path;
}






//////////////////////////////////////////////////////////////////////////


emLandsideItemType ParkingSpaceControlPoint3D::getType() const
{
	return _ParkingLotSpacePoint;
}

// CPath2008& ParkingSpaceControlPoint3D::getPath( LandsideParkingLot* pLot,int nPathIndex )
// {
// 	return pLot->m_baselevel.m_parkspaces.getSpace(nPathIndex).m_ctrlPath;
// }



LandsideParkingLot* LandsideParkingLot3DItem::getLot()const
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		LandsideParkingLot* pLot  = NULL;
		pData->GetData(KeyAltObjectPointer,(int&)pLot);
		return pLot;
	}
	return NULL;
}



int LandsideParkingLot3DItem::getLevelIdx() const
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		int idx;
		pData->GetData(KeyLevelIndex,idx);
		return idx;
	}
	return -1;
}




emLandsideItemType ParkingLotEntryControlPoint3D::getType() const
{
	return _ParkingLotEntryPoint;
}

// CPath2008& ParkingLotEntryControlPoint3D::getPath( LandsideParkingLot* pLot,int nPathIndex )
// {
// 	return pLot->m_baselevel.m_entryexits.getEnt(nPathIndex).m_line;
// }

emLandsideItemType ParkingLotAreaControlPath3D::getType() const
{
	return _ParkingLotAreaPath;
}

CPath2008& ParkingLotAreaControlPath3D::getPath( LandsideParkingLot* pLot,int nLevelIndex,int nPathIndex )
{
	LandsideParkingLotLevel* pLevel = pLot->GetLevel(nLevelIndex);
	ASSERT(pLevel);
	ParkingLotAreas::Area* pArea = pLevel->m_areas.getAreaPtr(nPathIndex);
	ASSERT(pArea);
	return pArea->mpoly;

	/*if(nLevelIndex>-1&&nLevelIndex<=(int)pLot->m_otherLevels.size())
	{
		return pLot->m_otherLevels.at(nLevelIndex).m_areas.getArea(nPathIndex).mpoly;
	}
	else
	{
		return pLot->m_baselevel.m_areas.getArea(nPathIndex).mpoly;
	}*/
}

void ParkingLotAreaControlPath3D::Update3D(LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex, const CPoint2008& ptCenter)
{
	//set data
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pLot);
		pData->SetData(KeyLevelIndex,nLevelIndex);
		pData->SetData(KeyControlPathIndex,iAreaIndex);
		pData->SetData(KeyLandsideItemType,getType());
	}
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	///
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());	
	pObj->clear();
	CPath2008 path = getPath(pLot,nLevelIndex,iAreaIndex);
	//pCtx->ConvertToVisualPath(path);
	path.DoOffset(-ptCenter.x,-ptCenter.y,0);
	//CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,path,ARCColor3(255,0,0));
	
	pObj->begin(VERTEXCOLOR_LIGTHOFF, Ogre::RenderOperation::OT_LINE_STRIP);
		for (int i = 0; i < path.getCount(); i++)
		{
			pObj->position(path[i].x, path[i].y, path[i].z);
			pObj->colour(Ogre::ColourValue(1.0, 0, 0));
		}
		pObj->position(path[0].x, path[0].y, path[0].z);
		pObj->colour(Ogre::ColourValue(1.0, 0, 0));
	pObj->end();

	AddObject(pObj,2);
	//update control points
	RemoveChild();
	UpdatePoints(pLot,nLevelIndex,iAreaIndex, path);
}

void ParkingLotAreaControlPath3D::UpdatePoints(LandsideParkingLot* pLot,int nLevelIndex, int iAreaIndex,CPath2008& path )
{
	for(int i=0;i<path.getCount();i++)
	{		
		ParkinglotAreaControlPoint3D ctrlPt = GetControlPoint3D(i);		
		ctrlPt.Update3D(pLot,nLevelIndex,iAreaIndex,i,getPath(pLot,nLevelIndex,iAreaIndex));
		ctrlPt.SetPosition(path[i]);
		ctrlPt.AttachTo(*this);
	}
}
void ParkingLotAreaControlPath3D::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{
	const Ray& sceneray = mouseInfo.mSceneRay;
	Point Origin = Point(sceneray.getOrigin().x,sceneray.getOrigin().y,sceneray.getOrigin().z );
	Point Direction = Point(sceneray.getDirection().x,sceneray.getDirection().y,sceneray.getDirection().z);
	ARCRay ray(Origin,Direction);
	if (SceneNodeQueryData* pData = InitQueryData())
	{
		int pathIndex = -1;
		int levelIndex = -1;
		pData->GetData(KeyControlPathIndex,pathIndex);
		pData->GetData(KeyLevelIndex,levelIndex);
		CPath2008 path2008 = getPath(getLot(),levelIndex,pathIndex);
        if(path2008.getCount() == 0)
        {
            pData->SetData(KeyRClickWorldPos, ARCStringConvert::toString(ARCVector3(0.0f, 0.0f, 0.0f)));
        }
        path2008.insertPointAfterAt(path2008.getPoint(0), path2008.getCount()-1);
		Path path = path2008;
		DistanceRay3Path3 dsitraypath(ray,path);
		dsitraypath.GetSquared(0);
		double dPos =  dsitraypath.m_fPathParameter;	
		CPoint2008 worldpos = path2008.GetIndexPoint(dPos);	
		pData->SetData(KeyRClickWorldPos, ARCStringConvert::toString(ARCVector3(worldpos.x, worldpos.y, worldpos.z)));
	}
}

emLandsideItemType ParkingSpaceControlPath3D::getType() const
{
	return _ParkingLotSpacePath;
}

CPath2008& ParkingSpaceControlPath3D::getPath( LandsideParkingLot* pLot,int nLevelIndex,int nPathIndex )
{
	//if(nLevelIndex>-1&&nLevelIndex<(int)pLot->m_otherLevels.size())
	{
		return pLot->GetLevel(nLevelIndex)->m_parkspaces.getSpace(nPathIndex).m_ctrlPath;
	}/*else
	{
		return pLot->m_baselevel.m_parkspaces.getSpace(nPathIndex).m_ctrlPath;
	}*/
}

void ParkingSpaceControlPath3D::OnRButtonDown(const MouseSceneInfo& mouseInfo)
{
	const Ray& sceneray = mouseInfo.mSceneRay;
	Point Origin = Point(sceneray.getOrigin().x,sceneray.getOrigin().y,sceneray.getOrigin().z );
	Point Direction = Point(sceneray.getDirection().x,sceneray.getDirection().y,sceneray.getDirection().z);
	ARCRay ray(Origin,Direction);
	if (SceneNodeQueryData* pData = InitQueryData())
	{
		int pathIndex = -1;
		int levelIndex = -1;
		pData->GetData(KeyControlPathIndex,pathIndex);
		pData->GetData(KeyLevelIndex,levelIndex);
		const CPath2008& path2008 = getPath(getLot(),levelIndex,pathIndex);
		Path path = path2008;
		DistanceRay3Path3 dsitraypath(ray,path);
		dsitraypath.GetSquared(0);
		double dPos =  dsitraypath.m_fPathParameter;	
		CPoint2008 worldpos = path2008.GetIndexPoint(dPos);	
		pData->SetData(KeyRClickWorldPos, ARCStringConvert::toString(ARCVector3(worldpos.x, worldpos.y, worldpos.z)));
	}
}

void ParkingSpaceControlPath3D::UpdatePoints( LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex,CPath2008& path )
{
	for(int i=0;i<path.getCount();i++)
	{		
		ParkingSpaceControlPoint3D ctrlPt = GetControlPoint3D(i);		
		ctrlPt.Update3D(pLot,nLevelIndex,iAreaIndex,i,getPath(pLot,nLevelIndex,iAreaIndex));
		ctrlPt.SetPosition(path[i]);
		ctrlPt.AttachTo(*this);
	}
}
//////////////////////////////////////////////////////////////////////////
//drive pipe
PROTYPE_INSTANCE(ParkingLotDrivePipeControlPath3D)
emLandsideItemType ParkingLotDrivePipeControlPath3D::getType() const
{
	return _ParkingLotDrivePipePath;
}

CPath2008& ParkingLotDrivePipeControlPath3D::getPath(LandsideParkingLot* pLot,int nLevelIndex,int nPipeIndex)
{
	//if(nLevelIndex>-1 && nLevelIndex<(int)pLot->m_otherLevels.size())
	{
		return pLot->GetLevel(nLevelIndex)->m_dirvepipes.getPipe(nPipeIndex).m_ctrlPath;
	}/*else
	{
		return pLot->m_baselevel.m_dirvepipes.getPipe(nPipeIndex).m_ctrlPath;

	}*/
}
void ParkingLotDrivePipeControlPath3D::UpdatePoints(LandsideParkingLot* pLot,int nLevelIndex, int nPipeIndex,CPath2008& path )
{
	for (int i = 0; i < path.getCount(); i++)
	{
		ParkingLotDrivePipeControlPoint3D ctrlPt = GetControlPoint3D(i);
		ctrlPt.Update3D(pLot,nLevelIndex, nPipeIndex, i,getPath(pLot,nLevelIndex,nPipeIndex));
		ctrlPt.SetPosition(path[i]);
		ctrlPt.AttachTo(*this);
	}
}

void ParkingLotDrivePipeControlPath3D::UpdatePoints(LandsideParkingLot* pLot,int nLevelIndex, int nPipeIndex,const CPath2008& path )
{
	for (int i = 0; i < path.getCount(); i++)
	{
		ParkingLotDrivePipeControlPoint3D ctrlPt = GetControlPoint3D(i);
		ctrlPt.Update3D(pLot,nLevelIndex, nPipeIndex, i,getPath(pLot,nLevelIndex,nPipeIndex));
		ctrlPt.SetPosition(path[i]);
		ctrlPt.AttachTo(*this);
	}
}

void ParkingLotDrivePipeControlPath3D::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{
	const Ray& sceneray = mouseInfo.mSceneRay;
	Point Origin = Point(sceneray.getOrigin().x,sceneray.getOrigin().y,sceneray.getOrigin().z );
	Point Direction = Point(sceneray.getDirection().x,sceneray.getDirection().y,sceneray.getDirection().z);
	ARCRay ray(Origin,Direction);
	if (SceneNodeQueryData* pData = InitQueryData())
	{
		int pathIndex = -1;
		int levelIndex = -1;
		pData->GetData(KeyControlPathIndex,pathIndex);
		pData->GetData(KeyLevelIndex,levelIndex);
		const CPath2008& path2008 = getPath(getLot(),levelIndex,pathIndex);
		Path path = path2008;
		DistanceRay3Path3 dsitraypath(ray,path);
		dsitraypath.GetSquared(0);
		double dPos =  dsitraypath.m_fPathParameter;	
		CPoint2008 worldpos = path2008.GetIndexPoint(dPos);	
		pData->SetData(KeyRClickWorldPos, ARCStringConvert::toString(ARCVector3(worldpos.x, worldpos.y, worldpos.z)));
	}
}

void ParkingLotDrivePipeControlPath3D::Update3D( LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex, const CPath2008& drawPath )
{
	//set data
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pLot);
		pData->SetData(KeyLevelIndex,nLevelIndex);
		pData->SetData(KeyControlPathIndex,iAreaIndex);
		pData->SetData(KeyLandsideItemType,getType());
	}
	///
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());	
	pObj->clear();
	CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,drawPath,ARCColor3(255,0,0));
	AddObject(pObj,2);
	//update control points
	RemoveChild();
	UpdatePoints(pLot,nLevelIndex,iAreaIndex, drawPath);

}

PROTYPE_INSTANCE(ParkingLotDrivePipeControlPoint3D)
emLandsideItemType ParkingLotDrivePipeControlPoint3D::getType() const
{
	return _ParkingLotDrivePipePoint;
}

// CPath2008& ParkingLotDrivePipeControlPoint3D::getPath( LandsideParkingLot* pLot,int nPipeIndex )
// {
// 	return pLot->m_baselevel.m_dirvepipes.getPipe(nPipeIndex).m_ctrlPath;
// }
//////////////////////////////////////////////////////////////////////////
emLandsideItemType ParkingLotEntryControlPath3D::getType() const
{
	return _ParkingLotEntryPath;
}
CPath2008& ParkingLotEntryControlPath3D::getPath( LandsideParkingLot* pLot,int nLevelIndex,int nPathIndex )
{
	//if (nLevelIndex>-1 && nLevelIndex<(int)pLot->m_otherLevels.size())
	{
		return pLot->GetLevel(nLevelIndex)->m_entryexits.getEnt(nPathIndex).m_line;
	}/*else
	{
		return pLot->m_baselevel.m_entryexits.getEnt(nPathIndex).m_line;
	}*/
}


void ParkingLotEntryControlPath3D::UpdatePoints( LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex,CPath2008& path )
{
	for(int i=0;i<path.getCount();i++)
	{		
		ParkingLotEntryControlPoint3D ctrlPt = GetControlPoint3D(i);		
		ctrlPt.Update3D(pLot,nLevelIndex,iAreaIndex,i,getPath(pLot,nLevelIndex,iAreaIndex));
		ctrlPt.SetPosition(path[i]);
		ctrlPt.AttachTo(*this);
	}
}


