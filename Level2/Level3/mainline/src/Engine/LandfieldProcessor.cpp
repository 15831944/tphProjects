#include "StdAfx.h"
#include ".\landfieldprocessor.h"
#include "../Common/BizierCurve.h"
#include "../Engine/LandfieldProcessor.h"
#include "../Engine/StretchProc.h"
#include "../Engine/IntersectionProc.h"
#include "../Engine/RoundAboutProc.h"
#include "../Engine/TurnOffProc.h"
#include "../Engine/OverPassProc.h"
#include "../Engine/UnderPassProc.h"
#include "../Engine/LaneAdapterProc.h"
#include "../Engine/TrafficlightProc.h"
#include "../Engine/StopSignProc.h"
#include "../Engine/TollGateProc.h"
#include "../Engine/YieldSignProc.h"
#include "../Engine/LineParkingProc.h"
#include "../Engine/NoseInParkingProc.h"
#include "../Engine/CloverLeafProc.h"

LandfieldProcessor::LandfieldProcessor(void)
{
	m_location.init(1);
}

LandfieldProcessor::~LandfieldProcessor(void)
{
}
void LandfieldProcessor::SetPath(const ARCPath3& path)
{
	m_path = path;
	Update();
	
}
void LandfieldProcessor::SetLaneNum(int n)
{
	m_nLaneNum =n;
	Update();
}

void LandfieldProcessor::SetLaneWidth(double width)
{
	m_dLaneWidth  = width;
	Update();
}

void LandfieldProcessor::SetLinkProcParts(const LandProcPartArray& _landproc)
{
	m_linkedProcs = _landproc;
	Update();
}

void LandfieldProcessor::SetClearance(double clearance)
{
	m_dclearance = clearance;
	Update();
}
void LandfieldProcessor::SetSpotLength(double spotlen)
{
	m_dSpotLen = spotlen;Update();
}
void LandfieldProcessor::SetSpotNumber(int spotN)
{
	m_nSpotNum = spotN;Update();
}
void LandfieldProcessor::SetSpotWidth(double spotwidth)
{
	m_dSpotWidth = spotwidth;Update();
}
void LandfieldProcessor::SetSpotSlopAnlge(double angleDegree)
{
	m_dSpotSlopAngle = angleDegree;Update();
}

int LandfieldProcessor::RefreshLinkedProcs(const ProcessorList& pProcList)
{
	LandProcPartArray newArray;
	LandProcPartArray::iterator itr;	
	for(itr = m_linkedProcs.begin();itr!= m_linkedProcs.end();itr++)
	{
		Processor* pProc = pProcList.getProcessor( (*itr).procName );
		if(pProc)
		{
			(*itr).pProc = pProc;
			newArray.push_back( (*itr) );
		}		
	}
	//remove null proc

	m_linkedProcs = newArray;Update();
	return TRUE;
}

int LandfieldProcessor::RefreshLinkedProcsName(const ProcessorList& pProcList)
{
	LandProcPartArray::iterator itr;	
	for(itr = m_linkedProcs.begin();itr!= m_linkedProcs.end();itr++)
	{
		(*itr).procName = (*itr).pProc->getIDName();
	}
	return TRUE;
}

//processor i/o	
int LandfieldProcessor::writeSpecialField(ArctermFile& procFile)const
{
	//constrol points write
	if(HasProperty(LandfieldProcessor::PropControlPoints))
	{
		int ncount = (int)m_path.size();
		procFile.writeInt(ncount);
		for(int i=0;i<ncount;i++)
		{
			procFile.writePoint( Point(m_path[i][VX],m_path[i][VY],m_path[i][VZ]) );
		}
	}
	//lane number write
	if(HasProperty(LandfieldProcessor::PropLaneNumber))
	{
		procFile.writeInt( GetLaneNum() );
	}
	//lane width write
	if(HasProperty(LandfieldProcessor::PropLaneWidth))
	{
		procFile.writeFloat( (float)GetLaneWidth() );
	}
	//linking procs write
	if(HasProperty(LandfieldProcessor::PropLinkingProces))
	{
		int nLinkprocCnt =(int) m_linkedProcs.size();
		procFile.writeInt(nLinkprocCnt);

		for(int i=0;i<nLinkprocCnt;i++)
		{
			procFile.writeField( m_linkedProcs[i].pProc->getIDName() );
			procFile.writeInt( m_linkedProcs[i].part);
			procFile.writeFloat( (float)m_linkedProcs[i].pos );
		}
	}
	//spot number write
	if(HasProperty(LandfieldProcessor::PropSpotNumber))
	{
		procFile.writeInt(GetSpotNumber());
	}
	//spot length write
	if(HasProperty(LandfieldProcessor::PropSpotLength))
	{
		procFile.writeFloat((float)GetSpotLength());
	}
	//spot width write
	if(HasProperty(LandfieldProcessor::PropSpotWidth))
	{
		procFile.writeFloat((float)GetSpotWidth());
	}
	//spot slop anlge
	if(HasProperty(LandfieldProcessor::PropSpotSlopAngle))
	{
		procFile.writeFloat((float)GetSpotSlopAnlge());
	}
	return TRUE;
}
int LandfieldProcessor::readSpecialField(ArctermFile& procFile) 
{
	int _integer; double _float;Point pt;char str[256];
	//constrol points read
	if(HasProperty(LandfieldProcessor::PropControlPoints))
	{
		procFile.getInteger(_integer);
		m_path.clear();m_path.reserve(_integer);
		for(int i=0;i<_integer;i++)
		{
			procFile.getPoint(pt);
			m_path.push_back( ARCVector3(pt) );
		}
	}
	//lane number read
	if(HasProperty(LandfieldProcessor::PropLaneNumber))
	{
		procFile.getInteger( _integer);
		m_nLaneNum = _integer;
	}
	//lane width read
	if(HasProperty(LandfieldProcessor::PropLaneWidth))
	{
		procFile.getFloat(_float);
		m_dLaneWidth = _float;
	}
	//linking procs read
	if(HasProperty(LandfieldProcessor::PropLinkingProces))
	{
		procFile.getInteger(_integer);
		int m_nCount = _integer;
		m_linkedProcs.reserve(m_nCount);
		for(int i=0;i<m_nCount;i++ )
		{
			LandProcPart newPart;
			procFile.getField(str,256);
			newPart.procName = str;
			procFile.getInteger(_integer);
			newPart.part = _integer;
			procFile.getFloat(_float);
			newPart.pos = _float;
			m_linkedProcs.push_back( newPart );
		}
	}
	//spot number read
	if(HasProperty(LandfieldProcessor::PropSpotNumber))
	{
		procFile.getInteger(_integer);
		SetSpotNumber(_integer);
	}
	//spot length read
	if(HasProperty(LandfieldProcessor::PropSpotLength))
	{
		procFile.getFloat(_float);
		SetSpotLength(_float);
	}
	//spot width read
	if(HasProperty(LandfieldProcessor::PropSpotWidth))
	{
		procFile.getFloat(_float);
		SetSpotWidth(_float);
	}
	//spot slop angle read;
	if(HasProperty(LandfieldProcessor::PropSpotSlopAngle))
	{
		procFile.getFloat(_float);
		SetSpotSlopAnlge(_float);
	}
	return TRUE;
}
void LandfieldProcessor::DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset )
{
	if(HasProperty(LandfieldProcessor::PropControlPoints))
	{
		for(size_t i=0;i< m_path.size();i++)
		{
			m_path[i] += ARCVector3(_xOffset,_yOffset,0);
		}
		Update();
	}

}

void LandfieldProcessor::Rotate( DistanceUnit _degree )
{
	if(HasProperty(LandfieldProcessor::PropControlPoints))
	{
		if(m_path.size()<1)return;
		//get the center pt;
		ARCVector3 center = m_path[0];
		
		for(size_t i=0;i<m_path.size();i++)
		{
			Point pcenter(center[VX],center[VY],center[VZ]);
			Point p = Point( m_path[i][VX],m_path[i][VY],m_path[i][VZ] );
			p.rotate(_degree,pcenter);
			m_path[i] = ARCVector3(p);
		}Update();
		
	}
	
}
LandfieldProcessor * LandfieldProcessor::NewProcessor(ProcessorClassList procType)
{
	LandfieldProcessor* pProc = NULL;
	switch(procType)
	{

	case StretchProcessor : 
		pProc = new StretchProc();	break;
	case IntersectionProcessor :
		pProc = new IntersectionProc(); break;
	case RoundAboutProcessor :
		pProc = new RoundAboutProc(); break;
	case TurnoffProcessor : 
		pProc = new TurnOffProc();break;
	case LaneAdapterProcessor :
		pProc = new LaneAdapterProc();break;
	case TrafficLightProceesor :
		pProc = new TrafficlightProc();break;
	case StopSignProcessor :
		pProc = new StopSignProc;break;
	case YieldDeviceProcessor :
		pProc = new YieldSignProc();break;
	case TollGateProcessor :
		pProc = new TollGateProc();break;
	case LineParkingProcessor :
		pProc = new LineParkingProc;break;
	case NoseInParkingProcessor :
		pProc = new NoseInParkingProc;break;
	case CloverLeafProcessor :
		pProc = new CloverLeafProc;break;
	default :
		ASSERT(FALSE);
	}
	return pProc;

}
LandfieldProcessor * LandfieldProcessor::GetCopy() 
{
	ASSERT(FALSE);
	return NULL;
}

void LandfieldProcessor::OffsetControlPoint(DistanceUnit _xOffset, DistanceUnit _yOffset , int ptidx)
{
	ASSERT( HasProperty(LandfieldProcessor::PropControlPoints) && ptidx <(int) m_path.size() );
	m_path[ptidx] += ARCVector3(_xOffset,_yOffset,0);Update();
}


void LandfieldProcessor::SetLaneDir(int iLane,int dir){
	m_lanedir.resize(m_nLaneNum);
	if(iLane>=m_nLaneNum || iLane<0) return ;
	m_lanedir[iLane] = dir;Update();
}

int LandfieldProcessor::GetLaneDir(int iLane)const{
	if(iLane>=m_nLaneNum || iLane<0) return 0;
	return m_lanedir[iLane];
}
