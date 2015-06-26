#include "stdafx.h"
#include ".\rendermovingsidewalk3d.h"
#include "Render3DScene.h"
#include <CommonData/ProcessorProp.h>
#include <CommonData/ExtraProcProp.h>

#include <CommonData/ExtraProcProp.h>
#include "common/IARCportLayoutEditContext.h"
#include "common/IEnvModeEditContext.h"
#include "shapebuilder.h"

#define MAT_CONVRY _T("processor/convey")
using namespace Ogre;
void CRenderMovingSideWalk3D::Update3D( CProcessor2Base* pProc )
{
	ProcessorProp* pProcProp = pProc->GetProcessorProp();
	ASSERT(pProcProp && pProcProp->getProcessorType() == MovingSideWalkProc);
	ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
	ASSERT(pExtraProp && pExtraProp->GetPropType() == ExtraProcProp::MovingSideWalk_Prop_Type);
	MovingSideWalkProp* pMSW = (MovingSideWalkProp*)pExtraProp;

	Path* floorPath = pProcProp->serviceLocationPath();
	if(!floorPath)
		return;
	const CProcessor2Base::CProcDispProperties& pdp = pProc->m_dispProperties;
	ARCVector3 parentPos = GetWorldPosition();
	ITerminalEditContext* pCtxTerminal =   GetRoot().GetModel()->GetTerminalContext();// m_p3DScene->mpEditor->GetContext(EnvMode_Terminal)->toTerminal();
	ASSERT(pCtxTerminal);

	ARCPath3 drawpath = pCtxTerminal->GetTerminalVisualPath(*floorPath);
	drawpath.DoOffset(-parentPos);

	double dSpeed;
	double dWidth;
	pCtxTerminal->GetMovingSideWalkWidthSpeed(*pProcProp->getID(),dSpeed,dWidth);

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
			,dWidth
			,76.2);
	}
	AddObject(pObj,true);


	UpdateNoShape(pProc,DRAWALLNOSHAPENOSERVLOC);
}

void CRenderMovingSideWalk3D::UpdateSelected( bool b )
{
}

void CRenderMovingSideWalk3D::Setup3D( CProcessor2Base* pProc, int idx )
{
	ARCVector3 center = pProc->GetLocation();
	SetPosition(center);

	Update3D(pProc);
}
