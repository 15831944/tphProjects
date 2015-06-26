#include "stdafx.h"
#include ".\renderconvey3d.h"
#include "Render3DScene.h"
#include <CommonData/ProcessorProp.h>
#include <CommonData/ExtraProcProp.h>
#include "common/IARCportLayoutEditContext.h"
#include "common/IEnvModeEditContext.h"
#include "shapebuilder.h"

#define MAT_CONVRY _T("processor/convey")
using namespace Ogre;
void CRenderConvey3D::Update3D( CProcessor2Base* pProc)
{
	ProcessorProp* pProcProp = pProc->GetProcessorProp();
	ASSERT(pProcProp && pProcProp->getProcessorType() == ConveyorProc);
	ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
	ASSERT(pExtraProp && pExtraProp->GetPropType() == ExtraProcProp::Conveyor_Prop_Type);
	ConveyorProp* pConvey = (ConveyorProp*)pExtraProp;

	Path* floorPath = pProcProp->serviceLocationPath();
	if(!floorPath)
		return;
	const CProcessor2Base::CProcDispProperties& pdp = pProc->m_dispProperties;
	ARCVector3 center = GetWorldPosition();

	ITerminalEditContext* pCtxTerminal =   GetRoot().GetModel()->GetTerminalContext();// m_p3DScene->mpEditor->GetContext(EnvMode_Terminal)->toTerminal();
	ASSERT(pCtxTerminal);
	ARCPath3 drawpath = pCtxTerminal->GetTerminalVisualPath(*floorPath);
	drawpath.DoOffset(-center);

	ManualObject* pObj = _GetOrCreateManualObject(GetName());//OgreUtil::createOrRetrieveManualObject(GetIDName()+_T("/Shape"),GetScene());
	if(!pObj)
		return;
	pObj->clear();
	if(pdp.bDisplay[PDP_DISP_SHAPE])
	{
		ARCColor3 stateOffColor(128,128,128);
		ARCColor3 cDisplay;
		cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_SHAPE] : stateOffColor;
		Ogre::ColourValue clr = OGRECOLOR(/*pdp.color[PDP_DISP_SHAPE]*/cDisplay);
		ProcessorBuilder::DrawTexturePath(pObj
			,MAT_CONVRY
			,clr
			,drawpath
			,pConvey->m_dWidth
			,30.0);
	}
	AddObject(pObj,true);
}

void CRenderConvey3D::UpdateSelected( bool b )
{
}

Ogre::TextureUnitState* CRenderConvey3D::GetTextureUnit()
{
	MaterialPtr matPtr = OgreUtil::createOrRetrieveMaterial(MAT_CONVRY);
	
	if( !matPtr.isNull() &&  matPtr->getNumTechniques() )
	{
		Technique* pTech = matPtr->getTechnique(0);
		if(pTech && pTech->getNumPasses())
		{
			Pass* pPas = pTech->getPass(0);
			if(pPas && pPas->getNumTextureUnitStates())
			{
				return pPas->getTextureUnitState(0);
			}
		}
	}
	return NULL;	
}

void CRenderConvey3D::Setup3D( CProcessor2Base* pProc, int idx )
{
	ARCVector3 center = pProc->GetLocation();
	SetPosition(center);
	Update3D(pProc);
}

