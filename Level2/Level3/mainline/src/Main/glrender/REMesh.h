#ifndef REDNERER_MESH_DEF
#define  REDNERER_MESH_DEF
#include <Common/ARCVector.h>
#include "./RenderEngineHeader.h"
#include "./glTextureResource.h"
#include "./REColor.h"

#pragma once

NAMESPACE_RENDERENGINE_BEGIN

typedef std::vector<int> IntVector;

struct Material;
class Mesh;

typedef union _UserData {
	void *p;
	//Lib3dsIntd i;
	GLint d;          //display list
	//Lib3dsFloat f;
	Material *material;
	Mesh *mesh;
	CTexture * texture;
	//Lib3dsCamera *camera;
	//Lib3dsLight *light;
	//Lib3dsNode *node;
	_UserData();
} UserData;

struct Vertex{
	ARCVector3 point;
	ARCVector2 texcord;
};

struct Face {	
	UserData usr;            // material pointer
	GLenum faceType;         //could be GL_TRIANGLES GL_POLYGON_STIPPLE
	IntVector vertexs;        //points number
	
	CString material;	
	ARCVector3 normal;
	Face();
};

struct Material {
	UserData usr;     //texture pointer
	CString texture;
	Color color;
	void Apply();
	void UnApply();

	Material();
};

class Mesh : public Referenced
{
public:
	UserData usr; //Display list
	ref_ptr<Mesh> nextMesh;
	CString name;
	
	std::vector<Vertex> vertexs;
	std::vector<Face> faces;
	std::map<CString, Material> materials;

	Mesh(void);
	~Mesh(void);
	void calculteNormal();
	void build(CTextureResource * pTextureRes);

};

void glRenderMesh(Mesh* mesh);

NAMESPACE_RENDERENGINE_END
#endif