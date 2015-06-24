#include "StdAfx.h"
#include ".\remesh.h"

NAMESPACE_RENDERENGINE_BEGIN

_UserData::_UserData(){ p = NULL; }

Mesh::Mesh(void){
	Material def;
	materials["default"] = def;
}

Mesh::~Mesh(void){
	glDeleteLists(usr.d,1);

}



void Mesh::calculteNormal(){
	
	for(size_t i = 0 ;i<faces.size();++i){
		faces[i].normal = cross( vertexs[ faces[i].vertexs[1] ].point  - vertexs[ faces[i].vertexs[0] ].point ,
								 vertexs[ faces[i].vertexs[2] ].point  - vertexs[ faces[i].vertexs[1] ].point );
		faces[i].normal.Normalize();
	}
}

Material::Material(){
	color = Renderer::Color::WHITE;
}
void Material::Apply(){
	if(usr.texture)usr.texture->Apply();
	glColor4ubv(color);
}
void Material::UnApply(){
	if(usr.texture)usr.texture->UnApply();
}

Face::Face(){
	faceType = GL_TRIANGLE_STRIP;
	vertexs.reserve(4);
}

void glRenderMesh(Mesh* mesh){
	if(!mesh)return;
	
	if(mesh->usr.d){ glCallList(mesh->usr.d); }
	else{
		mesh->usr.d = glGenLists(1);
		glNewList(mesh->usr.d, GL_COMPILE_AND_EXECUTE);

		for(size_t i=0;i<mesh->faces.size();i++){
		//
		Face * pface =& mesh->faces.at(i);
		if(pface->usr.material) pface->usr.material->Apply();
		
		glBegin(pface->faceType);
		for(size_t j=0;j<pface->vertexs.size();j++){
			glNormal3dv( pface->normal);
			int vexid = pface->vertexs[j];
			Vertex vex = mesh->vertexs[ vexid ];
			glTexCoord2dv( mesh->vertexs[ vexid ].texcord );
			glVertex3dv( mesh->vertexs[ vexid ].point );
		}
			
		glEnd();
		if(pface->usr.material) pface->usr.material->UnApply();
	}
	glEndList();
	}
	

	glRenderMesh(mesh->nextMesh.get());	
}
//refresh the usr data
void Mesh::build(CTextureResource * pTextureRes){
	calculteNormal();
	std::map<CString,Material>::iterator itr;
	for(itr=materials.begin();itr!=materials.end();itr++){
		Material * pMat = & itr->second;
		if( pMat->usr.texture ) continue;
		pMat->usr.texture = pTextureRes->getTexture( pMat->texture);
	}

	for(size_t i = 0 ;i<faces.size();++i){
		if(faces[i].usr.material) continue;
		if( materials.end() == materials.find(faces[i].material) ) continue;
		faces[i].usr.material = &materials[faces[i].material];
	}
	if(nextMesh.get())
		nextMesh->build(pTextureRes);
}


NAMESPACE_RENDERENGINE_END