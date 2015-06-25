#pragma once

class CRenderFloor;
class IFloorDataSet 
{
public:
	virtual void LoadData(CString _projpath, int nProjId)=0;
	virtual void SaveData(CString _projpath, int nProjId)=0;

	virtual int GetCount()const=0;
	virtual CRenderFloor * GetFloor(int idx)=0;
	virtual const CRenderFloor* GetFloor(int idx)const=0;


	virtual double GetVisualHeight(double dfloorIndex)const;
	virtual double GetRealHeight(double dfloorIndex)const;
};
