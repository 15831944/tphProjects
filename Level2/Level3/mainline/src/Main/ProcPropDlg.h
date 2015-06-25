#if !defined(AFX_PROCPROPDLG_H__2DCA0397_B824_4F8C_9C8B_C34BA79B3346__INCLUDED_)
#define AFX_PROCPROPDLG_H__2DCA0397_B824_4F8C_9C8B_C34BA79B3346__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcPropDlg.h : header file
//
#include "common\ARCVector.h"
#include "engine\Car.h"
#include "engine\IntegratedStation.h"
#include "engine\Process.h"
#include "inputs\StationLayout.h"
#include "TreeCtrlEx.h"
#include "Processor2.h"
#include "engine\ElevatorProc.h"
#include "engine\gate.h"
#include "IconComboBox.h"
#include "resource.h"
#include "../Engine/BillboardProc.h"
#include "../engine/procq.h"
#include "../Engine/BridgeConnector.h"
/////////////////////////////////////////////////////////////////////////////
// CProcPropDlg dialog

typedef std::vector< CCar*> CAR;
typedef std::vector< HTREEITEM > CARTREE, CARENTRYDOOR, CAREXITDOOR ,CARALLSUBDOOR,PREBORDINGAREA ;
class Conveyor;
class CProcPropDlg : public CDialog
{
// Construction
public:
	BOOL GetBrachValue(HTREEITEM hItem, CString& strValue, int iChildIndex = -1);
	BOOL GetBranchValue(HTREEITEM hItem, int& nValue, int iChildIndex = -1);
	HTREEITEM m_hLiftWidth;
	HTREEITEM m_hLiftLength;
	HTREEITEM m_hLiftNOL;
	HTREEITEM m_hWaitAreaWidth;
	HTREEITEM m_hWaitAreaLength;
	HTREEITEM m_hWaitAreaPos;
	HTREEITEM m_hMaxFloor;
	HTREEITEM m_hMinFloor;
	HTREEITEM m_hLift;
	HTREEITEM m_hWaitArea;

	//add 
	HTREEITEM m_hFixXY;
	HTREEITEM m_hFixLL;

	//latitude longitude
	CString		 m_strLat;
	CString		 m_strLong;
	
	//contour height
	HTREEITEM m_hHeight;
	DistanceUnit m_dHeight;

	
	void Convent_latlong_local(CString strLat,CString strLong,double &dx,double &dy);
	void Convent_local_latlong(double dx,double dy,CString &strLat,CString &strLong);

	Processor* GetProcessor();
	CProcPropDlg( Processor* _pProc, const ARCVector3& _location, CWnd* pParent );   // standard constructor
	virtual ~CProcPropDlg();
	int GetCurrentProcType();

	int DoFakeModal();
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);

	static const LPCTSTR s_szPropName[];

// Dialog Data
	//{{AFX_DATA(CProcPropDlg)
	enum { IDD = IDD_DIALOG_PROCPROP };
	CButton	m_btnEmergent;
	CButton	m_btnCancle;
	CButton m_btnStandPushBack;
	CButton	m_btnOk;
	CButton	m_btnOffgate;
	CComboBox	m_comboType;
	CComboBox	m_comboLevel4;
	CComboBox	m_comboLevel3;
	CComboBox	m_comboLevel2;
	CComboBox	m_comboLevel1;
	CTreeCtrlEx	m_treeProp;
	CString	m_csLevel1;
	CString	m_csLevel2;
	CString	m_csLevel3;
	CString	m_csLevel4;

	CIconComboBox	m_cmbShape;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

private:
		bool CheckProcessorName();
		bool m_bTypeCanChange;
		BOOL IsNameChanged(CString newName);
		bool SetProcessorValue(Processor* pProcessor);
		void InitTempProcInfo();
		void BuildAllSubProcessorID();
		void InitIntegratedProcessor();
		void InitStationLayout();		

		CStationLayout* getStationLayout(){return m_tempStation.GetStationLayout();}
		ARCVector3 m_stationServiceLocation;
		bool m_bHavePath;
		
		BOOL m_busepushback;
		int m_iSelectType;
		
		HTREEITEM m_hCars;
		HTREEITEM m_hCarWidth;
		HTREEITEM m_hCarLength;

		HTREEITEM m_hStationPreBordingAre;
	//	HTREEITEM m_hBordingAreaServiceLocation;
		HTREEITEM m_hBordingAreaInconstrain;
		
		HTREEITEM m_hPlatFormWidth;
		HTREEITEM m_hPlatFormLength;
		HTREEITEM m_hCarNum;
		HTREEITEM m_hCarCapacity;

		HTREEITEM m_hEntryDoorNum;
		HTREEITEM m_hExitDoorNum;
		HTREEITEM m_hExitDoorType;
		HTREEITEM m_hOrientation;
		HTREEITEM m_hStation;

		HTREEITEM m_hBaggageProcBarrier;
		HTREEITEM m_hBaggageMovingPath;

		HTREEITEM m_hGateType;
		HTREEITEM m_hPickupFromMapItem;
		
		HTREEITEM m_hRetailType;//Retail processor type
		HTREEITEM m_hStoreArea; //area for retail processor
		//CStationLayout m_stationLayout;
		IntegratedStation m_tempStation;

		ElevatorProc m_tempElevator;
		
		HTREEITEM m_hWidthItem;
		HTREEITEM m_hLengthItem;
		HTREEITEM m_hAngleItem;

		DistanceUnit m_dConveyWidth;
		DistanceUnit m_dProcessorLength;
		double	  m_dPathAngle;
		bool m_bZPosHasBeenChanged;	

		//	Add by kevin
		DistanceUnit m_dWidth;
		DistanceUnit m_dThreshold;
		DistanceUnit m_dThreshold2;
		DistanceUnit m_dLowerLimit;
		DistanceUnit m_dUpperLimit;
		int			 m_nAutoNumber;
		CString		 m_strMarking;
		CString		 m_strMarking2;


		//add by hans 06-3-29,add billboard processor
		BillboardProc m_tempBillBoardProc;
		HTREEITEM m_hBillboardHeight;
		HTREEITEM m_hBillboardThickness;
		HTREEITEM m_hBillboardAngle;
		HTREEITEM m_hBillboardType;
		HTREEITEM m_hBillBoardText;
		HTREEITEM m_hBillBoardBitmapPath;
		HTREEITEM m_hBillBoardCoefficient;
		
		HTREEITEM m_hBillboardRadius;
		HTREEITEM m_hExpSurePath;
		std::vector<ARCVector3> m_vBillboardExposureArea;

		//double m_dBbHeight;
		//double m_dBbThickness;
		//int m_nBbType;
		//CString m_strBbText;
		//CString m_strBbPath;
		//double m_dBbCoefficient;
		
private:
	//void CaculateConveyorCapacity( Conveyor* _pConveyor );
// Implementation
protected:
	CFloor2* GetPointFloor(int nLevel);
	CFloor2* GetPointFloor(const Point& pt);
	BOOL CreateTreeViewMenu(const CString& strCaption, UINT nID, CMenu* pMenu);
	HTREEITEM m_hServiceLocation;
	HTREEITEM m_hInConstraint;
	HTREEITEM m_hOutConstraint;
	HTREEITEM m_hQueue;

	HTREEITEM m_hBackup;

	
	//	add by kevin
	HTREEITEM m_hWidth;
	HTREEITEM m_hRunwayNumber;
	HTREEITEM m_hThreshold;
	HTREEITEM m_hThresholdA;
	HTREEITEM m_hThresholdB;
	HTREEITEM m_hLowerLimit;
	HTREEITEM m_hUpperLimit;
	HTREEITEM m_hAutomaticallyNumber;
	HTREEITEM m_hMarking;
	HTREEITEM m_hMarkingA;
	HTREEITEM m_hMarkingB;
	//Hold Item
	HTREEITEM m_hFix;
	HTREEITEM m_hInBound;
	HTREEITEM m_hOutBoundLegnm;
	HTREEITEM m_hOutBoundLegmin;
	HTREEITEM m_hRight;
	HTREEITEM m_hMaxAlt;
	HTREEITEM m_hMinAlt;
	
	HTREEITEM m_EntryPoint ;
	CString m_strFix;
	int m_nInBoundDegree;
	double m_dOutBoundnm;
	double m_dOutBoundmin;
	bool m_bRight;
	double m_dmaxAlt;
	double m_dminAlt;

	HTREEITEM m_hBandInterval;
	double m_dbandinterval;
	
	GateType m_emGateType;
	int		 m_emRetailType;
	std::vector<CString> m_strfixes;
	//HTREEITEM m_hMovingSideSpeed;
	//int m_iMovingSpeed;
	// data to save the modified valued.
	// it used only if the data changed.
	// otherwise keep as default.
	std::vector<ARCVector3> m_vServiceLocation;
	std::vector<ARCVector3> m_vInConstraint;
	std::vector<ARCVector3> m_vOutConstraint;
	std::vector<ARCVector3> m_vQueieList;

	std::vector<ARCVector3> m_vInOutWithBackup;


public:
	class CEntryPointAndIndex
	{
	public:
       ARCVector3 point ;
	   int  index ;
	   friend int operator == ( const CEntryPointAndIndex& _v, const CEntryPointAndIndex& _u )	
	   {
              return _v.index == _u.index ;
	   }
	   friend bool operator < ( const CEntryPointAndIndex& _v, const CEntryPointAndIndex& _u )	
		{
			return  _v.index < _u.index ;
		}
	} ;

protected:
	//entrypoint 
	std::vector<CEntryPointAndIndex *> m_EntryPointIndex ;
	CString m_csName;
	int m_nProcType;
	int m_nFixed;	// -1 for no changed, 0, 1 for changed.

	std::vector<ARCVector3> m_vBaggagMovingPath;
	std::vector<ARCVector3> m_vBaggageBarrier;

	std::vector<ARCVector3> m_vStoreArea;


	HTREEITEM m_hRClickItem;
	//Processor* m_pProc;
	Processor* m_pProc;

	//if station layout is changed, then all staion's port1,port2 will
	// changed also, so all railway linked with station will be changed ,too.
	int  m_iUpdateAllRailWay;
	

	// pre process the name entry
	// ,
	// space
	// upper case
	void PreProcessName();

	enum FallbackReason GetFallbackReason( int _nProcType );
	//get the z position 
	//the
	int GetZValue(float fZ);
	//fz :real altitude
	CFloor2* GetFloorFromZPos(float fZ);

	bool CheckServiceLocation( int _nProcType );

	bool CheckQueueLocation(int _nProcType);

	bool CheckInConstraintLocation(int _nProcType);

	bool CheckOutConstraintLocation(int _nProcType);

	void autoCalRunwayMark();

	void addTempAirsideProc(int _nProcType);

	int GetFloor(DistanceUnit zPos);
	// base on the string in the combo level1 
	void SetTypeByLevel1();

	CString GetProjPath();

	InputTerminal* GetInputTerminal();

	// load level1 only
	void LoadCombo();

	// load type 
	void LoadType();

	// base on the processor load the tree.
	void LoadTree();

	void convertPathByAngle(Path& pInPath ,double _newAngle);
	bool checkRelation(CString strlevelName);
	// Generated message map functions
	//{{AFX_MSG(CProcPropDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDropdownComboLevel2();
	afx_msg void OnDropdownComboLevel3();
	afx_msg void OnDropdownComboLevel4();
	afx_msg void OnKillfocusComboLevel1();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProcpropPickfrommap();
	afx_msg void OnProcpropToggledqueuefixed();
	afx_msg void OnProcpropToggledBackup();
	virtual void OnOK();
	afx_msg void OnSelchangeComboType();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDefineGateType();
	afx_msg void OnDefineRetailType();//create drop list for retail 
	afx_msg void OnCarCapacity();
	afx_msg void OnCarNumbers();
	afx_msg void OnPlatformWidth();
	afx_msg void OnPlatformLength();
	afx_msg void OnCarWidth();
	afx_msg void OnCarLength();
	afx_msg void OnDefineEntryDoorNum();
	afx_msg void OnDefineWaitAreaLength();
	afx_msg void OnDefineWaitAreaWidth();
	afx_msg void OnDefineNumberOfLift();
	afx_msg void OnDefineLiftLength();
	afx_msg void OnDefineLiftWidth();
	afx_msg void OnDefineMinFloor();
	afx_msg void OnDefineMaxFloor();
	afx_msg void OnDefineToggled();
	afx_msg void OnDefineExitDoorNumber();
	afx_msg void OnExitdoorToggled();
	afx_msg void OnStationOrientation();
	afx_msg void OnProcpropDelete();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnProcpropDefineZPos();
	afx_msg void OnMenueConveyWidth();
	afx_msg void OnAdjustLength();
	afx_msg void OnAdjustAngle();
	afx_msg LRESULT OnModifyValue(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	afx_msg void OnDropdownComboShape();
	afx_msg void OnProcpropModifyValue(UINT nID);
	DECLARE_MESSAGE_MAP()
	int m_nShapeIndex;
public:
	int GetShapeIndex(void);
protected:
	void ForceControlStatus(void);

public:
	int m_nForceProcType;
	afx_msg void OnBnClickedOk();
	afx_msg void OnProcpropChanglatlong();

private:
	int m_nFixeOperationType;
	bool m_bBackup;
	bool m_bFirst;

	bool m_bInOutConstraintProc;//Proc has In and Out Constrant
public:
	afx_msg void OnBillboardModifyheight();
	afx_msg void OnBillboardModifythickness();
	afx_msg void OnBillboardText();
	afx_msg void OnBillboardLogo();
	afx_msg void OnBillboardModifytext();
	afx_msg void OnBillboardModifypath();
	afx_msg void OnBillboardModifycoefficient();
	afx_msg void OnBillboardModifyangle();
	afx_msg void OnHoldChooseafix();
	afx_msg void OnHoldargumentEdit();
	afx_msg void OnRunwaythresholdaModify();
	afx_msg void OnRunwaymarkModify();

	//////////////////////////////////////////////////////////////////////////
	//added by cj for entry point , 2008/5/30/14:00
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnAddEntryPoint() ;
	afx_msg void OnDelEntryPoint() ;
protected:
		void LoadEntryTree(int nProcType) ;
		void FormatQueuePointString(CString& str,Point& point,int nProcType) ;
		void  SetEntryPointIndex(Processor* pro);
		void  InitEntryPoints();
		bool SortProTypeString(std::set<CString>& proTypeSet,std::map<CString,int>& ProTypeMap,int nStartPos, int nEndPos);
public:
	afx_msg void OnStnClickedStaticLine1();


///////////for Bridge processor//////////////////
	afx_msg void OnPropNumberModify();
protected:
	void InitBridgeConnectPointDefaultValue(int idx, HTREEITEM hItem);
	void AcquireDataForBridgeConnector();

	std::vector<BridgeConnector::ConnectPoint> m_vBridgeConnectPoints;
	int m_nBridgeConnectPointCount;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCPROPDLG_H__2DCA0397_B824_4F8C_9C8B_C34BA79B3346__INCLUDED_)
