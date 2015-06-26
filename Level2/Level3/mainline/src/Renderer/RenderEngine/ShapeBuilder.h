#pragma once
#include "AfxHeader.h"
#include "./3DNodeObject.h"
// #include ".\ShapeTransformer.h"

class ARCPipe;
class ARCPath3;
class CPath2008;
class ARCColor3;
class Path;


class TexCoordMapper
{
public:
	virtual ARCVector2 getCoord2D(const ARCVector3& v)=0;
};
class OgrePainter
{
public:
	OgrePainter();
	bool mbUseColor;
	bool mbBackFace;
	bool mbNorm;
	Ogre::ColourValue mColor;
	Ogre::String msmat;

	bool mbTexture;
	TexCoordMapper* mpTexMap;

	OgrePainter& SetMat(const CString& s){ msmat = s; return *this; }

	OgrePainter& UseColor(const Ogre::ColourValue& c ){ mbUseColor = true; mColor = c;  return *this ; }
	OgrePainter& DisableColor(){ mbUseColor = false; return *this ; }
	OgrePainter& BackFace(bool b){ mbBackFace  = b;return *this ; }
	OgrePainter& UseTextureCoord(TexCoordMapper* pMapper){ mpTexMap = pMapper; return *this ;}
};
//wrapper class of manual object
class OgreManualObject
{
public:
	OgreManualObject(Ogre::ManualObject* pObj);

	void DrawWallShape(const ARCPipe& pipePath, double dOffsetZ, const OgrePainter& painter);
protected:
	Ogre::ManualObject* m_pObject;
};


//
class CShapeBuilder
{
protected:
public:
	static void DrawWallShape(Ogre::ManualObject* pObj,const CString& matName,const ARCPipe& pipepath, double dheight);
	static void DrawPipe(Ogre::ManualObject* pObj,const ARCPipe& pipepath,bool bBackFace=false);
	static void DrawPipeOutLine(Ogre::ManualObject* pObj,const ARCPipe& pipepath,const OgrePainter& option);
	//draw stair steps
	static void DrawStair(Ogre::ManualObject* pObj,const ARCColor3& color,const ARCPipe& pipe);
	static void DrawSteps(Ogre::ManualObject* pObj,const ARCPath3& path,double dWidth,const ARCColor3& color, bool bTex= false);
	static void DrawStairSide(Ogre::ManualObject* pObj,const ARCPipe& pipepath);

	//
	static void DrawEscalator(Ogre::ManualObject* pObj,const ARCPipe& pipepath,const ARCColor3& color);
	static void DrawEscalatorNewewl(Ogre::ManualObject* pObj,const ARCPath3& path,const ARCColor3& color);

	//stat
	static void DrawArea(Ogre::ManualObject* pObj,CString matName, const CPath2008& path,const ARCColor3& color);
	static void DrawCube(const ARCVector3& vMin, const ARCVector3& vMax, bool bBackFace);

	static void DrawPath(Ogre::ManualObject* pObj,CString matName, const CPath2008& path,const ARCColor3& color);	

	static void DrawPipe(Ogre::ManualObject* pObj, const CPath2008& path1eft, const CPath2008& pathright, const OgrePainter& option);

	static void DrawArrowPath(Ogre::ManualObject* pObj, const Ogre::Vector3& ptFrom, const Ogre::Vector3& ptTo, const OgrePainter& option,double dLength = 40.0, double dWidth = 40.0);

};


class TextureClips;
class ProcessorBuilder
{
public:
	static void DrawPath(Ogre::ManualObject* pObj, const Path* path, const Ogre::ColourValue& clr, double dAlt  =0 , bool bLoop = false);
	static void DrawArrow(Ogre::ManualObject* pObj, const Point& ptFrom, const Point& ptTo, const Ogre::ColourValue& clr, double dLength = 40.0, double dWidth = 40.0);
	static void DrawHeadArrow(Ogre::ManualObject* pObj, const Path* path, const Ogre::ColourValue& clr);
	static void DrawTailArrow(Ogre::ManualObject* pObj, const Path* path, const Ogre::ColourValue& clr);
	static void DrawArea(Ogre::ManualObject* pObj, const Path* path,const Ogre::String& mat,const Ogre::ColourValue& clr);

	static void DrawColorPath(Ogre::ManualObject* pObj, const Ogre::ColourValue& clr, Ogre::OgreUtil::ColorTemplateType ctt, const ARCPath3&path, double dWidth, bool bLoop = false, double dZOffset = 0.0 );
	static void DrawColorPath(Ogre::ManualObject* pObj, const Ogre::ColourValue& clr, const ARCPath3&path, double dWidth, bool bLoop = false, double dZOffset = 0.0 );
	static void DrawColorPath( Ogre::ManualObject* pObj, const ARCPipe& pipe, CString matName);
	static void DrawColorPath( Ogre::ManualObject* pObj, const ARCPipe& pipe, CString matName, Ogre::ColourValue color );
	static void DrawColorCircle(Ogre::ManualObject* pObj, const Ogre::ColourValue& clr, Ogre::OgreUtil::ColorTemplateType ctt, const ARCVector3& center, double dMaxRadius, double dMinRadius, double dHeight );
	static void DrawColorCircle(Ogre::ManualObject* pObj, const Ogre::ColourValue& clr, const ARCVector3& center, double dMaxRadius, double dMinRadius, double dHeight );

	static void DrawTexturePath(Ogre::ManualObject* pObj, const Ogre::String& texmat, const Ogre::ColourValue& clr, const ARCPath3&path ,double dWidth, double dSideHeight, bool bLoop = false );
	static void DrawTexturePath(Ogre::ManualObject* pObj, const Ogre::String& texmat, const Ogre::ColourValue& clr, const ARCPath3&path, double dStartWidth, double dEndWidth, double dSideHeight );
	static void DrawTexturePath(Ogre::ManualObject* pObj, const Ogre::String& texmat, const Ogre::ColourValue& clr, const ARCPipe& pipe, double dSideHeight );
	static void DrawTextureCircle(Ogre::ManualObject* pObj, const Ogre::String& texmat,const Ogre::ColourValue& clr, const ARCVector3& center, double dMaxRadius, double dMinRadius, double dSideHeight );
	static void DrawTextureClips(Ogre::ManualObject* pObj,const TextureClips& texclips,const Ogre::ColourValue& clr, double dHeight);

	static void DrawTextureRepeatedPath(Ogre::ManualObject* pObj,const Ogre::String& texmat,const Ogre::ColourValue& clr, const ARCPath3&path ,double dWidth,
		double dTexLeft, double dTexRight, bool bLoop = false );
};





