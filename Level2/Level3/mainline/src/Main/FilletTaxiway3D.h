#pragma once

class FilletTaxiway;
class C3DView;
class FilletPoint;
class ALTObject3DList;
class IntersectionNodesInAirport;

#include "../InputAirside/FiletTaxiway.h"
#include "SelectionMap.h"
#include "../Common/ARCPath.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
class CFilletPoint3D : public Selectable
{
public:
	typedef ref_ptr<CFilletPoint3D> RefPtr;
	
	CFilletPoint3D( FilletTaxiway& filletTaxiway, int idx);

	virtual SelectType GetSelectType()const;
	virtual ARCPoint3 GetLocation()const;
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void AfterMove();

	void DrawOGL(C3DView * pView);

	FilletPoint& GetFilletPt();

	DistanceUnit GetDistInObj()const;
	IntersectionNode& GetIntersectionNode();

protected:
	FilletTaxiway& m_filletTx;
	int m_idx;


};
/************************************************************************/
/*                                                                      */
/************************************************************************/

class CFilletTaxiway3D : public Selectable
{
public:
	typedef ref_ptr<CFilletTaxiway3D> RefPtr;


	CFilletTaxiway3D(FilletTaxiway& filetTaxiway);

	virtual SelectType GetSelectType()const;
	virtual ARCPoint3 GetLocation()const;

public:

	void RenderLine(C3DView * pView,bool bEdit );
	void RenderSurface( C3DView * pView,bool bEdit);
	void RenderEditPoint(C3DView * pView, bool bSelect);

	void DrawSelect(C3DView *pView);

	bool IsInEdit()const{ return m_bInEditMode; }
	void SetEdit(bool b){ m_bInEditMode = b; }

public: 


	CFilletPoint3D::RefPtr m_filetPt1;
	CFilletPoint3D::RefPtr m_filetPt2;

	FilletTaxiway& m_filetTaxway;
	bool m_bInEditMode;

protected:
	void GenSmoothPath();
	ARCPath3 m_vPath;

};
/************************************************************************/
/*                                                                      */
/************************************************************************/
class CFilletTaxiway3DInAirport
{
public:
	void Init(int nARPID, const IntersectionNodesInAirport& nodeList);

	void UpdateFillets(const IntersectionNodesInAirport& nodelist,const std::vector<int>& vChangeNodeIndexs );

	void RenderSurface(C3DView * pView);
	void RenderLine(C3DView * pView, bool bSelect);

	void DrawSelect(C3DView * pView);

	CFilletTaxiway3D* FindFillet3D(const FilletTaxiway& filet);

	bool IsInEdit()const{return m_bInEdit;}
	void SetEdit(bool b){ m_bInEdit = b; }
	void SetInEditFillet(int id){ m_iInEditFillet = id; }
	CFilletTaxiway3D * GetInEditFillet();

	int GetCount()const{ return (int)m_vFillTaxiway3D.size(); }
	CFilletTaxiway3D* GetFillet3D(int idx)const{ return (CFilletTaxiway3D* )m_vFillTaxiway3D.at(idx).get(); }

	FilletTaxiwaysInAirport m_vFilletTaxiways;

protected:
	std::vector<CFilletTaxiway3D::RefPtr> m_vFillTaxiway3D;
	int m_nARPID;

	int m_iInEditFillet;
	bool m_bInEdit;
};

