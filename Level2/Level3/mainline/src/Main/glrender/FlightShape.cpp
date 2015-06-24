#include "StdAfx.h"
#include ".\flightshape.h"


const static char strAirlinefin[] = "airlinefin";
const static char strAirlineside[]= "airlinesid";
const static char strFlightBody[]= "FlightBody";
const static char strFlightTrial[]= "FlightTrail";
FlightShape::FlightShape(void)
{
}

FlightShape::~FlightShape(void)
{
}

void FlightShape::displayMesh(Lib3dsMesh *mesh){
	bool bincludemat =  true;   // include material in display list
	int p = mesh->faces;
	if(p<1)return;
	BOOL nFlags = TRUE;


	Lib3dsFace * pFace = NULL;
	pFace = &mesh->faceL[0];
	Lib3dsMaterial * mat_test = lib3ds_file_material_by_name(m_p3dsFile,pFace->material);
	
	if(mat_test && (strcmp(mat_test->name,strFlightBody)!=0)&&(strcmp(mat_test->name,strFlightTrial)!=0) ){
		return CglShape::displayMesh(mesh);
	}

//	glEnable(GL_COLOR_MATERIAL);
//	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	//glDisable(GL_LIGHTING);
	//chloe begin
//	glEnable(GL_BLEND);
	//end
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); 
	if(!mesh->user.d){
		mesh->user.d=glGenLists(1);
	glNewList(mesh->user.d, GL_COMPILE);

	{
		unsigned p;
		Lib3dsVector *normalL=(Lib3dsVector *)malloc(3*sizeof(Lib3dsVector)*mesh->faces);
					
		lib3ds_mesh_calculate_normals(mesh, normalL);

		
		for (p=0; p<mesh->faces; ++p) {
			Lib3dsFace *f=&mesh->faceL[p];		
			glBegin(GL_TRIANGLES);
			glNormal3fv(f->normal);
			for (int i=0; i<3; ++i) {
				glNormal3fv(normalL[3*p+i]);
				
				if ( mesh->texelL ) {
					glTexCoord2fv( mesh->texelL[f->points[i]] );
				}

				glVertex3fv(mesh->pointL[f->points[i]].pos);
			}
			glEnd();	

		}
		
		free(normalL);
	}

	glEndList();
	}

	if( m_airlineTexture.get())
	{
		m_airlineTexture->Apply();
	}
	if (mesh->user.d) {	

		glPushMatrix();


		glCallList(mesh->user.d);

		glPopMatrix();

	}
	//chloe
	//begin
	//glDisable(GL_BLEND);
	//end
	glEnable(GL_LIGHTING);
//	glDisable(GL_COLOR_MATERIAL);

}

void FlightShape::setAirlineTexture(CTexture * ptextures)
{
	 m_airlineTexture = ptextures;
}