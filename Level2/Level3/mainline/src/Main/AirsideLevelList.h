#pragma once
#include "AirsideGround.h"
class CAirsideLevelList
{
public:
	struct LevelInfo 
	{
		LevelInfo(){ nLevelID = 0; bMainLevel = 0;}
		int nLevelID;
		int bMainLevel;
		CString strName;

	};
public:
	CAirsideLevelList(int nAirportID);
	CAirsideLevelList();
	~CAirsideLevelList(void);

private:
	CAirsideLevelList(const CAirsideLevelList& otherList){}

public:
	void ReadLevelList();
	void SaveLevelList();


	void ReadFullLevelList();



	void AddLevelBeforeARP();
	void AddLevelAfterARP();
	
	LevelInfo GetItem(int nIndex);
	int GetCount(){	return (int)m_vAirsideLevel.size(); }
	CAirsideGround& GetLevel(int idx){ return *m_vFullLevel[idx]; }

	CAirsideGround * GetLevelByID(int id);

protected:
	std::vector<LevelInfo >  m_vAirsideLevel;
	std::vector<CAirsideGround*> m_vFullLevel;
	int m_nAirportID;

};
