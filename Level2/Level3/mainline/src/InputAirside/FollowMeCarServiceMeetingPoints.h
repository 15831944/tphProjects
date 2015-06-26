#pragma once
#include "../Common/ALTObjectID.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API FollowMeCarServiceMeetingPoints
{
public:
	FollowMeCarServiceMeetingPoints(void);
	~FollowMeCarServiceMeetingPoints(void);

	int GetServiceMeetingPointCount();
	ALTObjectID GetServiceMeetingPoint(int idx);

	void ReadData(int nPoolsID);
	void SaveData(int nPoolsID);
	void UpdateData(int nParentID ,const CString& strMeetingPoints);
	void DeleteData();

	bool AddMeetingPoint(const ALTObjectID& objName);
	void DelMeetingPoint(const ALTObjectID& objName);


private:
	std::vector<ALTObjectID> m_vMeetingPoints;
	int m_nID;
};
