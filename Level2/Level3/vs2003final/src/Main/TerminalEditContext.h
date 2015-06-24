#pragma once
#include "common\IEnvModeEditContext.h"

class CTerminalEditContext : public ITerminalEditContext
{
public:
	CTerminalEditContext(void);
	~CTerminalEditContext(void);

	//virtual

	virtual CRenderFloorList GetFloorList()const; 
	virtual std::vector<CPipeBase*> GetPipeList()const;

	virtual ARCPath3 GetTerminalVisualPath(const Path& flrpath,bool bInterMidFlr = false);//
	virtual void GetMovingSideWalkWidthSpeed(const ProcessorID& procid, double& doutspeed, double& doutwidth);
	virtual void GetFloorAltOn(std::vector<double>& dAlt, std::vector<BOOL>&bOn);

	virtual double GetActiveFloorHeight()const;

	virtual bool isTerminalProcessorDisplay(int ProcessorType);
	virtual CString GetTerminalStructureBaseFilename(CString texturename);
	bool GetAllRailWayInfo(std::vector<RailwayInfo*>& railWayVect);
	InputTerminal* getInput();
};
