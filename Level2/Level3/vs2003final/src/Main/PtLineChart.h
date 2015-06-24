#pragma once

#include <vector>
#include "../inputs/IN_TERM.H"
class Path;

#define ARROW_LAYER_COUNT			6
#define ARROW_WIDTH					(ARROW_LAYER_COUNT*2)
#define POINT_RECT_SIZE				4
#define WM_UPDATE_VALUE				WM_USER + 1024

class CPtLineChart : public CStatic
{
	DECLARE_DYNAMIC(CPtLineChart)

public:
	CPtLineChart();
	virtual ~CPtLineChart();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	// Set Data
	void SetData(std::vector<double>& vPointsXPos, std::vector<double>& vPointsYPos,std::vector<double>& vVertCoorData,Path* _pPath,CStringArray* pStrArrFloorName ,bool bRefresh=FALSE,EnvironmentMode envMode = EnvMode_Terminal);
	CRect CalcCoorRect(CRect rectWnd,bool bHorizontal);
	// Draw Coordinate
	void DrawCoordinate(CDC* pDC,CRect rectWnd,bool bHorizontal);
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
	Path* m_pPath;
	CStringArray* m_pStrArrFloorName;
	int m_nLeftestPos;
	int m_nBottomestPos;
	int m_nTopestPos;
	double m_fPixelPerUnitH;
	double m_fPixelPerUnitV;

	EnvironmentMode m_envMode;
public:
	int m_nNearestYIndex;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CRect  CalcArrow(CRect rectCoor, bool bHorizontal);
	void DrawArrow(CDC* pDC, CRect rectCoor, bool bHorizontal);
	void DrawTick(CDC* pDC,CRect rectCoor, bool bHorizontal);
	void DrawPoint(CDC* pDC,int nX,int nIndex);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int m_nSeledPointIndex;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	CRect DrawPointBox(CDC* pDC, int nX, int nY,BOOL bSeled=FALSE);
	CPoint m_ptLastDraged;
	bool m_bDragging;
	bool m_bCursorMove;
	int GetNearestYIndex(CPoint& ptTemp);
	void UpdataChart(int nStartIndex, int nEndIndex);
private:
	void calculateLengthAngle( int _idx );
};


