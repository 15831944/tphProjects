#include "stdafx.h"

#include "OgreConvert.h"

#include "Tesselator.h"
#include <Common/path.h>
#include <CommonData/Shape.h>
#include <CommonData/ProcessorProp.h>
#include <CommonData/ProcessorQueueBase.h>
#include "ShapeBuilder.h"
#include "3DProcessorNode.h"
#include "RenderProcessor3D.h"
#include "Render3DScene.h"
#include "RenderBillboard3D.h"
#include "RenderEscalator3D.h"
#include "RenderIntegratedStation3D.h"
#include "RenderConvey3D.h"
#include "RenderElevator3D.h"
#include "RenderMovingSideWalk3D.h"
#include "RenderBridge3D.h"
#include "RenderStairProc3D.h"
#include "renderfloorchangeProc3D.h"
#include "Render3DSceneTags.h"
#include "OgreBillboardObject.h"
#include "ogreExt/ManualObjectExt.h"
using namespace Ogre;



void C3DProcessorNode::Setup3D( CProcessor2Base* pProc, int idx )
{
	
	if (ProcessorProp* pProcProp = pProc->GetProcessorProp())
	{
		switch (pProcProp->getProcessorType())
		{
		case BillboardProcessor:
			{
				CRenderBillboard3D  node = *this;
				node.Setup3D(pProc,idx);
			}
			break;
		case BridgeConnectorProc:
			{
				CRenderBridge3D node = *this;
				node.Setup3D(pProc,idx);
			}
			break;
		case ConveyorProc:
			{
				CRenderConvey3D node = *this;
				node.Setup3D(pProc,idx);
			}
			break;
		case Elevator :
			{
				CRenderElevator3D node = *this;
				node.Setup3D(pProc,idx);
			}
			break;		
		case IntegratedStationProc:
			{
				CRenderIntegratedStation3D node = *this;
				node.Setup3D(pProc,idx);
			}
			break;
		case MovingSideWalkProc :
			{
				CRenderMovingSideWalk3D node = *this;
				node.Setup3D(pProc,idx);
			}
			break;
		case EscalatorProc:
			{
				CRenderEscalator3D node = *this;
				node.Setup3D(pProc,idx);
			}
			break;	
		case FloorChangeProc:
			{
				CRenderFloorChangeProc3D node = *this;
				node.Setup3D(pProc,idx);
			}
			break;
		case StairProc:
			{
				CRenderStairProc3D node = *this;
				node.Setup3D(pProc,idx);
			}
			break;
		default:
			{
				CRenderProcessor3D node = *this;
				node.Setup3D(pProc,idx);
			}
			break;
		}
	}
	else
	{
		CRenderProcessor3D node = *this;
		node.Setup3D(pProc,idx);
	}

	//show text
	C3DRootSceneNode root  = GetRoot();
	CRender3DSceneTags* pTagMan = root.GetTagManager();

	BOOL showText = pProc->m_dispProperties.bDisplay[PDP_DISP_PROCNAME];
	const ARCColor3& color =  pProc->m_dispProperties.color[PDP_DISP_PROCNAME];
	//
	CString procName = pProc->GetProcessorName();
	if(ProcessorProp* pProcProp = pProc->GetProcessorProp())
	{
		procName = pProcProp->getIDName();
	}
	else
	{
		procName = pProc->ShapeName().GetIDString();
	}


	if(showText)
	{
		pTagMan->m_processorTags.SetStaticLayoutTag(idx,m_pNode, procName, color);
		pTagMan->m_processorTags.Show(idx,true);
	}
	
}
