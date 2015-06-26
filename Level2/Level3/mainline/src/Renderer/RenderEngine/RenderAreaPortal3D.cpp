#include "StdAfx.h"
#include "RenderAreaPortal3D.h"
#include "OgreUtil.h"
#include "OgreConvert.h"
using namespace Ogre;
#define AREA_PORTTAL_MAT _T("GridLine")
void CRenderNamedPointList3D::Setup3D( CNamedPointList* pData, int idx )
{
	ASSERT(pData);
	// 	CNamedPointList* pData = GetData();
	if (pData)
	{
		const std::vector<ARCPoint2>& orgPoints = pData->points;

		ManualObject* pObj = _GetOrCreateManualObject(GetName());
		pObj->clear();
		if (orgPoints.size())
		{
			pObj->begin( AREA_PORTTAL_MAT, RenderOperation::OT_LINE_STRIP);
			pObj->colour( OgreConvert::GetColor(pData->color));
			for (std::vector<ARCPoint2>::const_iterator ite = orgPoints.begin();ite != orgPoints.end();ite++)
			{
				pObj->position(ite->x, ite->y, 0.0f);
			}
			pObj->position(orgPoints.front().x, orgPoints.front().y, 0.0f);
			pObj->end();
		}
		AddObject(pObj);
	}
	//UpdateSelected(GetSelected());
}
