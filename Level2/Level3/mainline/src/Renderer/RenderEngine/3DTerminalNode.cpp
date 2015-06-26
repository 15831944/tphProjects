#include "stdafx.h"
#include "3DTerminalNode.h"


#include <common/IEnvModeEditContext.h>
#include "3DFloorNode.h"
#include "CommonData/PipeBase.h"
#include "common/IARCportLayoutEditContext.h"
#include "3DProcessorNode.h"
#include "RenderStructure3D.h"
#include "RenderPipe3D.h"
#include "RenderAreaPortal3D.h"
#include "RenderWallShape3D.h"
#include "Engine/RailwayInfo.h"
#include "MaterialDef.h"
#include "CommonData\ProcessorProp.h"

using namespace Ogre;

template <class DataType,class _3DType>
void C3DTerminalNode::SetupObjectList( const CRenderFloorList& flist, IARCportLayoutEditContext* pEditContext )
{
	std::vector<void*> vecData ;
	if (pEditContext->OnQueryDataObjectList(vecData,DataType::getTypeGUID()))
	{
		for(int i=0;i<(int)vecData.size();i++)
		{
			DataType* pipe = (DataType*)vecData.at(i);
			int floorIndx = pipe->GetFloorIndex();
			if(floorIndx < (int)flist.size()  && floorIndx>=0)
			{
				CRenderFloor* pFloor = flist.at(floorIndx);
				C3DFloorNode floorNode  = GetCreateChildNode(pFloor->GetUID());

				_3DType pipe3D = floorNode.GetCreateChildNode(pipe->getGuid());
				pipe3D.Setup3D(pipe,(int)i);
			}		
		}
		
	}
}




void C3DTerminalNode::UpdateAll( IARCportLayoutEditContext* pEditContext)
{
	ITerminalEditContext* terminal = pEditContext->GetTerminalContext();
	if(!terminal){
		return ;
	}	
	//update floors
	CRenderFloorList flist = terminal->GetFloorList();
	for(size_t i=0;i<flist.size();++i){
		CRenderFloor* floor = flist.at(i);
		C3DFloorNode floorNode  = GetCreateChildNode(floor->GetUID());
		floorNode.UpdateAll(floor, terminal==pEditContext->GetCurContext());
	}

	//update pipe //floor object
	SetupObjectList<CPipeBase,CRenderPipe3D>(flist,pEditContext);
	SetupObjectList<CAreaBase,CRenderArea3D>(flist,pEditContext);
	SetupObjectList<CPortalBase,CRenderPortal3D>(flist,pEditContext);
	//SetupObjectList<CProcessor2Base,C3DProcessorNode>(flist,pEditContext);
	UpdateTerminalProcssor(pEditContext);
	SetupObjectList<WallShapeBase,CRenderWallShape3D>(flist,pEditContext);
	//SetupObjectList<CStructureBase,CRenderStructure3D>(flist,pEditContext);
	UpdateTerminalStructureBase(pEditContext);
	UpdateRailway(pEditContext);
}

void C3DTerminalNode::UpdateFloorAltitude( const CString& floorId, double d )
{
	C3DFloorNode floorNode  = GetCreateChildNode(floorId);
	floorNode.UpdateAltitude(d);
}

void C3DTerminalNode::UpdateRailway(IARCportLayoutEditContext* pEditContext)
{
	//{
	//	BOOL bInMode= (GetEditContext()->toTerminal()!=NULL);
	//
	//	UpdateRailway();
	//	UpdateDrawing();
	//}
	ITerminalEditContext* terminal = pEditContext->GetTerminalContext();
	if(!terminal){	return ;	}	

	std::vector<RailwayInfo*> railWayVect;
	if(!terminal->GetAllRailWayInfo(railWayVect))
		return;

	//SceneManager* pScene = getSceneManager();
	//if (pScene == NULL)
	//	return;

	ManualObject* pObj = _GetOrCreateManualObject("Railway");// OgreUtil::createOrRetrieveManualObject("Railway",pScene);
	if (pObj == NULL)
		return;


	std::vector<DistanceUnit> dAlt;
	std::vector<BOOL> bOn;

	terminal->GetFloorAltOn(dAlt,bOn);
	
	int nFloorCount = (int)dAlt.size();
	

	pObj->clear();
	ColourValue color(0/255.0f, 0/255.0f, 51/255.0f);
	for(int i=0; i<static_cast<int>(railWayVect.size()); i++) {
		if(railWayVect[i]->IsPathDefined()) {
			const Path& path = railWayVect[i]->GetRailWayPath();
			Point* pts = path.getPointList();
			int nIndexInFloor=static_cast<int>(pts[0].getZ()/SCALE_FACTOR);
			if(!bOn[nIndexInFloor]) continue;
			int c = path.getCount();

			double dRealAlt,dAltDiff1;
			pObj->begin(VERTEXCOLOR_LIGTHOFF,RenderOperation::OT_LINE_STRIP);
			for(int j=0; j<c; j++) {
				int nIndexInFloor1=static_cast<int>(pts[j].getZ()/SCALE_FACTOR);
				double mult = (pts[j].getZ()/SCALE_FACTOR)-nIndexInFloor1;
				if(nIndexInFloor1<nFloorCount-1) {
					dAltDiff1=dAlt[nIndexInFloor1+1]-dAlt[nIndexInFloor1];
				}
				else {
					dAltDiff1=1000.0;
				}
				dRealAlt=dAlt[nIndexInFloor1]+mult*dAltDiff1;
				pObj->position(pts[j].getX(), pts[j].getY(), dRealAlt);	
				pObj->colour(color);
			}
			pObj->end();
			for(int j=0; j<c; j++)
			{
				int nIndexInFloor1=static_cast<int>(pts[j].getZ()/SCALE_FACTOR);
				double mult = (pts[j].getZ()/SCALE_FACTOR)-nIndexInFloor1;
				if(nIndexInFloor1<nFloorCount-1) {
					dAltDiff1=dAlt[nIndexInFloor1+1]-dAlt[nIndexInFloor1];
				}
				else {
					dAltDiff1=1000.0;
				}
				dRealAlt=dAlt[nIndexInFloor1]+mult*dAltDiff1;
				pObj->begin(VERTEXCOLOR_LIGTHOFF,RenderOperation::OT_TRIANGLE_FAN);
				//draw flat square
				{
					pObj->normal(0.0,0.0,1.0);
					pObj->position(pts[j].getX()-50, pts[j].getY()-50,dRealAlt);
					pObj->colour(color);
					pObj->position(pts[j].getX()+50, pts[j].getY()-50, dRealAlt);
					pObj->position(pts[j].getX()+50, pts[j].getY()+50, dRealAlt);
					pObj->position(pts[j].getX()-50, pts[j].getY()+50, dRealAlt);
				}
				pObj->end();
			}
		}
	}

	AddObject(pObj);
	//C3DNodeObject rootNode = GetRoot();
	//rootNode.AddObject(pObj);
}

void C3DTerminalNode::UpdateTerminalProcssor( IARCportLayoutEditContext* pEditContext )
{
	ITerminalEditContext* terminal = pEditContext->GetTerminalContext();
	if(!terminal)
		return ;

	CRenderFloorList flist = terminal->GetFloorList();

	std::vector<void*> vecData ;
	if (pEditContext->OnQueryDataObjectList(vecData,CProcessor2Base::getTypeGUID()))
	{
		for(int i=0;i<(int)vecData.size();i++)
		{
			CProcessor2Base* ProBase = (CProcessor2Base*)vecData.at(i);
			int floorIndx = ProBase->GetFloorIndex();
			if(floorIndx < (int)flist.size()  && floorIndx>=0)
			{
				CRenderFloor* pFloor = flist.at(floorIndx);
				C3DFloorNode floorNode  = GetCreateChildNode(pFloor->GetUID());

				C3DProcessorNode pipe3D = floorNode.GetCreateChildNode(ProBase->getGuid());
				if(!ProBase->GetProcessorProp())
					continue;
				int nProType = ProBase->GetProcessorProp()->getProcessorType();
				if (!terminal->isTerminalProcessorDisplay(nProType))
				{
					pipe3D.RemoveFromParent();
					continue;
				}
				pipe3D.Setup3D(ProBase,(int)i);
			}		
		}
	}
}

void C3DTerminalNode::UpdateTerminalStructureBase( IARCportLayoutEditContext* pEditContext )
{
	ITerminalEditContext* terminal = pEditContext->GetTerminalContext();
	if(!terminal)
		return ;

	CRenderFloorList flist = terminal->GetFloorList();

	std::vector<void*> vecData ;
	if (pEditContext->OnQueryDataObjectList(vecData,CStructureBase::getTypeGUID()))
	{
		for(int i=0;i<(int)vecData.size();i++)
		{
			CStructureBase* ProBase = (CStructureBase*)vecData.at(i);
			int floorIndx = ProBase->GetFloorIndex();
			if(floorIndx < (int)flist.size()  && floorIndx>=0)
			{
				CRenderFloor* pFloor = flist.at(floorIndx);
				C3DFloorNode floorNode  = GetCreateChildNode(pFloor->GetUID());

				CRenderStructure3D pipe3D = floorNode.GetCreateChildNode(ProBase->getGuid());
				CString texturefilename = terminal->GetTerminalStructureBaseFilename(ProBase->getTexture());
				pipe3D.Setup3D(ProBase,(int)i,texturefilename);
			}		
		}
	}
}

