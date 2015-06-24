
#pragma once

#include "glTextureResource.h"
#include "glShapeResource.h"




class CglShape;

class CglRenderer : public Referenced
{
public:
	
	 void render_shape(const CString& shapeIDstr);
	
	CTextureResource* m_pTextures;
	CglShapeResource* m_pShapes;

	CglRenderer();
	~CglRenderer();

private:
	void render_node(Lib3dsNode *node, Lib3dsFile * file);
	void render_mesh(Lib3dsMesh *mesh, Lib3dsFile * file); 
	bool apply_material(Lib3dsMaterial * pMat);	
};

