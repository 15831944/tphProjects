#include "stdafx.h"


#include "ShapeRenderer.h"
#include <Common/ShapeGenerator.h>

static void DrawPath(const Path& pPathToDraw, double dAlt, BOOL bLoop = FALSE)
{
	static const double delta = 10.0;
	int c = pPathToDraw.getCount();
	if(c == 1) {
		Point pt = pPathToDraw.getPoint(0);
		glBegin(GL_LINE_LOOP);
		glVertex3d(pt.getX()-delta, pt.getY()-delta, dAlt);
		glVertex3d(pt.getX()-delta, pt.getY()+delta, dAlt);
		glVertex3d(pt.getX()+delta, pt.getY()+delta, dAlt);
		glVertex3d(pt.getX()+delta, pt.getY()-delta, dAlt);
		glEnd();
	}
	else {
		if(bLoop)
			glBegin(GL_LINE_LOOP);
		else
			glBegin(GL_LINE_STRIP);
		Point* pts = pPathToDraw.getPointList();
		for(int i=0; i<c; i++) {
			glVertex3d(pts[i].getX(), pts[i].getY(),dAlt);
		}
		glEnd();
	}
}

static void DrawHeadArrow(const Path& pPathToDraw, double dAlt)
{
	static const double dx = 20.0;
	static const double dy = 40.0;
	int c = pPathToDraw.getCount();
	if(c > 1) {

		Point pt =  pPathToDraw.getPoint(c-1);
		Point l(pPathToDraw.getPoint(c-2), pPathToDraw.getPoint(c-1));
		glPushMatrix();
		glTranslated(pt.getX(), pt.getY(), dAlt);
		glRotated(-l.getHeading(), 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);
		glVertex3d(-dx, -dy, 0.0);
		glVertex3d(dx, -dy, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glEnd();
		glPopMatrix();
	}
}




void ARCGLRender::DrawTransformShape( GLint dislist, ARCVector3 transf, ARCVector3 vdir, ARCVector3 scale )
{
	glPushMatrix();
	glTranslated(transf.x,transf.y, transf.z);
	//glRotated();
	glScaled(scale.x,scale.y, scale.z);
	
	glCallList(dislist);

	glPopMatrix();
}

void ARCGLRender::DrawArrowPath( const Path& path, double width )
{
	DrawWidthPath(path,width*0.5);
	// draw head arrow
	DrawHeadArrow(path,width*0.5);
}	

void ARCGLRender::DrawWidthPath( const Path& path, double width )
{
	ShapeGenerator::widthPath thewidthPath;
	ShapeGenerator::GenWidthPath(path,width,thewidthPath);
	//
	int nCnt = (int)thewidthPath.leftpts.size();
	glBegin(GL_QUAD_STRIP);
	for(int i =0 ;i< nCnt;i++){
		glVertex3dv(thewidthPath.leftpts[i]);
		glVertex3dv(thewidthPath.rightpts[i]);
	}		
	glEnd();
}

void ARCGLRender::DrawHeadArrow( const Path& path, double width )
{
	static const double dx = width * 2;
	static const double dy = width * 4;
	int c = path.getCount();
	if(c > 1) {

		Point pt =  path.getPoint(c-1);
		Point l(path.getPoint(c-2), path.getPoint(c-1));
		glPushMatrix();
		glTranslated(pt.getX(), pt.getY(), pt.getZ());
		glRotated(-l.getHeading(), 0.0, 0.0, 1.0);
		glBegin(GL_TRIANGLES);
		glVertex3d(-dx, -dy, 0.0);
		glVertex3d(dx, -dy, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glEnd();
		glPopMatrix();
	}
}