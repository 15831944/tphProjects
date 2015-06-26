#include "stdafx.h"
#include "RenderBillboard3D.h"
#include "Render3DScene.h"
#include <CommonData/ProcessorProp.h>
#include <CommonData/ExtraProcProp.h>
#include "ShapeBuilder.h"
#include "CommonData/Processor2Base.h"

using namespace Ogre;
void CRenderBillboard3D::UpdateSelected( bool b )
{

}
//
//void CRenderBillboard3D::Update3D(int nUpdateCode/* = Terminal3DUpdateAll*/)
//{
//	CProcessor2Base* pProc = GetProcessor();
//	ASSERT(pProc);
//	ProcessorProp* pProcProp = pProc->GetProcessorProp();
//	ASSERT(pProcProp && pProcProp->getProcessorType() == BillboardProcessor);
//	ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
//	ASSERT(pExtraProp && pExtraProp->GetType() == ExtraProcProp::Billboard_Prop_Type);
//	BillboardProp* pBillboarExtraProp = (BillboardProp*)pExtraProp;
//
//	//
//
//	UpdateSelected(GetSelected());
//}

void CRenderBillboard3D::Update3D( CProcessor2Base* pProc, int index /*= Terminal3DUpdateAll*/ )
{	
	ProcessorProp* pProcProp = pProc->GetProcessorProp();
	ASSERT(pProcProp && pProcProp->getProcessorType() == BillboardProcessor);
	ExtraProcProp* pExtraProp = pProcProp->GetExtraProp();
	ASSERT(pExtraProp && pExtraProp->GetPropType() == ExtraProcProp::Billboard_Prop_Type);
	BillboardProp* pBillboarExtraProp = (BillboardProp*)pExtraProp;
	
	const CProcessor2Base::CProcDispProperties& pdp = pProc->m_dispProperties;
	if (!pdp.bDisplay[PDP_DISP_SHAPE])
	{
		RemoveFromParent();
	}
	ARCVector3 center = GetWorldPosition();
	Vector3 centerOffset(-center.x,-center.y,0);
	//render shape 
	Real dheight = static_cast<Real>(pBillboarExtraProp->m_dHeight);
	Vector3 vHeight = Vector3(0,0,dheight);
	Real dThick = static_cast<Real>(pBillboarExtraProp->m_dThickness);
	Path* servPath = pProcProp->serviceLocationPath();
	if(!servPath)
		return;

	Point p1,p2;
	p1 = servPath->getPoint(0); p2 = servPath->getPoint(1);
	Vector3 pos1(p1.getX(),p1.getY(),0);
	Vector3 pos2(p2.getX(),p2.getY(),0);
	
	bool boardleftSide = false;
	Vector3 dir = pos2-pos1;dir.normalise();
	Quaternion quat;quat.FromAngleAxis(Degree(90),Vector3::UNIT_Z);
	Vector3 widthdir = quat*dir;
	Vector3 thickOffset = widthdir*dThick;

	Path* exposureAre = pBillboarExtraProp->GetExpsurePath();
	if(exposureAre && exposureAre->getCount()>0)
	{		
		Point exPosdir = exposureAre->getPoint(exposureAre->getCount()/2)-p1;
		boardleftSide = Vector3(exPosdir.getX(),exPosdir.getY(),0).crossProduct(dir).z>0;
	}

	//offset to center
	ARCColor3 stateOffColor(128,128,128);
	if(pdp.bDisplay[PDP_DISP_SHAPE])
	{
		pos1 += centerOffset;
		pos2 += centerOffset;
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject( GetName()+_T("/board"),getScene() );
		ASSERT(pObj);
		if(pObj)
		{	
			pObj->clear();//draw board
			//draw billboard
			pObj->begin("",RenderOperation::OT_TRIANGLE_FAN);
			pObj->position(pos1);pObj->normal(-widthdir);
			pObj->position(pos2);
			pObj->position(pos2+vHeight);
			pObj->position(pos1+vHeight);
			pObj->end();

			int vextBegin = 0;
			ARCColor3 cDisplay;
			cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_SHAPE] : stateOffColor;
			MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(cDisplay);		
			//draw side			
			pObj->begin(matPtr->getName(),RenderOperation::OT_TRIANGLE_LIST);
			pObj->position(pos1);pObj->normal(-dir);
			pObj->position(pos1+thickOffset);
			pObj->position(pos1+vHeight);
			pObj->position(pos1+vHeight+thickOffset);
			pObj->quad(vextBegin+1,vextBegin,vextBegin+2,vextBegin+3);
			vextBegin+=4;


			pObj->position(pos1+vHeight);pObj->normal(Vector3::UNIT_Z);
			pObj->position(pos1+vHeight+thickOffset);
			pObj->position(pos2+vHeight);
			pObj->position(pos2+vHeight+thickOffset);
			pObj->quad(vextBegin+1,vextBegin,vextBegin+2,vextBegin+3);
			vextBegin+=4;

			pObj->position(pos2+vHeight);pObj->normal(dir);
			pObj->position(pos2+vHeight+thickOffset);
			pObj->position(pos2);
			pObj->position(pos2+thickOffset);
			pObj->quad(vextBegin+1,vextBegin,vextBegin+2,vextBegin+3);
			vextBegin+=4;


			pObj->position(pos2);pObj->normal(Vector3::NEGATIVE_UNIT_Z);
			pObj->position(pos2+thickOffset);
			pObj->position(pos1);
			pObj->position(pos1+thickOffset);
			pObj->quad(vextBegin+1,vextBegin,vextBegin+2,vextBegin+3);
			vextBegin+=4;

			pObj->position(pos1+thickOffset);pObj->normal(widthdir);
			pObj->position(pos2+thickOffset);
			pObj->position(pos1+vHeight+thickOffset);
			pObj->position(pos2+vHeight+thickOffset);
			pObj->quad(vextBegin+1,vextBegin,vextBegin+2,vextBegin+3);

			pObj->end();			
			AddObject(pObj,true);	

		}	
	}
	
	//draw expose area
	if(exposureAre && pdp.bDisplay[PDP_DISP_OUTC])
	{
		Path drawexposure = *exposureAre;
		drawexposure.DoOffset(-center.x,-center.y,0);

		ManualObject* pAreaObj = OgreUtil::createOrRetrieveManualObject( GetName()+_T("/exposArea"),getScene() );
		ASSERT(pAreaObj);
		if (pAreaObj)
		{
			pAreaObj->clear();			
			ARCColor3 cDisplay;
			cDisplay = pProc->GetProcessorStateOffTag() ? pdp.color[PDP_DISP_OUTC] : stateOffColor;
			ProcessorBuilder::DrawPath(pAreaObj, &drawexposure, OGRECOLOR(cDisplay),0,true);
			AddObject(pAreaObj,true);			
		}

	}
}

void CRenderBillboard3D::Setup3D( CProcessor2Base* pProc, int idx )
{
	ARCVector3 center = pProc->GetLocation();
	SetPosition(center);
	Update3D(pProc,idx);
}
