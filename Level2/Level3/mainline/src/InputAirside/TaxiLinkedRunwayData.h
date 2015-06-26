#pragma once
#include "../Common/elaptime.h"
#include "InputAirsideAPI.h"

class CADORecordset;
class INPUTAIRSIDE_API CTaxiLinkedRunwayData
{
	friend class CDlgTaxiInterruptTimes;
public:
	CTaxiLinkedRunwayData(void);
	~CTaxiLinkedRunwayData(void);

	void SaveData(int nParentID);
	void ReadData(CADORecordset& recordset);
	void UpdateData();
	void DeleteData();

	int GetLinkRwyID();
	int GetLinkRwyMark();
	double GetLandingHoldDist();
	double GetTakeoffHoldDist();

	ElapsedTime GetLandingHoldTime();
	ElapsedTime GetTakeoffHoldTime();


private: 
	int m_nID;
	int m_nRwyID;
	int m_nRwyMark;
	double m_dLandingHoldDist;
	ElapsedTime m_tLandingHoldTime;
	double m_dTakeoffHoldDist;
	ElapsedTime m_tTakeoffHoldTime;


};
