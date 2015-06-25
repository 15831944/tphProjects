#pragma once
#include "ProjectEditContext.h"
#include <CommonData/QueryData.h>
#include "RenderFloor.h"
class InputAirside;
class InputLandside;
class InputTerminal;

class ILandsideEditContext;
class IAirsideEditContext;
class ITerminalEditContext;
class ILayoutObject;


class CNewMenu;

class IEnvModeEditContext : public CProjectEditContext
{
public:
	virtual ILandsideEditContext * toLandside(){ return NULL; };
	virtual ITerminalEditContext* toTerminal(){ return NULL; }
	virtual IAirsideEditContext* toAirside(){ return NULL; }


	IEnvModeEditContext();
	//clear data for transfer between dlg and view
	virtual void ClearTempData(){}	



	virtual void DoneCurEditOp();	
	virtual void CancelCurEditOp();
	virtual void StartEditOp(CEditOperationCommand* pOp, bool bCancelPre = false);

	//select object
	virtual ILayoutObject* getLastSelectObject(){ return NULL; }; //get most active select object
	virtual int getSelectCount()const{ return 0; }
	virtual ILayoutObject* getSelectObject(int idx){ return NULL; }
	virtual void selectObject(ILayoutObject* pObj, bool bMultiSelct = false){}

	//////////////////////
	QueryData& setRClickItemData(const QueryData& itemdata);
	QueryData& getRClickItemData(){ return mRClickItemData; }
	const QueryData& getRClickItemData()const{ return mRClickItemData; }
	void clearRClickItemData();

	//void 
	void NotifyViewCurentOperation();

	//context menu
	virtual	CNewMenu* GetContexMenu(CNewMenu& menu){ return NULL; }
	virtual void OnCtxDeleteRClickItem(){}

	
	virtual double GetWorkingHeight()const;
	virtual void UseTempWorkingHeight(double d);
	//use Active Floor height as the working height
	virtual void UseActiveFloorHeight(){ m_bUseTempHeight = false; }

protected:
	QueryData mRClickItemData;
	CEditOperationCommand* mpCurEditOp;

	virtual double GetActiveFloorHeight()const=0;
	double m_TempWorkingHeight; // the same as active floor's altitude
	bool m_bUseTempHeight;

};

//////////////////////////////////////////////////////////////////////////
class Path;
class ARCPath3;
class ProcessorID;
class CPipeBase;
class RailwayInfo;

class ITerminalEditContext : public IEnvModeEditContext
{
public:
	virtual InputTerminal* getInput()=0;
	ITerminalEditContext* toTerminal(){ return this; }

	virtual bool isTerminalProcessorDisplay(int ProcessorType) = 0;
	virtual CString GetTerminalStructureBaseFilename(CString texturename) = 0;

public:
	virtual CRenderFloorList GetFloorList()const=0;

	virtual ARCPath3 GetTerminalVisualPath(const Path& flrpath,bool bInterMidFlr = false)=0;//
	virtual void GetMovingSideWalkWidthSpeed(const ProcessorID& procid, double& doutspeed, double& doutwidth)=0;
	virtual void GetFloorAltOn(std::vector<double>& dAlt, std::vector<BOOL>&bOn)=0;

	virtual bool GetAllRailWayInfo(std::vector<RailwayInfo*>& railWayVect)=0;

};

//////////////////////////////////////////////////////////////////////////
class IAirsideEditContext : public IEnvModeEditContext
{
public:
	virtual InputAirside* getInput()=0;
	IAirsideEditContext* toAirside(){ return this; }
};
