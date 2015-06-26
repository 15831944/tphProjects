#pragma once
#include "renderprocessor3d.h"



class RENDERENGINE_API  CRenderBridge3D :
	public CRenderProcessor3D
{
public:
	SCENE_NODE(CRenderBridge3D, CRenderProcessor3D)
	virtual void Setup3D(CProcessor2Base* pProc, int idx);
	void DrawConnector( int idx, bool bMoving, const CPoint2008& ptFrom, const CPoint2008& ptTo,double dwidth,const ARCColor3& c);
protected:
	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	virtual void Update3D(CProcessor2Base* pProc);


	enum ConnectStatus
	{
		_moving,
		_static,
		_empty,
	};
	std::vector<ConnectStatus> m_conState; //connector state  true: moving, false : 
	
};
