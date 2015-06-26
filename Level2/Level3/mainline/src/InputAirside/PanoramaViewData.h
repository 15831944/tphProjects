#pragma once
#include "InputAirsideAPI.h"
#include "../Common/Point2008.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"


/************************************************************************/
// panorama view of airside
/************************************************************************/
class INPUTAIRSIDE_API PanoramaViewData : public DBElement
{
public:

	CPoint2008 m_pos;          //
	double m_dTurnSpeed;	   //degree/sec
	double m_dStartBearing;	   //degree
	double m_dEndBearing;      //  degree
	double m_dInclination;     // degree 0 - (-90)
	double m_dAltitude;        // 


	long m_startTime;
	long m_endTime;

	CString m_strName;

	void SetPos(const CPoint2008& pt);
	void SetTurnSpeed(const double& dTurnSpd);
	void SetStartBearing(const double& dSbearing);
	void SetEndBearing(const double& endBearing);
	void SetInclination(const double& dInc);
	void SetAltitude(const double& dAlt);
	void SetName(const CString& strName);

	void SetTimeRange(long startTime, long endTime);
	
	long GetStartTime()const{ return  m_startTime; }
	long GetEndTime()const{ return m_endTime; }

	double GetMaxInclination() const;
	double GetMinInclination() const;
public:
	void SaveData(int nAirportID);
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	void ExportData(CAirsideExportFile& exportFile){}
	void ImportData(CAirsideImportFile& importFile){}


public:
	PanoramaViewData(void);	

public:
	const static double DefaultTurnSpeed;
	const static double DefaultStartBearing;
	const static double DefaultEndBearing;
	const static double DefaultInclination;
	const static double MaxInclination;
	const static double MinInclination;
	const static double DefaultAltitude;	
};



/************************************************************************/
//	List of 
/************************************************************************/
class INPUTAIRSIDE_API PanoramaViewDataList : public DBElementCollection<PanoramaViewData>
{
public:
	PanoramaViewDataList();	
	~PanoramaViewDataList();

	CString GetCreateName()const;

	PanoramaViewData* CreateNewData();	
	
	void DeleteItemFromIndex(size_t nIndex);

	PanoramaViewData* GetSelectItem()const;
	BOOL SelectItem(int idx);
	BOOL SelectItem(PanoramaViewData* theData);
	int GetSelectItemIndex()const{  return m_nSelectItem; }

	void GetSelectElementSQL(int nParentID,CString& strSQL)const;
protected:	

	//std::vector<PanoramaViewData*> m_vDatalist;
	int m_nSelectItem;    // <size()
private:
	PanoramaViewDataList& operator =(const PanoramaViewDataList& otherList);  //uncopy able

};