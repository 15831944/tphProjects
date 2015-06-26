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

IMPLEMENT_DYNAMIC(CDlgImportFltPaxData, CDialog)
CDlgImportFltPaxData::CDlgImportFltPaxData(FLTPAXDATATTYPE emType,InputTerminal* pInTerm,CWnd* pParent)
	: CDialog(CDlgImportFltPaxData::IDD, pParent)
	,m_emType(emType)
	,m_pInterm(pInTerm)
{
	m_pDataSet = NULL;
	m_pConDB = NULL;
}


CDlgImportFltPaxData::~CDlgImportFltPaxData(void)
{
	if (m_pDataSet)
	{
		delete m_pDataSet;
		m_pDataSet = NULL;
	}
}

void CDlgImportFltPaxData::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
	DDX_Control(pDX,IDC_COMBO_PROJECT,m_wndCombox);
	DDX_Control(pDX,IDC_BROWSE_EXPORTED,m_wndExportedProject);
	DDX_Control(pDX,IDC_BROWSE_CSVFILE,m_wndCSVFile);
}

BEGIN_MESSAGE_MAP(CDlgImportFltPaxData, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PROJECT, OnCbnSelchangeComboProject)
	ON_EN_UPDATE(IDC_BROWSE_EXPORTED, OnUpdateExport)
	ON_EN_UPDATE(IDC_BROWSE_CSVFILE, OnUpdateCSVFile)
END_MESSAGE_MAP()


BOOL CDlgImportFltPaxData::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetImportDialogTitle();
	InitRadioStatus();
	InitListCtrlHeader();
	InitComboxContent();
	m_wndExportedProject.EnableFileBrowseButton(".zip","zip Files (*.zip)|*.zip||");
	m_wndCSVFile.EnableFileBrowseButton(".CSV","zip Files (*.CSV)|*.CSV||");

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

	try
	{
		switch( m_emType )
		{
		case FLIGHT_DELAYS:
			m_pDataSet = new FlightData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((FlightData*)m_pDataSet)->getDelays();
			break;
		case FLIGHT_LOAD_FACTORS:
			m_pDataSet = new FlightData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((FlightData*)m_pDataSet)->getLoads();
			break;
		case FLIGHT_AC_CAPACITIES:
			m_pDataSet = new FlightData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((FlightData*)m_pDataSet)->getCapacity();
			break;
		case PAX_GROUP_SIZE:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((PassengerData*)m_pDataSet)->getGroups();
			break;
		case PAX_LEAD_LAG:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((PassengerData*)m_pDataSet)->getLeadLagTime();
			break;
		case PAX_IN_STEP:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((PassengerData*)m_pDataSet)->getInStep();
			break;
		case PAX_SIDE_STEP:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((PassengerData*)m_pDataSet)->getSideStep();
			break;
		case PAX_SPEED:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((PassengerData*)m_pDataSet)->getSpeed();
			break;
		case PAX_VISIT_TIME:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((PassengerData*)m_pDataSet)->getVisitTime();
			break;
		case PAX_RESPONSE_TIME:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((PassengerData*)m_pDataSet)->getResponseTime();
			break;
		case PAX_SPEED_IMPACT:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((PassengerData*)m_pDataSet)->getImpactSpeed();
			break;
		case PAX_PUSH_PROPENSITY:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			m_pConDB = ((PassengerData*)m_pDataSet)->getPushPropensity();
			break;
		case VISITOR_STA_TRIGGER:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( pi.path );
			break;
		case ENTRY_FLIGHT_TIME_DISTRIBUTION:
			m_pDataSet = new BridgeConnectorPaxData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet(pi.path);
			m_pConDB = ((BridgeConnectorPaxData*)m_pDataSet)->getEntryFlightTimeDestribution();
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

	SetListCtrlContent();
}

void CDlgImportFltPaxData::OnUpdateExport()
{
	CString strFilePath;
	m_wndExportedProject.GetWindowText(strFilePath);
	CString strTempPathFile;
	CreateTempZipFile(strTempPathFile,strFilePath);

	CString strPath;
	strPath = PROJMANAGER->m_csAppPath + "\\ExportTemp";
	CreateDirectory( strPath, NULL );
	if( !FileManager::IsFile( strTempPathFile + "\\" + "arcterm_export.ini" ) ) 
	{
		FileManager::CopyDirectory( strTempPathFile, strPath ,NULL);
		FileManager::DeleteDirectory( strTempPathFile );
		return;
	}

	CString _strInputZip;
	readIniFileData( strTempPathFile, _strInputZip);
	unzipFiles( _strInputZip, strPath);

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
	LoadDataFromFile(strProjectPath);
	SetListCtrlContent();
}

void CDlgImportFltPaxData::OnUpdateCSVFile()
{
	CString strFilePath;
	m_wndCSVFile.GetWindowText(strFilePath);
	ArctermFile p_file;
	char str[64];
	if(p_file.openFile(strFilePath,READ) )
	{
		p_file.getLine();
		p_file.getField (str, SMALL);
		if (_stricmp (str, sFltPaxString[m_emType]))
		{
			AfxMessageBox(_T(""));
			return;
		}
			
		switch (m_emType)
		{
		case FLIGHT_DELAYS:
			m_pConDB = new FlightConstraintDatabase();
			m_pConDB->setUnits("MINUTES");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case FLIGHT_LOAD_FACTORS:
			m_pConDB = new FlightConstraintDatabaseWithSchedData();
			m_pConDB->setUnits("PERCENT");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case FLIGHT_AC_CAPACITIES:
			m_pConDB = new FlightConstraintDatabaseWithSchedData();
			m_pConDB->setUnits("COUNT");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_GROUP_SIZE:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("COUNT");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_LEAD_LAG:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("MIN");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_IN_STEP:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("METERS");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_SIDE_STEP:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("METERS");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_SPEED:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("METERS/SEC");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_VISIT_TIME:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("MIN");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_RESPONSE_TIME:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("SECONDS");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_SPEED_IMPACT:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("METERS");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_PUSH_PROPENSITY:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("PERCENT");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case VISITOR_STA_TRIGGER:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("MIN");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case ENTRY_FLIGHT_TIME_DISTRIBUTION:
			m_pConDB = new CMobileElemConstraintDatabase();
			m_pConDB->setUnits("SECONDS");
			m_pConDB->readDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		}
	}
	
	SetListCtrlContent();
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

void CDlgImportFltPaxData::InitComboxContent()
{
	std::vector<CString> vList;

	CProjectManager* pProjectManager=CProjectManager::GetInstance();
	pProjectManager->GetProjectList(vList);
	for(int i=0; i<static_cast<int>(vList.size()); i++) 
	{
		CString projName = vList[i];
		m_wndCombox.AddString(projName);
	}
}

void CDlgImportFltPaxData::InitListCtrlHeader()
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
	if (m_pConDB == NULL)
	{
		strcpy( columnLabel[1], "Distribution ( UNIT )" );
	}
	else
	{
		sprintf( columnLabel[1], "Distribution ( %s )", m_pConDB->getUnits() );
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

	int DefaultColumnWidth[] = { 200, 320 };
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

void CDlgImportFltPaxData::SetListCtrlContent()
{
	m_wndListCtrl.DeleteAllItems();

	if( m_pConDB == NULL )
		return;
	int nCount = m_pConDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		ConstraintEntry* pEntry = m_pConDB->getItem( i );
		const Constraint* pCon = pEntry->getConstraint();
		CString szName;
		pCon->screenPrint( szName, 0, 256 );
		const ProbabilityDistribution* pProbDist = pEntry->getValue();
		char szDist[1024];
		pProbDist->screenPrint( szDist);
		int nIdx = m_wndListCtrl.InsertItem( i, szName.GetBuffer(0) );
		m_wndListCtrl.SetItemText( nIdx, 1, szDist);

		m_wndListCtrl.SetItemData(i,(DWORD)pEntry);
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

void CDlgImportFltPaxData::LoadDataFromFile( const CString& strFileName )
{
	try
	{
		switch( m_emType )
		{
		case FLIGHT_DELAYS:
			m_pDataSet = new FlightData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet(strFileName);
			m_pConDB = ((FlightData*)m_pDataSet)->getDelays();
			break;
		case FLIGHT_LOAD_FACTORS:
			m_pDataSet = new FlightData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((FlightData*)m_pDataSet)->getLoads();
			break;
		case FLIGHT_AC_CAPACITIES:
			m_pDataSet = new FlightData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((FlightData*)m_pDataSet)->getCapacity();
			break;
		case PAX_GROUP_SIZE:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((PassengerData*)m_pDataSet)->getGroups();
			break;
		case PAX_LEAD_LAG:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((PassengerData*)m_pDataSet)->getLeadLagTime();
			break;
		case PAX_IN_STEP:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((PassengerData*)m_pDataSet)->getInStep();
			break;
		case PAX_SIDE_STEP:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((PassengerData*)m_pDataSet)->getSideStep();
			break;
		case PAX_SPEED:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((PassengerData*)m_pDataSet)->getSpeed();
			break;
		case PAX_VISIT_TIME:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((PassengerData*)m_pDataSet)->getVisitTime();
			break;
		case PAX_RESPONSE_TIME:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((PassengerData*)m_pDataSet)->getResponseTime();
			break;
		case PAX_SPEED_IMPACT:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((PassengerData*)m_pDataSet)->getImpactSpeed();
			break;
		case PAX_PUSH_PROPENSITY:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet( strFileName );
			m_pConDB = ((PassengerData*)m_pDataSet)->getPushPropensity();
			break;
		case VISITOR_STA_TRIGGER:
			m_pDataSet = new PassengerData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet(strFileName );
			break;
		case ENTRY_FLIGHT_TIME_DISTRIBUTION:
			m_pDataSet = new BridgeConnectorPaxData();
			m_pDataSet->SetInputTerminal(m_pInterm);
			m_pDataSet->loadDataSet(strFileName);
			m_pConDB = ((BridgeConnectorPaxData*)m_pDataSet)->getEntryFlightTimeDestribution();
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
	}
	else if (m_iOperation == 1)
	{
		((CButton*)GetDlgItem(IDC_RADIO_EXPORTED))->SetCheck(TRUE);
	}
	else if (m_iOperation == 2)
	{
		((CButton*)GetDlgItem(IDC_RADIO_CSVFILE))->SetCheck(TRUE);
	}
}
