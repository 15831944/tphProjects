#pragma once
#include <results/EventLogBuffer.h>
#include <Results/AirsideEventLogBuffer.h>
#include <CommonData/3DTextManager.h>
#include <Renderer/RenderEngine/AnimaPax3D.h>
#include <common/ARCVector.h>
#include "Common/LANDSIDE_BIN.h"
#include "Landside/IntersectionLinkGroupLogEntry.h"
#include "Results/LandsideEventLogBuffer.h"
class CTermPlanDoc;
class CAircraftTagItem;
class BridgeConnector;


struct AnimationPaxGeometryData
{
	ARCVector3 pos; // [out]
	double dAngleFromX; // [out]
	bool   bNeedRotate; // [out]

	double bestSpotAngle; // [out]
	bool   bBestSpotAngle; // [in], Interpolate dAngleFromX when true

	AnimationPaxGeometryData()
		: dAngleFromX(0.0)
		, bNeedRotate(false)
		, bestSpotAngle(0.0)
		, bBestSpotAngle(false)
	{

	}
};
class LandsideVehicleLogEntry;
class CCrossWalkLogEntry;
class CMovie;

class CAnimationManager
{
public:
	//return true if need to show the pax
	static bool GetPaxShow(CTermPlanDoc* pDoc, const part_event_list& part_list, long nTime, BOOL* bOn, double* dAlt
							, AnimationPaxGeometryData& geoData, PaxAnimationData& animaPaxData/*[in/out]*/, int& pesAIdx);

	static bool GetFlightShow(const CAirsideEventLogBuffer<AirsideFlightEventStruct>::ELEMENTEVENT& ItemEventList,long nCurTime,const DistanceUnit& airportAlt
							,ARCVector3& pos, ARCVector3& danlges, double& dSpeed,int& pesAIdx);

	static bool GetVehicleShow(CTermPlanDoc* pDoc,const CAirsideEventLogBuffer<AirsideVehicleEventStruct>::ELEMENTEVENT& ItemEventList,long nCurTime,const DistanceUnit& airportAlt
		,ARCVector3& pos, ARCVector3& danlges, double& dSpeed,int& pesAIdx);


	//terminal processor whether display state on tags
	static bool DisplayProcessorStateOnTags(CTermPlanDoc* pDoc,Processor* pProc, long nTime);
	//get vehicle display data
	static bool GetLandsideVehicleShow(const CLandsideEventLogBuffer::part_event_list& eventlist, 
											long nCurTime, 
											LandsideVehicleEventStruct& pesM,
											ARCVector3& danlges,
											int& pesAIdx);

	//get trafficLight display data
	static bool GetLandsideTrafficLightShow(const IntersectionLinkGroupLogEntry &itemLog,long nCurTime,int &pesAIdx);
	
	static bool GetCrossWalkLightShow(const CCrossWalkLogEntry &itemLog,long nCurTime,int &pesAIdx);

	static bool GetEscaltorShow();


	static void BuildPaxTag(CTermPlanDoc* pDoc, DWORD _ti, const MobDescStruct& _MDS, const MobEventStruct& _MESa, CString& _sPaxTag );


	static void BuildFlightTag(	const AirsideFlightDescStruct& _fds,
		const AirsideFlightEventStruct& _fes, 
		const CAircraftTagItem& _ati,
		bool bExpanded,
		CString& _sTag,
		std::vector<C3DTextManager::TextFormat>& formats,
		int& nNumLines, 
		int& nMaxCharsInLine,							 
		double fSpeed = 0.0,
		double dAlt = 0.0);


	static void BuildVehicleTag(const AirsideVehicleDescStruct& _fds,
		const AirsideVehicleEventStruct& _fes, 
		const DWORD& dwTagInfo,
		bool bExpanded,
		CString& _sTag,
		int& nNumLines, 
		int& nMaxCharsInLine,
		float fSpeed = 0.00);

	static void BuildLandsideVehicleTag(const LandsideVehicleDescStruct& _fds,
		const LandsideVehicleEventStruct& _fes,		
		const DWORD& dwTagInfo,
		bool bExpanded,
		const CString& sTypeName,
		CString& _sTag,
		/*int& nNumLines, 
		int& nMaxCharsInLine,*/
		int eventIndex);
	
	
	//return true if it is moving
	static bool GetBridgeShow( BridgeConnector* pConnector,int idx, CTermPlanDoc* pDoc,double dAlt, CPoint2008& ptFrom, CPoint2008& ptTO );

//
	static bool GetTrainShow()
	{

	}


	static void PreviewMovie(CMovie* pMovie,CTermPlanDoc* m_pDoc, BOOL* m_bRequestCancel );


	static BOOL PumpMessages();//returns true if ESC is pressed;
};
