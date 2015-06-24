#pragma once
#include "..\..\Common\elaptime.h"
#include "..\CalloutDispSpec.h"
#include "CalloutDlgLineData.h"
class CTermPlanDoc ;
class CCallOutBaseData ;
class CCallOutDataManager
{
public:
	CCallOutDataManager(CalloutDispSpecs* _CallOutSpecs,CTermPlanDoc* _Doc);
	~CCallOutDataManager(void);
protected:
	std::vector<CCallOutBaseData*> m_Data ;
	CalloutDispSpecs* m_CallOutSpecs ;
	int m_ProID ;
	CTermPlanDoc* m_TermPlanDoc ;
	CCalloutDlgLineData m_CallOutLineData ;
public:
	void GenerateCallOutData() ;
	void DestoryTheCallOutData(CCallOutBaseData* _Data) ;
	void NoticeTheTimeHasUpdated(const ElapsedTime& _currentTime) ;

	void ShowAllTheCallOutDialog() ;
	void ShutDownAllTheCallOutDialog() ;

protected:
	CCallOutBaseData* GenerateTerminalCallOutData(CalloutDispSpecDataItem* _PSpaceItem);
	CCallOutBaseData* GenerateTerminalCallOutProcessorData(CalloutDispSpecDataItem* _PSpaceItem);


	CCallOutBaseData* GenerateAirsideCallOutData(CalloutDispSpecDataItem* _PSpaceItem);
	CCallOutBaseData* GenerateLandSideCallOutData(CalloutDispSpecDataItem* _PSpaceItem);
	CCallOutBaseData* GenerateOnBoardCallOutData(CalloutDispSpecDataItem* _PSpaceItem);


	void ClearCallOutData() ;
public:
	void SetAnimationBeginTime(const ElapsedTime& _time);
	void SetCallOutPositon();
	CCalloutDlgLineData* GetDrawLineData() { return &m_CallOutLineData; }
};
