#pragma once
#include "../Database/ADODatabase.h"
#include "FlightTypeRelativeElement.h"
#include <vector>
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class INPUTAIRSIDE_API FlightTypeServiceLoactions
{
public:
	typedef std::vector<FlightTypeRelativeElement> ElementList;
	typedef ElementList::iterator ElementIter;

public:
	FlightTypeServiceLoactions(void);
	~FlightTypeServiceLoactions(void);

public: 

	BOOL  GetRelativeElements(ElementList& vect, int nTypeID);

	void ReadData(int nFltTypeDetailsItemID);
	void SaveData(int nFltTypeDetailsItemID);
	void UpdateData(int nFltTypeDetailsItemID);
	void DeleteData(int nFltTypeDetailsItemID);

	void InitDefault(int nProjID, int nID);


	void ConvertPointIntoLocalCoordinate(const CPoint2008& pt,const double& dDegree);

	ElementList& GetElementList(){	return m_listElement; }

	void ResetAllElements();

	const ElementList& GetEventList(){ return m_listElement;}



	int GetElementCount(int nTypeID);

	void AddItem(FlightTypeRelativeElement item);
	void DeleteItem(CPoint2008& point);
	void UpdateItem(CPoint2008& oldPoint, CPoint2008& newPoint);

	void   ExportData(CAirsideExportFile& exportFile, int nFltTypeDetailsItemID);
	void   ImportData(CAirsideImportFile& importFile, int nFltTypeDetailsItemID);


protected:
	int				m_nID;
	ElementList		m_listElement;
	ElementList     m_listNeedDelete;
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               