#pragma once 
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "ALTObject.h"
#include "ALT_BIN.h"

//this class has only a record at each project
class INPUTAIRSIDE_API CRunwaySeparationRelation
{
public:
	CRunwaySeparationRelation(const ALTObjectList& vRunways);
	CRunwaySeparationRelation(int nProjID, const ALTObjectList& vRunways);
	virtual ~CRunwaySeparationRelation(void);

	virtual void SaveData(int nProjID);
	virtual void DeleteData(void);
	virtual void ReadData(void);
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

public:
	//calculate relation of two runway,nRunway1ID must be landing runway,emRunway1Mark is landing mark; see RunwayRelation.doc
	bool CalculateRunwaysRelation(AIRCRAFTOPERATIONTYPE emOperationType,int nRunway1ID,RUNWAY_MARK emRunway1Mark,int nRunway2ID,RUNWAY_MARK emRunway2Mark,RunwayRelationType &emRelation,RunwayIntersectionType &emIntersection,double &dDisFromRunway1ThresholdToInterNode,double &dDisFromRunway2ThresholdToInterNode);
	RunwayRelationType GetLogicRunwayRelationship(AIRCRAFTOPERATIONTYPE emAppType,int nRwy1ID,RUNWAY_MARK eRwy1Mark,int nRwy2ID,RUNWAY_MARK eRwy2Mark,RunwayIntersectionType& emIntersection, CPoint2008& InterP, double& dNodeDisToRwy1,double& dNodeDisToRwy2);

	static CString GetApproachRelationTypeString(RunwayRelationType emType);
	static CString GetApproachIntersectionTypeString(RunwayIntersectionType emType);

protected:
	//2D line equation: Ax +By +C =0, to catch each parameter and k(slope)
	void Get2DLineParameters(const CPoint2008& point1, const CPoint2008& point2, double& dPara_X, double& dPara_Y, double& dCons, double& dAngle);

	// to get nearest distance from rwy1 to rwy2
	double GetNearestDistToRwy2(const CPoint2008& Rwy1P1, const CPoint2008& Rwy1P2,const CPoint2008& Rwy2P1, const CPoint2008& Rwy2P2);

	//to judge the two intersect runways direction is divergent or not
	bool IsDivergentDirectionOfRunway(AIRCRAFTOPERATIONTYPE emAppType,const CPoint2008& Rwy1P1, const CPoint2008& Rwy1P2,const CPoint2008& Rwy2P1, const CPoint2008& Rwy2P2,const CPoint2008& InterP, RunwayIntersectionType& IntersectType);

protected:
	int m_nID;
	int m_nProjID;
	ALTObjectList m_vRunways;
public:
	double m_dConvergentAngle;//0~90,<= mean parallel,and > mean convergent
	double m_dDivergentAngle;//0~90,<= mean parallel,and > mean divergent
	double m_dCloseDist;//< mean two runways are close
	double m_dFarDist;//> mean two runways are far
};