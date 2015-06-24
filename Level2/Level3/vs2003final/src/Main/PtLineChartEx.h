#pragma once
#include <vector>
using namespace std;

class CPath2008;

#define ARROW_LAYER_COUNT_EX			6
#define ARROW_WIDTH_EX					(ARROW_LAYER_COUNT_EX*2)
#define POINT_RECT_SIZE_EX				4
#define WM_UPDATE_VALUE_EX				WM_USER + 1088
#define WM_UPDATE_ZOOMVALUE             WM_USER + 1089

class CPtLineChartEx : public CStatic
{
	DECLARE_DYNAMIC(CPtLineChartEx)

public:
	CPtLineChartEx(void);
	virtual ~CPtLineChartEx(void);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	// Set Data
	void SetData(vector<double>& vPointsXPos, vector<double>& vPointsYPos, CPath2008* _pPath, bool bRefresh=FALSE);
	void SetVerticalZoomRate(double dZoomRate);
	double GetVerticalZoomRate(void);
	CRect CalcCoorRect(CRect rectWnd,bool bHorizontal);
	// Draw Coordinate
	void DrawCoordinate(CDC* pDC,CRect rectWnd,bool bHorizontal);

	void SetHeight(int nIndex, double dHeight);
	int GetCurSelectIndex(void);
	// Points' Y position
	std::vector<double> m_vPointYPos;
protected:
	// Points' X position
	std::vector<double> m_vPointXPos;
	// Vertical Coordinate Data
	std::vector<double> m_vVertCoorData;
	// Points' draw rect
	std::vector<CRect> m_vPointRect;
	// path to compute length and angle
	CPath2008* m_pPath;
	//CStringArray* m_pStrArrFloorName;
	int m_nLeftestPos;
	int m_nBottomestPos;
	int m_nTopestPos;
	double m_fPixelPerUnitH;
	double m_fPixelPerUnitV;
	double m_dZoomRate;    //current vertical zoom rate

	CPoint  m_ChartZeroPoint;
	int m_nSeledPointIndex;
	int m_nNearestYIndex;

	BOOL m_bFirstInitControl;
public:
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CRect  CalcArrow(CRect rectCoor, bool bHorizontal);
	void DrawArrow(CDC* pDC, CRect rectCoor, bool bHorizontal);
	void DrawTick(CDC* pDC,CRect rectCoor, bool bHorizontal);
	void DrawPoint(CDC* pDC,int nX,int nIndex);

	void DrawLine(CDC* pDC, double dFromX, double dFromY, double dToX, double dToY);
	void DrawHeightRange(CDC* pDC, CRect rectCoor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	CRect DrawPointBox(CDC* pDC, int nX, int nY,BOOL bSeled=FALSE);
	CPoint m_ptLastDraged;
	bool m_bDragging;
	bool m_bCursorMove;
	int GetNearestYIndex(CPoint& ptTemp);
	//return true if in range, return false if out of range
	BOOL IsHeightInRange(double dHeight);
private:
	void UpdatePointDistanceAndHeight(int nIndex);
	void CalcYPosition(int nNewYPosition);
};
