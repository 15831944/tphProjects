#include "StdAfx.h"
#include ".\renderstand3d.h"

#include "RenderAirport3D.h"
#include "OgreConvert.h"
#include <InputAirside/ALTObjectDisplayProp.h>
#include <Common/ARCPath.h>
#include "ShapeBuilder.h"
#include ".\CustomizedRenderQueue.h"

using namespace Ogre;

void CRenderStand3D::Update3D(ALTObject* pBaseObj)
{
	
	Stand* pStand = (Stand*)pBaseObj;

	StandDisplayProp dispProp;
	dispProp.ReadData(pStand->getID());
	StandDisplayProp* pDispProp = &dispProp;
	ARCVector3 pt = pStand->GetServicePoint();
	SetPosition(pt);

	if (pDispProp->m_dpShape.bOn)
	{
		// 		
		Entity * pEnt = _GetOrCreateEntity(GetName() + _T("-Shape"),_T("StandBar.mesh"));//OgreUtil::createOrRetrieveEntity(, , GetScene());
		if (pEnt)
		{
			OgreUtil::ReplaceEntityColor(pEnt,pDispProp->m_dpShape.cColor, _T("Airside/__BodyColor"));				
			AddObject(pEnt);			
		}
		//m_shapeStandBar.SetPosition(pt);
	}

	ManualObject* pObj = _GetOrCreateManualObject( GetName() +_T("Other") );
	pObj->clear();
	pObj->setRenderQueueGroup(RenderObject_Mark);

	if( pDispProp->m_dpInConstr.bOn )
	{
		Ogre::ColourValue clr = OgreConvert::GetColor(pDispProp->m_dpInConstr.cColor);

		for(size_t i=0;i<pStand->m_vLeadInLines.GetItemCount();i++)
		{
			StandLeadInLine  *pLeadIn = &pStand->m_vLeadInLines.ItemAt(i);
			const CPath2008& path = pLeadIn->getPath();
			int nCount = path.getCount();
			ARCPath3 drawPath(path);
			drawPath.DoOffset(-pt);

			ProcessorBuilder::DrawColorPath(pObj, clr, drawPath, 34, false, 0.1);
			ProcessorBuilder::DrawArrow(pObj, Point(drawPath[nCount-2].x, drawPath[nCount-2].y, 0.1),
				Point(drawPath[nCount-1].x, drawPath[nCount-1].y, 0.1), clr, 136.0, 136.0);
		}		
	}
	if( pDispProp->m_dpOutConstr.bOn)
	{
		Ogre::ColourValue clr = OgreConvert::GetColor(pDispProp->m_dpOutConstr.cColor);
		for(size_t i=0;i<pStand->m_vLeadOutLines.GetItemCount();i++)
		{
			StandLeadOutLine * pLine = &pStand->m_vLeadOutLines.ItemAt(i);
			const CPath2008& path = pLine->getPath();
			int nCount = path.getCount();
			ARCPath3 drawPath(path);
			drawPath.DoOffset(-pt);

			ProcessorBuilder::DrawColorPath(pObj, clr, drawPath, 34, false, 0.1);
			ProcessorBuilder::DrawArrow(pObj, Point(drawPath[nCount-2].x, drawPath[nCount-2].y, 0.1),
				Point(drawPath[nCount-1].x, drawPath[nCount-1].y, 0.1), clr, 136.0, 136.0);

			if (pLine->IsSetReleasePoint())
			{
				CPoint2008 pt = pLine->GetReleasePoint();
				ARCVector3 dir = pLine->getPath().GetIndexDir(pLine->getPath().getCount()-1.0f);
				dir.RotateXY(90);
				dir.SetLength(68);				

				CPath2008 path;
				path.init(2);
				path[0] = pt + dir ;
				path[1] = pt - dir;
				ARCPath3 drawPath2(path);
				ProcessorBuilder::DrawColorPath(pObj, clr, drawPath2, 34, false, 0.1);
			}
		}
	}

	AddObject(pObj);


}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderStand3D, Stand)

void CRenderStand3D::Update3D( ALTObjectDisplayProp* pDispObj/*, int nUpdateCode*/ /*= Terminal3DUpdateAll*/ )
{

}

