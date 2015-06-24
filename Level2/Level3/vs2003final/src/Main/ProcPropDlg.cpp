// ProcPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcPropDlg.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "3DView.h"
#include "Floor2.h"
#include <CommonData/Fallback.h>
#include "../common/UnitsManager.h"
#include "common\WinMsg.h"
#include "engine\dep_srce.h"
#include "engine\dep_sink.h"
#include "engine\lineproc.h"
#include "engine\baggage.h"
#include "engine\hold.h"
#include "engine\gate.h"
#include "engine\floorchg.h"
#include "engine\barrier.h"
#include "engine\proclist.h" 
#include "engine\ElevatorProc.h"
#include "engine\IntegratedStation.h"
#include "engine\MovingSideWalk.h"
#include "engine\conveyor.h"
#include "engine\Stair.h"
#include "engine\Escalator.h"
#include "engine\proclist.h"
#include "engine\Arp.h"
#include "engine\runwayproc.h"
#include "engine\Taxiwayproc.h"
#include "engine\NodeProc.h"
#include "engine\DeiceBayProc.h"
#include "engine\FixProc.h" 
#include "engine\StandProc.h"
#include "inputs\SideMoveWalkDataSet.h"
#include "inputs\RailWayData.h"
#include "inputs\AllCarSchedule.h"
#include "inputs\SubFlowList.h"
#include "inputs\schedule.h"
#include "inputs\procdata.h"
#include "ChangeVerticalProfileDlg.h"
#include "PtLineChart.h"
#include "UsedProcInfoBox.h"
#include "ModifyLatLong.h"
#include "procpropdlg.h"
#include "Engine/SectorProc.h"

#include <algorithm>
#include ".\procpropdlg.h"

#include "../Common/LatLong.h"
#include "DlgInitArp.h"

#include "../Common/LatLongConvert.h"
#include "../Inputs/AirportInfo.h"
#include "../Inputs/AirsideInput.h"

#include "../Engine/ContourProc.h"
#include "../Engine/ApronProc.h"
#include "../Engine/BillboardProc.h"
#include "../Engine/HoldProc.h"
#include "../Engine/FixProc.h"
#include "../Engine/fixedq.h"
#include "../Engine/RetailProcessor.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_PROCPROP_WIDTH	100
#define ID_PROCPROP_RUNWAYNUMBER	101
//#define ID_PROCPROP_THRESHOLD	102
//#define ID_PROCPROP_THRESHOLD2	103
#define ID_PROCPROP_LOWERLIMIT	104
#define ID_PROCPROP_UPPERLIMIT	105
#define ID_PROCPROP_AUTONUMBER	106
//#define ID_PROCPROP_MARKING		107
//#define ID_PROCPROP_MARKING2	108
#define ID_PROCPROP_LATLONG		110
#define ID_PROCPROP_HEIGHT		109
//_T("Threshold"), _T("Threshold2"),, _T("Marking"), _T("Marking2")
const LPCTSTR CProcPropDlg::s_szPropName[] = {
	_T("Width"), _T("Runway Number"),  _T("Lower Limit"),
	_T("Upper Limit"), _T("Automatically Number"),_T("Contour Height"),
	_T("Height"),_T("Thickness"),_T("Type"),_T("Text"),_T("Bitmap path")
};

/////////////////////////////////////////////////////////////////////////////
// CProcPropDlg dialog

//#define ENTRYPOINT_NODE  0x0010
//#define ENTRYPOINT_ROOT_NODE 0x0011

CProcPropDlg::CProcPropDlg(Processor* _pProc, const ARCVector3& _location,CWnd* pParent /*=NULL*/)
	: CDialog(CProcPropDlg::IDD, pParent)
	, m_nShapeIndex(0)
	, m_nForceProcType(0)
	,m_emGateType(ArrGate)
{
	m_pProc = _pProc;
	//{{AFX_DATA_INIT(CProcPropDlg)
	m_csLevel1 = _T("");
	m_csLevel2 = _T("");
	m_csLevel3 = _T("");
	m_csLevel4 = _T("");
	//}}AFX_DATA_INIT
	m_nFixed = -1;
	m_iSelectType = -1;
	//m_bIsCommomProcessor=true;
//	m_pTempHoldingArea=new HoldingArea();
	m_stationServiceLocation=_location;
	m_hRClickItem = NULL;
	m_bTypeCanChange = false;
	m_iUpdateAllRailWay = -1;
//	m_iMovingSpeed = 1;
	//m_nTempStationOrientation = 0; 
//	m_iPickUpItemType = -1;

	m_dConveyWidth= SCALE_FACTOR;
	m_dProcessorLength = 0.0;
	m_dPathAngle = 0.0;
	m_bZPosHasBeenChanged = false;
	m_bHavePath=false;

	m_dWidth = 0;
	m_dThreshold = 0;
	m_dThreshold2 = 0;
	m_dLowerLimit = 0;
	m_dUpperLimit = 0;
	m_nAutoNumber = 0;
	m_strMarking = _T("");
	m_strMarking2 = _T("");
	
	m_strLat = _T("N 0:0:0");
	m_strLong = _T("E 0:0:0");

	m_nForceProcType = -1;

	m_dHeight = 0.0;
	
	m_hFixLL = NULL;
	m_hFixXY = NULL;

	m_nFixeOperationType = 0;

	m_treeProp.setInputTerminal( GetInputTerminal() );
	m_busepushback=FALSE;

	m_bBackup = false;
	m_bFirst = true;

	m_bInOutConstraintProc = false;
//06-3-29 add billboard processor
	m_hBillboardHeight = NULL;
	m_hBillboardThickness= NULL;
	m_hBillboardAngle = NULL;
	m_hBillboardType= NULL;
	m_hBillBoardText= NULL;
	m_hBillBoardBitmapPath= NULL;
	m_hBillBoardCoefficient= NULL;
	m_hBillboardRadius = NULL;
	m_hExpSurePath = NULL;

	//m_dBbHeight = 0.0;
	//m_dBbThickness = 0.0;
	//m_nBbType = 0;
	//m_strBbText ="";
	//m_strBbPath ="";
	//m_dBbCoefficient = 0.0;
	 m_hFix = NULL;
	 m_hInBound = NULL ;
	 m_hOutBoundLegnm= NULL;
	 m_hOutBoundLegmin= NULL;
	 m_hRight= NULL;
	 m_hMaxAlt= NULL;
	 m_hMinAlt= NULL;

	 m_strFix = "";
	 m_nInBoundDegree=0;
	 m_dOutBoundnm=0;
	 m_dOutBoundmin=0;
	 m_bRight=true;
	 m_dmaxAlt=0;
	 m_dminAlt=0;

	 m_dbandinterval = 0;
	 m_vBridgeConnectPoints.clear();
	 m_nBridgeConnectPointCount =1;
}


void CProcPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcPropDlg)
	DDX_Control(pDX, IDC_CHECK_EMERGENT, m_btnEmergent);
//	DDX_Control(pDX, IDC_CHECK_ACSTANDGATE, m_butACStandGate);

	DDX_Control(pDX, IDCANCEL, m_btnCancle);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_CHECK_OFFGATE, m_btnOffgate);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_comboType);
	DDX_Control(pDX, IDC_COMBO_LEVEL4, m_comboLevel4);
	DDX_Control(pDX, IDC_COMBO_LEVEL3, m_comboLevel3);
	DDX_Control(pDX, IDC_COMBO_LEVEL2, m_comboLevel2);
	DDX_Control(pDX, IDC_COMBO_LEVEL1, m_comboLevel1);
	DDX_Control(pDX, IDC_TREE_PROPERTY, m_treeProp);
	DDX_CBString(pDX, IDC_COMBO_LEVEL1, m_csLevel1);
	DDX_CBString(pDX, IDC_COMBO_LEVEL2, m_csLevel2);
	DDX_CBString(pDX, IDC_COMBO_LEVEL3, m_csLevel3);
	DDX_CBString(pDX, IDC_COMBO_LEVEL4, m_csLevel4);

	DDX_Control(pDX, IDC_COMBO_SHAPE, m_cmbShape);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcPropDlg, CDialog)
	//{{AFX_MSG_MAP(CProcPropDlg)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL2, OnDropdownComboLevel2)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL3, OnDropdownComboLevel3)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL4, OnDropdownComboLevel4)
	ON_CBN_KILLFOCUS(IDC_COMBO_LEVEL1, OnKillfocusComboLevel1)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROCPROP_PICKFROMMAP, OnProcpropPickfrommap)
	ON_COMMAND(ID_PROCPROP_TOGGLEDQUEUEFIXED, OnProcpropToggledqueuefixed)
	ON_COMMAND(ID_PROCPROP_TOGGLEDBACKUP, OnProcpropToggledBackup)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnSelchangeComboType)
	ON_WM_SIZE()
	ON_MESSAGE(MODIFYVALUE,OnModifyValue)
	ON_COMMAND(ID_GATETYPE_DEFINE,OnDefineGateType)
	ON_COMMAND(ID_RETAILTYPE_RETAILTYPE,OnDefineRetailType)
	ON_COMMAND(ID_CAR_CAPACITY, OnCarCapacity)
	ON_COMMAND(ID_CAR_NUMBERS, OnCarNumbers)
	ON_COMMAND(ID_PLATFORM_WIDTH, OnPlatformWidth)
	ON_COMMAND(ID_PLATFORM_HEIGH, OnPlatformLength)
	ON_COMMAND(ID_CAR_WIDTH, OnCarWidth)
	ON_COMMAND(ID_CAR_HEIGTH, OnCarLength)
	ON_COMMAND(ID_DEFINE_ENTRY_DOOR_NUM, OnDefineEntryDoorNum)
	ON_COMMAND(ID_WAITAREA_LENGTH,OnDefineWaitAreaLength)
	ON_COMMAND(ID_WAITAREA_WIDTH,OnDefineWaitAreaWidth)
	ON_COMMAND(ID_NUMBER_OF_LIFT,OnDefineNumberOfLift)
	ON_COMMAND(ID_LIFT_LENGTH,OnDefineLiftLength)
	ON_COMMAND(ID_LIFT_WIDTH,OnDefineLiftWidth)
	ON_COMMAND(ID_MIN_FLOOR,OnDefineMinFloor)
	ON_COMMAND(ID_MAX_FLOOR,OnDefineMaxFloor)
	ON_COMMAND(ID_TOGGLED,OnDefineToggled)
	ON_COMMAND(ID_DEFINE_EXIT_DOOR_NUMBER, OnDefineExitDoorNumber)
	ON_COMMAND(ID_EXITDOOR_TOGGLED, OnExitdoorToggled)
	ON_COMMAND(ID_STATION_ORIENTATION, OnStationOrientation)
	ON_COMMAND(ID_PROCPROP_DELETE, OnProcpropDelete)
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_PROCPROP_DEFINE_Z_POS, OnProcpropDefineZPos)
	ON_COMMAND(ID_MENUE_CONVEY_WIDTH, OnMenueConveyWidth)
	ON_COMMAND(ID_ADJUST_LENGTH, OnAdjustLength)
	ON_COMMAND(ID_PICKUP_FORM_MAP, OnProcpropPickfrommap)
	ON_COMMAND(ID_ADJUST_ANGLE, OnAdjustAngle)
	//}}AFX_MSG_MAP
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_CBN_DROPDOWN(IDC_COMBO_SHAPE, OnDropdownComboShape)

	ON_COMMAND_RANGE(ID_PROCPROP_WIDTH, ID_PROCPROP_LATLONG, OnProcpropModifyValue)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_COMMAND(ID_PROCPROP_CHANGLATLONG, OnProcpropChanglatlong)
	ON_COMMAND(ID_BILLBOARD_MODIFYHEIGHT, OnBillboardModifyheight)
	ON_COMMAND(ID_BILLBOARD_MODIFYTHICKNESS, OnBillboardModifythickness)
	ON_COMMAND(ID_BILLBOARD_TEXT, OnBillboardText)
	ON_COMMAND(ID_BILLBOARD_LOGO, OnBillboardLogo)
	ON_COMMAND(ID_BILLBOARD_MODIFYTEXT, OnBillboardModifytext)
	ON_COMMAND(ID_BILLBOARD_MODIFYPATH, OnBillboardModifypath)
	ON_COMMAND(ID_BILLBOARD_MODIFYCOEFFICIENT, OnBillboardModifycoefficient)
	ON_COMMAND(ID_BILLBOARD_MODIFYANGLE, OnBillboardModifyangle)
	ON_COMMAND(ID_HOLD_CHOOSEAFIX, OnHoldChooseafix)
	ON_COMMAND(ID_HOLDARGUMENT_EDIT, OnHoldargumentEdit)
	ON_COMMAND(ID_RUNWAYTHRESHOLDA_MODIFY, OnRunwaythresholdaModify)
	ON_COMMAND(ID_RUNWAYMARK_MODIFY, OnRunwaymarkModify)
	ON_COMMAND(ID_ALTOBJECTNUMBER_DEFINENUMBER, OnPropNumberModify)

	///added by cj ,for entry point
	ON_COMMAND(ID_ADD_ENTRY_POINT,OnAddEntryPoint) 
	ON_COMMAND(ID_DEL_ENTRY_POINT,OnDelEntryPoint)
	ON_STN_CLICKED(IDC_STATIC_LINE1, OnStnClickedStaticLine1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcPropDlg message handlers

BOOL CProcPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitEntryPoints() ;
	if( m_pProc )
	{
		m_bTypeCanChange = false;
		m_bBackup = m_pProc->IsBackup();
	}		
	else
	{
		m_bTypeCanChange = true;
	}

	if (m_pProc)
	{
		int nProcType = m_pProc->getProcessorType();

		if (nProcType == IntegratedStationProc )
		{
			InitIntegratedProcessor();
		}
		else if(nProcType == Elevator)
		{
			m_tempElevator=*((ElevatorProc*)m_pProc);
		}
		else if (nProcType == BillboardProcessor)
		{
			m_tempBillBoardProc = *((BillboardProc *)m_pProc);
		}
		else if (nProcType == FixProcessor)
		{
			SetWindowText(_T("Waypoint Property"));
		}
		else if (nProcType == HoldProcessor)
		{
			SetWindowText(_T("Hold Property"));
		}
		
		if(nProcType == BridgeConnectorProc)
		{
			BridgeConnector* pConnector = (BridgeConnector*)m_pProc;
			if (pConnector)
			{
				m_nBridgeConnectPointCount = pConnector->GetConnectPointCount();
				for(int idx =0; idx < m_nBridgeConnectPointCount; idx++)
				{						
					BridgeConnector::ConnectPoint conPoint = pConnector->GetConnectPointByIdx(idx);
					m_vBridgeConnectPoints.push_back(conPoint);
				}
			}

		}
	}
	else
	{
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		int nFloorCount=pDoc->GetCurModeFloor().m_vFloors.size();
		m_tempElevator.SetMinFloor(0);
		m_tempElevator.SetMaxFloor(nFloorCount-1);
		m_tempElevator.GetWaitAreaPos().clear();
		for(int i=m_tempElevator.GetMinFloor()+1;i<=m_tempElevator.GetMaxFloor()+1;i++)
		{
			m_tempElevator.GetWaitAreaPos().push_back(TRUE);
		}
	}
	//

	//////////////////////////////////////////////////////////////////////////
	//	Set IconComboBox index
	//	Add by Kevin
	//	2005-4-11 11:16
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(pDoc != NULL &&pDoc->GetSelectProcessorsCount() > 0)
	{
		CObjectDisplay *pObjDisplay = pDoc->GetSelectedObjectDisplay(0);
		if(pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2* pProc2= (CProcessor2 *)pObjDisplay;
			CShape* pShape = pProc2->GetShape();
			if(pShape)
				m_cmbShape.SetSelectedIndex(pShape->Name());
		}
	}


	InitTempProcInfo();
	LoadCombo();
	LoadType();
	ForceControlStatus();
	
	m_btnOffgate.ShowWindow( SW_HIDE );	

	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcPropDlg::InitTempProcInfo()
{
  if(m_pProc)
  {
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	
	pDoc->m_tempProcInfo.SetProcIndex(m_pProc->getIndex());

	if(m_pProc->serviceLocationLength()>0)
	{
		std::vector<ARCVector3> _vServiceLocation;
		int iLocationNum=m_pProc->serviceLocationLength();
		Point* pTempPoint=m_pProc->serviceLocationPath()->getPointList();
		for( int i=0; i< iLocationNum ;++i,++pTempPoint)
		{
			_vServiceLocation.push_back(ARCVector3(pTempPoint->getX(),pTempPoint->getY(),0.0));			
		}
		
		pDoc->m_tempProcInfo.SetTempServicLocation(_vServiceLocation);

	}


	if(m_pProc->inConstraintLength()>0)
	{
		std::vector<ARCVector3> _vInConstraint;

		int iInConstrainNum=m_pProc->inConstraintLength();
		Point* pTempPoint=m_pProc->inConstraint()->getPointList();
		for( int i=0; i< iInConstrainNum ;++i,++pTempPoint)
		{
			_vInConstraint.push_back(ARCVector3(pTempPoint->getX(),pTempPoint->getY(),0.0));			
		}
		
		pDoc->m_tempProcInfo.SetTempInConstraint(_vInConstraint);
		

	}



	if(m_pProc->outConstraintLength()>0)
	{
		std::vector<ARCVector3> _vOutConstraint;

		int iOutConstraint=m_pProc->outConstraintLength();
		Point* pTempPoint=m_pProc->outConstraint()->getPointList();
		for( int i=0; i< iOutConstraint ;++i,++pTempPoint)
		{
			_vOutConstraint.push_back(ARCVector3(pTempPoint->getX(),pTempPoint->getY(),0.0));			
		}
		
		pDoc->m_tempProcInfo.SetTempOutConstrain(_vOutConstraint);

	}
	
	if(m_pProc->queueLength()>0)
	{
		std::vector<ARCVector3> _vQueieList;

		int iQueueLength=m_pProc->queueLength();
		Point* pTempPoint=m_pProc->queuePath()->getPointList();
		for( int i=0; i< iQueueLength ;++i,++pTempPoint)
		{
			_vQueieList.push_back(ARCVector3(pTempPoint->getX(),pTempPoint->getY(),0.0));			
		}
		pDoc->m_tempProcInfo.SetTempQueue(_vQueieList);
	}

	if( m_pProc->getProcessorType() == BaggageProc )
	{
		std::vector<ARCVector3>_vBarriers;
		int iCount = (( BaggageProcessor*)m_pProc)->GetBarrier()->serviceLocationLength();
		Point* pTempPoint = (( BaggageProcessor*)m_pProc)->GetBarrier()->serviceLocationPath()->getPointList();
		for( int i=0; i<iCount; ++i ,++pTempPoint)
		{
			_vBarriers.push_back(ARCVector3(pTempPoint->getX(),pTempPoint->getY(),0.0));			
		}
		pDoc->m_tempProcInfo.SetTempBaggageBarrier( _vBarriers );


		_vBarriers.clear();
		iCount = (( BaggageProcessor*)m_pProc)->GetPathBagMovingOn()->getCount();
		pTempPoint = (( BaggageProcessor*)m_pProc)->GetPathBagMovingOn()->getPointList();
		for( i=0; i<iCount; ++i ,++pTempPoint)
		{
			_vBarriers.push_back(ARCVector3(pTempPoint->getX(),pTempPoint->getY(),0.0));			
		}
		pDoc->m_tempProcInfo.SetTempBaggageMovingPath( _vBarriers );
	}
  }
}
// load level1 only
void CProcPropDlg::LoadCombo()
{
	m_comboLevel1.ResetContent();
	m_comboLevel2.ResetContent();
	m_comboLevel3.ResetContent();
	m_comboLevel4.ResetContent();

	// level1 
    StringList groups;
	GetInputTerminal()->GetProcessorList()->getAllGroupNames( groups, -1 );
    for( int i = 0; i < groups.getCount(); i++ )
        m_comboLevel1.AddString( groups.getString( i ) );


	if( m_pProc == NULL)
	{
		m_comboLevel1.EnableWindow();
		m_comboLevel2.EnableWindow();
		m_comboLevel3.EnableWindow();
		m_comboLevel4.EnableWindow();
		// might need clean the test.
	}
	else
	{
		m_comboLevel1.EnableWindow();
		m_comboLevel2.EnableWindow();
		m_comboLevel3.EnableWindow();
		m_comboLevel4.EnableWindow();
		m_comboType.EnableWindow( FALSE );
		const ProcessorID* pID = m_pProc->getID();
		char szName[128];
		pID->getNameAtLevel( szName, 0 );
		m_comboLevel1.SetWindowText( szName );
		szName[0] = 0;
		pID->getNameAtLevel( szName, 1 );
		m_comboLevel2.SetWindowText( szName );
		szName[0] = 0;
		pID->getNameAtLevel( szName, 2 );
		m_comboLevel3.SetWindowText( szName );
		szName[0] = 0;
		pID->getNameAtLevel( szName, 3 );
		m_comboLevel4.SetWindowText( szName );
	}
	

}
bool CProcPropDlg::SortProTypeString(std::set<CString>& proTypeSet,std::map<CString,int>& ProTypeMap,int nStartPos, int nEndPos)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	int iFloorCount = pDoc->GetCurModeFloor().m_vFloors.size();
	for (int i = nStartPos;i <= nEndPos; i++)
	{
		if( (i == StairProc || i == EscalatorProc ) && iFloorCount == 1 )
		{
			continue;
		}
		CString tempStr(PROC_TYPE_NAMES[i]);
		proTypeSet.insert(tempStr);
		ProTypeMap.insert(std::make_pair(tempStr,i));
	}
	return true;
}

void CProcPropDlg::LoadType()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	int iFloorCount = pDoc->GetCurModeFloor().m_vFloors.size();

	bool bSel = false;
	m_comboType.ResetContent();

	int startmode, endmode;
	switch( pDoc->m_systemMode )
	{
	case EnvMode_LandSide:
		startmode = StretchProcessor;
		endmode = ParkingLotProcessor;
		break;
	case EnvMode_Terminal:
		startmode = PointProc;
		endmode = RetailProc;
		break;
	case EnvMode_AirSide:
		startmode = ArpProcessor;
		endmode = SectorProcessor;
		break;
	default:
		{
			ASSERT(0);
			startmode = PointProc;
			endmode = BillboardProcessor;
			break;
		}

	}

	std::set<CString> strTypeSet;
	std::map<CString,int>strTypeMap;
	
	SortProTypeString(strTypeSet,strTypeMap,startmode,endmode);

	std::set<CString>::const_iterator setIter = strTypeSet.begin();
	for( int i=startmode; i<= endmode; i++ )
//	for( int i=0; i<PROC_TYPES; i++ )
	{
		//Change by Mark Chen
		//if( i == StairProc && iFloorCount == 1 )// stair processor must between two floors
		if( (i == StairProc || i == EscalatorProc ) && iFloorCount == 1 )// stair or escalator processor must between two floors
		{
			continue;
		}
		if (setIter != strTypeSet.end())
		{
			char *buff;
			int  nPos;
			CString tempStr(*setIter);
			buff = strstr(tempStr.GetBuffer(),"Processor");
			if (buff)
			{
				nPos = buff - tempStr.GetBuffer();
			}
			else
			{
				nPos = tempStr.GetLength();
			}
			if (strTypeSet.find(tempStr)!= strTypeSet.end())
			{
				CString strValue = tempStr.Left(nPos);
				strValue.Trim();
				int nIdx = m_comboType.AddString( strValue );
				m_comboType.SetItemData( nIdx, strTypeMap[tempStr] );
				if( m_pProc && m_pProc->getProcessorType() == strTypeMap[tempStr] )
				{
					m_comboType.SetCurSel( nIdx );
					bSel = true;
				}
			}
		}
		++setIter;
	}			
	if( !bSel )
		m_comboType.SetCurSel( -1 );
	
}
//only used for ArcVector3
int CProcPropDlg::GetZValue(float fZ)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	std::vector<CFloor2*> vFloors = pDoc->GetCurModeFloor().m_vFloors;
	std::vector<CFloor2*>::iterator iter;
	for (int i = 0; i < (int)vFloors.size(); i++)
	{
		if (vFloors[i]->Altitude() == fZ)
			break;
	}

	return i;
}
//fz:real altitude
CFloor2 * CProcPropDlg::GetFloorFromZPos(float fZ)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	std::vector<CFloor2*> vFloors = pDoc->GetCurModeFloor().m_vFloors;
	std::vector<CFloor2*>::iterator iter = vFloors.begin();
	for (;iter != vFloors.end();++iter)
	{
		if( ((*iter)->Altitude() - fZ) * ((*iter)->Altitude() - fZ) < 1.00000 )
		{
			return *iter;
		}
	}

	return NULL;
	
}

void CProcPropDlg::InitBridgeConnectPointDefaultValue( int idx, HTREEITEM hItem )
{
	CString sLabel;

	sLabel.Format("Connect point %d", idx+1);
	HTREEITEM hSubItem = m_treeProp.InsertItem(sLabel, hItem);
	m_treeProp.SetItemData(hSubItem,idx);
	BridgeConnector::ConnectPoint conPoint;

	if (idx < (int)m_vBridgeConnectPoints.size())
	{
		conPoint = m_vBridgeConnectPoints.at(idx);
	}	
	else
	{
		conPoint.m_dWidth = 3*SCALE_FACTOR;
		conPoint.m_dLength = 10*SCALE_FACTOR;
		m_vBridgeConnectPoints.push_back(conPoint);
	}

	Point pt = conPoint.m_Location;
	CString csPoint;
	csPoint.Format( "Location x = %.2f; y = %.2f", 
		UNITSMANAGER->ConvertLength(pt.getX()),
		UNITSMANAGER->ConvertLength(pt.getY()) );

	CFloor2* pFloor = GetPointFloor(pt);
	if (pFloor)
		csPoint += _T("; Floor:") + pFloor->FloorName();
	m_treeProp.InsertItem( csPoint, hSubItem);

	sLabel.Format("Width("+UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits() )+")(%.2f)", UNITSMANAGER->ConvertLength(conPoint.m_dWidth));
	HTREEITEM hValueItem = m_treeProp.InsertItem(sLabel, hSubItem);
	m_treeProp.SetItemData(hValueItem,(DWORD)conPoint.m_dWidth);

	sLabel.Format("Collapsed length("+UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits() )+")(%.2f)", UNITSMANAGER->ConvertLength(conPoint.m_dLength));
	hValueItem = m_treeProp.InsertItem(sLabel, hSubItem);
	m_treeProp.SetItemData(hValueItem, (DWORD)conPoint.m_dLength);

	pt = conPoint.m_dirFrom;
	csPoint.Format( "Static direction from x = %.2f; y = %.2f", 
		UNITSMANAGER->ConvertLength(pt.getX()),
		UNITSMANAGER->ConvertLength(pt.getY()) );
	m_treeProp.InsertItem( csPoint, hSubItem);

	pt = conPoint.m_dirTo;
	csPoint.Format( "Static direction to x = %.2f; y = %.2f", 
		UNITSMANAGER->ConvertLength(pt.getX()),
		UNITSMANAGER->ConvertLength(pt.getY()) );
	m_treeProp.InsertItem( csPoint, hSubItem);
	
	m_treeProp.Expand( hSubItem, TVE_EXPAND );
}

void CProcPropDlg::LoadTree()
{
	m_hServiceLocation = NULL;
	m_hInConstraint = NULL;
	m_hOutConstraint = NULL;
	m_hQueue = NULL;
	m_hFixXY=NULL;
	m_hFixLL=NULL;

	m_hFix = NULL;
	m_hInBound = NULL;
	m_hOutBoundLegmin = NULL;
	m_hOutBoundLegnm = NULL;
	m_hRight = NULL;
	m_hMaxAlt = NULL;
	m_hMinAlt = NULL;

	m_hBandInterval = NULL;
	m_EntryPoint = NULL;

	m_treeProp.DeleteAllItems();
	int nSelType = m_comboType.GetCurSel();
	int nProcType = nSelType >=0 ? m_comboType.GetItemData(nSelType) : -1;
	
	if( nProcType != IntegratedStationProc && nProcType!= HoldProcessor && nProcType!= SectorProcessor && nProcType!= BridgeConnectorProc)
	{
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

		CString csLabel = CString("Service Location (") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
		
		// Service Location
		m_hServiceLocation = m_treeProp.InsertItem( csLabel );

		if( m_vServiceLocation.size() > 0 )
		{
			for( int i=0; i<static_cast<int>(m_vServiceLocation.size()); i++ )
			{
				ARCVector3 v3D = m_vServiceLocation[i];
				CString csPoint;

				//Change by Mark Chen 
				//if ( nProcType == ConveyorProc ||nProcType == StairProc )
				if ( nProcType == ConveyorProc ||nProcType == StairProc || nProcType == EscalatorProc )
				{
					csPoint.Format( "x = %.2f; y = %.2f; z=%.2f", 
						UNITSMANAGER->ConvertLength(v3D[VX]), 
						UNITSMANAGER->ConvertLength(v3D[VY]), 
						float(GetZValue(float(v3D[VZ]))));//v3D[VZ] / SCALE_FACTOR );
					if ( StairProc == GetCurrentProcType() || EscalatorProc == GetCurrentProcType() || ConveyorProc == GetCurrentProcType())
					{
						if(m_bZPosHasBeenChanged)
						{
							csPoint.Format( "x = %.2f; y = %.2f; z=%.2f", 
								UNITSMANAGER->ConvertLength(v3D[VX]), 
								UNITSMANAGER->ConvertLength(v3D[VY]), 
								v3D[VZ] / SCALE_FACTOR);
						}
					}

				}
				else
				{
					csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(v3D[VX]), UNITSMANAGER->ConvertLength(v3D[VY]) );
				}

				CFloor2* pFloor = NULL;
				pFloor = GetFloorFromZPos( static_cast<float>(v3D[VZ]) );
				if (StairProc == GetCurrentProcType() || EscalatorProc == GetCurrentProcType() || ConveyorProc == GetCurrentProcType() )
				{
					if (m_bZPosHasBeenChanged)
					{
						Point pt(v3D[VX], v3D[VY], v3D[VZ]);
						pFloor = GetPointFloor(pt);
					}
					else
					{
						pFloor = pDoc->GetActiveFloor();
					}
				}

				if (pFloor)
					csPoint += _T("; Floor:") + pFloor->FloorName();

				m_hFixXY = m_treeProp.InsertItem( csPoint, m_hServiceLocation  );
			}
			//add longitude latitude
			if(nProcType == FixProcessor )
			{
				CString strLL;
				strLL.Format("Latitude= %s ;Longitude= %s", m_strLat, m_strLong);
				m_hFixLL= m_treeProp.InsertItem( strLL, m_hServiceLocation  );
			}
			m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND );
		}
		else if (nProcType==FixProcessor&&m_pProc==NULL)// FixProcess LL ,location
		{
			CString strXY="x = 0.00; y = 0.00";
			m_hFixXY=m_treeProp.InsertItem(strXY,m_hServiceLocation);

			CString strLL="Latitude = N 0:0:0 ; Longitude = E 0:0:0";
			//			strLL.Format("Longitude = %s ; Latitude = %s",strLong,strLat);
			m_hFixLL= m_treeProp.InsertItem(strLL,m_hServiceLocation);

			
			m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND );
		}
		
		else if( m_pProc )
		{
			Path* pPath = m_pProc->serviceLocationPath();
			for( int i=0; i<pPath->getCount(); i++ )
			{
				Point pt = pPath->getPoint(i);
				CString csPoint;
				
				//Change by Mark Chen 
				//if ( nProcType == ConveyorProc || nProcType == StairProc)
				if ( nProcType == ConveyorProc || nProcType == StairProc || nProcType == EscalatorProc )
				{
					csPoint.Format( "x = %.2f; y = %.2f; z=%.2f",
						UNITSMANAGER->ConvertLength(pt.getX()),
						UNITSMANAGER->ConvertLength(pt.getY()), 
						pt.getZ() / SCALE_FACTOR );
				}
				else
				{
					csPoint.Format( "x = %.2f; y = %.2f", 
						UNITSMANAGER->ConvertLength(pt.getX()),
						UNITSMANAGER->ConvertLength(pt.getY()) );
				}

				CFloor2* pFloor = GetPointFloor(pt);
				if (pFloor)
					csPoint += _T("; Floor:") + pFloor->FloorName();

				m_hFixXY= m_treeProp.InsertItem( csPoint, m_hServiceLocation);

				// if fix process  add latitude longitude
				if (nProcType == FixProcessor)
				{
					CString strLat,strLong;
					((FixProc *)m_pProc)->m_pLatitude->GetValue(strLat);
					((FixProc *)m_pProc)->m_pLongitude->GetValue(strLong);
					m_strLat=strLat;
					m_strLong=strLong;

					//
					CString strLL;
					strLL.Format("Latitude= %s ;Longitude= %s",strLat,strLong);
					m_hFixLL= m_treeProp.InsertItem( strLL, m_hServiceLocation  );
					
					m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND); 

				}
			}

			
			if (nProcType == ArpProcessor)
			{
				string s = ((ArpProc*)m_pProc)->GetLatlong(ArpProc::Latitude);
				CString strValue = s.c_str();
				strValue += _T("; ");
				s = ((ArpProc*)m_pProc)->GetLatlong(ArpProc::Longitude);
				strValue += s.c_str();
				m_treeProp.InsertItem(strValue, m_hServiceLocation);
			}
			
			m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND );
		}
		
		if(nProcType==Elevator)
		{
			//Wait Area
			CString csLabel=CString("Wait Area");
			m_hWaitArea=m_treeProp.InsertItem(csLabel);
			CString sUnit=UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
			csLabel.Format(" ( %.2f )",UNITSMANAGER->ConvertLength(m_tempElevator.GetPlatFormLength()));
			csLabel=CString("Length (")+sUnit+csLabel;
			m_hWaitAreaLength=m_treeProp.InsertItem(csLabel,m_hWaitArea);

			csLabel.Format(" ( %.2f )",UNITSMANAGER->ConvertLength(m_tempElevator.GetPlatFormWidth()));
			csLabel=CString("Width (")+sUnit+csLabel;
			m_hWaitAreaWidth=m_treeProp.InsertItem(csLabel,m_hWaitArea);
			m_treeProp.Expand(m_hWaitArea,TVE_EXPAND);

			//Lift
			csLabel=CString("Lift");
			m_hLift=m_treeProp.InsertItem(csLabel);

			csLabel.Format(" ( %d )",m_tempElevator.GetNumberOfLift());
			csLabel=CString("Number Of Lift")+csLabel;
			m_hLiftNOL=m_treeProp.InsertItem(csLabel,m_hLift);

			csLabel.Format(" ( %.2f )",UNITSMANAGER->ConvertLength(m_tempElevator.GetElevatorAreaLength()));
			csLabel=CString("Length (")+sUnit+csLabel;
			m_hLiftLength=m_treeProp.InsertItem(csLabel,m_hLift);

			csLabel.Format(" ( %.2f )",UNITSMANAGER->ConvertLength(m_tempElevator.GetElevatorAreaWidth()));
			csLabel=CString("Width (")+sUnit+csLabel;
			m_hLiftWidth=m_treeProp.InsertItem(csLabel,m_hLift);

			m_treeProp.Expand(m_hLift,TVE_EXPAND);

			//Min\Max Floor
			CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
			CFloorList& floorList = pDoc->GetCurModeFloor().m_vFloors;
			CFloor2* pMindFloor = floorList.at(m_tempElevator.GetMinFloor());
			csLabel.Format(" ( %s)",pMindFloor->FloorName());
			csLabel=CString("Min Floor")+csLabel;
			m_hMinFloor=m_treeProp.InsertItem(csLabel);
			CFloor2* pMaxFloor = floorList.at(m_tempElevator.GetMaxFloor());
			csLabel.Format(" ( %s )",pMaxFloor->FloorName());
			csLabel=CString("Max Floor")+csLabel;
			m_hMaxFloor=m_treeProp.InsertItem(csLabel);

			//Wait Area Position
			csLabel=CString("Wait Area Position");
			m_hWaitAreaPos=m_treeProp.InsertItem(csLabel);
			int nn=m_tempElevator.GetWaitAreaPos().size();
			for(int i=m_tempElevator.GetMinFloor();i<=m_tempElevator.GetMaxFloor();i++)
			{
				CString strFloor;
				CFloor2* pFloor = floorList.at( i);
				strFloor.Format("Floor %s (%s)",pFloor->FloorName(),m_tempElevator.GetWaitAreaPos().at(i-m_tempElevator.GetMinFloor())?"Front":"Hind");
				HTREEITEM hTempItem=m_treeProp.InsertItem(strFloor,m_hWaitAreaPos);
				m_treeProp.SetItemData(hTempItem,i);
			}
		}
		else if (nProcType==BillboardProcessor) 
		{

			//orientation and radius
			CString sLabel;
			sLabel = CString( "Exposure Area (" ) + 
				UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + 
				CString( ")" );

//			m_hExpSurePath = m_treeProp.InsertItem( sLabel );
			m_hOutConstraint = m_treeProp.InsertItem( sLabel );
			if(m_vOutConstraint.size() >0)
			{
				for( int i=0; i<static_cast<int>(m_vOutConstraint.size()); i++ )//m_vBillboardExposureArea
				{
					ARCVector3 v3D = m_vOutConstraint[i];
					CString csPoint;
					csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(v3D[VX]), UNITSMANAGER->ConvertLength(v3D[VY]) );
					m_treeProp.InsertItem( csPoint, m_hOutConstraint  );
				}
			}
			else
			{
//				Path* pPath = m_tempBillBoardProc.GetExpsurePath();
				Path* pPath = m_tempBillBoardProc.outConstraint();
				for( int i=0; i<pPath->getCount(); i++ )
				{
					Point pt = pPath->getPoint(i);
					CString csPoint;
					csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(pt.getX()), UNITSMANAGER->ConvertLength(pt.getY()) );
//					m_treeProp.InsertItem( csPoint, m_hExpSurePath  );
					m_treeProp.InsertItem(csPoint,m_hOutConstraint);
				}
			}
			m_treeProp.Expand( m_hOutConstraint, TVE_EXPAND );

			//height
			CString strHeight;
			strHeight.Format(_T("Height(%s):%.0f"),
				UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()),
				UNITSMANAGER->ConvertLength(m_tempBillBoardProc.getHeight()));

			m_hBillboardHeight = m_treeProp.InsertItem(strHeight);

			CString strThickness;
			strThickness.Format(_T("Thickness(%s):%.0f"),
				UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()),
				UNITSMANAGER->ConvertLength(m_tempBillBoardProc.getThickness()));
			m_hBillboardThickness = m_treeProp.InsertItem(strThickness);

			//CString strAngle;
			//strAngle.Format(_T("Angle(degree):%d"),m_tempBillBoardProc.getAngle());
			//m_hBillboardAngle = m_treeProp.InsertItem(strAngle);

			//CString strCoefficient;
			//strCoefficient.Format(_T("Coefficient(%%):%.0f"),m_tempBillBoardProc.getCoefficient() *100.0);
			//m_hBillBoardCoefficient = m_treeProp.InsertItem(strCoefficient);
			//type
			CString strType;
			if ( m_tempBillBoardProc.getType() == 0)
			{
				 strType = _T("Type:Text");
				 m_hBillboardType = m_treeProp.InsertItem(strType);

				 CString strText = m_tempBillBoardProc.getText();
				 m_hBillBoardText = m_treeProp.InsertItem(strText,m_hBillboardType);
				 m_hBillBoardBitmapPath = NULL;
			}
			else //if (m_tempBillBoardProc.getType() == 1)
			{
				strType  = _T("Type:Logo");
				m_hBillboardType = m_treeProp.InsertItem(strType);

				CString strPath = m_tempBillBoardProc.getBitmapPath();
				m_hBillBoardBitmapPath = m_treeProp.InsertItem(strPath,m_hBillboardType);
				m_hBillBoardText = NULL;
			}

			m_treeProp.Expand(m_hBillboardType,TVE_EXPAND);
		}
		else
		{
			//Queue path
			// DepsinkProc, HoldAreaProc,BarrierProc and ConveyorProc no queue
			//if( nProcType != DepSinkProc && nProcType != HoldAreaProc && nProcType != BarrierProc && nProcType != ConveyorProc )
				if( nProcType != DepSinkProc && nProcType != HoldAreaProc &&
				nProcType != BarrierProc && nProcType != ConveyorProc &&
				nProcType != ArpProcessor && nProcType != RunwayProcessor &&
				nProcType != TaxiwayProcessor && nProcType != StandProcessor &&
				nProcType != NodeProcessor && nProcType != DeiceBayProcessor &&
				nProcType != FixProcessor && nProcType != ContourProcessor&&
				nProcType != ApronProcessor && nProcType != StairProc)
			{
				CString csLabel = CString( "Queue Points (" ) +	UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
				
				m_hQueue = m_treeProp.InsertItem( csLabel );
				if( nProcType != LineProc || nProcType != BaggageProc || nProcType != RetailProc)
				{
					if( m_nFixed > -1)
					{
						if( m_nFixed == 1 )
							m_treeProp.InsertItem( "Fixed", m_hQueue  );
						else
							m_treeProp.InsertItem( "Non Fixed", m_hQueue  );
					}
					else if( m_pProc )
					{
						Path* pPath=m_pProc->queuePath();
						int iPathPointNum= pPath ? pPath->getCount() : 0;
						if( iPathPointNum)
						{
							if( m_pProc->queueIsFixed() )
								m_treeProp.InsertItem( "Fixed", m_hQueue  );
							else
								m_treeProp.InsertItem( "Non Fixed", m_hQueue  );
						}
					}
				}
				
				if( m_vQueieList.size() > 0 )
				{
					for( int i=0; i<static_cast<int>(m_vQueieList.size()); i++ )
					{
						ARCVector3 v3D = m_vQueieList[i];
						CString csPoint;
						//the following procs need show the Z information
						if ( nProcType == ConveyorProc ||nProcType == StairProc || nProcType == EscalatorProc )
						{	
						
							CFloor2* pFloor = NULL;	
							csPoint.Format( "x = %.2f; y = %.2f; z = %.2f", 
											UNITSMANAGER->ConvertLength(v3D[VX]), 
											UNITSMANAGER->ConvertLength(v3D[VY]),
											float(GetZValue((float)v3D[VZ])) );
				
							pFloor = GetFloorFromZPos( static_cast<float>(v3D[VZ]) );
						

							if (pFloor)
								csPoint += _T("; Floor:") + pFloor->FloorName();
						}
						else
						{
							csPoint.Format( "x = %.2f; y = %.2f", 
											UNITSMANAGER->ConvertLength(v3D[VX]), 
											UNITSMANAGER->ConvertLength(v3D[VY]) );
						}

						m_treeProp.InsertItem( csPoint, m_hQueue  );
					}
					m_treeProp.Expand( m_hQueue, TVE_EXPAND );
					if( nProcType != LineProc)
					{
						if(m_nFixed > -1)
						{
							if( m_nFixed == 1 )
								LoadEntryTree(nProcType) ;
						}
						else
						{
							if(m_pProc != NULL && m_pProc->queueIsFixed())
							{
								LoadEntryTree(nProcType) ;					
							}
						}
					}
				}
				else if( m_pProc )
				{
					Path* pPath = m_pProc->queuePath();
					if( pPath )
					{
						m_bHavePath = true ;
						for( int i=0; i<pPath->getCount(); i++ )
						{
							Point pt = pPath->getPoint(i);
							
							CString csPoint;
							//the following floor need to show the Z information
							if ( nProcType == ConveyorProc ||nProcType == StairProc || nProcType == EscalatorProc )
							{
								csPoint.Format( "x = %.2f; y = %.2f; z = %.2f",
									UNITSMANAGER->ConvertLength(pt.getX()),
									UNITSMANAGER->ConvertLength(pt.getY()),
									(pt.getZ()) / SCALE_FACTOR );

								CFloor2* pFloor = GetPointFloor(int((pt.getZ()) / SCALE_FACTOR));
								if (pFloor)
								{
									csPoint += _T("; Floor:") + pFloor->FloorName();
								}
							}
							else
							{
								csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(pt.getX()), UNITSMANAGER->ConvertLength(pt.getY()) );
							}
							m_treeProp.InsertItem( csPoint, m_hQueue  );
						}
						m_treeProp.Expand( m_hQueue, TVE_EXPAND );
					}
					if(nProcType != LineProc && nProcType != RetailProc)
					{
						if(m_pProc->queueIsFixed())//fixed queue in proc
						{
							if(m_nFixed == 0 )// 
							{
								//non-fixed queue
							}
							else 
							{
								//fixed queue
								LoadEntryTree(nProcType);

							}
						}
						else
						{
							if(m_nFixed == 1)//fixed queue
							{
								LoadEntryTree(nProcType);
							}
						}

					}
					//if((m_pProc->queueIsFixed()|| m_nFixed == 1) && nProcType != LineProc)
					//{
					//	LoadEntryTree(nProcType) ;
					//}
				}
			}

			//Use Back up
			if (nProcType == PointProc
				|| nProcType == DepSinkProc
				|| nProcType == LineProc
				|| nProcType == RetailProc
				|| nProcType == BaggageProc
				|| nProcType == HoldAreaProc
				|| nProcType == GateProc
				|| nProcType == FloorChangeProc
				|| nProcType == MovingSideWalkProc
				|| nProcType == StandProcessor
				|| nProcType == DeiceBayProcessor)
			{
				//If Used Backup

				m_bInOutConstraintProc = true;

				if( m_pProc && m_bFirst)
				{
					m_bFirst = false;					

					Path* pPath = m_pProc->inConstraint();
					for( int i=0; i<pPath->getCount(); i++ )
					{
						Point pt = pPath->getPoint(i);

						CFloor2 *pFloor = GetPointFloor(static_cast<int>((pt.getZ()) / SCALE_FACTOR));
						if (pFloor)
						{
							ARCVector3 mVec = ARCVector3(pt.getX(), pt.getY(), pFloor->Altitude());
							m_vInConstraint.push_back(mVec);
						}
					}
				}
			}
//			
			//In Constraint
			// BarrierProc , ConveyorProc no in_constraint
			if( nProcType != BarrierProc && nProcType != ConveyorProc &&
				nProcType != ArpProcessor && nProcType != RunwayProcessor &&
				nProcType != TaxiwayProcessor && nProcType != NodeProcessor &&
				nProcType != FixProcessor && nProcType != ContourProcessor &&
				nProcType != StairProc)
			{
				
				CString csLabel = CString( "In Constraint (" ) + 	UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
				 
				m_hInConstraint = m_treeProp.InsertItem( csLabel );
				if( m_vInConstraint.size() > 0)
				{
					for( int i=0; i<static_cast<int>(m_vInConstraint.size()); i++ )
					{
						ARCVector3 v3D = m_vInConstraint[i];
						CString csPoint;
						if ( nProcType == ConveyorProc ||nProcType == StairProc || nProcType == EscalatorProc )
						{
							csPoint.Format( "x = %.2f; y = %.2f; z = %.2f", 
							UNITSMANAGER->ConvertLength(v3D[VX]), 
							UNITSMANAGER->ConvertLength(v3D[VY]),
							float(GetZValue((float)v3D[VZ])) );
							
							CFloor2* pFloor = NULL;
							pFloor = GetFloorFromZPos( static_cast<float>(v3D[VZ])  );
							if (pFloor)
							{
								csPoint += _T("; Floor:") + pFloor->FloorName();
							}
						}
						else
						{
							csPoint.Format( "x = %.2f; y = %.2f", 
							UNITSMANAGER->ConvertLength(v3D[VX]), 
							UNITSMANAGER->ConvertLength(v3D[VY]) );
						}

						m_treeProp.InsertItem( csPoint, m_hInConstraint  );
					}

					m_treeProp.Expand( m_hInConstraint, TVE_EXPAND );
				}
				else if( m_pProc )
				{
					Path* pPath = m_pProc->inConstraint();
					for( int i=0; i<pPath->getCount(); i++ )
					{
						Point pt = pPath->getPoint(i);
						CString csPoint;
						if ( nProcType == ConveyorProc ||nProcType == StairProc || nProcType == EscalatorProc )
						{
							csPoint.Format( "x = %.2f; y = %.2f; z = %.2f", 
								UNITSMANAGER->ConvertLength(pt.getX()), 
								UNITSMANAGER->ConvertLength(pt.getY()), 
								(pt.getZ()) / SCALE_FACTOR );

							CFloor2* pFloor = GetPointFloor(int((pt.getZ()) / SCALE_FACTOR));
							if (pFloor)
							{
								csPoint += _T("; Floor:") + pFloor->FloorName();
							}
						}
						else
						{
							csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(pt.getX()), UNITSMANAGER->ConvertLength(pt.getY()) );
						}
						m_treeProp.InsertItem( csPoint, m_hInConstraint  );
					}
					m_treeProp.Expand( m_hInConstraint, TVE_EXPAND );
				}
			}
			
			//Out Constraint 
			// DepSourceProc, BarrierProc and ConveyorProc no out constraint
			//if( nProcType != DepSourceProc && nProcType != BarrierProc && nProcType != ConveyorProc)
			if( nProcType != DepSourceProc && nProcType != BarrierProc && 
				nProcType != ConveyorProc && nProcType != ArpProcessor &&
				nProcType != RunwayProcessor && nProcType != TaxiwayProcessor &&
				nProcType != NodeProcessor && nProcType != FixProcessor &&
				nProcType != ContourProcessor && nProcType != StairProc)

			{
				CString sLabel;
				sLabel = CString( "Out Constraint (" ) + 
												UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + 
												CString( ")" );
				
				m_hOutConstraint = m_treeProp.InsertItem( sLabel );
				if(m_bBackup)
					m_treeProp.InsertItem( _T("Back up"), m_hOutConstraint);
				if( m_vOutConstraint.size() > 0 )
				{
					for( int i=0; i<static_cast<int>(m_vOutConstraint.size()); i++ )
					{
						ARCVector3 v3D = m_vOutConstraint[i];
						CString csPoint;
						if ( nProcType == ConveyorProc ||nProcType == StairProc || nProcType == EscalatorProc )
						{				

							csPoint.Format( "x = %.2f; y = %.2f; z = %.2f", 
							UNITSMANAGER->ConvertLength(v3D[VX]), 
							UNITSMANAGER->ConvertLength(v3D[VY]),
							float(GetZValue((float)v3D[VZ])) );

							CFloor2* pFloor = NULL;
							pFloor = GetFloorFromZPos( static_cast<float>(v3D[VZ]) );
							
							if (pFloor)
								csPoint += _T("; Floor:") + pFloor->FloorName();
						}
						else
						{
							csPoint.Format( "x = %.2f; y = %.2f", 
							UNITSMANAGER->ConvertLength(v3D[VX]), 
							UNITSMANAGER->ConvertLength(v3D[VY]) );
						}

						m_treeProp.InsertItem( csPoint, m_hOutConstraint  );
					}

					m_treeProp.Expand( m_hOutConstraint, TVE_EXPAND );
				}
				else if( m_pProc )
				{
					Path* pPath = m_pProc->outConstraint();
					for( int i=0; i<pPath->getCount(); i++ )
					{
						Point pt = pPath->getPoint(i);
						CString csPoint;
						if ( nProcType == ConveyorProc ||nProcType == StairProc || nProcType == EscalatorProc )
						{
							csPoint.Format( "x = %.2f; y = %.2f; z = %.2f", UNITSMANAGER->ConvertLength(pt.getX()), 
							UNITSMANAGER->ConvertLength(pt.getY()), (pt.getZ()) / SCALE_FACTOR );
							
							CFloor2* pFloor = GetPointFloor(int((pt.getZ()) / SCALE_FACTOR));
							if (pFloor)
							{
								csPoint += _T("; Floor:") + pFloor->FloorName();
							}
						}
						else
						{
							csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(pt.getX()), UNITSMANAGER->ConvertLength(pt.getY()) );
						}

						m_treeProp.InsertItem( csPoint, m_hOutConstraint  );
					}
					m_treeProp.Expand( m_hOutConstraint, TVE_EXPAND );
				}
			}

			if (nProcType == GateProc)
			{
				GateProcessor* pGate = (GateProcessor*)m_pProc;
				if (pGate)
				{
					GateType nGateType = pGate->getGateType();
					if (nGateType == NoGateType)
					{
						pGate->setGateType(ArrGate);
					}
					m_emGateType = pGate->getGateType();
				}
				CString strTemp = _T("");
				CString strGateType = _T("");
				switch(m_emGateType)
				{
				case ArrGate:
					strTemp = _T("ArrGate");
					break;
				case DepGate:
					strTemp = _T("DepGate");
					break;
				default:
					break;
				}
				strGateType.Format(_T("Gate Type : %s"),strTemp);
				m_hGateType = m_treeProp.InsertItem(strGateType);
			}
			if (nProcType == RetailProc)
			{
				RetailProcessor* pRetail = (RetailProcessor*)m_pProc;

				CString csLable	= CString(_T("store areas (")) + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
				m_hStoreArea = m_treeProp.InsertItem(csLable);

				if (m_vStoreArea.size() > 0)
				{
					for( int i=0; i<static_cast<int>(m_vStoreArea.size()); i++ )
					{
						ARCVector3 v3D = m_vStoreArea[i];
						CString csPoint;
						csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(v3D.n[VX]), UNITSMANAGER->ConvertLength(v3D.n[VY]) );
						m_treeProp.InsertItem( csPoint, m_hStoreArea  );
					}
				}
				else if (pRetail)
				{
					const Pollygon& storeArea = pRetail->GetStoreArea();
					for( int i=0; i<storeArea.getCount(); i++ )
					{
						Point pt = storeArea.getPoint(i);
						CString csPoint;
						csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(pt.getX()), UNITSMANAGER->ConvertLength(pt.getY()) );
						m_treeProp.InsertItem( csPoint, m_hStoreArea  );
					}
				}
				m_treeProp.Expand( m_hStoreArea, TVE_EXPAND );

				CString strTemp = _T("Shop Section");
				if (pRetail)
				{
					m_emRetailType = pRetail->GetRetailType();
					switch(pRetail->GetRetailType())
					{
					case RetailProcessor::Retial_Shop_Section:
						strTemp = _T("Shop Section");
						break;
					case RetailProcessor::Retial_By_Pass:
						strTemp = _T("By Pass");
						break;
					case RetailProcessor::Retial_Check_Out:
						strTemp = _T("Check Out");
						break;
					default:
						break;
					}
				}
				else
				{
					m_emRetailType = RetailProcessor::Retial_Shop_Section;
				}
				CString strRetailType;
				strRetailType.Format(_T("Retail Type : %s"),strTemp);
				m_hRetailType = m_treeProp.InsertItem(strRetailType);
			}
			if( nProcType == BaggageProc  )
			{	
				//BaggageProcessor* pBag = ( BaggageProcessor* )m_pProc;
				CString csLabel = CString( "Barrier (" ) + 	UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
				m_hBaggageProcBarrier = m_treeProp.InsertItem( csLabel  );
				
				if( m_vBaggageBarrier.size() > 0 )
				{
					for( int i=0; i<static_cast<int>(m_vBaggageBarrier.size()); i++ )
					{
						ARCVector3 v3D = m_vBaggageBarrier[i];
						CString csPoint;
						csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(v3D[VX]), UNITSMANAGER->ConvertLength(v3D[VY]) );
						m_treeProp.InsertItem( csPoint, m_hBaggageProcBarrier  );
					}
				}
				else if( m_pProc )
				{
					Path* pPath = ((BaggageProcessor*)m_pProc)->GetBarrier()->serviceLocationPath();
					for( int i=0; i<pPath->getCount(); i++ )
					{
						Point pt = pPath->getPoint(i);
						CString csPoint;
						csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(pt.getX()), UNITSMANAGER->ConvertLength(pt.getY()) );
						m_treeProp.InsertItem( csPoint, m_hBaggageProcBarrier  );
					}
				}
				
				
				csLabel = CString( "Baggage Moving Path (" ) + 	UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
				m_hBaggageMovingPath = m_treeProp.InsertItem( csLabel  );
				
				if( m_vBaggagMovingPath.size() > 0 )
				{
					for( int i=0; i<static_cast<int>(m_vBaggagMovingPath.size()); i++ )
					{
						ARCVector3 v3D = m_vBaggagMovingPath[i];
						CString csPoint;
						csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(v3D[VX]), UNITSMANAGER->ConvertLength(v3D[VY]) );
						m_treeProp.InsertItem( csPoint, m_hBaggageMovingPath  );
					}
				}
				else if( m_pProc )
				{
					Path* pPath = ((BaggageProcessor*)m_pProc)->GetPathBagMovingOn();
					for( int i=0; i<pPath->getCount(); i++ )
					{
						Point pt = pPath->getPoint(i);
						CString csPoint;
						csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(pt.getX()), UNITSMANAGER->ConvertLength(pt.getY()) );
						m_treeProp.InsertItem( csPoint, m_hBaggageMovingPath  );
					}
				}
				
				m_treeProp.Expand( m_hBaggageProcBarrier, TVE_EXPAND );
				m_treeProp.Expand( m_hBaggageMovingPath, TVE_EXPAND );
				
				
			}	

			//Change by Mark Chen
			//if( nProcType == ConveyorProc || nProcType == StairProc )
			if( nProcType == ConveyorProc || nProcType == StairProc || nProcType == EscalatorProc )
			{
				if( m_pProc )
				{					
					if( nProcType == ConveyorProc )
					{
						m_dConveyWidth = ((Conveyor*)m_pProc)->GetWidth();
					}
					else 
					{
						m_dConveyWidth = ((Stair*)m_pProc)->GetWidth();
					}
				}

				CString csLabel = "Width ("  + 	UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + ")" ;
				CString sValue ;
				sValue.Format(" (%.2f)",UNITSMANAGER->ConvertLength( m_dConveyWidth ) );
				csLabel += sValue;
				m_hWidthItem  = m_treeProp.InsertItem( csLabel );												
			}

			// length & angle
			if( nProcType == ConveyorProc )
			{
				m_dProcessorLength = 0.0;
				m_dPathAngle	   = 0.0;

				CTermPlanDoc* _pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
				std::vector<double> vRealAltitude;
				_pDoc->GetCurModeFloorsRealAltitude(vRealAltitude);
				if( m_vServiceLocation.size()>0 )
				{
					Path _tmpPath;
					_tmpPath.init( m_vServiceLocation.size() );
					
					for( int i=0; i<static_cast<int>(m_vServiceLocation.size()); i++ )
					{
						ARCVector3 v3D = m_vServiceLocation[i];
						if( !m_bZPosHasBeenChanged )
							_tmpPath[i] = Point( v3D[VX], v3D[VY],m_stationServiceLocation[VZ] );
						else
							_tmpPath[i] = Point( v3D[VX], v3D[VY],v3D[VZ]);
					}
					m_dProcessorLength = _tmpPath.getFactLength(vRealAltitude);
					m_dPathAngle	   = _tmpPath.getMaxAngle(vRealAltitude);
				}

				else if( m_pProc )
				{
					Path* _pServicPath = m_pProc->serviceLocationPath();
					assert( _pServicPath );
					m_dProcessorLength = _pServicPath->getFactLength(vRealAltitude);
					m_dPathAngle	   = _pServicPath->getMaxAngle(vRealAltitude);
				}

				CString _strLengthItem;
				_strLengthItem.Format("Length (%s):(%.2f)", UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits() ),
															UNITSMANAGER->ConvertLength( m_dProcessorLength ) );
				m_hLengthItem = m_treeProp.InsertItem( _strLengthItem );

				CString _strAngleItem;
				_strAngleItem.Format("Slope (deg): %.2f ", m_dPathAngle );
				m_hAngleItem = m_treeProp.InsertItem( _strAngleItem );
			}
			// deicbay winspan && length
			if(nProcType == DeiceBayProcessor){
				if(m_pProc){
					m_dConveyWidth = ((DeiceBayProc*)m_pProc)->GetWinspan();
					m_dProcessorLength = ((DeiceBayProc*)m_pProc)->GetLength();					

				}

				CString csLabel = "Width ("  + 	UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + ")" ;
				CString sValue ;
				sValue.Format(" (%.2f)",UNITSMANAGER->ConvertLength( m_dConveyWidth ) );
				csLabel += sValue;
				m_hWidthItem  = m_treeProp.InsertItem( csLabel );												

				CString _strLengthItem;
				_strLengthItem.Format("Length (%s) (%.2f)", UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits() ),
					UNITSMANAGER->ConvertLength( m_dProcessorLength ) );
				m_hLengthItem = m_treeProp.InsertItem( _strLengthItem );

			}
		}

		//	Add by Kevin
		//	Last Modified: 2005-4-13 16:52
		//	Width
		if (nProcType == RunwayProcessor || nProcType == TaxiwayProcessor)
		{
			CString strLabel;
			if (m_pProc)
			{
				if (nProcType == RunwayProcessor)
					strLabel.Format(_T("Width ("+UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits() )+")(%.0f)"), UNITSMANAGER->ConvertLength(((RunwayProc*)m_pProc)->GetWidth()));
				else
					strLabel.Format(_T("Width ("+UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits() )+")(%.0f)"), UNITSMANAGER->ConvertLength(((TaxiwayProc*)m_pProc)->GetWidth()));
			}
			else
				strLabel.Format(_T("Width ("+UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits() )+")(%.0f)"), UNITSMANAGER->ConvertLength(m_dWidth));
			m_hWidth = m_treeProp.InsertItem(strLabel);
		}

		//	Runway number
		/*
		if (nProcType == RunwayProcessor)
				{
					CString strLabel = _T("Runway Number");
					 m_hRunwayNumber = m_treeProp.InsertItem(strLabel);
				}*/
		

		//	Threshold
		if (nProcType == RunwayProcessor)
		{
			CString strLabel = _T("Threshold (")+UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits() )+")";
			m_hThreshold = m_treeProp.InsertItem(strLabel);

			if (m_pProc)
			{
				strLabel.Format(_T("(%.0f)"), UNITSMANAGER->ConvertLength(((RunwayProc*)m_pProc)->GetThreshold1()));
				m_hThresholdA = m_treeProp.InsertItem(strLabel, m_hThreshold);
				strLabel.Format(_T("(%.0f)"), UNITSMANAGER->ConvertLength(((RunwayProc*)m_pProc)->GetThreshold2()));
				m_hThresholdB = m_treeProp.InsertItem(strLabel, m_hThreshold);
			}
			else
			{
				strLabel.Format(_T("(%.0f)"), UNITSMANAGER->ConvertLength(m_dThreshold));
				m_hThresholdA = m_treeProp.InsertItem(strLabel, m_hThreshold);
				strLabel.Format(_T("(%.0f)"), UNITSMANAGER->ConvertLength(m_dThreshold2));
				m_hThresholdB = m_treeProp.InsertItem(strLabel, m_hThreshold);
			}
			
			m_treeProp.Expand( m_hThreshold, TVE_EXPAND );
		}

		//	lower limit
		if (nProcType == FixProcessor)
		{
			CString strLabel;
			if (m_pProc)
				strLabel.Format(_T("Lower Limit (%s)(%.0f)"),UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits()), UNITSMANAGER->ConvertLength(((FixProc*)m_pProc)->GetLowerLimit()));
			else
				strLabel.Format(_T("Lower Limit (%s)(%.0f)"),UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits()), UNITSMANAGER->ConvertLength(m_dLowerLimit));
			m_hLowerLimit = m_treeProp.InsertItem(strLabel);
		}

		//	upper limit
		if (nProcType == FixProcessor)
		{
			CString strLabel;
			if (m_pProc)
				strLabel.Format(_T("Upper Limit (%s)(%.0f)"),UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits()), UNITSMANAGER->ConvertLength(((FixProc*)m_pProc)->GetUpperLimit()));
			else
				strLabel.Format(_T("Upper Limit (%s)(%.0f)"),UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits()), UNITSMANAGER->ConvertLength(m_dUpperLimit));
			m_hUpperLimit = m_treeProp.InsertItem(strLabel);
		}

		//	Automatically number
		if (nProcType == NodeProcessor /*|| nProcType == DeiceBayProcessor ||	nProcType == FixProcessor*/)
		{
			CString strLabel = _T("Automatically Number");
			if (m_pProc)
			{
				if (nProcType == NodeProcessor)
					strLabel.Format(_T("Automatically Number(%d)"), ((NodeProc*)m_pProc)->GetNumber());
				else if (nProcType == DeiceBayProcessor)
					strLabel.Format(_T("Automatically Number(%d)"), ((DeiceBayProc*)m_pProc)->GetNumber());
				else
					strLabel.Format(_T("Automatically Number(%d)"), ((FixProc*)m_pProc)->GetNumber());
			}
			else
				strLabel.Format(_T("Automatically Number(%d)"), m_nAutoNumber);
			m_hAutomaticallyNumber = m_treeProp.InsertItem(strLabel);
		}

		//	marking
		if (nProcType == RunwayProcessor || nProcType == TaxiwayProcessor)
		{
			CString strLabel = _T("Marking");
			m_hMarking = m_treeProp.InsertItem(strLabel);
			if (m_pProc)
			{
				if (nProcType == RunwayProcessor)
				{
					strLabel = ((RunwayProc*)m_pProc)->GetMarking1().c_str();
					m_hMarkingA = m_treeProp.InsertItem(strLabel, m_hMarking);
					strLabel = ((RunwayProc*)m_pProc)->GetMarking2().c_str();
					m_hMarkingB = m_treeProp.InsertItem(strLabel, m_hMarking);
				}
				else
				{
					strLabel = ((TaxiwayProc*)m_pProc)->GetMarking().c_str();
					m_hMarkingA =  m_treeProp.InsertItem(strLabel, m_hMarking);
				}
			}
			else
			{
				strLabel = m_strMarking;
				m_hMarkingA =  m_treeProp.InsertItem(strLabel, m_hMarking);
				if (nProcType == RunwayProcessor)
				{
					strLabel = m_strMarking2;
					m_hMarkingB = m_treeProp.InsertItem(strLabel, m_hMarking);
				}
			}
			m_treeProp.Expand( m_hMarking, TVE_EXPAND );
		}

		//contours
		if (nProcType == ContourProcessor)
		{
			CString strLabel;
			if (m_pProc)
			{
				strLabel.Format(_T("Contour Height(m)(%.2f)"),UNITSMANAGER->ConvertLength(((ContourProc *)m_pProc)->getHeight()));
			}
			else
			{
				strLabel.Format(_T("Contour Height(m)(%.2f)"),UNITSMANAGER->ConvertLength(m_dHeight));
			}

			m_hHeight = m_treeProp.InsertItem(strLabel);
		}		
	}
	else if(nProcType == HoldProcessor){
		CString strUnitLabel = CString("(") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString(")");

		HoldProc * pHold = (HoldProc*)m_pProc;
		//fix
		CString strLabel;
		if(pHold)
		{
			if(pHold->getFix())
				m_strFix = pHold->getFix()->getIDName();
		}
		strLabel.Format("Waypoint:%s",m_strFix);
		m_hFix = m_treeProp.InsertItem(strLabel);	
		//inbound degree
		if(pHold) m_nInBoundDegree  = pHold->getInBoundDegree();
		strLabel.Format("InBound Course: %d" , m_nInBoundDegree );
		m_hInBound  = m_treeProp.InsertItem(strLabel);
		//outbound leg
		if(pHold){
			m_dOutBoundmin = pHold->getOutBoundLegMin();
			m_dOutBoundnm = pHold->getOutBoundLegNm();
		}
		strLabel.Format("OutBound Leg");
		HTREEITEM outbitem = m_treeProp.InsertItem(strLabel);
		strLabel.Format("%.2f %s",UNITSMANAGER->ConvertLength(m_dOutBoundnm),strUnitLabel);
		m_hOutBoundLegnm = m_treeProp.InsertItem(strLabel,outbitem);

		strLabel.Format("%0.2f (min)",m_dOutBoundmin);
		m_hOutBoundLegmin = m_treeProp.InsertItem(strLabel,outbitem);

		m_treeProp.Expand(outbitem,TVE_EXPAND);
		//right or left turn
		if(pHold) m_bRight = pHold->isRightTurn();
		CString rightorleft = m_bRight?"Right":"Left";
		strLabel.Format("%s Turn",rightorleft);
		m_hRight = m_treeProp.InsertItem(strLabel);
		//max altitude

		if(pHold) m_dmaxAlt = pHold->getMaxAltitude();
		strLabel.Format("Max Altitude%s : %.2f", strUnitLabel, UNITSMANAGER->ConvertLength(m_dmaxAlt));
		m_hMaxAlt = m_treeProp.InsertItem(strLabel);
		//min altitude 
		if(pHold) m_dminAlt = pHold->getMinAltitude();
		strLabel.Format("Min Altitude%s : %.2f", strUnitLabel, UNITSMANAGER->ConvertLength(m_dminAlt));
		m_hMinAlt = m_treeProp.InsertItem(strLabel);
	}
	else if(nProcType == SectorProcessor){
		CString strUnitLabel = CString("(") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString(")");
		CString strlable;
		SectorProc * pSector = (SectorProc*)m_pProc;
		if(pSector) m_dminAlt = pSector->getMinAlt();
		strlable.Format("Max Altitude%s : %.2f", strUnitLabel, UNITSMANAGER->ConvertLength(m_dminAlt));
		m_hMinAlt = m_treeProp.InsertItem(strlable);
		//min altitude 
		if(pSector) m_dmaxAlt = pSector->getMaxAlt();
		strlable.Format("Min Altitude%s : %.2f", strUnitLabel, UNITSMANAGER->ConvertLength(m_dmaxAlt));
		m_hMaxAlt = m_treeProp.InsertItem(strlable);
		strlable =_T("Vertexs:");
		m_hServiceLocation = m_treeProp.InsertItem( strlable );
		
		if( m_vServiceLocation.size() > 0 )
		{
			for( int i=0; i<static_cast<int>(m_vServiceLocation.size()); i++ )
			{
				CString cstrvexi ;
				cstrvexi.Format("V%d",i);
				HTREEITEM hvexi = m_treeProp.InsertItem(cstrvexi,m_hServiceLocation);
				ARCVector3 v3D = m_vServiceLocation[i];
				CString csPoint;
				csPoint.Format( "x = %.2f; y = %.2f;", 
						UNITSMANAGER->ConvertLength(v3D[VX]), 
						UNITSMANAGER->ConvertLength(v3D[VY]) );		
				m_treeProp.InsertItem( csPoint, hvexi);
				CString strlatlong,strlat,strlong;
				Convent_local_latlong(v3D[VX],v3D[VY],strlat,strlong);
				strlatlong.Format("Latitude= %s ;Longitude= %s",strlong,strlat);
				m_treeProp.InsertItem( strlatlong, hvexi);
				m_treeProp.Expand( hvexi , TVE_EXPAND );
			}
			m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND );
		}else if(pSector){
			for( int i=0; i<pSector->serviceLocationPath()->getCount(); i++ )
			{
				CString cstrvexi ;
				cstrvexi.Format("V%d",i);
				HTREEITEM hvexi = m_treeProp.InsertItem(cstrvexi,m_hServiceLocation);
				ARCVector3 v3D = pSector->serviceLocationPath()->getPoint(i);
				CString csPoint;
				csPoint.Format( "x = %.2f; y = %.2f;", 
					UNITSMANAGER->ConvertLength(v3D[VX]), 
					UNITSMANAGER->ConvertLength(v3D[VY]) );		
				m_treeProp.InsertItem( csPoint, hvexi);
				CString strlatlong,strlat,strlong;
				Convent_local_latlong(v3D[VX],v3D[VY],strlat,strlong);
				strlatlong.Format("Latitude= %s ;Longitude= %s",strlong,strlat);
				m_treeProp.InsertItem( strlatlong, hvexi);
				m_treeProp.Expand( hvexi , TVE_EXPAND );
			}
			m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND );
		}
		if(pSector) m_dbandinterval = pSector->getAltBandInterval();
		CString cstrbandint;
		cstrbandint.Format("Altitude Band Interval:%d",(int)m_dbandinterval);
		m_hBandInterval = m_treeProp.InsertItem(cstrbandint);
	}
	else if (nProcType== BridgeConnectorProc)
	{
		CString sLabel;
		sLabel.Format("Connect points: %d",m_nBridgeConnectPointCount);
		HTREEITEM hItem = m_treeProp.InsertItem(sLabel);
		m_dProcessorLength = 10*SCALE_FACTOR;
		m_dConveyWidth = SCALE_FACTOR;

		for (int idx =0; idx < m_nBridgeConnectPointCount; idx++)
		{
			InitBridgeConnectPointDefaultValue(idx,hItem);
			m_treeProp.Expand( hItem, TVE_EXPAND );
		}
		
	}	
	else //is integrated station 
	{
		m_hStation = m_treeProp.InsertItem( "Station" );
		CString sUnit = UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );;
		CString sDisplay= CString("Service Location (") + sUnit;
		
		m_hServiceLocation = m_treeProp.InsertItem( sDisplay, m_hStation);

		CString csPoint;
		//		csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(m_stationServiceLocation.X()),UNITSMANAGER->ConvertLength(m_stationServiceLocation.Y()));
		csPoint.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(m_stationServiceLocation[VX] ),UNITSMANAGER->ConvertLength(m_stationServiceLocation[VY]));
		m_treeProp.InsertItem( csPoint, m_hServiceLocation  );
		m_treeProp.Expand( m_hServiceLocation, TVE_EXPAND );		
		
		sDisplay.Format("Orientation (deg)   ( %d )  ",m_tempStation.GetOrientation() );
		m_hOrientation = m_treeProp.InsertItem(sDisplay , m_hStation );
		
		
		
		//Pre-Boarding Holding Area
		m_hStationPreBordingAre=m_treeProp.InsertItem("Platform ("+sUnit,m_hStation);
		
		
		m_hBordingAreaInconstrain=m_treeProp.InsertItem("In Constraint ("+sUnit,m_hStationPreBordingAre);
		m_treeProp.SetItemData(m_hBordingAreaInconstrain,6);
		int pathPointCount=getStationLayout()->GetPreBoardingArea()->inConstraintLength();
		for( int j=0; j<pathPointCount; j++ )
		{							
			csPoint.Format( "x = %.2f; y = %.2f", 
				UNITSMANAGER->ConvertLength(getStationLayout()->GetPreBoardingArea()->inConstraint()->getPoint(j).getX()) ,
				UNITSMANAGER->ConvertLength(getStationLayout()->GetPreBoardingArea()->inConstraint()->getPoint(j).getY()) );
			
			m_treeProp.InsertItem( csPoint, m_hBordingAreaInconstrain);
		}
		m_treeProp.Expand( m_hBordingAreaInconstrain, TVE_EXPAND );
		
		sDisplay = "Platform Length ("+sUnit;
		CString sFormat ;
		sFormat.Format(" ( %.2f )", UNITSMANAGER->ConvertLength(getStationLayout()->GetPlatformLength()) );
		sDisplay += sFormat;
		m_hPlatFormLength = m_treeProp.InsertItem( sDisplay, m_hStationPreBordingAre);
		
		sFormat.Format(" ( %.2f )", UNITSMANAGER->ConvertLength(getStationLayout()->GetPlatformWidth()) );
		sDisplay = "Platform Width  ("+sUnit;
		sDisplay += sFormat;
		m_hPlatFormWidth= m_treeProp.InsertItem( sDisplay, m_hStationPreBordingAre);
		
		
		
		
		
		//cars
		m_hCars = m_treeProp.InsertItem( "Cars " ,m_hStation );
		int nCarNum= getStationLayout()->GetCarNumber();
		sDisplay.Format( "Car number  ( %d ) ",nCarNum );
		m_hCarNum = m_treeProp.InsertItem( sDisplay , m_hCars );
		
		
		sDisplay = "Car Length ("+sUnit;
		sFormat.Format( "   ( %.2f) ", UNITSMANAGER->ConvertLength(getStationLayout()->GetCarLength()) );
		sDisplay +=sFormat;
		m_hCarLength  = m_treeProp.InsertItem( sDisplay , m_hCars );
		
		sDisplay = "Car Width  ("+sUnit;
		sFormat.Format( "   ( %.2f ) ",UNITSMANAGER->ConvertLength(getStationLayout()->GetCarWidth())  );
		sDisplay +=sFormat;
		m_hCarWidth= m_treeProp.InsertItem( sDisplay , m_hCars );
		
		sDisplay.Format( "Car Capacity  ( %d )", getStationLayout()->GetCarCapacity() );
		m_hCarCapacity = m_treeProp.InsertItem( sDisplay ,m_hCars );
		
		HTREEITEM tempItem = m_treeProp.InsertItem( "Entry Doors " , m_hCars);
		sDisplay.Format( "Door Number  ( %d )", getStationLayout()->GetEntryDoorNumber() );
		m_hEntryDoorNum = m_treeProp.InsertItem( sDisplay, tempItem);
		m_treeProp.Expand( tempItem ,TVE_EXPAND  );
		
		
		HTREEITEM tempItemExit = m_treeProp.InsertItem( "Exit Doors " , m_hCars);
		
		sDisplay.Format( "Door Number  ( %d )", getStationLayout()->GetExitDoorNumber() );
		m_hExitDoorNum = m_treeProp.InsertItem( sDisplay, tempItemExit);
		
		if( getStationLayout()->GetExitDoorType() )
			sDisplay  = "Platform Side  ";
		else
			sDisplay = "Non Platform Side";
		m_hExitDoorType = m_treeProp.InsertItem( sDisplay ,tempItemExit );
		
		m_treeProp.Expand( m_hStation ,TVE_EXPAND  );
		m_treeProp.Expand( m_hStationPreBordingAre ,TVE_EXPAND  );
		m_treeProp.Expand( m_hBordingAreaInconstrain ,TVE_EXPAND  );
		m_treeProp.Expand( m_hCars ,TVE_EXPAND  );
		m_treeProp.Expand( tempItemExit ,TVE_EXPAND  );
		
		
		if ( m_iSelectType == 1 )
		{
			m_treeProp.SelectItem( m_hPlatFormWidth );
		}
		else if ( m_iSelectType == 2 )
		{
			m_treeProp.SelectItem( m_hPlatFormLength );
		}
		else if ( m_iSelectType == 3 )
		{
			m_treeProp.SelectItem( m_hCarWidth );
			
		}
		else if ( m_iSelectType == 4 )
		{
			m_treeProp.SelectItem( m_hCarLength );
			
		}
		else if ( m_iSelectType == 5)
		{
			m_treeProp.SelectItem( m_hCarNum );
			
		}
		else if( m_iSelectType == 6 )
		{
			m_treeProp.SelectItem( m_hCarCapacity );
			
		}
		else if( 	m_iSelectType == 7)
		{
			m_treeProp.SelectItem( m_hEntryDoorNum );
			
		}
		else if ( 	m_iSelectType == 8 )
		{
			m_treeProp.SelectItem( m_hExitDoorNum );
			
		}
		else if ( m_iSelectType == 9)
		{
			m_treeProp.SelectItem( m_hOrientation );	
		}
		else
		{
			m_treeProp.SelectItem( m_hBordingAreaInconstrain );
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	// show or hide check box
	m_btnOffgate.ShowWindow( SW_HIDE );	
//	m_butACStandGate.ShowWindow( SW_HIDE );
	m_btnEmergent.ShowWindow( SW_HIDE );
	

	// set emergent flag
	if( nProcType == GateProc ||
		nProcType == LineProc ||
		nProcType == RetailProc ||
		nProcType == PointProc )
	{
		m_btnEmergent.ShowWindow( SW_SHOW );
		
		BOOL bFlag = m_pProc ? m_pProc->getEmergentFlag() : FALSE ;

		m_btnEmergent.SetCheck( (int) bFlag );
	}
	
}


InputTerminal* CProcPropDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
    return (InputTerminal*)&pDoc->GetTerminal();
}

// base on what level 1 selection
void CProcPropDlg::OnDropdownComboLevel2() 
{
	// TODO: Add your control notification handler code here
	m_comboLevel2.ResetContent();

	CString csLevel1;
	m_comboLevel1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;

	ProcessorID id;
	id.SetStrDict( GetInputTerminal()->inStrDict );
	id.setID( csLevel1 );
	StringList strList;
	GetInputTerminal()->GetProcessorList()->getMemberNames( id, strList, -1 );
	for( int i=0; i<strList.getCount(); i++ )
		m_comboLevel2.AddString( strList.getString( i ) );
}

// base on what level 1-2 selection
void CProcPropDlg::OnDropdownComboLevel3() 
{
	// TODO: Add your control notification handler code here
	m_comboLevel3.ResetContent();

	CString csLevel1;
	m_comboLevel1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_comboLevel2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;

	CString csLevel12 = csLevel1 + "-" + csLevel2;
	ProcessorID id;
	id.SetStrDict( GetInputTerminal()->inStrDict );
	id.setID( csLevel12 );
	StringList strList;
	GetInputTerminal()->GetProcessorList()->getMemberNames( id, strList, -1 );
	for( int i=0; i<strList.getCount(); i++ )
		m_comboLevel3.AddString( strList.getString( i ) );	
}

// base on what level 1-2-3 selection
void CProcPropDlg::OnDropdownComboLevel4() 
{
	// TODO: Add your control notification handler code here
	m_comboLevel4.ResetContent();

	CString csLevel1;
	m_comboLevel1.GetWindowText( csLevel1 );
	if( csLevel1.IsEmpty() )
		return;
	CString csLevel2;
	m_comboLevel2.GetWindowText( csLevel2 );
	if( csLevel2.IsEmpty() )
		return;
	CString csLevel3;
	m_comboLevel3.GetWindowText( csLevel3 );
	if( csLevel3.IsEmpty() )
		return;

	CString csLevel123 = csLevel1 + "-" + csLevel2 + "-" + csLevel3;
	ProcessorID id;
	id.SetStrDict( GetInputTerminal()->inStrDict );
	id.setID( csLevel123 );
	StringList strList;
	GetInputTerminal()->GetProcessorList()->getMemberNames( id, strList, -1 );
	for( int i=0; i<strList.getCount(); i++ )
		m_comboLevel4.AddString( strList.getString( i ) );	
	
}

void CProcPropDlg::OnKillfocusComboLevel1() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	PreProcessName();
	
	if( m_bTypeCanChange )
	{
		SetTypeByLevel1();	
		int nSelType = m_comboType.GetCurSel();
		int nProcType = nSelType >=0 ? m_comboType.GetItemData(nSelType) : -1;
		if( nProcType == IntegratedStationProc )
		{
			InitStationLayout();
			LoadTree();
		}

	}

}

BOOL CProcPropDlg::IsNameChanged(CString newName)
{

	if(m_pProc==NULL)
			return true;

	if(newName.IsEmpty())
			return true;

	ProcessorID id;
	id.SetStrDict( GetInputTerminal()->inStrDict );
	id.setID( newName );

	if(!(m_pProc->getID()->compare(id)))
		return false;
	
	return true;

}
// base on the string in the combo level1 
void CProcPropDlg::SetTypeByLevel1()
{
	if( !m_csLevel1.IsEmpty() )
	{
		ProcessorID id;
		id.SetStrDict( GetInputTerminal()->inStrDict );
		id.setID( m_csLevel1 );
		StringList strList;
		GroupIndex groupIdx = GetInputTerminal()->GetProcessorList()->getGroupIndex( id );
		if( groupIdx.start >= 0 )
		{
			Processor* pProc = GetInputTerminal()->GetProcessorList()->getProcessor( groupIdx.start );
			int nTypeIdx = pProc->getProcessorType();
			for( int i=0; i<m_comboType.GetCount(); i++ )
			{
				if( m_comboType.GetItemData(i) == nTypeIdx )
					m_comboType.SetCurSel( i );
			}
		
			m_comboType.EnableWindow( FALSE );
			LoadTree();

			return;
		}

	}


	if (m_nForceProcType == -1)
		m_comboType.EnableWindow();
	LoadTree();
}


void CProcPropDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	m_hRClickItem = NULL;

	CRect rectTree;
	m_treeProp.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_treeProp.SetFocus();
	CPoint pt = point;
	m_treeProp.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem = m_treeProp.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}

	if (m_hRClickItem==NULL)
		return;

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu=NULL;
    
   if(m_hRClickItem == m_EntryPoint)
   {
	   pMenu = menuPopup.GetSubMenu(89) ;
      pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
	  return ;
   }
   if(m_EntryPoint != NULL)
   {
		HTREEITEM child =  m_treeProp.GetChildItem(m_EntryPoint) ;
		while(child != NULL)
		{
				if(child == m_hRClickItem)
				{
					pMenu = menuPopup.GetSubMenu(90) ;
					HTREEITEM sel = m_hRClickItem ;
					//if(sel == NULL )
					//{
						// pMenu->EnableMenuItem(ID_DEL_ENTRY_POINT,MF_GRAYED) ;
					//}
					//else
					//{
						// pMenu->EnableMenuItem(ID_DEL_ENTRY_POINT,MF_ENABLED) ;
					//}
					pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
					return ;
				}
				child = m_treeProp.GetNextSiblingItem(child) ;
		}
   }

	int nSelType = m_comboType.GetCurSel();
	int nProcType = nSelType >=0 ? m_comboType.GetItemData(nSelType) : -1;

	//Change by Mark Chen
	//if( nProcType == ConveyorProc || nProcType == StairProc )
	if( nProcType == ConveyorProc || nProcType == StairProc || nProcType == EscalatorProc || nProcType == DeiceBayProcessor )
	{
		pMenu = menuPopup.GetSubMenu( 5 );
		if( m_treeProp.GetParentItem( m_hRClickItem ) != NULL )
		{
			m_treeProp.SelectItem( NULL );
			return;
		}

		if( m_hRClickItem != m_hQueue  )
			pMenu->EnableMenuItem( ID_PROCPROP_TOGGLEDQUEUEFIXED, MF_GRAYED );
		m_treeProp.SelectItem( m_hRClickItem );

		if( m_hRClickItem == m_hWidthItem )
		{
			pMenu = menuPopup.GetSubMenu( 42 );
		}
		else if( m_hRClickItem == m_hLengthItem )
		{
			pMenu = menuPopup.GetSubMenu( 44 );
		}
		else if( m_hRClickItem == m_hAngleItem )
		{
			pMenu = menuPopup.GetSubMenu( 45 );
		}
		else if( m_hRClickItem == m_hServiceLocation )
		{
			pMenu->EnableMenuItem( ID_PROCPROP_DEFINE_Z_POS ,MF_ENABLED );
		}
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
	}
	else if(nProcType==Elevator)
	{
		if(m_hRClickItem)
		{
			m_treeProp.SelectItem(m_hRClickItem);
			
			if(m_hRClickItem==m_hServiceLocation)
			{
				pMenu=menuPopup.GetSubMenu(5);
				pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);
			}
			else if(m_hRClickItem==m_hWaitAreaLength)
				pMenu=menuPopup.GetSubMenu(34);
			else if(m_hRClickItem==m_hWaitAreaWidth)
				pMenu=menuPopup.GetSubMenu(35);
			else if(m_hRClickItem==m_hLiftNOL)
				pMenu=menuPopup.GetSubMenu(36);
			else if(m_hRClickItem==m_hLiftLength)
				pMenu=menuPopup.GetSubMenu(37);
			else if(m_hRClickItem==m_hLiftWidth)
				pMenu=menuPopup.GetSubMenu(38);
			else if(m_hRClickItem==m_hMinFloor)
				pMenu=menuPopup.GetSubMenu(39);
			else if(m_hRClickItem==m_hMaxFloor)
				pMenu=menuPopup.GetSubMenu(40);
			else if(m_treeProp.GetParentItem(m_hRClickItem)==m_hWaitAreaPos)
				pMenu=menuPopup.GetSubMenu(41);
			else
				return;
			pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
			
		}
	}
	else if(nProcType==IntegratedStationProc)
	{
		if(m_hRClickItem)
		{
			m_treeProp.SelectItem( m_hRClickItem );
			m_hPickupFromMapItem = m_hRClickItem;

			if( m_hRClickItem == m_hBordingAreaInconstrain )
			{
				pMenu = menuPopup.GetSubMenu( 10 );
			}
			else if ( m_hRClickItem == m_hPlatFormWidth )
			{
				pMenu = menuPopup.GetSubMenu( 15 );
			}
			else if ( m_hRClickItem == m_hPlatFormLength )
			{
				pMenu = menuPopup.GetSubMenu( 16 );
			}
			else if ( m_hRClickItem == m_hCarWidth )
			{
				pMenu = menuPopup.GetSubMenu( 17 );
			}
			else if ( m_hRClickItem == m_hCarLength )
			{
				pMenu = menuPopup.GetSubMenu( 18 );
			}
			else if ( m_hRClickItem == m_hCarNum )
			{
				pMenu = menuPopup.GetSubMenu( 14 );
			}
			else if( m_hRClickItem == m_hCarCapacity )
			{
				pMenu = menuPopup.GetSubMenu( 13 );
			}
			else if( m_hRClickItem == m_hEntryDoorNum )
			{
				pMenu = menuPopup.GetSubMenu( 19 );
			}
			else if ( m_hRClickItem == m_hExitDoorNum )
			{
				pMenu = menuPopup.GetSubMenu( 20 );
			}
			else if ( m_hRClickItem == m_hExitDoorType )
			{
				pMenu = menuPopup.GetSubMenu( 21 );
			}
			else if ( m_hRClickItem == m_hOrientation )
			{
				pMenu = menuPopup.GetSubMenu( 22 );
			}
			else 
				return;

			pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
		}	

	}
	else if (nProcType == ArpProcessor)
	{
		if (m_hRClickItem == m_hServiceLocation)
		{
			CMenu mnuPopup;
			pMenu = &mnuPopup;
			pMenu->CreatePopupMenu();
			pMenu->AppendMenu(MF_STRING, ID_PROCPROP_PICKFROMMAP, _T("Pick From Map"));
			pMenu->AppendMenu(MF_SEPARATOR);

			if (!m_treeProp.ItemHasChildren(m_hServiceLocation))
			{
				pMenu->AppendMenu(MF_STRING | MF_DISABLED | MF_GRAYED, ID_PROCPROP_LATLONG, _T("Modify Lat/LONG"));
			}
			else
				pMenu->AppendMenu(MF_STRING, ID_PROCPROP_LATLONG, _T("Modify Lat/LONG"));

			pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
	}
	else if (nProcType == BillboardProcessor)
	{
		if(m_hRClickItem)
		{
			m_treeProp.SelectItem(m_hRClickItem);

			if(m_hRClickItem==m_hServiceLocation)
			{
				pMenu=menuPopup.GetSubMenu(5);
				pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);
			}
			else if (m_hRClickItem == m_hOutConstraint)//m_hBillboardRadius) 
			{
				pMenu=menuPopup.GetSubMenu(5);
				pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);
				pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDQUEUEFIXED ,MF_BYCOMMAND);
				pMenu->DeleteMenu( ID_PROCPROP_TOGGLEDBACKUP ,MF_BYCOMMAND);
				pMenu->DeleteMenu( 2 ,MF_BYPOSITION);
			}
			else if (m_hRClickItem == m_hBillboardHeight)
				pMenu=menuPopup.GetSubMenu(52);
			else if(m_hRClickItem == m_hBillboardThickness)
				pMenu = menuPopup.GetSubMenu(53);
			else if(m_hRClickItem == m_hBillboardType)
			{
				pMenu = menuPopup.GetSubMenu(54);
				pMenu->CheckMenuItem(0,MF_BYPOSITION|(m_tempBillBoardProc.getType() == 0?MF_CHECKED:MF_UNCHECKED));
				pMenu->CheckMenuItem(1,MF_BYPOSITION|(m_tempBillBoardProc.getType() == 1?MF_CHECKED:MF_UNCHECKED));
			
			}
			else if(m_hRClickItem == m_hBillBoardText)
				pMenu = menuPopup.GetSubMenu(55);
			else if(m_hRClickItem == m_hBillBoardBitmapPath)
				pMenu = menuPopup.GetSubMenu(56);
			else if(m_hRClickItem == m_hBillBoardCoefficient)
				pMenu = menuPopup.GetSubMenu(57);
			else if (m_hRClickItem == m_hOutConstraint)
			{
				pMenu = menuPopup.GetSubMenu(5);
				pMenu->DeleteMenu(ID_PROCPROP_TOGGLEDBACKUP,MF_BYCOMMAND);
			}
			else if (m_hRClickItem == m_hBillboardAngle) 
			{
				pMenu = menuPopup.GetSubMenu(58);
			}
			else		
			return;

			pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
		}
	}
	else if (nProcType == HoldProcessor){
		if(m_hRClickItem)
		{
			m_treeProp.SelectItem(m_hRClickItem);
			if(m_hRClickItem == m_hFix || m_hRClickItem == m_hRight ){
				pMenu = menuPopup.GetSubMenu(59); 
			}
			else{
				pMenu = menuPopup.GetSubMenu(60);
			}
		}

		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
	}
	else if (nProcType == SectorProcessor){
		if(m_hRClickItem){
			m_treeProp.SelectItem(m_hRClickItem);
			if(m_hRClickItem == m_hMaxAlt || m_hRClickItem == m_hMinAlt || m_hRClickItem == m_hBandInterval){
				pMenu = menuPopup.GetSubMenu(60);
			}
			else if (m_hRClickItem == m_hServiceLocation){
				pMenu=menuPopup.GetSubMenu(5);
				pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);					
			}
		}
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
	}
	else if (nProcType == BridgeConnectorProc)
	{
		m_treeProp.SelectItem(m_hRClickItem);
		CString sLabel = m_treeProp.GetItemText(m_hRClickItem);

		HTREEITEM hParent = m_treeProp.GetParentItem(m_hRClickItem);
		if (hParent && m_treeProp.GetParentItem(hParent) == NULL)
			return;

		if (sLabel.Find("Width") >=0)
			pMenu = menuPopup.GetSubMenu(42);
		else if (sLabel.Find("Collapsed length") >=0)
			pMenu = menuPopup.GetSubMenu(44);
		else if (m_treeProp.GetParentItem(m_hRClickItem) == NULL)	//connect points
			pMenu = menuPopup.GetSubMenu(75);
		else
			pMenu = menuPopup.GetSubMenu(10);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
	}
	else
	{

		//if the proc is runway proc and not click the properties in the array
		if (nProcType == RunwayProcessor )
		{
			if (m_hRClickItem == m_hMarking || m_hRClickItem == m_hThreshold)
				return;
			if (m_hRClickItem == m_hThresholdA || m_hRClickItem == m_hThresholdB)
			{
				pMenu = menuPopup.GetSubMenu(62);
				pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
				return;
			}
			else if (m_hRClickItem == m_hMarkingA || m_hRClickItem == m_hMarkingB)
			{
				pMenu = menuPopup.GetSubMenu(63);
				pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
				return;
			}
			else if(m_hRClickItem==m_hServiceLocation)
			{
				pMenu=menuPopup.GetSubMenu(5);
				pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS ,MF_BYCOMMAND);	
				pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
				return;
			}
		}
		if (nProcType == TaxiwayProcessor )
		{			
			if (m_hRClickItem == m_hMarking)
				return;
			
			if (m_hRClickItem == m_hMarkingA )
			{
				pMenu = menuPopup.GetSubMenu(63);
				pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
				return;
			}
		}


		//pMenu = menuPopup.GetSubMenu( 5 );
		BOOL bFound = FALSE;

		CMenu mnuPopup;
		CString strCap = m_treeProp.GetItemText(m_hRClickItem);
		for (int i = 0; i < sizeof(s_szPropName) / sizeof(s_szPropName[0]); i++)
		{

			if (strCap.Find(s_szPropName[i]) != -1)
			{
				bFound = TRUE;
				strCap = _T("Modify ") + strCap;
				CreateTreeViewMenu(strCap, ID_PROCPROP_WIDTH + i, &mnuPopup);
				break;
			}
		
		}
		
		if (bFound)
			pMenu = &mnuPopup;
		else
		{
			if (nProcType != FixProcessor)
			{
				pMenu = menuPopup.GetSubMenu( 5 );
				//---------------------------------------------------------------
				//for proc has In&Out Constraint
				//
				//	PointProc DepSinkProc LineProc BaggageProc HoldAreaProc GateProc
				//	FloorChangeProc MovingSideWalkProc StandProcessor
				//---------------------------------------------------------------
				if (m_bInOutConstraintProc && m_hRClickItem == m_hOutConstraint)
					pMenu->EnableMenuItem( ID_PROCPROP_TOGGLEDBACKUP, MF_ENABLED );
				else
					pMenu->EnableMenuItem( ID_PROCPROP_TOGGLEDBACKUP, MF_GRAYED );

			}
		}

		//////////////////////////////////////////////////////////////////////////
		//special handle the fix process menu(location in local coordinate and longitude latitude)
		if (nProcType == FixProcessor)
		{
			if (m_hRClickItem == m_hServiceLocation) //unable the service location menu
			{
				return;
			}else if (m_hRClickItem == m_hFixXY)//location (x,y)
			{
				pMenu = menuPopup.GetSubMenu( 5 );
			}
			else if (m_hRClickItem == m_hFixLL)//longitude latitude(l,l) 
			{
				pMenu = menuPopup.GetSubMenu( 48 );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if (nProcType != FixProcessor)
		{
			if( m_treeProp.GetParentItem( m_hRClickItem ) != NULL )
			{
				m_treeProp.SelectItem( NULL );
				return;
			}
		}
		
	    if (nProcType == GateProc)
		{
			if (m_hRClickItem == m_hGateType)
			{
				pMenu = menuPopup.GetSubMenu(88);
				pMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
				return;
			}
		}

		if (nProcType == RetailProc)
		{
			if (m_hRClickItem == m_hRetailType)
			{
				pMenu = menuPopup.GetSubMenu(101);
				pMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
				return;
			}
		}

		pMenu->DeleteMenu( ID_PROCPROP_DEFINE_Z_POS,MF_BYCOMMAND );
		if( m_hRClickItem != m_hQueue || nProcType == LineProc || nProcType == BaggageProc )
			pMenu->EnableMenuItem( ID_PROCPROP_TOGGLEDQUEUEFIXED, MF_GRAYED );
		m_treeProp.SelectItem( m_hRClickItem );
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
	}
	

}

void CProcPropDlg::OnProcpropPickfrommap() 
{

	// check the type
	int nSelIdx = m_comboType.GetCurSel();
	if( nSelIdx < 0 )
		return;
	int nProcType = m_comboType.GetItemData( nSelIdx );

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;

	if( m_hRClickItem == m_hServiceLocation && ( nProcType == PointProc || nProcType == DepSourceProc ||
		nProcType == DepSinkProc || nProcType == GateProc ||
		nProcType == FloorChangeProc || nProcType == Elevator ||
		nProcType == ArpProcessor || nProcType == StandProcessor ||
		nProcType == NodeProcessor || nProcType == DeiceBayProcessor) )
		enumReason = PICK_ONEPOINT;
	else if((nProcType == FixProcessor &&m_hFixXY==m_hRClickItem)|| nProcType == BridgeConnectorProc)
	{
		enumReason= PICK_ONEPOINT;	
	}
	else if ( (m_hRClickItem == m_hServiceLocation && (nProcType == RunwayProcessor || nProcType == BillboardProcessor))||
		( nProcType==StandProcessor && m_bBackup ))
		enumReason = PICK_TWOPOINTS;
	else 
		enumReason = PICK_MANYPOINTS;


	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

void CProcPropDlg::OnProcpropToggledqueuefixed() 
{
	// TODO: Add your command handler code here
	if( m_nFixed == -1 )
	{
		if(m_pProc)
		{
			Path* pPath=m_pProc->queuePath();
			int iPathPointNum= pPath ? pPath->getCount() : 0;
			if( m_vQueieList.size() == 0 && iPathPointNum == 0 )
				return;		
			m_nFixed = m_pProc->queueIsFixed();
		}
		else
		{
			if( m_vQueieList.size() == 0 )
				return;
			m_nFixed=0;
		}

	}
	
	m_nFixed = m_nFixed == 1 ? 0 : 1;

	LoadTree();
	
}


int CProcPropDlg::DoFakeModal()
{
	if( CDialog::Create(CProcPropDlg::IDD, m_pParentWnd) ) 
	{
		CenterWindow();
		ShowWindow(SW_SHOW);
		GetParent()->EnableWindow(FALSE);
		EnableWindow();
		int nReturn = RunModalLoop();
		DestroyWindow();
		return nReturn;
	}
	else
		return IDCANCEL;
}


void CProcPropDlg::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CProcPropDlg::ShowDialog(CWnd* parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}

LRESULT CProcPropDlg::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	if (EscalatorProc == GetCurrentProcType() || StairProc == GetCurrentProcType())
	{
		m_bZPosHasBeenChanged = FALSE;
	}

		// check the type
	int nSelIdx = m_comboType.GetCurSel();
	if( nSelIdx < 0 )
		return FALSE;
	int nProcType = m_comboType.GetItemData( nSelIdx );
	/*
	CWnd* wnd = GetParent();
	while (!wnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		wnd->ShowWindow(SW_SHOW);
		wnd->EnableWindow(false);
		wnd = wnd->GetParent();
	}
	wnd->EnableWindow(false);
	*/
	ShowWindow(SW_SHOW);	
	EnableWindow();
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);

	//test code ,hans
	{
		for(int i=0; i<(int)pData->size(); i++)
		{
			ARCVector3 temp = pData->at(i);
			// TRACE("x = %.2f,y = %.2f , z = %.2f",temp[VX],temp[VY],temp[VZ]);
		}
	}
	/*
	if( nProcType == StairProc )
	{
		if( m_hRClickItem == m_hServiceLocation && pData->size() != 4 )
		{
			AfxMessageBox( " You must pick 4 points.");
			return FALSE;
		}
	}
	*/
	if(m_hRClickItem == NULL)
		return FALSE ;
    CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	pDoc->m_tempProcInfo.SetProcType(nProcType);
	if( nProcType != IntegratedStationProc && nProcType != BridgeConnectorProc)
	{
		
		if( m_hRClickItem == m_hServiceLocation )
		{
			m_vServiceLocation = *pData;	
			if(nProcType == RunwayProcessor) autoCalRunwayMark();
			//if(nProcType == StandProcessor ) addTempAirsideProc(nProcType);
			pDoc->m_tempProcInfo.SetTempServicLocation(m_vServiceLocation);
		}
		else if (nProcType==FixProcessor && m_hRClickItem==m_hFixXY)
		{
			m_vServiceLocation = *pData;			
			pDoc->m_tempProcInfo.SetTempServicLocation(m_vServiceLocation);
			
			double dx=((ARCVector3)m_vServiceLocation[0])[0]/100;
			double dy=((ARCVector3)m_vServiceLocation[0])[1]/100;
			Convent_local_latlong(dx,dy,m_strLat,m_strLong);
			if (m_pProc)
			{
				((FixProc *)m_pProc)->m_pLatitude->SetValue(m_strLat);
				((FixProc *)m_pProc)->m_pLongitude->SetValue(m_strLong);
			}

		}
		else if( m_hRClickItem == m_hInConstraint )
		{

			m_vInConstraint = *pData;
			pDoc->m_tempProcInfo.SetTempInConstraint(m_vInConstraint);
			//StandProcessor
			if(m_bBackup)
			{
				int nSelIdx = m_comboType.GetCurSel();
				if( nSelIdx >= 0 )
				{
					int nProcType = m_comboType.GetItemData( nSelIdx );
					if(StandProcessor == nProcType || DeiceBayProcessor == nProcType)
					{
						std :: vector<ARCVector3> m_vConsTemp = m_vInConstraint;
						std::reverse(m_vConsTemp.begin(), m_vConsTemp.end());
						m_vOutConstraint = m_vConsTemp;
						pDoc->m_tempProcInfo.SetTempOutConstrain(m_vOutConstraint);
					}
				}
			}

		}
		else if( m_hRClickItem == m_hOutConstraint )
		{
			m_vOutConstraint = *pData;
			pDoc->m_tempProcInfo.SetTempOutConstrain(m_vOutConstraint);

			//StandProcessor
			if(m_bBackup)
			{
				int nSelIdx = m_comboType.GetCurSel();
				if( nSelIdx >= 0 )
				{
					int nProcType = m_comboType.GetItemData( nSelIdx );
					if(StandProcessor == nProcType || DeiceBayProcessor == nProcType)
					{
						std :: vector<ARCVector3> m_vConsTemp = m_vOutConstraint;
						std::reverse(m_vConsTemp.begin(), m_vConsTemp.end());
						m_vInConstraint = m_vConsTemp;
						pDoc->m_tempProcInfo.SetTempInConstraint(m_vInConstraint);
					}
				}
			}

		}			
		else if( m_hRClickItem == m_hQueue )
		{
			m_vQueieList = *pData;
			pDoc->m_tempProcInfo.SetTempQueue(m_vQueieList);
			m_EntryPointIndex.clear() ;
		//	m_pProc->GetProcessorQueue()->DeleteAllEntryPoint() ;
		}
		else if(m_hRClickItem == m_EntryPoint)
		{
                ARCVector3 temp = pData->at(0);
                CEntryPointAndIndex  *entrypoint = new  CEntryPointAndIndex;
				if(m_pProc != NULL && m_pProc->GetProcessorQueue() != NULL)
				{
					Path* path = m_pProc->queuePath() ;
					Point point(temp[VX],temp[VY],temp[VY]) ;
					int index = path->getClosestPointIndex(point) ; //get path segment index
					entrypoint->point =  m_pProc->GetProcessorQueue()->corner(index);
					entrypoint->index = index ;
				}
				else
				{
					if(m_vQueieList.empty())
						return FALSE;
					DistanceUnit dis = m_vQueieList[0].DistanceTo(temp) ;
					int  nde = 0 ;
					for(int i = 1 ; i < (int)m_vQueieList.size() ; i++)
					{
                        if( m_vQueieList[i].DistanceTo(temp) < dis )
						{
							dis = m_vQueieList[i].DistanceTo(temp) ;
							nde = i ;
						}
					}
					entrypoint->point = m_vQueieList[nde] ;
					entrypoint->index = nde ;
				}
				//check the entry point is exist or not, if not exist, push to vector
				bool bEntryPointExist = false;

				std::vector<CEntryPointAndIndex *>::iterator iterEnryPoint = m_EntryPointIndex.begin() ;
				for( ;iterEnryPoint != m_EntryPointIndex.end() ;++iterEnryPoint)
				{
					if((*iterEnryPoint)->index == entrypoint->index)
					{
						bEntryPointExist = true;
						break;
					}
				}
			  if(bEntryPointExist == false)// not exist
			  {
				  m_EntryPointIndex.push_back(entrypoint) ;
			  }
			  else
			  {
				  delete entrypoint;
			  }
				
		}
		else if( m_hRClickItem == m_hBaggageProcBarrier )
		{
			m_vBaggageBarrier = *pData;
			pDoc->m_tempProcInfo.SetTempBaggageBarrier( m_vBaggageBarrier );
		}
		else if( m_hRClickItem == m_hBaggageMovingPath )
		{
			m_vBaggagMovingPath = *pData;
			pDoc->m_tempProcInfo.SetTempBaggageMovingPath( m_vBaggagMovingPath );
		}
		else if (m_hRClickItem == m_hStoreArea)
		{
			m_vStoreArea = *pData;
			pDoc->m_tempProcInfo.SetTempStoreArea(m_vStoreArea);
		}
		//else if (m_hRClickItem = m_hExpSurePath)
		//{
		//	m_vBillboardExposureArea = *pData;
		//}
	}
	else if (nProcType == BridgeConnectorProc)
	{
		Point pos;
		ARCVector3 v3D = pData->at(0);
		pos.setX( v3D[VX] );
		pos.setY( v3D[VY] );
		pos.setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );

		HTREEITEM hParItem = m_treeProp.GetParentItem(m_hRClickItem);
		if (hParItem)
		{
			int idx = m_treeProp.GetItemData(hParItem);
			BridgeConnector::ConnectPoint& conPoint = m_vBridgeConnectPoints[idx];
			CString sLabel = m_treeProp.GetItemText(m_hRClickItem);
			if (sLabel.Find("Location") >=0)
				conPoint.m_Location = pos;
			else if (sLabel.Find("Static direction from") >=0)
				conPoint.m_dirFrom = pos;
			else
				conPoint.m_dirTo = pos;
		}			

	}
	else
	{
		if(pData->size()>0)
		{
			Point ptList[MAX_POINTS];
			int nCount = pData->size();
			int iOratation = m_tempStation.GetOrientation();
			DistanceUnit dX = m_tempStation.getServicePoint( 0 ).getX();
			DistanceUnit dY = m_tempStation.getServicePoint( 0 ).getY();
			for( int i=0; i<nCount; i++ )
			{
				ARCVector3 v3D = pData->at(i);
				ptList[i].setX( v3D[VX] );
				ptList[i].setY( v3D[VY] );
				ptList[i].setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );
				
				if( iOratation > 0 )
				{
					ptList[i].DoOffset( -dX,-dY,0 );
					ptList[i].rotate( -iOratation );
					ptList[i].DoOffset( dX,dY ,0);
				}	
			}
			getStationLayout()->GetPreBoardingArea()->initInConstraint( nCount, ptList );
			pDoc->m_tempProcInfo.SetProcessorGroup(FALSE);

			//ADD IN 10-18 22:00
			//pDoc->m_tempProcInfo.SetIntegratedStation(&m_tempStation);
			m_tempStation.UpdateTempInfo( getStationLayout() );
			m_iSelectType = -1;
		}		
	}
		
	LoadTree();
	//pDoc->UpdateAllViews(NULL);
	
	return TRUE;
}
bool CProcPropDlg::checkRelation(CString strlevelName)
{
	ProcessorID PorcID;
	PorcID.SetStrDict(GetInputTerminal()->inStrDict);
	PorcID.setID(strlevelName);
	CUsedProcInfoBox *box = new CUsedProcInfoBox();
	CUsedProcInfo usedProcInfo((Terminal*)GetInputTerminal(),&PorcID);
	box->AddInfo(usedProcInfo);
	if (box->IsNoInfo())
	{
		delete box;
		return true;
	}
	return false;
}

void CProcPropDlg::OnOK() 
{
	// check the type
 	int nSelIdx = m_comboType.GetCurSel();
	if( nSelIdx < 0 )
		return;
	int nProcType = m_comboType.GetItemData( nSelIdx );

	m_nShapeIndex = m_cmbShape.GetSelectedIndex();
	
	// save the data from the m_pProc place.
	UpdateData();
	PreProcessName();

	if( !CheckProcessorName() )
		return;
	//Processor* pNewProcessor=NULL;// Only used when the old processor's name and type is all changed
	if( !CheckQueueLocation( nProcType ) )
		return ;

	if( !CheckInConstraintLocation( nProcType ) )
		return ;

	if( !CheckOutConstraintLocation( nProcType ) )
		return ;

	BOOL newProc = FALSE;
	if( nProcType != IntegratedStationProc )
	{
			if( m_pProc == NULL )
			{
				newProc = TRUE;
				// check the name				
				if( m_csLevel1.IsEmpty() )
				{
					MessageBox( "Name has not been defined yet", "Error", MB_OK|MB_ICONWARNING );
					return;
				}


				CString csName = m_csLevel1;
				if( !m_csLevel2.IsEmpty() )
					csName += "-" + m_csLevel2;
				if( !m_csLevel3.IsEmpty() )
					csName += "-" + m_csLevel3;
				if( !m_csLevel4.IsEmpty() )
					csName += "-" + m_csLevel4;
				
				ProcessorID id;
				id.SetStrDict( GetInputTerminal()->inStrDict );
				id.setID( csName );
				int nIdx = GetInputTerminal()->GetProcessorList()->findProcessor( id );
				if( nIdx != INT_MAX )
				{
					// EXIST.
					CString csMsg;
					csMsg.Format( "Processor %s exists in the Database.", csName );
					MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
					return;
				}

				//check this new name is valid or not
				if(!GetInputTerminal()->GetProcessorList()->IsValidProcessorName(csName))
					return ;
					
				if( !CheckServiceLocation( nProcType ) )
					return;
				
				switch( nProcType )
				{
				case PointProc:
					 m_pProc= new Processor;
					break;
				case DepSourceProc:
					m_pProc = new DependentSource;
					break;
				case DepSinkProc:
					m_pProc= new DependentSink;
					break;
				case LineProc:
					m_pProc= new LineProcessor;
					break;
				case RetailProc:
					m_pProc = new RetailProcessor;
					((RetailProcessor*)m_pProc)->SetRetailType((RetailProcessor::RetaiType)m_emRetailType);
					break;
				case BaggageProc:
					m_pProc = new BaggageProcessor;
					break;
				case HoldAreaProc:
					m_pProc = new HoldingArea;
					break;
				case GateProc:
					m_pProc = new GateProcessor;
					((GateProcessor*)m_pProc)->setGateType(m_emGateType);
					break;
				case FloorChangeProc:
					m_pProc = new FloorChangeProcessor;
					break;
				case BarrierProc:
					m_pProc = new Barrier;
					break;
				case MovingSideWalkProc:
					m_pProc = new MovingSideWalk();	
					break;
				case Elevator:
					m_pProc = new ElevatorProc;					
					m_tempElevator.init(id);
					break;
				case ConveyorProc:
					m_pProc = new Conveyor;
					((Conveyor*)m_pProc)->SetWidth( m_dConveyWidth );
					break;
				case StairProc:
					m_pProc =new Stair;
					((Stair*)m_pProc)->SetWidth( m_dConveyWidth );
					break;
				case EscalatorProc:
					m_pProc =new Escalator;
					((Escalator*)m_pProc)->SetWidth( m_dConveyWidth );
					break;

				//	add by kevin
				case ArpProcessor:
					m_pProc = new ArpProc;
					((ArpProc*)m_pProc)->SetLatlong(ArpProc::Latitude, string(m_strLat));
					((ArpProc*)m_pProc)->SetLatlong(ArpProc::Longitude, string(m_strLong));
					break;
				case RunwayProcessor:
					m_pProc = new RunwayProc;
					((RunwayProc*)m_pProc)->SetWidth(m_dWidth);
					((RunwayProc*)m_pProc)->SetThreshold1(m_dThreshold);
					((RunwayProc*)m_pProc)->SetThreshold2(m_dThreshold2);
					((RunwayProc*)m_pProc)->SetMarking1(m_strMarking);
					((RunwayProc*)m_pProc)->SetMarking2(m_strMarking2);
					break;
				case TaxiwayProcessor:
					m_pProc = new TaxiwayProc;
					((TaxiwayProc*)m_pProc)->SetWidth(m_dWidth);
					((TaxiwayProc*)m_pProc)->SetMarking(m_strMarking);
					((TaxiwayProc*)m_pProc)->SetMarkingPosition(int(m_vServiceLocation.size() * 0.5),0);
					break;
				case StandProcessor:
					m_pProc = new StandProc;					
					break;
				case NodeProcessor:
					m_pProc = new NodeProc;
					((NodeProc*)m_pProc)->SetNumber(m_nAutoNumber);
					break;
				case DeiceBayProcessor:
					m_pProc = new DeiceBayProc;
					((DeiceBayProc*)m_pProc)->SetNumber(m_nAutoNumber);
					((DeiceBayProc*)m_pProc)->SetWindspan(m_dConveyWidth);
					((DeiceBayProc*)m_pProc)->SetLength(m_dProcessorLength);
					break;
				case FixProcessor:
					m_pProc = new FixProc;
					((FixProc*)m_pProc)->SetLowerLimit(m_dLowerLimit);
					((FixProc*)m_pProc)->SetUpperLimit(m_dUpperLimit);
					((FixProc*)m_pProc)->SetNumber(m_nAutoNumber);
					((FixProc*)m_pProc)->m_pLatitude->SetValue(m_strLat);
					((FixProc*)m_pProc)->m_pLongitude->SetValue(m_strLong);
					((FixProc*)m_pProc)->setOperationType((FixProc::OperationType)m_nFixeOperationType);
					break;
				case ContourProcessor:
					m_pProc = new ContourProc;
					((ContourProc *)m_pProc)->setHeight(m_dHeight);
					break;
				case ApronProcessor :
					m_pProc = new ApronProc;
					break;
				case BillboardProcessor:
					m_pProc = new BillboardProc;
					m_tempBillBoardProc.init(id);
					//((BillboardProc *)m_pProc)->setHeight(m_dBbHeight);
					//((BillboardProc *)m_pProc)->setThickness(m_dBbThickness);
					//((BillboardProc *)m_pProc)->setType(m_nBbType);
					//((BillboardProc *)m_pProc)->setText(m_strBbText);
					//((BillboardProc *)m_pProc)->setBitmapPath(m_strBbPath);
					break;
				case SectorProcessor:
					m_pProc  = new SectorProc;
					
					((SectorProc*)m_pProc)->setAltBandInterval(m_dbandinterval);
					((SectorProc*)m_pProc)->setMinAlt(m_dminAlt);
					((SectorProc*)m_pProc)->setMaxAlt(m_dmaxAlt);
					break;
				case BridgeConnectorProc:
					m_pProc = new BridgeConnector;
					AcquireDataForBridgeConnector();
					break;
				case HoldProcessor:
					m_pProc = new HoldProc;
					HoldProc  * pHold = (HoldProc*)m_pProc;
					int setitem = m_treeProp.m_comboBox.GetCurSel();
					if(setitem<(int)m_strfixes.size() && setitem >=0 )
						m_strFix = m_strfixes[setitem];
					pHold->setFixName(m_strFix);
					pHold->RefreshRelateFix(GetInputTerminal()->GetProcessorList());
					pHold->setInBoundDegree(m_nInBoundDegree);
					pHold->setMaxAltitude(m_dmaxAlt);
					pHold->setMinAltitude(m_dminAlt);
					pHold->setOutBoundLegMin(m_dOutBoundmin);
					pHold->setOutBoundLegNm(m_dOutBoundnm);
					pHold->setRightTurn(m_bRight);
					break;				
				}
				m_pProc->init( id );
				
				m_pProc->SetTerminal( (Terminal*) GetInputTerminal() );// add by tutu 2002-12-27
				GetInputTerminal()->GetProcessorList()->addProcessor(m_pProc);
				GetInputTerminal()->GetProcessorList()->setIndexes();							
			}
			else   //  m_pProc->GetProcessor() != NULL 
			{
				if( !CheckServiceLocation( nProcType ) )
					return;

				char levelName[256] = {0}; 
				CString strLevel1Name =_T("");
				CString strLevel2Name = _T("");
				CString strLevel3Name = _T("");
				CString strLevel4Name = _T("");
				CString csIDName = _T("");
				CString csOldName = _T("");
				m_pProc->getID()->getNameAtLevel(levelName,0);
				strLevel1Name = levelName;
				memset(levelName,0,sizeof(levelName));
				m_pProc->getID()->getNameAtLevel(levelName,1);
				strLevel2Name = levelName;
				memset(levelName,0,sizeof(levelName));
				m_pProc->getID()->getNameAtLevel(levelName,2);
				strLevel3Name = levelName;
				memset(levelName,0,sizeof(levelName));
				m_pProc->getID()->getNameAtLevel(levelName,3);
				strLevel4Name = levelName;
				memset(levelName,0,sizeof(levelName));
				csOldName = strLevel1Name;
				if (!strLevel2Name.IsEmpty())
				{
					csOldName += "-" + strLevel2Name;
					if(checkRelation(csOldName))
					{
						csIDName  = csOldName;
					}
				}
				if (!strLevel3Name.IsEmpty())
				{
					csOldName += "-" + strLevel3Name;
					if(checkRelation(csOldName))
					{
						csIDName  = csOldName;
					}
				}
				if (!strLevel4Name.IsEmpty())
				{
					csOldName += "-" + strLevel4Name;
					if(checkRelation(csOldName))
					{
						csIDName  = csOldName;
					}
				}

				CString csName = m_csLevel1;
				
				if( !m_csLevel2.IsEmpty() )
				{
					csName += "-" + m_csLevel2;
		
				}
				if( !m_csLevel3.IsEmpty() )
				{
					csName += "-" + m_csLevel3;
					
				}
				if( !m_csLevel4.IsEmpty() )
				{
					csName += "-" + m_csLevel4;
					
				}
/*
				if( !IfProcType1CanChangeToType2( m_pProc->getProcessorType(), nProcType ) )
				{
					AfxMessageBox(" Can not change Processor type !");
					return;
				}
				
*/
				if (nProcType == BridgeConnectorProc)
				{
					AcquireDataForBridgeConnector();
				}
				if(IsNameChanged(csName)) //name is changed.
				{
					//check this new name is valid or not
					if(!GetInputTerminal()->GetProcessorList()->IsValidProcessorName(csName))
						return ;
					
					if( !CheckServiceLocation( nProcType ) )
                        return;

					if (nProcType == BillboardProcessor)
					{
						int nPathCount = static_cast<int>(m_vBillboardExposureArea.size());
						if( nPathCount == 0 && m_tempBillBoardProc.GetExpsurePathPonitCount() == 0)
						{
							MessageBox( "Exposure area has not been defined yet.", "Error", MB_OK|MB_ICONWARNING );
							return ;
						}
					}

					CUsedProcInfoBox *box = new CUsedProcInfoBox();
				
					ProcessorID id;
					id.SetStrDict(GetInputTerminal()->inStrDict);
					id.setID(csIDName);
					int nLevel = id.idLength();
//					CUsedProcInfo usedProcInfo((Terminal*)GetInputTerminal(),(ProcessorID*)m_pProc->getID());
					CUsedProcInfo usedProcInfo((Terminal*)GetInputTerminal(),&id);
					box->AddInfo(usedProcInfo);
					//GetDocument()->GetTerminal().procList->removeProcessor(((CProcessor2*)GetDocument()->m_vSelectedProcessors[j])->GetProcessor(),false);
                    if(!box->IsNoInfo())
					{
						if(MessageBox("Rename processor,Are you sure?",NULL,MB_OKCANCEL)==IDOK){
							
							if (m_pProc->getProcessorType() == ContourProcessor)
							{
								CString strOldName = m_pProc->getID()->GetIDString();						
								GetInputTerminal()->ProcessorRename(m_pProc,csName, GetProjPath());									
							//update contour trees
								CString strNewName = csName;
								GetInputTerminal()->m_AirsideInput->pContourTree->Rename(strOldName,strNewName);
								
								CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
								pDoc->GetSelectedNode()->Name(strNewName);
								pDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pDoc->GetSelectedNode()->Parent());
									
							}
							else
							{
								BOOL bRet = GetInputTerminal()->ProcessorRename(m_pProc,csName, GetProjPath());
								if(TRUE == bRet)//update temp Elevator groupIndex
								{
									if(m_pProc->getProcessorType() == Elevator)
									{
										m_tempElevator = *((ElevatorProc*)m_pProc);
									}
									if(m_pProc->getProcessorType() == BillboardProcessor)
									{
										m_tempBillBoardProc = *((BillboardProc*)m_pProc);
									}
									
								}
							}	
						}
					}
					else	
					{
						ProcessorID newID;
						newID.SetStrDict(GetInputTerminal()->inStrDict);
						newID.setID(csName);
						GroupIndex gIndex = GetInputTerminal()->GetProcessorList()->getGroupIndex(id);
						if(m_pProc->getID()->idLength()!= nLevel && (gIndex.start != gIndex.end ||\
							id.idFits(newID)))
						{
							if(MessageBox("Rename processor,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
							{
								GetInputTerminal()->ProcessorRename(m_pProc,csName, GetProjPath());

								CString strNewName = csName;
								CString strOldName = m_pProc->getID()->GetIDString();
								GetInputTerminal()->m_AirsideInput->pContourTree->Rename(strOldName,strNewName);
							}
						}
						else
						{
						    if(box->DoModal()==IDOK)
							{
							}
						}
					}
					delete box;
				}	
			}
			
			m_pProc->SetBackup(m_bBackup);

			//////////////////////////////////////////////////////////////////////////	
			// to set emergent flag
			if( m_pProc->getProcessorType() == PointProc ||
				m_pProc->getProcessorType() == LineProc  ||
				m_pProc->getProcessorType() == RetailProc ||
				m_pProc->getProcessorType() == GateProc )
				m_pProc->setEmergentFlag(m_btnEmergent.GetCheck() ? true : false);

			//////////////////////////////////////////////////////////////////////////
			if( m_pProc->getProcessorType() == Elevator )
			{
				CString szID = m_pProc->getID()->GetIDString();
				*((ElevatorProc*)m_pProc)=m_tempElevator;
				m_pProc->setID(szID);
			}
			if (m_pProc->getProcessorType() == BillboardProcessor)
			{
				CString szID = m_pProc->getID()->GetIDString();
				*((BillboardProc*)m_pProc) = m_tempBillBoardProc;
				m_pProc->setID(szID);
				m_pProc->SetTerminal( (Terminal*) GetInputTerminal() );
				GetInputTerminal()->GetProcessorList()->setIndexes();	
			}
			if( !SetProcessorValue(m_pProc) )
			{
				return;
			}
			if( m_pProc->getProcessorType() == Elevator )
			{
				
				(( ElevatorProc* )m_pProc)->InitLayout();
			}
			if(m_pProc->getProcessorType()==StandProcessor){				
				((StandProc *)m_pProc)->setPushBackWay(m_vInConstraint);
				((StandProc *)m_pProc)->UsePushBack(m_bBackup);
			}
			if(m_pProc->getProcessorType()==TaxiwayProcessor)
			{
				if(newProc)
					((Terminal*)GetInputTerminal())->GetAirsideInput()->AddTaxiwayProc((TaxiwayProc*)m_pProc);
				else
					((Terminal*)GetInputTerminal())->GetAirsideInput()->UpdateTaxiwayProc((TaxiwayProc*)m_pProc);
			}
			
			CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
			GetInputTerminal()->GetProcessorList()->saveDataSet( GetProjPath(), true );
			GetInputTerminal()->m_pMovingSideWalk->saveDataSet( GetProjPath(), false );
			//clear Doc's related data member
			pDoc->m_tempProcInfo.ClearAll();
		}
		else//is integrated Station
		{
			// check the name
				if( m_csLevel1.IsEmpty() )
				{
					MessageBox( "Name has not been defined yet", "Error", MB_OK|MB_ICONWARNING );
					return;
				}

				CString csName = m_csLevel1;
				if( !m_csLevel2.IsEmpty() )
					csName += "-" + m_csLevel2;
				if( !m_csLevel3.IsEmpty() )
					csName += "-" + m_csLevel3;
				if( !m_csLevel4.IsEmpty() )
					csName += "-" + m_csLevel4;

				ProcessorID id;
				id.SetStrDict( GetInputTerminal()->inStrDict );
				id.setID( csName );

				BuildAllSubProcessorID();//
				
				CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
				bool bIsNew = true;
				if(m_pProc)
				{
					char levelName[256] = {0}; 
					CString strLevel1Name =_T("");
					CString strLevel2Name = _T("");
					CString strLevel3Name = _T("");
					CString strLevel4Name = _T("");
					CString csIDName = _T("");
					CString csOldName = _T("");
					m_pProc->getID()->getNameAtLevel(levelName,0);
					strLevel1Name = levelName;
					memset(levelName,0,sizeof(levelName));
					m_pProc->getID()->getNameAtLevel(levelName,1);
					strLevel2Name = levelName;
					memset(levelName,0,sizeof(levelName));
					m_pProc->getID()->getNameAtLevel(levelName,2);
					strLevel3Name = levelName;
					memset(levelName,0,sizeof(levelName));
					m_pProc->getID()->getNameAtLevel(levelName,3);
					strLevel4Name = levelName;
					memset(levelName,0,sizeof(levelName));
					csOldName = strLevel1Name;
					if (!strLevel2Name.IsEmpty())
					{
						csOldName += "-" + strLevel2Name;
						if(checkRelation(csOldName))
						{
							csIDName  = csOldName;
						}
					}
					if (!strLevel3Name.IsEmpty())
					{
						csOldName += "-" + strLevel3Name;
						if(checkRelation(csOldName))
						{
							csIDName  = csOldName;
						}
					}
					if (!strLevel4Name.IsEmpty())
					{
						csOldName += "-" + strLevel4Name;
						if(checkRelation(csOldName))
						{
							csIDName  = csOldName;
						}
					}

					ProcessorID IDName;
					IDName.SetStrDict( GetInputTerminal()->inStrDict );
					IDName.setID( csIDName );

					int nIDLength = m_pProc->getID()->idLength();
					CString strIDName =  m_pProc->getID()->GetIDString();

					int nLevel = IDName.idLength();

					ProcessorID PorcID;
					PorcID.SetStrDict(GetInputTerminal()->inStrDict);
					PorcID.setID(csIDName);

					if(IsNameChanged(csName))
					{
						CUsedProcInfoBox *box = new CUsedProcInfoBox();
						CUsedProcInfo usedProcInfo((Terminal*)GetInputTerminal(),&PorcID);
						box->AddInfo(usedProcInfo);
						if(!box->IsNoInfo())
						{
							if(!GetInputTerminal()->GetProcessorList()->IsValidProcessorName(csName))
							{
								delete box;
								return ;
							}

							if(MessageBox("Rename processor,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
								GetInputTerminal()->ProcessorRename(m_pProc,csName, GetProjPath());
							else
							{
								delete box;
								CDialog::OnOK();
								return;
							}
						}
						else{
							ProcessorID newID;
							newID.SetStrDict(GetInputTerminal()->inStrDict);
							newID.setID(csName);
							GroupIndex gIndex = GetInputTerminal()->GetProcessorList()->getGroupIndex(IDName);
							if(m_pProc->getID()->idLength()!= nLevel && (gIndex.start != gIndex.end ||\
								IDName.idFits(newID)))
							{
								if(!GetInputTerminal()->GetProcessorList()->IsValidProcessorName(csName))
								{
									delete box;
									return ;
								}

								if(MessageBox("Rename processor,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
									GetInputTerminal()->ProcessorRename(m_pProc,csName, GetProjPath());
								else
								{
									delete box;
									CDialog::OnOK();
									return;
								}
							}
							else if(box->DoModal()==IDOK)
							{
								delete box;
								CDialog::OnOK();
								return;
							}
						}
				
					}	

					bIsNew = false;
				}
				else
				{
					/*
					ProcessorID id;
					id.SetStrDict( GetInputTerminal()->inStrDict );
					id.setID( csName );
					*/ //2002-10-11
					int nIdx = GetInputTerminal()->GetProcessorList()->findProcessor( id );
					if( nIdx != INT_MAX )
					{
						// EXIST.
						CString csMsg;
						csMsg.Format( "Processor %s exists in the Database.", csName );
						MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
						return;
					}

					if(!GetInputTerminal()->GetProcessorList()->IsValidProcessorName(csName))
						return ;

					m_pProc= new IntegratedStation();
					m_pProc->init( id );
	
					ProcessorID id;
					id.SetStrDict( GetInputTerminal()->inStrDict );
					id.setID( "PLATFORM" );
					m_tempStation.GetStationLayout()->GetPreBoardingArea()->init(id);
					
					GetInputTerminal()->GetProcessorList()->addProcessor(m_pProc);
					GetInputTerminal()->GetProcessorList()->setIndexes();	
					
				
					pDoc->m_trainTraffic.AddNewStation((IntegratedStation*)m_pProc);
					
				}
				

				m_tempStation.InitLocalData();
				*((IntegratedStation*)m_pProc)=m_tempStation;
				((IntegratedStation*)m_pProc)->init( id );

					
				//*(GetInputTerminal()->pStationLayout)=m_stationLayout;
				//GetInputTerminal()->pStationLayout->saveDataSet( GetProjPath(), false );

				
				
				GetInputTerminal()->GetProcessorList()->saveDataSet( GetProjPath(), true );
				GetInputTerminal()->GetProcessorList()->RefreshAllStationLocatonData();

				if( !bIsNew )
				{
					if( m_iUpdateAllRailWay > -1 )
					{
						if ( 1 == m_iUpdateAllRailWay )
						{
							pDoc->m_trainTraffic.AdjustAllLinkedRailWay();
						}
						else if ( 0 == m_iUpdateAllRailWay )
						{
							pDoc->m_trainTraffic.AdjustLinkedRailWay( (IntegratedStation*)m_pProc );
						}

						//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
						{
							CString strLog = _T("");
							strLog = _T("Property Dialog UpdateAllRailWay;");
							ECHOLOG->Log(RailWayLog,strLog);
						}

						GetInputTerminal()->pRailWay->saveDataSet( GetProjPath(), false );
						GetInputTerminal()->m_pAllCarSchedule->SetSystemRailWay( GetInputTerminal()->pRailWay );
						GetInputTerminal()->m_pAllCarSchedule->AdjustRailWay( &pDoc->m_trainTraffic );
						GetInputTerminal()->m_pAllCarSchedule->saveDataSet( GetProjPath(),false );
					}
					
					
					
					pDoc->m_tempProcInfo.SetTrafficGraph( &pDoc->m_trainTraffic );
				}

				
				
				pDoc->m_tempProcInfo.SetIntegratedStation(NULL);

				//set temp proc index as -1 ,then will draw all integrated station 
				//in the proclist ,because temp station will not valid when this
				// dialog is closed
				pDoc->m_tempProcInfo.SetProcIndex( -1 );

		}
	
	m_pProc->UpdateMinMax();
	//delete m_pTempHoldingArea;
	CDialog::OnOK();
}


CString CProcPropDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


bool CProcPropDlg::CheckServiceLocation( int _nProcType )
{
	
	if(HoldProcessor == _nProcType || BridgeConnectorProc == _nProcType)return true;
	int nServiceLocationSize = m_vServiceLocation.size();
	if( nServiceLocationSize == 0 && m_pProc )
		nServiceLocationSize = m_pProc->serviceLocationLength();

	if( nServiceLocationSize == 0 )
	{
		MessageBox( "Service Location has not been defined yet.", "Error", MB_OK|MB_ICONWARNING );
		return false;
	}
	//check Queue Point


	if (_nProcType == DepSourceProc )
	{
		if ( m_vQueieList.size() <= 0 && !m_bHavePath) 
		{
			{
				MessageBox( "Queue points has not been defined yet.", "Error", MB_OK|MB_ICONWARNING );
				return false;
			}		
		}	
	}
	

	// check the service location
	// only 1 point
	if( _nProcType == PointProc || _nProcType == DepSourceProc ||
		_nProcType == DepSinkProc || _nProcType == GateProc ||
		_nProcType == FloorChangeProc )
	{
		if( nServiceLocationSize > 1 )
		{
			MessageBox( "Only one point need to be defined.", "Error", MB_OK|MB_ICONWARNING );
			return false;
		}
	}

	// min 2
	if( _nProcType == LineProc || _nProcType == BarrierProc 
		|| _nProcType == ConveyorProc || _nProcType == StairProc 
		|| _nProcType == EscalatorProc || _nProcType == RetailProc)
	{
		if( nServiceLocationSize < 2 )
		{
			MessageBox( "Minimum two points need to be defined.", "Error", MB_OK|MB_ICONWARNING );
			return false;
		}
	}


	// min 3
	if( _nProcType == BaggageProc || _nProcType == HoldAreaProc )
	{
		if( nServiceLocationSize < 3 )
		{
			MessageBox( "Minimum three points need to be defined.", "Error", MB_OK|MB_ICONWARNING );
			return false;
		}
	}


	return true;
}


//check the queue and serice location whether on the same floor
bool CProcPropDlg::CheckQueueLocation(int _nProcType)
{
	int nServiceFloor = 0;
	int nSize = m_vServiceLocation.size();
	if(nSize > 0)
	{
		ARCVector3 v3D = m_vServiceLocation[0];	
		nServiceFloor = GetFloor(v3D[VZ]);
	}
	else if(m_pProc)
	{
		Path* pPath = m_pProc->serviceLocationPath();
		if(pPath)
		{
			if(pPath->getCount() <= 0)
				return true;
			nServiceFloor = int(pPath->getPoint(0).getZ() / SCALE_FACTOR);
		}
	}

	int nQueueFloor = -1;
	if( m_vQueieList.size() > 0 )
	{
		for( int i=0; i<static_cast<int>(m_vQueieList.size()); i++ )
		{
			ARCVector3 v3D = m_vQueieList[i];
			nQueueFloor = GetFloor(v3D[VZ]);

		}
	}
	else if( m_pProc )
	{
		Path* pPath = m_pProc->queuePath();
		if( pPath && pPath->getCount() >0 )
		{
			for( int i=0; i<pPath->getCount(); i++ )
			{
				Point pt = pPath->getPoint(i);
				nQueueFloor = int(pt.getZ() / SCALE_FACTOR);

			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}
	if(nServiceFloor != nQueueFloor)
	{
		MessageBox( "Queue points and entry point must be on the same floor.", "Error", MB_OK|MB_ICONWARNING );
		return false;
	}
	return true;
}
//check the in constraint and service location floor
bool CProcPropDlg::CheckInConstraintLocation(int _nProcType)
{
	int nServiceFloor = 0;
	int nSize = m_vServiceLocation.size();
	if(nSize > 0)
	{
		ARCVector3 v3D = m_vServiceLocation[0];
		nServiceFloor = GetFloor(v3D[VZ]);

	}
	else if(m_pProc)
	{
		Path* pPath = m_pProc->serviceLocationPath();
		if(pPath)
		{
			if(pPath->getCount() <= 0)
				return true;
			nServiceFloor = int(pPath->getPoint(0).getZ() / SCALE_FACTOR);
		}
	}


	int nInconstraintFloor = -1;

	if( m_vInConstraint.size() > 0)
	{
		for( int i=0; i<static_cast<int>(m_vInConstraint.size()); i++ )
		{
			ARCVector3 v3D = m_vInConstraint[i];
			nInconstraintFloor = GetFloor(v3D[VZ]);
		}
	}
	else if( m_pProc )
	{
		Path* pPath = m_pProc->inConstraint();
		if( pPath && pPath->getCount() >0 )
		{
			for( int i=0; i<pPath->getCount(); i++ )
			{
				Point pt = pPath->getPoint(i);
				nInconstraintFloor = int(pt.getZ() / SCALE_FACTOR);

			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}

	if(nServiceFloor != nInconstraintFloor)
	{
		MessageBox( "In constraint points and entry point must be on the same floor.", "Error", MB_OK|MB_ICONWARNING );
		return false;
	}

	return true;
}
//check the out constraint and the serve loacation wether in the same floor
bool CProcPropDlg::CheckOutConstraintLocation(int _nProcType)
{
	//get the service location floor
	int nServiceFloor = 0;
	int nSize = m_vServiceLocation.size();
	if(nSize > 0)
	{
		ARCVector3 v3D = m_vServiceLocation[nSize-1];
		nServiceFloor = GetFloor(v3D[VZ]);

	}
	else if(m_pProc)
	{
		Path* pPath = m_pProc->serviceLocationPath();
		if(pPath)
		{
			int nCount = pPath->getCount();
			if(nCount <= 0)
				return true;
			nServiceFloor = int(pPath->getPoint(nCount -1).getZ() / SCALE_FACTOR);
		}
	}

	//get the out constraint floor
	int nOutConstraintFloor = -1;
	if( m_vOutConstraint.size() > 0 )
	{
		for( int i=0; i<static_cast<int>(m_vOutConstraint.size()); i++ )
		{
			ARCVector3 v3D = m_vOutConstraint[i];
			nOutConstraintFloor = GetFloor(v3D[VZ]);
		}
	}
	else if( m_pProc )
	{
		Path* pPath = m_pProc->outConstraint();
		if( pPath && pPath->getCount() >0 )
		{
			for( int i=0; i<pPath->getCount(); i++ )
			{
				Point pt = pPath->getPoint(i);
				nOutConstraintFloor = int(pt.getZ() / SCALE_FACTOR);

			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}

	//verify
	if(nServiceFloor != nOutConstraintFloor)
	{
		MessageBox( "Out constraint points and exit point must be on the same floor.", "Error", MB_OK|MB_ICONWARNING );
		return false;
	}

	return true;
}


Processor* CProcPropDlg::GetProcessor()
{
	return m_pProc;
}

void CProcPropDlg::OnSelchangeComboType() 
{
	int nSelType = m_comboType.GetCurSel();
	int nProcType = nSelType >=0 ? m_comboType.GetItemData(nSelType) : -1;
	if( nProcType == IntegratedStationProc )
	{
		InitStationLayout();
	}
	LoadTree();
}
/*
void CProcPropDlg::CaculateConveyorCapacity(  Conveyor* _pConveyor )
{
	Path* pServicePath = _pConveyor->serviceLocationPath();
	ASSERT( pServicePath );
	//pServicePath->

	long lCapacity = 3;

	MiscProcessorData* pMiscDB = GetInputTerminal()->miscData->getDatabase( ConveyorProc );
	if( pMiscDB == NULL )
		return;
	MiscData* pMiscData = new MiscConveyorData;
	pMiscData->SetQueueCapacity( lCapacity );
	MiscDataElement* pDataElement = new MiscDataElement( *_pConveyor->getID() );
	pDataElement->setData( pMiscData );	
	pMiscDB->addItem( pDataElement );
	GetInputTerminal()->miscData->saveDataSet( GetProjPath(), true );	
}
*/
bool CProcPropDlg::SetProcessorValue(Processor* pProcessor)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	int nProcType=pProcessor->getProcessorType();

	// set service location
	if( m_vServiceLocation.size() > 0 )
	{
	    Point ptList[MAX_POINTS];
		int nCount = m_vServiceLocation.size();
		for( int i=0; i<nCount; i++ )
		{
			ARCVector3 v3D = m_vServiceLocation[i];
			ptList[i].setX( v3D[VX] );
			ptList[i].setY( v3D[VY] );

			//change by Mark Chen
			//if( (nProcType == ConveyorProc ||nProcType == StairProc )
			if( (nProcType == ConveyorProc ||nProcType == StairProc || nProcType == EscalatorProc )
				&& m_bZPosHasBeenChanged  )
			{
				ptList[i].setZ( GetZValue((float)v3D[VZ]) * SCALE_FACTOR );
				if (StairProc == GetCurrentProcType() || EscalatorProc == GetCurrentProcType() || ConveyorProc == GetCurrentProcType())
				{
					ptList[i].setZ( v3D[VZ]);
				}
			}
			else
			{
				ptList[i].setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );
			}

		}
		
		pProcessor->initServiceLocation( nCount, ptList );

		if( nProcType == ConveyorProc )
		{
			((Conveyor*)pProcessor)->CalculateTheBisectLine();			
		}
		else if( nProcType == StairProc )
		{
			((Stair*)pProcessor)->CalculateTheBisectLine();
		}
		//add by Mark
		else if( nProcType == EscalatorProc )
		{
			((Escalator*)pProcessor)->CalculateTheBisectLine();
		}
	}
	else if(pProcessor!=m_pProc&&m_pProc!=NULL)
	{

		int nCount=m_pProc->serviceLocationLength();
		pProcessor->initServiceLocation( nCount, m_pProc->serviceLocationPath()->getPointList());
		if( nProcType == ConveyorProc )
		{
			((Conveyor*)pProcessor)->CalculateTheBisectLine();
		}

	}

	
	// set in constraint.
	if( nProcType != BarrierProc && nProcType != ConveyorProc )
	{
			
		if( m_vInConstraint.size() > 0)
		{
			Point ptList[MAX_POINTS];
			int nCount = m_vInConstraint.size();
			for( int i=0; i<nCount; i++ )
			{
				ARCVector3 v3D = m_vInConstraint[i];
				ptList[i].setX( v3D[VX] );
				ptList[i].setY( v3D[VY] );
				//the In/Out/Queue constraint's floor information is deference from the common proc,
				//because the Processors's In/out/Queue constraint and the service location maybe not on the same floor
				//
				if(nProcType == ConveyorProc || nProcType == StairProc || nProcType == EscalatorProc)
				{
					ptList[i].setZ( GetZValue((float)v3D[VZ]) * SCALE_FACTOR );
				}
				else
				{
					ptList[i].setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );
				}
			}

			pProcessor->initInConstraint( nCount, ptList );
				
		}	
		else if(pProcessor!=m_pProc&&m_pProc!=NULL)
		{
			Path* pPath = m_pProc->inConstraint();
			pProcessor->initInConstraint(m_pProc->inConstraintLength(),pPath->getPointList());
		}
	}

	// set out constraint

	if( nProcType != DepSourceProc && nProcType != BarrierProc && nProcType != ConveyorProc )
		{

			if( m_vOutConstraint.size() > 0 )
			{
				Point ptList[MAX_POINTS];
				int nCount = m_vOutConstraint.size();
				for( int i=0; i<nCount; i++ )
				{
					ARCVector3 v3D = m_vOutConstraint[i];
					ptList[i].setX( v3D[VX] );
					ptList[i].setY( v3D[VY] );
					if(nProcType == ConveyorProc || nProcType == StairProc || nProcType == EscalatorProc)
					{
						ptList[i].setZ( GetZValue((float)v3D[VZ]) * SCALE_FACTOR );
					}
					else
					{
						ptList[i].setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );
					}
				}
				pProcessor->initOutConstraint( nCount, ptList );
			}
			else if(pProcessor!=m_pProc&&m_pProc!=NULL)
			{
				pProcessor->initOutConstraint(m_pProc->outConstraintLength(),m_pProc->outConstraint()->getPointList());
			}
		}

		if( nProcType == BillboardProcessor )
		{

			//if( m_vBillboardExposureArea.size() > 0 )
			//{
			//	Point ptList[MAX_POINTS];
			//	int nCount = m_vBillboardExposureArea.size();
			//	for( int i=0; i<nCount; i++ )
			//	{
			//		ARCVector3 v3D = m_vBillboardExposureArea[i];
			//		ptList[i].setX( v3D[VX] );
			//		ptList[i].setY( v3D[VY] );
			//		ptList[i].setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );
			//	}

			//	((BillboardProc*) pProcessor)->InitExpsurePath( nCount, ptList );
			//}
			//else if(pProcessor!=m_pProc&&m_pProc!=NULL)
			//{
			//	pProcessor->initOutConstraint(m_pProc->outConstraintLength(),m_pProc->outConstraint()->getPointList());
			//}
		}
	//set queue
	if( nProcType != DepSinkProc && nProcType != HoldAreaProc && nProcType != BarrierProc && nProcType != ConveyorProc )
	{
	
		if( m_vQueieList.size() > 0)
		{
			Point ptList[MAX_POINTS];
			int nCount = m_vQueieList.size();
			for( int i=0; i<nCount; i++ )
			{
				ARCVector3 v3D = m_vQueieList[i];
				ptList[i].setX( v3D[VX] );
				ptList[i].setY( v3D[VY] );
				if(nProcType == ConveyorProc || nProcType == StairProc || nProcType == EscalatorProc)
				{
					ptList[i].setZ( GetZValue((float)v3D[VZ]) * SCALE_FACTOR );
				}
				else
				{
					ptList[i].setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );
				}
			}
			if(m_nFixed == -1)
			{
				pProcessor->initQueue( m_pProc->queueIsFixed(), nCount, ptList );
				if(m_pProc->queueIsFixed())
					SetEntryPointIndex(pProcessor) ;
			}
			else
			{
				pProcessor->initQueue( m_nFixed, nCount, ptList );	
			   if(m_nFixed)
                     SetEntryPointIndex(pProcessor) ;
			}
		   
		}
		else
		{
			Path* pPath = m_pProc->queuePath();
			Path temp;
			int iPointNum=0;
			if( pPath )
			{
				iPointNum=pPath->getCount();
				temp.init(iPointNum,pPath->getPointList());
			}
				

			if(-1==m_nFixed)
			{
				if(pProcessor!=m_pProc)
				{
					pProcessor->initQueue( m_pProc->queueIsFixed(), iPointNum,temp.getPointList());
				}
				if(m_pProc->queueIsFixed())
					SetEntryPointIndex(m_pProc) ;
			}
			else
			{
				if(pProcessor!=m_pProc)
					pProcessor->initQueue( m_nFixed, iPointNum,temp.getPointList());
				else
					m_pProc->initQueue( m_nFixed, iPointNum,temp.getPointList());
				if(m_nFixed)
                    SetEntryPointIndex(m_pProc) ;
			}		
		}

	}


	if( nProcType == BaggageProc )
	{
		if( m_vBaggageBarrier.size() > 0 )
		{
		    Point ptList[MAX_POINTS];
			int nCount = m_vBaggageBarrier.size();
			for( int i=0; i<nCount; i++ )
			{
				ARCVector3 v3D = m_vBaggageBarrier[i];
				ptList[i].setX( v3D[VX] );
				ptList[i].setY( v3D[VY] );
				ptList[i].setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );
			}
			
			((BaggageProcessor*)pProcessor)->GetBarrier()->initServiceLocation( nCount, ptList );
			
		}
		else if(pProcessor!=m_pProc&&m_pProc!=NULL)
		{
			int nCount=	((BaggageProcessor*)m_pProc)->GetBarrier()->serviceLocationLength();
			((BaggageProcessor*)pProcessor)->GetBarrier()->initServiceLocation( nCount, ((BaggageProcessor*)m_pProc)->GetBarrier()->serviceLocationPath()->getPointList() );
		}


		if( m_vBaggagMovingPath.size() > 0 )
		{
			if( m_vBaggagMovingPath.size() <3 )
			{
				AfxMessageBox("Baggage Moving Path must have at least 3 points !");
				return false;
			}
		    Point ptList[MAX_POINTS];
			int nCount = m_vBaggagMovingPath.size();
			for( int i=0; i<nCount; i++ )
			{
				ARCVector3 v3D = m_vBaggagMovingPath[i];
				ptList[i].setX( v3D[VX] );
				ptList[i].setY( v3D[VY] );
				ptList[i].setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );
			}
			
			((BaggageProcessor*)pProcessor)->GetPathBagMovingOn()->init( nCount, ptList );
			
		}
		else if(pProcessor!=m_pProc&&m_pProc!=NULL)
		{
			int nCount=	((BaggageProcessor*)m_pProc)->GetPathBagMovingOn()->getCount();
			((BaggageProcessor*)pProcessor)->GetPathBagMovingOn()->init(nCount, ((BaggageProcessor*)m_pProc)->GetPathBagMovingOn()->getPointList() );
		}
	}

	if (nProcType == RetailProc)
	{
		if (!m_vStoreArea.empty())
		{
			if( m_vStoreArea.size() <3 )
			{
				AfxMessageBox("Retail store area path must have at least 3 points !");
				return false;
			}
			Point ptList[MAX_POINTS];
			int nCount = m_vStoreArea.size();
			for( int i=0; i<nCount; i++ )
			{
				ARCVector3 v3D = m_vStoreArea[i];
				ptList[i].setX( v3D[VX] );
				ptList[i].setY( v3D[VY] );
				ptList[i].setZ( pDoc->m_nActiveFloor * SCALE_FACTOR );
			}
			((RetailProcessor*)pProcessor)->initStoreArea(nCount,ptList);
		}
	}

	return true;

}




void CProcPropDlg::PreProcessName()
{
	m_csLevel1.TrimLeft();
	m_csLevel1.TrimRight();
	m_csLevel1.Remove( _T(',') );
	m_csLevel1.Replace(_T(' '), _T('_'));
	m_csLevel1.MakeUpper();

	m_csLevel2.TrimLeft();
	m_csLevel2.TrimRight();
	m_csLevel2.Remove( _T(',') );
	m_csLevel2.Replace(_T(' '), _T('_'));
	m_csLevel2.MakeUpper();

	m_csLevel3.TrimLeft();
	m_csLevel3.TrimRight();
	m_csLevel3.Remove( _T(',') );
	m_csLevel3.Replace(_T(' '), _T('_'));
	m_csLevel3.MakeUpper();

	m_csLevel4.TrimLeft();
	m_csLevel4.TrimRight();
	m_csLevel4.Remove( _T(',') );
	m_csLevel4.Replace(_T(' '), _T('_'));
	m_csLevel4.MakeUpper();
}

void CProcPropDlg::BuildAllSubProcessorID()
{
		typedef std::vector< Processor > DOOR;  
		
		ProcessorID id;
		id.SetStrDict( GetInputTerminal()->inStrDict );
		
		CString tempStr;
		CString nameStr;
		CString sCarName;
        CAR _allCar;
		 getStationLayout()->GetAllCars( _allCar );
		CAR::const_iterator iter=_allCar.begin();
		CAR::const_iterator iterLast=_allCar.end();
		for(int i=0; iter!=iterLast;++i,++iter)
		{
			nameStr="CAR";
			tempStr.Format("%d",i);
			nameStr+=tempStr;
			sCarName = nameStr;
			id.setID(nameStr + "-CARAREA");
			(*iter)->m_carArea->init(id);
			for(int  j=0;j<static_cast<int>((*iter)->m_exitDoors.size());++j)		
			{
			  tempStr.Format("%d",j);
			  nameStr=sCarName + "-EXITDOOR" + tempStr;
			  id.setID(nameStr);
			  (*iter)->m_exitDoors[j]->init(id);
			}

			for(j=0;j<static_cast<int>((*iter)->m_entryDoors.size());++j)
			{
			  tempStr.Format("%d",j);
			  nameStr=sCarName+"-ENTRYDOOR" + tempStr;
			  id.setID(nameStr);
			   (*iter)->m_entryDoors[j]->init(id);
			}
			
		}
}

void CProcPropDlg::InitStationLayout()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	m_tempStation.SetTerminal( &(pDoc->GetTerminal()) );
	//Point servicePoint( m_stationServiceLocation[VX], m_stationServiceLocation[VY], 0 );
	Point servicePoint( m_stationServiceLocation[VX], m_stationServiceLocation[VY], m_stationServiceLocation[VZ] );
	m_tempStation.initServiceLocation( 1, &servicePoint );
	m_tempStation.InitLocalData();

	if( m_pProc )
	{
	//	m_nTempStationOrientation = ((IntegratedStation*)m_pProc)->GetOrientation();
		IntegratedStation* pIntegratedStation = (IntegratedStation*)m_pProc;
		m_tempStation.SetOrientation( pIntegratedStation->GetOrientation());

		*getStationLayout() = *(pIntegratedStation->GetStationLayout());

		Point adjustPoint=pIntegratedStation->GetStationLayout()->GetAdjustPoint();
		DistanceUnit _x= m_stationServiceLocation[VX] - adjustPoint.getX();
		DistanceUnit _y= m_stationServiceLocation[VY] - adjustPoint.getY();
		getStationLayout()->SetAdjustPoint(servicePoint);

		getStationLayout()->DoOffSet( _x, _y );

	}

	pDoc->m_tempProcInfo.SetIntegratedStation(&m_tempStation);

	if(m_pProc == NULL)
	{
		CStationLayout* pStationLayout=GetInputTerminal()->pStationLayoutSample;
		*getStationLayout() = *pStationLayout;
		Point adjustPoint=pStationLayout->GetAdjustPoint();
		DistanceUnit _x= m_stationServiceLocation[VX] - adjustPoint.getX();
		DistanceUnit _y= m_stationServiceLocation[VY] - adjustPoint.getY();
		getStationLayout()->SetAdjustPoint(servicePoint);

		getStationLayout()->DoOffSet( _x, _y );

	}



	if( getStationLayout()->GetCarCount() <= 0 || getStationLayout()->GetEntryDoorNumber() <= 0 
		  || getStationLayout()->GetExitDoorNumber() <= 0 )
	{
		getStationLayout()->SetCarNumber( 1 );
		getStationLayout()->SetExitDoorNumber( 1);
		getStationLayout()->SetEntryDoorNumber( 1 );
		getStationLayout()->ComputeLayoutInfo();
	}



}
void CProcPropDlg::InitIntegratedProcessor()
{
	//m_bIsCommomProcessor=false;
	m_btnOffgate.ShowWindow(SW_HIDE);
	InitStationLayout();
}

void CProcPropDlg::OnCancel() 
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	
	pDoc->m_tempProcInfo.SetIntegratedStation(NULL);
	
	//clear Doc's related data member
	pDoc->m_tempProcInfo.ClearAll();
	
	CDialog::OnCancel();
}

void CProcPropDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	const int nIconWidth = 80;
	
	CRect btnRc, treeRect,comboType,comboLevel,staticProcGroup;

	GetDlgItem(IDC_STATIC_NAME)->MoveWindow( 10,7,100,15 );
	

	//int iLevelWidth = (cx-15-15)/4;	//comment by kevin
	int iLevelWidth = (cx-15-15 - nIconWidth)/4;


	m_comboLevel1.MoveWindow(10,25,iLevelWidth,12);
	GetDlgItem(IDC_STATIC_LINE1)->MoveWindow( 10+iLevelWidth,32,5,3 );
	
	m_comboLevel2.MoveWindow(10+iLevelWidth+5,25,iLevelWidth,12 );
	GetDlgItem(IDC_STATIC_LINE2)->MoveWindow( 10+2*iLevelWidth+5,32,5,3 );

	m_comboLevel3.MoveWindow(10+2*iLevelWidth+10, 25, iLevelWidth,12 );
	GetDlgItem(IDC_STATIC_LINE3)->MoveWindow( 10+3*iLevelWidth+10,32,5,3 );

	m_comboLevel4.MoveWindow(10+3*iLevelWidth+15, 25, iLevelWidth,12 );

	m_cmbShape.MoveWindow(10 + 4 * iLevelWidth + 15, 25, nIconWidth, 80);

	GetDlgItem(IDC_STATIC_TYPE)->MoveWindow( 10,50,24,15 );
	
//	m_comboType.GetWindowRect( comboType );
	//m_comboType.MoveWindow( 10 ,68,cx-15,12);
	m_comboType.MoveWindow( 10 ,68,cx-17-nIconWidth,12);
	
	GetDlgItem(IDC_STATIC_PROPER)->MoveWindow( 10,93,50,12 );
//	m_treeProp.GetWindowRect( treeRect );
	m_treeProp.MoveWindow( 10,120, cx-15,cy-175 );


	m_btnOffgate.MoveWindow( 10, cy-35, 150, 10 );
//	m_butACStandGate.MoveWindow( 160, cy-35, 150, 10 );
	m_btnEmergent.MoveWindow( 10, cy-20, 150, 10 );
	m_btnOk.GetWindowRect( btnRc );
	
	m_btnOk.MoveWindow( cx-35-2*btnRc.Width(),cy-btnRc.Height()-10,btnRc.Width(),btnRc.Height() );
	m_btnCancle.MoveWindow( cx-15-btnRc.Width(),cy-btnRc.Height()-10,btnRc.Width(),btnRc.Height() );
	//m_btnCancle.MoveWindow( cx-54-25, cy-25, 75,30 );
	//m_btnOk.MoveWindow( cx-2*54-25, cy-25, 75,30 );
	
	CRect rect( 0,0,cx,cy );
	InvalidateRect( rect );
	
}


void CProcPropDlg::OnCarCapacity() 
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( getStationLayout()->GetCarCapacity() );
	m_treeProp.SetSpinRange( 0,200);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

LRESULT CProcPropDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{


	if( message == WM_INPLACE_SPIN )
	{
		int nSelType = m_comboType.GetCurSel();
		int nProcType = nSelType >=0 ? m_comboType.GetItemData(nSelType) : -1;
		
		LPSPINTEXT pst = (LPSPINTEXT) lParam;

		bool bMustRotat = false;
		if( nProcType == Elevator )
		{	
			if(m_hRClickItem==m_hWaitAreaLength)
				m_tempElevator.SetPlatFormLength(UNITSMANAGER->UnConvertLength(pst->iPercent));
			else if(m_hRClickItem==m_hWaitAreaWidth)
				m_tempElevator.SetPlatFormWidth(UNITSMANAGER->UnConvertLength(pst->iPercent));
			else if(m_hRClickItem==m_hLiftNOL)
				m_tempElevator.SetNumberOfLift(pst->iPercent);
			else if(m_hRClickItem==m_hLiftLength)
				m_tempElevator.SetElevatorAreaLength(UNITSMANAGER->UnConvertLength(pst->iPercent));
			else if(m_hRClickItem==m_hLiftWidth)
				m_tempElevator.SetElevatorAreaWidth(UNITSMANAGER->UnConvertLength(pst->iPercent));
			LoadTree();
			return TRUE;
		}

		else if (nProcType == BillboardProcessor )
		{
			if(m_hRClickItem == m_hBillboardHeight)
			{
				m_tempBillBoardProc.setHeight(UNITSMANAGER->UnConvertLength(pst->iPercent));
			}
			else if (m_hRClickItem == m_hBillboardThickness) 
			{
				m_tempBillBoardProc.setThickness(UNITSMANAGER->UnConvertLength(pst->iPercent));
			}
			else if (m_hRClickItem == m_hBillBoardCoefficient) 
			{
				double dCoefficent = pst->iPercent / 100.0;
//				m_tempBillBoardProc.setCoefficient(dCoefficent);
			}
			else if (m_hRClickItem == m_hBillboardAngle)
			{
//				m_tempBillBoardProc.setAngle(pst->iPercent);
			}
			LoadTree();
			return TRUE;
		}
		else if (nProcType == BridgeConnectorProc)
		{
			HTREEITEM hParItem = m_treeProp.GetParentItem(m_hRClickItem);
			if (hParItem == NULL)
				m_nBridgeConnectPointCount = pst->iPercent;
			else
			{
				int idx = m_treeProp.GetItemData(hParItem);
				BridgeConnector::ConnectPoint& conPoint = m_vBridgeConnectPoints[idx];
				CString sLabel = m_treeProp.GetItemText(m_hRClickItem);
				if (sLabel.Find("Width") >=0)
					conPoint.m_dWidth = UNITSMANAGER->UnConvertLength(pst->iPercent);
				else
					conPoint.m_dLength = UNITSMANAGER->UnConvertLength(pst->iPercent);
			}

			LoadTree();
			return TRUE;
		}
		//	add by Kevin
		if (m_hRClickItem == m_hWidth)
		{
			if (m_pProc)
			{
				if (m_pProc->getProcessorType() == RunwayProcessor)
					((RunwayProc*)m_pProc)->SetWidth( UNITSMANAGER->UnConvertLength(pst->iPercent) );

				if (m_pProc->getProcessorType() == TaxiwayProcessor)
					((TaxiwayProc*)m_pProc)->SetWidth( UNITSMANAGER->UnConvertLength(pst->iPercent) );
			}
			else
				m_dWidth = UNITSMANAGER->UnConvertLength(pst->iPercent);

		    LoadTree();
			return TRUE;
		}
		
		if (m_hRClickItem == m_hThresholdA || m_hRClickItem == m_hThresholdB)
		{
			if (m_pProc)
			{
				if (m_pProc->getProcessorType() == RunwayProcessor)
				{
					if (m_hRClickItem == m_hThresholdA)
						((RunwayProc*)m_pProc)->SetThreshold1 ( UNITSMANAGER->UnConvertLength(pst->iPercent) );
					else
						((RunwayProc*)m_pProc)->SetThreshold2 ( UNITSMANAGER->UnConvertLength(pst->iPercent) );
				}
			}
			else
			{
				if (m_hRClickItem == m_hThresholdA)
					m_dThreshold = UNITSMANAGER->UnConvertLength(pst->iPercent);
				else
					m_dThreshold2 = UNITSMANAGER->UnConvertLength(pst->iPercent);
			}

			LoadTree();
			return TRUE;
		}

		if (m_hRClickItem == m_hAutomaticallyNumber)
		{
			if (m_pProc)
			{
				if (m_pProc->getProcessorType() == NodeProcessor)
					((NodeProc*)m_pProc)->SetNumber ( pst->iPercent );
				else if (m_pProc->getProcessorType() == DeiceBayProcessor)
					((DeiceBayProc*)m_pProc)->SetNumber ( pst->iPercent );
				//else if (m_pProc->getProcessorType() == FixProcessor)
				//	((FixProc*)m_pProc)->SetNumber ( pst->iPercent );
			}
			else
				m_nAutoNumber = pst->iPercent;

			LoadTree();
			return TRUE;
		}

		if (m_hRClickItem == m_hLowerLimit)
		{
			if (m_pProc)
			{
				if (m_pProc->getProcessorType() == FixProcessor)
					((FixProc*)m_pProc)->SetLowerLimit ( UNITSMANAGER->UnConvertLength(pst->iPercent) );
			}
			else
				m_dLowerLimit = UNITSMANAGER->UnConvertLength(pst->iPercent);

			LoadTree();
			return TRUE;
		}

		if (m_hRClickItem == m_hUpperLimit)
		{
			if (m_pProc)
			{
				if (m_pProc->getProcessorType() == FixProcessor)
					((FixProc*)m_pProc)->SetUpperLimit ( UNITSMANAGER->UnConvertLength(pst->iPercent) );
			}
			else
				m_dUpperLimit = UNITSMANAGER->UnConvertLength(pst->iPercent);

			LoadTree();
			return TRUE;
		}

		if( m_hRClickItem  == m_hWidthItem )
		{
			if( m_pProc )
			{
				if( m_pProc->getProcessorType() == ConveyorProc )
				{
					((Conveyor*)m_pProc)->SetWidth( UNITSMANAGER->UnConvertLength(pst->iPercent) );
					((Conveyor*)m_pProc)->CalculateTheBisectLine();		
				}
				else if( m_pProc->getProcessorType() == StairProc )
				{
					((Stair*)m_pProc)->SetWidth( UNITSMANAGER->UnConvertLength(pst->iPercent) );
					((Stair*)m_pProc)->CalculateTheBisectLine();		
				}
				//add by Mark
				else if( m_pProc->getProcessorType() == EscalatorProc )
				{
					((Escalator*)m_pProc)->SetWidth( UNITSMANAGER->UnConvertLength(pst->iPercent) );
					((Escalator*)m_pProc)->CalculateTheBisectLine();	
				}
				else if( m_pProc->getProcessorType() == DeiceBayProcessor){
					((DeiceBayProc*)m_pProc)->SetWindspan( UNITSMANAGER->UnConvertLength(pst->iPercent));
				}
			}
			else
			{
				m_dConveyWidth = UNITSMANAGER->UnConvertLength(pst->iPercent);
			}

			LoadTree();
			return TRUE;
		}
		else if( m_hRClickItem == m_hLengthItem )	// zoom conveyor's length
		{
			
			
			DistanceUnit _dNewLength = UNITSMANAGER->UnConvertLength( pst->iPercent );
			if(nProcType == DeiceBayProcessor){
				if(m_pProc){ ((DeiceBayProc*)m_pProc)->SetLength(_dNewLength);}
				m_dProcessorLength = _dNewLength;
				LoadTree();
				return TRUE;
			}
			if( m_dProcessorLength == _dNewLength )	// need not to change length
				return TRUE;;
			
			bool _bZoomResult = true;
			CString _strFailedReason;
			
			CTermPlanDoc* _pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
			std::vector<double> vRealAltitude;
			_pDoc->GetCurModeFloorsRealAltitude(vRealAltitude);
			
			if( m_vServiceLocation.size()>0 )
			{
				Path _tmpPath;
				_tmpPath.init( m_vServiceLocation.size() );

				ARCVector3 v3D;
				for( int i=0; i< static_cast<int>(m_vServiceLocation.size()); i++ )
				{
					v3D = m_vServiceLocation[i];
					if( !m_bZPosHasBeenChanged )
						_tmpPath[i] = Point( v3D[VX], v3D[VY],m_stationServiceLocation[VZ] );
					else
						_tmpPath[i] = Point( v3D[VX], v3D[VY],v3D[VZ] );
				}

				_bZoomResult =  _tmpPath.zoomPathByLength( _dNewLength, _strFailedReason ,vRealAltitude);
				if( _bZoomResult )
				{
					for( int i=0; i<static_cast<int>(m_vServiceLocation.size()); i++ )
					{
						v3D[VX] = _tmpPath[i].getX();
						v3D[VY] = _tmpPath[i].getY();
						v3D[VZ] = _tmpPath[i].getZ();
						m_vServiceLocation[i] = v3D;
					}
					m_bZPosHasBeenChanged = true;
				}
			}
			else if( m_pProc )
			{
				_bZoomResult = m_pProc->serviceLocationPath()->zoomPathByLength( _dNewLength, _strFailedReason,vRealAltitude );
			}
			
			if( !_bZoomResult )
				MessageBox( _strFailedReason, NULL, MB_OK|MB_ICONINFORMATION );

			LoadTree();
			return TRUE;		
		}

		else if( m_hRClickItem == m_hAngleItem )	// change path's angle
		{
			double _dNewAngle = pst->iPercent;
			if( m_dPathAngle == _dNewAngle )
				return TRUE;
			
			if( m_vServiceLocation.size()>0 )
			{
				Path _tmpPath;
				_tmpPath.init( m_vServiceLocation.size() );
				
				ARCVector3 v3D;
				for( int i=0; i< static_cast<int>(m_vServiceLocation.size()); i++ )
				{
					v3D = m_vServiceLocation[i];
					if( !m_bZPosHasBeenChanged )
						_tmpPath[i] = Point( v3D[VX], v3D[VY],m_stationServiceLocation[VZ] );
					else
						_tmpPath[i] = Point( v3D[VX], v3D[VY],v3D[VZ] );
				}
				convertPathByAngle( _tmpPath,_dNewAngle );
				
				for( i=0; i<static_cast<int>(m_vServiceLocation.size()); i++ )
				{
					v3D[VX] = _tmpPath[i].getX();
					v3D[VY] = _tmpPath[i].getY();
					v3D[VZ] = _tmpPath[i].getZ();
					m_vServiceLocation[i] = v3D;
				}
				m_bZPosHasBeenChanged = true;
			}
			else if( m_pProc )
			{

				convertPathByAngle( *(m_pProc->serviceLocationPath()), _dNewAngle );
			}
			
			LoadTree();
			return TRUE;		

		}
		else if ( m_hRClickItem == m_hPlatFormWidth )
		{
			getStationLayout()->SetPlatformWidth( UNITSMANAGER->UnConvertLength(pst->iPercent ));
			m_iSelectType =1;
			CString str = _T("");
			str.Format("Platform Width  (m)  (%.2f)",(double)(pst->iPercent));
			m_treeProp.SetItemText(m_hRClickItem,str);
		}
		else if ( m_hRClickItem == m_hPlatFormLength )
		{
			getStationLayout()->SetPlatformLength( UNITSMANAGER->UnConvertLength(pst->iPercent));
			m_iSelectType =2;
			m_iUpdateAllRailWay = 1;
			CString str = _T("");
			str.Format("Platform Length  (m)  (%.2f)",(double)(pst->iPercent));
			m_treeProp.SetItemText(m_hRClickItem,str);
		}
		else if ( m_hRClickItem == m_hCarWidth )
		{
			getStationLayout()->SetCarWidth( UNITSMANAGER->UnConvertLength(pst->iPercent));
			m_iSelectType =3;
			CString str = _T("");
			str.Format("Car Width  (m)  (%.2f)",(double)(pst->iPercent));
			m_treeProp.SetItemText(m_hRClickItem,str);
		}
		else if ( m_hRClickItem == m_hCarLength )
		{
			getStationLayout()->SetCarLength( UNITSMANAGER->UnConvertLength(pst->iPercent));
			m_iSelectType =4;
			CString str = _T("");
			str.Format("Car Length  (m)  (%.2f)",(double)(pst->iPercent));
			m_treeProp.SetItemText(m_hRClickItem,str);
		}
		else if ( m_hRClickItem == m_hCarNum )
		{
			getStationLayout()->SetCarNumber( pst->iPercent );
			m_iSelectType =5;
			CString str = _T("");
			str.Format("Car number  (%d)",pst->iPercent);
			m_treeProp.SetItemText(m_hRClickItem,str);
		}
		else if( m_hRClickItem == m_hCarCapacity ){
			getStationLayout()->SetCarCapacity( pst->iPercent );
			m_iSelectType =6;
			CString str = _T("");
			str.Format("Car Capacity  (%d)",pst->iPercent);
			m_treeProp.SetItemText(m_hRClickItem,str);
		}
		else if( m_hRClickItem == m_hEntryDoorNum )
		{
			getStationLayout()->SetEntryDoorNumber( pst->iPercent );
			m_iSelectType =7;
			CString str = _T("");
			str.Format("Door Number  (%d)",pst->iPercent);
			m_treeProp.SetItemText(m_hRClickItem,str);
		}
		else if ( m_hRClickItem == m_hExitDoorNum )
		{
			getStationLayout()->SetExitDoorNumber( pst->iPercent );
			m_iSelectType =8;
			CString str = _T("");
			str.Format("Door Number  (%d)",pst->iPercent);
			m_treeProp.SetItemText(m_hRClickItem,str);
		}
		else if ( m_hRClickItem == m_hOrientation )
		{
			m_tempStation.SetOrientation( pst->iPercent );
			m_iSelectType =9;
			bMustRotat = true;
			m_iUpdateAllRailWay = 0;
			CString str = _T("");
			str.Format("Orientation (deg)   ( %d )",pst->iPercent);
			m_treeProp.SetItemText(m_hRClickItem,str);

		}
		else if( m_hRClickItem == m_hHeight){
			if (m_pProc)
			{
				if (m_pProc->getProcessorType() == ContourProcessor)
					((ContourProc *)m_pProc)->setHeight(UNITSMANAGER->UnConvertLength(pst->iPercent) );
			}
			else
				m_dHeight = UNITSMANAGER->UnConvertLength(pst->iPercent);

			LoadTree();
			return TRUE;
		
		}		
		else if(m_hRClickItem == m_hMinAlt){
			double dUnConvertedLength = UNITSMANAGER->UnConvertLength(pst->iPercent);
			if(m_pProc){
				if(m_pProc->getProcessorType() == HoldProcessor)
					((HoldProc*)m_pProc)->setMinAltitude(dUnConvertedLength);
				if(m_pProc->getProcessorType() == SectorProcessor)
					((SectorProc*)m_pProc)->setMinAlt(dUnConvertedLength);
			}
			else{
				m_dminAlt = dUnConvertedLength;
			}
			LoadTree();
			return TRUE;			
		}
		else if(m_hRClickItem == m_hMaxAlt){
			double dUnConvertedLength = UNITSMANAGER->UnConvertLength(pst->iPercent);
			if(m_pProc){
				if(m_pProc->getProcessorType() == HoldProcessor)
					((HoldProc*)m_pProc)->setMaxAltitude(dUnConvertedLength);
				if(m_pProc->getProcessorType() == SectorProcessor)
					((SectorProc*)m_pProc)->setMaxAlt(dUnConvertedLength);
			}
			else{
				m_dmaxAlt = dUnConvertedLength;
			}
			LoadTree();
			return TRUE;
		}
		else if(m_hRClickItem == m_hBandInterval){
			double dUnConvertedLength = (pst->iPercent);
			if(m_pProc){
				if(m_pProc->getProcessorType() == SectorProcessor)
					((SectorProc*)m_pProc)->setAltBandInterval(dUnConvertedLength);			
			}
			else{
				m_dbandinterval = dUnConvertedLength;
			}
			LoadTree();
			return TRUE;
		}
		else if(m_hRClickItem == m_hInBound){
			if(m_pProc){
				if(m_pProc->getProcessorType() == HoldProcessor)
					((HoldProc*)m_pProc)->setInBoundDegree(pst->iPercent);
			}
			else{
				m_nInBoundDegree = pst->iPercent;
			}
			LoadTree();
			return TRUE;
		}
		else if(m_hRClickItem == m_hOutBoundLegmin){
			if(m_pProc){
				if(m_pProc->getProcessorType() == HoldProcessor)
					((HoldProc*)m_pProc)->setOutBoundLegMin(pst->iPercent);
			}
			else{
				m_dOutBoundmin = pst->iPercent;
			}
			LoadTree();
			return TRUE;
		}
		else if(m_hRClickItem == m_hOutBoundLegnm){
			if(m_pProc){
				if(m_pProc->getProcessorType() == HoldProcessor)
					((HoldProc*)m_pProc)->setOutBoundLegNm(UNITSMANAGER->UnConvertLength(pst->iPercent));
			}
			else{
				m_dOutBoundnm = UNITSMANAGER->UnConvertLength(pst->iPercent);
			}
			LoadTree();
			return TRUE;
		}else if(m_hRClickItem == m_hRight){
			if(m_pProc){
				if(m_pProc->getProcessorType() == HoldProcessor)
					((HoldProc*)m_pProc)->setRightTurn(m_bRight);
			}else{
				m_bRight = m_treeProp.m_comboBox.GetCurSel()==0?true:false;
			}
		}
		
			getStationLayout()->ComputeLayoutInfo();
			m_tempStation.UpdateTempInfo( getStationLayout() );
			if( bMustRotat )
			{
				m_tempStation.Rotate( pst->iPercent );
			//	m_nTempStationOrientation = pst->iPercent;
			}
			
	//	LoadTree();
		return TRUE;
	}
	else if (message == WM_INPLACE_EDIT)
	{
		CString strValue = *(CString*)lParam;

		if (m_hRClickItem == m_hMarkingA ||m_hRClickItem == m_hMarkingB)
		{
			if (m_pProc)
			{
				if (m_pProc->getProcessorType() == RunwayProcessor)
				{
					if (m_hRClickItem == m_hMarkingA)
						((RunwayProc*)m_pProc)->SetMarking1(strValue);
					else
						((RunwayProc*)m_pProc)->SetMarking2(strValue);
				}

				else if (m_pProc->getProcessorType() == TaxiwayProcessor)
					((TaxiwayProc*)m_pProc)->SetMarking(strValue);
			}
			else
			{
				if (m_hRClickItem == m_hMarkingB)
					m_strMarking2 = strValue;
				else
					m_strMarking = strValue;
			}
		}
		else if (m_hRClickItem == m_hBillBoardText)
		{
			if (strValue.GetLength() == 0)
				strValue = "No Text";
			m_tempBillBoardProc.setText(strValue);
		}

		LoadTree();
		return TRUE;
	}
	else if(message == WM_INPLACE_COMBO2)
	{
		if(m_hRClickItem == m_hFix)
		{
			int selitem = m_treeProp.m_comboBox.GetCurSel();
			if((int)m_strfixes.size()>selitem && selitem>=0)
			{
				m_strFix = m_strfixes[selitem];
			} 			
			if(m_pProc)
			{
				if(m_pProc->getProcessorType()== HoldProcessor)
				{
					((HoldProc*)m_pProc)->setFixName(m_strFix);
					((HoldProc*)m_pProc)->RefreshRelateFix(GetInputTerminal()->GetProcessorList());
				}
			}
	//		LoadTree();
			return TRUE;
		}
		else if(m_hRClickItem == m_hRight)
		{
			int selitem = m_treeProp.m_comboBox.GetCurSel();
			m_bRight = selitem==0?true:false;
			if(m_pProc)
			{
				if(m_pProc->getProcessorType() == HoldProcessor)
				{
					((HoldProc*)m_pProc)->setRightTurn(m_bRight);
				}
			}
			LoadTree();
			return TRUE;
		}
		else if (m_hRClickItem == m_hGateType)
		{
			CString strItemText = _T("");
			CString strTemp = _T("");
			int nItem = m_treeProp.m_comboBox.GetCurSel();
			GateType nGateType = nItem ==0?ArrGate:DepGate;
			if (m_pProc)
			{
				((GateProcessor*)m_pProc)->setGateType(nGateType);
			}
			switch(nGateType)
			{
			case ArrGate:
				strTemp = _T("ArrGate");
				break;
			case DepGate:
				strTemp = _T("DepGate");
				break;
			default:
				break;
			}
			m_emGateType = nGateType;
			strItemText.Format(_T("Gate Type : %s"),strTemp);
			m_treeProp.SetItemText(m_hRClickItem,strItemText);
		}
		else if (m_hRClickItem == m_hRetailType)
		{
			RetailProcessor::RetaiType emRetailType;
			int nSel = m_treeProp.m_comboBox.GetCurSel();
			emRetailType = RetailProcessor::RetaiType(nSel);
			if (m_pProc)
			{
				((RetailProcessor*)m_pProc)->SetRetailType(emRetailType);
			}
			CString strTemp;
			CString strItemText;
			switch(emRetailType)
			{
			case RetailProcessor::Retial_Shop_Section:
				strTemp = _T("Shop Section");
				break;
			case RetailProcessor::Retial_By_Pass:
				strTemp = _T("By Pass");
				break;
			case RetailProcessor::Retial_Check_Out:
				strTemp = _T("Check Out");
				break;
			default:
				break;
			}
			m_emRetailType = (int)emRetailType;
			strItemText.Format(_T("Retail Type : %s"),strTemp);
			m_treeProp.SetItemText(m_hRClickItem,strItemText);
		}
		else if (m_hRClickItem==m_hMinFloor)
		{
			int nSel = m_treeProp.m_comboBox.GetCurSel();
			if (nSel != LB_ERR)
			{
				m_tempElevator.SetMinFloor(nSel);
				m_tempElevator.GetWaitAreaPos().clear();

				for(int i=m_tempElevator.GetMinFloor()+1;i<=m_tempElevator.GetMaxFloor()+1;i++)
				{
					m_tempElevator.GetWaitAreaPos().push_back(TRUE);
				}
			}
			
			LoadTree();
			return TRUE;
		}
		else if(m_hRClickItem==m_hMaxFloor)
		{
			int nSel = m_treeProp.m_comboBox.GetCurSel();
			if (nSel != LB_ERR)
			{
				m_tempElevator.SetMaxFloor(nSel);
				m_tempElevator.GetWaitAreaPos().clear();
				for(int i=m_tempElevator.GetMinFloor()+1;i<=m_tempElevator.GetMaxFloor()+1;i++)
				{
					m_tempElevator.GetWaitAreaPos().push_back(TRUE);
				}
			}
			
			LoadTree();
			return TRUE;
		}
	}
	else if (message == UM_CEW_EDITSPIN_BEGIN)
	{
		m_treeProp.SelectItem(m_hRClickItem);
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( getStationLayout()->GetCarCount() );
		m_treeProp.SetSpinRange( 1,20);
		return TRUE;
//		m_treeProp.SpinEditLabel( m_hRClickItem );
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CProcPropDlg::OnCarNumbers() 
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( getStationLayout()->GetCarCount() );
	m_treeProp.SetSpinRange( 1,20);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CProcPropDlg::OnPlatformWidth() 
{
	m_treeProp.SetDisplayType( 1 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(getStationLayout()->GetPlatformWidth())) );
	m_treeProp.SetSpinRange( 1,10000);
	m_treeProp.SpinEditLabel( m_hRClickItem );
	
	
}

void CProcPropDlg::OnPlatformLength() 
{
	m_treeProp.SetDisplayType( 1 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(getStationLayout()->GetPlatformLength())) );
	m_treeProp.SetSpinRange( 1,10000);
	m_treeProp.SpinEditLabel( m_hRClickItem );
	
}

void CProcPropDlg::OnCarLength() 
{
	m_treeProp.SetDisplayType( 3 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(getStationLayout()->GetCarLength())) );
	m_treeProp.SetSpinRange( 1,10000);
	m_treeProp.SpinEditLabel( m_hRClickItem );
	
	
	
}

void CProcPropDlg::OnCarWidth() 
{
	m_treeProp.SetDisplayType( 3 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(getStationLayout()->GetCarWidth())) );
	m_treeProp.SetSpinRange( 1,10000);
	m_treeProp.SpinEditLabel( m_hRClickItem );
	
}

void CProcPropDlg::OnDefineEntryDoorNum() 
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( getStationLayout()->GetEntryDoorNumber());
	m_treeProp.SetSpinRange( 1,15);
	m_treeProp.SpinEditLabel( m_hRClickItem );
	
}

void CProcPropDlg::OnDefineExitDoorNumber() 
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( getStationLayout()->GetExitDoorNumber());
	m_treeProp.SetSpinRange( 1, 15);
	m_treeProp.SpinEditLabel( m_hRClickItem );	
}



void CProcPropDlg::OnExitdoorToggled() 
{
	getStationLayout()->SetExitDoorType	( !getStationLayout()->GetExitDoorType() );
	getStationLayout()->ComputeLayoutInfo();
	m_tempStation.UpdateTempInfo( getStationLayout() );
	LoadTree();
}

void CProcPropDlg::OnStationOrientation() 
{
	m_treeProp.SetDisplayType( 1 );
	m_treeProp.SetDisplayNum( m_tempStation.GetOrientation() );
	m_treeProp.SetSpinRange( 0,360 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}



void CProcPropDlg::OnProcpropDelete() 
{
	int nSelIdx = m_comboType.GetCurSel();
	if( nSelIdx < 0 )
		return ;

	int nProcType = m_comboType.GetItemData( nSelIdx );
	if( nProcType != IntegratedStationProc )
	{
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		Point nothing;
		if( m_hRClickItem == m_hInConstraint )
		{
			m_vInConstraint.clear();
			if( m_pProc )
				m_pProc->initInConstraint(0,&nothing);
			pDoc->m_tempProcInfo.SetTempInConstraint(m_vInConstraint);
		}
		else if( m_hRClickItem == m_hOutConstraint )
		{
			if( m_pProc )
				m_pProc->initOutConstraint(0,&nothing);
			m_vOutConstraint.clear();
			pDoc->m_tempProcInfo.SetTempOutConstrain(m_vOutConstraint);
		}
		else if( m_hRClickItem == m_hQueue )
		{
			if( m_pProc )
			{
				m_pProc->initQueue(0,0,&nothing);
			}
			m_vQueieList.clear();
			m_EntryPointIndex.clear();
			pDoc->m_tempProcInfo.SetTempQueue(m_vQueieList);
		}else if (m_hRClickItem == m_hFixXY)
		{
			
		}
		else if (m_hRClickItem == m_hExpSurePath)
		{
			if (nProcType == BillboardProcessor)
			{
//				m_tempBillBoardProc.InitExpsurePath(0,&nothing);
			}
		}

		LoadTree();
	}
	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CProcPropDlg::CheckProcessorName()
{
	//////////////////////////////////////////////////////////////////////////
	//  processor's name can't equal process' name
	CString strMsg;
	CString strProcName = m_csLevel1;

	if( !m_csLevel1.IsEmpty() )
	{
		if( GetInputTerminal()->m_pSubFlowList->IfProcessUnitExist( strProcName ) )
		{
			strMsg.Format("%s already exist in PROCESS list!\r\n You can't use the name.",strProcName );
			MessageBox(strMsg,"ERROR",MB_OK|MB_ICONINFORMATION );
			m_comboLevel1.SetFocus();
			return false;		
		}
	}
	
	if( !m_csLevel2.IsEmpty() )
	{
		strProcName = strProcName+"-"+m_csLevel2;
		if( GetInputTerminal()->m_pSubFlowList->IfProcessUnitExist( strProcName ) )
		{
			strMsg.Format("%s already exist in PROCESS list!\r\n You can't use the name.",strProcName );
			MessageBox(strMsg,"ERROR",MB_OK|MB_ICONINFORMATION );
			m_comboLevel1.SetFocus();
			return false;		
		}
	}

	if( !m_csLevel3.IsEmpty() )
	{
		strProcName = strProcName+ "-"+m_csLevel3;
		if( GetInputTerminal()->m_pSubFlowList->IfProcessUnitExist( strProcName ) )
		{
			strMsg.Format("%s already exist in PROCESS list!\r\n You can't use the name.",strProcName );
			MessageBox(strMsg,"ERROR",MB_OK|MB_ICONINFORMATION );
			m_comboLevel1.SetFocus();
			return false;		
		}
	}

	if( !m_csLevel4.IsEmpty() )
	{
		strProcName = strProcName+ "-"+m_csLevel4;
		if( GetInputTerminal()->m_pSubFlowList->IfProcessUnitExist( strProcName ) )
		{
			strMsg.Format("%s already exist in PROCESS list!\r\n You can't use the name.",strProcName );
			MessageBox(strMsg,"ERROR",MB_OK|MB_ICONINFORMATION );
			m_comboLevel1.SetFocus();
			return false;		
		}
	}

	return true;
}
/*
bool CProcPropDlg::IfProcType1CanChangeToType2( int _iType1, int _iType2 )const
{
	if( _iType1 == _iType2 )
	{
		return true;
	}
	
	if( (  _iType1 == PointProc || _iType1 == DepSourceProc || _iType1 == DepSinkProc
			|| _iType1 == GateProc || _iType1 == FloorChangeProc 
		)
		&& 
		(  _iType2 == PointProc || _iType2 == DepSourceProc || _iType2 == DepSinkProc
			|| _iType2 == GateProc || _iType2 == FloorChangeProc 
		) 
	  )
	{
		return true;
	}
	else
	{
		return false;	
	}
	
}
// return 0-->used by linkage 1--->used by gate assignment 
//reutrn -1--> not used by linkage or gate assigement
int CProcPropDlg::IfProcUsedByLinkageOrGateAssign( const ProcessorID& _procID )
{
	InputTerminal* pTerm =  GetInputTerminal();
	if( pTerm->miscData->IfUsedThisProcssor( _procID ) )
	{
		return 0;
	}

	if( pTerm->flightSchedule->IfUsedThisGate( _procID ) )
	{
		return 1;
	}
	//pTeml->
	return -1;
}
*/
void CProcPropDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 600;
	lpMMI->ptMinTrackSize.y = 400;
	CDialog::OnGetMinMaxInfo(lpMMI);
}


void CProcPropDlg::OnDefineWaitAreaLength()
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(m_tempElevator.GetPlatFormLength())) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
	
}
void CProcPropDlg::OnDefineWaitAreaWidth()
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(m_tempElevator.GetPlatFormWidth())) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CProcPropDlg::OnDefineNumberOfLift()
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( m_tempElevator.GetNumberOfLift());
	m_treeProp.SetSpinRange( 1,10);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CProcPropDlg::OnDefineLiftLength()
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(m_tempElevator.GetElevatorAreaLength())) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CProcPropDlg::OnDefineLiftWidth()
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(m_tempElevator.GetElevatorAreaWidth())) );
	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CProcPropDlg::OnDefineMinFloor()
{
// 	m_treeProp.SetDisplayType( 2 );
// 	m_treeProp.SetDisplayNum( m_tempElevator.GetMinFloor()+1);
// 	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
// 	int nFloorCount=pDoc->GetCurModeFloor().m_vFloors.size();
// 	ASSERT(nFloorCount>1);
// 	m_treeProp.SetSpinRange( 1, m_tempElevator.GetMaxFloor()-1+1);
// 	m_treeProp.SpinEditLabel( m_hRClickItem );
	std::vector<CString> strList;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	int nFloorCount=pDoc->GetCurModeFloor().m_vFloors.size();
	for (int i = 0; i < nFloorCount; i++)
	{
		CFloor2* pFloor = pDoc->GetCurModeFloor().m_vFloors.at(i);
		strList.push_back(pFloor->FloorName());
	}
	m_treeProp.SetComboString(m_hRClickItem,strList);
	int nMinFloor = m_tempElevator.GetMinFloor();
	m_treeProp.m_comboBox.SetCurSel(nMinFloor);

}

void CProcPropDlg::OnDefineMaxFloor()
{
// 	m_treeProp.SetDisplayType( 2 );
// 	m_treeProp.SetDisplayNum( m_tempElevator.GetMaxFloor()+1);
// 	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
// 	int nFloorCount=pDoc->GetCurModeFloor().m_vFloors.size();
// 	ASSERT(nFloorCount>1);
// 	m_treeProp.SetSpinRange( m_tempElevator.GetMinFloor()+1+1,nFloorCount );
// 	m_treeProp.SpinEditLabel( m_hRClickItem );
	std::vector<CString> strList;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	int nFloorCount=pDoc->GetCurModeFloor().m_vFloors.size();
	for (int i = 0; i < nFloorCount; i++)
	{
		CFloor2* pFloor = pDoc->GetCurModeFloor().m_vFloors.at(i);
		strList.push_back(pFloor->FloorName());
	}
	m_treeProp.SetComboString(m_hRClickItem,strList);
	int nMaxFloor = m_tempElevator.GetMaxFloor();
	m_treeProp.m_comboBox.SetCurSel(nMaxFloor);
}

void CProcPropDlg::OnDefineToggled()
{
	DWORD dwItemData=m_treeProp.GetItemData(m_hRClickItem);
	BOOL bValue=m_tempElevator.GetPosOfWaitArea(dwItemData);
	bValue=!bValue;
	m_tempElevator.SetPosOfWaitArea(dwItemData, bValue ? true : false);
	LoadTree();
	m_treeProp.Expand(m_hWaitAreaPos,TVE_EXPAND);
}


void CProcPropDlg::OnProcpropDefineZPos() 
{	
	// get every floor's real height
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	int nFloorCount = pDoc->GetCurModeFloor().m_vFloors.size();
	std::vector<double>vFloorAltitudes;
	for(int i=0; i<nFloorCount; i++) 
	{
		//vFloorAltitudes.push_back( UNITSMANAGER->ConvertLength(pDoc->GetCurModeFloor().m_vFloors[i]->RealAltitude() ));				
		vFloorAltitudes.push_back( i );				
	}

	//get x (distance from point 0) and z
	std::vector<double>vXPos;
	std::vector<double>vZPos;
	Path _tmpPath;

	if( m_vServiceLocation.size() > 0 )
	{
		vXPos.reserve(m_vServiceLocation.size());
		vZPos.reserve(m_vServiceLocation.size());
		vXPos.push_back(0.0);
		if( m_bZPosHasBeenChanged )
		{
			vZPos.push_back( m_vServiceLocation[0][VZ]/SCALE_FACTOR );
		}
		else
		{
			vZPos.push_back( pDoc->m_nActiveFloor );
		}
		
		double dLenSoFar = 0.0;
		for( int i=1; i<static_cast<int>(m_vServiceLocation.size()); i++ )
		{
			ARCVector3 v3D = m_vServiceLocation[i]-m_vServiceLocation[i-1];	
			v3D[VZ]=0.0;
			dLenSoFar += UNITSMANAGER->ConvertLength(v3D.Magnitude());
			vXPos.push_back(dLenSoFar);
			if( m_bZPosHasBeenChanged )
			{
				vZPos.push_back( m_vServiceLocation[i][VZ] /SCALE_FACTOR);
			}
			else
			{
				vZPos.push_back( pDoc->m_nActiveFloor );				
			}
		}			

		// init temp path
		_tmpPath.init( m_vServiceLocation.size() );
		for( i=0; i< static_cast<int>(m_vServiceLocation.size()); i++ )
		{
			ARCVector3 v3D = m_vServiceLocation[i];
			if( m_bZPosHasBeenChanged )
				_tmpPath[i] = Point( v3D[VX], v3D[VY], v3D[VZ] );
			else
				_tmpPath[i] = Point( v3D[VX], v3D[VY], pDoc->m_nActiveFloor* SCALE_FACTOR );
		}
	}

	else if( m_pProc )
	{
		Path* pPath = m_pProc->serviceLocationPath();
		Point* ptList = pPath->getPointList();
		int ptCount = pPath->getCount();
		if(ptCount>0) 
		{
			vXPos.reserve(ptCount);
			vZPos.reserve(ptCount);
			vXPos.push_back(0.0);
			vZPos.push_back(ptList[0].getZ() /SCALE_FACTOR );
			double dLenSoFar = 0.0;
			for( int i=1; i<ptCount; i++ )
			{			
				ARCVector3 v3D(ptList[i].getX()-ptList[i-1].getX(), ptList[i].getY()-ptList[i-1].getY(), 0.0);
				dLenSoFar += UNITSMANAGER->ConvertLength(v3D.Magnitude());
				vXPos.push_back(dLenSoFar);				
				vZPos.push_back( ptList[i].getZ() /SCALE_FACTOR );				
			}
		}
		
		// init tem path
		_tmpPath = *pPath;
	}
	
	CStringArray strArrFloorName;
	strArrFloorName.SetSize(nFloorCount);
	for(int k=0;k<nFloorCount;k++)
	{
		strArrFloorName[k]=pDoc->GetCurModeFloor().m_vFloors[k]->FloorName();
	}
	// call dialog to define z pos
	CChangeVerticalProfileDlg dlg(vXPos, vZPos, vFloorAltitudes, _tmpPath ,&strArrFloorName);
	
	if( dlg.DoModal() == IDOK )
	{
		ASSERT( vXPos.size() == vZPos.size() );
		vZPos = dlg.m_wndPtLineChart.m_vPointYPos;
		// convert z value from real height to the format which is same to the log's z format
		for( i=0; i<static_cast<int>(vZPos.size()); ++i )
		{
			vZPos[ i ] = vZPos[ i ] * SCALE_FACTOR;	
			
			// TRACE( "%f\n", vZPos[i] );
		}

		int nSelType = m_comboType.GetCurSel();
		int nProcType = nSelType >=0 ? m_comboType.GetItemData(nSelType) : -1;

		// update z value
		if( m_vServiceLocation.size() > 0 )
		{
			for( int i=0; i<static_cast<int>(vZPos.size()); i++ )
			{
				ARCVector3& v3D = m_vServiceLocation[ i ];
				CFloor2* pFloor = GetPointFloor(static_cast<int>(vZPos[i]/SCALE_FACTOR));
				if(pFloor != NULL)
				{
					v3D[ VZ ] = pFloor->Altitude();
					if (EscalatorProc == GetCurrentProcType() || 
						StairProc == GetCurrentProcType() ||
						ConveyorProc == GetCurrentProcType()
						)
					{
						v3D[ VZ ] = vZPos[ i ];
					}
				}
			}			
		}
		else if( m_pProc )
		{
			Point* pPath = m_pProc->serviceLocationPath()->getPointList();
			for( int i=0; i<static_cast<int>(vZPos.size()); i++ )
				pPath[ i ].setZ( vZPos[i] );
		}

		//in constraint
		int nSize = (int)m_vInConstraint.size();
		if( nSize > 0)
		{
			for( int i = 0; i < nSize; i++)
			{
				ARCVector3& v3D = m_vInConstraint[ i ];

				CFloor2* pFloor = NULL;
				pFloor = GetPointFloor(int(vZPos[0]/SCALE_FACTOR));
				v3D[ VZ ] =  pFloor->Altitude();
			}
		}
		else if(m_pProc)
		{
			Point* pPoint = m_pProc->inConstraint()->getPointList();
			if (pPoint)
			{
				for( int i=0; i<m_pProc->inConstraint()->getCount(); i++ )
				{
					pPoint[ i ].setZ( vZPos[0] );
				}
			}
		}
		//queue
		nSize = (int)m_vQueieList.size();
		if( nSize > 0)
		{
			for( int i = 0; i < nSize; i++)
			{
				ARCVector3& v3D = m_vQueieList[ i ];

				CFloor2* pFloor = NULL;

				pFloor = GetPointFloor(int(vZPos[0]/SCALE_FACTOR));

				v3D[ VZ ] =  pFloor->Altitude();
			}
		}
		else if(m_pProc)
		{
			Path* pPath = m_pProc->queuePath();
			if(pPath != NULL)
			{
				Point* pPoint = m_pProc->queuePath()->getPointList();
				for( int i=0; i<m_pProc->queuePath()->getCount(); i++ )
				{
					pPoint[ i ].setZ( vZPos[0] );
				}
			}
		}
		//out constraint
		nSize = (int)m_vOutConstraint.size();
		int nCount = (int)vZPos.size();
		if( nSize > 0)
		{
			for( int i = 0; i < nSize; i++)
			{
				ARCVector3& v3D = m_vOutConstraint[ i ];
				CFloor2* pFloor = NULL;

				pFloor = GetPointFloor(int(vZPos[nCount-1]/SCALE_FACTOR));

				v3D[ VZ ] =  pFloor->Altitude();
			}
		}
		else if(m_pProc)
		{
			Point* pPoint = m_pProc->outConstraint()->getPointList();
			if (pPoint)
			{
				for( int i=0; i<m_pProc->outConstraint()->getCount(); i++ )
				{
					pPoint[ i ].setZ( vZPos[nCount-1] );
				}
			}
		}
		/////////////////////end set z value


		m_bZPosHasBeenChanged = true;
		LoadTree();
	}

}

void CProcPropDlg::OnMenueConveyWidth() 
{
	m_treeProp.SetDisplayType( 2 );
	m_hRClickItem = m_treeProp.GetSelectedItem();
	if( m_pProc )
	{
		if( m_pProc->getProcessorType() == ConveyorProc )
		{
			m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(((Conveyor*)m_pProc)->GetWidth() )) );
		}
		else if( m_pProc->getProcessorType() == StairProc  )
		{
			m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(((Stair*)m_pProc)->GetWidth() )) );
		}
		//add by Mark
		else if( m_pProc->getProcessorType() == EscalatorProc )
		{
			m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(((Escalator*)m_pProc)->GetWidth() )) );
		}
		else if (m_pProc->getProcessorType() == BridgeConnectorProc)
		{
			double dWidth = (double)m_treeProp.GetItemData(m_hRClickItem);
			m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(dWidth)));
		}
	}
	else
	{
		int nSelType = m_comboType.GetCurSel();
		int nProcType = nSelType >=0 ? m_comboType.GetItemData(nSelType) : -1;
		if (nProcType == BridgeConnectorProc && m_hRClickItem)
		{
			double dWidth = (double)m_treeProp.GetItemData(m_hRClickItem);
			m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(dWidth)));
		}
		else
			m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength( m_dConveyWidth )) );		
	}

	m_treeProp.SetSpinRange( 1,10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
	
}

void CProcPropDlg::OnAdjustLength() 
{
	m_treeProp.SetDisplayType( 2 );
	m_hRClickItem = m_treeProp.GetSelectedItem();
	int nProcType = -1;
	if (m_pProc == NULL)
	{
		int nSelType = m_comboType.GetCurSel();
		nProcType = nSelType >=0 ? m_comboType.GetItemData(nSelType) : -1;
	}
	else
	{
		nProcType = m_pProc->getProcessorType();
	}

	if (nProcType == BridgeConnectorProc && m_hRClickItem)
	{
		double dLength = (double)m_treeProp.GetItemData(m_hRClickItem);
		m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength( dLength )) );
	}
	else	
		m_treeProp.SetDisplayNum(  static_cast<int>(UNITSMANAGER->ConvertLength( m_dProcessorLength )) );
	
	m_treeProp.SetSpinRange( 1, 10000 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CProcPropDlg::OnAdjustAngle() 
{
	m_treeProp.SetDisplayType( 2 );

	m_treeProp.SetDisplayNum( static_cast<int>(m_dPathAngle) );

	m_treeProp.SetSpinRange( -90, 90 );

	m_treeProp.SpinEditLabel( m_hAngleItem );
	
}

void CProcPropDlg::OnProcpropToggledBackup()
{
		
	m_bBackup = !m_bBackup;

	//if use backup, StandProcessor's data need update immediately
	if(m_bBackup)
	{
		int nSelIdx = m_comboType.GetCurSel();
		if( nSelIdx >= 0 )
		{
			int nProcType = m_comboType.GetItemData( nSelIdx );
			if( (StandProcessor == nProcType||DeiceBayProcessor == nProcType) && m_vInConstraint.size() >= 2)
			{
				std::vector<ARCVector3> m_vConsTemp;

				m_vConsTemp.push_back(m_vInConstraint[0]);
				m_vConsTemp.push_back(m_vInConstraint[1]);

				m_vInConstraint.clear();
				m_vOutConstraint.clear();
				m_vInConstraint = m_vConsTemp;
				std::reverse(m_vConsTemp.begin(), m_vConsTemp.end());
				m_vOutConstraint = m_vConsTemp;
			}
		}
	}
	LoadTree();
}


void CProcPropDlg::OnDropdownComboShape()
{
	m_cmbShape.DisplayListWnd() ;
}

BOOL CProcPropDlg::CreateTreeViewMenu(const CString &strCaption, UINT nID, CMenu *pMenu)
{
	ASSERT(pMenu);
	pMenu->CreatePopupMenu();
	pMenu->AppendMenu(MF_STRING, nID, strCaption);
	
	
	return TRUE;
}

void CProcPropDlg::OnProcpropModifyValue(UINT nID)
{
	
	//m_treeProp.SetDisplayNum( UNITSMANAGER->ConvertLength(m_tempElevator.GetPlatFormWidth()));
	CString strValue = _T("NULL");
	CModifyLatLong dlg;
	
	switch (nID) {
//	case ID_PROCPROP_MARKING:
//	case ID_PROCPROP_MARKING2:
	case ID_PROCPROP_RUNWAYNUMBER:
		//if (nID == ID_PROCPROP_MARKING)
		//{
		//	GetBrachValue(m_hRClickItem, strValue, 0);
		//	m_nModifyFlag = 1;
		//}
		//else if (nID == ID_PROCPROP_MARKING2)
		//{
		//	GetBrachValue(m_hRClickItem, strValue, 2);
		//	m_nModifyFlag = 2;
		//}
		//else
			GetBrachValue(m_hRClickItem, strValue);
		m_treeProp.StringEditLabel(m_hRClickItem, strValue);
		break;


	case ID_PROCPROP_LATLONG:
		if (m_pProc)
		{
			string s = ((ArpProc*)m_pProc)->GetLatlong(ArpProc::Latitude);
			dlg.m_strLat = s.c_str();
			s = ((ArpProc*)m_pProc)->GetLatlong(ArpProc::Longitude);
			dlg.m_strLong = s.c_str();
		}
		else
		{
			dlg.m_strLat = m_strLat;
			dlg.m_strLong = m_strLong;
		}
		if (dlg.DoModal())
		{
			//	
			if (m_pProc)
			{
				((ArpProc*)m_pProc)->SetLatlong(ArpProc::Latitude, string(dlg.m_strLat));
				((ArpProc*)m_pProc)->SetLatlong(ArpProc::Longitude, string(dlg.m_strLong));
			}
			else
			{
				m_strLat = dlg.m_strLat;
				m_strLong = dlg.m_strLong;
			}

			LoadTree();
		}
		break;

	case ID_PROCPROP_HEIGHT:
		{
			int nValue;
			m_treeProp.SetDisplayType( 2 );
			m_treeProp.SetSpinRange( 1,10000 );

			if (GetBranchValue(m_hRClickItem, nValue))
				m_treeProp.SetDisplayNum(nValue);
			m_treeProp.SpinEditLabel( m_hRClickItem );

		}
		break;
		
	default:
		int nValue;
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetSpinRange( 1,10000 );


		if (GetBranchValue(m_hRClickItem, nValue))
			m_treeProp.SetDisplayNum(nValue);
		m_treeProp.SpinEditLabel( m_hRClickItem );
		break;
	}
	
}

BOOL CProcPropDlg::GetBranchValue(HTREEITEM hItem, int& nValue, int iChildIndex /* = -1*/)
{

	HTREEITEM hItemChild = hItem;
	
	if (iChildIndex > -1)
	{
		m_treeProp.GetChildItem(hItem);
		for (int i = 0; i < iChildIndex; i++)
			hItemChild = m_treeProp.GetNextItem(hItemChild, TVGN_NEXT);
	}


	CString str = m_treeProp.GetItemText(hItemChild);
	int nStart = str.ReverseFind(_T('('));
	int nEnd = str.ReverseFind(_T(')'));

	CString strNum = str.Mid(nStart + 1, nEnd - nStart - 1);

	nValue = atoi(strNum);


	return TRUE;
}

BOOL CProcPropDlg::GetBrachValue(HTREEITEM hItem, CString &strValue, int iChildIndex /*= -1 */)
{
	if (iChildIndex == -1)
	{
		CString str = m_treeProp.GetItemText(hItem);
		int nStart = str.Find(_T('('));
		int nEnd = str.ReverseFind(_T(')'));
		
		strValue = str.Mid(nStart + 1, nEnd - nStart - 1);
	}
	else
	{
		HTREEITEM hItemChild = m_treeProp.GetChildItem(hItem);
		for (int i = 0; i < iChildIndex; i++)
			hItemChild = m_treeProp.GetNextItem(hItemChild, TVGN_NEXT);
		strValue = m_treeProp.GetItemText(hItemChild);
	}

	if (strValue == _T(""))
		strValue = _T("NULL");

	return TRUE;
}

int CProcPropDlg::GetFloor(DistanceUnit zPos)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	std::vector<CFloor2*> vFloors = pDoc->GetCurModeFloor().m_vFloors;
	std::vector<CFloor2*>::iterator iter;
	for (int i = 0; i < (int)vFloors.size(); i++)
	{
		if ((vFloors[i]->Altitude() - zPos)*(vFloors[i]->Altitude() - zPos) < 1.00000)
			break;
	}

	return i;
}

CFloor2* CProcPropDlg::GetPointFloor(const Point& pt)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	std::vector<CFloor2*> vFloors = pDoc->GetCurModeFloor().m_vFloors;
	std::vector<CFloor2*>::iterator iter;
	int nLevel = static_cast<int>(pt.getZ() / SCALE_FACTOR);
	for (iter = vFloors.begin(); iter != vFloors.end(); iter++)
	{
		if ((*iter)->Level() == nLevel)
			return (*iter);
	}

	return NULL;
}

CFloor2* CProcPropDlg::GetPointFloor(int nLevel)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	std::vector<CFloor2*> vFloors = pDoc->GetCurModeFloor().m_vFloors;
	std::vector<CFloor2*>::iterator iter;
	
	for (iter = vFloors.begin(); iter != vFloors.end(); iter++)
	{
		if ((*iter)->Level() == nLevel)
			return (*iter);
	}
	
	return NULL;
}

int CProcPropDlg::GetShapeIndex(void)
{
	return m_nShapeIndex;
}

void CProcPropDlg::ForceControlStatus(void)
{
	if (m_nForceProcType == -1)
		return;
	
	for (int i = 0; i < m_comboType.GetCount(); i++)
	{
		if (m_nForceProcType == static_cast<int>(m_comboType.GetItemData(i)))
		{
			m_comboType.SetCurSel(i);
			m_comboType.EnableWindow(FALSE);
			break;
		}
	}

	m_cmbShape.EnableWindow(FALSE);
}

void CProcPropDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CProcPropDlg::OnProcpropChanglatlong()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(m_pProc)
	{
		CDlgInitArp initArp;
		CString strLat,strLong;
		initArp.m_strLatitude	=	m_strLat;
		initArp.m_strLongitude	=	m_strLong;

		if (initArp.DoModal()==IDOK)
		{

			if ((m_strLong.CompareNoCase(initArp.m_strLongitude)==0)&&(m_strLat.CompareNoCase(initArp.m_strLatitude)==0))
			{
				return;
			}

			m_strLong=initArp.m_strLongitude;
			m_strLat=initArp.m_strLatitude;

			((FixProc *)m_pProc)->m_pLongitude->SetValue(m_strLong);
			((FixProc *)m_pProc)->m_pLatitude->SetValue(m_strLat);

			double dx,dy;
			Convent_latlong_local(m_strLat,m_strLong,dx,dy);
			
			ARCVector3 pt;
			pt[VX]=dx*100.0;
			pt[VY]=dy*100.0;
			pt[VZ]=0.0;
			m_vServiceLocation.clear();
			m_vServiceLocation.push_back(pt);
			pDoc->m_tempProcInfo.SetTempServicLocation(m_vServiceLocation);

			LoadTree();
		}
	
	}
	else
	{	
		CDlgInitArp initArp;
		CString strLat,strLong;
		initArp.m_strLatitude	=	m_strLat;
		initArp.m_strLongitude	=	m_strLong;

		if (initArp.DoModal()==IDOK)
		{

			if ((m_strLong.CompareNoCase(initArp.m_strLongitude)==0)&&(m_strLat.CompareNoCase(initArp.m_strLatitude)==0))
			{
				return;
			}
			m_strLong=initArp.m_strLongitude;
			m_strLat=initArp.m_strLatitude;
			
			double dx,dy; // unit: m
			Convent_latlong_local(m_strLat,m_strLong,dx,dy);

			ARCVector3 pt=ARCVector3(dx*100,dy*100,0.0); // unit: cm
			m_vServiceLocation.clear();
			m_vServiceLocation.push_back(pt);
			pDoc->m_tempProcInfo.SetTempServicLocation(m_vServiceLocation);


			LoadTree();
		}
	}
}

void CProcPropDlg::Convent_latlong_local(CString strLat,CString strLong,double &dx,double &dy)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CLatitude *pLat= pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_latitude;
	CLongitude *pLon=pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_longtitude;
	
	double dx_origin=pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_dx;
    double dy_origin=pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_dy;

	CLatLongConvert convent;
	convent.Set_LatLong_Origin(pLat,pLon,dx,dy);

	CLatitude *pConvlat=new CLatitude();
	CLongitude *pConvlong=new CLongitude();
	pConvlat->SetValue(strLat);
	pConvlong->SetValue(strLong);

	LatLong_Structure *pConvented=new LatLong_Structure(pConvlat,pConvlong);

	float x_dist,y_dist,dist;
	convent.Distance_LatLong(pConvented,x_dist,y_dist,dist);
	dx=dx_origin + x_dist*1852 ;
	dy=dy_origin + y_dist*1852;
	
	//release
	delete pConvlat;
	delete pConvlong;
	delete pConvented;

}
void CProcPropDlg::Convent_local_latlong(double dx,double dy,CString &strLat,CString &strLong)
{

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CLatitude *pLat= pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_latitude;
	CLongitude *pLon=pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_longtitude;

	double dx_origin=pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_dx;
	double dy_origin=pDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_dy;

	CLatLongConvert convent;
	convent.Set_LatLong_Origin(pLat,pLon,dx,dy);

	CLatitude *pConvlat=new CLatitude();
	CLongitude *pConvlong=new CLongitude();

	LatLong_Structure *pConvented=new LatLong_Structure(pConvlat,pConvlong);

	convent.Point_LatLong((float)((dx-dx_origin)/1852) , (float)((dy-dy_origin)/1852),pConvented);
//	convent.Point_LatLong(0.0,0.0,pConvented);
	
	pConvented->m_pLatitude->GetValue(strLat);
	pConvented->m_pLongitude->GetValue(strLong);

	//release
	if (pConvlat)
		delete pConvlat;
	if (pConvlong)
		delete pConvlong;
	if (pConvented)
		delete pConvented;
}


void CProcPropDlg::OnBillboardModifyheight()
{
	m_treeProp.SetDisplayType( 2 );
	//UNITSMANAGER->ConvertLength( ) 
	m_treeProp.SetDisplayNum(  static_cast<int>(UNITSMANAGER->ConvertLength(m_tempBillBoardProc.getHeight()) ));

	m_treeProp.SetSpinRange( 1, 100 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CProcPropDlg::OnBillboardModifythickness()
{
	m_treeProp.SetDisplayType( 2 );
	//UNITSMANAGER->ConvertLength( )
	m_treeProp.SetDisplayNum(  static_cast<int>(UNITSMANAGER->ConvertLength(m_tempBillBoardProc.getThickness())) );

	m_treeProp.SetSpinRange( 1, 100 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CProcPropDlg::OnBillboardText()
{
	m_tempBillBoardProc.setType(0);
	
	//ASSERT(m_hBillBoardBitmapPath);
	//m_hBillBoardText = m_hBillBoardBitmapPath;
	//m_hBillBoardBitmapPath = NULL;
	LoadTree();
}

void CProcPropDlg::OnBillboardLogo()
{
	m_tempBillBoardProc.setType(1);
	LoadTree();
}

void CProcPropDlg::OnBillboardModifytext()
{
	CString strValue= m_treeProp.GetItemText(m_hBillBoardText);
	m_treeProp.StringEditLabel(m_hBillBoardText, strValue);
}

void CProcPropDlg::OnBillboardModifypath()
{
	CString strFileName = "";
	CFileDialog fileDlg(TRUE, _T("bmp"),NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Bitmap Files (*.bmp)|*.bmp|Jpeg Files(*.jpg)|*.jpg|All Files (*.*)|*.*||"), NULL);
	if(fileDlg.DoModal() == IDOK)
	{
		strFileName = fileDlg.GetPathName();
		m_treeProp.SetItemText(m_hRClickItem,strFileName);
		m_tempBillBoardProc.setBitmapPath(strFileName);
	}
}

void CProcPropDlg::OnBillboardModifycoefficient()
{
	m_treeProp.SetDisplayType( 2 );

//	m_treeProp.SetDisplayNum(  static_cast<int>(m_tempBillBoardProc.getCoefficient() * 100.0) );

	m_treeProp.SetSpinRange( 1, 100 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CProcPropDlg::OnBillboardModifyangle()
{	
	m_treeProp.SetDisplayType( 2 );

//	m_treeProp.SetDisplayNum(  m_tempBillBoardProc.getAngle() );

	m_treeProp.SetSpinRange( 0, 180 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
	// TODO: Add your command handler code here
}

void CProcPropDlg::OnHoldChooseafix()
{
	if(m_hRClickItem == m_hFix){
		m_strfixes.clear();
		const ProcessorList* prolist = GetInputTerminal()->GetProcessorList();
		for(int i=0;i<prolist->getProcessorCount();i++){
			const Processor* proc = prolist->getProcessor(i);
			if(proc->getProcessorType() == FixProcessor)m_strfixes.push_back(proc->getIDName());
		}
		m_treeProp.SetComboString(m_hFix,m_strfixes);
		
	}
	else if(m_hRClickItem == m_hRight){
		std::vector<CString> strrightorleft;
		strrightorleft.push_back("right");strrightorleft.push_back("left");
		m_treeProp.SetComboString(m_hRight,strrightorleft);
	}
//	LoadTree();
}

void CProcPropDlg::OnHoldargumentEdit()
{
	m_treeProp.SetDisplayType(2);
	m_treeProp.SetSpinRange(0,360);
	if(m_hRClickItem==m_hMinAlt ){
		m_treeProp.SetDisplayNum(int(UNITSMANAGER->ConvertLength(m_dminAlt)));
	}
	else if(m_hRClickItem == m_hMaxAlt){
		m_treeProp.SetDisplayNum(int(UNITSMANAGER->ConvertLength(m_dmaxAlt)));
	}
	else if( m_hRClickItem == m_hInBound){
		m_treeProp.SetDisplayNum(int(m_nInBoundDegree));
	}
	else if(m_hRClickItem == m_hOutBoundLegmin){
		m_treeProp.SetDisplayNum(int(m_dOutBoundmin));
	}else if(m_hRClickItem==m_hOutBoundLegnm){
		m_treeProp.SetDisplayNum(int(UNITSMANAGER->ConvertLength(m_dOutBoundnm)));
	}
	else if (m_hRClickItem == m_hBandInterval){
		m_treeProp.SetDisplayNum(int(m_dbandinterval));
	}
	m_treeProp.SpinEditLabel(m_hRClickItem);

}

void CProcPropDlg::OnRunwaythresholdaModify()
{
	int nValue;
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange( 1,10000 );

	if (GetBranchValue(m_hRClickItem, nValue))
		m_treeProp.SetDisplayNum(nValue);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}

void CProcPropDlg::OnRunwaymarkModify()
{

	CString strValue= m_treeProp.GetItemText(m_hRClickItem);
	m_treeProp.StringEditLabel(m_hRClickItem, strValue);
	
}

int CProcPropDlg::GetCurrentProcType()
{
	int nSelIdx = m_comboType.GetCurSel();
	return m_comboType.GetItemData( nSelIdx );
}
void CProcPropDlg::autoCalRunwayMark(){
	if(m_vServiceLocation.size()<2)return;
	ARCVector2 vnorth (0,1);
	ARCVector3 vrunway3 = m_vServiceLocation[1]- m_vServiceLocation[0];
	ARCVector2 vrunway2(vrunway3[VX],vrunway3[VY]);
	double dangle =  ARCMath::RadiansToDegrees(vrunway2.GetCosOfTwoVector(vnorth));
	m_strMarking.Format("%d",(int)(dangle/10) );
	m_strMarking2.Format("%d" , (int) (dangle/10 + 18));

}

void CProcPropDlg::addTempAirsideProc(int _nProcType){
	/*CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(m_vServiceLocation.size() < 2 && m_vServiceLocation.size() >0 && _nProcType == StandProcessor){
		Point pts[1];
		pts[0]= Point(m_vServiceLocation[0][VX],m_vServiceLocation[0][VY],m_vServiceLocation[0][VZ]);
		pDoc->GetCurrentPlacement(EnvMode_AirSide)->m_vUndefined.clear();
		CProcessor2 * pProc2 = new CProcessor2;
		new StandProc()
		pProc2->SetProcessor();
		pProc2->GetProcessor()->serviceLocationPath()->init(1,pts);
	
	}*/
}

void CProcPropDlg::convertPathByAngle( Path& pInPath,double _newAngle )
{
	const double pi = 3.1415926535;
	CTermPlanDoc* _pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
//	CTermPlanDoc* _pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	assert( _pDoc );
	
	if( pInPath.getCount() <=1 )
		return ;
	
	long double _radians = _newAngle * pi / 180.0;
	//temp path, to save new points
	Path _newPath;
	_newPath.init( pInPath.getCount() );

	// build new path
	DistanceUnit _distance = 0.0;
	DistanceUnit _dVerticalDistance = 0.0;
	DistanceUnit _dHorizonDistance = 0.0;
	_newPath[0] = pInPath[0];

	std::vector<double> vRealAltitude;
	_pDoc->GetCurModeFloorsRealAltitude(vRealAltitude);

	for( int i=1; i < pInPath.getCount(); i++ )
	{
		// get distance of two point
		_distance = pInPath.getFactLength( pInPath[i-1], pInPath[i],vRealAltitude );
		
		// get ver & hor length
		_dVerticalDistance = sin( _radians ) * _distance;
		_dHorizonDistance  = cos( _radians ) * _distance;
		
		// adjust z value
		double _dRealHight = _pDoc->GetCurModeFloor().getRealAltitude( _newPath[i-1].getZ() );
		_dRealHight += _dVerticalDistance;
		double _dLogicHight = _pDoc->GetCurModeFloor().getLogicAltitude( _dRealHight );
		
		// get the new point
		_dVerticalDistance = pInPath.getFactLength( Point( 0.0, 0.0, _newPath[i-1].getZ()),Point( 0.0, 0.0, _dLogicHight ),vRealAltitude );

		if( _dVerticalDistance != 0.0 && _radians != 0.0 )
		{
			_dHorizonDistance = _dVerticalDistance / tan( _radians );
		}

		Point _tmpVector( Point( pInPath[i-1].getX(), pInPath[i-1].getY(), 0.0 ),
						  Point( pInPath[i].getX(), pInPath[i].getY(), 0.0 ) );
		_tmpVector.length( _dHorizonDistance );
		Point _newPoint = Point( _newPath[i-1].getX(), _newPath[i-1].getY(), 0.0 ) + _tmpVector;
		_newPoint.setZ( _dLogicHight );
		
		// add the new point to new path
		_newPath[i] = _newPoint;
	}

	// change the old path
	pInPath.init( _newPath );
	return ;	
}

// void CProcPropDlg::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	*pResult = 1;
// 	CPoint  point;
// 	UINT uFlags;
// 	GetCursorPos(&point);
// 	m_treeProp.ScreenToClient(&point);
// 	HTREEITEM hItem = m_treeProp.HitTest(point, &uFlags);
// 	m_hRClickItem = hItem;
// 
// 	if ((hItem != NULL) && (TVHT_ONITEM & uFlags) && m_pProc->getProcessorType() == IntegratedStationProc)
// 	{
// 		if(hItem == m_hPlatFormWidth)
// 			OnPlatformWidth();
// 		else if(hItem == m_hPlatFormLength)
// 		{
//  			m_treeProp.SetDisplayType( 1 );
//  			m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(m_stationLayout.GetPlatformLength())) );
//  			m_treeProp.SetSpinRange( 1,10000);
//  			m_treeProp.SpinEditLabel( hItem );
// 		}
// 		//	OnPlatformLength();
// 		else if(hItem == m_hCarWidth)
// 			OnCarWidth();
// 		else if(hItem == m_hCarLength)
// 			OnCarLength();
// 		else if(hItem == m_hCarNum)
// 			OnCarNumbers();
// 		else if( hItem == m_hCarCapacity)
// 			OnCarCapacity();
// 		else if (hItem == m_hEntryDoorNum)
// 			OnDefineEntryDoorNum();
// 	}
// 	m_treeProp.SelectItem(hItem);
// }

LRESULT CProcPropDlg::OnModifyValue(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hItem = (HTREEITEM)wParam;
	m_hRClickItem = hItem;
	if (hItem == m_hCarCapacity)
	{
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( getStationLayout()->GetCarCapacity() );
		m_treeProp.SetSpinRange( 0,200);
		m_treeProp.m_bEableEdit = TRUE;
	}
	else if (hItem == m_hPlatFormWidth)
	{
		m_treeProp.SetDisplayType( 1 );
		m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(getStationLayout()->GetPlatformWidth())) );
		m_treeProp.SetSpinRange( 1,10000);
		m_treeProp.m_bEableEdit = TRUE;
	}
	else if (hItem == m_hPlatFormLength)
	{
		m_treeProp.SetDisplayType( 1 );
		m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(getStationLayout()->GetPlatformLength())) );
		m_treeProp.SetSpinRange( 1,10000);
		m_treeProp.m_bEableEdit = TRUE;
	}
	else if (hItem == m_hEntryDoorNum)
	{
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( getStationLayout()->GetEntryDoorNumber());
		m_treeProp.SetSpinRange( 1,15);
		m_treeProp.m_bEableEdit = TRUE;
	}
	else if (hItem == m_hCarNum)
	{
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( getStationLayout()->GetCarCount() );
		m_treeProp.SetSpinRange( 1,20);
		m_treeProp.m_bEableEdit = TRUE;
	}
	else if (m_hRClickItem == m_hCarLength)
	{
		m_treeProp.SetDisplayType( 3 );
		m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(getStationLayout()->GetCarLength())) );
		m_treeProp.SetSpinRange( 1,10000);
		m_treeProp.m_bEableEdit = TRUE;
	}
	else if (hItem == m_hCarWidth)
	{
		m_treeProp.SetDisplayType( 3 );
		m_treeProp.SetDisplayNum( static_cast<int>(UNITSMANAGER->ConvertLength(getStationLayout()->GetCarWidth())) );
		m_treeProp.SetSpinRange( 1,10000);
		m_treeProp.m_bEableEdit = TRUE;
	}
	else if (hItem == m_hExitDoorNum)
	{
		m_treeProp.SetDisplayType( 2 );
		m_treeProp.SetDisplayNum( getStationLayout()->GetExitDoorNumber());
		m_treeProp.SetSpinRange( 1, 15);
		m_treeProp.m_bEableEdit = TRUE;
	}
	else if (hItem == m_hOrientation)
	{
		m_treeProp.SetDisplayType( 1 );
		m_treeProp.SetDisplayNum( m_tempStation.GetOrientation() );
		m_treeProp.SetSpinRange( 0,360 );
		m_treeProp.m_bEableEdit = TRUE;
	}
	else
		m_treeProp.m_bEableEdit = FALSE;
	return TRUE;
}

void CProcPropDlg::OnDefineGateType()
{
	std::vector<CString> strList;
	strList.push_back(_T("ArrGate"));
	strList.push_back(_T("DepGate"));
    m_treeProp.SetComboString(m_hGateType,strList);
//	GateProcessor* pGate = (GateProcessor*)m_pProc;
	switch(m_emGateType)
	{
	case ArrGate:
		m_treeProp.m_comboBox.SetCurSel(0);
		break;
	case DepGate:
		m_treeProp.m_comboBox.SetCurSel(1);
		break;
	default:
		break;
	}
}

void CProcPropDlg::OnDefineRetailType()
{
	std::vector<CString> strList;
	strList.push_back(_T("Shop Section"));
	strList.push_back(_T("By Pass"));
	strList.push_back(_T("Check Out"));
	m_treeProp.SetComboString(m_hRetailType,strList);
	switch(m_emRetailType)
	{
	case RetailProcessor::Retial_Shop_Section:
		m_treeProp.m_comboBox.SetCurSel(0);
		break;
	case RetailProcessor::Retial_By_Pass:
		m_treeProp.m_comboBox.SetCurSel(1);
		break;
	case RetailProcessor::Retial_Check_Out:
		m_treeProp.m_comboBox.SetCurSel(2);
		break;
	default:
		break;
	}
}

static bool CEntryPointAndIndexLess(CProcPropDlg::CEntryPointAndIndex * pObj1, CProcPropDlg::CEntryPointAndIndex * pObj2 )
{
	if(pObj1 == NULL || pObj2 == NULL)
	{
		ASSERT(0);
		return false;
	}

	return (*pObj1) < (*pObj2);

}
void CProcPropDlg::LoadEntryTree(int nProcType)
{
	CString CRootNode(_T("Interim Entry Point:"));
	m_EntryPoint = m_treeProp.InsertItem(CRootNode,m_hQueue) ;
	//m_treeProp.SetItemData(m_EntryPoint,(DWORD_PTR)ENTRYPOINT_ROOT_NODE) ;
	 CString csPoint;
	    HTREEITEM P_Item = NULL ;
		if(m_EntryPointIndex.size() == 0 )
		{
	/*		CEntryPointAndIndex entrypoint ;
			if(m_vQueieList.size() == 0 )
				{
					entrypoint.index = m_pProc->queuePath()->getCount() - 1 ;
                    entrypoint.point = m_pProc->GetProcessorQueue()->corner(entrypoint.index) ;
				}
			else
			{
			 ARCVector3 point = m_vQueieList[(m_vQueieList.size()- 1 )] ; 
			 entrypoint.index = m_vQueieList.size()- 1  ;
			 entrypoint.point = point ;
			}		
			m_EntryPointIndex.push_back(entrypoint) ;
			Point v3D;
			v3D.setX(entrypoint.point[VX]) ;
			v3D.setY(entrypoint.point[VY]) ;
			v3D.setZ(entrypoint.point[VZ]) ;
			FormatQueuePointString(csPoint,v3D,nProcType) ;
          P_Item =  m_treeProp.InsertItem( csPoint, m_EntryPoint ); 
		  m_treeProp.SetItemData(P_Item,(DWORD_PTR)&m_EntryPointIndex[0]) ;
		  m_treeProp.SelectItem(P_Item) ;
		  m_treeProp.Expand(m_EntryPoint,TVE_EXPAND);*/
		  return ;
		}
		std::sort(m_EntryPointIndex.begin(),m_EntryPointIndex.end(),CEntryPointAndIndexLess) ;
		std::vector<CEntryPointAndIndex *>::iterator iter = m_EntryPointIndex.begin() ;
		for( ;iter != m_EntryPointIndex.end() ;++iter)
		{
             Point v3D;
			 v3D.setX((*iter)->point[VX]) ;
			 v3D.setY((*iter)->point[VY]) ;
             v3D.setZ((*iter)->point[VZ]) ;
			 FormatQueuePointString(csPoint,v3D,nProcType) ;
			 P_Item =  m_treeProp.InsertItem( csPoint, m_EntryPoint );
			 m_treeProp.SetItemData(P_Item,(DWORD_PTR)(*iter)) ;
		}
		//m_treeProp.SelectItem(P_Item) ;
		m_treeProp.Expand(m_EntryPoint,TVE_EXPAND);
		return ;

}
void  CProcPropDlg::FormatQueuePointString(CString& str,Point& point,int nProcType)
{

	if ( nProcType == ConveyorProc ||nProcType == StairProc || nProcType == EscalatorProc )
	{
		str.Format( "x = %.2f; y = %.2f; z = %.2f",
			UNITSMANAGER->ConvertLength(point.getX()),
			UNITSMANAGER->ConvertLength(point.getY()),
			(point.getZ()) / SCALE_FACTOR );

		CFloor2* pFloor = GetPointFloor(int((point.getZ()) / SCALE_FACTOR));
		if (pFloor)
		{
			str += _T("; Floor:") + pFloor->FloorName();
		}
	}
	else
	{
		str.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength(point.getX()), UNITSMANAGER->ConvertLength(point.getY()) );
	}
}
 void CProcPropDlg::OnAddEntryPoint() 
{

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(pDoc == NULL)
		return ;
	//const ProcessorID* proID = m_pProc->getID() ;
	//if(proID == NULL)
	//	return ;
 //   CProcessor2* pro = pDoc->GetProcessor2FromID(*proID,pDoc->GetCurrentMode()) ;
	//if(pro !=NULL)
	//		pro->EnableHighLightEntryPoint(TRUE) ;
	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;
	enumReason= PICK_ONEPOINT;
	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}
 void CProcPropDlg::OnDelEntryPoint() 
{
	if(m_hRClickItem == NULL)
		return;

    HTREEITEM item = m_hRClickItem;
	
	CEntryPointAndIndex* point = (CEntryPointAndIndex*)m_treeProp.GetItemData(item) ;
	if(point != NULL)
	{
        std::vector<CEntryPointAndIndex *>::iterator iter = std::find(m_EntryPointIndex.begin(),m_EntryPointIndex.end(),point) ;
		if(iter != m_EntryPointIndex.end())
		{
			//if(m_pProc != NULL && m_pProc->GetProcessorQueue() != NULL)
			//{
			//	FixedQueue* queue = static_cast<FixedQueue*>(m_pProc->GetProcessorQueue()) ;
			//	queue->DeleteEntryPoint((*iter).index) ;
			//}
			delete *iter;
			m_EntryPointIndex.erase(iter);
			m_treeProp.DeleteItem(item);
		}

	}

	 
}
void  CProcPropDlg::SetEntryPointIndex(Processor* pro)
{
	if(pro == NULL)
		return ;
	Path* path = pro->queuePath() ;
	int index = 0 ;
	FixedQueue* queue = NULL ;
	try{
		queue = static_cast<FixedQueue*>(pro->GetProcessorQueue()) ;
	}catch(std::bad_cast)
	{
		return ;
	}
	if (queue == NULL)
	   return ;
	queue->DeleteAllEntryPoint() ;
	for (int i = 0 ; i< (int)m_EntryPointIndex.size() ;i++)
	{
		Point point ;
		point.setX(m_EntryPointIndex[i]->point[VX]) ;
		point.setY(m_EntryPointIndex[i]->point[VY]) ;
		point.setZ(m_EntryPointIndex[i]->point[VZ]) ;
		index = path->getClosestPointIndex(point)  ;
        if(index == 0)
			index = path->getCount() - 1 ;
		queue->AddEntryPoint(index) ;
	}
}
void CProcPropDlg::InitEntryPoints()
{
    if(m_pProc != NULL && m_pProc->GetProcessorQueue() != NULL)
	{
        if(m_pProc->GetProcessorQueue()->isFixed() == 'Y')
		{
               FixedQueue* queue = NULL ;
			try
			{
				queue = static_cast<FixedQueue*>(m_pProc->GetProcessorQueue()) ;
			}
			catch (std::bad_cast)
			{}
			std::vector<int>* entrypoint = queue->GetEntryPoints() ;
		   for (int i = 0 ;i < (int)entrypoint->size() ;i++)
		   {
			if(entrypoint->at(i) == 0)
				continue ;
            CEntryPointAndIndex* pointandindx = new CEntryPointAndIndex;
			pointandindx->index = entrypoint->at(i) ;
		    pointandindx->point = queue->corner(entrypoint->at(i)) ;
			m_EntryPointIndex.push_back(pointandindx) ;
		   }
		   	
		}
	}
}

void CProcPropDlg::OnStnClickedStaticLine1()
{
	// TODO: Add your control notification handler code here
}

CProcPropDlg::~CProcPropDlg()
{
	std::vector<CEntryPointAndIndex *>::iterator iter = m_EntryPointIndex.begin() ;
	for( ;iter != m_EntryPointIndex.end() ;++iter)
	{
		delete *iter;
	}

	m_EntryPointIndex.clear();
}

void CProcPropDlg::AcquireDataForBridgeConnector()
{
	BridgeConnector* pConnector = dynamic_cast<BridgeConnector*>(m_pProc);
	if (pConnector)
	{
		pConnector->setConnectPointCount(m_nBridgeConnectPointCount);
		pConnector->ClearConnectPointData();

		for (int i =0; i < m_nBridgeConnectPointCount; i++)
		{
			BridgeConnector::ConnectPoint conPoint = m_vBridgeConnectPoints.at(i);
			pConnector->AddConnectPoint(conPoint);
		}
	}
}

void CProcPropDlg::OnPropNumberModify()
{
	m_treeProp.SetDisplayType( 2 );

	m_treeProp.SetDisplayNum(1);

	m_treeProp.SetSpinRange( 1, 100 );
	m_treeProp.SpinEditLabel( m_hRClickItem );
}