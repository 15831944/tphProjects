#pragma once
#include "afxcmn.h"
#include "InputAirside/Runway.h"
#include "InputAirside/Taxiway.h"
#include "InputAirside/Structure.h"
#include ".\Common\latlong.h"
#include ".\InputAirside\ALTAirport.h"
#include "PrintableListCtrl.h"
#include ".\Common\dataset.h"
#include <Controls\XTResizeDialog.h>


//////////////////for export
class ObstructionReport : public DataSet
{
public:
	ObstructionReport():DataSet(ObstructionReportFile,(float)2.2)
		,m_vDataString(NULL)
	{

	}
	virtual const char *getTitle (void) const { return "Obstruction Report"; }
	virtual const char *getHeaders (void) const{  return "Name,Surface generator,Protrusion point,Elevation,Protrusion,"; }
	virtual void writeData (ArctermFile& p_file) const;
	virtual void readData (ArctermFile& p_file) {}

	void AddItem(CString& strData) {m_vDataString.push_back(strData);}
	void clear(){m_vDataString.clear();}
private:
	std::vector<CString>m_vDataString;
};

// DlgObstructionReport dialog
class DlgObstructionReport : public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgObstructionReport)

public:
	DlgObstructionReport(CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgObstructionReport();

// Dialog Data
	enum { IDD = IDD_OBSTRUCTIONREPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CPrintableListCtrl m_wndListCtrl;

private:
	void InitListCtrl();
	void LoadListCtrlData();
	void ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude );
protected:
	ALTAirport m_AirportInfo;
	ObstructionReport m_ObStructionReport;
public:
	afx_msg void OnBnClickedPrint();
	afx_msg void OnBnClickedButtonExport();
};

class ObstructionInterface
{
public:
	static bool LoadRunwayData(Structure& structObj,Runway& runObj,std::vector<DistanceUnit>& vObstructionHeights,RUNWAY_MARK emRunwayMark);
	static bool LoadTaxiwayData(Structure& structObj,Taxiway& taxiObj,std::vector<DistanceUnit>& vObstructionHeights);
private:
	static bool isObstructionsValid(const std::vector<DistanceUnit>& vObstructionHeights,Structure& structObj);
	static ARCPoint3 GetLocation( const Structure& structObj );
};