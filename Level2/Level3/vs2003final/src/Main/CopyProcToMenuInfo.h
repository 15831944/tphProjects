#pragma once

#include <vector>

class CTermPlanDoc;
class CFloor2;

class CCopyProcToMenuInfo
{
public:
	CCopyProcToMenuInfo(void);
	virtual ~CCopyProcToMenuInfo(void);

	int GetAppDocuments(std::vector<CTermPlanDoc*>& _vDocs);
	int	GetFloorsByDocument(const CTermPlanDoc* _pDoc, std::vector<CFloor2*>& _vFloors);
	BOOL GetRegularInfoByMenuID(int _nMenuID, CTermPlanDoc** _ppDoc, int& _nFloorID);

protected:
	std::vector<CTermPlanDoc*>	m_vDocs;
};
