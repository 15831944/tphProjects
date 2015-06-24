#pragma once

#include ".\objectcontrolpoint.h"

class CProcessor2;
class CProcessorPathControlPoint :public PathControlPoint
{
public:
	CProcessorPathControlPoint(Path& path,int ptID,CProcessor2 * pOwner);
	~CProcessorPathControlPoint(void);

	virtual void DrawSelect(C3DView * pView);
public:
	CProcessor2 * m_pOwner;
};

typedef std::vector< ref_ptr<CProcessorPathControlPoint> > ProcessorPathControlPointList;

class C3DView;
class CProcessorControlPointManager
{
public:
	CProcessorControlPointManager(void);
	~CProcessorControlPointManager(void);
	void DrawSelect(C3DView * pView);
	void DrawSelectServiceLocationControlPoints(C3DView * pView);
	void DrawSelectQueueControlPoints(C3DView * pView);
	void DrawSelectInConstraintControlPoints(C3DView * pView);
	void DrawSelectOutConstraintControlPoints(C3DView * pView);

	void DrawServiceLocationControlPoints(C3DView * pView);
	void DrawQueueControlPoints(C3DView * pView);
	void DrawInConstraintControlPoints(C3DView * pView);
	void DrawOutConstraintControlPoints(C3DView * pView);
	void Draw(C3DView * pView);
	void Initialize(Path * ServiceLocationPath,Path * QueuePath,Path * InConstraintPath,Path * OutConstraint,CProcessor2 * pOwner);
protected:
	void Clear(void);
protected:
	ProcessorPathControlPointList m_vServiceLocationControlPoints;
	ProcessorPathControlPointList m_vQueueControlPoints;
	ProcessorPathControlPointList m_vInConstraintControlPoints;
	ProcessorPathControlPointList m_vOutConstraintControlPoints;
public:
	bool m_bVisible;
};
