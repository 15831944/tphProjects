// DlgObstructionReport.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgObstructionReport.h"
#include ".\dlgobstructionreport.h"
#include ".\Common\point.h"
#include ".\Common\DistanceLineLine.h"
#include "MainFrm.h"
#include "OffsetMap.h"
#include ".\Common\LatLongConvert.h"
#include <InputAirside\ARCUnitManager.h>

#include <Common/ARCUnit.h>
#include <InputAirside/ARCUnitManager.h>

// DlgObstructionReport dialog
IMPLEMENT_DYNAMIC(DlgObstructionReport, CXTResizeDialog)
DlgObstructionReport::DlgObstructionReport(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgObstructionReport::IDD, pParent)
{
}

DlgObstructionReport::~DlgObstructionReport()
{
}

void DlgObstructionReport::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBSTRUCTION_REPORT, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(DlgObstructionReport, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnBnClickedPrint)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnBnClickedButtonExport)
END_MESSAGE_MAP()


// DlgObstructionReport message handlers

BOOL DlgObstructionReport::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_OBSTRUCTION_REPORT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_PRINT,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_EXPORT,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	InitListCtrl();
	LoadListCtrlData();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void DlgObstructionReport::InitListCtrl()
{
	CMDIChildWnd* pMCW=(CMDIChildWnd*)((CMainFrame*)GetParent())->GetActiveFrame();
	CView* pView=pMCW->GetActiveView();
	ASSERT(pView);
	ASSERT(pView->GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	CTermPlanDoc* pDoc=(CTermPlanDoc*)pView->GetDocument();

	LPCSTR strTitle[] = { _T("Name"),_T("Surface generator"),_T("Protrusion point"),_T("Elevation "),_T("Protrusion ")};
	CString strUnit = CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit());
	CString strValue = _T("");
	strValue.Format(_T("%s(%s)"),strTitle[3],strUnit);
	strTitle[3] = strValue;

	CString strData = _T("");
	strData.Format(_T("%s(%s)"),strTitle[4],strUnit);
	strTitle[4] = strData;
	int DefaultColumnWidth[] = { 85, 100, 220,120,120};
	for (int i = 0; i < 5; i++)
	{
		m_wndListCtrl.InsertColumn(i,strTitle[i],LVCFMT_LEFT,DefaultColumnWidth[i]);
	}
}

void DlgObstructionReport::LoadListCtrlData()
{
	CMDIChildWnd* pMCW=(CMDIChildWnd*)((CMainFrame*)GetParent())->GetActiveFrame();
	CView* pView=pMCW->GetActiveView();
	ASSERT(pView);
	ASSERT(pView->GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	CTermPlanDoc* pDoc=(CTermPlanDoc*)pView->GetDocument();
	
	int nProjID = -1;
	nProjID = pDoc->GetInputAirside().GetProjectID(pDoc->m_ProjInfo.name);
	std::vector<ALTAirport> vAirport;
	ALTObject altObject;
	ALTAirport::GetAirportList(nProjID,vAirport);
	if (vAirport.size())
	{
		m_AirportInfo.ReadAirport(vAirport.at(0).getID());
	}
	for (int i = 0; i < (int)vAirport.size(); i++)
	{
	 	int nAirportID = vAirport.at(i).getID();
	 	std::vector<ALTObject> vObject1;
	 	ALTObject::GetObjectList(ALT_RUNWAY,nAirportID,vObject1);
	
	 	std::vector<ALTObject> vObject2;
	 	ALTObject::GetObjectList(ALT_TAXIWAY,nAirportID,vObject2);

		std::vector<ALTObject> vObject3;
		ALTObject::GetObjectList(ALT_STRUCTURE,nAirportID,vObject3);
		for (int j = 0; j < (int)vObject3.size(); j++)
		{
			altObject = vObject3.at(j);
			Structure structObj;
			structObj.ReadObject(altObject.getID());
			for(int k = 0; k < (int)vObject1.size(); k++)
			{
				std::vector<DistanceUnit> vObstructionHeights;
				Runway runObj;
				altObject = vObject1.at(k);
				runObj.ReadObject(altObject.getID());
				if (ObstructionInterface::LoadRunwayData(structObj,runObj,vObstructionHeights,RUNWAYMARK_FIRST))
				{
					for (int m = 0; m < structObj.GetPath().getCount(); m++)
					{
						if(structObj.getUserData(m)->dStructHeight - vObstructionHeights[m] > 0)
						{
							CLatitude fixlatitude;
							CLongitude fixlongitude;
							CPoint2008 point = structObj.GetPath().getPoint(m);
							ConvertPosToLatLong(point,fixlatitude, fixlongitude);
							CString strLL;
							CString strLat,strLong;
							fixlatitude.GetValue(strLat);
							fixlongitude.GetValue(strLong);

							strLL.Format("Latitude = %s ; Longitude = %s",strLat,strLong);

							m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(),structObj.GetObjNameString());
							CString strName = _T("");
							strName.Format(_T("Runway %sL"),runObj.GetMarking1().c_str());
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,1,strName);
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,2,strLL);
							CString strValue = _T("");
							strValue.Format(_T("%.f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,\
								pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vObstructionHeights[m]));
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,3,strValue);

							CString strData = _T("");
							strData.Format(_T("%.f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,\
								pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),structObj.getUserData(m)->dStructHeight - vObstructionHeights[m]));
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,4,strData);
						
							CString strObstructionRpt = _T("");
							strObstructionRpt.Format(_T("%s,%s,%s,%s,%s"),structObj.GetObjNameString(),strName,strLL,strValue,strData);
							m_ObStructionReport.AddItem(strObstructionRpt);
						}
					}
				}
				if (ObstructionInterface::LoadRunwayData(structObj,runObj,vObstructionHeights,RUNWAYMARK_SECOND))
				{
					for (int m = 0; m < structObj.GetPath().getCount(); m++)
					{
						if(structObj.getUserData(m)->dStructHeight - vObstructionHeights[m] > 0)
						{
							CLatitude fixlatitude;
							CLongitude fixlongitude;
							CPoint2008 point = structObj.GetPath().getPoint(m);
							ConvertPosToLatLong(point,fixlatitude, fixlongitude);
							CString strLL;
							CString strLat,strLong;
							fixlatitude.GetValue(strLat);
							fixlongitude.GetValue(strLong);

							strLL.Format("Latitude = %s ; Longitude = %s",strLat,strLong);
							m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(),structObj.GetObjNameString());
							CString strName = _T("");
							strName.Format(_T("Runway %sR"),runObj.GetMarking2().c_str());
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,1,strName);
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,2,strLL);
							CString strValue = _T("");
							strValue.Format(_T("%.f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,\
								pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vObstructionHeights[m]));
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,3,strValue);

							CString strData = _T("");
							strData.Format(_T("%.f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,\
								pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),structObj.getUserData(m)->dStructHeight - vObstructionHeights[m]));
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,4,strData);

							CString strObstructionRpt = _T("");
							strObstructionRpt.Format(_T("%s,%s,%s,%s,%s"),structObj.GetObjNameString(),strName,strLL,strValue,strData);
							m_ObStructionReport.AddItem(strObstructionRpt);
						}
					}
				}		
			}
			for ( int n = 0; n < (int)vObject2.size(); n++)
			{
				std::vector<DistanceUnit> vObstructionHeights;
				Taxiway taxiObj;
				altObject = vObject2.at(n);
				taxiObj.ReadObject(altObject.getID());
				if(ObstructionInterface::LoadTaxiwayData(structObj,taxiObj,vObstructionHeights))
				{
					for (int m = 0; m < (int)vObstructionHeights.size(); m++)
					{
						if(structObj.getUserData(m)->dStructHeight - vObstructionHeights[m] > 0)
						{
							CLatitude fixlatitude;
							CLongitude fixlongitude;
							CPoint2008 point = structObj.GetPath().getPoint(m);
							ConvertPosToLatLong(point,fixlatitude, fixlongitude);
							CString strLL;
							CString strLat,strLong;
							fixlatitude.GetValue(strLat);
							fixlongitude.GetValue(strLong);

							strLL.Format("Latitude = %s ; Longitude = %s",strLat,strLong);

							m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(),structObj.GetObjNameString());
							CString strName = _T("");
							strName.Format(_T("Taxiway %s"),taxiObj.GetMarking().c_str());
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,1,strName);
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,2,strLL);
							CString strValue = _T("");
							strValue.Format(_T("%.f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,\
								pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vObstructionHeights[m]));
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,3,strValue);

							CString strData = _T("");
							strData.Format(_T("%.f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,\
								pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),structObj.getUserData(m)->dStructHeight - vObstructionHeights[m]));
							m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount()-1,4,strData);

							CString strObstructionRpt = _T("");
							strObstructionRpt.Format(_T("%s,%s,%s,%s,%s"),structObj.GetObjNameString(),strName,strLL,strValue,strData);
							m_ObStructionReport.AddItem(strObstructionRpt);
						}
					}
				}
			}
		}
	}
}
void DlgObstructionReport::ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude )
{
	CLatLongConvert convent;
	CLatitude clat;
	CLongitude cLong;
 	clat.SetValue(m_AirportInfo.getLatitude());
 	cLong.SetValue(m_AirportInfo.getLongtitude());	
 	CPoint2008 refPoint = m_AirportInfo.getRefPoint() * SCALE_FACTOR;
 	convent.Set_LatLong_Origin(&clat,&cLong,0,0);
 
 	LatLong_Structure convented(&latitude,&longitude);
 
 	float deltaX = (float)(pos.getX() - refPoint.getX());
 	float deltaY = (float)(pos.getY() - refPoint.getY());
 	deltaX = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaX);
 	deltaY = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaY);

	convent.Point_LatLong(deltaX,deltaY,&convented);
}

/////////////Obstruction interface
bool ObstructionInterface::LoadRunwayData(Structure& structObj,Runway& runObj,std::vector<DistanceUnit>& vObstructionHeights,RUNWAY_MARK emRunwayMark)
{
	int nPointCnt = structObj.GetPath().getCount();
	vObstructionHeights.clear();
	vObstructionHeights.resize(nPointCnt, ARCMath::DISTANCE_INFINITE);
	//find the first intersections
	ARCVector3 vPos = GetLocation(structObj);

	ARCVector3 vPoint1ToPos = vPos - runObj.GetPath().getPoint(0);
	ARCVector3 vPoint2ToPos = vPos - runObj.GetPath().getPoint(1);

	double cosAngleOfDirRunway1 = runObj.GetDir(RUNWAYMARK_FIRST).GetCosOfTwoVector( vPoint1ToPos  ) ;
	double cosAngleOfDirRunway2 = runObj.GetDir(RUNWAYMARK_SECOND).GetCosOfTwoVector( vPoint2ToPos );


	//check the if center point is in the which side
	if(cosAngleOfDirRunway1 > 0 && cosAngleOfDirRunway2  < 0 ) //means point behind the end 
	{
		
		OffsetHightMap offsetHightMap1;
		offsetHightMap1.BuildHeightMap(runObj.GetObstruction(emRunwayMark).ThresHold);

		for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
		{
			ARCVector3 ptTpPoint1 =structObj.GetPath().getPoint(ptIdx) - runObj.GetPath().getPoint(1);
			double cosAngleOfDirRunway1 = runObj.GetDir((RUNWAY_MARK)((emRunwayMark+1)%2)).GetCosOfTwoVector( ptTpPoint1 );
			DistanceUnit doffset = ptTpPoint1.Length() * abs(cosAngleOfDirRunway1);
			if(doffset < offsetHightMap1.getMaxOffset() )
				vObstructionHeights[ptIdx] = offsetHightMap1.getOffsetHeight(doffset);
			else
				vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
		}
		if(isObstructionsValid(vObstructionHeights,structObj))
			return true;
		return false;
	}
	if(cosAngleOfDirRunway1 >0 && cosAngleOfDirRunway2 > 0) //means point at the side
	{			
	
		OffsetHightMap ofsetHiehgMapRw1;
		ofsetHiehgMapRw1.BuildHeightMap(runObj.GetObstruction(emRunwayMark).Lateral);

		for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
		{
			ARCVector3 ptToPoint1 = structObj.GetPath().getPoint(ptIdx) - runObj.GetPath().getPoint(0);
			double cosAngleOfDirRunway1 = runObj.GetDir((RUNWAY_MARK)((emRunwayMark+1)%2)).GetCosOfTwoVector( ptToPoint1  );
			DistanceUnit dOffset = ptToPoint1.Length() * sqrt(1-cosAngleOfDirRunway1*cosAngleOfDirRunway1);
			dOffset -= runObj.GetWidth() * 0.5;
			if(dOffset < ofsetHiehgMapRw1.getMaxOffset() )				
				vObstructionHeights[ptIdx] = ofsetHiehgMapRw1.getOffsetHeight(dOffset);
			else
				vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
		}
		if( isObstructionsValid(vObstructionHeights,structObj) )
			return true;
		return false;
	
	}
	if( cosAngleOfDirRunway1 < 0 && cosAngleOfDirRunway2 > 0) //means point behind the threshold
	{
		OffsetHightMap offsetHightMap1;
		offsetHightMap1.BuildHeightMap(runObj.GetObstruction(emRunwayMark).ThresHold);

		for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
		{
			ARCVector3 ptTpPoint1 = structObj.GetPath().getPoint(ptIdx) - runObj.GetPath().getPoint(0);
			double cosAngleOfDirRunway1 = runObj.GetDir((RUNWAY_MARK)((emRunwayMark+1)%2)).GetCosOfTwoVector( ptTpPoint1 );
			DistanceUnit doffset = ptTpPoint1.Length() * abs(cosAngleOfDirRunway1);
			if(doffset < offsetHightMap1.getMaxOffset() )
				vObstructionHeights[ptIdx] = offsetHightMap1.getOffsetHeight(doffset);
			else
				vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
		}
		if(isObstructionsValid(vObstructionHeights,structObj))
			return true;
		return false;
	}
	return false;
}

bool ObstructionInterface::LoadTaxiwayData(Structure& structObj,Taxiway& taxiObj,std::vector<DistanceUnit>& vObstructionHeights)
{

	int nPointCnt = structObj.GetPath().getCount();
	OffsetHightMap offsetHightMap;
	offsetHightMap.BuildHeightMap( taxiObj.GetObjSurface() );
	vObstructionHeights.clear();
	vObstructionHeights.resize(nPointCnt, ARCMath::DISTANCE_INFINITE);

	for(int ptIdx=0;ptIdx < nPointCnt;ptIdx++)
	{
		CPoint2008 pt = structObj.GetPath().getPoint(ptIdx);
		DistancePointPath3D distPtLine(pt, taxiObj.GetPath());
		DistanceUnit doffset = sqrt(distPtLine.GetSquared());
		doffset -= taxiObj.GetWidth() * 0.5;
		if(doffset < offsetHightMap.getMaxOffset() )
			vObstructionHeights[ptIdx] = offsetHightMap.getOffsetHeight(doffset);
		else
			vObstructionHeights[ptIdx] = ARCMath::DISTANCE_INFINITE;
	}
	if(isObstructionsValid(vObstructionHeights,structObj))
		return true;

	return false;
}

bool ObstructionInterface::isObstructionsValid(const std::vector<DistanceUnit>& vObstructionHeights,Structure& structObj)
{
	int nPtCount = structObj.GetPath().getCount();
	for(int i=0;i<nPtCount;++i)
	{
		UserInputData * pUseInput = structObj.getUserData(i);
		if(  vObstructionHeights[i] < pUseInput->dElevation + pUseInput->dStructHeight )
			return true;
	}

	return false;
}

ARCPoint3 ObstructionInterface::GetLocation( const Structure& structObj )
{
	const CPath2008& path = structObj.GetPath();
	ARCPoint3 location(0,0,0);
	for(int i =0 ;i<path.getCount();i++){
		location += path.getPoint(i);
	}
	location /= path.getCount();
	return location; 
}
void DlgObstructionReport::OnBnClickedPrint()
{
	// TODO: Add your control notification handler code here
	m_wndListCtrl.PrintList(_T("Obstructions Report"),_T("Obstructions Report"));
}

void DlgObstructionReport::OnBnClickedButtonExport()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( FALSE,"spd", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Obstructions Report File (*.spd)|*.spd;*.SPD|All type (*.*)|*.*|", NULL );
	if(filedlg.DoModal()!=IDOK)
		return;

	CString csFileName=filedlg.GetPathName();
	m_ObStructionReport.saveDataSetToOtherFile(csFileName);
}

void ObstructionReport::writeData(ArctermFile& p_file)const
{
	for (int i =0; i < (int)m_vDataString.size(); i++)
	{
		p_file.writeField(m_vDataString.at(i));
		p_file.writeLine();
	}
}