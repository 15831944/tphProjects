#pragma once


class ARCPipe;
class ARCPath3;
class CPath2008;
class ARCColor3;
class Path;


class CommonShapes
{
public:
	CommonShapes(Ogre::ManualObject* pObj)
	{		
		mpObj= pObj;
	}

	CString msMat;
	Ogre::ColourValue mColor;	
	Ogre::ManualObject* mpObj;


	void BuildCylinder3D(const CPath2008& path, double drad);

	void BuildTexturePipe2D(const ARCPipe& pipe,double duTexF, double duTextT);

	void DrawWallShape(const ARCPipe& pipe, double dHeight, bool bBottom = false);

	void DrawSquare(const ARCVector3& pos, const ARCVector3& dir, double dwidth, double dlen);
	void DrawSquareFlip(const ARCVector3& pos, const ARCVector3& dir, double dwidth, double dlen);

	//draw arrow 
	void DrawPipeArrow(const ARCPath3&path, double dwidth, double dArrowHeader);

	//draw arrow line
	void DrawPipeArrowLine(const ARCPath3&path, double dwidth, double dArrowHeader);

	



};
///
