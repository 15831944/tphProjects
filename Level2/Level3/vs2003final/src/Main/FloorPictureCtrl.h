#ifndef FLOOR_PICTURE_H
#define FLOOR_PICTURE_H
#include "common\ARCColor.h"
#include "Common\ARCVector.h"
#include "common\path.h"
#define  PICKFROMMAP    WM_USER+10001 
class CxImage;
class CFloorPictureCtrl: public CWnd
{
	DECLARE_DYNAMIC(CFloorPictureCtrl)
public:
	CFloorPictureCtrl();
	virtual ~CFloorPictureCtrl();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC );
	afx_msg void OnPaint();

protected:
	void DrawPicture();
	void DrawReferenceLine();
	void GenPicTexture();
	void SetupViewport(int cx,int cy);
	int SetupScene();
	void RenderSecen();
	BOOL SetupPixelFormat(HDC hDC, DWORD dwFlags);
	ARCVector3 GetWorldMousePos(const CPoint& pt);
	BOOL LoadImage(CString sFileName);
	CString FindExtension(const CString& name);
	CString RemoveExtension(const CString& name);
public:
	void Create(CRect rect, CWnd *parent);
	void SetFilePath(const CString& _strPath){m_strPicturePath = _strPath;}
	void SetPoint(ARCVector2 pt);
	const ARCVector2& GetPoint()const {return m_pt;}
	void SetHeight(double dHeight){m_dHeight = dHeight;}
	double GetHeight() {return m_dHeight;}
	void SetWidth(double dWidth){m_dWidth = dWidth;}
	double GetWidth() {return m_dWidth;}
	void SetImageWidth(double dImageWidth){m_dImageWidth = dImageWidth;}
	double GetImageWidth()const{return m_dImageWidth;}
	void SetImageHeight(double dImageHeight){m_dImageHeight = dImageHeight;}
	double GetImageHeight()const{return m_dImageHeight;}
	void SetReferenceLine(BOOL bReferenceLine){m_bReferenceLine = bReferenceLine;m_refLine.clear();}	
	Path& GetPath(){return m_refLinePath;}
	void SetPath(Path path);
	void SetPictureLoad(BOOL bLoad) {m_bPicLoaded = bLoad;}
	void SetTextureValid(BOOL bValid) {m_bPicTextureValid = bValid;}
	void AutoAdjustCameraFocusDistance(double dFocusPlaneAlt);
	void SetPan(BOOL bPan){m_bPan = bPan;}
	BOOL GetPan()const {return m_bPan;}
	void SetZoom(BOOL bZoom){m_bScale = bZoom;}
	BOOL GetZoom()const {return m_bScale;}
	void SetMove(BOOL bMove){m_bMove = bMove;}
private:
	CString m_strPicturePath;
	GLuint m_iPicTexture;
	BOOL m_bPicTextureValid;
	BOOL m_bPicLoaded;
	BOOL m_bReferenceLine;
	BOOL m_bReferLine;
	ARCVector2 m_pt;
	double m_offsetX;
	double m_offsetY;
	Path m_refLine; //only two point
	Path m_refLinePath;
	std::vector<ARCVector2>m_PointList;
	double m_dWidth;
	double m_dHeight;
	double m_dImageWidth;
	double m_dImageHeight;
	BOOL m_bScale;
	BOOL m_bPan;
	BOOL m_bMove;

	HGLRC m_hRC;	//opengl RC
	CDC* m_pDC;		//windows DC
	int m_iPixelFormat;	//pixel format used
	double m_dAspect;	//aspect ratio of cwnd
	CPoint	m_ptFrom;	// screen point where mouse button was last clicked
	ARCVector3 m_ptMousePos;
	ARCVector3 m_ptMouseWorldPos;
	BOOL m_bLeftButtonDown;

	//camera position
	ARCVector3 m_vLocation;		// x, y, z location
	//camera direction
	ARCVector3 m_vLook;			// viewing direction vector
	//camera up
	ARCVector3 m_vUp;			// up vector (local z)
	double m_dFocusDistance;
	CxImage* m_pImage;
};

#endif