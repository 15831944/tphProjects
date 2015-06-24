#pragma once

class CPoint2008;

class ARCColor3;
class CTermPlanDoc;

class CRenderUtility
{
public:
	CRenderUtility(void);
	~CRenderUtility(void);
	static void DrawBridgeConPtr( int idx,BridgeConnector* pConnector, CTermPlanDoc* pDoc , const ARCColor3& color,double dAlt);
	static void SelectBridgeConPtr(const BridgeConnector::ConnectPoint& p,double dAlt);

	static void DrawStair( const CPoint2008& vFrom, const CPoint2008& vTo,double dwidth );
};
