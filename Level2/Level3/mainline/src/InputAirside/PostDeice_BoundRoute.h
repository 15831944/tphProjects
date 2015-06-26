#pragma once
#include "TaxiRoute.h"
#include "InputAirsideAPI.h"


class INPUTAIRSIDE_API CPostDeice_BoundRoute: public CIn_OutBoundRoute
{
public:
	CPostDeice_BoundRoute(void);
	~CPostDeice_BoundRoute(void);

	typedef pair<int, int> LogicRwyID;

public:
	BOUNDROUTETYPE GetRouteType()const{ return POSTDEICING; }

	//DBElement
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;

	//DBElement
	virtual void ReadOriginAndDestination();
	virtual void SaveOriginAndDestination();
	virtual void DeleteOriginAndDestination();

	void SetLogicRunwayIDList(const std::vector<LogicRwyID>& vRwyportIDList);
	void GetLogicRunwayIDList(std::vector<LogicRwyID>& vRwyportIDList)const;

	void ReadLogicRunwayList();
	void SaveLogicRunwayList();
	void DeleteLogicRunwayList();

	bool IsAvailableRoute(const ALTObjectID& padName, int nRwyID, int nMark);

private:
	std::vector<LogicRwyID> m_vLogicRunwayIDList;
};
