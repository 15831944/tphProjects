#pragma once
#include "altobject.h"
#include "../Common/path2008.h"
#include "RunwayExit.h"
#include "../Common/ARCVector.h"
#include "../common/ARCTypes.h"

class IntersectionNodesInAirport;
class CPoint2008;

class Taxiway;
enum SurfaceType
{
	No_Type,
	ThresHold_Type,
	Lateral_Type,
	EndOfRunway_Type
};

struct SurfaceData 
{
	SurfaceData()
		:nID(-1)
		,nAngle(90)
		,dDistance(0.0)
	{

	}
	int nID;
	int nAngle;
	double dDistance;
};
struct ObjSurface 
{
	ObjSurface()
		:nID(-1)
		,dOffset(0)
	{

	}
	ObjSurface(const ObjSurface& anSurface)
	{
		if(this == &anSurface)
			return;

		vSurface.clear();
		std::vector<SurfaceData*>::const_iterator iter = anSurface.vSurface.begin();
		for (; iter != anSurface.vSurface.end(); ++iter)
		{
			SurfaceData* pData = new SurfaceData(**iter);
			vSurface.push_back(pData);
		}
		dOffset = anSurface.dOffset;
	}
	
	ObjSurface& operator = (const ObjSurface& anSurface)
	{
		if(this == &anSurface)
			return *this;

		clear();
		std::vector<SurfaceData*>::const_iterator iter = anSurface.vSurface.begin();
		for (; iter != anSurface.vSurface.end(); ++iter)
		{
			SurfaceData* pData = new SurfaceData(**iter);
			vSurface.push_back(pData);
		}
		dOffset = anSurface.dOffset;
		return *this;
	}
	
	~ObjSurface(){releaseMemory();}
	int nID;
	double dOffset;
	std::vector<SurfaceData*>vSurface;
	////////method for surfacedata command
	void AddItem(SurfaceData* pSurData){vSurface.push_back(pSurData);}
	void DeleteItem(int nIndex){ASSERT(nIndex > -1 && nIndex < (int)vSurface.size());vSurface.erase(vSurface.begin()+nIndex);}
	SurfaceData* getItem(int nIndex) {ASSERT(nIndex>-1&&nIndex<(int)vSurface.size());return vSurface.at(nIndex);}
	void clear() {releaseMemory();vSurface.clear();}
	void releaseMemory()
	{
		std::vector<SurfaceData*>::iterator iter = vSurface.begin();
		for (; iter != vSurface.end(); ++iter)
		{
			delete (*iter);
		}
	}
	int getCount(){return (int)vSurface.size();}
	SurfaceData* findItem(int nSurfaceID);
	int findIndex(int nSurfaceID)
	{
		int nIndex = 0;
		std::vector<SurfaceData*>::iterator iter = vSurface.begin();
		for (; iter != vSurface.end(); ++iter)
		{
			if (nSurfaceID == (*iter)->nID)
			{
				return nIndex;
			}
			nIndex++;
		}
		return nIndex;
	}

	int GetUniqeID()
	{
		int nID = 0;
		BOOL bFind = TRUE;
		while (bFind)
		{
			bFind = FALSE;
			std::vector<SurfaceData*>::iterator iter = vSurface.begin();
			for (; iter != vSurface.end(); ++iter)
			{
				if (nID == (*iter)->nID)
				{
					bFind = TRUE;
					break;
				}
			}
			nID++;
		}
		return nID;
	}

	void ConveyToSurface(CString& strSurface);
	CString FormatSurface(std::vector<SurfaceData*>&vData);
	//////method for database
	int SaveData(int nObjID,BOOL bFirst,ALTOBJECT_TYPE emALTType,SurfaceType emSurface);
	void UpdateData();
	void ReadData(int nObjID,BOOL bFirst,ALTOBJECT_TYPE emALTType,SurfaceType emSurface);
	void DeleteData();
};

struct ObjRuwayStruct 
{
	ObjSurface ThresHold;
	ObjSurface Lateral;
	ObjSurface EndOfRunway;
};
class ALTAirport;

class INPUTAIRSIDE_API Runway :
	public ALTObject
{

friend class ALTObjectInput;
friend class Runway3D;

public:
	typedef ref_ptr<Runway> RefPtr;
	static const GUID& getTypeGUID();


public:
	Runway(void);
	virtual ~Runway(void);

	explicit Runway(const Runway&);


	virtual ALTOBJECT_TYPE GetType()const { return ALT_RUNWAY; } ;
	virtual CString GetTypeName()const  { return _T("Runway"); };


	//virtual inline ALTOBJECT_TYPE GetObjectType()const { return ALT_RUNWAY; }
	const CPath2008& GetPath()const;
	void SetPath(const CPath2008& _path);
	// Width in cm
	void SetWidth(double width);
	double GetWidth()const;
	// Marking
	void SetMarking1(const std::string& _sMarking);	
	void SetMarking2(const std::string& _sMarking);		

	const std::string& GetMarking1() const;
	const std::string& GetMarking2() const;
	//use 
	CPath2008& getPath();

	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();

	void SetMark1LandingThreshold(double dThreshold);
	double GetMark1LandingThreshold()const;

	void SetMark1TakeOffThreshold(double dThreshold);
	double GetMark1TakeOffThreshold()const;

	void SetMark2LandingThreshold(double dThreshold);
	double GetMark2LandingThreshold()const;

	void SetMark2TakeOffThreshold(double dThreshold);
	double GetMark2TakeOffThreshold()const;


	CPoint2008 GetLandThreshold1Point()const;
	CPoint2008 GetLandThreshold2Point()const;

/////////objsurface
	void SetMark1ThresholdAngle(int nIndex, int nAngle){m_ObjSurface1.ThresHold.getItem(nIndex)->nAngle = nAngle;}
	const int GetMark1ThresholdAngle(int nIndex){return m_ObjSurface1.ThresHold.getItem(nIndex)->nAngle;}
	ObjSurface& GetMark1Threshold() {return m_ObjSurface1.ThresHold;}

	void SetMark1ThresholdDistance(int nIndex,double dDistance) {m_ObjSurface1.ThresHold.getItem(nIndex)->dDistance = dDistance;}
	const double GetMark1ThresholdDistance(int nIndex){return m_ObjSurface1.ThresHold.getItem(nIndex)->dDistance;}
	
	void SetMark1LateralAngle(int nIndex,int nAngle) { m_ObjSurface1.Lateral.getItem(nIndex)->nAngle = nAngle;}
	const int GetMark1LateralAngle(int nIndex) { return m_ObjSurface1.Lateral.getItem(nIndex)->nAngle;}
	ObjSurface& GetMark1Lateral(){return m_ObjSurface1.Lateral;}

	void SetMark1LateralDistance(int nIndex,double dDistance){m_ObjSurface1.Lateral.getItem(nIndex)->dDistance = dDistance;}
	const double GetMark1LateralDistance(int nIndex) { return m_ObjSurface1.Lateral.getItem(nIndex)->dDistance;}

	void SetMark1EndOfRunwayAngle(int nIndex,int nAngle) {m_ObjSurface1.EndOfRunway.getItem(nIndex)->nAngle = nAngle;}
	const int GetMark1EndOfRunwayAngle(int nIndex) {return m_ObjSurface1.EndOfRunway.getItem(nIndex)->nAngle;}
	ObjSurface& GetMark1EnOfRunway(){return m_ObjSurface1.EndOfRunway;}

	void SetMark1EndOfRunwayDistance(int nIndex, double dDistance){m_ObjSurface1.EndOfRunway.getItem(nIndex)->dDistance = dDistance;}
	const double GetMark1EndOfRunwayDistance(int nIndex){return m_ObjSurface1.EndOfRunway.getItem(nIndex)->dDistance;}

	void SetMark1ThresholdOffset(double dOffset){m_ObjSurface1.ThresHold.dOffset = dOffset;}
	const double GetMark1ThresholdOffset(){return m_ObjSurface1.ThresHold.dOffset;}

	void SetMark1LateralOffset(double dOffset) { m_ObjSurface1.Lateral.dOffset = dOffset;}
	const double GetMark1LateralOffset() { return m_ObjSurface1.Lateral.dOffset;}

	void SetMark1EndOfRunwayOffset(double dOffset) {m_ObjSurface1.EndOfRunway.dOffset = dOffset;}
	const double GetMark1EndOfRunwayOffset() {return m_ObjSurface1.EndOfRunway.dOffset;}
//////surface2
	void SetMark2ThresholdAngle(int nIndex,int nAngle){m_ObjSurface2.ThresHold.getItem(nIndex)->nAngle = nAngle;}
	const int GetMark2ThresholdAngle(int nIndex){return m_ObjSurface2.ThresHold.getItem(nIndex)->nAngle;}
	ObjSurface& GetMark2Threshold() {return m_ObjSurface2.ThresHold;}

	void SetMark2ThresholdDistance(int nIndex,double dDistance){m_ObjSurface2.ThresHold.getItem(nIndex)->dDistance = dDistance;}
	const double GetMark2ThresholdDistance(int nIndex){return m_ObjSurface2.ThresHold.getItem(nIndex)->dDistance;}

	void SetMark2LateralAngle(int nIndex,int nAngle) { m_ObjSurface2.Lateral.getItem(nIndex)->nAngle = nAngle;}
	const int GetMark2LateralAngle(int nIndex) { return m_ObjSurface2.Lateral.getItem(nIndex)->nAngle;}

	void SetMark2LateralDistance(int nIndex,double dDistance){m_ObjSurface2.Lateral.getItem(nIndex)->dDistance = dDistance;}
	const double GetMark2LateralDistance(int nIndex){return m_ObjSurface2.Lateral.getItem(nIndex)->dDistance;}
	ObjSurface& GetMark2Lateral(){return m_ObjSurface2.Lateral;}

	void SetMark2EndOfRunwayAngle(int nIndex,int nAngle) {m_ObjSurface2.EndOfRunway.getItem(nIndex)->nAngle= nAngle;}
	const int GetMark2EndOfRunwayAngle(int nIndex) {return m_ObjSurface2.EndOfRunway.getItem(nIndex)->nAngle;}
	ObjSurface& GetMark2EndOfRunway(){return m_ObjSurface2.EndOfRunway;}

	void SetMark2EndOfRunwayDistance(int nIndex,double dDistance){m_ObjSurface2.EndOfRunway.getItem(nIndex)->dDistance = dDistance;}
	const double GetMark2EndOfRunwayDistance(int nIndex){ return m_ObjSurface2.EndOfRunway.getItem(nIndex)->dDistance;}

	void SetMark2ThresholdOffset(double dOffset){m_ObjSurface2.ThresHold.dOffset = dOffset;}
	const double GetMark2ThresholdOffset(){return m_ObjSurface2.ThresHold.dOffset;}

	void SetMark2LateralOffset(double dOffset) { m_ObjSurface2.Lateral.dOffset = dOffset;}
	const double GetMark2LateralOffset() { return m_ObjSurface2.Lateral.dOffset;}

	void SetMark2EndOfRunwayOffset(double dOffset) {m_ObjSurface2.EndOfRunway.dOffset = dOffset;}
	const double GetMark2EndOfRunwayOffset() {return m_ObjSurface2.EndOfRunway.dOffset;}

	ObjRuwayStruct getMark1RunwayStruct(){return m_ObjSurface1;}
	ObjRuwayStruct getMark2RunwayStruct(){return m_ObjSurface2;}

	void SetMark1Show(BOOL bShow) { m_bSurface1Show =bShow;}
	BOOL GetMark1Show() {return m_bSurface1Show;}

	void SetMark2Show(BOOL bShow){ m_bSurface2Show = bShow;}
	BOOL GetMark2Show(){return m_bSurface2Show;}


/////////////////end objsurface
	virtual bool CopyData(const ALTObject* pObj);

	ObjRuwayStruct& GetObstruction(RUNWAY_MARK mark);;
private:
	CPath2008 m_path;
	double m_dWidth;
	std::string m_sMarking1;
	std::string m_sMarking2;
	double m_dMark1LandingThreshold;
	double m_dMark1TakeOffThreshold;

	double m_dMark2LandingThreshold;
	double m_dMark2TakeOffThreshold;

	ObjRuwayStruct m_ObjSurface1;
	BOOL		   m_bSurface1Show;
	ObjRuwayStruct m_ObjSurface2;
	BOOL		   m_bSurface2Show;
	//database operation
public:
	
	void GetIntersectTaxiway(const ALTObjectList& vTaxiway, ALTObjectList& reslt )const;
	bool Intersect(const Taxiway& taixway)const;

	void ReadRecord(CADORecordset& adoRecordset);

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	void UpdatePath();
	virtual void DeleteObject(int nObjID);

	static void ExportRunways(int nAirportID,CAirsideExportFile& exportFile);

	void ExportRunway(CAirsideExportFile& exportFile);
	void ImportObject(CAirsideImportFile& importFile);
	
	void ReadSurfaceData();
	int SaveSurfaceData();
	void UpdataSurfaceData();
	void DeleteSurfaceData();
protected:
	int m_nPathID;

	CString GetSelectScript(int nObjID) const;
	CString GetSelectScriptForPath2008(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;

	//Landing and hold short position 
public:

	//void CalculateHoldPosition(Runway * pRunway, HoldPositionList& list);
	//void CalculateHoldPosition(Taxiway * pTaxiway, HoldPositionList& list);

	//evaluates hold position form object intersections and save to database
	//void UpdateHoldPositions(ALTObjectList& objList);

	//get intersectNode form database
	IntersectionNodeList GetIntersectNodes() const;

	//get Runway exit from database
	//int GetHoldPositionsIndex(const IntersectNode& intersect, bool bfirst)const;

	//bool GetHoldPositionLine(int idx, Line& line)const;

	//get hold positions to other taxiway
	//int GetIntersectionWithTaxiway(IntersectNodeList& output)const;
	//int GetHoldPositionsToObject(int otherID, HoldPositionList& output)const;

	//get exit of RunwayXX
	int GetExitList(RUNWAY_MARK rwMark, RunwayExitList& exitlist)const;
	
	//Get Intersect Runway id
	std::vector<int> GetIntersectRunways()const;	

	void UpdateRunwayExits( const IntersectionNodesInAirport& vNodes );

public:
	//mutable HoldPositionList m_vAllHoldPositions;
	//RunwayExitList m_vExits;	

	
protected:
	//void UpdateIntersectionList(IntersectNodeList& lstIntersecNode) const;
	//void UpdateHodePositionLists(IntersectNodeList& lstIntersecNode) const;
	//void UpdateHodePositionList(HoldPositionList& holdPositionList,HoldPositionList& dbHoldPositionList,IntersectNodeList& lstIntersecNode) const;


public:
	bool IsPointLeft(RUNWAY_MARK rwMark, const CPoint2008& pt)const;
	ARCVector3 GetDir(RUNWAY_MARK rwMark)const;
	double GetAngle(RUNWAY_MARK rwMark, const ARCVector3& vdir)const; // (0 ,180)

	EnumCardinalDirection GetPointCadDir(const ALTAirport&aptInfo, const CPoint2008& pt)const;
};

