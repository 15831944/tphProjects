#include "StdAfx.h"
#include ".\renderheliport3d.h"
#include "RenderRunway3D.h"
#include <Common/ARCPath.h>
#include "shapebuilder.h"
using namespace Ogre;

static void Build(Heliport* pHeliport, ManualObject* pObj)
{
	pObj->clear();

	if (pHeliport->GetPath().getCount()!=2)
		return;

	ColourValue transparentColor(1.0, 1.0, 1.0, 0.0);

	const CPath2008& path = pHeliport->GetPath();
	ARCPoint3 startPt = path.getPoint(0);
	ARCPoint3 endPt = path.getPoint(1);
	ARCPath3 drawPath;
	drawPath.push_back(startPt);
	drawPath.push_back(endPt);
	ProcessorBuilder::DrawTexturePath(pObj, _T("Runway/RunwayBase"), transparentColor, drawPath, pHeliport->GetRadius()/2, 0.0);

	// calculate position for 
	Vector3 dir(endPt.x - startPt.x, endPt.y - startPt.y, 0.0);
	Vector3 center(endPt.x, endPt.y, 0.0);
	dir *= (pHeliport->GetRadius()/dir.length());
	Vector3 normDir = Vector3::UNIT_Z.crossProduct(dir);

	TextureClips clips;
	clips.resize(1);
	{
		TextureClip& clip = clips.front();
		clip.texture_name = _T("Airside/Heliport");
		clip.vertex_coords.resize(4);
		clip.texture_coords.resize(4);
		std::vector<ARCVector2>& vertex_coords = clip.vertex_coords;
		std::vector<ARCVector2>& texture_coords = clip.texture_coords;
		vertex_coords[0]  = ARCPoint2(center.x + dir.x + normDir.x, center.y + dir.y + normDir.y);
		vertex_coords[1]  = ARCPoint2(center.x - dir.x + normDir.x, center.y - dir.y + normDir.y);
		vertex_coords[2]  = ARCPoint2(center.x - dir.x - normDir.x, center.y - dir.y - normDir.y);
		vertex_coords[3]  = ARCPoint2(center.x + dir.x - normDir.x, center.y + dir.y - normDir.y);
		texture_coords[0] = ARCPoint2(0.0, 0.0);
		texture_coords[1] = ARCPoint2(0.0, 1.0);
		texture_coords[2] = ARCPoint2(1.0, 1.0);
		texture_coords[3] = ARCPoint2(1.0, 0.0);
	}
	ProcessorBuilder::DrawTextureClips(pObj, clips, transparentColor, 0.1);

}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderHeliport3D, Heliport)

void CRenderHeliport3D::Update3D( ALTObjectDisplayProp* pDispObj )
{

}

void CRenderHeliport3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* pObj = _GetOrCreateManualObject(GetName());
	Build((Heliport*)pBaseObj,pObj);
	AddObject(pObj);
}
