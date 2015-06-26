#pragma once

#include "Terminal3DObject.h"

#include <Common/CommandHistory.h>
#include "Terminal3DCommands.h"
#include <common/ARCPath.h>
#include <common/path.h>
#include <common/ProjectEditContext.h>

class ProcessorID;
class RENDERENGINE_API CRender3DContext : public CProjectEditContext
{
public:
	CRender3DContext(void);
	~CRender3DContext(void);

	void StartNewEditOp(Terminal3DCommand::OperationType opType);
	void DoneCurEditOp();
	void CancelCurEditOp();

	//CTerminal3DObject::SharedPtr GetSelObj3D() const { return m_pSelObj3D; }
	void SetSelNodeObj( CModeBase3DObject::SharedPtr pObj3D);
	
	Terminal3DCommand* GetCurCommand() const { return m_pCurCommand; }

	void flipViewLock();
	bool IsViewLocked() const;

	ARCVector3 GetStartPoint() const { return m_ptStart; }
	void SetStartPoint(ARCVector3 val) { m_ptStart = val; }

	virtual ARCPath3 GetTerminalVisualPath(const Path& flrpath,bool bInterMidFlr = false)=0;//
	virtual void GetMovingSideWalkWidthSpeed(const ProcessorID& procid, double& doutspeed, double& doutwidth)=0;
	virtual void GetTermialFloorAlt(std::vector<DistanceUnit>& dAlt)=0;
private:
	//CTerminal3DObject::SharedPtr m_pSelObj3D;	
	Terminal3DCommand* m_pCurCommand;
	ARCVector3 m_ptStart;

	bool m_bViewLock;
};
