#pragma once

class Terminal;
#include <CommonData/procid.h>
class CCallOutBaseData ;
class CCallOutLines
{
public:
	CRect m_rect ; // the dialog position , it will been changed by UpdateTheCurrentDlgPosition of class CCalloutDlgLineDataItem
	std::vector<Point> m_processorPoint ; // the server point of processors
};
class CCalloutDlgLineDataItem
{
protected:
	 CCallOutBaseData* m_CalloutBaseData ; // the call out data,
	CCallOutLines m_Lines ;
	Terminal* m_Terminal ;
	const ProcessorID& m_proID ;

public:
	CCalloutDlgLineDataItem(CCallOutBaseData* _dlgdata,Terminal* _terminal, const ProcessorID& _proID ) ;
	void InitTheToPoints() ; // int the processor server points, if processors is family
	CCallOutLines* GetDrawLinesData() { return &m_Lines ;} ;
	void UpdateTheCurrentDlgPosition(); // update the position at the current time, it should call before drawing lines.
	const CCallOutBaseData* GetCallOutBaseData() { return m_CalloutBaseData ;} ;
	BOOL NeedDrawLine()  ;
};

class CCalloutDlgLineData
{
public:
	CCalloutDlgLineData() ;
	~CCalloutDlgLineData(void);
	int GetCount()const{ return (int)m_Data.size(); }
	CCalloutDlgLineDataItem* GetItem(int idx){ return m_Data.at(idx); }
protected:
	std::vector<CCalloutDlgLineDataItem*> m_Data ;
public:
	void RegisterTheDlg(CCallOutBaseData* _data,const ProcessorID& _proID,Terminal* _terminal) ;//register the call out
	CCalloutDlgLineDataItem* GetTheDlgLineData(CCallOutBaseData* _data) ;//get the lines data
	void ClearAllTheData() ;
protected:
	CCalloutDlgLineDataItem* FindTheDlgLineDataItem(CCallOutBaseData* _data) ;
};
