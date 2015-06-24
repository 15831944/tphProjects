#pragma once

#include <CommonData/ObjectDisplay.h>

class CPipeDataSet;
class CPipe;
class CPipeDisplay : public CObjectDisplay
{
public:
	CPipeDisplay(CPipeDataSet* pPipes,int nIndex);
	
public:
	int m_nIndex;
	CPipeDataSet* m_pPipes;
	//virtual void OffsetProperties(double dx, double dy);
	//virtual void RotateProperties(double dr, const ARCVector3& point);
	//virtual ARCVector3 GetLocation() const ;
	//virtual int GetFloorIndex()const;
	virtual CBaseObject* GetBaseObject() const;



protected:
	CPipe* GetPipe()const;
};

class CPipeDisplayList : public CObjectDisplayList
{
public:
	CPipeDisplay* GetItem(int idx);
};


