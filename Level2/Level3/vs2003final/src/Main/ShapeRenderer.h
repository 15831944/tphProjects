#ifndef __SHAPE_RENDERER_H
#define __SHAPE_RENDERER_H

#include ".\..\Common\ARCColor.h"
#include ".\..\Common\point.h"
#include ".\..\Common\path.h"
#include ".\..\Common\ARCVector.h"

static const GLfloat default_lightPosition[] =	{ -1.0f, 0.0f, 2.0f, 0.0f };

static const GLushort StipplePatterns[] = { 0xFFFF, 0x0FFF, 0x5555 };

static const ARCColor3 StationProcColors[] = {
	(255,255,102),	//Car area
		(204,204,255),	//Pre-Board Holding Area
		(64,210,64),	//Entry Doors
		(255,100,0),	//In constraint of preboarding area
		(153,0,0),		//Exit Doors
		(0,0,0),		//Station Name
};

#define PIPEALPHA 32
static const GLubyte PipeColors[][4] = {
	237,28,36,PIPEALPHA,
		242,101,34,PIPEALPHA,
		247,148,29,PIPEALPHA,
		255,242,0,PIPEALPHA,
		141,198,63,PIPEALPHA,
		57,181,74,PIPEALPHA,
		0,166,81,PIPEALPHA,
		0,169,157,PIPEALPHA,
		0,174,239,PIPEALPHA,
		0,114,188,PIPEALPHA,
		0,84,166,PIPEALPHA,
		46,49,146,PIPEALPHA,
		102,45,145,PIPEALPHA,
		146,39,143,PIPEALPHA,
		236,0,140,PIPEALPHA,
		237,20,191,PIPEALPHA,
};

static const ARCColor3 railwaycolor(0, 0, 51);
static const ARCColor3 railwaycolorselected(233,233,255);
static const ARCColor3 msoutlinecolor(137,137,137);
static const ARCColor3 traincolor(80,80,120);
static const ARCColor3 linecolor(20,20,20);
static const ARCColor3 markercolor(200,200,200);
static const ARCColor3 processorselectboxcolor1(210,0,34);
static const ARCColor3 processorselectboxcolor2(210,72,95);
static const ARCColor3 processortagcolor(32,32,32);
static const ARCColor3 processortagbkgcolor(191,223,255);
static const ARCColor3 actagcolor1(172,171,196);
static const ARCColor3 actagcolor2(228,227,227);
static const ARCColor3 accolor(75,75,75);
#include <Renderer/RenderEngine\Renderer.h>

//#include <Ogre\OgreQuaternion.h>
//#include <ogre\OgreVector3.h>
inline 
static void DrawPAXShape(double x, double y, double z,  double angle, GLuint dlidx,const ARCVector4& quat)
{
	double deg=0;
	ARCVector3 axis(0,0,1);
	CRenderer::GetRotate(quat,deg,axis);
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(angle,0,0,1);
	glRotated(deg,axis.x,axis.y,axis.z);	
	//glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_COLOR_MATERIAL);
	glCallList(dlidx);
	glEnable(GL_COLOR_MATERIAL);
	//glPopAttrib();
	glPopMatrix();
}
inline 
static void DrawPAXShape(double x, double y, double z, float scale, double angle, GLuint dlidx)
{
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(angle,0,0,1);
	glScalef(scale,scale,scale);
	//glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_COLOR_MATERIAL);
	glCallList(dlidx);
	glEnable(GL_COLOR_MATERIAL);
	//glPopAttrib();
	glPopMatrix();
}

inline 
static void DrawPAXShape(double x, double y, double z, double angle, GLuint dlidx)
{
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(angle,0,0,1);
	glDisable(GL_COLOR_MATERIAL);
	glCallList(dlidx);
	glEnable(GL_COLOR_MATERIAL);	
	glPopMatrix();
}

inline 
static void DrawPAXShape(double x, double y, double z,  GLuint dlidx){
	glPushMatrix();
	glTranslated(x,y,z);		
	glCallList(dlidx);	
	glPopMatrix();
}

inline 
static void DrawHightQuad(const Point& p1,const Point& p2,const Point& p3,const Point& p4,DistanceUnit height){
	glBegin(GL_QUADS);
	//top
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3d(p1.getX(),p1.getY(),p1.getZ()+height);
	glVertex3d(p2.getX(),p2.getY(),p2.getZ()+height);
	glVertex3d(p3.getX(),p3.getY(),p3.getZ()+height);
	glVertex3d(p4.getX(),p4.getY(),p4.getZ()+height);

	glEnd();
	//sides
	glBegin(GL_QUAD_STRIP);
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3d(p1.getX(),p1.getY(),p1.getZ());
	glVertex3d(p1.getX(),p1.getY(),p1.getZ()+height);	
	glVertex3d(p2.getX(),p2.getY(),p2.getZ());
	glVertex3d(p2.getX(),p2.getY(),p2.getZ()+height);	
	glVertex3d(p3.getX(),p3.getY(),p3.getZ());
	glVertex3d(p3.getX(),p3.getY(),p3.getZ()+height);
	glVertex3d(p4.getX(),p4.getY(),p4.getZ());
	glVertex3d(p4.getX(),p4.getY(),p4.getZ()+height);
	glVertex3d(p1.getX(),p1.getY(),p1.getZ());
	glVertex3d(p1.getX(),p1.getY(),p1.getZ()+height);	
	glEnd();	




}

inline
static void DrawSphere(GLuint nDLid, const ARCVector3& _pos, DistanceUnit _radius)
{
	glPushMatrix();
	glTranslated(_pos[VX], _pos[VY], _pos[VZ]);
	glScalef(static_cast<GLfloat>(_radius), static_cast<GLfloat>(_radius), static_cast<GLfloat>(_radius));
	if(glIsList(nDLid))
		glCallList(nDLid);
	glPopMatrix();
}

inline
static void DrawCone(GLuint nDLid, double x, double y, double z, double radius, double h)
{
	glPushMatrix();
	glTranslated(x,y,z);
	glScalef(static_cast<GLfloat>(radius), static_cast<GLfloat>(radius), static_cast<GLfloat>(h));
	if(glIsList(nDLid))
		glCallList(nDLid);
	glPopMatrix();

}

inline
static void DrawFlatSquare(double x, double y, double z, double size) {
	glPushMatrix();
	glTranslated(x,y,z);
	glScaled(size,size,size);
	glBegin(GL_QUADS);
	glNormal3f(0.0,0.0,1.0);
	glVertex3f(-0.5, -0.5, 0.0);
	glVertex3f(+0.5, -0.5, 0.0);
	glVertex3f(+0.5, +0.5, 0.0);
	glVertex3f(-0.5, +0.5, 0.0);
	glEnd();
	glPopMatrix();
}

inline
static void DrawCube(double x, double y, double z, double size) {
	glPushMatrix();
	glTranslated(x,y,z);
	glScaled(size,size,size);
	glBegin(GL_QUADS);
	glNormal3f(0.0,0.0,1.0);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(+0.5, -0.5, -0.5);
	glVertex3f(+0.5, +0.5, -0.5);
	glVertex3f(-0.5, +0.5, -0.5);

	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(+0.5, -0.5, 0.5);
	glVertex3f(+0.5, +0.5, 0.5);
	glVertex3f(-0.5, +0.5, 0.5);

	glNormal3f(0.0,-1.0,0.0);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(+0.5, -0.5, -0.5);
	glVertex3f(+0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);

	glNormal3f(0.0,1.0,0.0);
	glVertex3f(-0.5, +0.5, -0.5);
	glVertex3f(+0.5, +0.5, -0.5);
	glVertex3f(+0.5, +0.5, 0.5);
	glVertex3f(-0.5, +0.5, 0.5);

	glNormal3f(1.0,0.0,0.0);
	glVertex3f(+0.5, -0.5, -0.5);
	glVertex3f(+0.5, +0.5, -0.5);
	glVertex3f(+0.5, +0.5, 0.5);
	glVertex3f(+0.5, -0.5, 0.5);

	glNormal3f(-1.0,0.0,0.0);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, +0.5, -0.5);
	glVertex3f(-0.5, +0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);

	glEnd();
	glPopMatrix();
}

inline
static void DrawElevTray(double x, double y, double z, double xsize, double ysize, double rot) {
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(rot, 0.0,0.0,1.0);
	glScaled(xsize,ysize,1.0);
	glBegin(GL_QUADS);
	//floor
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	//length wall
	glNormal3f(0.0f,-1.0f,0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glVertex3f(-1.0f, 0.0f, 100.0f);
	//length wall
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 100.0f);
	glVertex3f(-1.0f, 1.0f, 100.0f);
	//width wall
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	//width wall
	glNormal3f(-1.0f,0.0f,0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 100.0f);
	glVertex3f(-1.0f, 0.0f, 100.0f);

	glEnd();
	glPopMatrix();

}

inline
static void DrawCar(double x, double y, double z, double xsize, double ysize, double rot, int carcount) {
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(rot, 0.0,0.0,1.0);
	glScaled(xsize*carcount,ysize,1.0);
	glBegin(GL_QUADS);
	//floor
	glNormal3f(0.0,0.0,1.0);
	glVertex3f(-0.5, -0.5, 0.0);
	glVertex3f(+0.5, -0.5, 0.0);
	glVertex3f(+0.5, +0.5, 0.0);
	glVertex3f(-0.5, +0.5, 0.0);
	//length wall
	glNormal3f(0.0,-1.0,0.0);
	glVertex3f(-0.5, -0.5, 0.0);
	glVertex3f(+0.5, -0.5, 0.0);
	glVertex3f(+0.5, -0.5, 100.0);
	glVertex3f(-0.5, -0.5, 100.0);
	//length wall
	glNormal3f(0.0,1.0,0.0);
	glVertex3f(-0.5, +0.5, 0.0);
	glVertex3f(+0.5, +0.5, 0.0);
	glVertex3f(+0.5, +0.5, 100.0);
	glVertex3f(-0.5, +0.5, 100.0);
	//width wall
	glNormal3f(1.0,0.0,0.0);
	glVertex3f(+0.5, -0.5, 0.0);
	glVertex3f(+0.5, +0.5, 0.0);
	glVertex3f(+0.5, +0.5, 100.0);
	glVertex3f(+0.5, -0.5, 100.0);
	//width wall
	glNormal3f(-1.0,0.0,0.0);
	glVertex3f(-0.5, -0.5, 0.0);
	glVertex3f(-0.5, +0.5, 0.0);
	glVertex3f(-0.5, +0.5, 100.0);
	glVertex3f(-0.5, -0.5, 100.0);

	//add carcount-1 dividers
	for(int i=1; i<=carcount-1; i++) {
		double xpos = -0.5+i*(1.0/carcount);
		glNormal3f(1.0,0.0,0.0);
		glVertex3f(static_cast<GLfloat>(xpos), -0.5, 0.0);
		glVertex3f(static_cast<GLfloat>(xpos), +0.5, 0.0);
		glVertex3f(static_cast<GLfloat>(xpos), +0.5, 100.0);
		glVertex3f(static_cast<GLfloat>(xpos), -0.5, 100.0);
	}

	glEnd();
	glPopMatrix();
}

static void inline DrawWidthQuad(const Point& a,const Point& b, double dwidth ){
	ARCVector2 dir = ARCVector2(b-a);
	Point center = (a + b) * 0.5;
	double angle  = dir.AngleFromCoorndinateX();
	double len = dir.Length();
	glPushMatrix();
	glTranslated(center.getX(),center.getY(),0);
	glRotated(angle,0,0,1);
	glScaled(len,dwidth,0);
	glRectd(-0.5,-0.5,0.5,0.5);
	glPopMatrix();

}



static void inline DrawFlatSquare(double width, double length){
	double halfw = 0.5 * width;
	double halfl = 0.5 * length;
	glBegin(GL_QUADS);
	glVertex2d(-halfw,-halfl);
	glVertex2d(halfw, -halfl);
	glVertex2d(halfw,halfl);
	glVertex2d(-halfw,halfl);
	glEnd();
}

class ARCGLRender
{
public:
	static void DrawArrowPath(const Path& path,double width );
	static void DrawTransformShape(GLint dislist, ARCVector3 transf, ARCVector3 vdir, ARCVector3 scale );
	
	static void DrawWidthPath(const Path& path, double width);
	static void DrawHeadArrow(const Path& path, double  width);
};


#endif