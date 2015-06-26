#include "StdAfx.h"
#include ".\landsideexternalnode3d.h"
#include "Landside\LandsideExternalNode.h"
#include "Landside\ILandsideEditContext.h"

using namespace Ogre;

const static String matEntryNode = "Landside/Entry";
const static String matExitNode = "Landside/Exit";

PROTYPE_INSTANCE(LandsideExternalNode3D)


void LandsideExternalNode3D::OnUpdate3D()
{
	LandsideExternalNode* pNode = GetExtNode();
	if(!pNode)
		return;
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx)
		return;

	const String& mat  = pNode->IsLinkStretchPort1()?matEntryNode:matExitNode;


	//draw area	
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());

	ILayoutObjectDisplay::DspProp& dspShape = pNode->getDisplayProp().GetProp(ILayoutObjectDisplay::_Shape);

	CPoint2008 pos;	ARCVector3 dir;DistanceUnit dwidth;
	if(pObj && pNode->GetPosDirDirectionWidth(pos,dir,dwidth) && dspShape.bOn )
	{
		pCtx->ConvertToVisualPos(pos);

		Vector3 vHead = dwidth * 0.4 * Vector3(dir.x,dir.y,0).normalisedCopy();
		Vector3 vleft = dwidth * 0.6 * Vector3(-dir.y,dir.x,0).normalisedCopy();
		
		//MaterialPtr mat = OgreUtil::createOrRetrieveColorMaterial((COLORREF)dspShape.cColor);

		pObj->clear();
		pObj->begin(mat,RenderOperation::OT_TRIANGLE_FAN);		
		
		pObj->position(-vleft);pObj->textureCoord(Vector2::ZERO),pObj->colour(OGRECOLOR(dspShape.cColor));
		pObj->position(-vleft+vHead);pObj->textureCoord(Vector2::UNIT_Y);
		pObj->position(vleft+vHead);pObj->textureCoord(Vector2::UNIT_SCALE);
		pObj->position(vleft);pObj->textureCoord(Vector2::UNIT_X);
		
		pObj->end();

		if(pNode->IsNewObject())
			AddObject(pObj,2);
		else
			AddObject(pObj,1);
		
		SetPosition(pos);
		//SetDirection(dir,ARCVector3(0,0,1));
	}

}

LandsideExternalNode* LandsideExternalNode3D::GetExtNode()
{
	ILayoutObject*pObj = getLayOutObject();
	if(pObj->GetType()!=ALT_LEXT_NODE)
		return NULL;
	return (LandsideExternalNode*) pObj;
}
