#include "stdafx.h"
#include "RenderEscalator3D.h"
#include "Render3DScene.h"
#include <CommonData/ProcessorProp.h>
#include <CommonData/ExtraProcProp.h>
#include "MaterialDef.h"

#include "common/IARCportLayoutEditContext.h"
#include "common/IEnvModeEditContext.h"
#include <common/Path2008.h>
#include <common/ARCPipe.h>
#include <common/Pollygon2008.h>

#define MAT_ESCALATOR _T("processor/escalator")
using namespace Ogre;
void CRenderEscalator3D::Update3D( CProcessor2Base* pProc )
{
	ProcessorProp* pProcProp = pProc->GetProcessorProp();
	ASSERT(pProcProp && pProcProp->getProcessorType() == EscalatorProc);
	ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
	ASSERT(pExtraProp && pExtraProp->GetPropType() == ExtraProcProp::Escalator_Prop_Type);
	EscalatorProp* pMSW = (EscalatorProp*)pExtraProp;

	Path* floorPath = pProcProp->serviceLocationPath();
	if(!floorPath)
		return;

	ARCVector3 center = GetWorldPosition();


	const CProcessor2Base::CProcDispProperties& pdp = pProc->m_dispProperties;

	ITerminalEditContext* pCtxTerminal =   GetRoot().GetModel()->GetTerminalContext();
	ASSERT(pCtxTerminal);
	ARCPath3 drawpath = pCtxTerminal->GetTerminalVisualPath(*floorPath);
	drawpath.DoOffset(-center);

	ManualObject* pObj =  _GetOrCreateManualObject(GetName());// OgreUtil::createOrRetrieveManualObject(GetIDName()+_T("/Shape"),GetScene());
	if(!pObj)
		return;
	pObj->clear();
	if(pdp.bDisplay[PDP_DISP_SHAPE])
	{
		ARCColor3 stateOffColor(128,128,128);
		ARCColor3 cDisplay;
		cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_SHAPE] : stateOffColor;
		Ogre::ColourValue clr = OGRECOLOR(/*pdp.color[PDP_DISP_SHAPE]*/cDisplay);
		DrawStairTexturePath(pObj
			,MAT_ESCALATOR
			,clr
			,drawpath
			,pMSW->m_dWidth
			,76.2);
	}
	AddObject(pObj,true);

	UpdateNoShape(pProc,DRAWALLNOSHAPENOSERVLOC);
}

void CRenderEscalator3D::DrawStairTexturePath( Ogre::ManualObject* pObj,const Ogre::String& texmat,const Ogre::ColourValue& clr, const ARCPath3&path ,double dWidth, double dSideHeight, bool bLoop/* = false*/ )
{
	if(path.size()<1)return;


	ARCPipe pipe(path, dWidth, bLoop);
	Real dLen1=0;
	ARCPoint3 V2 = pipe.m_sidePath2[1] - pipe.m_sidePath2[0];
	ARCPoint3 V3 = pipe.m_sidePath2[0] - pipe.m_sidePath1[0];
	V2.SetLength(1.0);
	Real dLen2 = V3.dot(V2);

	pObj->begin(texmat,RenderOperation::OT_TRIANGLE_STRIP);
	Vector3 prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	Vector3 prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);
	for(size_t i=1;i<pipe.m_sidePath1.size();++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);
		//	Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

		pObj->position(prepos1);pObj->textureCoord(0,dLen1);pObj->colour(clr);pObj->normal(0.0,0.0,1.0);
		pObj->position(prepos2);pObj->textureCoord(1,dLen2);

		Vector3 tempPos1(pos1.x,pos1.y,0.0);
		Vector3 tempPos2(pos2.x,pos2.y,0.0);
		Vector3 tempPrepos1(prepos1.x,prepos1.y,0.0);
		Vector3 tempPrepos2(prepos2.x,prepos2.y,0.0);

		dLen1+=(tempPos1-tempPrepos1).length();
		dLen2+=(tempPos2-tempPrepos2).length();

		/*	dLen1+=(pos1-prepos1).length();
		dLen2+=(pos2-prepos2).length();*/
		pObj->position(pos1);pObj->textureCoord(0,dLen1);
		pObj->position(pos2);pObj->textureCoord(1,dLen2);

		//pObj->quad(nVexCnt,nVexCnt+1,nVexCnt+3,nVexCnt+2);

		//nVexCnt+=4;
		prepos1=  pos1;
		prepos2 = pos2;
		std::swap(dLen1,dLen2);
	}
	pObj->end();

	Vector3 heightV(0,0,dSideHeight);
	int nVexCnt = 0;
	pObj->begin(VERTEXCOLOR_LIGHTON_NOCULL,RenderOperation::OT_TRIANGLE_LIST);//side1
	prepos1 = OGREVECTOR(pipe.m_sidePath1[0]);
	prepos2 = OGREVECTOR(pipe.m_sidePath2[0]);
	for(size_t i=1;i<pipe.m_sidePath1.size();++i)
	{
		Vector3 pos1 = OGREVECTOR(pipe.m_sidePath1[i]);
		Vector3 pos2 = OGREVECTOR(pipe.m_sidePath2[i]);

		//	Vector3 norm = -(pos1-prepos1).crossProduct(heightV).normalisedCopy();
		pObj->position(prepos1);pObj->colour(clr);pObj->normal(0.0,0.0,1.0);
		pObj->position(prepos1+heightV);
		pObj->position(pos1);
		pObj->position(pos1+heightV);	
		pObj->quad(nVexCnt,nVexCnt+1,nVexCnt+3,nVexCnt+2);
		nVexCnt+=4;

		//	norm = (pos2-prepos2).crossProduct(heightV).normalisedCopy();
		pObj->position(prepos2);pObj->colour(clr);pObj->normal(0.0,0.0,1.0);
		pObj->position(prepos2+heightV);
		pObj->position(pos2);
		pObj->position(pos2+heightV);	
		pObj->quad(nVexCnt,nVexCnt+1,nVexCnt+3,nVexCnt+2);
		nVexCnt+=4;

		prepos1=  pos1;
		prepos2 = pos2;		
	}
	pObj->end();
}

void CRenderEscalator3D::UpdateSelected( bool b )
{
}

void CRenderEscalator3D::Setup3D( CProcessor2Base* pProc,int idx )
{
	ARCVector3 center = pProc->GetLocation();
	SetPosition(center);
	Update3D(pProc);
}
