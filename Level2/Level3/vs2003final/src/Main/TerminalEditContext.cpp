#include "StdAfx.h"
#include ".\terminaleditcontext.h"
#include "TermPlanDoc.h"
#include <Inputs/SideMoveWalkDataSet.h>
#include ".\floor2.h"
#include "inputs/PipeDataSet.h"
#include "Inputs/TerminalProcessorTypeSet.h"


CTerminalEditContext::CTerminalEditContext(void)
{
}

CTerminalEditContext::~CTerminalEditContext(void)
{
}

InputTerminal* CTerminalEditContext::getInput()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	return &pDoc->getARCport()->getTerminal();
}

static const double dBias = 0.999999;
//return true if on the floor extract
static int GetLowFloorIdx(double d)
{
	if(d>=0)
		return  static_cast<int>(d);	

	return static_cast<int>(d-dBias);
}
static int GetUpperFloorIdx(double d)
{
	if(d>=0)
		return  static_cast<int>(d+dBias);	
	return static_cast<int>(d);
}
static bool InRage(int i, int iF,int iT)
{
	int iMin = min(iF,iT);
	int iMax = max(iF,iT);
	return i<iMax && i>iMin;
}
ARCPath3 CTerminalEditContext::GetTerminalVisualPath( const Path& flrpath,bool bInterMidFlr /*= false*/ )
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	CFloors& floorlist = pDoc->GetFloorByMode(EnvMode_Terminal);
	ARCPath3 ret;
	if(flrpath.getCount()<1)
		return ret;

	ret.reserve(flrpath.getCount());

	Point prePt = flrpath[0];
	ret.push_back( ARCVector3( prePt.getX(),prePt.getY(),floorlist.getVisibleAltitude(prePt.getZ()) ) );
	for(int i=1;i<flrpath.getCount();i++)
	{
		Point thisPt = flrpath[i];

		if(bInterMidFlr)
		{
			int nFlrRangeFrom, nFlrRangeTo;			
			int nStep;

			if(prePt.getZ() > thisPt.getZ())
			{
				nStep = -1;
				nFlrRangeFrom = GetUpperFloorIdx(prePt.getZ()/SCALE_FACTOR);//
				nFlrRangeTo = GetLowFloorIdx(thisPt.getZ()/SCALE_FACTOR);
			}
			else
			{
				nStep = 1;
				nFlrRangeFrom = GetLowFloorIdx(prePt.getZ()/SCALE_FACTOR);//
				nFlrRangeTo = GetUpperFloorIdx(thisPt.getZ()/SCALE_FACTOR);
			}

			double prePtRealZ =floorlist.getRealAltitude(prePt.getZ());
			double thisPtRealZ = floorlist.getRealAltitude(thisPt.getZ());
			for(int idxFlr = nFlrRangeFrom+nStep; InRage(idxFlr,nFlrRangeFrom,nFlrRangeTo); idxFlr+=nStep)
			{
				double floorZ = floorlist.getLeavlRealAttitude(idxFlr);
				double dFactor = (floorZ-prePtRealZ)/(thisPtRealZ-prePtRealZ);
				double dMidX = prePt.getX()*(1.0-dFactor)+thisPt.getX()*dFactor;
				double dMidY = prePt.getY()*(1.0-dFactor)+thisPt.getY()*dFactor;
				double dZ = floorlist.getLevelAltitude(idxFlr);
				ret.push_back(ARCVector3(dMidX,dMidY,dZ));
			}				
		}
		ret.push_back( ARCVector3(flrpath[i].getX(),flrpath[i].getY(),floorlist.getVisibleAltitude(flrpath[i].getZ())) );
		prePt = thisPt;
	}
	return ret;
}

void CTerminalEditContext::GetMovingSideWalkWidthSpeed( const ProcessorID& procid, double& doutspeed, double& doutwidth )
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;	
	CSideMoveWalkProcData * pData=pDoc->GetTerminal().m_pMovingSideWalk->GetLinkedSideWalkProcData(procid);
	if(pData)
	{
		doutspeed =  pData->GetMoveSpeed();
		doutwidth = pData->GetWidth();
	}
	else
	{
		doutspeed = 100.0;	
		doutwidth = 100.0;
	}
}

void CTerminalEditContext::GetFloorAltOn(std::vector<double>& dAlt, std::vector<BOOL>&bOn)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	dAlt.clear();bOn.clear();
	CFloors& floorlist = pDoc->GetFloorByMode(EnvMode_Terminal);
	for(int i=0;i<floorlist.GetCount();i++)
	{
		dAlt.push_back(floorlist.GetFloor2(i)->Altitude());
		bOn.push_back(floorlist.GetFloor2(i)->IsVisible());
	}
}



double CTerminalEditContext::GetActiveFloorHeight() const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	CFloors& floorlist = pDoc->GetFloorByMode(EnvMode_Terminal);
	if( CRenderFloor* pfloor = floorlist.GetActiveFloor() )
	{
		return pfloor->Altitude();
	}
	return 0;
}

CRenderFloorList CTerminalEditContext::GetFloorList() const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	CFloors& floorlist = pDoc->GetFloorByMode(EnvMode_Terminal);
	CRenderFloorList list;
	list.insert(list.end(),floorlist.m_vFloors.begin(),floorlist.m_vFloors.end());
	return list;
}

std::vector<CPipeBase*> CTerminalEditContext::GetPipeList() const
{	
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	Terminal& term = pDoc->GetTerminal();

	CPipeDataSet* pPipeDataSet = term.m_pPipeDataSet;
	int nIncrease = pPipeDataSet->GetPipeCount();
	std::vector<CPipeBase*> vData;
	for(int i=0;i<nIncrease;i++)
		vData.push_back(pPipeDataSet->GetPipeAt(i));
	return vData;
	
}

bool CTerminalEditContext::GetAllRailWayInfo( std::vector<RailwayInfo*>& railWayVect )
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	if (pDoc)
	{
		pDoc->m_trainTraffic.GetAllRailWayInfo(railWayVect);	
		return true;
	}
	return false;
}

bool CTerminalEditContext::isTerminalProcessorDisplay( int ProcessorType )
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	if (pDoc->GetTerminal().m_pTerminalProcessorTypeSet->IsProcessorTypeVisiable(ProcessorType))
	{
		return true;
	}
	return false;
}

CString CTerminalEditContext::GetTerminalStructureBaseFilename( CString texturename )
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)mpParentDoc;
	return pDoc->GetSurfaceMaterialLib().GetTextureMatName(texturename);
}
