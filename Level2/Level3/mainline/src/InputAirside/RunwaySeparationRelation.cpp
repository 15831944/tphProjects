#include "stdafx.h"
#include "ApproachSeparationCirteria.h"
#include "RunwaySeparationRelation.h"
#include "runway.h"
#include <math.h>

CRunwaySeparationRelation::CRunwaySeparationRelation(const ALTObjectList& vRunways):
m_nID(-1),
m_nProjID(-1),
m_dConvergentAngle(45.0),
m_dDivergentAngle(45.0),
m_dCloseDist(1000.0),
m_dFarDist(2000.0)
{
	m_vRunways = vRunways;
}

CRunwaySeparationRelation::CRunwaySeparationRelation(int nProjID,const ALTObjectList& vRunways):
m_nID(-1),
m_nProjID(nProjID),
m_dConvergentAngle(45.0),
m_dDivergentAngle(45.0),
m_dCloseDist(1000.0),
m_dFarDist(2000.0)
{
	m_vRunways = vRunways;
}

CRunwaySeparationRelation::~CRunwaySeparationRelation(void)
{
}

void CRunwaySeparationRelation::SaveData(int nProjID)
{
	CString strSQL = _T("");
	if (m_nID < 0) {
		strSQL.Format(_T("INSERT INTO RUNWAYRELATION (PROJID,CONANGLE,DIVANGLE,NEARPT,FARPT) VALUES (%d,%f,%f,%f,%f);"),m_nProjID = nProjID,m_dConvergentAngle,m_dDivergentAngle,m_dCloseDist,m_dFarDist);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}else {
		strSQL.Format(_T("UPDATE RUNWAYRELATION SET PROJID = %d,CONANGLE = %f,DIVANGLE = %f,NEARPT = %f,FARPT = %f WHERE (ID = %d);"),m_nProjID = nProjID,m_dConvergentAngle,m_dDivergentAngle,m_dCloseDist,m_dFarDist,m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CRunwaySeparationRelation::DeleteData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM RUNWAYRELATION WHERE (ID = %d);"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CRunwaySeparationRelation::ReadData(void)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM RUNWAYRELATION WHERE (PROJID = %d);"),m_nProjID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	if (!adoRecordset.IsEOF()) {
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("CONANGLE"),m_dConvergentAngle);
		adoRecordset.GetFieldValue(_T("DIVANGLE"),m_dDivergentAngle);
		adoRecordset.GetFieldValue(_T("NEARPT"),m_dCloseDist);
		adoRecordset.GetFieldValue(_T("FARPT"),m_dFarDist);
	}
}

void CRunwaySeparationRelation::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeDouble(m_dConvergentAngle);
	exportFile.getFile().writeDouble(m_dDivergentAngle);
	exportFile.getFile().writeDouble(m_dCloseDist);
	exportFile.getFile().writeDouble(m_dFarDist);

	exportFile.getFile().writeLine();
}

void CRunwaySeparationRelation::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	importFile.getFile().getFloat(m_dConvergentAngle);
	importFile.getFile().getFloat(m_dDivergentAngle);
	importFile.getFile().getFloat(m_dCloseDist);
	importFile.getFile().getFloat(m_dFarDist);
	importFile.getFile().getLine();
}
/******************************************************************************************************************************
** function: bool CRunwaySeparationRelation::CalculateRunwaysRelation(APPROACHTYPE emAppType,int nRunway1ID,RUNWAY_MARK emRunway1Mark,int nRunway2ID,RUNWAY_MARK emRunway2Mark,ApproachRelationType &emRelation,ApproachIntersectionType &emIntersection,double &dDisFromRunway1ThresholdToInterNode,double &dDisFromRunway2ThresholdToInterNode);
** input:nRunway1ID must be landing runway
** description:calculate success return ture.else false,
** two runways have the follow position at surface:
** (1) same;(2)parallel;(3)intersect at lines outstretched;(4)intersect at one of the two lines;(5)intersect at two lines;
** at state(4),this is no parallel.and , landing flight always show as a line ,not radial.so the angle of two runways is in range :0~90
******************************************************************************************************************************/
bool CRunwaySeparationRelation::CalculateRunwaysRelation(AIRCRAFTOPERATIONTYPE emAppType,int nRunway1ID,RUNWAY_MARK emRunway1Mark,int nRunway2ID,RUNWAY_MARK emRunway2Mark,RunwayRelationType &emRelation,RunwayIntersectionType &emIntersection,double &dDisFromRunway1ThresholdToInterNode,double &dDisFromRunway2ThresholdToInterNode)
{
	if (emAppType == AIRCRAFTOPERATION_TYPE_NONE) return (false);

	if (nRunway1ID == nRunway2ID) { // if same runway,state(1)
		emRelation = RRT_SAMERUNWAY;
		emIntersection = RIT_NONE;
		dDisFromRunway1ThresholdToInterNode = -1.0;
		dDisFromRunway2ThresholdToInterNode = -1.0;
		return (true);
	}

	double dTemp = -1.0,dTemp1 = -1.0;
	Runway runway1,runway2;
	runway1.ReadObject(nRunway1ID);
	runway2.ReadObject(nRunway2ID);
	if (runway1.GetPath().getCount() < 1)
		return (false);
	if (runway2.GetPath().getCount() < 1) 
		return (false);

	//two point at runway1
	double dRw1Pt1X = runway1.GetPath().getPoint(0).getX();//jumping-off point of runway1 at point1
	double dRw1Pt1Y = runway1.GetPath().getPoint(0).getY();//end-point of runway1 at point1
	double dRw1Pt2X = runway1.GetPath().getPoint(runway1.GetPath().getCount() - 1).getX();//jumping-off point of runway1 at point2
	double dRw1Pt2Y = runway1.GetPath().getPoint(runway1.GetPath().getCount() - 1).getY();//end-point of runway1 at point2

	//two point at runway2
	double dRw2Pt1X = runway2.GetPath().getPoint(0).getX();//jumping-off point of runway2 at point1
	double dRw2Pt1Y = runway2.GetPath().getPoint(0).getY();//end-point of runway2 at point1
	double dRw2Pt2X = runway2.GetPath().getPoint(runway2.GetPath().getCount() - 1).getX();//jumping-off point of runway2 at point2
	double dRw2Pt2Y = runway2.GetPath().getPoint(runway2.GetPath().getCount() - 1).getY();//end-point of runway2 at point2

	bool bRw1Vertical  = false, bRw2Vertical = false;//ture is x-axis' vertical
	double dTan1 = -1.0;//line function of tan(),for runway1
	double dTan2 = -1.0;//line function of tan(),for runway2
	double dAngle = -1.0;//two lines'angle 

	//calculate lines' angle
	if ((dRw1Pt2X - dRw1Pt1X) < ARCMath::EPSILON)
		bRw1Vertical = true;
	else
		dTan1 = (dRw1Pt2Y - dRw1Pt1Y)/(dRw1Pt2X - dRw1Pt1X);

	if ((dRw2Pt2X - dRw2Pt1X) < ARCMath::EPSILON) 
		bRw2Vertical = true;
	else
		dTan2 = (dRw2Pt2Y - dRw2Pt1Y)/(dRw2Pt2X - dRw2Pt1X);

	if ( bRw1Vertical && bRw2Vertical) {//parallel ,state(2)
		dTemp = dRw2Pt1X - dRw1Pt1X;
		if (dTemp < m_dCloseDist) 
			emRelation = RRT_CLOSE;
		else if(dTemp > m_dFarDist)
			emRelation = RRT_FAR;
		else 
			emRelation = RRT_INTERMEDIATE;
		emIntersection = RIT_NONE;
		dDisFromRunway1ThresholdToInterNode = -1.0;
		dDisFromRunway2ThresholdToInterNode = -1.0;
		return (true);
	}else if (bRw1Vertical && !bRw2Vertical) 
		dAngle = fabs(90.0 - atan(dTan2)*180.0/3.14159);
	else if (!bRw1Vertical && bRw2Vertical)
		dAngle = fabs(atan(dTan1)*180.0/3.14159 - 90.0);
	else
		dAngle = fabs((atan(dTan1) - atan(dTan2))*180.0/3.14159);
	if (dAngle > 90.0)
		dAngle = 180.0 - dAngle;

	if (dAngle <= 1.0) {//angle <= 1 degree, parallel ,state(2)
		dTemp = dRw2Pt1Y + (dRw1Pt2Y - dRw1Pt1Y)*dRw1Pt1X/(dRw1Pt2X-dRw1Pt1X)-dRw2Pt1Y-(dRw2Pt2Y - dRw2Pt1Y)*dRw2Pt1X/(dRw2Pt2X-dRw2Pt1X);
		if (dTemp < m_dCloseDist) 
			emRelation = RRT_CLOSE;
		else if(dTemp > m_dFarDist)
			emRelation = RRT_FAR;
		else 
			emRelation = RRT_INTERMEDIATE;
		emIntersection = RIT_NONE;
		dDisFromRunway1ThresholdToInterNode = -1.0;
		dDisFromRunway2ThresholdToInterNode = -1.0;
		return (true);
	}

	//intersect point
	double dInterX = ( (dRw2Pt1Y - dRw1Pt1Y)*(dRw2Pt2X - dRw2Pt1X)*(dRw1Pt2X - dRw1Pt1X) - (dRw1Pt2X - dRw1Pt1X)*(dRw2Pt2Y - dRw2Pt1Y)*dRw2Pt1X + (dRw2Pt2X - dRw2Pt1X)*(dRw1Pt2Y - dRw1Pt1Y)*dRw1Pt1X )/((dRw2Pt2X - dRw2Pt1X)*(dRw1Pt2Y - dRw1Pt1Y) - (dRw1Pt2X - dRw1Pt1X)*(dRw2Pt2Y - dRw2Pt1Y));
	double dInterY = (dRw1Pt2Y - dRw1Pt1Y)*(dInterX - dRw1Pt1X)/(dRw1Pt2X - dRw1Pt1X) + dRw1Pt1Y;
	//////////////////////////////////////////////////////////////////////////
	// discarded code
	//double dInterX = ( (dRw1Pt2Y - dRw1Pt1Y)*(dRw2Pt2X - dRw2Pt1X)*(dRw1Pt2X - dRw1Pt1X) + (dRw1Pt2X - dRw1Pt1X)*(dRw2Pt2Y - dRw2Pt1Y)*dRw2Pt1X - (dRw2Pt2X - dRw2Pt1X)*(dRw1Pt2Y - dRw1Pt1Y)*dRw1Pt1X )/((dRw1Pt2X - dRw1Pt1X)*(dRw2Pt2Y - dRw2Pt1Y) - (dRw2Pt2X - dRw2Pt1X)*(dRw1Pt2Y - dRw1Pt1Y));
	//double dInterY = dTan2*(dInterX - dRw1Pt1X) + dRw1Pt1Y;
	//////////////////////////////////////////////////////////////////////////

	//distance from end point to intersect point
	double dRw1Pt1DisFromInterNode = sqrt( (dInterY - dRw1Pt1Y)*(dInterY - dRw1Pt1Y) + (dInterX - dRw1Pt1X)*(dInterX - dRw1Pt1X) );
	double dRw1Pt2DisFromInterNode = sqrt( (dRw1Pt2Y - dInterY)*(dRw1Pt2Y - dInterY) + (dRw1Pt2X - dInterX)*(dRw1Pt2X - dInterX) );
	double dRw2Pt1DisFromInterNode = sqrt( (dInterY - dRw2Pt1Y)*(dInterY - dRw2Pt1Y) + (dInterX - dRw2Pt1X)*(dInterX - dRw2Pt1X) );
	double dRw2Pt2DisFromInterNode = sqrt( (dRw2Pt2Y - dInterY)*(dRw2Pt2Y - dInterY) + (dRw2Pt2X - dInterX)*(dRw2Pt2X - dInterX) );

	//intersect at runway,state(5)
	std::vector<int> vrInterRunway = runway1.GetIntersectRunways();
	if( std::find(vrInterRunway.begin(),vrInterRunway.end(), runway2.getID()) != vrInterRunway.end() )
	{
		{
			emRelation = RRT_INTERSECT;
			emIntersection = RIT_RUNWAY;
			if (emRunway1Mark == RUNWAYMARK_FIRST) 
				dDisFromRunway1ThresholdToInterNode = dRw1Pt1DisFromInterNode;
			else
				dDisFromRunway1ThresholdToInterNode = dRw1Pt2DisFromInterNode;

			if (emRunway2Mark == RUNWAYMARK_FIRST)
				dDisFromRunway2ThresholdToInterNode = dRw2Pt1DisFromInterNode;
			else
				dDisFromRunway2ThresholdToInterNode = dRw2Pt2DisFromInterNode;
			return (true);
		}
	}

	double dMinDisRws = -1.0;
	bool bRw2Front = false;//intersect node is on front of vector , or not
	//find two runway's's minimum distance
	dTemp = sqrt( (dRw1Pt2Y - dRw1Pt1Y)*(dRw1Pt2Y - dRw1Pt1Y) + (dRw1Pt2X - dRw1Pt1X)*(dRw1Pt2X - dRw1Pt1X) );
	dTemp1 = sqrt( (dRw2Pt2Y - dRw2Pt1Y)*(dRw2Pt2Y - dRw2Pt1Y) + (dRw2Pt2X - dRw2Pt1X)*(dRw2Pt2X - dRw2Pt1X) );

	if (dRw1Pt1DisFromInterNode + dRw1Pt2DisFromInterNode <= dTemp) {//state(4)
		dMinDisRws = MIN(dRw2Pt1DisFromInterNode,dRw2Pt2DisFromInterNode);//runways' distance
		//calculate the direction
		if (emAppType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF){
			if (emRunway2Mark == RUNWAYMARK_FIRST) {//pt1 -> pt2
				if (dRw2Pt1DisFromInterNode < dRw2Pt2DisFromInterNode)
					bRw2Front = true;
				else
					bRw2Front = false;
			}else{//pt2 -> pt1
				if (dRw2Pt1DisFromInterNode < dRw2Pt2DisFromInterNode)
					bRw2Front = false;
				else
					bRw2Front = true;
			}

			if (bRw2Front) {
				if (dAngle <= m_dConvergentAngle) {//parallel
					if (dMinDisRws < m_dCloseDist) {
						emRelation = RRT_CLOSE;
					}else if (dMinDisRws > m_dFarDist) {
						emRelation = RRT_FAR;
					}else{
						emRelation = RRT_INTERMEDIATE;
					}
					emIntersection = RIT_NONE;
					dDisFromRunway1ThresholdToInterNode = -1.0;
					dDisFromRunway2ThresholdToInterNode = -1.0;
				}else{//as convergent
					emRelation = RRT_CONVERGENT;
					emIntersection = RIT_RUNWAY;
					dDisFromRunway1ThresholdToInterNode = emRunway1Mark == RUNWAYMARK_FIRST?dRw1Pt1DisFromInterNode:dRw1Pt2DisFromInterNode;
					dDisFromRunway2ThresholdToInterNode = dMinDisRws;
				}
				return (true);
			}else{
				if (dAngle <= m_dDivergentAngle) {//parallel
					if (dMinDisRws < m_dCloseDist) {
						emRelation = RRT_CLOSE;
					}else if (dMinDisRws > m_dFarDist) {
						emRelation = RRT_FAR;
					}else{
						emRelation = RRT_INTERMEDIATE;
					}
					emIntersection = RIT_NONE;
					dDisFromRunway1ThresholdToInterNode = -1.0;
					dDisFromRunway2ThresholdToInterNode = -1.0;
				}else{//as divergent
					emRelation = RRT_DIVERGENT;
					emIntersection = RIT_RUNWAY;
					dDisFromRunway1ThresholdToInterNode = emRunway1Mark == RUNWAYMARK_FIRST?dRw1Pt1DisFromInterNode:dRw1Pt2DisFromInterNode;
					dDisFromRunway2ThresholdToInterNode = dMinDisRws;
				}
				return (true);
			}
		}else if (emAppType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED){//landing behind landed always deal as convergent at this state.
			if (dAngle <= m_dConvergentAngle) {//parallel
				if (dMinDisRws < m_dCloseDist) {
					emRelation = RRT_CLOSE;
				}else if (dMinDisRws > m_dFarDist) {
					emRelation = RRT_FAR;
				}else{
					emRelation = RRT_INTERMEDIATE;
				}
				emIntersection = RIT_NONE;
				dDisFromRunway1ThresholdToInterNode = -1.0;
				dDisFromRunway2ThresholdToInterNode = -1.0;
			}else{//as convergent
				emRelation = RRT_CONVERGENT;
				emIntersection = RIT_RUNWAY;
				dDisFromRunway1ThresholdToInterNode = emRunway1Mark == RUNWAYMARK_FIRST?dRw1Pt1DisFromInterNode:dRw1Pt2DisFromInterNode;
				dDisFromRunway2ThresholdToInterNode = dMinDisRws;
			}
			return (true);
		}else
			return (false);
	}

	if (dRw2Pt1DisFromInterNode + dRw2Pt2DisFromInterNode <= dTemp1) {//state(4),landing fly cross the second runway	
		dMinDisRws = MIN(dRw1Pt1DisFromInterNode,dRw1Pt2DisFromInterNode);//runways' distance
		if (dAngle <= m_dDivergentAngle) {//parallel
			if (dMinDisRws < m_dCloseDist) {
				emRelation = RRT_CLOSE;
			}else if (dMinDisRws > m_dFarDist) {
				emRelation = RRT_FAR;
			}else{
				emRelation = RRT_INTERMEDIATE;
			}
			emIntersection = RIT_NONE;
			dDisFromRunway1ThresholdToInterNode = -1.0;
			dDisFromRunway2ThresholdToInterNode = -1.0;
		}else{//as divergent
			emRelation = RRT_DIVERGENT;
			emIntersection = RIT_RUNWAY;
			dDisFromRunway1ThresholdToInterNode = dMinDisRws;
			dDisFromRunway2ThresholdToInterNode = emRunway2Mark == RUNWAYMARK_FIRST?dRw2Pt1DisFromInterNode:dRw2Pt2DisFromInterNode;
		}
		return (true);
	}


	//otherwise,state(3)

	//two lines' minimum distance
	double dR1P1R2P1 =  sqrt( (dRw2Pt1Y - dRw1Pt1Y)*(dRw2Pt1Y - dRw1Pt1Y) + (dRw2Pt1X - dRw1Pt1X)*(dRw2Pt1X - dRw1Pt1X) );
	double dR1P1R2P2 =  sqrt( (dRw2Pt2Y - dRw1Pt1Y)*(dRw2Pt2Y - dRw1Pt1Y) + (dRw2Pt2X - dRw1Pt1X)*(dRw2Pt2X - dRw1Pt1X) );
	double dR1P2R2P1 =  sqrt( (dRw2Pt1Y - dRw1Pt2Y)*(dRw2Pt1Y - dRw1Pt2Y) + (dRw2Pt1X - dRw1Pt2X)*(dRw2Pt1X - dRw1Pt2X) );
	double dR1P2R2P2 =  sqrt( (dRw2Pt2Y - dRw1Pt2Y)*(dRw2Pt2Y - dRw1Pt2Y) + (dRw2Pt2X - dRw1Pt2X)*(dRw2Pt2X - dRw1Pt2X) );

	if ( (dMinDisRws = dR1P1R2P1) >  dR1P1R2P2) {
		dMinDisRws = dR1P1R2P2;
	}
	if (dMinDisRws > dR1P2R2P1) {
		dMinDisRws = dR1P2R2P1;
	}
	if (dMinDisRws > dR1P2R2P2) {
		dMinDisRws = dR1P2R2P2;
	}

	//calculate the direction	
	if (emAppType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF){
		if (emRunway2Mark == RUNWAYMARK_FIRST) {//pt1 -> pt2
			if (dRw2Pt1DisFromInterNode < dRw2Pt2DisFromInterNode)
				bRw2Front = true;
			else
				bRw2Front = false;
		}else{//pt2 -> pt1
			if (dRw2Pt1DisFromInterNode < dRw2Pt2DisFromInterNode)
				bRw2Front = false;
			else
				bRw2Front = true;
		}	
		if (bRw2Front) {//all at front
			if (dAngle <= m_dConvergentAngle) {//parallel
				if (dMinDisRws < m_dCloseDist) {
					emRelation = RRT_CLOSE;
				}else if (dMinDisRws > m_dFarDist) {
					emRelation = RRT_FAR;
				}else{
					emRelation = RRT_INTERMEDIATE;
				}
				emIntersection = RIT_NONE;
				dDisFromRunway1ThresholdToInterNode = -1.0;
				dDisFromRunway2ThresholdToInterNode = -1.0;
			}else{//as convergent
				emRelation = RRT_CONVERGENT;
				emIntersection = RIT_SPACE;
				dDisFromRunway1ThresholdToInterNode = MIN(dRw1Pt1DisFromInterNode,dRw1Pt2DisFromInterNode);
				dDisFromRunway2ThresholdToInterNode = MIN(dRw2Pt1DisFromInterNode,dRw2Pt2DisFromInterNode);
			}
		}else{
			if (dAngle <= m_dDivergentAngle) {//parallel
				if (dMinDisRws < m_dCloseDist) {
					emRelation = RRT_CLOSE;
				}else if (dMinDisRws > m_dFarDist) {
					emRelation = RRT_FAR;
				}else{
					emRelation = RRT_INTERMEDIATE;
				}
				emIntersection = RIT_NONE;
				dDisFromRunway1ThresholdToInterNode = -1.0;
				dDisFromRunway2ThresholdToInterNode = -1.0;
			}else{//as divergent
				emRelation = RRT_DIVERGENT;
				emIntersection = RIT_SPACE;
				dDisFromRunway1ThresholdToInterNode = MIN(dRw1Pt1DisFromInterNode,dRw1Pt2DisFromInterNode);
				dDisFromRunway2ThresholdToInterNode = MIN(dRw2Pt1DisFromInterNode,dRw2Pt2DisFromInterNode);
			}
		}
	}else if (emAppType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED){//landing behind landed always deal as convergent at this state.
		if (dAngle <= m_dConvergentAngle) {//parallel
			if (dMinDisRws < m_dCloseDist) {
				emRelation = RRT_CLOSE;
			}else if (dMinDisRws > m_dFarDist) {
				emRelation = RRT_FAR;
			}else{
				emRelation = RRT_INTERMEDIATE;
			}
			emIntersection = RIT_NONE;
			dDisFromRunway1ThresholdToInterNode = -1.0;
			dDisFromRunway2ThresholdToInterNode = -1.0;
		}else{//as convergent
			emRelation = RRT_CONVERGENT;
			emIntersection = RIT_SPACE;
			dDisFromRunway1ThresholdToInterNode = MIN(dRw1Pt1DisFromInterNode,dRw1Pt2DisFromInterNode);
			dDisFromRunway2ThresholdToInterNode = MIN(dRw2Pt1DisFromInterNode,dRw2Pt2DisFromInterNode);
		}
	}else
		return (false);		

	return (true);
}

CString CRunwaySeparationRelation::GetApproachRelationTypeString(RunwayRelationType emType)
{
	CString strReturn = _T("");
	switch(emType) {
	case RRT_CONVERGENT:
		strReturn = _T("Convergent");
		break;
	case RRT_DIVERGENT:
		strReturn = _T("Divergent");
		break;
	case RRT_CLOSE:
		strReturn = _T("Close");
		break;
	case RRT_INTERMEDIATE:
		strReturn = _T("Intermediate");
		break;
	case RRT_FAR:
		strReturn = _T("Far");
		break;
	case RRT_INTERSECT:
		strReturn = _T("Intersection");
		break;
	case RRT_SAMERUNWAY:
		strReturn = _T("Same Runway");
		break;
	default:
		break;
	}
	return (strReturn);
}
CString CRunwaySeparationRelation::GetApproachIntersectionTypeString(RunwayIntersectionType emType)
{
	CString strReturn = _T("");
	switch(emType) {
	case RIT_NONE:
		strReturn = _T("None");
		break;
	case RIT_RUNWAY:
		strReturn = _T("Runway");
		break;
	case RIT_SPACE:
		strReturn = _T("Space");
		break;
	default:
		break;
	}
	return (strReturn);
}

RunwayRelationType CRunwaySeparationRelation::GetLogicRunwayRelationship(AIRCRAFTOPERATIONTYPE emAppType, int nRwy1ID,RUNWAY_MARK eRwy1Mark,int nRwy2ID,RUNWAY_MARK eRwy2Mark, RunwayIntersectionType& emIntersection, CPoint2008& InterP, double& dNodeDisToRwy1,double& dNodeDisToRwy2 )
{
	if (nRwy1ID == nRwy2ID) // same runway
	{ 
		emIntersection = RIT_NONE;
		dNodeDisToRwy1 = 0;
		dNodeDisToRwy2 = 0;
		return RRT_SAMERUNWAY;
	}

	RunwayRelationType emRelation = RRT_SAMERUNWAY;

	Runway* pRwy1 = NULL;
	Runway* pRwy2 = NULL;

	size_t nCount = m_vRunways.size();
	for (size_t i = 0; i < nCount; i++ )
	{
		ALTObject* pObj = m_vRunways.at(i).get();
		if (pObj->getID() == nRwy1ID)
			pRwy1 = (Runway*)pObj;

		if (pObj->getID() == nRwy2ID)
			pRwy2 = (Runway*)pObj;

		if (pRwy1 != NULL && pRwy2 != NULL)
			break;
	}

	//two point at runway1
	CPoint2008 rwy1Point1,rwy1Point2;
	double negdist1 = pRwy1->GetPath().GetTotalLength();
	if(eRwy1Mark == RUNWAYMARK_FIRST)
	{ 
		rwy1Point1 = pRwy1->GetPath().GetDistPoint(0);
		rwy1Point2 = pRwy1->GetPath().GetDistPoint(negdist1);
	}
	else
	{	
		rwy1Point2 = pRwy1->GetPath().GetDistPoint(0);
		rwy1Point1 = pRwy1->GetPath().GetDistPoint(negdist1);	
	}	

	//two point at runway2
	CPoint2008 rwy2Point1, rwy2Point2;
	double negdist2 = pRwy2->GetPath().GetTotalLength();
	if(eRwy2Mark == RUNWAYMARK_FIRST)
	{ 
		rwy2Point1 = pRwy2->GetPath().GetDistPoint(0);
		rwy2Point2 = pRwy2->GetPath().GetDistPoint(negdist2);
	}
	else
	{	
		rwy2Point2 = pRwy2->GetPath().GetDistPoint(0);
		rwy2Point1 = pRwy2->GetPath().GetDistPoint(negdist2);	
	}	

	//to get runway's line equation: Ax+By+C=0
	double dA1 =0,dB1 =0, dC1 =0, dAngle1 =0;
	double dA2 =0,dB2 =0, dC2 =0, dAngle2 =0;

	Get2DLineParameters(rwy1Point1, rwy1Point2, dA1, dB1, dC1, dAngle1);
	Get2DLineParameters(rwy2Point1, rwy2Point2, dA2, dB2, dC2, dAngle2);

	double dDistToRwy2 = GetNearestDistToRwy2(rwy1Point1, rwy1Point2, rwy2Point1, rwy2Point2);

	if (dA1*dB2 == dA2*dB1)		//parallel
	{
		emIntersection = RIT_NONE;
		dNodeDisToRwy1 = 0;
		dNodeDisToRwy2 = 0;

		if(dDistToRwy2 < m_dCloseDist)
			emRelation = RRT_CLOSE;
		else if (dDistToRwy2 > m_dFarDist)
			emRelation = RRT_FAR;
		else
			emRelation = RRT_INTERMEDIATE;
		return emRelation;
	}

	double dAngle = 0;
	if (dA1*dA2 + dB1*dB2 ==0)	//vertical
	{
		dAngle = 90;
	}
	else			//not strict parallel
	{
		dAngle = abs(dAngle1 -dAngle2); 
		if (dAngle > 90)
			dAngle = 180 -dAngle;
	}

	double dIntersect_X = (dC2*dB1 - dC1*dB2)/(dA1*dB2 - dA2*dB1);
	double dIntersect_Y = (dC2*dA1 - dC1*dA2)/(dA2*dB1 - dA1*dB2);
	InterP.setX(dIntersect_X);
	InterP.setY(dIntersect_Y);

	if (InterP.within(rwy1Point1,rwy1Point2) >0 && InterP.within(rwy2Point1,rwy2Point2) >0)		//Intersected on runway
	{
		emIntersection = RIT_RUNWAY;
		dNodeDisToRwy1 = rwy1Point1.distance(InterP);
		dNodeDisToRwy2 = rwy2Point1.distance(InterP);

		emRelation = RRT_INTERSECT;
		return emRelation;
	}

	bool bDivergent = IsDivergentDirectionOfRunway(emAppType,rwy1Point1,rwy1Point2, rwy2Point1, rwy2Point2, InterP, emIntersection);

	if (bDivergent)		//divergent
	{
		if (dAngle < m_dDivergentAngle || emIntersection == RIT_NONE)		//not influenced each other
		{
			//emIntersection = RIT_NONE;
			dNodeDisToRwy1 = 0;
			dNodeDisToRwy2 = 0;

			if(dDistToRwy2 < m_dCloseDist)
				emRelation = RRT_CLOSE;
			else if (dDistToRwy2 > m_dFarDist)
				emRelation = RRT_FAR;
			else
				emRelation = RRT_INTERMEDIATE;
		}
		else
		{
			dNodeDisToRwy1 = rwy1Point1.distance(InterP);
			dNodeDisToRwy2 = rwy2Point1.distance(InterP);

			emRelation = RRT_DIVERGENT;
		}
	}
	else		//convergent
	{
		if (dAngle < m_dConvergentAngle || emIntersection == RIT_NONE)		//not influenced each other
		{
			//emIntersection = RIT_NONE;
			dNodeDisToRwy1 = 0;
			dNodeDisToRwy2 = 0;

			if(dDistToRwy2 < m_dCloseDist)
				emRelation = RRT_CLOSE;
			else if (dDistToRwy2 > m_dFarDist)
				emRelation = RRT_FAR;
			else
				emRelation = RRT_INTERMEDIATE;
			
		}
		else
		{
			dNodeDisToRwy1 = rwy1Point1.distance(InterP);
			dNodeDisToRwy2 = rwy2Point1.distance(InterP);

			emRelation = RRT_CONVERGENT;
		}
	}

	return emRelation;
}

void CRunwaySeparationRelation::Get2DLineParameters( const CPoint2008& point1, const CPoint2008& point2, double& dPara_X, double& dPara_Y, double& dCons, double& dAngle )
{
	double dDx = point2.getX() - point1.getX();
	double dDy = point2.getY() - point1.getY();

	dPara_X = dDy;
	dPara_Y = -dDx;
	dCons = point1.getY()*dDx - point1.getX()*dDy;

	if (dDx >0 || dDx <0)
		dAngle = atan(dDy/dDx)*(180.0/3.14159);
	else
		dAngle = 90;
}

double CRunwaySeparationRelation::GetNearestDistToRwy2(const CPoint2008& Rwy1P1, const CPoint2008& Rwy1P2,const CPoint2008& Rwy2P1, const CPoint2008& Rwy2P2)
{
	double dDist1 = Rwy1P1.distance(Rwy2P1);
	double dDist2 = Rwy1P1.distance(Rwy2P2);
	double dDistToRwy2 = min(dDist1, dDist2);

	dDist1 = Rwy1P2.distance(Rwy2P1);
	dDist2 = Rwy1P2.distance(Rwy2P2);

	if (dDist1 <= dDist2)
		dDistToRwy2 = min(dDistToRwy2, dDist1);
	else
		dDistToRwy2 = min(dDistToRwy2, dDist2);

	return dDistToRwy2;
}

bool CRunwaySeparationRelation::IsDivergentDirectionOfRunway(AIRCRAFTOPERATIONTYPE emAppType, const CPoint2008& Rwy1P1, const CPoint2008& Rwy1P2,const CPoint2008& Rwy2P1, const CPoint2008& Rwy2P2,const CPoint2008& InterP, RunwayIntersectionType& IntersectType )
{
	ASSERT(Rwy1P1 != Rwy1P2);	//error
	ASSERT(Rwy2P1 != Rwy2P2);	//error

	bool bEffectLandRwy1 = false;
	bool bEffectLandRwy2 = false;
	bool bEffectTakeoffRwy1 = false;
	bool bEffectTakeoffRwy2 = false;

	bool bIntersectOnRwy1 = InterP.within(Rwy1P1,Rwy1P2) >0 ?true:false;
	bool bIntersectOnRwy2 = InterP.within(Rwy2P1,Rwy2P2) >0 ?true:false;

	//judge intersection influence on rwy1
	if (Rwy1P1.getX() - Rwy1P2.getX() <0)
	{
		if (InterP.getX() <= Rwy1P2.getX())
			bEffectLandRwy1 = true;

		if (InterP.getX() >= Rwy1P1.getX())
			bEffectTakeoffRwy1 = true;
	}

	if (Rwy1P1.getX() - Rwy1P2.getX() >0)
	{
		if (InterP.getX() >= Rwy1P2.getX())
			bEffectLandRwy1 = true;

		if (InterP.getX() <= Rwy1P1.getX())
			bEffectTakeoffRwy1 = true;
	}

	if (Rwy1P1.getX() - Rwy1P2.getX() ==0)
	{
		if (Rwy1P1.getY() < Rwy1P2.getY())
		{
			if (InterP.getY() <= Rwy1P2.getY())
				bEffectLandRwy1 = true;

			if (InterP.getY() >= Rwy1P1.getY())
				bEffectTakeoffRwy1 = true;
		}
		else
		{
			if (InterP.getY() >= Rwy1P2.getY())
				bEffectLandRwy1 = true;

			if (InterP.getY() <= Rwy1P1.getY())
				bEffectTakeoffRwy1 = true;
		}
	}

	//judge intersection influence on rwy2
	if (Rwy2P1.getX() - Rwy2P2.getX() <0)
	{
		if (InterP.getX() <= Rwy2P2.getX())
			bEffectLandRwy2 = true;

		if (InterP.getX() >= Rwy2P1.getX())
			bEffectTakeoffRwy2 = true;
	}

	if (Rwy2P1.getX() - Rwy2P2.getX() >0)
	{
		if (InterP.getX() >= Rwy2P2.getX())
			bEffectLandRwy2 = true;

		if (InterP.getX() <= Rwy2P1.getX())
			bEffectTakeoffRwy2 = true;
	}

	if (Rwy2P1.getX() - Rwy2P2.getX() ==0)
	{
		if (Rwy2P1.getY() < Rwy2P2.getY())
		{
			if (InterP.getY() <= Rwy2P2.getY())
				bEffectLandRwy2 = true;

			if (InterP.getY() >= Rwy2P1.getY())
				bEffectTakeoffRwy2 = true;
		}
		else
		{
			if (InterP.getY() >= Rwy2P2.getY())
				bEffectLandRwy2 = true;

			if (InterP.getY() <= Rwy2P1.getY())
				bEffectTakeoffRwy2 = true;
		}
	}

	bool bDivergent = false;
	switch(emAppType)
	{
	case AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED:
		{
			if (bEffectLandRwy1 && bEffectLandRwy2)
			{
				if (bIntersectOnRwy1 || bIntersectOnRwy2)
				{
					IntersectType = RIT_RUNWAY;
					bDivergent = false;
				}
				else
				{
					IntersectType = RIT_SPACE;
					bDivergent = true;
				}
			}
			else
			{
				bDivergent = true;
				IntersectType = RIT_NONE;
			}
		}
		break;
	case AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF:
		{
			if (bEffectLandRwy1 && bEffectTakeoffRwy2)
			{
				bDivergent = false;
				if (bIntersectOnRwy1 || bIntersectOnRwy2)
					IntersectType = RIT_RUNWAY;
				else
					IntersectType = RIT_SPACE;
			}
			else
			{
				bDivergent = true;
				IntersectType = RIT_NONE;
			}		
		}
		break;
	case AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED:
		{
			if (bEffectTakeoffRwy1 && bEffectLandRwy2)
			{
				bDivergent = false;
				if (bIntersectOnRwy1 || bIntersectOnRwy2)
					IntersectType = RIT_RUNWAY;
				else
					IntersectType = RIT_SPACE;
			}
			else
			{
				bDivergent = true;
				IntersectType = RIT_NONE;
			}		
		}
	    break;
	case AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF:
		{
			if (bEffectTakeoffRwy1 && bEffectTakeoffRwy2)
			{
				bDivergent = false;
				if (bIntersectOnRwy1 || bIntersectOnRwy2)
					IntersectType = RIT_RUNWAY;
				else
					IntersectType = RIT_SPACE;
			}
			else
			{
				bDivergent = true;
				IntersectType = RIT_NONE;
			}		
		}
	    break;
	default:
		IntersectType = RIT_NONE;
	    break;
	}

	return bDivergent;
}
