#pragma once
#include "altobject3d.h"
class Structure;
#include "glRender/glTextureResource.h"
#include "glRender/glShapeResource.h"

class CStructure3D :
	public ALTObject3D
{
public:
	CStructure3D(int nID);
	virtual ~CStructure3D(void);


	virtual ARCPoint3 GetLocation(void) const;

	Structure * GetStructure()const;
	//StructureDisplayProp* GetDisplayProp()const;

	virtual void DrawOGL(C3DView * m_pView);
	virtual void DrawSelect(C3DView * pView);

	virtual void Render(C3DView * pView);
	virtual void RenderShadow(C3DView * pView);

	void UpdateIntersectionPoints(C3DView* pView);

	void FlushChange();

	virtual void Update();

	void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */);
	virtual void OnRotate(DistanceUnit dx);

	std::vector<DistanceUnit> m_vObstructionHeights;
	bool isObstructionsValid(DistanceUnit dAlt)const ;

public:
protected:
	std::vector<CTexture2::RefPtr> m_vTextures;
	CTexture2* getTexture(CString fileName, C3DView* pView);
};
