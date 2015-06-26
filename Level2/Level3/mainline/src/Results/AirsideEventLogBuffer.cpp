#include "StdAfx.h"
#include ".\airsideeventlogbuffer.h"
#include "../Results/AirsideSimLogs.h"
#include <fstream>
#include "../Common/BizierCurve.h"
#include "../Engine/Airside/CommonInSim.h"
#include "../Common/Path2008.h"
#include "../Common/DynamicMovement.h"

//flight  500 miles
const float fFlightDistance = 50000.0;
//vehicle  4 miles
const float fVehicleDistance = 400.0;
const double fPI = 3.1415926535;


//CAirsideEventLogBuffer::CAirsideEventLogBuffer(const CString& strLogPath)
//:m_strLogPath(strLogPath)
//{
//}
//
//CAirsideEventLogBuffer::~CAirsideEventLogBuffer(void)
//{
//}
//
//void CAirsideEventLogBuffer::ReadData()
//{
//
//}


//////////////////////////////////////////////////////////////////////////
//CAirsideEventLogBufferManager
CAirsideEventLogBufferManager::CAirsideEventLogBufferManager(CAirsideSimLogs *pAirsideLog)
:m_pAirsideLog(pAirsideLog)
{

}

CAirsideEventLogBufferManager::~CAirsideEventLogBufferManager()
{

}
void CAirsideEventLogBufferManager::SetFlightLogFilePath(const CString& strPath)
{
	m_strFlightLogPath = strPath;
}
void CAirsideEventLogBufferManager::SetVehicleLogFilepath(const CString& strPath)
{
	m_strVehicleLogPath =strPath;
}
void CAirsideEventLogBufferManager::LoadDataIfNecessary()
{
	m_flightLog.Clear();
	m_vehicleLog.Clear();
	LoadFlightLog();
	LoadVehicleLog();
}

void CAirsideEventLogBufferManager::LoadFlightLog()
{
	FileManager fileMan;

	if ( !fileMan.IsFile ( m_strFlightLogPath ) )
		return;


	// open log file
	std::ifstream logfile( m_strFlightLogPath.GetBuffer() , std::ios::in | std::ios::binary );
	if( logfile.bad() )
		return;

	// read data

	AirsideFlightLogEntry element;
	element.SetEventLog(&( m_pAirsideLog->m_airsideFlightEventLog));

	AirsideFlightLog& airsideFlightLog = m_pAirsideLog->m_airsideFlightLog;
	int nCount = airsideFlightLog.getCount();

	for (int i = 0; i < nCount; i++)
	{
		airsideFlightLog.getItem(element, i);

		long beginPos	= element.GetAirsideDescStruct().startPos;
		long endPos		= element.GetAirsideDescStruct().endPos + sizeof( AirsideFlightEventStruct ) ;
		long eventNum   = ( endPos-beginPos )/sizeof( AirsideFlightEventStruct );


		m_flightLog.m_lstElementEvent.push_back(std::make_pair(i, CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT(eventNum)));

		CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT&  data_in_vector = m_flightLog.m_lstElementEvent.back().second;
		// read all event to memory			
		if( logfile.fail() && !logfile.bad() )
		{
			logfile.clear();//all error bits are cleared
		}
		logfile.seekg( beginPos, std::ios::beg );

		logfile.read( (char*)&( data_in_vector[0] ), endPos - beginPos );
	}

	logfile.close();

	//smooth flight airroute
	SmoothFlightMotion(m_flightLog);
}

bool CAirsideEventLogBufferManager::IsPointOnAirRoute(AirsideFlightEventStruct& eventStruct)
{
	if ((eventStruct.mode < OnLanding && eventStruct.mode>OnBirth)
		||(eventStruct.mode == OnWaitInHold)
		|| ((eventStruct.mode < OnTerminate) && (OnClimbout < eventStruct.mode)))
	{
		return true;
	}

	return false;
}

void CreateSmoothPath(const CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT& vSmoothEvent,CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT& vResult)
{
	if(vSmoothEvent.size()<3)
		return;

	std::vector<CPoint2008> vInputs;
	for(size_t i=0;i<vSmoothEvent.size();i++)
	{
		const AirsideFlightEventStruct& evt = vSmoothEvent[i];
		vInputs.push_back( CPoint2008(evt.x, evt.y, evt.z) );
	}
	

	const AirsideFlightEventStruct& evtB = vSmoothEvent.front();
	const AirsideFlightEventStruct& evtE = vSmoothEvent.back();

	std::vector<CPoint2008> vOutPuts;
	BizierCurve::GenCurvePoints(vInputs,vOutPuts,20);
	CPath2008 outPath;
	outPath.init(vOutPuts.size(),&vOutPuts[0]);
	DynamicMovement dynMove(evtB.speed,evtE.speed,outPath.GetTotalLength());

	vResult.push_back(vSmoothEvent.front());
	for(size_t i=1;i<vOutPuts.size()-1;i++)
	{
		const CPoint2008& p = vOutPuts[i];
		double s = outPath.GetIndexDist((float)i);
		AirsideFlightEventStruct midEvt = vSmoothEvent[1];
		double trat = dynMove.getTrate(s);
		midEvt.speed = evtB.speed*(1-trat)+evtE.speed*trat;
		midEvt.x = float(p.x); midEvt.y=float(p.y);midEvt.z=float(p.z);
		midEvt.time = long( evtB.time*(1-trat)+evtE.time*trat);
		vResult.push_back(midEvt);
	}
	vResult.push_back(vSmoothEvent.back());

}


bool CAirsideEventLogBufferManager::GenerateSmoothFlightPath(const CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT& vInputEvents,CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT& vResult)const
{
	//const static DistanceUnit halfSmoothLen = 600000; //150m
	std::vector<AirsideFlightEventStruct> mapEvent;
	std::vector<CPoint2008> _inputPts;
	for (int i = 0; i < (int)vInputEvents.size(); i++)
	{
		const AirsideFlightEventStruct& event = vInputEvents[i];
		CPoint2008 pt;
		pt.init(event.x,event.y,event.z);
		if (_inputPts.empty())
		{
			_inputPts.push_back(pt);
			mapEvent.push_back(event);
			continue;
		}

		double dDist = pt.distance3D(_inputPts.back());
		if (dDist >= 3000)
		{
			_inputPts.push_back(pt);
			mapEvent.push_back(event);
		}
	}

	vResult.push_back(mapEvent.front());	
	for(int i=1;i<(int)mapEvent.size()-1;++i)
	{
		CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT vTempSmoothEvts;

		const AirsideFlightEventStruct& preEvt  = mapEvent[i-1];		
		const AirsideFlightEventStruct& thisEvt = mapEvent[i];
		const AirsideFlightEventStruct& nextEvt = mapEvent[i+1];
		CPoint2008 p1 = _inputPts[i-1];//(preEvt.x,preEvt.y,preEvt.z);
		CPoint2008 p2 = _inputPts[i];//(thisEvt.x,thisEvt.y,thisEvt.z);
		CPoint2008 p3 = _inputPts[i+1];//(nextEvt.x, nextEvt.y, nextEvt.z);

		
		DistanceUnit distPre = p2.distance3D(p1);	
		DistanceUnit distNext = p2.distance3D(p3);
		

		double smoothLen;
		if(  GetSmoothLength(thisEvt.speed, CPoint2008(p2-p1), CPoint2008(p3-p2), smoothLen ) )
		{
			//generate smooth path and push to result
			{//midevet1
				DistanceUnit dist1 = MAX(distPre/2.0, distPre - smoothLen);
				DynamicMovement dynMov(preEvt.speed,thisEvt.speed,distPre );
				double trat = dynMov.getTrate(dist1);
				double srat = dist1/distPre;
				//end the smooth
				CPoint2008 midP = p1*(1-srat)+p2*srat;
				double midSpd = preEvt.speed*(1-trat)+thisEvt.speed*trat;
				AirsideFlightEventStruct midEvt = preEvt;
				midEvt.time = preEvt.time + (long)((thisEvt.time-preEvt.time)*trat);
				midEvt.speed  = midSpd;
				midEvt.x = (float)(midP.x); midEvt.y=float(midP.y); midEvt.z = float(midP.z);
				vTempSmoothEvts.push_back(midEvt);			
			}
			vTempSmoothEvts.push_back(thisEvt);
			{//midevt2
				DistanceUnit dist2 = MIN(distNext/2.0, smoothLen);
				DynamicMovement dynMov(thisEvt.speed,nextEvt.speed,distNext );
				double trat = dynMov.getTrate(dist2);
				double srat = dist2/distNext;
				//end the smooth
				CPoint2008 midP = p2*(1-srat)+p3*srat;
				double midSpd = thisEvt.speed*(1-trat)+nextEvt.speed*trat;
				AirsideFlightEventStruct midEvt = thisEvt;
				midEvt.time = thisEvt.time + (long)((nextEvt.time-thisEvt.time)*trat);
				midEvt.speed  = midSpd;
				midEvt.x = (float)(midP.x); midEvt.y=float(midP.y); midEvt.z = float(midP.z);
				vTempSmoothEvts.push_back(midEvt);		
			}

			CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT vSubResult;
			CreateSmoothPath(vTempSmoothEvts,vSubResult);
			vResult.insert(vResult.end(),vSubResult.begin(),vSubResult.end());
		}
		else
		{
			vResult.push_back(thisEvt);
		}
		
		
	}
	vResult.push_back(mapEvent.back());


	return true;

}


//bool CAirsideEventLogBufferManager::GenerateSmoothFlightPath(const CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT& vSmoothEvent,CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT& vResult)const
//{
//	std::vector<CPoint2008> _inputPts;
//	//remove duplicate point and closely point
//	std::vector<std::pair<AirsideFlightEventStruct,CPoint2008> > mapEvent;
//	for (int i = 0; i < (int)vSmoothEvent.size(); i++)
//	{
//		const AirsideFlightEventStruct& event = vSmoothEvent[i];
//		CPoint2008 pt;
//		pt.init(event.x,event.y,event.z);
//		if (_inputPts.empty())
//		{
//			_inputPts.push_back(pt);
//			mapEvent.push_back(std::make_pair(event,pt));
//			continue;
//		}
//
//		double dDist = pt.distance3D(_inputPts.back());
//		if (dDist >= 3000)
//		{
//			_inputPts.push_back(pt);
//			mapEvent.push_back(std::make_pair(event,pt));
//		}
//	}
//
//	int nInputCount = (int)_inputPts.size();
//
//	CPath2008 inPath;
//	inPath.init(nInputCount,&_inputPts[0]);
//	CPath2008 reslt;
//
//	if(nInputCount<3)
//	{
//		vResult = vSmoothEvent;
//		return false;
//	}
//	const static int outPtsCnt = 10;
//
//
//	std::vector<CPoint2008> _out;
//	_out.reserve( (nInputCount-2)*outPtsCnt + 2);
//	_out.push_back(inPath[0]);
//
//	//first event
//	vResult.push_back(mapEvent.front().first);
//
//	CPoint2008 ctrPts[3];
//
//	long lStartTime = mapEvent.front().first.time;
//	long lEndTime = mapEvent.back().first.time;
//	long lCurrentTime = lStartTime;
//	long dT = lEndTime - lStartTime;
//	double dTotalDist = inPath.GetTotalLength();
//
//	for(int i=1;i<(int)nInputCount-1;i++)
//	{
//		ctrPts[0] = inPath[i-1];
//		ctrPts[1] = inPath[i];
//		ctrPts[2] = inPath[i+1];
//
//		//get the two side point
//		ARCVector3 v1 = ctrPts[0] - ctrPts[1];
//		ARCVector3 v2 = ctrPts[2] - ctrPts[1];
//		double minLen1,minLen2;
//		AirsideFlightEventStruct currentEvent = mapEvent.at(i).first;
//		CPoint2008 dir1 = CPoint2008(ctrPts[1] - ctrPts[0]);
//
//		double smoothLen = GetSmoothLength(currentEvent.speed,dir1,CPoint2008(v2) );
//		minLen1 = smoothLen;
//		minLen2	= minLen1;
//		if(v1.Length() * 0.45 <smoothLen) 
//			minLen1= v1.Length() * 0.45;
//		if(v2.Length() * 0.45 <smoothLen) 
//			minLen2 = v2.Length() * 0.45;
//		v1.SetLength(minLen1);
//		v2.SetLength(minLen2);
//		ctrPts[0] = ctrPts[1] + v1 ; 
//		ctrPts[2] = ctrPts[1] + v2;
//		std::vector<CPoint2008> ctrlPoints(ctrPts,ctrPts+3);
//		std::vector<CPoint2008> output;
//		BizierCurve::GenCurvePoints(ctrlPoints,output,outPtsCnt);
//
//		for(int j = 0;j<outPtsCnt;j++)
//		{
//			AirsideFlightEventStruct event = mapEvent.at(i).first;
//			event.x = (float)output[j].getX();
//			event.y = (float)output[j].getY();
//			event.z = (float)output[j].getZ();
//			vResult.push_back(event);
//			_out.push_back(output[j]);
//		}
//	}
//
//	_out.push_back( inPath[nInputCount-1] );
//	//last event
//	vResult.push_back(mapEvent.back().first);
//
//	//calculate event time
//	CPoint2008 prePt, pCurrentPt;
//	prePt = _out[0];
//	for (i = 0; i < (int)_out.size(); i++)
//	{
//		AirsideFlightEventStruct& event = vResult[i];
//		pCurrentPt = _out[i];
//		double dDist = pCurrentPt.distance3D(prePt);
//		lCurrentTime += (long)((dDist/dTotalDist)*dT);
//		event.time = lCurrentTime;
//		prePt = pCurrentPt;
//	}
//	return true;
//}

bool CAirsideEventLogBufferManager::GetSmoothLength(double dSpeed, const CPoint2008& dir1,const CPoint2008& dir2,double& dlen) const
{
	double dCosAngle = 0.0;
	DistanceUnit dDotValue = (dir1.getX()*dir2.getX() + dir1.getY()*dir2.getY()+dir1.getZ()*dir2.getZ());
	//dir1.crossProduct(dir2);
	if (dir1.length3D() != 0 && dir2.length3D() != 0)
	{
		dCosAngle = dDotValue / ( dir1.length3D() * dir2.length3D() );
	}
	if(dCosAngle>=1)
		return false;

	if(dCosAngle<-1)
		dCosAngle = -1;	
	double dDegree = ARCMath::RadiansToDegrees(acos(dCosAngle));

	//calculate the time 2.0 degree per second
	double time = dDegree / 2.0;
	
	double dDist = dSpeed * time / 2.0;

	dlen = dDist;
	return true;
}

void CAirsideEventLogBufferManager::SmoothFlightMotion(CAirsideEventLogBuffer<AirsideFlightEventStruct>& flightLog)
{
 	for (int i=0; i<(int)flightLog.m_lstElementEvent.size(); i++)
 	{
 		CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT&  data_in_vector = flightLog.m_lstElementEvent[i].second;
 		CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT smoothData = flightLog.m_lstElementEvent[i].second;
 		data_in_vector.clear();
 
 		CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT vSmoothEvent;
 		while(!smoothData.empty())
 		{
 			//init need generate smooth path events
 			AirsideFlightEventStruct& event = smoothData.front();
 			if (IsPointOnAirRoute(event) && smoothData.size() != 1)
 			{
 				vSmoothEvent.push_back(event);
 			}
 			else
 			{
 				if ((int)vSmoothEvent.size() > 2) // need generate smooth path
 				{
 					CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT vResult;
 					GenerateSmoothFlightPath(vSmoothEvent,vResult);
 					data_in_vector.insert(data_in_vector.end(),vResult.begin(),vResult.end());
 				}
 				else //flit the data
 				{
 					data_in_vector.insert(data_in_vector.end(),vSmoothEvent.begin(),vSmoothEvent.end());
 				}
				data_in_vector.push_back(event);
 				vSmoothEvent.clear();
 			}
 
 			smoothData.erase(smoothData.begin());
 		}
 	}
}

//calculate middlepoint which has fDistance form startPoit
void CAirsideEventLogBufferManager::CalcPointOnLine(AirsideFlightEventStruct& startPoint, AirsideFlightEventStruct& endPoint, Point& middlePoint, long& midPointTime, 
													float fDistance)
{
	double dStartX = startPoint.x;
	double dStartY = startPoint.y;
	double dStartZ = startPoint.z;

	double dEndX = endPoint.x;
	double dEndY = endPoint.y;
	double dEndZ = endPoint.z;

	double dMidX = 0.0;
	double dMidY = 0.0;
	double dMidZ = 0.0;

	double dStartEndX = dStartX - dEndX;
	double dStartEndY = dStartY - dEndY;
	double dStartEndZ = dStartZ - dEndZ;

	double dDisStartEnd = sqrt(dStartEndX*dStartEndX + dStartEndY*dStartEndY + dStartEndZ*dStartEndZ);

	double dRat = fDistance/dDisStartEnd;

	dMidX = dEndX*dRat + dStartX*(1-dRat);
	dMidY = dEndY*dRat + dStartY*(1-dRat);
	dMidZ = dEndZ*dRat + dStartZ*(1-dRat);

	middlePoint.setPoint((DistanceUnit)dMidX, (DistanceUnit)dMidY, (DistanceUnit)dMidZ);

	long lStartTime = startPoint.time;
	long lEndTime   = endPoint.time;

	midPointTime = (long)(lEndTime*dRat + lStartTime*(1-dRat));
}

void CAirsideEventLogBufferManager::LoadVehicleLog()
{
	FileManager fileMan;

	if ( !fileMan.IsFile ( m_strVehicleLogPath ) )
		return;


	// open log file
	std::ifstream logfile( m_strVehicleLogPath.GetBuffer() , std::ios::in | std::ios::binary );
	if( logfile.bad() )
		return;

	// read data

	AirsideVehicleLogEntry element;
	element.SetEventLog(&( m_pAirsideLog->m_airsideVehicleEventLog));

	AirsideVehicleLog& airsideVehicleLog =m_pAirsideLog->m_airsideVehicleLog;
	int nCount = airsideVehicleLog.getCount();


	for (int i = 0; i < nCount; i++)
	{
		airsideVehicleLog.getItem(element, i);

		long beginPos	= element.GetAirsideDesc().startPos;
		long endPos		= element.GetAirsideDesc().endPos + sizeof( AirsideVehicleEventStruct ) ;
		long eventNum   = ( endPos-beginPos )/sizeof( AirsideVehicleEventStruct );


		m_vehicleLog.m_lstElementEvent.push_back(std::make_pair(i, CAirsideEventLogBuffer<AirsideVehicleEventStruct>::VECTOREVENT(eventNum)));

		CAirsideEventLogBuffer<AirsideVehicleEventStruct>::VECTOREVENT&  data_in_vector = m_vehicleLog.m_lstElementEvent.back().second;
		// read all event to memory			
		if( logfile.fail() && !logfile.bad() )
		{
			logfile.clear();//all error bits are cleared
		}
		logfile.seekg( beginPos, std::ios::beg );

		logfile.read( (char*)&( data_in_vector[0] ), endPos - beginPos );

	}

	logfile.close();

	//smooth vehicle motion
	//SmoothVehicleMotion(m_vehicleLog);
}

void CAirsideEventLogBufferManager::SmoothVehicleMotion(CAirsideEventLogBuffer<AirsideVehicleEventStruct>& vehicleLog)
{
	//no vehicle
	if (vehicleLog.m_lstElementEvent.size() == 0)
	{
		return;
	}

	for (int i=0; i<(int)vehicleLog.m_lstElementEvent.size(); i++)
	{
		CAirsideEventLogBuffer<AirsideVehicleEventStruct>::VECTOREVENT&  data_in_vector = vehicleLog.m_lstElementEvent[i].second;

		CAirsideEventLogBuffer<AirsideVehicleEventStruct>::VECTOREVENT smoothData;

		if (data_in_vector.size() < (size_t)2)
		{
			continue;
		}

		AirsideVehicleEventStruct eventOne = data_in_vector[0];

		smoothData.push_back(data_in_vector[0]);
		data_in_vector.erase(data_in_vector.begin());

		while ((int)data_in_vector.size() >= 2)
		{
			AirsideVehicleEventStruct eventTwo = data_in_vector[0];
			AirsideVehicleEventStruct eventThree = data_in_vector[1];

			//point one
			float fOneX = eventOne.x;
			float fOneY = eventOne.y;
			float fOneZ = eventOne.z;

			//point two
			float fTwoX = eventTwo.x;
			float fTwoY = eventTwo.y;
			float fTwoZ = eventTwo.z;

			//vector oneTwo
			float fOneTwoX = fTwoX - fOneX;
			float fOneTwoY = fTwoY - fOneY;
			float fOneTwoZ = fTwoZ - fOneZ;

			//distance between one and Two point
			float fDistanceOneTwo = sqrt(fOneTwoX*fOneTwoX + fOneTwoY*fOneTwoY + fOneTwoZ*fOneTwoZ);

			//if distance is small than 4 miles
			if (fDistanceOneTwo < 400.0)
			{
				smoothData.push_back(data_in_vector[0]);
				eventOne = eventTwo;
				//remove the first item
				data_in_vector.erase(data_in_vector.begin());

				continue;
			}

			//point three
			float fThreeX = eventThree.x;
			float fThreeY = eventThree.y;
			float fThreeZ = eventThree.z;

			//vector twoThree
			float fTwoThreeX = fThreeX - fTwoX;
			float fTwoThreeY = fThreeY - fTwoY;
			float fTwoThreeZ = fThreeZ - fTwoZ;

			//distance between two and three point
			float fDistanceTwoThree = sqrt(fTwoThreeX*fTwoThreeX + fTwoThreeY*fTwoThreeY + fTwoThreeZ*fTwoThreeZ);

			Point pointOne;
			Point pointTwo;
			Point pointThree;
			long lStartTime = 0;
			bool bNeedAddPointStart = false;
			bool bNeedSkipEndPoint = false;
			long lEndTime;

			//if distance is small than 4 miles
			if (fDistanceTwoThree < 400.0)
			{
				smoothData.push_back(data_in_vector[0]);
				smoothData.push_back(data_in_vector[1]);

				//remove the first and second item
				data_in_vector.erase(data_in_vector.begin());
				data_in_vector.erase(data_in_vector.begin());

				if ((int)data_in_vector.size() < 1)
				{
					continue;
				}

				eventTwo = eventThree;
				eventThree = data_in_vector[0];

				//point two
				fTwoX = eventTwo.x;
				fTwoY = eventTwo.y;
				fTwoZ = eventTwo.z;

				//point three
				fThreeX = eventThree.x;
				fThreeY = eventThree.y;
				fThreeZ = eventThree.z;

				//vector twoThree
				fTwoThreeX = fThreeX - fTwoX;
				fTwoThreeY = fThreeY - fTwoY;
				fTwoThreeZ = fThreeZ - fTwoZ;

				//distance between two and three point
				fDistanceTwoThree = sqrt(fTwoThreeX*fTwoThreeX + fTwoThreeY*fTwoThreeY + fTwoThreeZ*fTwoThreeZ);

				while (fDistanceTwoThree < 400.0
					&& (int)data_in_vector.size() >= 2)
				{
					smoothData.push_back(data_in_vector[0]);

					//remove the first and second item
					data_in_vector.erase(data_in_vector.begin());

					eventTwo = eventThree;
					eventThree = data_in_vector[0];

					//point two
					fTwoX = eventTwo.x;
					fTwoY = eventTwo.y;
					fTwoZ = eventTwo.z;

					//point three
					fThreeX = eventThree.x;
					fThreeY = eventThree.y;
					fThreeZ = eventThree.z;

					//vector twoThree
					fTwoThreeX = fThreeX - fTwoX;
					fTwoThreeY = fThreeY - fTwoY;
					fTwoThreeZ = fThreeZ - fTwoZ;

					//distance between two and three point
					fDistanceTwoThree = sqrt(fTwoThreeX*fTwoThreeX + fTwoThreeY*fTwoThreeY + fTwoThreeZ*fTwoThreeZ);
				}

				if (fDistanceTwoThree - 400.0 < 0.0000000001)
				{
					continue;
				}

				//vector oneTwo
				fOneTwoX = fTwoX - fOneX;
				fOneTwoY = fTwoY - fOneY;
				fOneTwoZ = fTwoZ - fOneZ;

				//distance between one and Two point
				fDistanceOneTwo = sqrt(fOneTwoX*fOneTwoX + fOneTwoY*fOneTwoY + fOneTwoZ*fOneTwoZ);	

				//cos of the angle
				float fAngleValue = (fOneTwoX*fTwoThreeX + fOneTwoY*fTwoThreeY + fOneTwoZ*fTwoThreeZ) 
					/ (sqrt(fOneTwoX*fOneTwoX + fOneTwoY*fOneTwoY + fOneTwoZ*fOneTwoZ) * sqrt(fTwoThreeX*fTwoThreeX + fTwoThreeY*fTwoThreeY + fTwoThreeZ*fTwoThreeZ));

				//cos150
				double fConstValue = cos(30 * (2*fPI / 360));

				// if the angle range is 0<=x<=30       330<=x<=360
				if (fConstValue <= fAngleValue
					&& fAngleValue <= 1.001)
				{
					eventOne = eventTwo;

					continue;
				}

				//first point
				pointOne.setPoint((DistanceUnit)fTwoX, (DistanceUnit)fTwoY, (DistanceUnit)fTwoZ);
				lStartTime = eventTwo.time;
				bNeedAddPointStart = false;

				//calculate the second point
				long lTime = 0;
				CalcVehiclePointOnLine(eventTwo, eventOne, pointTwo, lTime, -fVehicleDistance);

				//third point
				pointThree.setPoint((DistanceUnit)fThreeX, (DistanceUnit)fThreeY, (DistanceUnit)fThreeZ);
				lEndTime = eventThree.time;
				bNeedSkipEndPoint = true;

			}
			else
			{
				//cos of the angle
				float fAngleValue = (fOneTwoX*fTwoThreeX + fOneTwoY*fTwoThreeY + fOneTwoZ*fTwoThreeZ) 
					/ (sqrt(fOneTwoX*fOneTwoX + fOneTwoY*fOneTwoY + fOneTwoZ*fOneTwoZ) * sqrt(fTwoThreeX*fTwoThreeX + fTwoThreeY*fTwoThreeY + fTwoThreeZ*fTwoThreeZ));

				//cos150
				double fConstValue = cos(30 * (2*fPI / 360));

				// if the angle range is 0<=x<=30       330<=x<=360
				if (fConstValue <= fAngleValue
					&& fAngleValue <= 1.001)
				{
					smoothData.push_back(data_in_vector[0]);
					eventOne = data_in_vector[0];
					data_in_vector.erase(data_in_vector.begin());

					continue;
				}

				data_in_vector.erase(data_in_vector.begin());
				
				//calculate the first point
				if (fDistanceOneTwo < fVehicleDistance)
				{
					pointOne.setPoint((DistanceUnit)fOneX, (DistanceUnit)fOneY, (DistanceUnit)fOneZ);
					lStartTime = eventOne.time;

					bNeedAddPointStart = false;
				}
				else
				{
					CalcVehiclePointOnLine(eventTwo, eventOne, pointOne, lStartTime, fVehicleDistance);

					bNeedAddPointStart = true;
				}

				//second point
				pointTwo.setPoint((DistanceUnit)fTwoX, (DistanceUnit)fTwoY, (DistanceUnit)fTwoZ);

				
				//calculate the second point
				if (fDistanceTwoThree < fVehicleDistance)
				{
					pointThree.setPoint((DistanceUnit)fThreeX, (DistanceUnit)fThreeY, (DistanceUnit)fThreeZ);
					lEndTime = eventThree.time;

					bNeedSkipEndPoint = true;
				}
				else
				{
					CalcVehiclePointOnLine(eventTwo, eventThree, pointThree, lEndTime, fVehicleDistance);

					bNeedSkipEndPoint = false;
				}
			}			

			std::vector<Point> _inputPts;
			std::vector<Point> _outputPts;
			int nOutCnt = 10;
			_inputPts.push_back(pointOne);
			_inputPts.push_back(pointTwo);
			_inputPts.push_back(pointThree);

			BizierCurve::GenCurvePoints(_inputPts, _outputPts,nOutCnt);

			AirsideVehicleEventStruct smoothVehicleEvent = eventThree;
			//add first item
			if (bNeedAddPointStart)
			{
				smoothVehicleEvent.x = (float)_outputPts[0].getX();
				smoothVehicleEvent.y = (float)_outputPts[0].getY();
				smoothVehicleEvent.z = (float)_outputPts[0].getZ();
				smoothVehicleEvent.time = lStartTime;

				smoothData.push_back(smoothVehicleEvent);
			}

			long dT = lEndTime - lStartTime;
			//add middle items
			for (int i=1; i<(int)_outputPts.size()-1; i++)
			{
				smoothVehicleEvent.x = (float)_outputPts[i].getX();
				smoothVehicleEvent.y = (float)_outputPts[i].getY();
				smoothVehicleEvent.z = (float)_outputPts[i].getZ();
				smoothVehicleEvent.time = lStartTime + dT*i/(_outputPts.size()-1);

				smoothData.push_back(smoothVehicleEvent);
			}

			//add last item
			if (bNeedSkipEndPoint)
			{
				smoothVehicleEvent = eventThree;		
				smoothData.push_back(smoothVehicleEvent);

				eventOne = eventThree;

				data_in_vector.erase(data_in_vector.begin());
			}
			else
			{
				smoothVehicleEvent.x = (float)_outputPts[_outputPts.size()-1].getX();
				smoothVehicleEvent.y = (float)_outputPts[_outputPts.size()-1].getY();
				smoothVehicleEvent.z = (float)_outputPts[_outputPts.size()-1].getZ();
				smoothVehicleEvent.time = lEndTime;

				smoothData.push_back(smoothVehicleEvent);

				eventOne = smoothVehicleEvent;
			}
		}

		if (data_in_vector.size() > (size_t)0)
		{
			smoothData.push_back(data_in_vector[0]);
			data_in_vector.clear();
		}		

		CAirsideEventLogBuffer<AirsideVehicleEventStruct>::VECTOREVENT::iterator iter = smoothData.begin();
		while (iter != smoothData.end())
		{
			AirsideVehicleEventStruct eventStruct = *iter;
			data_in_vector.push_back(eventStruct);
			iter++;
		}
	}
}

void CAirsideEventLogBufferManager::CalcVehiclePointOnLine(AirsideVehicleEventStruct& startPoint, AirsideVehicleEventStruct& endPoint, 
														   Point& middlePoint, long& midPointTime, float fDistance)
{
	double dStartX = startPoint.x;
	double dStartY = startPoint.y;
	double dStartZ = startPoint.z;

	double dEndX = endPoint.x;
	double dEndY = endPoint.y;
	double dEndZ = endPoint.z;

	double dMidX = 0.0;
	double dMidY = 0.0;
	double dMidZ = 0.0;

	double dStartEndX = dStartX - dEndX;
	double dStartEndY = dStartY - dEndY;
	double dStartEndZ = dStartZ - dEndZ;

	double dDisStartEnd = sqrt(dStartEndX*dStartEndX + dStartEndY*dStartEndY + dStartEndZ*dStartEndZ);

	double dRat = fDistance/dDisStartEnd;

	dMidX = dEndX*dRat + dStartX*(1-dRat);
	dMidY = dEndY*dRat + dStartY*(1-dRat);
	dMidZ = dEndZ*dRat + dStartZ*(1-dRat);

	middlePoint.setPoint((DistanceUnit)dMidX, (DistanceUnit)dMidY, (DistanceUnit)dMidZ);

	long lStartTime = startPoint.time;
	long lEndTime   = endPoint.time;

	midPointTime = (long)(lEndTime*dRat + lStartTime*(1-dRat));
}
