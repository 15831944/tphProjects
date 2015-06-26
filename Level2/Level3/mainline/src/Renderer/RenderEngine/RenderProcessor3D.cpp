#include "stdafx.h"
#include "OgreConvert.h"

#include "Tesselator.h"
#include <Common/path.h>
#include <CommonData/Shape.h>
#include <CommonData/ProcessorProp.h>
#include <CommonData/ProcessorQueueBase.h>
#include "ShapeBuilder.h"
#include "RenderProcessor3D.h"

using namespace Ogre;


// Materials defined Databases\media\Shapes\ShapesCommon.material
static CString GetMeshFileName(LPCTSTR originFileName)
{
	LPCTSTR pDotPos = originFileName;
	LPCTSTR pDirPos = originFileName;
	while (*originFileName)
	{
		if ('\\' == *originFileName || '/' == *originFileName)
		{
			pDirPos = originFileName + 1;
		}
		else if ('.' == *originFileName)
		{
			pDotPos = originFileName;
		}
		originFileName++;
	}
	if (pDotPos<pDirPos)
	{
		pDotPos = originFileName;
	}
	return CString(pDirPos, pDotPos - pDirPos) + _T(".mesh");
}

void CRenderProcessor3D::Setup3D(CProcessor2Base* pProc, int idx)
{

	ARCVector3 center = pProc->GetLocation();
	SetPosition(center);

	const CProcessor2Base::CProcDispProperties& pdp = pProc->m_dispProperties;

	if (!pdp.bDisplay[PDP_DISP_SHAPE])
	{
		RemoveFromParent();
	}
	else
	{
		ARCColor3 stateOffColor(128,128,128);
		ARCColor3 cDisplay;
		cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_SHAPE] : stateOffColor;

		CShape* pShape = pProc->GetShape();
		if (pShape)
		{
			C3DSceneNode m_shape = GetCreateChildNode(GetName()+"-Shape");			
			Entity * pEnt = OgreUtil::createOrRetrieveEntity( m_shape.GetName(), GetMeshFileName(pShape->ShapeFileName()), getScene() );
			if (pEnt)
			{
				//CString strShapeMat = GetIDName() + _T("-ShapeMaterial");
				MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(/*pdp.color[PDP_DISP_SHAPE]*/cDisplay);
				if(!matPtr.isNull())
				{
					pEnt->setMaterial(matPtr);
				}
				m_shape.SetPosition(pProc->GetLocation()-center);
				m_shape.SetRotation(ARCVector3(0.0, 0.0, pProc->GetRotation()));
				m_shape.SetScale(pProc->GetScale());
				m_shape.AddObject(pEnt);				
			}
		}
	}


	UpdateNoShape(pProc,DRAWALL);
}


void CRenderProcessor3D::_updateNoShape( CProcessor2Base* pProc,C3DSceneNode& node, UINT nDrawMask )
{
	const CProcessor2Base::CProcDispProperties& pdp = pProc->m_dispProperties;
	ManualObject* pObj = node._GetOrCreateManualObject(node.GetName());
	pObj->clear();
	ProcessorProp* pProcProp = pProc->GetProcessorProp();
	if (pProcProp)
	{
		ARCColor3 stateOffColor(128,128,128);
		if (pdp.bDisplay[PDP_DISP_SERVLOC] && (nDrawMask & DRAWSERVLOC) )
		{
			ARCColor3 cPathDisplay;
			cPathDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_SERVLOC] : stateOffColor;

			Path drawpath = *pProcProp->serviceLocationPath();
			ProcessorBuilder::DrawPath(pObj, pProcProp->serviceLocationPath(), OgreConvert::GetColor(cPathDisplay/*pdp.color[PDP_DISP_SERVLOC]*/),0, pProcProp->getProcessorType() == HoldAreaProc);

			if( pProcProp->getProcessorType() == BaggageProc )
			{
				Path* path = pProcProp->GetBarrierPath();
				if( path )
				{	
					ARCColor3 cDisplay;
					cDisplay = pProc->GetProcessorStateOffTag() ? ARCColor3(0,0,0) : stateOffColor;
					ProcessorBuilder::DrawPath( pObj, path, /*ColourValue::Black*/OgreConvert::GetColor(cDisplay) ,0, true );
				}

				path = pProcProp->GetPathBagMovingOn();
				if( path )
				{	
					ARCColor3 cDisplay;
					cDisplay = pProc->GetProcessorStateOffTag() ? ARCColor3(128,64,64) : stateOffColor;
					ProcessorBuilder::DrawPath( pObj, path, /*ColourValue(128/225.0f,64/225.0f,64/225.0f)*/OgreConvert::GetColor(cDisplay) ,0, true );
				}
			}				
		}
		if (pdp.bDisplay[PDP_DISP_INC]&& (nDrawMask & DRAWINC))
		{
			ARCColor3 cPathDisplay;
			cPathDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_INC] : stateOffColor;
			ProcessorBuilder::DrawPath(pObj, pProcProp->inConstraint(), OgreConvert::GetColor(/*pdp.color[PDP_DISP_INC]*/cPathDisplay));
			ProcessorBuilder::DrawHeadArrow(pObj, pProcProp->inConstraint(), OgreConvert::GetColor(/*pdp.color[PDP_DISP_INC]*/cPathDisplay));
		}
		if (pdp.bDisplay[PDP_DISP_OUTC]&& (nDrawMask & DRAWOUTC))
		{
			ARCColor3 cPathDisplay;
			cPathDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_OUTC] : stateOffColor;
			ProcessorBuilder::DrawPath(pObj, pProcProp->outConstraint(), OgreConvert::GetColor(/*pdp.color[PDP_DISP_OUTC]*/cPathDisplay));
			ProcessorBuilder::DrawHeadArrow(pObj, pProcProp->outConstraint(), OgreConvert::GetColor(/*pdp.color[PDP_DISP_OUTC]*/cPathDisplay));
		}
		ProcessorQueueBase* pQueue = pProcProp->GetQueue();
		if (pQueue && pdp.bDisplay[PDP_DISP_QUEUE] && (nDrawMask & DRAWQUEUE))
		{
			pQueue->isFixed(); // Benny Tag
			ARCColor3 cPathDisplay;
			cPathDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_QUEUE] : stateOffColor;
			ProcessorBuilder::DrawPath(pObj, pQueue->corner(), OgreConvert::GetColor(/*pdp.color[PDP_DISP_QUEUE]*/cPathDisplay));
			ProcessorBuilder::DrawTailArrow(pObj, pQueue->corner(), OgreConvert::GetColor(/*pdp.color[PDP_DISP_QUEUE]*/cPathDisplay));
		}
	}
	node.AddObject(pObj);
	//UpdateSelected(GetSelected());

}

void CRenderProcessor3D::UpdateNoShape( CProcessor2Base* pProc, UINT nDrawMask/*=DRAWALL*/ )
{
	ARCVector3 center = pProc->GetLocation();

	C3DSceneNode noshape = GetCreateChildNode(GetName()+_T("NoShape"));
	noshape.SetPosition(-center);
	_updateNoShape(pProc,noshape,nDrawMask);
}

void CRenderProcessor3D::Update3D( CObjectDisplay* pDispObj )
{

}

