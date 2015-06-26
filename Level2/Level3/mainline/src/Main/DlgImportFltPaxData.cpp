#include "StdAfx.h"
#include "DlgImportFltPaxData.h"
#include "Common/ProjectManager.h"
#include "..\inputs\fltdata.h"
#include "..\inputs\paxdata.h"
#include "..\Inputs\AircraftEntryProcessorData.h"
#include "..\Inputs\CON_DB.H"
#include "Common\exeption.h"
#include "Common\fileman.h"
#include "Common\ZipInfo.h"
#include "Inputs\PROBAB.H"

IMPLEMENT_DYNAMIC(CDlgImportFltPaxData, CXTResizeDialog)
CDlgImportFltPaxData::CDlgImportFltPaxData(FLTPAXDATATTYPE emType,InputTerminal* pInTerm,ConstraintDatabase* pFltPaxDB,CWnd* pParent)
	: CXTResizeDialog(CDlgImportFltPaxData::IDD, pParent)
	,m_emType(emType)
	,m_pInterm(pInTerm)
	,m_pFltPaxDB(pFltPaxDB)
{
	m_pContraintDB = NULL;
	m_bHit = false;
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
	ON_NOTIFY(NM_CLICK, IDC_LIST_DATA, OnClickListContentItem)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedListcontrol)
END_MESSAGE_MAP()


BOOL CDlgImportFltPaxData::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetImportDialogTitle();
	InitComboxContent();
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(TRUE);
	SetListCtrlContent();
	m_wndExportedProject.EnableFileBrowseButton(".zip","zip Files (*.zip)|*.zip||");
	m_wndCSVFile.EnableFileBrowseButton(".CSV","CSV Files (*.CSV)|*.CSV||");

	ResetButtonStatus(FALSE);

	InitRadioStatus();
	SetResize(IDC_RADIO_LOCAL,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_COMBO_PROJECT,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_RADIO_EXPORTED,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_BROWSE_EXPORTED,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_RADIO_CSVFILE,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_BROWSE_CSVFILE,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_CHECK_ALL,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_CHECK_APPEND,SZ_BOTTOM_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_APPEND,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_REPLACE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
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

	CString strFileName;
	strFileName = GetFileName();
	CString strProjectPath;
	strProjectPath.Format(_T("%s\\INPUT\\%s"),pi.path,strFileName);

	LoadDataFromFile(strProjectPath,false);

	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(TRUE);
	SetListCtrlContent();

	ResetButtonStatus(TRUE);
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
		SetListCtrlContent();
		return;
	}

	CString _strInputZip;
	readIniFileData( strTempPathFile, _strInputZip);
	if(unzipFiles( _strInputZip, strPath) == false)
	{
		SetListCtrlContent();
		return;
	}

	CString strFileName;
	strFileName = GetFileName();
	CString strProjectPath;
	strProjectPath.Format(_T("%s\\%s"),strPath,strFileName);
	LoadDataFromFile(strProjectPath,false);
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(TRUE);
	SetListCtrlContent();

	ResetButtonStatus(TRUE);
}

void CDlgImportFltPaxData::OnUpdateCSVFile()
{
	ClearData();//reset data
	CString strFilePath;
	m_wndCSVFile.GetWindowText(strFilePath);
	if (strFilePath.IsEmpty())
		return;
	
	if(LoadDataFromFile(strFilePath,true) == false)
	{
		CString strMsg;
		strMsg.Format("The Importing File is not complying with required file format.  file:\r\n%s", strFilePath );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONERROR );
	}
	((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(TRUE);
	SetListCtrlContent();
	ResetButtonStatus(TRUE);
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
		s.LoadString(IDS_TVNN_ACENTRYPROCS);
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
	char columnLabel[3][128];
	if (m_pContraintDB == NULL)
	{
		strcpy( columnLabel[1], "Distribution ( UNIT )" );
	}
	else
	{
		sprintf( columnLabel[1], "Distribution ( %s )", m_pContraintDB->getUnits() );
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

	strcpy( columnLabel[2], "Importing Result" );	

	int DefaultColumnWidth[] = { 180, 300,255 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER,LVCFMT_CENTER };
	for( int i=0; i<3; i++ )
	{
		CStringList strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.fmt = nFormat[i];
		m_wndListCtrl.InsertColumn( i, &lvc );
	}
}

void CDlgImportFltPaxData::SetListCtrlContent()
{
	InitListCtrlHeader();
	m_wndListCtrl.DeleteAllItems();

	if( m_pContraintDB == NULL )
		return;
	int nCount = m_pContraintDB->getCount();
	BOOL bCheckAll = ((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck();
	bool bFailed = false;
	for( int i=0; i<nCount; i++ )
	{
		ConstraintEntry* pEntry = m_pContraintDB->getItem( i );
		const Constraint* pCon = pEntry->getConstraint();
		CString szName;
		pCon->screenPrint( szName, 0, 256 );
		const ProbabilityDistribution* pProbDist = pEntry->getValue();
		char szDist[1024];
		pProbDist->screenPrint( szDist);
		int nIdx = m_wndListCtrl.InsertItem( i, szName.GetBuffer(0) );
		m_wndListCtrl.SetItemText( nIdx, 1, szDist);
		m_wndListCtrl.SetCheck(i,bCheckAll);
		mapConstraintData::iterator iter = m_mapConstraintData.find(pEntry);
		if(iter != m_mapConstraintData.end())
		{
			m_wndListCtrl.SetItemText(nIdx,2,iter->second);
			if (_strcmpi(iter->second,"Successfully"))
			{
				m_wndListCtrl.SetCheck(i,FALSE);
				bFailed = true;
			}
		}
		m_wndListCtrl.SetItemData(i,(DWORD)pEntry);
	}	

	if (bFailed)
	{
		 ((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(FALSE);
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
bool CDlgImportFltPaxData::CheckFileFormat( ArctermFile& p_file )
{
	while (p_file.getLine())
	{
		if (sFltPaxString[m_emType])
		{
			char str[64];
			p_file.getField (str, SMALL);
			if (_stricmp (str, sFltPaxString[m_emType]) == 0)
				return true;
		}
	}
	return false;
}

bool CDlgImportFltPaxData::LoadDataFromFile( const CString& strFileName,bool bCheckFile )
{
	ClearData();
	ArctermFile p_file;
	if(p_file.openFile(strFileName,READ) )
	{
		if (bCheckFile)//check load from csv
		{
			if(CheckFileFormat(p_file) == false)
			{
				return false;
			}
		}
		switch (m_emType)
		{
		case FLIGHT_DELAYS:
			m_pContraintDB = new FlightConstraintDatabase();
			m_pContraintDB->setUnits("MINUTES");
			ReadFlightConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case FLIGHT_LOAD_FACTORS:
			m_pContraintDB = new FlightConstraintDatabaseWithSchedData();
			m_pContraintDB->setUnits("PERCENT");
			ReadFlightConstrainWithSchedDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case FLIGHT_AC_CAPACITIES:
			m_pContraintDB = new FlightConstraintDatabaseWithSchedData();
			m_pContraintDB->setUnits("COUNT");
			ReadFlightConstrainWithSchedDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_GROUP_SIZE:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("COUNT");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_LEAD_LAG:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("MIN");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_IN_STEP:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("METERS");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_SIDE_STEP:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("METERS");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_SPEED:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("METERS/SEC");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_VISIT_TIME:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("MIN");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_RESPONSE_TIME:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("SECONDS");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_SPEED_IMPACT:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("METERS");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case PAX_PUSH_PROPENSITY:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("PERCENT");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case VISITOR_STA_TRIGGER:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("MIN");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		case ENTRY_FLIGHT_TIME_DISTRIBUTION:
			m_pContraintDB = new CMobileElemConstraintDatabase();
			m_pContraintDB->setUnits("SECONDS");
			ReadMobileElemConstrainDatabase(p_file,sFltPaxString[m_emType],m_pInterm);
			break;
		}
	}

	return true;
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
		m_wndExportedProject.OnBrowse();
	}
	else if (m_iOperation == 2)
	{
		((CButton*)GetDlgItem(IDC_RADIO_CSVFILE))->SetCheck(TRUE);
		m_wndCombox.EnableWindow(FALSE);
		m_wndExportedProject.EnableWindow(FALSE);
		m_wndCSVFile.OnBrowse();
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
	m_mapConstraintData.clear();
	if (m_pContraintDB)
	{
		delete m_pContraintDB;
		m_pContraintDB = NULL;
	}
}

void CDlgImportFltPaxData::OnCheckAll()
{
	BOOL bCheckAll = ((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck();
	
	for (int i = 0; i < m_wndListCtrl.GetItemCount(); i++)
	{
		m_wndListCtrl.SetCheck(i,bCheckAll);
	}
}

void CDlgImportFltPaxData::OnCheckLocalProject()
{
	GetDlgItem(IDC_COMBO_PROJECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BROWSE_EXPORTED)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE_CSVFILE)->EnableWindow(FALSE);

	m_wndCombox.ShowDropDown();
	CString strContent;
	m_wndCombox.GetWindowText(strContent);
	if (strContent.IsEmpty() == false)
	{
		PROJECTINFO pi;	
		PROJMANAGER->GetProjectInfo(strContent, &pi, "");//get the project information

		CString strFileName;
		strFileName = GetFileName();
		CString strProjectPath;
		strProjectPath.Format(_T("%s\\INPUT\\%s"),pi.path,strFileName);

		LoadDataFromFile(strProjectPath,false);

		SetListCtrlContent();

		ResetButtonStatus(TRUE);
	}
}

void CDlgImportFltPaxData::OnCheckExportedProject()
{
	GetDlgItem(IDC_COMBO_PROJECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE_EXPORTED)->EnableWindow(TRUE);
	GetDlgItem(IDC_BROWSE_CSVFILE)->EnableWindow(FALSE);

	m_wndExportedProject.OnBrowse();
}

void CDlgImportFltPaxData::OnCheckCSVFile()
{
	GetDlgItem(IDC_COMBO_PROJECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE_EXPORTED)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE_CSVFILE)->EnableWindow(TRUE);

	m_wndCSVFile.OnBrowse();
}

void CDlgImportFltPaxData::ResetButtonStatus( BOOL bTrue )
{
	GetDlgItem(IDC_BUTTON_APPEND)->EnableWindow(bTrue);
	GetDlgItem(IDC_BUTTON_REPLACE)->EnableWindow(bTrue);
}

void CDlgImportFltPaxData::OnClickListContentItem( NMHDR* pNMHDR, LRESULT* pResult )
{
	CPoint Pt = GetMessagePos();
	m_wndListCtrl.ScreenToClient(&Pt);
	UINT uFlags;
	int nHitItem = m_wndListCtrl.HitTest(Pt, &uFlags);
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck();
	if (uFlags == LVHT_ONITEMSTATEICON)
	{
		m_bHit = true;
	}
	else
	{
		m_bHit = false;
	}
	*pResult = 0;
}

void CDlgImportFltPaxData::OnLvnItemchangedListcontrol( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (m_bHit)
	{
		for (int i = 0; i < m_wndListCtrl.GetItemCount(); i++)
		{
			if (m_wndListCtrl.GetCheck(i) == FALSE)
			{
				((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(FALSE);
				*pResult = 0;
				return;
			}
		}
		((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(TRUE);
	}
	*pResult = 0;
}

void CDlgImportFltPaxData::ReadFlightConstrainDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	p_file.reset();
	p_file.skipLines (3);

	SortedContainer<ConstraintEntry> sortedList;
	sortedList.ownsElements (0);

	FlightConstraint *aConst;
	ProbabilityDistribution *aProbDist;
	ConstraintEntry *anEntry;
	char str[64];
	int probField = 1 + (p_keyword != NULL) + (m_pContraintDB->getUnits() != NULL);

	while (p_file.getLine())
	{
		if (p_keyword)
		{
			p_file.getField (str, SMALL);
			if (_stricmp (str, p_keyword))
				continue;
		}
		char strConstraint[256] = {0};
		p_file.getSubField (strConstraint, ';');
		p_file.setToField(1);

		aConst = new FlightConstraint;
		aConst->SetAirportDB( _pInTerm->m_pAirportDB );
		aConst->readConstraint (p_file);

		CString strResult;
		char strCompare[2560] = {0}; 
		aConst->WriteSyntaxStringWithVersion (strCompare);
		if (_strcmpi(strConstraint,strCompare) == 0)
		{
			strResult = _T("Successfully");
		}
		else 
		{
			strResult.Format(_T("Cannot parse flight type: %s"),strConstraint);
		}

		if (m_pContraintDB->getUnits())
			p_file.getField (str, SMALL);

		p_file.setToField (probField);
		aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);

		anEntry = new ConstraintEntry;
		anEntry->initialize (aConst, aProbDist);
		m_mapConstraintData[anEntry] = strResult;
	}

	mapConstraintData::iterator iter = m_mapConstraintData.begin();
	for (; iter != m_mapConstraintData.end(); ++iter)
	{
		m_pContraintDB->addEntry(iter->first);
	}
}

void CDlgImportFltPaxData::ReadFlightConstrainWithSchedDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm)
{
	p_file.reset();
	p_file.skipLines (3);

	FlightConstraint *aConst;
	ProbabilityDistribution *aProbDist;
	ConstraintEntry *anEntry;
	char str[64];
	int probField = 1 + (p_keyword != NULL) + (m_pContraintDB->getUnits() != NULL);

	while (p_file.getLine())
	{
		if (p_keyword)
		{
			p_file.getField (str, SMALL);
			if (_stricmp (str, p_keyword))
				continue;
		}

		char strConstraint[256] = {0};
		p_file.getSubField (strConstraint, ';');
		p_file.setToField(1);

		aConst = new FlightConstraint;
		aConst->SetAirportDB( _pInTerm->m_pAirportDB );
		aConst->readConstraint (p_file);

		CString strResult;
		char strCompare[2560] = {0}; 
		aConst->WriteSyntaxStringWithVersion (strCompare);
		if (_strcmpi(strConstraint,strCompare) == 0)
		{
			strResult = _T("Successfully");
		}
		else 
		{
			strResult.Format(_T("Cannot parse flight type: %s"),strConstraint);
		}

		if (m_pContraintDB->getUnits())
			p_file.getField (str, SMALL);

		p_file.setToField (probField);
		aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);

		int nValue;
		p_file.getInteger(nValue);
		((FlightConstraintDatabaseWithSchedData*)m_pContraintDB)->SetNeglectSchedData(nValue >0?true:false);
		anEntry = new ConstraintEntry;
		anEntry->initialize (aConst, aProbDist);
		m_mapConstraintData[anEntry] = strResult;
	}

	mapConstraintData::iterator iter = m_mapConstraintData.begin();
	for (; iter != m_mapConstraintData.end(); ++iter)
	{
		m_pContraintDB->addEntry(iter->first);
	}
}

void CDlgImportFltPaxData::ReadMobileElemConstrainDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm)
{
	p_file.reset();
	p_file.skipLines (3);

	CMobileElemConstraint* pConst;
	ProbabilityDistribution* pProbDist;
	ConstraintEntry* pEntry;
	char str[64];
	int probField = 1 + (p_keyword != NULL) + (m_pContraintDB->getUnits() != NULL);

	while (p_file.getLine())
	{
		if (p_keyword)
		{
			p_file.getField (str, SMALL);
			if (_stricmp (str, p_keyword))
				continue;
		}

		char strConstraint[256] = {0};
		p_file.getSubField (strConstraint, ';');
		p_file.setToField(1);
		pConst = new CMobileElemConstraint(_pInTerm);
		pConst->readConstraint (p_file);

		CString strResult;
		char strCompare[2560] = {0}; 
		pConst->WriteSyntaxStringWithVersion (strCompare);
		if (_strcmpi(strConstraint,strCompare) == 0)
		{
			strResult = _T("Successfully");
		}
		else 
		{
			strResult.Format(_T("Cannot parse mobile element type: %s"),strConstraint);
		}

		if (m_pContraintDB->getUnits())
			p_file.getField (str, SMALL);

		p_file.setToField (probField);
		pProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);

		pEntry = new ConstraintEntry;
		pEntry->initialize( pConst, pProbDist );
		m_mapConstraintData[pEntry] = strResult;
	}

	mapConstraintData::iterator iter = m_mapConstraintData.begin();
	for (; iter != m_mapConstraintData.end(); ++iter)
	{
		m_pContraintDB->addEntry(iter->first);
	}
}

CString CDlgImportFltPaxData::GetFileName()
{
	CString strFileName;
	switch (m_emType)
	{
	case FLIGHT_DELAYS:
	case FLIGHT_LOAD_FACTORS:
	case FLIGHT_AC_CAPACITIES:
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
		strFileName = "PAXDATA.MSC";
		break;
	case  ENTRY_FLIGHT_TIME_DISTRIBUTION:
		strFileName = "BCPAXDATA.TXT";
		break;
	}
	return strFileName;
}

void CBrowserARCEdit::OnBrowse()
{
	ASSERT_VALID(this);
	ENSURE(GetSafeHwnd() != NULL);

	if (m_Mode == BrowseMode_File)
	{
		CString strFile;
		GetWindowText(strFile);

		if (!strFile.IsEmpty())
		{
			TCHAR fname [_MAX_FNAME];

			_tsplitpath_s(strFile, NULL, 0, NULL, 0, fname, _MAX_FNAME, NULL, 0);

			CString strFileName = fname;
			strFileName.TrimLeft();
			strFileName.TrimRight();

			if (strFileName.IsEmpty())
			{
				strFile.Empty();
			}

			const CString strInvalidChars = _T("*?<>|");
			if (strFile.FindOneOf(strInvalidChars) >= 0)
			{
				if (!OnIllegalFileName(strFile))
				{
					SetFocus();
					return;
				}
			}
		}

		CFileDialog dlg(TRUE, !m_strDefFileExt.IsEmpty() ? (LPCTSTR)m_strDefFileExt : (LPCTSTR)NULL, strFile, 0, !m_strFileFilter.IsEmpty() ? (LPCTSTR)m_strFileFilter : (LPCTSTR)NULL, NULL);
		if (dlg.DoModal() == IDOK && dlg.GetPathName().IsEmpty() == FALSE)
		{
			SetWindowText(dlg.GetPathName());
			SetModify(TRUE);
			OnAfterUpdate();
		}

		if (GetParent() != NULL)
		{
			GetParent()->RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
		}

	}

	SetFocus();
}
