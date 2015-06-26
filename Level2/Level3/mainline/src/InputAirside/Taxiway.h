#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include "Runway.h"
#include <string>


class Runway;
class Stand;
class DeicePad;
#include "HoldPosition.h"

class IntersectionNodesInAirport;
class FilletTaxiwaysInAirport;

class INPUTAIRSIDE_API Taxiway :
	public ALTObject
{
friend class CTaxiway3D;
public:
	typedef ref_ptr<Taxiway> RefPtr;

	static const GUID& getTypeGUID();

public:
	Taxiway(void);
	virtual ~Taxiway(void);
	virtual ALTOBJECT_TYPE GetType()const { return ALT_TAXIWAY; } ;
	virtual CString GetTypeName()const  { return _T("Taxiway"); };

	const CPath2008& GetPath()const;
	void SetPath(const CPath2008& _path);
	// Width in cm
	void SetWidth(double width);
	double GetWidth();
	// Marking
	void SetMarking(const std::string& _sMarking);	
	const std::string& GetMarking() const;
	void SetMarkingPos(double fpos);
	double GetMarkingPos()const;
	//use 
	CPath2008& getPath();

	void GetMarkingPosition(CPoint2008 & startPos, double  & rotateAngle );

	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();
	virtual bool CopyData(const ALTObject* pObj);

	void SetAngle(int nIndex,int nAngle) { m_ObjTaxiSur.getItem(nIndex)->nAngle = nAngle;}
	const int GetAngle(int nIndex){return m_ObjTaxiSur.getItem(nIndex)->nAngle;}

	void SetDistance(int nIndex,double dDistance) {m_ObjTaxiSur.getItem(nIndex)->dDistance = dDistance;}
	const double GetDistance(int nIndex){return m_ObjTaxiSur.getItem(nIndex)->dDistance;}

	void SetOffset(double dOffset) { m_ObjTaxiSur.dOffset = dOffset;}
	const double GetOffset() { return m_ObjTaxiSur.dOffset;}

	ObjSurface& getSurface(){return m_ObjTaxiSur;}
	void SetShow(BOOL bShow) {m_bShow = bShow;}
	BOOL GetShow() { return m_bShow;}

	ObjSurface& GetObjSurface(){ return m_ObjTaxiSur; }
private:
	CPath2008 m_path;
	double m_dWidth; 
	std::string m_sMarking; 
	double m_fMarkingPos;
	ObjSurface m_ObjTaxiSur;
	BOOL m_bShow;
//database operation
public:

	 void ReadRecord(CADORecordset& adoRecordset);
	
	 void ReadObject(int nObjID);
	int SaveObject(int AirportID);
	virtual void UpdateObject(int nObjID);
	void UpdatePath();
	virtual void DeleteObject(int nObjID);
	
	void ReadSurfaceData();
	int SaveSurfaceData();
	void UpdateSurfaceData();
	void DeleteSurfaceData();

	IntersectionNodeList GetIntersectNodes()const;

	int GetHoldPositionsIndex(const IntersectionNode& intersect, bool bfirst)const;


	bool GetHoldPositionLine(int idx, CLine2008& line)const;	

	void InitHoldPositions(const IntersectionNodesInAirport& nodelist);
	void UpdateHoldPositions(const ALTAirport& airport, std::vector<Runway*>vRunways, const FilletTaxiwaysInAirport& fillets, const IntersectionNodesInAirport& nodelist);
	
	static void ExportTaxiways(int nAirportID,CAirsideExportFile& exportFile);

	void ExportTaxiway(CAirsideExportFile& exportFile);

	void ImportObject(CAirsideImportFile& importFile);	
	void CleanAllHoldPosition();

protected:
	int m_nPathID;

	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nTaxiwayID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;


	void CalculateHoldPosition(Runway * pRunway, const ALTAirport& airport,HoldPositionList& list,const IntersectionNodesInAirport& nodelist);
	void CalculateHoldPosition(Taxiway * pTaxiway, HoldPositionList& list,const IntersectionNodesInAirport& nodelist);
	void CalculateHoldPosition(Stand  *pStand, HoldPositionList& list,const IntersectionNodesInAirport& nodelist);
	void CalculateHoldPosition(DeicePad* pDeicepad ,HoldPositionList& list,const IntersectionNodesInAirport& nodelist);


	//IntersectNode
	void UpdateIntersectNodeWith(Taxiway * pTaxiway);

public:	

	mutable HoldPositionList m_vAllHoldPositions;

	void GetAllHoldPositions(HoldPositionList& holdlist)const;	


protected:
	void UpdateHoldPositionsToDB();
	//void UpdateIntersectionList(IntersectNodeList& lstIntersecNode) const;	
	//void UpdateHodePositionLists(IntersectNodeList& lstIntersecNode) const;
	//void UpdateHodePositionList(HoldPositionList& holdPositionList,HoldPositionList& dbHoldPositionList,IntersectNodeList& lstIntersecNode) const;

};
