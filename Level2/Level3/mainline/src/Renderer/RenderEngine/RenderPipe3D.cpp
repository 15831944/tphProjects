#include "StdAfx.h"
#include "RenderPipe3D.h"
#include <CommonData/PipeBase.h>

using namespace Ogre;



#define PIPE_MATERIAL   "PipeMaterial"
#define PIPE_CROSS_MATERIAL "PipeCrossMaterial"
#define PIPE_Z_HEIGHT   0.0

#pragma warning(disable:4244)


static void DrawFlatSquareZNormal(ManualObject* pObj, ARCVector3 pos, double size, const ColourValue& clr)
{
	double halfSize = size/2.0;
	pObj->begin(PIPE_MATERIAL, RenderOperation::OT_TRIANGLE_FAN);
	pObj->position(pos.x - halfSize, pos.y - halfSize, pos.z);
	pObj->colour(clr);
	pObj->position(pos.x - halfSize, pos.y + halfSize, pos.z);
	pObj->position(pos.x + halfSize, pos.y + halfSize, pos.z);
	pObj->position(pos.x + halfSize, pos.y - halfSize, pos.z);
	pObj->end();
}


#define PIPEALPHA 32
static const unsigned char PipeColors[][4] =
{
	237,  28,  36, PIPEALPHA,
	242, 101,  34, PIPEALPHA,
	247, 148,  29, PIPEALPHA,
	255, 242,   0, PIPEALPHA,
	141, 198,  63, PIPEALPHA,
	 57, 181,  74, PIPEALPHA,
	  0, 166,  81, PIPEALPHA,
	  0, 169, 157, PIPEALPHA,
	  0, 174, 239, PIPEALPHA,
	  0, 114, 188, PIPEALPHA,
	  0,  84, 166, PIPEALPHA,
	 46,  49, 146, PIPEALPHA,
	102,  45, 145, PIPEALPHA,
	146,  39, 143, PIPEALPHA,
	236,   0, 140, PIPEALPHA,
	237,  20, 191, PIPEALPHA,
};

void CRenderPipe3D::Build(CPipeBase* pPipe,int index, ManualObject* pObj)
{
	// Draw all polygons
	const unsigned char* clr = PipeColors[index%16];
	ColourValue color(clr[0]/255.0, clr[1]/255.0, clr[2]/255.0, clr[3]/255.0);
	ColourValue colorEditPoint(clr[0]/255.0, clr[1]/255.0, clr[2]/255.0, 1.0);
	POLLYGONVECTOR polyVect;
	pPipe->GetPipeCoveredRegion(polyVect);
	pObj->clear();
	for(size_t i=0; i<polyVect.size(); i++)
	{
		const Pollygon& poly = polyVect[i];
		int c = poly.getCount();
		Point* pts = poly.getPointList();
		pObj->begin(PIPE_MATERIAL, RenderOperation::OT_TRIANGLE_FAN);
		for(int j=c-1;j>=0;j--)
		{
			pObj->position(pts[j].getX(), pts[j].getY(), PIPE_Z_HEIGHT);	
			pObj->colour(color);
		}
		pObj->end();
	}	
	//render pipe crosswalk
	POLLYGONVECTOR polyCrossVect;
	POLLYGONVECTOR polyTexVect;
	ColourValue crossC = OGRECOLOR(ARCColor4(255,255,255,132)); 
	pPipe->GetPipeCrosswalkRegion(polyCrossVect,polyTexVect);

	for(int j=0; j<static_cast<int>(polyCrossVect.size()); j++) {
		const Pollygon& poly = polyCrossVect[j];
		const Pollygon& texpoly  = polyTexVect[j];
		int c = poly.getCount();
		Point* pts = poly.getPointList();
		Point* tex = texpoly.getPointList();
		pObj->begin(PIPE_CROSS_MATERIAL, RenderOperation::OT_TRIANGLE_FAN);
		for(int k=c-1;k>=0;k--)
		{
			pObj->position(pts[k].getX(), pts[k].getY(), PIPE_Z_HEIGHT);	
			pObj->colour(crossC);
			pObj->textureCoord(tex[k].getX()/500,tex[k].getY()/500);
		}
		pObj->end();		
	}
}

/*class C3DPipeNodeImpl : public GUIDItem3DImpl
{
public:
	C3DPipeNodeImpl(const CGuid& id):GUIDItem3DImpl(id){}
	virtual bool Update3D(C3DSceneNode& node){  }
protected:
}*/;

void CRenderPipe3D::Update3D(CPipeBase* pPipe, int idx)
{
	ASSERT(pPipe);
	if(!pPipe)return;

	ManualObject* pObj = _GetOrCreateManualObject(GetName());
	if(pObj)
	{
		Build(pPipe,idx, pObj);
		AddObject(pObj);
	}
}

void CRenderPipe3D::Setup3D( CPipeBase* pPipe, int idx )
{
	//SetImpl(new C3DPipeNodeImpl(pPipe->getGuid()));
	Update3D(pPipe,idx);
}


