#include "StdAfx.h"
#include "DlgImportFltPaxData.h"
#include "Common/ProjectManager.h"
#include "..\inputs\fltdata.h"
#include "..\inputs\paxdata.h"
#include "..\Inputs\BridgeConnectorPaxData.h"
#include "..\Inputs\CON_DB.H"
#include "Common\exeption.h"
#include "Common\fileman.h"
#include "Common\ZipInfo.h"

IMPLEMENT_DYNAMIC(CDlgImportFltPaxData, CXTResizeDialog)
CDlgImportFltPaxData::CDlgImportFltPaxData(FLTPAXDATATTYPE emType,InputTerminal* pInTerm,ConstraintDatabase* pFltPaxDB,CWnd* pParent)
	: CXTResizeDialog(CDlgImportFltPaxData::IDD, pParent)
	,m_emType(emType)
	,m_pInterm(pInTerm)
	,m_pFltPaxDB(pFltPaxDB)
{
	m_pDataSet = NULL;
	m_pCSVDB = NULL;
}


CDlgImportFltPaxData::~CDlgImportFltPaxData(void)
{
	ClearData();
}

void CDlgImportFltPaxData::DoDataExchange( CDataExchange* pDX )
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
	DDX_Control(pDX,IDC_COMBO_PROJECT,m_wndCombox);
	DDX_Control(pDX,IDC_BROWSE_EXPORTED,m_wndExportedProject);
	DDX_Control(pDX,IDC_BROWSE_CSVFILE,m_wndCSVFile);
}

BEGIN_MESSAGE_MAP(CDlgImportFltPaxData, CXTResizeDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PROJECT, OnCbnSelchangeComboProject)
	ON_EN_UPDATE(IDC_BROWSE_EXPORTED, OnUpdateExport)
	ON_EN_UPDATE(IDC_BROWSE_CSVFILE, OnUpdateCSVFile)
	ON_BN_CLICKED(IDC_BUTTON_APPEND,OnAppend)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE,OnReplace)
	ON_BN_CLICKED(IDC_CHECK_ALL,OnCheckAll)
	ON_BN_CLICKED(IDC_RADIO_LOCAL,OnCheckLocalProject)
	ON_BN_CLICKED(IDC_RADIO_EXPORTED,OnCheckExportedProject)
	ON_BN_CLICKED(IDC_RADIO_CSVFILE,OnCheckCSVFile)
END_MESSAGE_MAP()


BOOL CDlgImportFltPaxData::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetImportDialogTitle();
	InitRadioStatus();
	InitComboxContent();
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(TRUE);
	SetListCtrlContent(NULL);
	m_wndExportedProject.EnableFileBrowseButton(".zip","zip Files (*.zip)|*.zip||");
	m_wndCSVFile.EnableFileBrowseButton(".CSV","CSV Files (*.CSV)|*.CSV||");

	SetResize(IDC_RADIO_LOCAL,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_COMBO_PROJECT,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_RADIO_EXPORTED,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_BROWSE_EXPORTED,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_RADIO_CSVFILE,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_BROWSE_CSVFILE,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_CHECK_ALL,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_CHECK_APPEND,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_APPEND,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_REPLACE,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgImportFltPaxData::OnCbnSelchangeComboProject()
{
	int iCursel = m_wndCombox.GetCurSel();
	if (iCursel == LB_ERR)
		return;
	
	CString strLocalProject;
	m_wndCombox.GetLBText(iCursel,strLocalProject);

	PROJECTINFO pi;	
	PROJMANAGER->GetProjectInfo(strLocalProject, &pi, "");//get the project information

	ClearData();//reset data
	ConstraintDatabase* pConDB = LoadDataFromFile(pi.path);

	SetListCtrlContent(pConDB);
}

void CDlgImportFltPaxData::OnUpdateExport()
{
	CString strFilePath;
	m_wndExportedProject.GetWindowText(strFilePath);
	if (strFilePath.IsEmpty())
		return;
	
	CString strTempPathFile;
	CreateTempZipFile(strTempPathFile,strFilePath);

	CString strPath;
	strPath = PROJMANAGER->m_csAppPath + "\\ExportTemp";
	FileManager::DeleteDirectory( strPath );
	CreateDirectory( strPath, NULL );
	if( !FileManager::IsFile( strTempPathFile + "\\" + "arcterm_export.ini" ) ) 
	{
		FileManager::CopyDirectory( strTempPathFile, strPath ,NULL);
		FileManager::DeleteDirectory( strTempPathFile );
		SetListCtrlContent(NULL);
		return;
	}

	CString _strInputZip;
	readIniFileData( strTempPathFile, _strInputZip);
	if(unzipFiles( _strInputZip, strPath) == false)
	{
		SetListCtrlContent(NULL);
		return;
	}

	InputFiles fileType = FlightDataFile;
	CString strFileName;
	switch (m_emType)
	{
	case FLIGHT_DELAYS:
	case FLIGHT_LOAD_FACTORS:
	case FLIGHT_AC_CAPACITIES:
		 fileType = FlightDataFile;
		 strFileName = "FLIGHT.DAT";
		break;
	case PAX_GROUP_SIZE:
	case PAX_LEAD_LAG:
	case PAX_IN_STEP:
	case PAX_SIDE_STEP:
	case PAX_SPEED:
	case PAX_VISIT_TIME:
	case PAX_RESPONSE_TIME:
	case PAX_SPEED_IMPACT:
	case PAX_PUSH_PROPENSITY:
	case VISITOR_STA_TRIGGER:
		fileType = PassengerDataFile;
		strFileName = "PAXDATA.MSC";
		break;
	case  ENTRY_FLIGHT_TIME_DISTRIBUTION:
		fileType = BridgeConnectorPaxDataFile;
		strFileName = "BCPAXDATA.TXT";
		break;
	}

	CString strProjectPath;
	strProjectPath.Format(_T("%s\\%s"),strPath,strFileName);
	ConstraintDatabase* pConDB = LoadDataFromFile(strProjectPath);
	SetListCtrlContent(pConDB);
}

void CDlgImportFltPaxData::OnUpdateCSVFile()
{
	ClearData();//reset data
	CString strFilePath;
	m_wndCSVFile.GetWindowText(strFilePath);
	if (strFilePath.IsEmpty())
		return;
	
	ArctermFile p_file;
	if(p_file.openFile(strFilePath,READ) )
	{
		p_file.getLine();
		char str[64];
		p_file.getField (str, SMALL);
		if (_stricmp (str, sFltPaxString[m_emType]))
		{
			CString strMsg;
			strMsg.Format("Can not open file:\r\n%s", strFilePath );
			MessageBox( strMsg, "ERROR", MB_OK|MB_ICONERROR );
			SetListCtrlContent(NULL);
			return;
		}
			
		switch (m_emType)
		{
		case FLIGHT_DELAYS:
			m_pCSVDB = new FlightConstraintDatabase();
			m_pCSVDB->setUnits("MINUTES");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case FLIGHT_LOAD_FACTORS:
			m_pCSVDB = new FlightConstraintDatabaseWithSchedData();
			m_pCSVDB->setUnits("PERCENT");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case FLIGHT_AC_CAPACITIES:
			m_pCSVDB = new FlightConstraintDatabaseWithSchedData();
			m_pCSVDB->setUnits("COUNT");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_GROUP_SIZE:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("COUNT");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_LEAD_LAG:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("MIN");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_IN_STEP:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("METERS");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_SIDE_STEP:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("METERS");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_SPEED:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("METERS/SEC");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_VISIT_TIME:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("MIN");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_RESPONSE_TIME:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("SECONDS");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_SPEED_IMPACT:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("METERS");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_PUSH_PROPENSITY:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("PERCENT");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case VISITOR_STA_TRIGGER:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("MIN");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case ENTRY_FLIGHT_TIME_DISTRIBUTION:
			m_pCSVDB = new CMobileElemConstraintDatabase();
			m_pCSVDB->setUnits("SECONDS");
			m_pCSVDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		}
	}
	
	SetListCtrlContent(m_pCSVDB);
}

void CDlgImportFltPaxData::SetImportDialogTitle()
{
	switch (m_emType)
	{
	case FLIGHT_DELAYS:
		SetWindowText("Importing(Flight Delays)");
		break;
	case FLIGHT_LOAD_FACTORS:
		SetWindowText("Importing(Passenger Loads)");
		break;
	case FLIGHT_AC_CAPACITIES:
		SetWindowText("Importing(Aircraft Capacity)");
		break;
	case PAX_GROUP_SIZE:
		SetWindowText("Importing(Group Size)");
		break;
	case PAX_LEAD_LAG:
		SetWindowText("Importing(Lead Lag Time)");
		break;
	case PAX_IN_STEP:
		SetWindowText("Importing(In Step)");
		break;
	case PAX_SIDE_STEP:
		SetWindowText("Importing(Side Step)");
		break;
	case PAX_SPEED:
		SetWindowText("Importing(Speed)");
		break;
	case PAX_VISIT_TIME:
		SetWindowText("Importing(Visit Time)");
		break;
	case PAX_RESPONSE_TIME:
		SetWindowText("Importing(Response Time)");
		break;
	case PAX_SPEED_IMPACT:
		SetWindowText("Importing(Speed Impact)");
		break;
	case PAX_PUSH_PROPENSITY:
		SetWindowText("Importing(Push Propensity)");
		break;
	case VISITOR_STA_TRIGGER:
		SetWindowText("Importing(Visitor STA Trigger)");
		break;
	case ENTRY_FLIGHT_TIME_DISTRIBUTION:
	{
		CString s;
		s.LoadString(IDS_TVNN_ENTRYFLIGHTTIMEDISTRIBUTION);
		CString strTitle;
		strTitle.Format(_T("Importing(%s)"),s);
		SetWindowText(strTitle);
	}
		break;
	default:
		break;
	}
}

static bool CompareModifedTime(const PROJECTINFO& pi1,const PROJECTINFO& pi2)
{
	if (pi1.modifytime < pi2.modifytime)
	{
		return false;
	}
	return true;
}

void CDlgImportFltPaxData::InitComboxContent()
{
	std::vector<CString> vList;

	CProjectManager* pProjectManager=CProjectManager::GetInstance();
	pProjectManager->GetProjectList(vList);
	std::vector<PROJECTINFO> vProjectList;
	for(int i=0; i<static_cast<int>(vList.size()); i++) 
	{
		CString projName = vList[i];
		PROJECTINFO pi;	
		PROJMANAGER->GetProjectInfo(projName, &pi, "");//get the project information
		vProjectList.push_back(pi);
	}

	std::sort(vProjectList.begin(),vProjectList.end(),CompareModifedTime);

	for (unsigned nPorject = 0; nPorject < vProjectList.size(); nPorject++)
	{
		PROJECTINFO& pProject = vProjectList[nPorject];
		m_wndCombox.AddString(pProject.name);
	}
}

void CDlgImportFltPaxData::InitListCtrlHeader(ConstraintDatabase* pConDB)
{
	while(m_wndListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		m_wndListCtrl.DeleteColumn(0);

	// set list ctrl	
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	LVCOLUMN lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[2][128];
	if (pConDB == NULL)
	{
		strcpy( columnLabel[1], "Distribution ( UNIT )" );
	}
	else
	{
		sprintf( columnLabel[1], "Distribution ( %s )", pConDB->getUnits() );
	}

	CString csLabel;
	switch( m_emType )
	{
	case FLIGHT_DELAYS:
	case FLIGHT_LOAD_FACTORS:
	case FLIGHT_AC_CAPACITIES:
		strcpy( columnLabel[0], "Flight Type" );
		break;

	default:
		strcpy( columnLabel[0], "Mobile Element Type" );		
		break;
	}

	int DefaultColumnWidth[] = { 288, 450 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	for( int i=0; i<2; i++ )
	{
		CStringList strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.fmt = LVCFMT_NOEDIT;
		m_wndListCtrl.InsertColumn( i, &lvc );
	}
}

void CDlgImportFltPaxData::SetListCtrlContent(ConstraintDatabase* pConDB)
{
	InitListCtrlHeader(pConDB);
	m_wndListCtrl.DeleteAllItems();

	if( pConDB == NULL )
		return;
	int nCount = pConDB->getCount();
	BOOL bCheckAll = ((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck();
	for( int i=0; i<nCount; i++ )
	{
		ConstraintEntry* pEntry = pConDB->getItem( i );
		const Constraint* pCon = pEntry->getConstraint();
		CString szName;
		pCon->screenPrint( szName, 0, 256 );
		const ProbabilityDistribution* pProbDist = pEntry->getValue();
		char szDist[1024];
		pProbDist->screenPrint( szDist);
		int nIdx = m_wndListCtrl.InsertItem( i, szName.GetBuffer(0) );
		m_wndListCtrl.SetItemText( nIdx, 1, szDist);

		m_wndListCtrl.SetItemData(i,(DWORD)pEntry);
		m_wndListCtrl.SetCheck(i,bCheckAll);
	}	
}

bool CDlgImportFltPaxData::CreateTempZipFile( CString& strTempExtractPath,const CString& strFilePath )
{
	CString strTempPath = getTempPath();
	strTempPath.TrimRight('\\');
	strTempExtractPath = strTempPath + "\\" + "ARCTempExtractPath";
	FileManager::DeleteDirectory( strTempExtractPath );
	CreateDirectory( strTempExtractPath, NULL );
	// extract zip to temp path
	if( !unzipFiles( strFilePath, strTempExtractPath ) )
	{
		return false;
	}	
	return true;
}

bool CDlgImportFltPaxData::unzipFiles( const CString& _strZip, const CString& _strPath )
{
	if(PathFileExists(_strZip) == FALSE)
		return true;

	BeginWaitCursor();

	CZipInfo InfoZip;
	if (!InfoZip.ExtractFiles(_strZip,CString(_strPath + "\\")))
	{
		EndWaitCursor();
		AfxMessageBox("Unzip file failed! file: " + _strZip, MB_OK);
		return false;
	}


	EndWaitCursor();
	return true;
}

bool CDlgImportFltPaxData::readIniFileData( const CString& _strTempExtractPath, CString& _strInputZip)
{
	ArctermFile inifile;
	inifile.openFile( _strTempExtractPath + "\\" + "arcterm_export.ini", READ );

	int _iVersion = 0;
	inifile.getLine();
	inifile.getInteger( _iVersion );

	if(_iVersion >= 4)
	{
		inifile.getLine();
	}
	char _szFileName[MAXPATH];
	inifile.getLine();
	inifile.skipField(1);
	inifile.getField( _szFileName, MAXPATH );
	_strInputZip = _strTempExtractPath + "\\" + _szFileName;

	return true;
}

CString CDlgImportFltPaxData::getTempPath( const CString& _strDefault  )
{
	char _szTempPath[ MAX_PATH + 1];
	if( GetTempPath( MAX_PATH, _szTempPath ) == 0 )
		return  _strDefault;
	return CString( _szTempPath );
}

ConstraintDatabase* CDlgImportFltPaxData::LoadDataFromFile( const CString& strFileName )
{
	ClearData();
	ConstraintDatabase* pConDB = NULL;
	try
	{
		switch( m_emType )
		{
		case FLIGHT_DELAYS:
			m_pDataSet = new FlightData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet(strFileName);
			pConDB = ((FlightData*)m_pDataSet)->getDelays();
			break;
		case FLIGHT_LOAD_FACTORS:
			m_pDataSet = new FlightData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((FlightData*)m_pDataSet)->getLoads();
			break;
		case FLIGHT_AC_CAPACITIES:
			m_pDataSet = new FlightData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((FlightData*)m_pDataSet)->getCapacity();
			break;
		case PAX_GROUP_SIZE:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getGroups();
			break;
		case PAX_LEAD_LAG:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getLeadLagTime();
			break;
		case PAX_IN_STEP:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getInStep();
			break;
		case PAX_SIDE_STEP:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getSideStep();
			break;
		case PAX_SPEED:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getSpeed();
			break;
		case PAX_VISIT_TIME:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getVisitTime();
			break;
		case PAX_RESPONSE_TIME:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getResponseTime();
			break;
		case PAX_SPEED_IMPACT:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getImpactSpeed();
			break;
		case PAX_PUSH_PROPENSITY:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getPushPropensity();
			break;
		case VISITOR_STA_TRIGGER:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet(strFileName );
			pConDB = ((PassengerData*)m_pDataSet)->getVisitorSTATrigger();
			break;
		case ENTRY_FLIGHT_TIME_DISTRIBUTION:
			m_pDataSet = new BridgeConnectorPaxData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet(strFileName);
			pConDB = ((BridgeConnectorPaxData*)m_pDataSet)->getEntryFlightTimeDestribution();
			break;
		}
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;	
	}

	return pConDB;
}

void CDlgImportFltPaxData::SetOperation( int iType )
{
	m_iOperation = iType;
}

void CDlgImportFltPaxData::InitRadioStatus()
{
	if (m_iOperation == 0)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LOCAL))->SetCheck(TRUE);
		m_wndExportedProject.EnableWindow(FALSE);
		m_wndCSVFile.EnableWindow(FALSE);
	}
	else if (m_iOperation == 1)
	{
		((CButton*)GetDlgItem(IDC_RADIO_EXPORTED))->SetCheck(TRUE);
		m_wndCombox.EnableWindow(FALSE);
		m_wndCSVFile.EnableWindow(FALSE);
	}
	else if (m_iOperation == 2)
	{
		((CButton*)GetDlgItem(IDC_RADIO_CSVFILE))->SetCheck(TRUE);
		m_wndCombox.EnableWindow(FALSE);
		m_wndExportedProject.EnableWindow(FALSE);
	}
}

void CDlgImportFltPaxData::AddData( ConstraintEntry* pEntry )
{
	if (m_emType == FLIGHT_DELAYS || m_emType == FLIGHT_LOAD_FACTORS || m_emType == FLIGHT_AC_CAPACITIES)
	{
		FlightConstraint* pFltCon = new FlightConstraint;
		*pFltCon = *(FlightConstraint*)pEntry->getConstraint();

		ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pEntry->getValue());
		ConstraintEntry* pNewEntry = new ConstraintEntry();
		pNewEntry->initialize( pFltCon, pDist );
		m_pFltPaxDB->addEntry(pNewEntry);
	}
	else
	{
		char buf[2560];
		pEntry->getConstraint()->WriteSyntaxStringWithVersion( buf );
		CMobileElemConstraint* pMobileCon	= new CMobileElemConstraint(m_pInterm);
		pMobileCon->setConstraintWithVersion( buf );

		ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pEntry->getValue());
		ConstraintEntry* pNewEntry = new ConstraintEntry();
		pNewEntry->initialize( pMobileCon, pDist );
		m_pFltPaxDB->addEntry(pNewEntry);
	}
}


void CDlgImportFltPaxData::OnAppend()
{
	BOOL bReplaceEqual = ((CButton*)GetDlgItem(IDC_CHECK_APPEND))->GetCheck();
	//remove the exist item
	for (int i = 0; i < m_wndListCtrl.GetItemCount(); i++)
	{
		if (m_wndListCtrl.GetCheck(i) == TRUE)
		{
			ConstraintEntry* pEntry = (ConstraintEntry*)m_wndListCtrl.GetItemData(i);
			if (bReplaceEqual)
			{
				int idx = GetExsitFltPaxData(pEntry);
				if (idx != -1)
				{
					m_pFltPaxDB->removeItem(idx);
				}
			}
		}
	}

	//insert 
	for (int j = 0; j < m_wndListCtrl.GetItemCount(); j++)
	{
		if (m_wndListCtrl.GetCheck(j) == TRUE)
		{
			ConstraintEntry* pEntry = (ConstraintEntry*)m_wndListCtrl.GetItemData(j);
			AddData(pEntry);
		}
	}
	CXTResizeDialog::OnOK();
}

void CDlgImportFltPaxData::OnReplace()
{
	m_pFltPaxDB->clear();
	for (int i = 0; i < m_wndListCtrl.GetItemCount(); i++)
	{
		if (m_wndListCtrl.GetCheck(i) == TRUE)
		{
			ConstraintEntry* pEntry = (ConstraintEntry*)m_wndListCtrl.GetItemData(i);
			AddData(pEntry);
		}
	}
	CXTResizeDialog::OnOK();
}

int CDlgImportFltPaxData::GetExsitFltPaxData( ConstraintEntry* pEntry )
{
	for (int i = 0; i < m_pFltPaxDB->getCount(); i++)
	{
		ConstraintEntry* pItem = m_pFltPaxDB->getItem(i);
		if (*pEntry == *pItem)
		{
			return i;
		}
	}
	return -1;
}

void CDlgImportFltPaxData::ClearData()
{
	if (m_pDataSet)
	{
		delete m_pDataSet;
		m_pDataSet = NULL;
	}

	if (m_pCSVDB)
	{
		delete m_pCSVDB;
		m_pCSVDB = NULL;
	}
}

void CDlgImportFltPaxData::OnCheckAll()
{
	BOOL bCheckAll = ((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck();
	if (bCheckAll)//need check all item of list control
	{
		for (int i = 0; i < m_wndListCtrl.GetItemCount(); i++)
		{
			m_wndListCtrl.SetCheck(i,TRUE);
		}
	}
}

void CDlgImportFltPaxData::OnCheckLocalProject()
{
	GetDlgItem(IDC_COMBO_PROJECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BROWSE_EXPORTED)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE_CSVFILE)->EnableWindow(FALSE);
}

void CDlgImportFltPaxData::OnCheckExportedProject()
{
	GetDlgItem(IDC_COMBO_PROJECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE_EXPORTED)->EnableWindow(TRUE);
	GetDlgItem(IDC_BROWSE_CSVFILE)->EnableWindow(FALSE);
}

void CDlgImportFltPaxData::OnCheckCSVFile()
{
	GetDlgItem(IDC_COMBO_PROJECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE_EXPORTED)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE_CSVFILE)->EnableWindow(TRUE);
}
