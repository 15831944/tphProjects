#pragma once
#include "OgreFont.h"

namespace Ogre
{
	class ManualObject;
	class ColourValue;
	class Vector3;
}

//setting of how to draw shapes
class ARCPipe;
class ARCPath3;
class Painter
{
public:
	Painter();
	Painter(const CString& mat);

	bool mbUseVertexColor;
	bool mbBackFace;
	bool mbAutoNorm;
	bool mbAutoTextCoord;
	ARCColor4 color;
	CString msMat;


	Painter& UseVertexColor(const ARCColor4& c){ mbUseVertexColor = true; color = c;  return *this; }
	
};
//util for ogre manual object ,adapter with arcport system
class ManualObjectUtil
{
public:
	ManualObjectUtil(Ogre::ManualObject* p);

	//basic 
	void beginTriangleList(const Painter& painter);
	void beginLineList(const Painter& mat);
	void end();
	void clear();

	//draw basic shapes
	//draw pipe in trianle list, make sure call beginTriangleList() first
	void drawPath(const ARCPipe& pipe,double texcordL=0, double texcordR = 1, double texLenScale = 1);
	void drawPipe(const ARCPath3& leftside, const ARCPath3& rightside, double texcordL=0, double texcordR = 1, double texLenScale = 1);
	void drawQuad(const ARCVector3& p1,  const ARCVector3& p2,const ARCVector3& p3,const ARCVector3& p4);
	
	//box with top open
	void drawOpenBox(double xsize, double ysize,double dheight);

	//draw text
	void beginText(  double charHegiht,const CString& fontName = "ArialFont");
	void drawTopLeftAligmentText( const CString& caption );
	void drawCenterAligmentText(const CString& caption);
	void setCharHeight(double  height){ mCharHeight = height; }
	void setColor(const ARCColor4& c){ mCurPen.color = c; }

	//transform function
	void setIdentTransform();
	void translate(const ARCVector3& v);
	void scale( const ARCVector3& scale );
	void rotate( const ARCVector3& axis, double dangle );
	void rotate(const ARCVector3& vfrom, const ARCVector3& vto);
	void setTranslate( const ARCVector3& v );


protected:
	Ogre::ManualObject* pObj;	
	void setTransform(const Ogre::Matrix4& transf){ mTransform = transf; }

	Painter mCurPen;

	Ogre::Matrix4 mTransform;

	//text
	double	mCharHeight;
	Ogre::FontPtr mCurFont;
	CString perpareFont( const CString& mName );
	//get char width of current height
	double getCharWidth( int c  )const;
	double getStringWidth(const CString& caption)const;
	//return char  width
	double drawChar( int c ,Ogre::Real left, Ogre::Real top );

	void _drawQuad(const Ogre::Vector3& v0, const Ogre::Vector3& v1,const Ogre::Vector3& v2,const Ogre::Vector3& v3);
	
};