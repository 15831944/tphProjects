#include "StdAfx.h"
#include ".\animationmanager.h"
#include <common/util.h>
#include "TermPlanDoc.h"
#include <engine\Airside\CommonInSim.h>
#include <common/ARCUnit.h>
#include <common/CARCUnit.h>
#include <common\UnitsManager.h>
#include <Common\EventBinarySearch.h>
#include <InputAirside\ARCUnitManager.h>
#include <engine\BridgeConnector.h>
#include <results\BridgeConnectorLog.h>
#include <minmax.h>
#include <results\fltlog.h>
#include <results\proclog.h>
#include "Landside\IntersectionLinkGroupLogEntry.h"
#include "Landside\CrossWalkLogEntry.h"
#include "Movie.h"
#include "TermPlanDoc.h"
#include "Render3DView.h"
#include "3DView.h"
#include "common\WinMsg.h"
#include "results\EventLogBufManager.h"
#include "ChildFrm.h"
#include "Common\LandsideVehicleState.h"

static const double CORNEREPSION = 0.0035;


static inline double LinearInterpolate(double x1, double y1, double x2, double y2, double x)
{
	return y1 + (y2 - y1)/(x2 - x1)*(x - x1);
}

static inline ARCVector2 LinearInterpolateVector(double x1, ARCVector2 dir1, double x2, ARCVector2 dir2, double x)
{
	return ARCVector2(LinearInterpolate(x1, dir1.x, x2, dir2.x, x), LinearInterpolate(x1, dir1.y, x2, dir2.y, x));
}

//terminal processor whether display state on tags
bool CAnimationManager::DisplayProcessorStateOnTags(CTermPlanDoc* pDoc,Processor* pProc, long nTime)
{
	if (nTime < 0l)//animation non
		return true;
	

	if (pDoc->GetTerminal().procLog->getCount() == 0)//does not run terminal mode
		return true;
	
	ProcLogEntry logEntry ;
	logEntry.SetOutputTerminal(&(pDoc->GetTerminal()));
	logEntry.SetEventLog(pDoc->GetTerminal().m_pProcEventLog);
	pDoc->GetTerminal().procLog->getItem (logEntry,pProc->getIndex());
	
	//check processor activity
	int lSize = logEntry.getCount();
	if(lSize == 0 )//processor event is empity
		return true;

	//current time exceed processor activity time. processor display gray with light
	if (logEntry.getCurrentCount() == 0) //check log arleady load
	{
		logEntry.loadEvents();
	}

	long nextIdx = LinearSearchEvent(logEntry.getEventList(),nTime);
	if(nextIdx<=0)
	{
		nextIdx = lSize;
	}

	//check current time in processor alive time
	int testAIdx = nextIdx - 1;
	
	for (long i = testAIdx; i >= 0; i--)
	{
		const ProcEventStruct& tesA = logEntry.getEvent(i);
		//processor close
		if (CloseForService == tesA.type  || StartToClose == tesA.type)
		{
			return false;
		}

		if (OpenForService == tesA.type)
		{
			return true;
		}
	}
	
	return true;
}

static const long  g_snSpotAngleMaxTimeDiff = 1000; // 10 seconds, modify this variable to adapt effect
static const long  g_snPaxPutLuggageTime = 1000; // 10 seconds

bool CAnimationManager::GetPaxShow(CTermPlanDoc* pDoc, const part_event_list& part_list, long nTime, BOOL* bOn, double* dAlt,
								   AnimationPaxGeometryData& geoData, PaxAnimationData& animaPaxData, int& pesAIdx)
{

	int nSize = (int)part_list.size();

	long nextIdx = LinearSearchEvent(part_list,nTime);
	if(nextIdx<=0)
		return false;

	const MobEventStruct& pesB = part_list[nextIdx];
	const MobEventStruct& pesA = part_list[nextIdx-1];
	pesAIdx = nextIdx-1;

	if(pesA.state == On_Vehicle ||
		pesA.state == StayOnVehicle)
		return false;

	double r = 0 ;
	//ASSERT(pesB.time == pesA.time) ;
	if((pesA.speed==0&& pesB.speed==0))
	{
		int w = pesB.time - pesA.time;
		int d = pesB.time - nTime;
		r = ((double) d) / w;
	}
	else
	{	
		double rate = static_cast<double>(nTime-pesA.time)/static_cast<double>(pesB.time-pesA.time);				
		double nCurSpeed = (1-rate)*pesA.speed+rate*pesB.speed;				
		double flyedLength = (pesA.speed+nCurSpeed)*(nTime-pesA.time);//pesA.speed*(nCurTime-pesA.time)+accel*(nCurTime-pesA.time)*(nCurTime-pesA.time)/2;
		double flyingLength  = (pesA.speed+pesB.speed)*(pesB.time-pesA.time);//pesA.speed*(pesB.time-pesA.time)+accel*(pesB.time-pesA.time)*(pesB.time-pesA.time)/2;			
		r = 1.0 - flyedLength/flyingLength;
	}

	//3)interpolate position based on the two PaxEventStructs and the time
	//int w = pesB.time - pesA.time;
	//int d = pesB.time - nTime;
	//float r = ((float) d) / w;
	double xpos = (1.0f-r)*pesB.x + r*pesA.x;
	double ypos = (1.0f-r)*pesB.y + r*pesA.y;

	double zA ,zB;
	
	double scaleFactor = pDoc->m_iScale;
	int nFloorCount = pDoc->GetFloorCount();

	if(pesA.state >= EntryLandside){
		zA = pDoc->GetFloorByMode(EnvMode_LandSide).GetVisualHeight(pesA.m_RealZ);
	}
	else
	{
		if(pesA.m_bWithOwner) //non pax 
		{
			if(pesA.m_IsRealZ)
			{
				zA = pesA.m_RealZ;
			}
			else
			{
				int nFloorA = (int) (pesA.z/SCALE_FACTOR);
				double mult = pesA.z/SCALE_FACTOR - nFloorA;
				double dAltDiff = 0;

				if( nFloorA+1 < nFloorCount )
					dAltDiff = dAlt[nFloorA+1]-dAlt[nFloorA];
				zA = dAlt[nFloorA]+mult*dAltDiff;
				zA += pesA.m_RealZ;

			}						
		}
		else //pax
		{
			if(pesA.m_IsRealZ)
			{
				zA = pesA.m_RealZ;
			}
			else
			{
				int nFloorA = (int) (pesA.z/SCALE_FACTOR);
				double mult = pesA.z/SCALE_FACTOR - nFloorA;
				double dAltDiff = 0;

				if( nFloorA+1 < nFloorCount )
					dAltDiff = dAlt[nFloorA+1]-dAlt[nFloorA];
				zA = dAlt[nFloorA]+mult*dAltDiff;	

			}			
		}
	}

	//pes B
	int nFloorB = 0;
	if(pesB.state>= EntryLandside)
	{
		zB = pDoc->GetFloorByMode(EnvMode_LandSide).GetVisualHeight(pesB.m_RealZ);
	}
	else
	{
		nFloorB = (int) (pesB.z/SCALE_FACTOR);
		if( pesB.m_bWithOwner)
		{
			if(pesB.m_IsRealZ)
			{
				zB = pesB.m_RealZ/** scaleFactor*/;
				nFloorB = 0;
			}
			else
			{
				//zB = pDoc->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude(pesB.z/SCALE_FACTOR);
				double mult = pesB.z/SCALE_FACTOR - nFloorB;
				double dAltDiff = 0.0;
				if( nFloorB+1 < nFloorCount )
					dAltDiff = dAlt[nFloorB+1]-dAlt[nFloorB];
				zB = dAlt[nFloorB]+mult*dAltDiff;
				zB += pesB.m_RealZ;
			}
		}
		else
		{
			if(pesB.m_IsRealZ)
			{
				zB = pesB.m_RealZ/** scaleFactor*/;
				nFloorB = 0;
			}
			else
			{
				//zB = pDoc->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude(pesB.z/SCALE_FACTOR);
				double mult = pesB.z/SCALE_FACTOR - nFloorB;
				double dAltDiff = 0.0;
				if( nFloorB+1 < nFloorCount )
					dAltDiff = dAlt[nFloorB+1]-dAlt[nFloorB];
				zB = dAlt[nFloorB]+mult*dAltDiff;	
			}
		}
	}
	double zpos = (1-r)*zB + r*zA;
	//float zpos_high=0.0;
	//zpos_high += (float)dAltDiff;

	//calc direction
	ARCVector2 dir;
	double dMag;

	long tmpIdx = nextIdx;
	do // find pre-event which has diff pos
	{
		tmpIdx--;
		const MobEventStruct& pesTemp = part_list[tmpIdx];
		dir[VX] = pesB.x - pesTemp.x;
		dir[VY] = pesB.y - pesTemp.y;
	} while((dMag=dir.Magnitude())<ARCMath::EPSILON && (nextIdx-tmpIdx)<5 && tmpIdx > 0);

	double anglefromX = dir.AngleFromCoorndinateX();
	if(pesA.backup)
	{
		anglefromX += 180.0;
	}
	else
	{
		if (pesA.m_motionDir == 'L')
		{
			anglefromX += 90.0;
		}
		else if (pesA.m_motionDir == 'R')
		{
			anglefromX -= 90.0;
		}
		else if(pesA.m_motionDir == 'B')//backward direction
		{
			anglefromX += 180;
		}
	}

	if (geoData.bBestSpotAngle)
	{
		geoData.bestSpotAngle = anglefromX;
		if (pesB.time - nTime < g_snSpotAngleMaxTimeDiff)
		{
			long nListSize = part_list.size();
			ARCVector2 dirPost;
			long tmpIdxPost = nextIdx + 1;
			long nConcernIndex = min(nListSize, nextIdx + 10);
			while (tmpIdxPost<nConcernIndex)
			{
				const MobEventStruct& pesPost = part_list[tmpIdxPost];
				dirPost[VX] = pesPost.x - pesB.x;
				dirPost[VY] = pesPost.y - pesB.y;
				if (dirPost.Magnitude()>=ARCMath::EPSILON)
				{
					// when found post event has diff pos
					dir.Normalize(); if (pesA.backup) dir = -dir;
					dirPost.Normalize(); if (pesB.backup) dirPost = -dirPost;
					ARCVector2 dirInter = LinearInterpolateVector(pesB.time - g_snSpotAngleMaxTimeDiff, dir,
						pesB.time, dirPost, nTime);
					geoData.bestSpotAngle = dirInter.AngleFromCoorndinateX();
					break;
				}
				tmpIdxPost++;
			}
		}
	}

	if (pesA.state>=EntryOnboard&& pesA.state<EntryLandside) // onboard animation calculation
	{
		long nDeltaTime = pesB.time - pesA.time;
		float dAvgSpeed = nDeltaTime>0 ? sqrt(pow(pesB.x - pesA.x, 2) + pow(pesB.y - pesA.y, 2))/nDeltaTime : 0.0f;
		if (dAvgSpeed<ARCMath::EPSILON)
		{
			// standing, putting luggage or seated
			// back track event log:
			// a. SitOnSeat found while StandUp not found, start or continue PaxSeated
			// b. PutHandsUp found while PutHandsDown not found, start or continue PaxPuttingLuggage,
			//    calc animation clip time length as time(PutHandsDown) - time(PutHandsUp)
			// c. StandUp found, or PutHandsDown found, or else, start PaxStanding
			int pesAIdx2 = pesAIdx;
			const int nMaxTrackCount = 10;
			const int nBackEndIndex = max(0, pesAIdx2 - nMaxTrackCount);

			bool bAnimationOK = false;
			while (pesAIdx2>=nBackEndIndex)
			{
				const MobEventStruct& pesPre = part_list[pesAIdx2];
				switch (pesPre.state)
				{
				case PutHandsUp://pick up
					{
// 						long nTimeLength = -1;
// 						if (PaxPuttingLuggage != animaPaxData.eAnimaState)
// 						{
// 							// if new start new putting luggage, need to calculate animation time length
// 							int pesBIdx = nextIdx;
// 							const int nForeEndIndex = min(static_cast<int>(part_list.size()), pesBIdx + nMaxTrackCount);
// 
// 							while (pesBIdx<nForeEndIndex)
// 							{
// 								const MobEventStruct& pesPost = part_list[pesBIdx];
// 								if (PutHandsDown == pesPost.state)
// 								{
// 									nTimeLength = pesPost.time - pesPre.time;
// 									break;
// 								}
// 								pesBIdx++;
// 							}
// 						}
//						animaPaxData.StartOrContinueAnimation(PaxPuttingLuggage, pesPre.time, nTime, nTimeLength);
						animaPaxData.StartOrContinueAnimation(PaxPushOn, pesPre.time, nTime);
						bAnimationOK = true;
					}
					break;
				case StorageCarryOn:
					animaPaxData.StartOrContinueAnimation(PaxPickup, pesPre.time, nTime);
					bAnimationOK = true;
					break;
				case PutHandsDown:
					animaPaxData.StartOrContinueAnimation(PaxPushDown, pesPre.time, nTime);
					bAnimationOK = true;
					break;
				case SitOnSeat:
					{
						animaPaxData.StartOrContinueAnimation(PaxSeated, pesPre.time, nTime);
						bAnimationOK = true;
					}
					break;
				case StandUp:
				case WaittingAtCheckingPoint:
				case WaitAtTempPlace:
					animaPaxData.StartOrContinueAnimation(PaxStanding, pesPre.time, nTime);
					bAnimationOK = true;
					break;

				default://default is standing
					animaPaxData.StartOrContinueAnimation(PaxStanding, pesPre.time, nTime);
					bAnimationOK = true;
					break;
				}
				if (bAnimationOK)
					break;
	
				pesAIdx2--;
			}
			if (!bAnimationOK)
			{
				animaPaxData.StartOrContinueAnimation(PaxStanding, pesA.time, nTime);
			}
		}
		else
		{
			// certainly he or she is walking now
// 			const float d2PacesSize = ;
// 			long nTimeLength = long(d2PacesSize/dAvgSpeed);
			if (pesA.m_motionDir == 'L')
			{
				animaPaxData.StartOrContinueAnimation(PaxSidleLeft, pesA.time, nTime);
			}
			else if (pesA.m_motionDir == 'R')
			{
				animaPaxData.StartOrContinueAnimation(PaxSidleRight, pesA.time, nTime);
			}
			else if (pesA.m_motionDir == 'B' || pesA.backup)
			{
				animaPaxData.StartOrContinueAnimation(PaxBackaway, pesA.time, nTime);
			}
			else
			{
				animaPaxData.StartOrContinueAnimation(PaxWalking, pesA.time, nTime, 250/*2.5s*/);
			}
			
		}
	}
	

	//handle bridge show
	/*BOOL bBridgePaxShow = FALSE;
	BOOL bBridge = FALSE;
	if (pesA.procNumber == pesB.procNumber)
	{
		Processor* pPro = pDoc->GetTerminal().GetTerminalProcessorList()->getProcessor(pesA.procNumber);
		if (pPro && pPro->getProcessorType() == BridgeConnectorProc)
		{
			const ProcessorID& id = *(pPro->getID());
			CProcessor2* pPro2 = pDoc->GetProcessor2FromID(id, EnvMode_Terminal);

			int nFloor = pPro2->GetFloor();
			bBridgePaxShow = bOn[nFloor];
			bBridge = TRUE;
		}
	}*/

	geoData.pos = ARCVector3(xpos,ypos,zpos);
	geoData.dAngleFromX = anglefromX;
	geoData.bNeedRotate = pesB.m_bWithOwner;
	BOOL bOnFloor = TRUE;
	if(!pesB.m_IsRealZ)
		bOnFloor = bOn[nFloorB];

	
	return (/*(bBridgePaxShow && bBridge)|| */(bOnFloor && pesA.m_bVisible));

}

bool CAnimationManager::GetFlightShow(const CAirsideEventLogBuffer<AirsideFlightEventStruct>::ELEMENTEVENT& ItemEventList,long nCurTime,const DistanceUnit& airportAlt,
				   ARCVector3& pos, ARCVector3& danlges, double& dSpeed
				   ,int& pesAIdx)
{
	long nextIdx = LinearSearchEvent(ItemEventList.second,nCurTime);
	if(nextIdx<=0)
		return false;

	const AirsideFlightEventStruct& pesB = ItemEventList.second[nextIdx];
	const AirsideFlightEventStruct& pesA = ItemEventList.second[nextIdx-1];
	pesAIdx = nextIdx - 1;

	AirsideFlightEventStruct pesC;
	AirsideFlightEventStruct pesD,pesE,pesF;
	size_t nEventCount = ItemEventList.second.size();

	for (int k=1;;k++)
	{
		if (nextIdx+k<static_cast<int>(nEventCount))
		{
			pesC = ItemEventList.second[nextIdx + k];
			if (pesC.x!=pesB.x||pesC.y!=pesB.y||pesC.z!=pesB.z)
				break;
		}
		else
		{
			pesC = ItemEventList.second[nextIdx]; 
			break;
		}
	}
	//
	// calculate the current location
	//double accel = (pesB.speed-pesA.speed)/(pesB.time-pesA.time);
	double r=0.0;
	{
		double rate = static_cast<double>(nCurTime-pesA.time)/static_cast<double>(pesB.time-pesA.time);
		if(pesA.speed+pesB.speed>ARCMath::EPSILON)
		{
			double nCurSpeed = (1-rate)*pesA.speed+rate*pesB.speed;				
			r = (pesA.speed+nCurSpeed)*rate/(pesA.speed+pesB.speed);
		}
		else 
			r = rate;
	}

	double xpos = (1.0 - r) * pesA.x + r * pesB.x;
	double ypos = (1.0 - r) * pesA.y + r * pesB.y;
	double offsetAngle = (1.0 -r )*pesA.offsetAngle + r * pesB.offsetAngle;
	dSpeed = (1.0-r)* pesA.speed + r * pesB.speed;


	double zA = pesA.z;
	double zB = pesB.z;
	double zpos = ((1.0 - r)*zA + r*zB) - airportAlt;
	//zpos += 440;

	//calc direction
	//calc the smooth direction

	double dAngleFromX = 0.0;
	long tmpIdx = nextIdx;
	ARCVector3 dir;
	double dMag;
	do
	{
		tmpIdx--;
		const AirsideFlightEventStruct& pesTemp = ItemEventList.second.at(tmpIdx);
		dir[VX] = pesB.x - pesTemp.x;
		dir[VY] = pesB.y - pesTemp.y;
		dir[VZ] = pesB.z - pesTemp.z;					
	} while((dMag=dir.Magnitude())<ARCMath::EPSILON && (nextIdx-tmpIdx)<50 && tmpIdx > 1);
	dir.Normalize();	

	ARCVector2 dir2d(dir[VX],dir[VY]);dir2d.Normalize();
	double dangleZ = ARCMath::RadiansToDegrees(acos(dir2d[VX])) + offsetAngle;
	if(dir2d[VY]<0)dangleZ = -dangleZ;

	if(pesA.IsBackUp)
	{
		dangleZ += 180.0;
	}
	double dangleY = -ARCMath::RadiansToDegrees(asin(dir[VZ]));

	ARCVector3 dirCB(pesC.x-pesB.x, pesC.y-pesB.y, pesC.z-pesB.z); 
	ARCVector3 dirBA(pesB.x-pesA.x, pesB.y-pesA.y, pesB.z-pesA.z);
	double dangleX= 0.0;
	int absIndex = 0, addIndex=0, totalIndex=0, midIndex=0;
	ARCVector3 vectorDE(0.0,0.0,0.0), vectorEF(0.0,0.0,0.0);

	ARCVector3 pesApt(pesA.x,pesA.y,pesA.z);
	ARCVector3 pesBpt(pesB.x,pesB.y,pesB.z);
	double dDistAB = pesApt.DistanceTo(pesBpt);

	if(pesA.mode != OnBirth &&  pesC.mode < OnLanding && dDistAB < 100000/*|| pesC.mode > OnTakeoff*/)
	{
		if (fabs(dirCB.Normalize().dot(dirBA.Normalize()))<=1.0-CORNEREPSION&& dirCB!=ARCVector3(0.0,0.0,0.0))
		{
			do 
			{
				absIndex++;
				pesD = ItemEventList.second[nextIdx-absIndex+1];
				pesE = ItemEventList.second[nextIdx-absIndex];
				if(nextIdx-absIndex-1<0)
					break;									
				pesF = ItemEventList.second[nextIdx-absIndex-1];
				vectorDE = ARCVector3(pesD.x-pesE.x, pesD.y-pesE.y, pesD.z-pesE.z);
				vectorEF = ARCVector3(pesE.x-pesF.x, pesE.y-pesF.y, pesE.z-pesF.z);
				double y = 1.0-fabs(vectorDE.Normalize().dot(vectorEF.Normalize()));
			} while(fabs(vectorDE.Normalize().dot(vectorEF.Normalize()))<=1.0-CORNEREPSION);

			do 
			{
				addIndex++;
				if(nextIdx+addIndex+1>static_cast<int>(nEventCount))
					break;
				pesD = ItemEventList.second[nextIdx+addIndex+1];
				pesE = ItemEventList.second[nextIdx+addIndex];
				pesF = ItemEventList.second[nextIdx+addIndex-1];
				vectorDE = ARCVector3(pesD.x-pesE.x, pesD.y-pesE.y, pesD.z-pesE.z);
				vectorEF = ARCVector3(pesE.x-pesF.x, pesE.y-pesF.y, pesE.z-pesF.z);
				double x = 1.0-fabs(vectorDE.Normalize().dot(vectorEF.Normalize()));
			} while(fabs(vectorDE.Normalize().dot(vectorEF.Normalize()))<=1.0-CORNEREPSION);

			totalIndex = absIndex+addIndex+1;
			midIndex = totalIndex/2;

			if (absIndex<=midIndex)
			{
				dangleX = absIndex*5;
			}
			else
			{
				dangleX = (totalIndex-absIndex)*5;
			}

			ARCVector3 dirCorss = cross(dirBA.Normalize(),dirCB.Normalize()).Normalize();
			if(dirCorss[VZ]>=0.0)
			{
				dangleX=-dangleX;
			}
		}
	}

	danlges = ARCVector3(dangleX,dangleY,dangleZ);
	pos = ARCVector3(xpos,ypos,zpos);
	return true;
}



bool CAnimationManager::GetVehicleShow(CTermPlanDoc* pDoc,const CAirsideEventLogBuffer<AirsideVehicleEventStruct>::ELEMENTEVENT& ItemEventList,long nCurTime,const DistanceUnit& airportAlt
					,ARCVector3& pos, ARCVector3& danlges, double& dSpeed,int& pesAIdx)
{

	long nextIdx = LinearSearchEvent(ItemEventList.second, nCurTime);			
	if(nextIdx<=0)
		return false;

		const AirsideVehicleEventStruct& pesB = ItemEventList.second[nextIdx];
		const AirsideVehicleEventStruct& pesA = ItemEventList.second[nextIdx - 1];
		//
		// calculate the current location
		double r;
		if(pesA.speed==0&&pesB.speed==0)
		{
			r = static_cast<double>(nCurTime-pesA.time)/static_cast<double>(pesB.time-pesA.time);
		}
		else
		{	
			double rate = static_cast<double>(nCurTime-pesA.time)/static_cast<double>(pesB.time-pesA.time);				
			double nCurSpeed = (1-rate)*pesA.speed+rate*pesB.speed;				
			double flyedLength = (pesA.speed+nCurSpeed)*(nCurTime-pesA.time);//pesA.speed*(nCurTime-pesA.time)+accel*(nCurTime-pesA.time)*(nCurTime-pesA.time)/2;
			double flyingLength  = (pesA.speed+pesB.speed)*(pesB.time-pesA.time);//pesA.speed*(pesB.time-pesA.time)+accel*(pesB.time-pesA.time)*(pesB.time-pesA.time)/2;			
			r = flyedLength/flyingLength;
		}

		double xpos = (1.0 - r) * pesA.x + r * pesB.x;
		double ypos = (1.0 - r) * pesA.y + r * pesB.y;

		double zA = pesA.z;
		double zB = pesB.z;
		
		double scaleFactor = static_cast<double>(pDoc->m_iScale);
		double zpos = (1.0 - r)*zA*scaleFactor + r*zB*scaleFactor;
		
		//calc direction
		//calc the smooth direction
		double dangleX = 0.0;
		long tmpIdx = nextIdx;
		ARCVector3 dir;
		double dMag;
		do
		{
			tmpIdx--;
			const AirsideVehicleEventStruct& pesTemp = ItemEventList.second[tmpIdx];
			dir[VX] = pesB.x - pesTemp.x;
			dir[VY] = pesB.y - pesTemp.y;
			dir[VZ] = pesB.z*scaleFactor - pesTemp.z*scaleFactor;					
		} while((dMag=dir.Magnitude())<ARCMath::EPSILON && (nextIdx-tmpIdx)<50 && tmpIdx > 0);

		dir.Normalize();
		ARCVector2 dir2d(dir[VX],dir[VY]);dir2d.Normalize();
		double dangleZ = ARCMath::RadiansToDegrees(acos(dir2d[VX]));
		if(dir2d[VY]<0)dangleZ = -dangleZ;

		if(pesA.IsBackUp)
		{
			dangleZ += 180.0;
		}
		double dangleY = ARCMath::RadiansToDegrees(asin(dir[VZ]));
		pos = ARCVector3(xpos,ypos,zpos);
		danlges = ARCVector3(dangleX,dangleY,dangleZ);
		pesAIdx = nextIdx - 1;
		return true;

}

void CAnimationManager::BuildFlightTag( const AirsideFlightDescStruct& _fds, const AirsideFlightEventStruct& _fes, const CAircraftTagItem& _ati, bool bExpanded, CString& _sTag, std::vector<C3DTextManager::TextFormat>& formats, int& nNumLines, int& nMaxCharsInLine, double fSpeed /*= 0.0*/, double dAlt /*= 0.0*/ )
{
	CString strTemp;
	strTemp.Empty();
	BOOL bTagShow = fSpeed > 0.00 ? TRUE : FALSE;
	nNumLines=0;
	nMaxCharsInLine=0;

	C3DTextManager::TextFormat fmtBlack;
	fmtBlack.clr = ARCColor3::Black;

	C3DTextManager::TextFormat fmtRed;
	fmtRed.clr = ARCColor3::Red;

	DWORD dwTagInfo = _ati.GetTagInfo();	
	if( ACTAG_FLIGHTID & dwTagInfo )
	{
		CString strAirline;	
		if(_fes.state == 'A')
			strAirline = _fds.arrID;
		else if(_fes.state == 'D')
			strAirline = _fds.depID;
		else	
			strAirline = _fds.flightID;

		strTemp.Format("\nFlight ID: %s%s(%d)",_fds._airline.GetValue(), strAirline, _fds.id);		
		_sTag += strTemp;
		formats.push_back(fmtBlack);
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}
	if( ACTAG_OPERATEMODE & dwTagInfo )
	{
		CString sLab;
		if( (int)strlen(_fds.arrID)> 0 && (int)strlen(_fds.depID)> 0)
			sLab.Format("Turn Round\t arrID:%s%s depID:%s%s", _fds.icao_code,_fds.arrID,_fds.icao_code,_fds.depID);
		else if ((int)strlen(_fds.arrID)> 0)
			sLab.Format("Arriving:\t arrID:%s%s", _fds.icao_code,_fds.arrID);
		else if ((int)strlen(_fds.depID)> 0)
			sLab.Format("Departing:\t depID:%s%s", _fds.icao_code,_fds.depID);
		strTemp.Format("\nOperation Mode: %s", sLab);
		_sTag += strTemp;
		formats.push_back(fmtBlack);
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}

	if( ACTAG_SPEED & dwTagInfo )
	{
		double convertspd = CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_MPS,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),fSpeed/SCALE_FACTOR);
		strTemp.Format("\nSpeed: %.2f %s",convertspd,CARCVelocityUnit::GetVelocityUnitString(CARCUnitManager::GetInstance().GetCurSelVelocityUnit() ) );
		_sTag += strTemp;
		formats.push_back(_fes.IsSpeedConstraint ? fmtRed : fmtBlack);
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}
	if( ACTAG_GATE & dwTagInfo )
	{	
		//pFlight->getGate().printID(strbuf);
		strTemp.Format("\nGate: %s", _fes.gatecode);
		_sTag += strTemp;
		formats.push_back(fmtBlack);
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}
	if( ACTAG_RUNWAY & dwTagInfo )
	{
		strTemp.Format("\nRunway: %s", _fes.rwcode);
		_sTag += strTemp;
		formats.push_back(fmtBlack);
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}
	if( ACTAG_SIDSTAR & dwTagInfo )
	{//Temp
		switch(_fes.eventCode) {
case 'a':
	strTemp.Format("\nStar: %s", _fes.starsidname);
	_sTag += strTemp;
	formats.push_back(fmtBlack);
	break;
case 'd':
	strTemp.Format("\nSID: %ld", _fes.starsidname);
	_sTag += strTemp;
	formats.push_back(fmtBlack);
	break;
default:
	break;
		}
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}
	if( ACTAG_ORIGDEST & dwTagInfo )
	{
		strTemp.Format("\nOrig - Dest: %s - %s", _fds.origin, _fds.dest);
		_sTag += strTemp;
		formats.push_back(fmtBlack);
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}	
	if( ACTAG_ACTYPE & dwTagInfo )
	{
		strTemp.Format("\nACType: %s", _fds.acType);
		_sTag += strTemp;
		formats.push_back(fmtBlack);
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}
	if( ACTAG_ALTITUDEC & dwTagInfo )
	{
		//NO Data Now
		strTemp.Format("\nAltitude: %.2f ft",ARCUnit::ConvertLength(dAlt, ARCUnit::CM,ARCUnit::FEET) );
		_sTag += strTemp;
		formats.push_back(fmtBlack);
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}
	if( ACTAG_STATE & dwTagInfo )
	{
		strTemp.Format("\nState: %s",AIRSIDEMOBILE_MODE_NAME[_fes.mode] );
		_sTag += strTemp;
		formats.push_back(fmtBlack);
		nMaxCharsInLine = max(nMaxCharsInLine, strTemp.GetLength());
		nNumLines ++;
	}
}

void CAnimationManager::BuildVehicleTag( const AirsideVehicleDescStruct& _fds, const AirsideVehicleEventStruct& _fes, const DWORD& dwTagInfo, bool bExpanded, CString& _sTag, int& nNumLines, int& nMaxCharsInLine, float fSpeed /*= 0.00*/ )
{
	CString strTemp;
	strTemp.Empty();
	//	BOOL bTagShow = fSpeed > 0.00 ? TRUE : FALSE;
	//Flight * pFlight = (Flight*)_fds.userCode;
	//	char strbuf[256];
	nNumLines=0;
	nMaxCharsInLine=0;


	if(VEHICLE_TAG_FLAG_VEHICLE_ID & dwTagInfo)
	{
		strTemp.Format("\nVehicle ID: %s(%d)",_fds.vehicleType,_fds.IndexID);
		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}
	if (VEHICLE_TAG_FLAG_POOL & dwTagInfo)
	{
		strTemp.Format("\nPool: %ld",_fds.poolNO);
		_sTag += strTemp;
		nMaxCharsInLine =max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}
	if(VEHICLE_TAG_FLAG_SERVICE_STAND & dwTagInfo)
	{		
		strTemp.Format("\nStand ID: %s",_fes.serveStand);
		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}
	if(VEHICLE_TAG_FLAG_SERVICE_FLIGHT & dwTagInfo)
	{
		if (_fes.m_ServerFlightID != -1)
		{
			strTemp.Format("\nService Flight: %s%s(%d)",_fes.serverFlightAirline,_fes.serverFlightID, _fes.m_ServerFlightID);		
		}else
		{
			strTemp.Format("\nService Flight:");		
		}
		//strTemp.Format("\nService Flight: %d",_fes.m_ServerFlightID);
		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}	
}

bool CAnimationManager::GetBridgeShow( BridgeConnector* pConnector,int idx, CTermPlanDoc* pDoc,double dAlt , CPoint2008& pStartPos, CPoint2008& pEndPos )
{
	
	CBridgeConnectorLog* pLog = pDoc->GetTerminal().m_pBridgeConnectorLog;
	const BridgeConnector::ConnectPoint& p = pConnector->m_connectPoint;//(idx);

	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none)
	{
		long nTime = pDoc->m_animData.nLastAnimTime;
		CBridgeConnectorLog* pLog = pDoc->GetTerminal().m_pBridgeConnectorLog;

		int nLogCount = pLog->getCount();
		for(int i=0;i<nLogCount;i++)
		{
		
			CBridgeConnectorLogEntry bridgeElment;
			BridgeConnectorDescStruct bds;
			pLog->getItem(bridgeElment,i);
			bridgeElment.SetOutputTerminal(&(pDoc->GetTerminal()));
			bridgeElment.SetEventLog(pDoc->GetTerminal().m_pBridgeConnectorEventLog);
			bridgeElment.initStruct(bds);	

			//CString sLift;
			//sLift.Format("%d", idx );
			ProcessorID bid;
			bid.setIDS(bds.name);
			if( !(bid == *pConnector->getID()) )
				continue;
		

			BridgeConnectorEventStruct besA,besB;
			bool bRet = bridgeElment.getEventsAtTime(nTime,besA,besB);
			if(bRet)
			{

				float besBZ, besAZ;
				int w = besB.time - besA.time;
				int d = besB.time - nTime;
				double r = ((double) d) / w;
				float xpos = static_cast<float>((1-r)*besB.x + r*besA.x);
				float ypos = static_cast<float>((1-r)*besB.y + r*besA.y);
				float zPos;

				if (besB.state == 'C'|| besB.state == 'D')//state 'C' and 'D' is terminal mode
				{
					besBZ = static_cast<float>(besB.z);
				}
				else
				{
					besBZ = static_cast<float>(dAlt);
				}

				if (besA.state == 'C' || besA.state == 'D')//state 'C' and 'D' is terminal mode
				{
					besAZ = static_cast<float>(besA.z);
				}
				else
				{
					besAZ = static_cast<float>(dAlt);
				}

				zPos = static_cast<float>((1-r)*besBZ + r*besAZ);

				pStartPos = CPoint2008(p.m_Location.getX(),p.m_Location.getY(),0);						
				pEndPos = CPoint2008(xpos,ypos,zPos-dAlt);			
				return true;
			}
		}
	}
	pStartPos = CPoint2008(p.m_Location.getX(),p.m_Location.getY(),0);		
	pEndPos = CPoint2008(p.m_dirFrom.getX(),p.m_dirFrom.getY(),0);	
	ARCVector3 dir = (pEndPos-pStartPos);
	dir.SetLength(p.m_dLength);
	pEndPos = pStartPos+dir;
	return false;
}

void CAnimationManager::BuildPaxTag( CTermPlanDoc* pDoc,DWORD _ti, const MobDescStruct& _MDS,const MobEventStruct& _MESa, CString& _sPaxTag )
{
	static char buf[256];
	CString stmp;

	Terminal* pTerm = &pDoc->GetTerminal(); //&GetTerminal();

	if(_ti & PAX_ID) {
		switch( _MDS.mobileType )
		{
		case 0:
			stmp.Format("\nPax ID: %d", _MDS.id);
			break;
		case 1:
			stmp.Format("\nVisitor ID: %d (%d)", _MDS.id, _MDS.ownerID );
			break;
		case 2:
			stmp.Format("\nChecked Baggage ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 3:
			stmp.Format("\nHand Bag ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 4:
			stmp.Format("\nCarte ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 5:
			stmp.Format("\nWheelchair ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 6:
			stmp.Format("\nAnimal ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 7:
			stmp.Format("\nCar ID: %d", _MDS.id);
			break;
			//case 8:
			//case 9:
			//case 10:
			//case 11:
			//case 12:
			//case 13:
			//case 14:
			//case 15:
			//case 16:
		default:
			stmp.Format("\n%s ID: %d (%d)", pTerm->m_pMobElemTypeStrDB->GetString( _MDS.mobileType), _MDS.id, _MDS.ownerID );
			break;
		}
		_sPaxTag=_sPaxTag+stmp;
	}
	if(_ti & PAX_FLIGHTID) {
		int idx = -1;
		if(_MDS.depFlight != -1)
		{
			idx = _MDS.depFlight;
			if(idx != -1) 
			{
				pTerm->flightLog->getFlightID( buf, idx, 'D' );
				stmp.Format("\nFlight ID: %s",buf);
				_sPaxTag = _sPaxTag+stmp;
			}
		}
		else if(_MDS.arrFlight != -1)
		{
			idx = _MDS.arrFlight;
			if(idx != -1) 
			{
				pTerm->flightLog->getFlightID( buf, idx, 'A' );
				stmp.Format("\nFlight ID: %s",buf);
				_sPaxTag = _sPaxTag+stmp;
			}
		}
	}
	if(_ti & PAX_PAXTYPE) {
		CMobileElemConstraint pc(pTerm);
		//pc.SetInputTerminal(pTerm);
		pc.setIntrinsicType(_MDS.intrinsic);
		for(int l=0; l<MAX_PAX_TYPES; l++)
			pc.setUserType(l,_MDS.paxType[l]);
		pc.SetTypeIndex( _MDS.mobileType );
		pc.screenPrint(buf,0,256);
		stmp.Format("\nPax Type: %s",buf);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_GATE) {
		//int idx = -1;
		//if(_MDS.depFlight != -1)
		//	idx = _MDS.depFlight;
		//else if(_MDS.arrFlight != -1)
		//	idx = _MDS.arrFlight;
		//if(idx != -1) 
		//{
		//	FlightLogEntry fltLogEntry;
		//	pTerm->flightLog->getItem( fltLogEntry, idx );
		//	int nGateIdx = fltLogEntry.getGate();
		//	ProcLogEntry proLogEntry;
		//	pTerm->procLog->getItem( proLogEntry, nGateIdx );
		//	proLogEntry.SetOutputTerminal(pTerm);
		//	proLogEntry.getID( buf );
		//	stmp.Format("\nGate: %s",buf);
		//	_sPaxTag = _sPaxTag+stmp;
		//}
		FlightLogEntry fltLogEntry;
		ProcLogEntry   proLogEntry;
		int idx = -1,nGateIdx;
		if(_MDS.depFlight != -1)
		{
			idx = _MDS.depFlight;
			if (idx != -1)
			{
				pTerm->flightLog->getItem(fltLogEntry, idx);
				nGateIdx = fltLogEntry.getDepGate();
				if(nGateIdx >= 0)
				{
					pTerm->procLog->getItem(proLogEntry, nGateIdx);
					proLogEntry.SetOutputTerminal(pTerm);
					proLogEntry.getID(buf);
					stmp.Format("\nGate: %s",buf);
					_sPaxTag = _sPaxTag+stmp;
				}
			}
		}
		else if (_MDS.arrFlight != -1)
		{
			idx = _MDS.arrFlight;
			if (idx != -1)
			{
				pTerm->flightLog->getItem(fltLogEntry, idx);
				nGateIdx = fltLogEntry.getArrGate();
				if(nGateIdx >= 0)
				{
					pTerm->procLog->getItem(proLogEntry, nGateIdx);
					proLogEntry.SetOutputTerminal(pTerm);
					proLogEntry.getID(buf);
					stmp.Format("\nGate: %s",buf);
					_sPaxTag = _sPaxTag+stmp;
				}
			}
		}
	}
	if(_ti & PAX_GATETIME) {
		int idx = -1;
		if(_MDS.depFlight != -1)
			idx = _MDS.depFlight;
		else if(_MDS.arrFlight != -1)
			idx = _MDS.arrFlight;
		if(idx != -1) {
			FlightLogEntry fltLogEntry;
			pTerm->flightLog->getItem( fltLogEntry, idx );
			fltLogEntry.getGateTime().printTime( buf, FALSE );
			stmp.Format("\nGate Time: %s",buf);
			_sPaxTag = _sPaxTag+stmp;
		}
	}
	if(_ti & PAX_BAGCOUNT) {
		stmp.Format("\nBag Count: %d",_MDS.bagCount);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_CARTCOUNT) {
		stmp.Format("\nCart Count: %d",_MDS.cartCount);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_STATE) {
		stmp.Format("\nState: %s",STATE_NAMES[_MESa.state]);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_PROCESSOR) 
	{
		Processor *pProc = pTerm->procList->getProcessor(_MESa.procNumber);
		if(pProc)
			pProc->getID()->printID(buf);
		
		stmp.Format("\nProcessor: %s",buf);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_WALKINGSPEED) {		
		stmp.Format("\nWalking Speed: %.1f",_MDS.speed);
		//stmp.Format("\nWalking Speed: %.1f",_MESa.speed);
		_sPaxTag = _sPaxTag+stmp;
	}
}



bool CAnimationManager::GetLandsideVehicleShow(const CLandsideEventLogBuffer::part_event_list& eventlist,
											   long nCurTime,
											   LandsideVehicleEventStruct& pesM, 
											   ARCVector3& dir,
											   int& pesAIdx)
{
	
	if(eventlist.empty())
		return false;

	long nStartTime = eventlist.front().time;
	long nEndTime = eventlist.back().time;

	if (nStartTime <= nCurTime && nEndTime >= nCurTime) 
	{
		long nextIdx = LinearSearchEvent(eventlist, nCurTime);			
		if(nextIdx<=0)
			return false;

		const LandsideVehicleEventStruct& pesB = eventlist[nextIdx];
		const LandsideVehicleEventStruct& pesA = eventlist[nextIdx-1];
		pesM = pesA;
		pesAIdx = nextIdx-1;
		//
		// calculate the current location
		float dtr = static_cast<float>(nCurTime-pesA.time)/static_cast<float>(pesB.time-pesA.time);
		float r = dtr;
		if(pesA.speed + pesB.speed > 0)
		{
			r = (pesB.speed-pesA.speed)*dtr*dtr + 2.0f*pesA.speed*dtr;
			r/=(pesB.speed+pesA.speed);
		}
		pesM.speed = pesB.speed*dtr + pesA.speed*(1-dtr);
	

		pesM.x = (1.0f - r) * pesA.x + r * pesB.x;
		pesM.y = (1.0f - r) * pesA.y + r * pesB.y;
		pesM.z = (1.0f - r) * pesA.z + r * pesB.z;
		pesM.level = (1.0f - r) * pesA.level + r * pesB.level;
		pesM.time = nCurTime;
		pesM.dist = (1.0f - r) * pesA.dist + r * pesB.dist;
		
		//calculate the smooth direction
		double dangleX = 0.0;
		long tmpIdx = nextIdx;
		
		double dMag;
		do
		{
			tmpIdx--;
			const LandsideVehicleEventStruct& pesTemp = eventlist[tmpIdx];
			dir[VX] = pesTemp.x - pesB.x;
			dir[VY] = pesTemp.y - pesB.y;
			dir[VZ] = pesTemp.z - pesB.z;
		} while((dMag=dir.Magnitude())<ARCMath::EPSILON && (nextIdx-tmpIdx)<50 && tmpIdx > 0);

		dir.Normalize();
		if(pesA.bpush)
		{
			dir = -dir;
		}	
		return true;
	}

	return false;
}


bool CAnimationManager::GetLandsideTrafficLightShow(const IntersectionLinkGroupLogEntry &element,long nCurTime,int &pesAIdx)
{
	long nStartTime = element.getStartTime();
	long nEndTime = element.getEndTime();
	if (nStartTime <= nCurTime && (nEndTime == 0 || nEndTime >= nCurTime)) 
	{
		long nextIdx = LinearSearchEvent(element.getBufferEvents(), nCurTime);			
		if(nextIdx<=0)
			return false;
		pesAIdx = nextIdx - 1;

		return true;
	}
	return false;

}

bool CAnimationManager::GetCrossWalkLightShow(const CCrossWalkLogEntry &element,long nCurTime,int &pesAIdx)
{
	long nStartTime = element.getStartTime();
	long nEndTime = element.getEndTime();
	if (nStartTime <= nCurTime && (nEndTime == 0 || nEndTime >= nCurTime)) 
	{
		long nextIdx = LinearSearchEvent(element.getBufferEvents(), nCurTime);			
		if(nextIdx<=0)
			return false;
		pesAIdx = nextIdx - 1;

		return true;
	}
	return false;
}
void CAnimationManager::BuildLandsideVehicleTag( const LandsideVehicleDescStruct& _fds, 
												const LandsideVehicleEventStruct& _fes, 
												const DWORD& dwTagInfo, bool bExpanded,
												const CString& sTypeName,
												CString& _sTag, 
												//int& nNumLines, 
												//int& nMaxCharsInLine,
												int nEventIdx )
{
	CString strTemp;
	strTemp.Empty();
	
	int nNumLines=0;
	int nMaxCharsInLine=0;
	
	//vehicle id
	if(0x0000000000000001 & dwTagInfo)
	{
		strTemp.Format("\nVehicle ID: %d",_fds.vehicleID);
		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}	
	//flight id
	if (0x0000000000000002 & dwTagInfo)
	{
		strTemp.Format("\nFlight ID: %d",_fds.fltid);
		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}
	//mobile type
	if (0x0000000000000004 & dwTagInfo)
	{
		strTemp.Format("\nMobile type: %s",sTypeName);
		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}
	//state
	if (0x0000000000000008 & dwTagInfo)
	{
		if(_fes.state < LANDSIDE_STATE_NUM && _fes.state>=0)
		{
			strTemp.Format("\nState: %s",LANDSIDEVEHICLE_STATE_NAMES[_fes.state]);
			_sTag += strTemp;
			nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
			nNumLines++;
		}
	}
	//position
	if (0x0000000000000010 & dwTagInfo)
	{
		ARCUnit_Length curLenUnit = CARCUnitManager::GetInstance().GetCurSelLengthUnit();
		double ux = CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,curLenUnit,_fes.x);
		double uy = CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,curLenUnit,_fes.y);
		double uz = CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,curLenUnit,_fes.z);

		double udist  = CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,curLenUnit,_fes.dist);

#ifdef _DEBUG
		strTemp.Format("\nPosition: (%.2f %.2f %.2f) %.2f",ux,uy,uz,udist);
#else
		strTemp.Format("\nPosition: (%.2f %.2f %.2f)",ux,uy,uz);
#endif

		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}
	//place
	if (0x0000000000000020 & dwTagInfo)
	{
// 		strTemp.Format("\nPlace: %.2f(m/s)",_fes.state);
// 		_sTag += strTemp;
// 		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
// 		nNumLines++;
	}
	//speed
	if (0x0000000000000040 & dwTagInfo)
	{
		ARCUnit_Velocity curVelUnit = CARCUnitManager::GetInstance().GetCurSelVelocityUnit();
		CString strV = 	CARCVelocityUnit::GetVelocityUnitString(curVelUnit);
		double uV = CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_MPS,curVelUnit,_fes.speed/100);
		strTemp.Format("\nSpeed(%s): %.2f",strV.GetString(),uV);
		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}
#ifdef _DEBUG
	if(true)
	{
		strTemp.Format("\nEvent Index: %d / %d ",nEventIdx, _fes.eventNum );
		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}
	if(true)
	{
		strTemp.Format("\nAhead Vehicle: %d",_fes.mode );
		_sTag += strTemp;
		nMaxCharsInLine = max(nMaxCharsInLine,strTemp.GetLength());
		nNumLines++;
	}
#endif
	
}

void CAnimationManager::PreviewMovie( CMovie* pMovie,CTermPlanDoc* m_pDoc, BOOL* m_bRequestCancel )
{
	//CMovie* pMovie = m_pDoc->m_movies.GetMovie(m_nSelectedMovie);
	int nKFCount = pMovie->GetKFCount();
	if(nKFCount>0) 
	{
		LARGE_INTEGER nPerfFreq, nThisPerfCount, nLastPerfCount;
		QueryPerformanceFrequency(&nPerfFreq);

		long nStartTime = pMovie->GetKFTime(0);
		long nEndTime = pMovie->GetKFTime(nKFCount-1);
		long nTime = nStartTime;

		QueryPerformanceCounter(&nLastPerfCount);
		C3DCamera newCam;
		int nLastAnimDirection = 0;

		while(nTime <= nEndTime) 
		{
			pMovie->Interpolate(nTime, newCam);
			//set anim direction
			IRender3DView* pView = NULL;
			long nLastAnimTime = m_pDoc->m_animData.nLastAnimTime;
			m_pDoc->m_animData.nLastAnimTime = nTime;
			if(m_pDoc->m_animData.m_AnimationModels.IsOnBoardSel() || m_pDoc->m_animData.m_AnimationModels.IsLandsideSel())
			{
				CRender3DView* pRender3DView = m_pDoc->GetRender3DView();
				if (pRender3DView)
				{
					pView = pRender3DView;					
					nLastAnimDirection = pRender3DView->m_nAnimDirection;
					pRender3DView->m_nAnimDirection = nTime>nLastAnimTime?CEventLogBufManager::ANIMATION_FORWARD:CEventLogBufManager::ANIMATION_BACKWARD;
					if(nLastAnimDirection!=pRender3DView->m_nAnimDirection)
						pRender3DView->m_bAnimDirectionChanged=TRUE;


					pRender3DView->SetUserCurrentCamera(newCam);
					pRender3DView->Invalidate(FALSE);
					pRender3DView->UpdateWindow();
				}
			}
			else
			{
				C3DView* p3DView = m_pDoc->Get3DView();
				if (p3DView)
				{
					pView = p3DView;					
					nLastAnimDirection = pView->m_nAnimDirection;
					pView->m_nAnimDirection = nTime>nLastAnimTime?CEventLogBufManager::ANIMATION_FORWARD:CEventLogBufManager::ANIMATION_BACKWARD;
					if(nLastAnimDirection!=pView->m_nAnimDirection)
						pView->m_bAnimDirectionChanged=TRUE;

					//redraw view			 
					*p3DView->GetCameraData()=newCam;
					p3DView->GetCamera()->Update();
					m_pDoc->UpdateViewSharpTextures();
					pView->Invalidate(FALSE);
					pView->UpdateWindow();
				}
			}

			IRenderFrame* pFrame = (IRenderFrame*)pView->GetParentFrame();
			pFrame->m_wndStatusBar.OnUpdateCmdUI(pFrame, FALSE);

			*m_bRequestCancel = PumpMessages();

			if(*m_bRequestCancel) {
				m_pDoc->StopAnimation();
				//ShowWindow(SW_SHOW);
				return;
			}

			//calc real elapsed time since last nTime increment
			QueryPerformanceCounter(&nThisPerfCount);
			double dElapsedTime = static_cast<double>(nThisPerfCount.QuadPart - nLastPerfCount.QuadPart) / nPerfFreq.QuadPart;
			nLastPerfCount = nThisPerfCount;

			nTime += ((long)(dElapsedTime*10.0*pMovie->GetAnimSpeed()));
		}
	}
	m_pDoc->StopAnimation();
}

BOOL CAnimationManager::PumpMessages() /*returns true if ESC is pressed */
{
	MSG msg;
	BOOL ret = FALSE;
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if(msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
			ret = TRUE;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return ret;
}
