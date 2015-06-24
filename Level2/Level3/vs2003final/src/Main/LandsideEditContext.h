#pragma once
#include "common/IARCportLayoutEditContext.h"
#include "landside/ILandsideEditContext.h"

//////////////////////////////////////////////////////////////////////////
class CNewMenu;
class CTermPlanDoc;
class CLandsidePortal;
class CLandsideArea;
#define MENU_LEVEL_ID	0x06FF
#define MENU_PARKINGLOT_LEVEL 0x08FF
class CLandsideEditContext : public ILandsideEditContext
{
public:
	virtual InputLandside* getInput()const;

	virtual CNewMenu* GetContexMenu(CNewMenu& menu);
	virtual void OnCtxDeleteRClickItem();

	void OnCtxSnapPointToLevel(int iLevel);
	void OnSnapPointToParkLotLevel(int nLevel);

	//set to floor index
	void OnSetPointToAlt(double d);
	//void OnSetPointToIndexAlt(int idx);
	double GetPointAlt();

	void OnCopyParkLotItem();
	void OnDelParkLotItem();

	void OnCtxNewLandsidePortal();
	void OnCtxEditLandsidePortal(CLandsidePortal *pPortal);
	void OnCtxDelLandsidePortal(CLandsidePortal *pPortal);
	void OnCtxLandsidePortalColor(CLandsidePortal *pPortal);

	void OnCtxNewLandsideArea();
	void OnCtxEditLandsideArea(CLandsideArea *pArea);
	void OnCtxDelLandsideArea(CLandsideArea *pArea);
	void OnCtxLandsideAreaColor(CLandsideArea *pArea);
	//virtual BOOL IsEditMode()const;;

	//int GetRemAltCount()const{ return (int)mRemberAltitudes.size(); }
	////double GetRemAlt(int idx)const{ return mRemberAltitudes.at(idx); }
	//bool AddRemAlt(double dalt);
	//bool hasRemAlt(double dalt)const;

	void UpdateDrawing();

	virtual double GetVisualHeight(double dIndex)const;

	virtual int GetActiveLevel()const;


	CRenderFloorList GetFloorList() const;

	virtual bool isLeftDrive()const;

protected:
	//std::vector<double> mRemberAltitudes;
	virtual double GetActiveFloorHeight()const;

	CTermPlanDoc* GetDoc()const{ return (CTermPlanDoc*)GetDocument(); }
};
