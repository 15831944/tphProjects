#pragma once
#include "altobject.h"


class AirWayPoint;

class INPUTAIRSIDE_API AirHold :
	public ALTObject
{
public:

	
	static const GUID& getTypeGUID();


	AirHold(void);
	virtual ~AirHold(void);

	virtual ALTOBJECT_TYPE GetType()const { return ALT_HOLD; } ;
	virtual CString GetTypeName()const  { return "Hold"; };


	Path GetPath(const Point& fixPt, double airportVariation)const;
	
	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();
	virtual ALTObjectDisplayProp* NewDisplayProp();
	bool CopyData(const ALTObject* pObj);

private:
	
	int m_nWayPointID;
	double m_inBoundDegree;
	double m_outBoundLegLength;
	double m_outBoundLegTime;
	BOOL m_bRightTurn;
	double m_dMaxAlt;
	double m_dMinAlt;

public:
	void SetWayPoint(int nWaypointID){ m_nWayPointID = nWaypointID;}
	int GetWatpoint();
	void setInBoundDegree(double inboundDegree){ m_inBoundDegree = inboundDegree; }

	void setOutBoundLegNm(double outboundnm){ m_outBoundLegLength = outboundnm; }
	void setOutBoundLegMin(double outboundmin){ m_outBoundLegTime = outboundmin; }

	void setRightTurn(bool brightTurn){ m_bRightTurn = brightTurn; }
	void setMaxAltitude(double maxaltfeet){  m_dMaxAlt = maxaltfeet; }
	void setMinAltitude(double minaltfeet){  m_dMinAlt = minaltfeet; }


	double getInBoundDegree()const { return m_inBoundDegree; }
	double getOutBoundLegNm()const { return m_outBoundLegLength; }
	double getOutBoundLegMin()const { return m_outBoundLegTime; }

	BOOL isRightTurn()const { return m_bRightTurn; }
	double getMaxAltitude()const;
	double getMinAltitude()const;
	//database operation
public:

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

protected:
	CString GetSelectScript(int nObjID) const;
	CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;

public:
	static void ExportAirHolds(CAirsideExportFile& exportFile);
	
	void ExportAirHold(CAirsideExportFile& exportFile);

	void ImportObject(CAirsideImportFile& importFile);

};
