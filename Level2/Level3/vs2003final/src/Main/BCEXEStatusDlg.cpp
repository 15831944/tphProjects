// BCEXEStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "BCEXEStatusDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const CString CBCEXEStatusDlg::M_CSSTATUSFILENAME = "exestatus.txt";
const int CBCEXEStatusDlg::M_NHEARTBEATCOUNTMAX = 10;

/////////////////////////////////////////////////////////////////////////////
// CBCEXEStatusDlg dialog


CBCEXEStatusDlg::CBCEXEStatusDlg(const CString _csExeFileName, const CString _csTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CBCEXEStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBCEXEStatusDlg)
	m_csStatus = _T("");
	//}}AFX_DATA_INIT
	m_csTitle = _csTitle;
	m_csExeFileName = _csExeFileName;
	m_bRunning = false;
	m_nHeartBeat = 0;
	m_nHeartBeatCount = 0;
}


void CBCEXEStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBCEXEStatusDlg)
	DDX_Control(pDX, IDC_PROGRESS_BCEXE, m_progressBCEXE);
	DDX_Text(pDX, IDC_STATIC_BCEXE, m_csStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBCEXEStatusDlg, CDialog)
	//{{AFX_MSG_MAP(CBCEXEStatusDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCEXEStatusDlg message handlers

void CBCEXEStatusDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CBCEXEStatusDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// set title
	SetWindowText( LPCTSTR ( m_csTitle ) );

	// start the timer
	SetTimer( 0, 500, NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBCEXEStatusDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == 0 )
	{

		// if m_bRunning is false run the program.
		if( !m_bRunning )
		{
			// run the program
			STARTUPINFO si;

			si.cb = sizeof( STARTUPINFO );
			si.lpReserved = NULL;
			si.lpDesktop = NULL;
			si.lpTitle = NULL;
			si.dwX = 0;
			si.dwY = 0;
			si.dwXSize = 0;
			si.dwYSize = 0;
			si.dwXCountChars = 0;
			si.dwYCountChars = 0;
			si.dwFillAttribute = 0;
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
			si.cbReserved2 = 0;
			si.lpReserved2 = NULL;
			si.hStdInput = 0;
			si.hStdOutput = 0; 
			si.hStdError = 0; 


			PROCESS_INFORMATION pi;
			if( CreateProcess(	LPCTSTR( m_csExeFileName ), 
							NULL,
							NULL,	// LPSECURITY_ATTRIBUTES lpProcessAttributes,  // process security attributes
							NULL,	//LPSECURITY_ATTRIBUTES lpThreadAttributes,   // thread security attributes
							TRUE,	//BOOL bInheritHandles,  // handle inheritance flag
							0,		//DWORD dwCreationFlags, // creation flags
							NULL,	//LPVOID lpEnvironment,  // pointer to new environment block
							NULL,	//LPCTSTR lpCurrentDirectory,   // pointer to current directory name
							&si,	// LPSTARTUPINFO lpStartupInfo,  // pointer to STARTUPINFO
							&pi	// LPPROCESS_INFORMATION lpProcessInformation  // pointer to PROCESS_INFORMATION
							) == 0 )
			{
				DWORD wd = GetLastError();
				m_bOkOnExit = false;
				m_csErrorMsg.Format( "Error on Creating Process, Error Code : %d", wd );
				EndDialog(0);
			}
			else
			{
				// successful						
				m_bRunning = true;
			}
		}
		else
		{
			// The file format is:
			//  HeartBeat,Exit Msg
			//  Status #, Status Msg
			//
			// file exist in current directory 
			// file name is exestatus.txt
			// 
			// update status
			if( !ProcessEXEStatus() )
			{
				EndDialog( 0 );
			}		 
		}

	}
	CDialog::OnTimer(nIDEvent);
}



// return false for exit
bool CBCEXEStatusDlg::ProcessEXEStatus()
{
	m_nHeartBeatCount++;

	// open file
	FILE* statusFile = fopen( M_CSSTATUSFILENAME, "rt" );
	if( statusFile == NULL )
	{
		m_bOkOnExit = false;
		m_csErrorMsg.Format( "Error on Communication with Report Process" );
		return false;
	}

	// Read heartbeat
	char buf[512];
	if( fgets( buf, 512, statusFile) == NULL )
	{
		m_bOkOnExit = false;
		m_csErrorMsg.Format( "Error on Communication with Report Process" );
		fclose( statusFile );
		return false;
	}

	int nHeartBeat = atoi( buf );
	if( nHeartBeat == 0 )
	{
		// exit ok
		m_bOkOnExit = true;
		fclose( statusFile );
		return false;
	}

	if( nHeartBeat == -1 )
	{
		// exit error
		if( fgets( buf, 512, statusFile) )
		{
			m_csErrorMsg.Format( "%s", buf );
		}
		else
		{
			m_csErrorMsg.Format( "Report Process Error on Exit " );
		}
		m_bOkOnExit = false;
		fclose( statusFile );
		return false;
	}

	
	if( nHeartBeat == m_nHeartBeat )
	{
		// the heartbeat stay the same.
		if( m_nHeartBeatCount > M_NHEARTBEATCOUNTMAX )
		{
			// crash
			m_bOkOnExit = false;
			m_csErrorMsg.Format( "Report Process does not Respond" );
			return false;
		}
		else
		{
			// nothing happen
			return true;
		}
	}
	
	m_nHeartBeat = nHeartBeat;


	// get status number ( mandatory )
	if( fgets( buf, 512, statusFile) == NULL )
	{
		m_bOkOnExit = false;
		m_csErrorMsg.Format( "Error on Communication with Report Process" );
		fclose( statusFile );
		return false;
	}

	int nStatusNum = atoi( buf );
	if( nStatusNum < 0 || nStatusNum > 100 )
	{
		m_bOkOnExit = false;
		m_csErrorMsg.Format( "Error on Communication with Report Process" );
		fclose( statusFile );
		return false;
	}

	m_progressBCEXE.SetPos( nStatusNum );

	if( fgets( buf, 512, statusFile) )
	{
		m_csStatus.Format( "%s", buf );
	}

	UpdateData( false );
	fclose( statusFile ); 
	return true;
}
