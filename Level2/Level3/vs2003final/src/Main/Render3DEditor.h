#pragma once
#include <renderer\renderengine\render3dcontext.h>

class CRender3DEditor :
	public CRender3DContext
{
public:
	CRender3DEditor(){}
	CRender3DEditor(CDocument* pDoc);
	~CRender3DEditor(void);

	virtual ARCPath3 GetTerminalVisualPath(const Path& flrpath,bool bInterMidFlr);//
	virtual void GetMovingSideWalkWidthSpeed(const ProcessorID& procid, double& doutspeed, double& doutwidth);
	virtual void GetTermialFloorAlt(std::vector<DistanceUnit>& dAlt);
};
