#pragma once

class ARCPath3;
class CPath2008;
class ARCVector3;
class C3DView;
class ARCPipe;

class CAirsideThickness3D
{
public:
	CAirsideThickness3D(void);
	~CAirsideThickness3D(void);
protected:
	double m_dThick;
public:
	double GetThickness();
	//modify Altitude (multiple scale factor)
	CPath2008 ModifyPathAlt(CPath2008& path,C3DView* pView);
	ARCPath3 ModifyPathAlt(ARCPath3& path,C3DView* pView);

    CPath2008 ReviewPath(const CPath2008& path);
    
	// Add the Thickness
	void AddThickness(const CPath2008& path,CPath2008& newPath);
	ARCPath3 AddThickness(ARCPath3& path);

	ARCPath3 AddWallAlt(ARCPath3& path);

	void SetWallPath(const CPath2008& path, CPath2008& bottomPath, CPath2008& toPath);

	void RenderWall(C3DView* pView, const CPath2008& path,double pathWidth);

	// Draw the side of path
	void RenderSidePath(C3DView* pView, const ARCPipe& abovePipe, const ARCPipe& belowPipe);
	void RenderSidePath(const ARCPath3& aboveLeft, const ARCPath3& aboveRight, const ARCPath3& belowLeft, const ARCPath3& belowRight);
	void RenderSubSidePath(const ARCPath3& abovePath, const ARCPath3& belowPath,BOOL bFlag);

	void RenderHeadTrailPath(const ARCPipe& abovePipe, const ARCPipe& belowPipe);

	// calculate the normalize
	void CalculateNormalize(const ARCPath3& abovePath, const ARCPath3& belowPath,ARCVector3& vNormal,int index,BOOL bFlag);
	void RenderPathWall(C3DView* pView, const ARCPipe& abovePipe);
	void RenderSubPathWall(const ARCPath3& subPath);
	void RenderBridge(C3DView* pView, const CPath2008& path,double pathWidth);
	void RenderCube(const CPath2008& path, int index, double pathWidth,ARCVector3& vHeight);
	void RenderCylinder(const CPath2008& path, int index, double pathWidth,const ARCVector3& vHeight);
	void GetHorizonPoint(const ARCVector3& vVector0, const ARCVector3& vVector1, ARCVector3& vVector);
};
